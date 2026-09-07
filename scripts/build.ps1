param(
    [switch]$Rebuild
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$C51Bin = "D:\C51\BIN"
$Build = Join-Path $Root "build"
$Rel = Join-Path $Root "releases"
New-Item -ItemType Directory -Force -Path $Build, $Rel | Out-Null

$env:PATH = "$C51Bin;" + $env:PATH
$env:C51INC = "D:\C51\INC"
$env:C51LIB = "D:\C51\LIB"
Set-Location $Root

if ($Rebuild) {
    Get-ChildItem $Build -File -ErrorAction SilentlyContinue | ForEach-Object {
        Remove-Item -LiteralPath $_.FullName -Force -ErrorAction SilentlyContinue
    }
}

function Assert-LastExit([string]$Name) {
    if ($LASTEXITCODE -ne 0) {
        throw "$Name failed with exit $LASTEXITCODE"
    }
}

Write-Host "+ A51 STARTUP.A51"
& "$C51Bin\A51.exe" "STARTUP.A51" "DEBUG" "PRINT(build\STARTUP.lst)" "OBJECT(build\STARTUP.obj)"
Assert-LastExit "A51"

$inc = ".\firmware\include;.\firmware\bsp;.\firmware\app;.\firmware\third_party\sinone;.\firmware\third_party\sense_lib"
$c51model = "LARGE"

function Invoke-C51([string]$Src, [string]$Name) {
    Write-Host "+ C51 $Src"
    & "$C51Bin\C51.exe" $Src $c51model "OMF2" "INTVECTOR(0)" "DEBUG" "OBJECTEXTEND" "INCDIR($inc)" "OPTIMIZE(8,SIZE)" "PRINT(build\$Name.lst)" "OBJECT(build\$Name.obj)"
    Assert-LastExit "C51 $Src"
}

Invoke-C51 "firmware\app\main.c" "main"
Invoke-C51 "firmware\bsp\log_uart.c" "log_uart"
Invoke-C51 "firmware\bsp\timebase.c" "timebase"
Invoke-C51 "firmware\bsp\tm1640.c" "tm1640"
Invoke-C51 "firmware\bsp\beep.c" "beep"
Invoke-C51 "firmware\app\disp_test.c" "disp_test"
Invoke-C51 "firmware\app\keys.c" "keys"
Invoke-C51 "firmware\third_party\sense_lib\TKDriver.C" "TKDriver"

Copy-Item -LiteralPath "firmware\third_party\sense_lib\SC95F8X6X_HighSensitive.LIB" -Destination (Join-Path $Build "touch.lib") -Force

$lnp = Join-Path $Build "link.lnp"
@"
build\STARTUP.obj,
build\main.obj,
build\log_uart.obj,
build\timebase.obj,
build\tm1640.obj,
build\beep.obj,
build\disp_test.obj,
build\keys.obj,
build\TKDriver.obj,
build\touch.lib
TO build\Stage1B
CLASSES (CODE (C:0x0080-C:0xFFFF))
"@ | Set-Content -LiteralPath $lnp -Encoding ascii

Write-Host "+ LX51"
& "$C51Bin\LX51.exe" "@build\link.lnp"
if ($LASTEXITCODE -gt 1) {
    throw "LX51 failed with exit $LASTEXITCODE"
}

Write-Host "+ OHX51"
& "$C51Bin\Ohx51.exe" "build\Stage1B" "HEXFILE(build\Stage1B.hex)"
Assert-LastExit "OHX51"

$hex = Join-Path $Build "Stage1B.hex"
$h = Get-FileHash -LiteralPath $hex -Algorithm SHA256
Copy-Item -LiteralPath $hex -Destination (Join-Path $Rel "Stage1B.hex") -Force
Write-Host "HEX $hex"
Write-Host "SHA256 $($h.Hash)"
Write-Host "SIZE $((Get-Item -LiteralPath $hex).Length)"
Get-ChildItem $Build | Format-Table Name, Length
Write-Host "BUILD OK"

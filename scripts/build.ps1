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
Invoke-C51 "firmware\app\disp_ui.c" "disp_ui"
Invoke-C51 "firmware\app\hmi.c" "hmi"
Invoke-C51 "firmware\app\keys.c" "keys"
Invoke-C51 "firmware\bsp\pwr_uart.c" "pwr_uart"
Invoke-C51 "firmware\app\pwr_link.c" "pwr_link"
Invoke-C51 "firmware\bsp\ir_rx.c" "ir_rx"
Invoke-C51 "firmware\app\ir_link.c" "ir_link"
Invoke-C51 "firmware\bsp\ntc.c" "ntc"
Invoke-C51 "firmware\bsp\wifi_uart.c" "wifi_uart"
Invoke-C51 "firmware\app\tuya_link.c" "tuya_link"
Invoke-C51 "firmware\bsp\eeprom.c" "eeprom"
Invoke-C51 "firmware\app\nvm.c" "nvm"
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
build\disp_ui.obj,
build\hmi.obj,
build\keys.obj,
build\pwr_uart.obj,
build\pwr_link.obj,
build\ir_rx.obj,
build\ir_link.obj,
build\ntc.obj,
build\wifi_uart.obj,
build\tuya_link.obj,
build\eeprom.obj,
build\nvm.obj,
build\TKDriver.obj,
build\touch.lib
TO build\Stage4C
PRINT (build\Stage4C.MAP)
CLASSES (CODE (C:0x0080-C:0xFFFF))
"@ | Set-Content -LiteralPath $lnp -Encoding ascii

Write-Host "+ LX51"
& "$C51Bin\LX51.exe" "@build\link.lnp"
if ($LASTEXITCODE -gt 1) {
    throw "LX51 failed with exit $LASTEXITCODE"
}

Write-Host "+ OHX51"
& "$C51Bin\Ohx51.exe" "build\Stage4C" "HEXFILE(build\Stage4C.hex)"
Assert-LastExit "OHX51"

$hex = Join-Path $Build "Stage4C.hex"
$h = Get-FileHash -LiteralPath $hex -Algorithm SHA256
Copy-Item -LiteralPath $hex -Destination (Join-Path $Rel "Stage4C.hex") -Force
Write-Host "HEX $hex"
Write-Host "SHA256 $($h.Hash)"
Write-Host "SIZE $((Get-Item -LiteralPath $hex).Length)"
Get-ChildItem $Build | Format-Table Name, Length
Write-Host "BUILD OK"

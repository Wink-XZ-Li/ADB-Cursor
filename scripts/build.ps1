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

$inc = ".\firmware\include;.\firmware\bsp;.\firmware\third_party\sinone"

Write-Host "+ C51 main.c"
& "$C51Bin\C51.exe" "firmware\app\main.c" "SMALL" "DEBUG" "OBJECTEXTEND" "INCDIR($inc)" "OPTIMIZE(8,SIZE)" "PRINT(build\main.lst)" "OBJECT(build\main.obj)"
Assert-LastExit "C51 main.c"

Write-Host "+ C51 log_uart.c"
& "$C51Bin\C51.exe" "firmware\bsp\log_uart.c" "SMALL" "DEBUG" "OBJECTEXTEND" "INCDIR($inc)" "OPTIMIZE(8,SIZE)" "PRINT(build\log_uart.lst)" "OBJECT(build\log_uart.obj)"
Assert-LastExit "C51 log_uart.c"

Write-Host "+ C51 timebase.c"
& "$C51Bin\C51.exe" "firmware\bsp\timebase.c" "SMALL" "DEBUG" "OBJECTEXTEND" "INCDIR($inc)" "OPTIMIZE(8,SIZE)" "PRINT(build\timebase.lst)" "OBJECT(build\timebase.obj)"
Assert-LastExit "C51 timebase.c"

Write-Host "+ BL51"
& "$C51Bin\BL51.exe" "build\STARTUP.obj,build\main.obj,build\log_uart.obj,build\timebase.obj" "TO" "build\Stage0" "RAMSIZE(256)"
if ($LASTEXITCODE -gt 1) {
    throw "BL51 failed with exit $LASTEXITCODE"
}

Write-Host "+ OH51"
& "$C51Bin\OH51.exe" "build\Stage0" "HEXFILE(build\Stage0.hex)"
Assert-LastExit "OH51"

$hex = Join-Path $Build "Stage0.hex"
$h = Get-FileHash -LiteralPath $hex -Algorithm SHA256
Copy-Item -LiteralPath $hex -Destination (Join-Path $Rel "Stage0.hex") -Force
Write-Host "HEX $hex"
Write-Host "SHA256 $($h.Hash)"
Write-Host "SIZE $((Get-Item -LiteralPath $hex).Length)"
Get-ChildItem $Build | Format-Table Name, Length
Write-Host "BUILD OK"

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

$appInc = ".\firmware\include;.\firmware\bsp;.\firmware\app;.\firmware\third_party\sinone;.\firmware\third_party\sense_lib"
$bootInc = ".\firmware\include;.\firmware\third_party\sinone;.\boot"

Write-Host "+ A51 boot\vectors.a51"
& "$C51Bin\A51.exe" "boot\vectors.a51" "DEBUG" "PRINT(build\boot_vectors.lst)" "OBJECT(build\boot_vectors.obj)"
Assert-LastExit "A51 boot vectors"

function Invoke-BootC51([string]$Src, [string]$Name) {
    Write-Host "+ C51 SMALL $Src"
    & "$C51Bin\C51.exe" $Src "SMALL" "OMF2" "NOINTVECTOR" "DEBUG" "OBJECTEXTEND" "INCDIR($bootInc)" "OPTIMIZE(8,SIZE)" "PRINT(build\$Name.lst)" "OBJECT(build\$Name.obj)"
    Assert-LastExit "C51 $Src"
}

Invoke-BootC51 "boot\main.c" "boot_main"
Invoke-BootC51 "boot\boot_iap.c" "boot_iap"

$bootLnp = Join-Path $Build "boot.lnp"
@"
build\boot_vectors.obj,
build\boot_main.obj,
build\boot_iap.obj
TO build\Boot
PRINT (build\Boot.MAP)
CLASSES (CODE (C:0x0060-C:0x0FFF))
"@ | Set-Content -LiteralPath $bootLnp -Encoding ascii

Write-Host "+ LX51 Boot"
& "$C51Bin\LX51.exe" "@build\boot.lnp"
if ($LASTEXITCODE -gt 1) {
    throw "LX51 Boot failed with exit $LASTEXITCODE"
}

Write-Host "+ OHX51 Boot"
& "$C51Bin\Ohx51.exe" "build\Boot" "HEXFILE(build\Boot.hex)"
Assert-LastExit "OHX51 Boot"

Write-Host "+ A51 STARTUP.A51"
& "$C51Bin\A51.exe" "STARTUP.A51" "DEBUG" "PRINT(build\STARTUP.lst)" "OBJECT(build\STARTUP.obj)"
Assert-LastExit "A51"

$c51model = "LARGE"

function Invoke-C51([string]$Src, [string]$Name) {
    Write-Host "+ C51 $Src"
    & "$C51Bin\C51.exe" $Src $c51model "OMF2" "INTVECTOR(0x1000)" "DEBUG" "OBJECTEXTEND" "INCDIR($appInc)" "OPTIMIZE(8,SIZE)" "PRINT(build\$Name.lst)" "OBJECT(build\$Name.obj)"
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
Invoke-C51 "firmware\bsp\iap.c" "iap"
Invoke-C51 "firmware\app\ota_dl.c" "ota_dl"
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
build\iap.obj,
build\ota_dl.obj,
build\TKDriver.obj,
build\touch.lib
TO build\Stage4B
PRINT (build\Stage4B.MAP)
CLASSES (CODE (C:0x1080-C:0x87EF), CONST (C:0x1080-C:0x87EF))
"@ | Set-Content -LiteralPath $lnp -Encoding ascii

Write-Host "+ LX51 App"
& "$C51Bin\LX51.exe" "@build\link.lnp"
if ($LASTEXITCODE -gt 1) {
    throw "LX51 App failed with exit $LASTEXITCODE"
}

Write-Host "+ OHX51 App"
& "$C51Bin\Ohx51.exe" "build\Stage4B" "HEXFILE(build\Stage4B.hex)"
Assert-LastExit "OHX51 App"

Write-Host "+ merge factory.hex"
python "scripts\merge_factory_hex.py" "build\Boot.hex" "build\Stage4B.hex" "-o" "releases\factory.hex"
Assert-LastExit "merge_factory_hex.py"

Write-Host "+ OADB bin"
python "scripts\make_ota_bin.py" "build\Stage4B.hex" "-o" "releases\Stage4B_ota.bin"
Assert-LastExit "make_ota_bin.py"

$hex = Join-Path $Build "Stage4B.hex"
$h = Get-FileHash -LiteralPath $hex -Algorithm SHA256
Copy-Item -LiteralPath $hex -Destination (Join-Path $Rel "Stage4B.hex") -Force
Write-Host "HEX $hex"
Write-Host "SHA256 $($h.Hash)"
Write-Host "SIZE $((Get-Item -LiteralPath $hex).Length)"
Get-ChildItem $Build | Format-Table Name, Length
Write-Host "BUILD OK"

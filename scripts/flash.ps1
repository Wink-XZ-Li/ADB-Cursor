param(
    [string]$Uv4 = "D:\UV4\UV4.exe",
    [string]$Project = "",
    [string]$Target = "Target 1",
    [switch]$ConfirmPowerDisconnected
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
if (-not $Project) {
    $Project = Join-Path $Root "ADB-Cursor.uvproj"
}
$LogDir = Join-Path $Root "build"
New-Item -ItemType Directory -Force -Path $LogDir | Out-Null
$Log = Join-Path $env:TEMP "adb_cursor_uv4_flash.log"

if (-not $ConfirmPowerDisconnected) {
    Write-Host "Refuse to flash: pass -ConfirmPowerDisconnected after the power-board supply is disconnected."
    exit 2
}

Write-Host "UV4 -f $Project -t $Target"
$p = Start-Process -FilePath $Uv4 -ArgumentList @("-f", $Project, "-t", $Target, "-o", $Log) -Wait -PassThru
Write-Host "UV4 flash exit $($p.ExitCode)"
if (Test-Path -LiteralPath $Log) {
    Get-Content -LiteralPath $Log -ErrorAction SilentlyContinue
}
exit $p.ExitCode

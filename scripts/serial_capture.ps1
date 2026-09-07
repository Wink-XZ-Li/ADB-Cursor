param(
    [string]$Port = "COM7",
    [int]$Baud = 115200,
    [int]$Duration = 8,
    [string]$Save = ""
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
if (-not $Save) {
    $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $Save = Join-Path $Root "records\stage-minus1-$stamp.log"
}
$Script = "C:\Users\powtek\.agents\skills\serial-monitor\scripts\serial_monitor.py"
if (-not (Test-Path -LiteralPath $Script)) {
    throw "serial_monitor.py not found: $Script"
}

python $Script --port $Port --baud $Baud --duration $Duration --timestamp --save $Save -v
Write-Host "saved $Save"

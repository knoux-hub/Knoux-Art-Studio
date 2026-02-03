# Common helpers for Knoux deployment scripts (cross-platform aware).
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-KnouxRoot {
    if ($env:KNOUX_ROOT) {
        return $env:KNOUX_ROOT
    }

    if ($IsWindows) {
        return "F:\\KnouxArtStudio"
    }

    return (Join-Path $HOME "KnouxArtStudio")
}

function Write-KnouxLog {
    param(
        [string]$Message,
        [ValidateSet("INFO", "SUCCESS", "WARN", "ERROR")] [string]$Level = "INFO"
    )

    $color = switch ($Level) {
        "SUCCESS" { "Green" }
        "WARN" { "Yellow" }
        "ERROR" { "Red" }
        default { "Cyan" }
    }

    Write-Host "[$Level] $Message" -ForegroundColor $color
}

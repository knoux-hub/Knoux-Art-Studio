
# Knoux Art Studio - Project Creation Script
# Architecture & Execution Order Supreme

. "$PSScriptRoot/knoux_helpers.ps1"
$RootPath = Get-KnouxRoot
Write-KnouxLog "Starting $($MyInvocation.MyCommand.Name)." "INFO"

try {
$Structure = @(
    "",
    "components",
    "views",
    "services",
    "assets",
    "assets/icons",
    "assets/models"
)

Write-Host "🏛️ Initializing Knoux Art Studio Sovereignty Order..." -ForegroundColor Cyan

foreach ($Dir in $Structure) {
    $FullPath = Join-Path $RootPath $Dir
    if (-not (Test-Path $FullPath)) {
        New-Item -Path $FullPath -ItemType Directory | Out-Null
        Write-Host "CREATED: $FullPath" -ForegroundColor Green
    }
}

Write-Host "`n📁 Project structure verified on $RootPath" -ForegroundColor Cyan
Write-Host "✅ Knoux Art Studio is ready for deployment." -ForegroundColor White
Write-KnouxLog "Completed $($MyInvocation.MyCommand.Name)." "SUCCESS"
} catch {
    Write-KnouxLog "Failed $($MyInvocation.MyCommand.Name): $($_.Exception.Message)" "ERROR"
    throw
}


# Knoux Art Studio - Project Creation Script
# Architecture & Execution Order Supreme

$RootPath = "F:\KnouxArtStudio"
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

Write-Host "`n📁 Project structure verified on F:\KnouxArtStudio" -ForegroundColor Cyan
Write-Host "✅ Knoux Art Studio is ready for deployment." -ForegroundColor White

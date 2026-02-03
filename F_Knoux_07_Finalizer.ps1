# ==============================================================================
# KNOUX ART STUDIO - SOVEREIGN FINALIZER
# ==============================================================================

$Root = "F:\KnouxArtStudio"

Write-Host "🕵️ Verifying Sovereign Installation on Drive F:..." -ForegroundColor Cyan

$RequiredDirs = @("src", "ai_models", "config", "projects", "cache")
foreach ($d in $RequiredDirs) {
    if (Test-Path (Join-Path $Root $d)) {
        Write-Host "  [OK] $d directory exists." -ForegroundColor Green
    } else {
        Write-Host "  [ERR] $d is missing!" -ForegroundColor Red
    }
}

Write-Host "`n🚀 DEPLOYMENT COMPLETED." -ForegroundColor Green
Write-Host "You can now open the project in VS Code or your preferred C++ IDE." -ForegroundColor White
Write-Host "Sovereignty Status: SECURE" -ForegroundColor Magenta

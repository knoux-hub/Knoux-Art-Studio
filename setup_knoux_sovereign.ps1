
# ==============================================================================
# KNOUX ART STUDIO - SUPREME ARCHITECTURE & EXECUTION ORDER
# Deployment Script v1.0.0
# "Safety is not a luxury, it's a life"
# ==============================================================================

. "$PSScriptRoot/knoux_helpers.ps1"
$RootPath = Get-KnouxRoot
Write-KnouxLog "Starting $($MyInvocation.MyCommand.Name)." "INFO"
try {

# 1. Verification Phase
Write-Host "🏛️ Initializing Knoux Art Studio Sovereignty Order..." -ForegroundColor Cyan

if (-not (Test-Path $Drive)) {
    Write-Host "❌ DRIVE F: NOT FOUND. PLEASE MOUNT DRIVE OR ADJUST SCRIPT." -ForegroundColor Red
    exit
}

# 2. Structure Creation Phase
$Structure = @(
    "src",
    "src/components",
    "src/views",
    "src/services",
    "src/assets",
    "src/assets/icons",
    "src/assets/models",
    "src/assets/tutorials",
    "src/types",
    "src/styles",
    "projects",
    "cache",
    "logs",
    "dist"
)

Write-Host "`n📁 Building Sovereign Directory Structure..." -ForegroundColor White

foreach ($Dir in $Structure) {
    $FullPath = Join-Path $RootPath $Dir
    if (-not (Test-Path $FullPath)) {
        New-Item -Path $FullPath -ItemType Directory -Force | Out-Null
        Write-Host "CREATED: $FullPath" -ForegroundColor Green
    } else {
        Write-Host "EXISTS: $FullPath" -ForegroundColor Gray
    }
}

# 3. Environment Protection
$SovereignMarker = Join-Path $RootPath ".sovereign"
if (-not (Test-Path $SovereignMarker)) {
    New-Item -Path $SovereignMarker -ItemType File | Out-Null
    Set-ItemProperty -Path $SovereignMarker -Name Attributes -Value Hidden
}

# 4. Final Verification
Write-Host "`n✅ Structure Verification Complete." -ForegroundColor Green
Write-Host "📍 Project Root: $RootPath" -ForegroundColor Cyan
Write-Host "🔒 Sovereign Marker: Active (Hidden)" -ForegroundColor Gray
Write-Host "`n[ Knoux Art Studio is ready for core module population. ]" -ForegroundColor Yellow
Write-Host "==============================================================================" -ForegroundColor Gray
Write-KnouxLog "Completed $($MyInvocation.MyCommand.Name)." "SUCCESS"
} catch {
    Write-KnouxLog "Failed $($MyInvocation.MyCommand.Name): $($_.Exception.Message)" "ERROR"
    throw
}

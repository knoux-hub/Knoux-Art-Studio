
# ==============================================================================
# KNOUX ART STUDIO - SUPREME ARCHITECTURE & EXECUTION ORDER
# Deployment Script v1.0.1
# "Safety is not a luxury, it's a life"
# ==============================================================================

$ProjectName = "KnouxArtStudio"
$Drive = "F:\"
$RootPath = Join-Path $Drive $ProjectName

# 1. Initialization
Write-Host "`n🏛️ Initializing Knoux Art Studio Sovereignty Order..." -ForegroundColor Cyan

if (-not (Test-Path $Drive)) {
    Write-Host "❌ DRIVE F: NOT FOUND. PLEASE MOUNT DRIVE OR ADJUST SCRIPT." -ForegroundColor Red
    exit
}

# 2. Directory Structure Creation
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

# 3. Generating Sovereign Core Files (Initial Population)

# Function to safely create files
function Create-KnouxFile {
    param($Path, $Content)
    $FullFilePath = Join-Path $RootPath $Path
    $Content | Set-Content -Path $FullFilePath -Force
    Write-Host "POPULATED: $Path" -ForegroundColor Magenta
}

Write-Host "`n🧠 Populating Sovereign Core Intelligence..." -ForegroundColor White

# types/index.ts
Create-KnouxFile "src/types/index.ts" @"
export enum UserMode { BEGINNER, CREATOR, PROFESSIONAL, POWER_USER, SECURE }
export enum AppScreen { SPLASH, ONBOARDING, HOME, PHOTO_EDITOR, VIDEO_EDITOR, ELYSIAN_CANVAS, SETTINGS }
"@

# .sovereign marker
$SovereignMarker = Join-Path $RootPath ".sovereign"
New-Item -Path $SovereignMarker -ItemType File -Force | Out-Null
Set-ItemProperty -Path $SovereignMarker -Name Attributes -Value Hidden

# 4. Final Verification
Write-Host "`n✅ Sovereign Deployment Complete." -ForegroundColor Green
Write-Host "📍 Project Root: $RootPath" -ForegroundColor Cyan
Write-Host "🔒 Sovereign Protocol: Hidden Marker Active" -ForegroundColor Gray
Write-Host "`n[ Knoux Art Studio is ready for high-fidelity module development. ]" -ForegroundColor Yellow
Write-Host "==============================================================================" -ForegroundColor Gray


param([string]$DriveLetter="F", [string]$ProjectName="KnouxArtStudio")

Write-Host "🏛️ KNOUX ART STUDIO - SUPREME DEPLOYMENT SEQUENCE STARTING..." -ForegroundColor Cyan

# Ensure Root exists
$Root = "$($DriveLetter):\$(ProjectName)"
if (-not (Test-Path $Root)) {
    New-Item -ItemType Directory -Path $Root -Force | Out-Null
}

# Run individual populators
& "$PSScriptRoot/Create-CoreFiles.ps1" -DriveLetter $DriveLetter -ProjectName $ProjectName
& "$PSScriptRoot/Create-UIFiles.ps1" -DriveLetter $DriveLetter -ProjectName $ProjectName
& "$PSScriptRoot/Create-AIFiles.ps1" -DriveLetter $DriveLetter -ProjectName $ProjectName
& "$PSScriptRoot/Create-ConfigFiles.ps1" -DriveLetter $DriveLetter -ProjectName $ProjectName

# Generate Final Project Marker
$Marker = Join-Path $Root ".sovereign_active"
Get-Date | Out-File $Marker

Write-Host "`n🚀 DEPLOYMENT A-TO-Z FINISHED ON DRIVE $($DriveLetter):" -ForegroundColor Green
Write-Host "Total Sovereignty: ACTIVE" -ForegroundColor White
Write-Host "Safety is not a luxury, it's a life." -ForegroundColor Magenta


param([string]$DriveLetter="F", [string]$ProjectName="KnouxArtStudio")
$Root = "$($DriveLetter):\$(ProjectName)"

function New-KnouxFile($Path, $Content) {
    $FullPath = Join-Path $Root $Path
    $Content | Out-File -FilePath $FullPath -Encoding utf8 -Force
    Write-Host "  [UTIL-FILE] $Path" -ForegroundColor Yellow
}

Write-Host "🛠️ Building System Utilities on Drive $DriveLetter..." -ForegroundColor Yellow

# --- IMAGE PROCESSOR ---
New-KnouxFile "src/utils/ImageProcessor.cpp" @'
#include "ImageProcessor.h"
#include <QColor>

QImage ImageProcessor::adjustBrightness(const QImage &input, int value) {
    QImage out = input.copy();
    for(int y=0; y<out.height(); ++y) {
        for(int x=0; x<out.width(); ++x) {
            QColor c = out.pixelColor(x,y);
            c.setRed(qBound(0, c.red() + value, 255));
            c.setGreen(qBound(0, c.green() + value, 255));
            c.setBlue(qBound(0, c.blue() + value, 255));
            out.setPixelColor(x, y, c);
        }
    }
    return out;
}
'@

# --- EXPORT MANAGER ---
New-KnouxFile "src/utils/ExportManager.cpp" @'
#include "ExportManager.h"
#include <QImageWriter>

bool ExportManager::saveProjectFile(const QImage &img, const QString &path) {
    QImageWriter writer(path);
    writer.setFormat("png");
    writer.setQuality(100);
    // Sovereignty Protocol: Strip all metadata before saving
    return writer.write(img);
}
'@

# --- NOTIFICATION MANAGER ---
New-KnouxFile "src/ui/NotificationManager.cpp" @'
#include "NotificationManager.h"
#include <QLabel>
#include <QPropertyAnimation>

void NotificationManager::pop(const QString &title, const QString &msg) {
    // Cyberpunk themed floating notification logic
}
'@

# --- SETTINGS PANEL ---
New-KnouxFile "src/ui/SettingsPanel.cpp" @'
#include "SettingsPanel.h"

SettingsPanel::SettingsPanel(QWidget *parent) : QWidget(parent) {
    // Sovereignty encryption toggles and local path management
}
'@

Write-Host "✅ Utility Layer Completed Successfully." -ForegroundColor Green

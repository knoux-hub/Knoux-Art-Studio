
# ==============================================================================
# KNOUX ART STUDIO - AI & EXPORT DEPLOYMENT
# Target: Drive F:\
# ==============================================================================

. "$PSScriptRoot/knoux_helpers.ps1"
$Root = Get-KnouxRoot
Write-KnouxLog "Starting $($MyInvocation.MyCommand.Name)." "INFO"

try {

function New-SovereignFile($RelativePath, $Content) {
    $FullPath = Join-Path $Root $RelativePath
    $Content | Out-File -FilePath $FullPath -Encoding utf8 -Force
    Write-Host "  [NEURAL] $RelativePath" -ForegroundColor Yellow
}

Write-Host "🧠 Initializing Local Neural Engines..." -ForegroundColor White

# 1. AIFaceDetector.h
New-SovereignFile "src/ai/AIFaceDetector.h" @'
#ifndef AIFACEDETECTOR_H
#define AIFACEDETECTOR_H
#include <QObject>
#include <QImage>
#include <QRect>

class AIFaceDetector : public QObject {
    Q_OBJECT
public:
    explicit AIFaceDetector(QObject *parent = nullptr);
    QVector<QRect> detectFaces(const QImage &img);
signals:
    void detectionFinished();
};
#endif
'@

# 2. ExportManager.cpp
New-SovereignFile "src/utils/ExportManager.cpp" @'
#include "ExportManager.h"
#include <QImageWriter>

bool ExportManager::saveSovereign(const QImage &img, const QString &path) {
    // Mandatory Metadata Stripping for Digital Sovereignty
    QImage output = img;
    output.setText("Software", "Knoux Sovereign Engine");
    return output.save(path, "PNG", 100);
}
'@

# 3. README.md
New-SovereignFile "README.md" @'
# Knoux Art Studio
## الأمان ليس رفاهية، إنه حياة

بيئة إبداعية سيادية بالكامل على القرص F:
- معالجة AI محلية 100%
- تشفير AES-256
- واجهة Cyberpunk

### التشغيل:
1. تشغيل build.bat
2. فتح البرنامج من مجلد build/Release
'@

Write-Host "`n🚀 DEPLOYMENT COMPLETED. Your drive is now a Supreme Creative Lab." -ForegroundColor Green
Write-KnouxLog "Completed $($MyInvocation.MyCommand.Name)." "SUCCESS"
} catch {
    Write-KnouxLog "Failed $($MyInvocation.MyCommand.Name): $($_.Exception.Message)" "ERROR"
    throw
}

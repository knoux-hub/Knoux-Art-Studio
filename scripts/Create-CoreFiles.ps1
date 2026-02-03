
param([string]$DriveLetter="F", [string]$ProjectName="KnouxArtStudio")
$Root = "$($DriveLetter):\$(ProjectName)"

function New-KnouxFile($Path, $Content) {
    $FullPath = Join-Path $Root $Path
    $Content | Out-File -FilePath $FullPath -Encoding utf8 -Force
    Write-Host "  [CORE-FILE] $Path" -ForegroundColor Magenta
}

Write-Host "🧠 Populating Sovereign Core Intelligence on $Root..." -ForegroundColor Cyan

# Populate MainWindow.cpp (Logic for Mode Switching & Notifications)
New-KnouxFile "src/core/MainWindow.cpp" @'
#include "MainWindow.h"
#include "../ui/GlassPanel.h"
#include "../ui/GlassButton.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    m_notificationManager = new NotificationManager(this);
    switchMode(Mode_Home);
}

void MainWindow::switchMode(AppMode mode) {
    m_currentMode = mode;
    m_centralStack->setCurrentIndex(static_cast<int>(mode));
    showNotification("Knoux System", "Switched to Sovereign Mode: " + QString::number(mode));
}
'@

# Populate main.cpp
New-KnouxFile "src/core/main.cpp" @'
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Knoux Art Studio");
    MainWindow w;
    w.showMaximized();
    return app.exec();
}
'@

Write-Host "✅ Core Files Populated Successfully." -ForegroundColor Green

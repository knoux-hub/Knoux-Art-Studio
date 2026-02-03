
param([string]$DriveLetter="F", [string]$ProjectName="KnouxArtStudio")
$Root = "$($DriveLetter):\$(ProjectName)"

function New-KnouxFile($Path, $Content) {
    $FullPath = Join-Path $Root $Path
    $Content | Out-File -FilePath $FullPath -Encoding utf8 -Force
    Write-Host "  [UI-FILE] $Path" -ForegroundColor Yellow
}

Write-Host "🎨 Building Glassmorphism UI Components..." -ForegroundColor White

# GlassPanel.cpp - The foundation of our visual identity
New-KnouxFile "src/ui/GlassPanel.cpp" @'
#include "GlassPanel.h"
#include <QPainter>

void GlassPanel::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient grad(0, 0, width(), height());
    grad.setColorAt(0, QColor(255, 255, 255, 30));
    grad.setColorAt(1, QColor(255, 255, 255, 10));
    painter.setBrush(grad);
    painter.setPen(QPen(QColor(255,255,255,40), 1));
    painter.drawRoundedRect(rect().adjusted(1,1,-1,-1), 12, 12);
}
'@

# HomeScreen.cpp - Professional Dashboard
New-KnouxFile "src/ui/HomeScreen.cpp" @'
#include "HomeScreen.h"
#include <QLabel>
#include <QVBoxLayout>

HomeScreen::HomeScreen(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *title = new QLabel("Knoux Art Studio Sovereign Dashboard");
    title->setStyleSheet("font-size: 32px; color: #9B59FF; font-weight: bold;");
    layout->addWidget(title, 0, Qt::AlignCenter);
}
'@

Write-Host "✅ UI Layer Completed." -ForegroundColor Green

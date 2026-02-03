#include "MainWindow.h"
#include "../ui/GlassPanel.h"
#include "../core/StyleManager.h"
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    applyCyberpunkTheme();
}

void MainWindow::switchMode(AppMode mode) {
    m_centralStack->setCurrentIndex(static_cast<int>(mode));
    m_notificationManager->showNotification("System", "Sovereign Mode Activated");
}

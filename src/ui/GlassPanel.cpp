#include "GlassPanel.h"
#include <QPainter>
#include <QGraphicsBlurEffect>

GlassPanel::GlassPanel(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
}

void GlassPanel::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Sovereign Glassmorphism Logic
    QLinearGradient grad(0, 0, width(), height());
    grad.setColorAt(0, QColor(255, 255, 255, 35));
    grad.setColorAt(1, QColor(255, 255, 255, 10));
    
    painter.setBrush(grad);
    painter.setPen(QPen(QColor(255, 255, 255, 45), 1));
    painter.drawRoundedRect(rect().adjusted(1,1,-1,-1), 15, 15);
    
    // Inner Glow
    painter.setPen(QPen(QColor(155, 89, 255, 30), 2));
    painter.drawRoundedRect(rect().adjusted(3,3,-3,-3), 13, 13);
}

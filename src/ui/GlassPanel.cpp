#include "GlassPanel.h"
#include <QPainter>
#include <QLinearGradient>

GlassPanel::GlassPanel(QWidget *parent)
    : QWidget(parent)
    , m_glassOpacity(0.1f)
    , m_glowIntensity(0.0f)
    , m_accentColor(0, 255, 255)
    , m_blurRadius(20)
    , m_borderRadius(12)
    , m_borderWidth(1)
    , m_borderColor(255, 255, 255, 30)
    , m_blurEffect(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void GlassPanel::setGlassOpacity(float opacity)
{
    m_glassOpacity = opacity;
    update();
}

void GlassPanel::setGlowIntensity(float intensity)
{
    m_glowIntensity = intensity;
    update();
}

void GlassPanel::setAccentColor(const QColor &color)
{
    m_accentColor = color;
    update();
}

void GlassPanel::setBlurRadius(int radius)
{
    m_blurRadius = radius;
    update();
}

void GlassPanel::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void GlassPanel::setBorderWidth(int width)
{
    m_borderWidth = width;
    update();
}

void GlassPanel::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    update();
}

void GlassPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawBorder(painter);
}

void GlassPanel::drawBackground(QPainter &painter)
{
    QRectF rect = this->rect().adjusted(2, 2, -2, -2);

    // Frosted glass gradient
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(255, 255, 255, int(25 * m_glassOpacity)));
    gradient.setColorAt(0.5, QColor(255, 255, 255, int(15 * m_glassOpacity)));
    gradient.setColorAt(1, QColor(255, 255, 255, int(8 * m_glassOpacity)));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);

    // Top highlight
    QLinearGradient highlight(rect.topLeft(), QPointF(rect.left(), rect.top() + 30));
    highlight.setColorAt(0, QColor(255, 255, 255, 30));
    highlight.setColorAt(1, Qt::transparent);
    painter.setBrush(highlight);
    painter.drawRoundedRect(rect.adjusted(0, 0, 0, -rect.height() + 35), 
                            m_borderRadius, m_borderRadius);
}

void GlassPanel::drawBorder(QPainter &painter)
{
    QRectF rect = this->rect().adjusted(1, 1, -1, -1);

    // Outer glow
    if (m_glowIntensity > 0) {
        QColor glow = m_accentColor;
        glow.setAlpha(int(m_glowIntensity));
        QPen glowPen(glow, 2);
        painter.setPen(glowPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), m_borderRadius + 2, m_borderRadius + 2);
    }

    // Main border
    QPen borderPen(m_borderColor, m_borderWidth);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);
}

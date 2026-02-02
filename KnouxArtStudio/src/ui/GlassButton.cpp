#include "GlassButton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QRadialGradient>
#include <QLinearGradient>

GlassButton::GlassButton(QWidget *parent)
    : QPushButton(parent)
    , m_glassOpacity(0.15f)
    , m_glowIntensity(0.0f)
    , m_hoverGlowIntensity(30.0f)
    , m_accentColor(0, 255, 255)
    , m_hoverColor(255, 0, 128)
    , m_borderRadius(12)
    , m_borderWidth(1)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_glowAnimation(nullptr)
    , m_opacityAnimation(nullptr)
{
    setupAnimations();
    setCursor(Qt::PointingHandCursor);
    setMinimumSize(44, 44);
}

GlassButton::GlassButton(const QString &text, QWidget *parent)
    : GlassButton(parent)
{
    setText(text);
}

void GlassButton::setupAnimations()
{
    m_glowAnimation = new QPropertyAnimation(this, "glowIntensity", this);
    m_glowAnimation->setDuration(200);
    m_glowAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_opacityAnimation = new QPropertyAnimation(this, "glassOpacity", this);
    m_opacityAnimation->setDuration(150);
}

void GlassButton::setGlassOpacity(float opacity)
{
    m_glassOpacity = opacity;
    update();
}

void GlassButton::setGlowIntensity(float intensity)
{
    m_glowIntensity = intensity;
    update();
}

void GlassButton::setAccentColor(const QColor &color)
{
    m_accentColor = color;
    update();
}

void GlassButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawGlassBackground(painter);
    drawGlowEffect(painter);
    drawBorder(painter);
    drawText(painter);
}

void GlassButton::drawGlassBackground(QPainter &painter)
{
    QRectF rect = this->rect().adjusted(2, 2, -2, -2);

    // Base glass gradient
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());

    if (isChecked()) {
        // Active/pressed state - more opaque with accent tint
        gradient.setColorAt(0, QColor(m_accentColor.red(), m_accentColor.green(), 
                                       m_accentColor.blue(), 60));
        gradient.setColorAt(1, QColor(m_accentColor.red(), m_accentColor.green(), 
                                       m_accentColor.blue(), 30));
    } else if (m_isPressed) {
        gradient.setColorAt(0, QColor(255, 255, 255, 40));
        gradient.setColorAt(1, QColor(255, 255, 255, 20));
    } else {
        gradient.setColorAt(0, QColor(255, 255, 255, int(40 * m_glassOpacity)));
        gradient.setColorAt(0.5, QColor(255, 255, 255, int(20 * m_glassOpacity)));
        gradient.setColorAt(1, QColor(255, 255, 255, int(10 * m_glassOpacity)));
    }

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);

    // Top highlight
    QLinearGradient highlightGradient(rect.topLeft(), QPointF(rect.left(), rect.top() + rect.height() * 0.4));
    highlightGradient.setColorAt(0, QColor(255, 255, 255, 40));
    highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));

    painter.setBrush(highlightGradient);
    painter.drawRoundedRect(rect.adjusted(0, 0, 0, -rect.height() * 0.5), m_borderRadius, m_borderRadius);
}

void GlassButton::drawGlowEffect(QPainter &painter)
{
    if (m_glowIntensity <= 0 && !m_isHovered) return;

    float intensity = m_isHovered ? m_hoverGlowIntensity : m_glowIntensity;
    QColor glowColor = m_accentColor;
    glowColor.setAlpha(int(intensity * 2.5));

    // Outer glow
    QRadialGradient glowGradient(rect().center(), rect().width() * 0.8);
    glowGradient.setColorAt(0, glowColor);
    glowGradient.setColorAt(0.5, QColor(glowColor.red(), glowColor.green(), 
                                         glowColor.blue(), int(intensity)));
    glowGradient.setColorAt(1, Qt::transparent);

    painter.setBrush(glowGradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(-10, -10, 10, 10), m_borderRadius + 5, m_borderRadius + 5);
}

void GlassButton::drawBorder(QPainter &painter)
{
    QRectF rect = this->rect().adjusted(1, 1, -1, -1);

    QColor borderColor = m_accentColor;
    if (m_isHovered) {
        borderColor = m_hoverColor;
    }
    borderColor.setAlpha(m_isHovered ? 180 : 80);

    QPen pen(borderColor, m_borderWidth);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);

    // Inner border glow
    if (m_isHovered || isChecked()) {
        QColor innerGlow = m_accentColor;
        innerGlow.setAlpha(60);
        QPen innerPen(innerGlow, 2);
        painter.setPen(innerPen);
        painter.drawRoundedRect(rect.adjusted(2, 2, -2, -2), m_borderRadius - 2, m_borderRadius - 2);
    }
}

void GlassButton::drawText(QPainter &painter)
{
    if (text().isEmpty()) return;

    painter.setPen(Qt::white);

    QFont font = this->font();
    if (text().length() <= 2) {
        // Emoji/single char - larger
        font.setPointSize(20);
    } else {
        font.setPointSize(12);
        font.setBold(true);
    }
    painter.setFont(font);

    // Text shadow
    painter.setPen(QColor(0, 0, 0, 128));
    painter.drawText(rect().adjusted(1, 1, 1, 1), Qt::AlignCenter, text());

    // Main text with glow
    QColor textColor = isChecked() ? m_accentColor : Qt::white;
    if (m_isHovered) {
        textColor = m_hoverColor;
    }
    painter.setPen(textColor);
    painter.drawText(rect(), Qt::AlignCenter, text());
}

void GlassButton::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;

    m_glowAnimation->stop();
    m_glowAnimation->setStartValue(m_glowIntensity);
    m_glowAnimation->setEndValue(m_hoverGlowIntensity);
    m_glowAnimation->start();

    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_glassOpacity);
    m_opacityAnimation->setEndValue(0.25f);
    m_opacityAnimation->start();

    QPushButton::enterEvent(event);
}

void GlassButton::leaveEvent(QEvent *event)
{
    m_isHovered = false;

    m_glowAnimation->stop();
    m_glowAnimation->setStartValue(m_glowIntensity);
    m_glowAnimation->setEndValue(0.0f);
    m_glowAnimation->start();

    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_glassOpacity);
    m_opacityAnimation->setEndValue(0.15f);
    m_opacityAnimation->start();

    QPushButton::leaveEvent(event);
}

void GlassButton::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
    update();
    QPushButton::mousePressEvent(event);
}

void GlassButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    update();
    QPushButton::mouseReleaseEvent(event);
}

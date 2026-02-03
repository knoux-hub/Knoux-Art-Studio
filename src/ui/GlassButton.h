#ifndef GLASSBUTTON_H
#define GLASSBUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>

class GlassButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(float glassOpacity READ glassOpacity WRITE setGlassOpacity)
    Q_PROPERTY(float glowIntensity READ glowIntensity WRITE setGlowIntensity)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor)

public:
    explicit GlassButton(QWidget *parent = nullptr);
    explicit GlassButton(const QString &text, QWidget *parent = nullptr);

    float glassOpacity() const { return m_glassOpacity; }
    void setGlassOpacity(float opacity);

    float glowIntensity() const { return m_glowIntensity; }
    void setGlowIntensity(float intensity);

    QColor accentColor() const { return m_accentColor; }
    void setAccentColor(const QColor &color);

    void setBorderRadius(int radius) { m_borderRadius = radius; update(); }
    void setBorderWidth(int width) { m_borderWidth = width; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupAnimations();
    void drawGlassBackground(QPainter &painter);
    void drawGlowEffect(QPainter &painter);
    void drawBorder(QPainter &painter);
    void drawText(QPainter &painter);

    float m_glassOpacity;
    float m_glowIntensity;
    float m_hoverGlowIntensity;
    QColor m_accentColor;
    QColor m_hoverColor;
    int m_borderRadius;
    int m_borderWidth;
    bool m_isHovered;
    bool m_isPressed;

    QPropertyAnimation *m_glowAnimation;
    QPropertyAnimation *m_opacityAnimation;
};

#endif // GLASSBUTTON_H

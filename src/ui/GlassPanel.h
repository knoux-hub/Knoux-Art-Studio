#ifndef GLASSPANEL_H
#define GLASSPANEL_H

#include <QWidget>
#include <QGraphicsBlurEffect>

class GlassPanel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float glassOpacity READ glassOpacity WRITE setGlassOpacity)
    Q_PROPERTY(float glowIntensity READ glowIntensity WRITE setGlowIntensity)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor)

public:
    explicit GlassPanel(QWidget *parent = nullptr);

    float glassOpacity() const { return m_glassOpacity; }
    void setGlassOpacity(float opacity);

    float glowIntensity() const { return m_glowIntensity; }
    void setGlowIntensity(float intensity);

    QColor accentColor() const { return m_accentColor; }
    void setAccentColor(const QColor &color);

    void setBlurRadius(int radius);
    void setBorderRadius(int radius);
    void setBorderWidth(int width);
    void setBorderColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBackground(QPainter &painter);
    void drawBlur(QPainter &painter);
    void drawBorder(QPainter &painter);

    float m_glassOpacity;
    float m_glowIntensity;
    QColor m_accentColor;
    int m_blurRadius;
    int m_borderRadius;
    int m_borderWidth;
    QColor m_borderColor;

    QGraphicsBlurEffect *m_blurEffect;
};

#endif // GLASSPANEL_H

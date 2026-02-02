#ifndef CYBERPUNKSPLASH_H
#define CYBERPUNKSPLASH_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>

class CyberpunkSplash : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float progress READ progress WRITE setProgress)
    Q_PROPERTY(float glowIntensity READ glowIntensity WRITE setGlowIntensity)

public:
    explicit CyberpunkSplash(QWidget *parent = nullptr);

    float progress() const { return m_progress; }
    void setProgress(float progress);

    float glowIntensity() const { return m_glowIntensity; }
    void setGlowIntensity(float intensity);

public slots:
    void updateLoadingText(const QString &text);
    void finish();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawBackground(QPainter &painter);
    void drawLogo(QPainter &painter);
    void drawProgressBar(QPainter &painter);
    void drawLoadingText(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawParticles(QPainter &painter);
    void setupAnimations();
    void updateParticles();

    float m_progress;
    float m_glowIntensity;
    QString m_loadingText;
    QStringList m_loadingTexts;
    int m_currentTextIndex;

    QPropertyAnimation *m_progressAnimation;
    QPropertyAnimation *m_glowAnimation;
    QTimer *m_textTimer;
    QTimer *m_particleTimer;

    struct Particle {
        QPointF pos;
        QPointF velocity;
        float size;
        float alpha;
        QColor color;
    };
    QVector<Particle> m_particles;
};

#endif // CYBERPUNKSPLASH_H

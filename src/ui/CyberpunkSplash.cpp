#include "CyberpunkSplash.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontDatabase>
#include <QRandomGenerator>

CyberpunkSplash::CyberpunkSplash(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_progress(0.0f)
    , m_glowIntensity(1.0f)
    , m_currentTextIndex(0)
    , m_progressAnimation(nullptr)
    , m_glowAnimation(nullptr)
    , m_textTimer(nullptr)
    , m_particleTimer(nullptr)
{
    setFixedSize(900, 600);
    setAttribute(Qt::WA_TranslucentBackground);

    // Center on screen
    QRect screen = QApplication::primaryScreen()->geometry();
    move(screen.center() - rect().center());

    m_loadingTexts = {
        tr("جارٍ التحميل..."),
        tr("تهيئة النظام..."),
        tr("تحميل نماذج AI..."),
        tr("تهيئة واجهة المستخدم..."),
        tr("فحص GPU..."),
        tr("تحميل الموارد..."),
        tr("جاهز للبدء!")
    };
    m_loadingText = m_loadingTexts.first();

    // Initialize particles
    for (int i = 0; i < 50; ++i) {
        Particle p;
        p.pos = QPointF(QRandomGenerator::global()->bounded(width()),
                        QRandomGenerator::global()->bounded(height()));
        p.velocity = QPointF(QRandomGenerator::global()->bounded(-2.0, 2.0),
                             QRandomGenerator::global()->bounded(-2.0, 2.0));
        p.size = QRandomGenerator::global()->bounded(1.0, 4.0);
        p.alpha = QRandomGenerator::global()->bounded(0.3, 1.0);
        p.color = QRandomGenerator::global()->bounded(2) == 0 
                  ? QColor(0, 255, 255) : QColor(255, 0, 128);
        m_particles.append(p);
    }

    setupAnimations();
}

void CyberpunkSplash::setupAnimations()
{
    // Progress animation
    m_progressAnimation = new QPropertyAnimation(this, "progress", this);
    m_progressAnimation->setDuration(2500);
    m_progressAnimation->setStartValue(0.0f);
    m_progressAnimation->setEndValue(1.0f);
    m_progressAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_progressAnimation->start();

    // Glow pulse animation
    m_glowAnimation = new QPropertyAnimation(this, "glowIntensity", this);
    m_glowAnimation->setDuration(1000);
    m_glowAnimation->setStartValue(0.5f);
    m_glowAnimation->setEndValue(1.5f);
    m_glowAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_glowAnimation->setLoopCount(-1);
    m_glowAnimation->start();

    // Text rotation timer
    m_textTimer = new QTimer(this);
    connect(m_textTimer, &QTimer::timeout, this, [this]() {
        m_currentTextIndex = (m_currentTextIndex + 1) % m_loadingTexts.size();
        m_loadingText = m_loadingTexts[m_currentTextIndex];
        update();
    });
    m_textTimer->start(350);

    // Particle update timer
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, &CyberpunkSplash::updateParticles);
    m_particleTimer->start(16); // ~60fps
}

void CyberpunkSplash::updateParticles()
{
    for (Particle &p : m_particles) {
        p.pos += p.velocity;

        // Wrap around screen
        if (p.pos.x() < 0) p.pos.setX(width());
        if (p.pos.x() > width()) p.pos.setX(0);
        if (p.pos.y() < 0) p.pos.setY(height());
        if (p.pos.y() > height()) p.pos.setY(0);
    }
    update();
}

void CyberpunkSplash::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGrid(painter);
    drawParticles(painter);
    drawLogo(painter);
    drawProgressBar(painter);
    drawLoadingText(painter);
}

void CyberpunkSplash::drawBackground(QPainter &painter)
{
    // Dark cyberpunk gradient
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(5, 5, 15));
    gradient.setColorAt(0.5, QColor(10, 10, 25));
    gradient.setColorAt(1, QColor(15, 15, 35));
    painter.fillRect(rect(), gradient);

    // Vignette effect
    QRadialGradient vignette(width() / 2, height() / 2, width() * 0.8);
    vignette.setColorAt(0, Qt::transparent);
    vignette.setColorAt(1, QColor(0, 0, 0, 180));
    painter.fillRect(rect(), vignette);
}

void CyberpunkSplash::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor(0, 255, 255, 15), 1));

    int gridSize = 40;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }
}

void CyberpunkSplash::drawParticles(QPainter &painter)
{
    for (const Particle &p : m_particles) {
        QColor c = p.color;
        c.setAlphaF(p.alpha * 0.6);
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(p.pos, p.size, p.size);
    }
}

void CyberpunkSplash::drawLogo(QPainter &painter)
{
    int centerX = width() / 2;
    int centerY = height() / 2 - 50;

    // Glow effect
    float glow = m_glowIntensity * 20;
    for (int i = 5; i >= 0; --i) {
        QColor glowColor(0, 255, 255, int(30 - i * 5));
        painter.setPen(QPen(glowColor, glow - i * 3));
        painter.drawEllipse(centerX - 60 - i * 2, centerY - 60 - i * 2, 
                           120 + i * 4, 120 + i * 4);
    }

    // Main logo hexagon
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        float angle = M_PI / 3 * i - M_PI / 2;
        hexagon << QPointF(centerX + 50 * cos(angle), centerY + 50 * sin(angle));
    }

    QLinearGradient hexGrad(centerX - 50, centerY - 50, centerX + 50, centerY + 50);
    hexGrad.setColorAt(0, QColor(0, 255, 255));
    hexGrad.setColorAt(1, QColor(255, 0, 128));

    painter.setBrush(hexGrad);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawPolygon(hexagon);

    // Inner K
    painter.setPen(QPen(Qt::white, 4));
    QFont font("Orbitron", 36, QFont::Bold);
    painter.setFont(font);
    painter.drawText(centerX - 25, centerY + 15, "K");

    // Title
    QFont titleFont("Dubai", 32, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(QPen(Qt::white));
    painter.drawText(centerX - 100, centerY + 100, "KNOUX");

    QFont subFont("Dubai", 14);
    painter.setFont(subFont);
    painter.setPen(QPen(QColor(255, 0, 128)));
    painter.drawText(centerX - 80, centerY + 125, "ART STUDIO");

    // Arabic tagline
    QFont arFont("Dubai", 11);
    painter.setFont(arFont);
    painter.setPen(QPen(QColor(0, 255, 255)));
    painter.drawText(centerX - 90, centerY + 150, "الأمان ليس رفاهية، إنه حياة");
}

void CyberpunkSplash::drawProgressBar(QPainter &painter)
{
    int barWidth = 400;
    int barHeight = 6;
    int x = (width() - barWidth) / 2;
    int y = height() - 120;

    // Background
    painter.setBrush(QColor(30, 30, 50));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(x, y, barWidth, barHeight, 3, 3);

    // Progress with gradient
    int progressWidth = int(barWidth * m_progress);
    QLinearGradient progressGrad(x, y, x + progressWidth, y);
    progressGrad.setColorAt(0, QColor(0, 255, 255));
    progressGrad.setColorAt(1, QColor(255, 0, 128));

    painter.setBrush(progressGrad);
    painter.drawRoundedRect(x, y, progressWidth, barHeight, 3, 3);

    // Glow
    painter.setPen(QPen(QColor(0, 255, 255, 50), 4));
    painter.drawRoundedRect(x - 2, y - 2, barWidth + 4, barHeight + 4, 5, 5);
}

void CyberpunkSplash::drawLoadingText(QPainter &painter)
{
    QFont font("Dubai", 11);
    painter.setFont(font);
    painter.setPen(QPen(QColor(180, 180, 200)));

    QRect textRect(0, height() - 80, width(), 30);
    painter.drawText(textRect, Qt::AlignCenter, m_loadingText);

    // Version
    QFont verFont("Dubai", 9);
    painter.setFont(verFont);
    painter.setPen(QPen(QColor(100, 100, 120)));
    painter.drawText(width() - 100, height() - 20, "v1.0.0");
}

void CyberpunkSplash::setProgress(float progress)
{
    m_progress = progress;
    update();
}

void CyberpunkSplash::setGlowIntensity(float intensity)
{
    m_glowIntensity = intensity;
    update();
}

void CyberpunkSplash::updateLoadingText(const QString &text)
{
    m_loadingText = text;
    update();
}

void CyberpunkSplash::finish()
{
    m_textTimer->stop();
    m_particleTimer->stop();
    close();
}

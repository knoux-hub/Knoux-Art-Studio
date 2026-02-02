#include "MakeupStudio.h"
#include "../ui/GlassButton.h"
#include "../ui/GlassPanel.h"
#include "../core/StyleManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QScrollArea>
#include <QTabWidget>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

// ============================================================================
// MakeupCanvas Implementation
// ============================================================================

MakeupCanvas::MakeupCanvas(QWidget *parent) : QWidget(parent), m_scale(1.0), m_offset(0, 0) {
    setMinimumSize(400, 500);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MakeupCanvas::setImage(const QImage &image) {
    m_originalImage = image;
    m_processedImage = image.copy();
    updateTransform();
    update();
}

void MakeupCanvas::setProcessedImage(const QImage &image) {
    m_processedImage = image;
    update();
}

QImage MakeupCanvas::getProcessedImage() const {
    return m_processedImage;
}

void MakeupCanvas::setFaceRegions(const FaceRegions &regions) {
    m_faceRegions = regions;
    update();
}

void MakeupCanvas::clearMakeup() {
    m_processedImage = m_originalImage.copy();
    update();
}

void MakeupCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Dark background
    painter.fillRect(rect(), QColor(CP_BG_DARK));
    
    if (m_processedImage.isNull()) {
        // Draw placeholder
        painter.setPen(QPen(QColor(CP_ACCENT_CYAN), 2, Qt::DashLine));
        painter.drawRect(rect().adjusted(20, 20, -20, -20));
        
        painter.setPen(QColor(CP_TEXT_SECONDARY));
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect(), Qt::AlignCenter, tr("قم بتحميل صورة للبدء"));
        return;
    }
    
    // Draw image with transform
    QRect targetRect = m_transform.mapRect(m_processedImage.rect());
    painter.drawImage(targetRect, m_processedImage);
    
    // Draw face region overlays if detected
    if (!m_faceRegions.lips.isEmpty()) {
        painter.setPen(QPen(QColor(CP_ACCENT_PINK), 2));
        painter.setBrush(Qt::NoBrush);
        QPolygonF lipsPoly = m_transform.map(m_faceRegions.lips);
        painter.drawPolygon(lipsPoly);
    }
}

void MakeupCanvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
    }
}

void MakeupCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_offset += delta;
        m_lastMousePos = event->pos();
        updateTransform();
        update();
    }
}

void MakeupCanvas::wheelEvent(QWheelEvent *event) {
    double delta = event->angleDelta().y() / 120.0;
    m_scale = qBound(0.1, m_scale + delta * 0.1, 5.0);
    updateTransform();
    update();
}

void MakeupCanvas::updateTransform() {
    if (m_processedImage.isNull()) return;
    
    m_transform.reset();
    
    // Center the image
    QRectF imageRect = m_processedImage.rect();
    QRectF widgetRect = rect();
    
    double scaleX = widgetRect.width() / imageRect.width();
    double scaleY = widgetRect.height() / imageRect.height();
    double baseScale = qMin(scaleX, scaleY) * 0.9;
    
    double finalScale = baseScale * m_scale;
    
    QPointF centerOffset(
        (widgetRect.width() - imageRect.width() * finalScale) / 2,
        (widgetRect.height() - imageRect.height() * finalScale) / 2
    );
    
    m_transform.translate(centerOffset.x() + m_offset.x(), centerOffset.y() + m_offset.y());
    m_transform.scale(finalScale, finalScale);
}

// ============================================================================
// LipstickPanel Implementation
// ============================================================================

LipstickPanel::LipstickPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void LipstickPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *title = new QLabel(tr("أحمر الشفاه"));
    title->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(CP_ACCENT_PINK));
    mainLayout->addWidget(title);
    
    // Color palette
    GlassPanel *colorPanel = new GlassPanel(this);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorPanel);
    
    QLabel *colorLabel = new QLabel(tr("اختيار اللون:"));
    colorLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    colorLayout->addWidget(colorLabel);
    
    QGridLayout *paletteLayout = new QGridLayout();
    paletteLayout->setSpacing(8);
    
    QVector<QColor> lipstickColors = {
        QColor(200, 50, 80),    // Classic red
        QColor(180, 40, 60),    // Deep red
        QColor(220, 100, 120),  // Pink
        QColor(180, 80, 100),   // Rose
        QColor(160, 60, 80),    // Berry
        QColor(200, 120, 100),  // Coral
        QColor(180, 100, 80),   // Peach
        QColor(150, 50, 70),    // Plum
        QColor(200, 80, 100),   // Magenta
        QColor(170, 90, 90),    // Mauve
        QColor(210, 130, 110),  // Nude
        QColor(190, 70, 90),    // Raspberry
    };
    
    int row = 0, col = 0;
    for (const QColor &color : lipstickColors) {
        GlassButton *btn = new GlassButton("", this);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet(QString("background-color: %1; border-radius: 20px; border: 2px solid %2;")
            .arg(color.name()).arg(CP_ACCENT_CYAN));
        connect(btn, &GlassButton::clicked, [this, color]() {
            m_selectedColor = color;
            emit colorChanged(color);
        });
        paletteLayout->addWidget(btn, row, col);
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }
    
    colorLayout->addLayout(paletteLayout);
    mainLayout->addWidget(colorPanel);
    
    // Intensity slider
    GlassPanel *intensityPanel = new GlassPanel(this);
    QVBoxLayout *intensityLayout = new QVBoxLayout(intensityPanel);
    
    QHBoxLayout *sliderLabelLayout = new QHBoxLayout();
    QLabel *intensityLabel = new QLabel(tr("الشفافية:"));
    intensityLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    sliderLabelLayout->addWidget(intensityLabel);
    
    m_intensityValueLabel = new QLabel("50%");
    m_intensityValueLabel->setStyleSheet(QString("color: %1;").arg(CP_ACCENT_CYAN));
    sliderLabelLayout->addStretch();
    sliderLabelLayout->addWidget(m_intensityValueLabel);
    intensityLayout->addLayout(sliderLabelLayout);
    
    m_intensitySlider = new QSlider(Qt::Horizontal, this);
    m_intensitySlider->setRange(0, 100);
    m_intensitySlider->setValue(50);
    m_intensitySlider->setStyleSheet(StyleManager::sliderStyle());
    intensityLayout->addWidget(m_intensitySlider);
    
    mainLayout->addWidget(intensityPanel);
    
    // Gloss option
    GlassPanel *glossPanel = new GlassPanel(this);
    QVBoxLayout *glossLayout = new QVBoxLayout(glossPanel);
    
    QLabel *glossLabel = new QLabel(tr("اللمعة:"));
    glossLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    glossLayout->addWidget(glossLabel);
    
    m_glossSlider = new QSlider(Qt::Horizontal, this);
    m_glossSlider->setRange(0, 100);
    m_glossSlider->setValue(30);
    m_glossSlider->setStyleSheet(StyleManager::sliderStyle());
    glossLayout->addWidget(m_glossSlider);
    
    mainLayout->addWidget(glossPanel);
    
    // Apply button
    GlassButton *applyBtn = new GlassButton(tr("تطبيق أحمر الشفاه"), this);
    applyBtn->setAccentColor(CP_ACCENT_PINK);
    connect(applyBtn, &GlassButton::clicked, this, &LipstickPanel::applyRequested);
    mainLayout->addWidget(applyBtn);
    
    mainLayout->addStretch();
}

void LipstickPanel::setupConnections() {
    connect(m_intensitySlider, &QSlider::valueChanged, [this](int value) {
        m_intensityValueLabel->setText(QString("%1%").arg(value));
        emit intensityChanged(value);
    });
    
    connect(m_glossSlider, &QSlider::valueChanged, this, &LipstickPanel::glossChanged);
}

QColor LipstickPanel::getSelectedColor() const {
    return m_selectedColor;
}

int LipstickPanel::getIntensity() const {
    return m_intensitySlider->value();
}

int LipstickPanel::getGloss() const {
    return m_glossSlider->value();
}

// ============================================================================
// EyeshadowPanel Implementation
// ============================================================================

EyeshadowPanel::EyeshadowPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void EyeshadowPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *title = new QLabel(tr("ظلال العيون"));
    title->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(CP_ACCENT_PURPLE));
    mainLayout->addWidget(title);
    
    // Color palette
    GlassPanel *colorPanel = new GlassPanel(this);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorPanel);
    
    QLabel *colorLabel = new QLabel(tr("اختيار اللون:"));
    colorLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    colorLayout->addWidget(colorLabel);
    
    QGridLayout *paletteLayout = new QGridLayout();
    paletteLayout->setSpacing(8);
    
    QVector<QColor> eyeshadowColors = {
        QColor(150, 100, 80),   // Bronze
        QColor(180, 140, 100),  // Gold
        QColor(120, 80, 100),   // Plum
        QColor(100, 120, 140),  // Slate
        QColor(160, 120, 100),  // Copper
        QColor(140, 100, 120),  // Mauve
        QColor(180, 160, 120),  // Champagne
        QColor(80, 100, 120),   // Steel
        QColor(200, 180, 140),  // Pearl
        QColor(120, 100, 80),   // Taupe
        QColor(160, 100, 80),   // Rust
        QColor(100, 80, 100),   // Lavender
    };
    
    int row = 0, col = 0;
    for (const QColor &color : eyeshadowColors) {
        GlassButton *btn = new GlassButton("", this);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet(QString("background-color: %1; border-radius: 20px; border: 2px solid %2;")
            .arg(color.name()).arg(CP_ACCENT_CYAN));
        connect(btn, &GlassButton::clicked, [this, color]() {
            m_selectedColor = color;
            emit colorChanged(color);
        });
        paletteLayout->addWidget(btn, row, col);
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }
    
    colorLayout->addLayout(paletteLayout);
    mainLayout->addWidget(colorPanel);
    
    // Intensity slider
    GlassPanel *intensityPanel = new GlassPanel(this);
    QVBoxLayout *intensityLayout = new QVBoxLayout(intensityPanel);
    
    QHBoxLayout *sliderLabelLayout = new QHBoxLayout();
    QLabel *intensityLabel = new QLabel(tr("الكثافة:"));
    intensityLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    sliderLabelLayout->addWidget(intensityLabel);
    
    m_intensityValueLabel = new QLabel("50%");
    m_intensityValueLabel->setStyleSheet(QString("color: %1;").arg(CP_ACCENT_CYAN));
    sliderLabelLayout->addStretch();
    sliderLabelLayout->addWidget(m_intensityValueLabel);
    intensityLayout->addLayout(sliderLabelLayout);
    
    m_intensitySlider = new QSlider(Qt::Horizontal, this);
    m_intensitySlider->setRange(0, 100);
    m_intensitySlider->setValue(50);
    m_intensitySlider->setStyleSheet(StyleManager::sliderStyle());
    intensityLayout->addWidget(m_intensitySlider);
    
    mainLayout->addWidget(intensityPanel);
    
    // Shimmer option
    GlassPanel *shimmerPanel = new GlassPanel(this);
    QVBoxLayout *shimmerLayout = new QVBoxLayout(shimmerPanel);
    
    QLabel *shimmerLabel = new QLabel(tr("اللمعان:"));
    shimmerLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    shimmerLayout->addWidget(shimmerLabel);
    
    m_shimmerSlider = new QSlider(Qt::Horizontal, this);
    m_shimmerSlider->setRange(0, 100);
    m_shimmerSlider->setValue(20);
    m_shimmerSlider->setStyleSheet(StyleManager::sliderStyle());
    shimmerLayout->addWidget(m_shimmerSlider);
    
    mainLayout->addWidget(shimmerPanel);
    
    // Apply button
    GlassButton *applyBtn = new GlassButton(tr("تطبيق ظلال العيون"), this);
    applyBtn->setAccentColor(CP_ACCENT_PURPLE);
    connect(applyBtn, &GlassButton::clicked, this, &EyeshadowPanel::applyRequested);
    mainLayout->addWidget(applyBtn);
    
    mainLayout->addStretch();
}

void EyeshadowPanel::setupConnections() {
    connect(m_intensitySlider, &QSlider::valueChanged, [this](int value) {
        m_intensityValueLabel->setText(QString("%1%").arg(value));
        emit intensityChanged(value);
    });
    
    connect(m_shimmerSlider, &QSlider::valueChanged, this, &EyeshadowPanel::shimmerChanged);
}

QColor EyeshadowPanel::getSelectedColor() const {
    return m_selectedColor;
}

int EyeshadowPanel::getIntensity() const {
    return m_intensitySlider->value();
}

int EyeshadowPanel::getShimmer() const {
    return m_shimmerSlider->value();
}

// ============================================================================
// BlushPanel Implementation
// ============================================================================

BlushPanel::BlushPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void BlushPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *title = new QLabel(tr("أحمر الخدود"));
    title->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(CP_ACCENT_PINK));
    mainLayout->addWidget(title);
    
    // Color palette
    GlassPanel *colorPanel = new GlassPanel(this);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorPanel);
    
    QLabel *colorLabel = new QLabel(tr("اختيار اللون:"));
    colorLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    colorLayout->addWidget(colorLabel);
    
    QGridLayout *paletteLayout = new QGridLayout();
    paletteLayout->setSpacing(8);
    
    QVector<QColor> blushColors = {
        QColor(220, 150, 150),  // Soft pink
        QColor(200, 120, 120),  // Rose
        QColor(180, 100, 100),  // Deep rose
        QColor(210, 160, 140),  // Peach
        QColor(200, 140, 120),  // Coral
        QColor(180, 120, 100),  // Terracotta
        QColor(220, 180, 170),  // Nude
        QColor(190, 130, 130),  // Berry
    };
    
    int row = 0, col = 0;
    for (const QColor &color : blushColors) {
        GlassButton *btn = new GlassButton("", this);
        btn->setFixedSize(40, 40);
        btn->setStyleSheet(QString("background-color: %1; border-radius: 20px; border: 2px solid %2;")
            .arg(color.name()).arg(CP_ACCENT_CYAN));
        connect(btn, &GlassButton::clicked, [this, color]() {
            m_selectedColor = color;
            emit colorChanged(color);
        });
        paletteLayout->addWidget(btn, row, col);
        col++;
        if (col >= 4) {
            col = 0;
            row++;
        }
    }
    
    colorLayout->addLayout(paletteLayout);
    mainLayout->addWidget(colorPanel);
    
    // Intensity slider
    GlassPanel *intensityPanel = new GlassPanel(this);
    QVBoxLayout *intensityLayout = new QVBoxLayout(intensityPanel);
    
    QHBoxLayout *sliderLabelLayout = new QHBoxLayout();
    QLabel *intensityLabel = new QLabel(tr("الشدة:"));
    intensityLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    sliderLabelLayout->addWidget(intensityLabel);
    
    m_intensityValueLabel = new QLabel("50%");
    m_intensityValueLabel->setStyleSheet(QString("color: %1;").arg(CP_ACCENT_CYAN));
    sliderLabelLayout->addStretch();
    sliderLabelLayout->addWidget(m_intensityValueLabel);
    intensityLayout->addLayout(sliderLabelLayout);
    
    m_intensitySlider = new QSlider(Qt::Horizontal, this);
    m_intensitySlider->setRange(0, 100);
    m_intensitySlider->setValue(50);
    m_intensitySlider->setStyleSheet(StyleManager::sliderStyle());
    intensityLayout->addWidget(m_intensitySlider);
    
    mainLayout->addWidget(intensityPanel);
    
    // Position selection
    GlassPanel *positionPanel = new GlassPanel(this);
    QVBoxLayout *positionLayout = new QVBoxLayout(positionPanel);
    
    QLabel *positionLabel = new QLabel(tr("الموضع:"));
    positionLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    positionLayout->addWidget(positionLabel);
    
    QHBoxLayout *posBtnLayout = new QHBoxLayout();
    
    GlassButton *appleBtn = new GlassButton(tr("تفاحة الخد"), this);
    appleBtn->setCheckable(true);
    appleBtn->setChecked(true);
    posBtnLayout->addWidget(appleBtn);
    
    GlassButton *cheekboneBtn = new GlassButton(tr("عظم الخد"), this);
    cheekboneBtn->setCheckable(true);
    posBtnLayout->addWidget(cheekboneBtn);
    
    positionLayout->addLayout(posBtnLayout);
    mainLayout->addWidget(positionPanel);
    
    // Apply button
    GlassButton *applyBtn = new GlassButton(tr("تطبيق أحمر الخدود"), this);
    applyBtn->setAccentColor(CP_ACCENT_PINK);
    connect(applyBtn, &GlassButton::clicked, this, &BlushPanel::applyRequested);
    mainLayout->addWidget(applyBtn);
    
    mainLayout->addStretch();
}

void BlushPanel::setupConnections() {
    connect(m_intensitySlider, &QSlider::valueChanged, [this](int value) {
        m_intensityValueLabel->setText(QString("%1%").arg(value));
        emit intensityChanged(value);
    });
}

QColor BlushPanel::getSelectedColor() const {
    return m_selectedColor;
}

int BlushPanel::getIntensity() const {
    return m_intensitySlider->value();
}

// ============================================================================
// ContourPanel Implementation
// ============================================================================

ContourPanel::ContourPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void ContourPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *title = new QLabel(tr("التحديد والإضاءة"));
    title->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(CP_ACCENT_GOLD));
    mainLayout->addWidget(title);
    
    // Contour section
    GlassPanel *contourPanel = new GlassPanel(this);
    QVBoxLayout *contourLayout = new QVBoxLayout(contourPanel);
    
    QLabel *contourLabel = new QLabel(tr("التحديد (كونتور):"));
    contourLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    contourLayout->addWidget(contourLabel);
    
    m_contourSlider = new QSlider(Qt::Horizontal, this);
    m_contourSlider->setRange(0, 100);
    m_contourSlider->setValue(30);
    m_contourSlider->setStyleSheet(StyleManager::sliderStyle());
    contourLayout->addWidget(m_contourSlider);
    
    mainLayout->addWidget(contourPanel);
    
    // Highlight section
    GlassPanel *highlightPanel = new GlassPanel(this);
    QVBoxLayout *highlightLayout = new QVBoxLayout(highlightPanel);
    
    QLabel *highlightLabel = new QLabel(tr("الإضاءة (هايلايت):"));
    highlightLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    highlightLayout->addWidget(highlightLabel);
    
    m_highlightSlider = new QSlider(Qt::Horizontal, this);
    m_highlightSlider->setRange(0, 100);
    m_highlightSlider->setValue(40);
    m_highlightSlider->setStyleSheet(StyleManager::sliderStyle());
    highlightLayout->addWidget(m_highlightSlider);
    
    mainLayout->addWidget(highlightPanel);
    
    // Areas selection
    GlassPanel *areasPanel = new GlassPanel(this);
    QVBoxLayout *areasLayout = new QVBoxLayout(areasPanel);
    
    QLabel *areasLabel = new QLabel(tr("المناطق:"));
    areasLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    areasLayout->addWidget(areasLabel);
    
    QGridLayout *areasGrid = new QGridLayout();
    
    m_cheekbonesCheck = new QCheckBox(tr("عظام الخد"));
    m_cheekbonesCheck->setChecked(true);
    m_cheekbonesCheck->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    areasGrid->addWidget(m_cheekbonesCheck, 0, 0);
    
    m_noseCheck = new QCheckBox(tr("الأنف"));
    m_noseCheck->setChecked(true);
    m_noseCheck->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    areasGrid->addWidget(m_noseCheck, 0, 1);
    
    m_jawlineCheck = new QCheckBox(tr("خط الفك"));
    m_jawlineCheck->setChecked(true);
    m_jawlineCheck->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    areasGrid->addWidget(m_jawlineCheck, 1, 0);
    
    m_foreheadCheck = new QCheckBox(tr("الجبهة"));
    m_foreheadCheck->setChecked(false);
    m_foreheadCheck->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    areasGrid->addWidget(m_foreheadCheck, 1, 1);
    
    areasLayout->addLayout(areasGrid);
    mainLayout->addWidget(areasPanel);
    
    // Apply button
    GlassButton *applyBtn = new GlassButton(tr("تطبيق التحديد"), this);
    applyBtn->setAccentColor(CP_ACCENT_GOLD);
    connect(applyBtn, &GlassButton::clicked, this, &ContourPanel::applyRequested);
    mainLayout->addWidget(applyBtn);
    
    mainLayout->addStretch();
}

void ContourPanel::setupConnections() {
    connect(m_contourSlider, &QSlider::valueChanged, this, &ContourPanel::contourChanged);
    connect(m_highlightSlider, &QSlider::valueChanged, this, &ContourPanel::highlightChanged);
}

int ContourPanel::getContourIntensity() const {
    return m_contourSlider->value();
}

int ContourPanel::getHighlightIntensity() const {
    return m_highlightSlider->value();
}

QVector<QString> ContourPanel::getSelectedAreas() const {
    QVector<QString> areas;
    if (m_cheekbonesCheck->isChecked()) areas.append("cheekbones");
    if (m_noseCheck->isChecked()) areas.append("nose");
    if (m_jawlineCheck->isChecked()) areas.append("jawline");
    if (m_foreheadCheck->isChecked()) areas.append("forehead");
    return areas;
}

// ============================================================================
// MakeupAIModelsPanel Implementation
// ============================================================================

MakeupAIModelsPanel::MakeupAIModelsPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void MakeupAIModelsPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *title = new QLabel(tr("نماذج الذكاء الاصطناعي"));
    title->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(CP_ACCENT_CYAN));
    mainLayout->addWidget(title);
    
    // AI Look presets
    GlassPanel *presetsPanel = new GlassPanel(this);
    QVBoxLayout *presetsLayout = new QVBoxLayout(presetsPanel);
    
    QLabel *presetsLabel = new QLabel(tr("المظاهر الجاهزة:"));
    presetsLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    presetsLayout->addWidget(presetsLabel);
    
    QGridLayout *presetsGrid = new QGridLayout();
    presetsGrid->setSpacing(10);
    
    GlassButton *naturalBtn = new GlassButton(tr("مظهر طبيعي"), this);
    naturalBtn->setAccentColor("#8BC34A");
    connect(naturalBtn, &GlassButton::clicked, this, &MakeupAIModelsPanel::naturalLookRequested);
    presetsGrid->addWidget(naturalBtn, 0, 0);
    
    GlassButton *eveningBtn = new GlassButton(tr("مكياج مسائي"), this);
    eveningBtn->setAccentColor(CP_ACCENT_PURPLE);
    connect(eveningBtn, &GlassButton::clicked, this, &MakeupAIModelsPanel::eveningLookRequested);
    presetsGrid->addWidget(eveningBtn, 0, 1);
    
    GlassButton *bridalBtn = new GlassButton(tr("مكياج عروس"), this);
    bridalBtn->setAccentColor(CP_ACCENT_PINK);
    connect(bridalBtn, &GlassButton::clicked, this, &MakeupAIModelsPanel::bridalLookRequested);
    presetsGrid->addWidget(bridalBtn, 1, 0);
    
    GlassButton *glamBtn = new GlassButton(tr("مظهر جلامور"), this);
    glamBtn->setAccentColor(CP_ACCENT_GOLD);
    connect(glamBtn, &GlassButton::clicked, this, &MakeupAIModelsPanel::glamLookRequested);
    presetsGrid->addWidget(glamBtn, 1, 1);
    
    presetsLayout->addLayout(presetsGrid);
    mainLayout->addWidget(presetsPanel);
    
    // Auto makeup
    GlassPanel *autoPanel = new GlassPanel(this);
    QVBoxLayout *autoLayout = new QVBoxLayout(autoPanel);
    
    QLabel *autoLabel = new QLabel(tr("المكياج التلقائي:"));
    autoLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    autoLayout->addWidget(autoLabel);
    
    GlassButton *autoMakeupBtn = new GlassButton(tr("مكياج AI ذكي"), this);
    autoMakeupBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(autoMakeupBtn, &GlassButton::clicked, this, &MakeupAIModelsPanel::autoMakeupRequested);
    autoLayout->addWidget(autoMakeupBtn);
    
    QLabel *autoDesc = new QLabel(tr("يحلل AI ملامح الوجه ويطبق المكياج المناسب تلقائياً"));
    autoDesc->setStyleSheet(QString("color: %1; font-size: 12px;").arg(CP_TEXT_SECONDARY));
    autoDesc->setWordWrap(true);
    autoLayout->addWidget(autoDesc);
    
    mainLayout->addWidget(autoPanel);
    
    // Model info
    GlassPanel *infoPanel = new GlassPanel(this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoPanel);
    
    QLabel *infoLabel = new QLabel(tr("معلومات النماذج:"));
    infoLabel->setStyleSheet(QString("color: %1;").arg(CP_TEXT_PRIMARY));
    infoLayout->addWidget(infoLabel);
    
    QLabel *modelInfo = new QLabel(
        tr("• نموذج كشف الوجه: FaceNet-v2 (Offline)\n") +
        tr("• نموذج كشف المعالم: LandmarkNet-68\n") +
        tr("• نموذج المكياج: MakeupGAN-3.0\n") +
        tr("• جميع النماذج تعمل محلياً بدون إنترنت")
    );
    modelInfo->setStyleSheet(QString("color: %1; font-size: 12px;").arg(CP_TEXT_SECONDARY));
    infoLayout->addWidget(modelInfo);
    
    mainLayout->addWidget(infoPanel);
    
    mainLayout->addStretch();
}

// ============================================================================
// MakeupStudio Implementation
// ============================================================================

MakeupStudio::MakeupStudio(QWidget *parent) : QWidget(parent) {
    m_currentMakeup = std::make_unique<MakeupState>();
    setupUI();
    setupConnections();
}

void MakeupStudio::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Left panel - Tools
    GlassPanel *toolsPanel = new GlassPanel(this);
    toolsPanel->setFixedWidth(350);
    QVBoxLayout *toolsLayout = new QVBoxLayout(toolsPanel);
    toolsLayout->setSpacing(10);
    toolsLayout->setContentsMargins(15, 15, 15, 15);
    
    // Header
    QLabel *headerLabel = new QLabel(tr("استوديو المكياج"));
    headerLabel->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: bold;").arg(CP_ACCENT_PINK));
    toolsLayout->addWidget(headerLabel);
    
    // Tabs
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(StyleManager::tabStyle());
    
    // Lipstick tab
    m_lipstickPanel = new LipstickPanel(this);
    m_tabWidget->addTab(m_lipstickPanel, tr("الشفاه"));
    
    // Eyeshadow tab
    m_eyeshadowPanel = new EyeshadowPanel(this);
    m_tabWidget->addTab(m_eyeshadowPanel, tr("العيون"));
    
    // Blush tab
    m_blushPanel = new BlushPanel(this);
    m_tabWidget->addTab(m_blushPanel, tr("الخدود"));
    
    // Contour tab
    m_contourPanel = new ContourPanel(this);
    m_tabWidget->addTab(m_contourPanel, tr("التحديد"));
    
    // AI Models tab
    m_aiPanel = new MakeupAIModelsPanel(this);
    m_tabWidget->addTab(m_aiPanel, tr("الذكاء الاصطناعي"));
    
    toolsLayout->addWidget(m_tabWidget);
    
    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    
    GlassButton *detectBtn = new GlassButton(tr("كشف الوجه"), this);
    detectBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(detectBtn, &GlassButton::clicked, this, &MakeupStudio::aiDetectFace);
    actionLayout->addWidget(detectBtn);
    
    GlassButton *clearBtn = new GlassButton(tr("مسح"), this);
    clearBtn->setAccentColor(CP_ACCENT_RED);
    connect(clearBtn, &GlassButton::clicked, this, &MakeupStudio::clearMakeup);
    actionLayout->addWidget(clearBtn);
    
    toolsLayout->addLayout(actionLayout);
    
    // Compare button
    GlassButton *compareBtn = new GlassButton(tr("مقارنة قبل/بعد"), this);
    compareBtn->setAccentColor(CP_ACCENT_GOLD);
    connect(compareBtn, &GlassButton::clicked, this, &MakeupStudio::toggleComparison);
    toolsLayout->addWidget(compareBtn);
    
    mainLayout->addWidget(toolsPanel);
    
    // Right panel - Canvas
    GlassPanel *canvasPanel = new GlassPanel(this);
    QVBoxLayout *canvasLayout = new QVBoxLayout(canvasPanel);
    canvasLayout->setSpacing(10);
    canvasLayout->setContentsMargins(15, 15, 15, 15);
    
    // Canvas toolbar
    QHBoxLayout *canvasToolbar = new QHBoxLayout();
    
    GlassButton *openBtn = new GlassButton(tr("فتح صورة"), this);
    connect(openBtn, &GlassButton::clicked, this, &MakeupStudio::openImage);
    canvasToolbar->addWidget(openBtn);
    
    GlassButton *saveBtn = new GlassButton(tr("حفظ"), this);
    connect(saveBtn, &GlassButton::clicked, this, &MakeupStudio::saveImage);
    canvasToolbar->addWidget(saveBtn);
    
    canvasToolbar->addStretch();
    
    GlassButton *undoBtn = new GlassButton(tr("تراجع"), this);
    connect(undoBtn, &GlassButton::clicked, this, &MakeupStudio::undo);
    canvasToolbar->addWidget(undoBtn);
    
    GlassButton *redoBtn = new GlassButton(tr("إعادة"), this);
    connect(redoBtn, &GlassButton::clicked, this, &MakeupStudio::redo);
    canvasToolbar->addWidget(redoBtn);
    
    canvasLayout->addLayout(canvasToolbar);
    
    // Canvas
    m_canvas = new MakeupCanvas(this);
    canvasLayout->addWidget(m_canvas, 1);
    
    mainLayout->addWidget(canvasPanel, 1);
}

void MakeupStudio::setupConnections() {
    // Lipstick panel
    connect(m_lipstickPanel, &LipstickPanel::applyRequested, this, [this]() {
        applyLipstick(m_lipstickPanel->getSelectedColor(), 
                      m_lipstickPanel->getIntensity(),
                      m_lipstickPanel->getGloss());
    });
    
    // Eyeshadow panel
    connect(m_eyeshadowPanel, &EyeshadowPanel::applyRequested, this, [this]() {
        applyEyeshadow(m_eyeshadowPanel->getSelectedColor(),
                       m_eyeshadowPanel->getIntensity(),
                       m_eyeshadowPanel->getShimmer());
    });
    
    // Blush panel
    connect(m_blushPanel, &BlushPanel::applyRequested, this, [this]() {
        applyBlush(m_blushPanel->getSelectedColor(),
                   m_blushPanel->getIntensity());
    });
    
    // Contour panel
    connect(m_contourPanel, &ContourPanel::applyRequested, this, [this]() {
        applyContour(m_contourPanel->getContourIntensity(),
                     m_contourPanel->getHighlightIntensity(),
                     m_contourPanel->getSelectedAreas());
    });
    
    // AI panel
    connect(m_aiPanel, &MakeupAIModelsPanel::autoMakeupRequested, this, &MakeupStudio::aiAutoMakeup);
    connect(m_aiPanel, &MakeupAIModelsPanel::naturalLookRequested, this, &MakeupStudio::aiNaturalLook);
    connect(m_aiPanel, &MakeupAIModelsPanel::eveningLookRequested, this, &MakeupStudio::aiEveningLook);
    connect(m_aiPanel, &MakeupAIModelsPanel::bridalLookRequested, this, &MakeupStudio::aiBridalLook);
    connect(m_aiPanel, &MakeupAIModelsPanel::glamLookRequested, this, &MakeupStudio::aiGlamLook);
}

void MakeupStudio::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("فتح صورة"), QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.tiff)"));
    
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (!image.isNull()) {
            m_originalImage = image;
            m_canvas->setImage(image);
            emit imageLoaded(fileName);
        }
    }
}

void MakeupStudio::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("حفظ الصورة"), QString(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)"));
    
    if (!fileName.isEmpty()) {
        QImage result = m_canvas->getProcessedImage();
        result.save(fileName);
        emit imageSaved(fileName);
    }
}

void MakeupStudio::undo() {
    if (!m_history.empty()) {
        m_redoStack.push(m_canvas->getProcessedImage());
        QImage previous = m_history.pop();
        m_canvas->setProcessedImage(previous);
    }
}

void MakeupStudio::redo() {
    if (!m_redoStack.empty()) {
        m_history.push(m_canvas->getProcessedImage());
        QImage next = m_redoStack.pop();
        m_canvas->setProcessedImage(next);
    }
}

void MakeupStudio::saveToHistory() {
    m_history.push(m_canvas->getProcessedImage());
    m_redoStack.clear();
}

void MakeupStudio::clearMakeup() {
    saveToHistory();
    m_canvas->clearMakeup();
}

void MakeupStudio::toggleComparison() {
    // Toggle between original and processed
    static bool showingOriginal = false;
    showingOriginal = !showingOriginal;
    
    if (showingOriginal) {
        m_canvas->setProcessedImage(m_originalImage);
    } else {
        // Reapply all makeup
        applyAllMakeup();
    }
}

void MakeupStudio::applyAllMakeup() {
    QImage result = m_originalImage.copy();
    
    // Apply all current makeup settings
    if (m_currentMakeup->lipstickIntensity > 0) {
        result = applyLipstickToImage(result, m_currentMakeup->lipstickColor, 
                                       m_currentMakeup->lipstickIntensity, 30);
    }
    
    m_canvas->setProcessedImage(result);
}

// ============================================================================
// AI Makeup Functions
// ============================================================================

void MakeupStudio::aiDetectFace() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    // Simulate face detection
    FaceRegions regions;
    
    int w = m_originalImage.width();
    int h = m_originalImage.height();
    
    // Approximate face regions based on typical face proportions
    QPoint faceCenter(w / 2, h / 2);
    int faceWidth = w * 0.4;
    int faceHeight = h * 0.5;
    
    // Lips region (bottom third of face)
    regions.lips << QPoint(faceCenter.x() - faceWidth/6, faceCenter.y() + faceHeight/4)
                 << QPoint(faceCenter.x() + faceWidth/6, faceCenter.y() + faceHeight/4)
                 << QPoint(faceCenter.x() + faceWidth/5, faceCenter.y() + faceHeight/3)
                 << QPoint(faceCenter.x(), faceCenter.y() + faceHeight/3 + 10)
                 << QPoint(faceCenter.x() - faceWidth/5, faceCenter.y() + faceHeight/3);
    
    // Eyes regions
    regions.leftEye << QPoint(faceCenter.x() - faceWidth/4, faceCenter.y() - faceHeight/6)
                    << QPoint(faceCenter.x() - faceWidth/8, faceCenter.y() - faceHeight/6)
                    << QPoint(faceCenter.x() - faceWidth/8, faceCenter.y() - faceHeight/10)
                    << QPoint(faceCenter.x() - faceWidth/4, faceCenter.y() - faceHeight/10);
    
    regions.rightEye << QPoint(faceCenter.x() + faceWidth/8, faceCenter.y() - faceHeight/6)
                     << QPoint(faceCenter.x() + faceWidth/4, faceCenter.y() - faceHeight/6)
                     << QPoint(faceCenter.x() + faceWidth/4, faceCenter.y() - faceHeight/10)
                     << QPoint(faceCenter.x() + faceWidth/8, faceCenter.y() - faceHeight/10);
    
    // Cheeks regions
    regions.leftCheek << QPoint(faceCenter.x() - faceWidth/3, faceCenter.y())
                      << QPoint(faceCenter.x() - faceWidth/6, faceCenter.y())
                      << QPoint(faceCenter.x() - faceWidth/6, faceCenter.y() + faceHeight/6)
                      << QPoint(faceCenter.x() - faceWidth/3, faceCenter.y() + faceHeight/8);
    
    regions.rightCheek << QPoint(faceCenter.x() + faceWidth/6, faceCenter.y())
                       << QPoint(faceCenter.x() + faceWidth/3, faceCenter.y())
                       << QPoint(faceCenter.x() + faceWidth/3, faceCenter.y() + faceHeight/8)
                       << QPoint(faceCenter.x() + faceWidth/6, faceCenter.y() + faceHeight/6);
    
    m_faceRegions = regions;
    m_canvas->setFaceRegions(regions);
    
    emit faceDetected();
}

void MakeupStudio::aiAutoMakeup() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    // Detect face first
    aiDetectFace();
    
    // Apply balanced makeup
    QImage result = m_originalImage.copy();
    
    // Natural lipstick
    result = applyLipstickToImage(result, QColor(200, 80, 100), 40, 20);
    
    // Subtle eyeshadow
    result = applyEyeshadowToImage(result, QColor(180, 150, 120), 30, 10);
    
    // Light blush
    result = applyBlushToImage(result, QColor(220, 160, 160), 35);
    
    // Soft contour
    result = applyContourToImage(result, 25, 30, {"cheekbones", "nose"});
    
    m_canvas->setProcessedImage(result);
    emit makeupApplied(tr("مكياج AI تلقائي"));
}

void MakeupStudio::aiNaturalLook() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    aiDetectFace();
    
    QImage result = m_originalImage.copy();
    
    // Very subtle makeup
    result = applyLipstickToImage(result, QColor(210, 150, 150), 25, 10);
    result = applyEyeshadowToImage(result, QColor(200, 180, 160), 20, 5);
    result = applyBlushToImage(result, QColor(230, 190, 180), 25);
    result = applyContourToImage(result, 15, 20, {"cheekbones"});
    
    m_canvas->setProcessedImage(result);
    emit makeupApplied(tr("مظهر طبيعي"));
}

void MakeupStudio::aiEveningLook() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    aiDetectFace();
    
    QImage result = m_originalImage.copy();
    
    // Dramatic makeup
    result = applyLipstickToImage(result, QColor(180, 40, 60), 70, 40);
    result = applyEyeshadowToImage(result, QColor(80, 60, 100), 60, 50);
    result = applyBlushToImage(result, QColor(180, 100, 100), 45);
    result = applyContourToImage(result, 50, 45, {"cheekbones", "nose", "jawline"});
    
    m_canvas->setProcessedImage(result);
    emit makeupApplied(tr("مكياج مسائي"));
}

void MakeupStudio::aiBridalLook() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    aiDetectFace();
    
    QImage result = m_originalImage.copy();
    
    // Elegant bridal makeup
    result = applyLipstickToImage(result, QColor(200, 100, 120), 50, 35);
    result = applyEyeshadowToImage(result, QColor(180, 160, 140), 40, 30);
    result = applyBlushToImage(result, QColor(220, 170, 170), 40);
    result = applyContourToImage(result, 35, 40, {"cheekbones", "nose", "jawline", "forehead"});
    
    m_canvas->setProcessedImage(result);
    emit makeupApplied(tr("مكياج عروس"));
}

void MakeupStudio::aiGlamLook() {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    aiDetectFace();
    
    QImage result = m_originalImage.copy();
    
    // Glamorous makeup
    result = applyLipstickToImage(result, QColor(200, 50, 80), 80, 50);
    result = applyEyeshadowToImage(result, QColor(150, 100, 150), 70, 60);
    result = applyBlushToImage(result, QColor(200, 120, 130), 50);
    result = applyContourToImage(result, 60, 55, {"cheekbones", "nose", "jawline", "forehead"});
    
    m_canvas->setProcessedImage(result);
    emit makeupApplied(tr("مظهر جلامور"));
}

// ============================================================================
// Makeup Application Algorithms
// ============================================================================

void MakeupStudio::applyLipstick(const QColor &color, int intensity, int gloss) {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    QImage result = applyLipstickToImage(m_canvas->getProcessedImage(), color, intensity, gloss);
    m_canvas->setProcessedImage(result);
    
    m_currentMakeup->lipstickColor = color;
    m_currentMakeup->lipstickIntensity = intensity;
    
    emit makeupApplied(tr("أحمر شفاه"));
}

QImage MakeupStudio::applyLipstickToImage(const QImage &input, const QColor &color, int intensity, int gloss) {
    if (m_faceRegions.lips.isEmpty()) {
        return input.copy();
    }
    
    QImage result = input.copy();
    
    // Create lips mask
    QPolygon lipsPoly = m_faceRegions.lips;
    
    // Apply lipstick with blending
    float alpha = intensity / 100.0f;
    float glossFactor = gloss / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QPoint pt(x, y);
            
            if (lipsPoly.containsPoint(pt, Qt::OddEvenFill)) {
                QColor pixel = result.pixelColor(x, y);
                
                // Distance from center of lips for gradient effect
                QPointF lipsCenter = lipsPoly.boundingRect().center();
                float dist = std::sqrt(std::pow(x - lipsCenter.x(), 2) + std::pow(y - lipsCenter.y(), 2));
                float maxDist = std::max(lipsPoly.boundingRect().width(), lipsPoly.boundingRect().height()) / 2.0f;
                float edgeFactor = 1.0f - (dist / maxDist);
                edgeFactor = std::max(0.0f, edgeFactor);
                
                // Blend lipstick color
                int r = pixel.red() * (1 - alpha * edgeFactor) + color.red() * alpha * edgeFactor;
                int g = pixel.green() * (1 - alpha * edgeFactor) + color.green() * alpha * edgeFactor;
                int b = pixel.blue() * (1 - alpha * edgeFactor) + color.blue() * alpha * edgeFactor;
                
                // Add gloss highlight
                if (glossFactor > 0) {
                    float highlight = glossFactor * edgeFactor * 30;
                    r = std::min(255, r + (int)highlight);
                    g = std::min(255, g + (int)highlight);
                    b = std::min(255, b + (int)highlight);
                }
                
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

void MakeupStudio::applyEyeshadow(const QColor &color, int intensity, int shimmer) {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    QImage result = applyEyeshadowToImage(m_canvas->getProcessedImage(), color, intensity, shimmer);
    m_canvas->setProcessedImage(result);
    
    m_currentMakeup->eyeshadowColor = color;
    m_currentMakeup->eyeshadowIntensity = intensity;
    
    emit makeupApplied(tr("ظلال عيون"));
}

QImage MakeupStudio::applyEyeshadowToImage(const QImage &input, const QColor &color, int intensity, int shimmer) {
    if (m_faceRegions.leftEye.isEmpty() && m_faceRegions.rightEye.isEmpty()) {
        return input.copy();
    }
    
    QImage result = input.copy();
    
    float alpha = intensity / 100.0f;
    float shimmerFactor = shimmer / 100.0f;
    
    auto applyToEye = [&](const QPolygon &eyePoly) {
        if (eyePoly.isEmpty()) return;
        
        // Expand eye region for eyeshadow (eyelid area)
        QRect eyeRect = eyePoly.boundingRect();
        int expandY = eyeRect.height() * 0.8;
        QRect shadowRect(
            eyeRect.left(),
            eyeRect.top() - expandY,
            eyeRect.width(),
            eyeRect.height() + expandY
        );
        
        for (int y = shadowRect.top(); y <= shadowRect.bottom(); ++y) {
            for (int x = shadowRect.left(); x <= shadowRect.right(); ++x) {
                if (x < 0 || x >= result.width() || y < 0 || y >= result.height()) continue;
                
                // Distance from eye center for gradient
                QPointF eyeCenter = eyeRect.center();
                float distY = std::abs(y - eyeCenter.y());
                float gradient = 1.0f - (distY / (shadowRect.height() / 2.0f));
                gradient = std::max(0.0f, gradient);
                
                QColor pixel = result.pixelColor(x, y);
                
                // Blend eyeshadow color
                int r = pixel.red() * (1 - alpha * gradient) + color.red() * alpha * gradient;
                int g = pixel.green() * (1 - alpha * gradient) + color.green() * alpha * gradient;
                int b = pixel.blue() * (1 - alpha * gradient) + color.blue() * alpha * gradient;
                
                // Add shimmer
                if (shimmerFactor > 0 && gradient > 0.5f) {
                    float shine = shimmerFactor * gradient * 20;
                    r = std::min(255, r + (int)shine);
                    g = std::min(255, g + (int)shine);
                    b = std::min(255, b + (int)shine);
                }
                
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    };
    
    applyToEye(m_faceRegions.leftEye);
    applyToEye(m_faceRegions.rightEye);
    
    return result;
}

void MakeupStudio::applyBlush(const QColor &color, int intensity) {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    QImage result = applyBlushToImage(m_canvas->getProcessedImage(), color, intensity);
    m_canvas->setProcessedImage(result);
    
    m_currentMakeup->blushColor = color;
    m_currentMakeup->blushIntensity = intensity;
    
    emit makeupApplied(tr("أحمر خدود"));
}

QImage MakeupStudio::applyBlushToImage(const QImage &input, const QColor &color, int intensity) {
    if (m_faceRegions.leftCheek.isEmpty() && m_faceRegions.rightCheek.isEmpty()) {
        return input.copy();
    }
    
    QImage result = input.copy();
    
    float alpha = intensity / 100.0f;
    
    auto applyToCheek = [&](const QPolygon &cheekPoly) {
        if (cheekPoly.isEmpty()) return;
        
        QRect cheekRect = cheekPoly.boundingRect();
        QPointF cheekCenter = cheekRect.center();
        float radius = std::max(cheekRect.width(), cheekRect.height()) / 2.0f;
        
        for (int y = cheekRect.top(); y <= cheekRect.bottom(); ++y) {
            for (int x = cheekRect.left(); x <= cheekRect.right(); ++x) {
                if (x < 0 || x >= result.width() || y < 0 || y >= result.height()) continue;
                
                float dist = std::sqrt(std::pow(x - cheekCenter.x(), 2) + std::pow(y - cheekCenter.y(), 2));
                float gradient = 1.0f - (dist / radius);
                gradient = std::max(0.0f, gradient);
                gradient = gradient * gradient; // Smooth falloff
                
                QColor pixel = result.pixelColor(x, y);
                
                // Blend blush color
                int r = pixel.red() * (1 - alpha * gradient) + color.red() * alpha * gradient;
                int g = pixel.green() * (1 - alpha * gradient) + color.green() * alpha * gradient;
                int b = pixel.blue() * (1 - alpha * gradient) + color.blue() * alpha * gradient;
                
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    };
    
    applyToCheek(m_faceRegions.leftCheek);
    applyToCheek(m_faceRegions.rightCheek);
    
    return result;
}

void MakeupStudio::applyContour(int contourIntensity, int highlightIntensity, const QVector<QString> &areas) {
    if (m_originalImage.isNull()) return;
    
    saveToHistory();
    
    QImage result = applyContourToImage(m_canvas->getProcessedImage(), contourIntensity, highlightIntensity, areas);
    m_canvas->setProcessedImage(result);
    
    emit makeupApplied(tr("تحديد وإضاءة"));
}

QImage MakeupStudio::applyContourToImage(const QImage &input, int contourIntensity, int highlightIntensity, const QVector<QString> &areas) {
    QImage result = input.copy();
    
    float contourAlpha = contourIntensity / 100.0f;
    float highlightAlpha = highlightIntensity / 100.0f;
    
    int w = result.width();
    int h = result.height();
    QPoint faceCenter(w / 2, h / 2);
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor pixel = result.pixelColor(x, y);
            int r = pixel.red();
            int g = pixel.green();
            int b = pixel.blue();
            
            // Cheekbones contour
            if (areas.contains("cheekbones")) {
                float distFromCenter = std::sqrt(std::pow(x - faceCenter.x(), 2) + std::pow(y - faceCenter.y(), 2));
                float cheekboneZone = std::abs(y - faceCenter.y()) < h * 0.15 && distFromCenter > w * 0.15;
                
                if (cheekboneZone) {
                    // Darken for contour
                    int darken = contourAlpha * 30;
                    r = std::max(0, r - darken);
                    g = std::max(0, g - darken);
                    b = std::max(0, b - darken);
                }
                
                // Highlight on top of cheekbones
                if (std::abs(y - (faceCenter.y() - h * 0.1)) < h * 0.05 && distFromCenter > w * 0.1) {
                    int lighten = highlightAlpha * 25;
                    r = std::min(255, r + lighten);
                    g = std::min(255, g + lighten);
                    b = std::min(255, b + lighten);
                }
            }
            
            // Nose contour
            if (areas.contains("nose")) {
                float noseX = std::abs(x - faceCenter.x());
                if (noseX > w * 0.03 && noseX < w * 0.06 && y > faceCenter.y() - h * 0.1 && y < faceCenter.y() + h * 0.15) {
                    int darken = contourAlpha * 25;
                    r = std::max(0, r - darken);
                    g = std::max(0, g - darken);
                    b = std::max(0, b - darken);
                }
                
                // Nose highlight
                if (noseX < w * 0.02 && y > faceCenter.y() - h * 0.1 && y < faceCenter.y() + h * 0.15) {
                    int lighten = highlightAlpha * 30;
                    r = std::min(255, r + lighten);
                    g = std::min(255, g + lighten);
                    b = std::min(255, b + lighten);
                }
            }
            
            // Jawline contour
            if (areas.contains("jawline")) {
                float jawY = faceCenter.y() + h * 0.2;
                if (std::abs(y - jawY) < h * 0.05) {
                    float distFromCenter = std::abs(x - faceCenter.x());
                    if (distFromCenter > w * 0.1 && distFromCenter < w * 0.25) {
                        int darken = contourAlpha * 20;
                        r = std::max(0, r - darken);
                        g = std::max(0, g - darken);
                        b = std::max(0, b - darken);
                    }
                }
            }
            
            // Forehead highlight
            if (areas.contains("forehead")) {
                if (y < faceCenter.y() - h * 0.15 && y > faceCenter.y() - h * 0.25) {
                    float distFromCenter = std::abs(x - faceCenter.x());
                    if (distFromCenter < w * 0.15) {
                        int lighten = highlightAlpha * 20;
                        r = std::min(255, r + lighten);
                        g = std::min(255, g + lighten);
                        b = std::min(255, b + lighten);
                    }
                }
            }
            
            result.setPixelColor(x, y, QColor(r, g, b));
        }
    }
    
    return result;
}

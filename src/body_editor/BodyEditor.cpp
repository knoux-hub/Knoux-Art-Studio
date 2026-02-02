#include "BodyEditor.h"
#include "../ui/GlassButton.h"
#include "../ui/GlassPanel.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QKeyEvent>
#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QtMath>
#include <QPropertyAnimation>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)
#define CP_ACCENT_GREEN QColor(0, 255, 128)
#define CP_ACCENT_ORANGE QColor(255, 128, 0)

BodyEditor::BodyEditor(QWidget *parent)
    : QWidget(parent)
    , m_canvas(nullptr)
    , m_adjustmentsPanel(nullptr)
    , m_presetsPanel(nullptr)
    , m_aiPanel(nullptr)
    , m_isModified(false)
    , m_bodyDetected(false)
    , m_brushSize(50)
    , m_brushIntensity(50)
    , m_isDrawing(false)
    , m_processingProgress(0.0f)
    , m_isProcessing(false)
    , m_previewTimer(nullptr)
{
    setupUI();
    setupConnections();
    
    // Initialize default params
    m_defaultParams = m_params;
    
    // Setup preview timer
    m_previewTimer = new QTimer(this);
    m_previewTimer->setSingleShot(true);
    m_previewTimer->setInterval(100);
    connect(m_previewTimer, &QTimer::timeout, this, &BodyEditor::applyAdjustments);
}

BodyEditor::~BodyEditor()
{
}

void BodyEditor::setupUI()
{
    setObjectName("bodyEditor");
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Left toolbar
    createToolbar();
    mainLayout->addWidget(m_leftToolbar, 0);
    
    // Center area
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    
    // Top toolbar
    m_topToolbar = new GlassPanel(this);
    m_topToolbar->setFixedHeight(60);
    m_topToolbar->setGlassOpacity(0.1);
    m_topToolbar->setAccentColor(CP_ACCENT_CYAN);
    
    QHBoxLayout *topLayout = new QHBoxLayout(m_topToolbar);
    topLayout->setContentsMargins(15, 10, 15, 10);
    topLayout->setSpacing(10);
    
    m_undoBtn = new GlassButton("↩", m_topToolbar);
    m_undoBtn->setFixedSize(40, 40);
    m_undoBtn->setToolTip(tr("تراجع"));
    topLayout->addWidget(m_undoBtn);
    
    m_redoBtn = new GlassButton("↪", m_topToolbar);
    m_redoBtn->setFixedSize(40, 40);
    m_redoBtn->setToolTip(tr("إعادة"));
    topLayout->addWidget(m_redoBtn);
    
    topLayout->addSpacing(20);
    
    m_saveBtn = new GlassButton("💾", m_topToolbar);
    m_saveBtn->setFixedSize(40, 40);
    m_saveBtn->setToolTip(tr("حفظ"));
    topLayout->addWidget(m_saveBtn);
    
    m_exportBtn = new GlassButton("📤", m_topToolbar);
    m_exportBtn->setFixedSize(40, 40);
    m_exportBtn->setToolTip(tr("تصدير"));
    topLayout->addWidget(m_exportBtn);
    
    topLayout->addSpacing(20);
    
    m_detectBtn = new GlassButton("🔍 " + tr("كشف الجسم"), m_topToolbar);
    m_detectBtn->setFixedHeight(40);
    m_detectBtn->setAccentColor(CP_ACCENT_CYAN);
    topLayout->addWidget(m_detectBtn);
    
    m_resetBtn = new GlassButton("🔄 " + tr("إعادة ضبط"), m_topToolbar);
    m_resetBtn->setFixedHeight(40);
    topLayout->addWidget(m_resetBtn);
    
    topLayout->addStretch();
    
    m_progressBar = new QProgressBar(m_topToolbar);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedSize(200, 20);
    m_progressBar->setStyleSheet(
        "QProgressBar { background: rgba(0,0,0,0.3); border-radius: 10px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 10px; }"
    );
    topLayout->addWidget(m_progressBar);
    
    centerLayout->addWidget(m_topToolbar, 0);
    
    // Canvas
    createCanvas();
    centerLayout->addWidget(m_canvas, 1);
    
    // Bottom bar
    m_bottomBar = new GlassPanel(this);
    m_bottomBar->setFixedHeight(35);
    m_bottomBar->setGlassOpacity(0.08);
    
    QHBoxLayout *bottomLayout = new QHBoxLayout(m_bottomBar);
    bottomLayout->setContentsMargins(15, 5, 15, 5);
    
    QLabel *statusLabel = new QLabel(tr("جاهز - افتح صورة للبدء"), m_bottomBar);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    bottomLayout->addWidget(statusLabel);
    
    bottomLayout->addStretch();
    
    QLabel *infoLabel = new QLabel("0 x 0", m_bottomBar);
    infoLabel->setObjectName("dimLabel");
    infoLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    bottomLayout->addWidget(infoLabel);
    
    centerLayout->addWidget(m_bottomBar, 0);
    mainLayout->addLayout(centerLayout, 1);
    
    // Right panels
    createSidePanels();
    mainLayout->addWidget(m_rightPanel, 0);
}

void BodyEditor::createToolbar()
{
    m_leftToolbar = new GlassPanel(this);
    m_leftToolbar->setFixedWidth(60);
    m_leftToolbar->setGlassOpacity(0.08);
    m_leftToolbar->setAccentColor(CP_ACCENT_CYAN);
    
    QVBoxLayout *layout = new QVBoxLayout(m_leftToolbar);
    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    // Tools
    QStringList tools = {"move", "select", "brush", "pin", "mesh"};
    QStringList icons = {"↖", "▭", "🖌", "📍", "⬡"};
    
    for (int i = 0; i < tools.size(); ++i) {
        GlassButton *btn = new GlassButton(icons[i], m_leftToolbar);
        btn->setFixedSize(45, 45);
        btn->setCheckable(true);
        btn->setToolTip(tools[i]);
        btn->setProperty("tool", tools[i]);
        if (i == 0) btn->setChecked(true);
        layout->addWidget(btn);
    }
    
    layout->addSpacing(20);
    
    // Brush settings
    QLabel *brushLabel = new QLabel(tr("الفرشاة"), m_leftToolbar);
    brushLabel->setStyleSheet("color: #888888; font-size: 10px;");
    layout->addWidget(brushLabel);
    
    QSlider *brushSlider = new QSlider(Qt::Vertical, m_leftToolbar);
    brushSlider->setRange(10, 200);
    brushSlider->setValue(50);
    brushSlider->setFixedHeight(100);
    layout->addWidget(brushSlider);
    
    layout->addStretch();
}

void BodyEditor::createCanvas()
{
    m_canvas = new BodyCanvas(this);
    m_canvas->setMinimumSize(400, 400);
}

void BodyEditor::createSidePanels()
{
    m_rightPanel = new GlassPanel(this);
    m_rightPanel->setFixedWidth(320);
    m_rightPanel->setGlassOpacity(0.1);
    m_rightPanel->setAccentColor(CP_ACCENT_CYAN);
    
    QVBoxLayout *layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(10, 15, 10, 15);
    layout->setSpacing(15);
    
    // Tabs
    QHBoxLayout *tabLayout = new QHBoxLayout();
    
    GlassButton *adjustTab = new GlassButton(tr("تعديلات"), m_rightPanel);
    adjustTab->setCheckable(true);
    adjustTab->setChecked(true);
    adjustTab->setFixedHeight(35);
    tabLayout->addWidget(adjustTab);
    
    GlassButton *presetsTab = new GlassButton(tr("قوالب"), m_rightPanel);
    presetsTab->setCheckable(true);
    presetsTab->setFixedHeight(35);
    tabLayout->addWidget(presetsTab);
    
    GlassButton *aiTab = new GlassButton(tr("AI"), m_rightPanel);
    aiTab->setCheckable(true);
    aiTab->setFixedHeight(35);
    tabLayout->addWidget(aiTab);
    
    layout->addLayout(tabLayout);
    
    // Stacked panels
    QStackedWidget *stack = new QStackedWidget(m_rightPanel);
    
    m_adjustmentsPanel = new BodyAdjustmentsPanel();
    m_presetsPanel = new BodyPresetsPanel();
    m_aiPanel = new BodyAIModelsPanel();
    
    stack->addWidget(m_adjustmentsPanel);
    stack->addWidget(m_presetsPanel);
    stack->addWidget(m_aiPanel);
    
    layout->addWidget(stack, 1);
    
    // Tab connections
    connect(adjustTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(0); });
    connect(presetsTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(1); });
    connect(aiTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(2); });
}

void BodyEditor::setupConnections()
{
    // Toolbar
    connect(m_undoBtn, &GlassButton::clicked, this, &BodyEditor::resetAllAdjustments);
    connect(m_saveBtn, &GlassButton::clicked, this, &BodyEditor::saveImage);
    connect(m_exportBtn, &GlassButton::clicked, this, &BodyEditor::exportImage);
    connect(m_resetBtn, &GlassButton::clicked, this, &BodyEditor::resetAllAdjustments);
    connect(m_detectBtn, &GlassButton::clicked, this, &BodyEditor::aiDetectBodyParts);
    
    // Canvas
    connect(m_canvas, &BodyCanvas::mousePressed, this, &BodyEditor::onCanvasMousePress);
    connect(m_canvas, &BodyCanvas::mouseMoved, this, &BodyEditor::onCanvasMouseMove);
    connect(m_canvas, &BodyCanvas::mouseReleased, this, &BodyEditor::onCanvasMouseRelease);
    
    // Panels
    connect(m_adjustmentsPanel, &BodyAdjustmentsPanel::adjustmentChanged,
            this, &BodyEditor::onAdjustmentChanged);
    connect(m_adjustmentsPanel, &BodyAdjustmentsPanel::resetClicked,
            this, &BodyEditor::resetAllAdjustments);
    connect(m_adjustmentsPanel, &BodyAdjustmentsPanel::autoEnhanceClicked,
            this, &BodyEditor::aiAutoEnhanceBody);
    
    connect(m_presetsPanel, &BodyPresetsPanel::presetSelected,
            this, &BodyEditor::onPresetSelected);
    
    // AI Panel
    connect(m_aiPanel, &BodyAIModelsPanel::detectBodyPartsClicked,
            this, &BodyEditor::aiDetectBodyParts);
    connect(m_aiPanel, &BodyAIModelsPanel::autoEnhanceClicked,
            this, &BodyEditor::aiAutoEnhanceBody);
    connect(m_aiPanel, &BodyAIModelsPanel::slimBodyClicked,
            this, &BodyEditor::aiSlimBody);
    connect(m_aiPanel, &BodyAIModelsPanel::enhanceMusclesClicked,
            this, &BodyEditor::aiEnhanceMuscles);
    connect(m_aiPanel, &BodyAIModelsPanel::perfectProportionsClicked,
            this, &BodyEditor::aiPerfectProportions);
    connect(m_aiPanel, &BodyAIModelsPanel::hourglassFigureClicked,
            this, &BodyEditor::aiHourglassFigure);
    connect(m_aiPanel, &BodyAIModelsPanel::athleticBuildClicked,
            this, &BodyEditor::aiAthleticBuild);
    connect(m_aiPanel, &BodyAIModelsPanel::tallEffectClicked,
            this, &BodyEditor::aiTallEffect);
}

void BodyEditor::openImage(const QString &path)
{
    QImage image(path);
    if (image.isNull()) {
        emit statusMessage(tr("فشل تحميل الصورة"));
        return;
    }
    
    m_originalImage = image;
    m_currentImage = image;
    m_currentPath = path;
    m_isModified = false;
    m_bodyDetected = false;
    m_detectedParts.clear();
    
    m_canvas->setImage(m_currentImage);
    m_canvas->setBodyParts(QVector<BodyPart>());
    
    // Reset adjustments
    m_params = m_defaultParams;
    m_adjustmentsPanel->resetAll();
    
    // Update UI
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    if (statusLabel) {
        statusLabel->setText(tr("تم فتح: %1").arg(QFileInfo(path).fileName()));
    }
    
    QLabel *dimLabel = findChild<QLabel*>("dimLabel");
    if (dimLabel) {
        dimLabel->setText(QString("%1 x %2").arg(image.width()).arg(image.height()));
    }
    
    emit statusMessage(tr("تم فتح الصورة - اضغط 'كشف الجسم' للبدء"));
}

void BodyEditor::saveImage()
{
    if (m_currentPath.isEmpty()) {
        QString path = QFileDialog::getSaveFileName(this, tr("حفظ الصورة"), QString(),
            tr("PNG (*.png);;JPEG (*.jpg *.jpeg)"));
        if (path.isEmpty()) return;
        m_currentPath = path;
    }
    
    if (m_currentImage.save(m_currentPath)) {
        m_isModified = false;
        emit statusMessage(tr("تم الحفظ"));
    } else {
        emit statusMessage(tr("فشل الحفظ"));
    }
}

void BodyEditor::exportImage()
{
    QString path = QFileDialog::getSaveFileName(this, tr("تصدير الصورة"), QString(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;WebP (*.webp)"));
    
    if (!path.isEmpty() && m_currentImage.save(path)) {
        emit statusMessage(tr("تم التصدير: %1").arg(path));
    }
}

void BodyEditor::resetAllAdjustments()
{
    m_params = m_defaultParams;
    m_adjustmentsPanel->resetAll();
    applyAdjustments();
    emit statusMessage(tr("تم إعادة ضبط جميع التعديلات"));
}

// Adjustment setters
void BodyEditor::setShoulderWidth(float value) { m_params.shoulderWidth = value; m_previewTimer->start(); }
void BodyEditor::setChestWidth(float value) { m_params.chestWidth = value; m_previewTimer->start(); }
void BodyEditor::setArmWidth(float value) { m_params.armWidth = value; m_previewTimer->start(); }
void BodyEditor::setArmLength(float value) { m_params.armLength = value; m_previewTimer->start(); }
void BodyEditor::setNeckWidth(float value) { m_params.neckWidth = value; m_previewTimer->start(); }
void BodyEditor::setNeckLength(float value) { m_params.neckLength = value; m_previewTimer->start(); }
void BodyEditor::setWaistWidth(float value) { m_params.waistWidth = value; m_previewTimer->start(); }
void BodyEditor::setTorsoLength(float value) { m_params.torsoLength = value; m_previewTimer->start(); }
void BodyEditor::setStomachFlatness(float value) { m_params.stomachFlatness = value; m_previewTimer->start(); }
void BodyEditor::setHipWidth(float value) { m_params.hipWidth = value; m_previewTimer->start(); }
void BodyEditor::setLegLength(float value) { m_params.legLength = value; m_previewTimer->start(); }
void BodyEditor::setThighWidth(float value) { m_params.thighWidth = value; m_previewTimer->start(); }
void BodyEditor::setCalfWidth(float value) { m_params.calfWidth = value; m_previewTimer->start(); }
void BodyEditor::setButtockSize(float value) { m_params.buttockSize = value; m_previewTimer->start(); }
void BodyEditor::setHeight(float value) { m_params.height = value; m_previewTimer->start(); }
void BodyEditor::setBodyProportion(float value) { m_params.bodyProportion = value; m_previewTimer->start(); }
void BodyEditor::setSlimness(float value) { m_params.slimness = value; m_previewTimer->start(); }
void BodyEditor::setMuscleDefinition(float value) { m_params.muscleDefinition = value; m_previewTimer->start(); }

void BodyEditor::onAdjustmentChanged(const QString &name, float value)
{
    if (name == "shoulderWidth") setShoulderWidth(value);
    else if (name == "chestWidth") setChestWidth(value);
    else if (name == "armWidth") setArmWidth(value);
    else if (name == "armLength") setArmLength(value);
    else if (name == "neckWidth") setNeckWidth(value);
    else if (name == "neckLength") setNeckLength(value);
    else if (name == "waistWidth") setWaistWidth(value);
    else if (name == "torsoLength") setTorsoLength(value);
    else if (name == "stomachFlatness") setStomachFlatness(value);
    else if (name == "hipWidth") setHipWidth(value);
    else if (name == "legLength") setLegLength(value);
    else if (name == "thighWidth") setThighWidth(value);
    else if (name == "calfWidth") setCalfWidth(value);
    else if (name == "buttockSize") setButtockSize(value);
    else if (name == "height") setHeight(value);
    else if (name == "bodyProportion") setBodyProportion(value);
    else if (name == "slimness") setSlimness(value);
    else if (name == "muscleDefinition") setMuscleDefinition(value);
}

void BodyEditor::applyAdjustments()
{
    if (m_originalImage.isNull()) return;
    
    m_currentImage = applyBodyTransformations(m_originalImage);
    m_canvas->setImage(m_currentImage);
    m_isModified = true;
    
    emit imageModified(true);
}

QImage BodyEditor::applyBodyTransformations(const QImage &input)
{
    QImage result = input.copy();
    
    if (!m_bodyDetected || m_detectedParts.isEmpty()) {
        // Apply global adjustments
        if (m_params.slimness > 0) {
            result = liquifyArea(result, QPoint(result.width()/2, result.height()/2), 
                                result.width() * 0.4f, -m_params.slimness * 0.3f);
        }
        return result;
    }
    
    // Apply part-specific transformations
    for (const BodyPart &part : m_detectedParts) {
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        
        if (part.name == "shoulders") {
            scaleX = m_params.shoulderWidth;
        } else if (part.name == "chest") {
            scaleX = m_params.chestWidth;
        } else if (part.name == "waist") {
            scaleX = m_params.waistWidth;
        } else if (part.name == "hips") {
            scaleX = m_params.hipWidth;
        } else if (part.name == "thighs") {
            scaleX = m_params.thighWidth;
        } else if (part.name == "calves") {
            scaleX = m_params.calfWidth;
        } else if (part.name == "arms") {
            scaleX = m_params.armWidth;
            scaleY = m_params.armLength;
        } else if (part.name == "neck") {
            scaleX = m_params.neckWidth;
            scaleY = m_params.neckLength;
        }
        
        if (scaleX != 1.0f || scaleY != 1.0f) {
            result = warpBodyPart(result, part, scaleX, scaleY);
        }
    }
    
    // Apply muscle definition
    if (m_params.muscleDefinition > 0) {
        result = enhanceMusclesInternal(result);
    }
    
    // Smooth contours
    result = smoothBodyContours(result);
    
    return result;
}

QImage BodyEditor::warpBodyPart(const QImage &input, const BodyPart &part, float scaleX, float scaleY)
{
    QImage result = input.copy();
    
    // Create warp mesh around body part
    QPointF center = part.center;
    float radius = qMax(part.boundingBox.width(), part.boundingBox.height()) * 0.6f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            float dx = x - center.x();
            float dy = y - center.y();
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < radius) {
                float factor = 1.0f - (dist / radius);
                factor = factor * factor * (3.0f - 2.0f * factor); // Smoothstep
                
                float warpX = dx * (scaleX - 1.0f) * factor;
                float warpY = dy * (scaleY - 1.0f) * factor;
                
                int srcX = qBound(0, int(x - warpX), input.width() - 1);
                int srcY = qBound(0, int(y - warpY), input.height() - 1);
                
                result.setPixelColor(x, y, input.pixelColor(srcX, srcY));
            }
        }
    }
    
    return result;
}

QImage BodyEditor::liquifyArea(const QImage &input, const QPoint &center, float radius, float strength)
{
    QImage result = input.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            float dx = x - center.x();
            float dy = y - center.y();
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < radius && dist > 0) {
                float factor = 1.0f - (dist / radius);
                factor = factor * factor;
                
                float push = strength * factor;
                float srcX = x + (dx / dist) * push * radius;
                float srcY = y + (dy / dist) * push * radius;
                
                // Bilinear interpolation
                int x0 = qBound(0, int(srcX), input.width() - 1);
                int y0 = qBound(0, int(srcY), input.height() - 1);
                int x1 = qMin(x0 + 1, input.width() - 1);
                int y1 = qMin(y0 + 1, input.height() - 1);
                
                float fx = srcX - x0;
                float fy = srcY - y0;
                
                QColor c00 = input.pixelColor(x0, y0);
                QColor c10 = input.pixelColor(x1, y0);
                QColor c01 = input.pixelColor(x0, y1);
                QColor c11 = input.pixelColor(x1, y1);
                
                int r = int((1-fx)*(1-fy)*c00.red() + fx*(1-fy)*c10.red() + 
                           (1-fx)*fy*c01.red() + fx*fy*c11.red());
                int g = int((1-fx)*(1-fy)*c00.green() + fx*(1-fy)*c10.green() + 
                           (1-fx)*fy*c01.green() + fx*fy*c11.green());
                int b = int((1-fx)*(1-fy)*c00.blue() + fx*(1-fy)*c10.blue() + 
                           (1-fx)*fy*c01.blue() + fx*fy*c11.blue());
                
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

QImage BodyEditor::smoothBodyContours(const QImage &input)
{
    // Simple Gaussian-like smoothing
    QImage result(input.size(), QImage::Format_ARGB32);
    
    int kernel[5][5] = {
        {1, 4, 7, 4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1, 4, 7, 4, 1}
    };
    
    for (int y = 2; y < input.height() - 2; ++y) {
        for (int x = 2; x < input.width() - 2; ++x) {
            int r = 0, g = 0, b = 0, sum = 0;
            
            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    QColor c = input.pixelColor(x + kx, y + ky);
                    int w = kernel[ky + 2][kx + 2];
                    r += c.red() * w;
                    g += c.green() * w;
                    b += c.blue() * w;
                    sum += w;
                }
            }
            
            result.setPixelColor(x, y, QColor(r / sum, g / sum, b / sum));
        }
    }
    
    return result;
}

QImage BodyEditor::enhanceMusclesInternal(const QImage &input)
{
    QImage result = input.copy();
    
    // Edge detection for muscle definition
    for (int y = 1; y < input.height() - 1; ++y) {
        for (int x = 1; x < input.width() - 1; ++x) {
            QColor c = input.pixelColor(x, y);
            QColor left = input.pixelColor(x - 1, y);
            QColor right = input.pixelColor(x + 1, y);
            QColor up = input.pixelColor(x, y - 1);
            QColor down = input.pixelColor(x, y + 1);
            
            int edge = qAbs(c.lightness() - left.lightness()) +
                      qAbs(c.lightness() - right.lightness()) +
                      qAbs(c.lightness() - up.lightness()) +
                      qAbs(c.lightness() - down.lightness());
            
            if (edge > 30) {
                // Enhance edge
                int factor = int(m_params.muscleDefinition * 20);
                int r = qBound(0, c.red() - factor, 255);
                int g = qBound(0, c.green() - factor, 255);
                int b = qBound(0, c.blue() - factor, 255);
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

// AI Operations
void BodyEditor::aiDetectBodyParts()
{
    if (m_originalImage.isNull()) {
        emit statusMessage(tr("افتح صورة أولاً"));
        return;
    }
    
    emit processingStarted(tr("كشف أجزاء الجسم"));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_isProcessing = true;
    
    // Simulate AI detection with progress
    QTimer *timer = new QTimer(this);
    int progress = 0;
    connect(timer, &QTimer::timeout, this, [this, &progress, timer]() {
        progress += 5;
        m_progressBar->setValue(progress);
        emit processingProgressChanged(progress);
        
        if (progress >= 100) {
            timer->stop();
            timer->deleteLater();
            
            detectBodyPartsInternal();
            
            m_progressBar->setVisible(false);
            m_isProcessing = false;
            m_bodyDetected = true;
            
            emit processingFinished(tr("تم كشف %1 جزء").arg(m_detectedParts.size()));
            emit bodyPartsDetected(m_detectedParts);
            emit statusMessage(tr("تم كشف الجسم - يمكنك الآن التعديل"));
        }
    });
    timer->start(50);
}

void BodyEditor::detectBodyPartsInternal()
{
    m_detectedParts.clear();
    
    int w = m_originalImage.width();
    int h = m_originalImage.height();
    
    // Simulate body part detection
    BodyPart head;
    head.name = "head";
    head.boundingBox = QRect(w * 0.4, h * 0.05, w * 0.2, h * 0.15);
    head.center = QPointF(w * 0.5, h * 0.12);
    head.confidence = 0.95f;
    m_detectedParts.append(head);
    
    BodyPart neck;
    neck.name = "neck";
    neck.boundingBox = QRect(w * 0.45, h * 0.18, w * 0.1, h * 0.05);
    neck.center = QPointF(w * 0.5, h * 0.2);
    neck.confidence = 0.88f;
    m_detectedParts.append(neck);
    
    BodyPart shoulders;
    shoulders.name = "shoulders";
    shoulders.boundingBox = QRect(w * 0.25, h * 0.22, w * 0.5, h * 0.08);
    shoulders.center = QPointF(w * 0.5, h * 0.26);
    shoulders.confidence = 0.92f;
    m_detectedParts.append(shoulders);
    
    BodyPart chest;
    chest.name = "chest";
    chest.boundingBox = QRect(w * 0.35, h * 0.28, w * 0.3, h * 0.12);
    chest.center = QPointF(w * 0.5, h * 0.34);
    chest.confidence = 0.90f;
    m_detectedParts.append(chest);
    
    BodyPart waist;
    waist.name = "waist";
    waist.boundingBox = QRect(w * 0.38, h * 0.42, w * 0.24, h * 0.08);
    waist.center = QPointF(w * 0.5, h * 0.46);
    waist.confidence = 0.85f;
    m_detectedParts.append(waist);
    
    BodyPart hips;
    hips.name = "hips";
    hips.boundingBox = QRect(w * 0.32, h * 0.48, w * 0.36, h * 0.1);
    hips.center = QPointF(w * 0.5, h * 0.53);
    hips.confidence = 0.87f;
    m_detectedParts.append(hips);
    
    BodyPart leftArm;
    leftArm.name = "arms";
    leftArm.boundingBox = QRect(w * 0.15, h * 0.25, w * 0.15, h * 0.3);
    leftArm.center = QPointF(w * 0.22, h * 0.4);
    leftArm.confidence = 0.82f;
    m_detectedParts.append(leftArm);
    
    BodyPart rightArm;
    rightArm.name = "arms";
    rightArm.boundingBox = QRect(w * 0.7, h * 0.25, w * 0.15, h * 0.3);
    rightArm.center = QPointF(w * 0.78, h * 0.4);
    rightArm.confidence = 0.82f;
    m_detectedParts.append(rightArm);
    
    BodyPart leftThigh;
    leftThigh.name = "thighs";
    leftThigh.boundingBox = QRect(w * 0.35, h * 0.58, w * 0.12, h * 0.2);
    leftThigh.center = QPointF(w * 0.41, h * 0.68);
    leftThigh.confidence = 0.86f;
    m_detectedParts.append(leftThigh);
    
    BodyPart rightThigh;
    rightThigh.name = "thighs";
    rightThigh.boundingBox = QRect(w * 0.53, h * 0.58, w * 0.12, h * 0.2);
    rightThigh.center = QPointF(w * 0.59, h * 0.68);
    rightThigh.confidence = 0.86f;
    m_detectedParts.append(rightThigh);
    
    BodyPart leftCalf;
    leftCalf.name = "calves";
    leftCalf.boundingBox = QRect(w * 0.36, h * 0.78, w * 0.1, h * 0.18);
    leftCalf.center = QPointF(w * 0.41, h * 0.87);
    leftCalf.confidence = 0.80f;
    m_detectedParts.append(leftCalf);
    
    BodyPart rightCalf;
    rightCalf.name = "calves";
    rightCalf.boundingBox = QRect(w * 0.54, h * 0.78, w * 0.1, h * 0.18);
    rightCalf.center = QPointF(w * 0.59, h * 0.87);
    rightCalf.confidence = 0.80f;
    m_detectedParts.append(rightCalf);
    
    m_canvas->setBodyParts(m_detectedParts);
}

void BodyEditor::aiAutoEnhanceBody()
{
    if (!m_bodyDetected) {
        aiDetectBodyParts();
        return;
    }
    
    emit processingStarted(tr("تحسين الجسم تلقائياً"));
    
    // Apply optimal adjustments
    m_params.shoulderWidth = 1.05f;
    m_params.waistWidth = 0.92f;
    m_params.hipWidth = 1.03f;
    m_params.legLength = 1.04f;
    m_params.slimness = 0.15f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit processingFinished(tr("تم التحسين"));
    emit statusMessage(tr("تم تحسين الجسم تلقائياً"));
}

void BodyEditor::aiSlimBody()
{
    m_params.slimness = 0.25f;
    m_params.waistWidth = 0.88f;
    m_params.thighWidth = 0.92f;
    m_params.armWidth = 0.90f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم نحت الجسم"));
}

void BodyEditor::aiEnhanceMuscles()
{
    m_params.muscleDefinition = 0.6f;
    m_params.chestWidth = 1.08f;
    m_params.shoulderWidth = 1.06f;
    m_params.armWidth = 1.05f;
    m_params.calfWidth = 1.04f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم تعزيز العضلات"));
}

void BodyEditor::aiPerfectProportions()
{
    m_params.bodyProportion = 1.08f;
    m_params.legLength = 1.06f;
    m_params.torsoLength = 0.98f;
    m_params.shoulderWidth = 1.04f;
    m_params.waistWidth = 0.90f;
    m_params.hipWidth = 1.02f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم تطبيق النسب المثالية"));
}

void BodyEditor::aiHourglassFigure()
{
    m_params.shoulderWidth = 1.06f;
    m_params.chestWidth = 1.05f;
    m_params.waistWidth = 0.78f;
    m_params.hipWidth = 1.10f;
    m_params.buttockSize = 1.08f;
    m_params.thighWidth = 0.95f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم تطبيق قوام الساعة الرملية"));
}

void BodyEditor::aiAthleticBuild()
{
    m_params.shoulderWidth = 1.10f;
    m_params.chestWidth = 1.08f;
    m_params.waistWidth = 0.88f;
    m_params.muscleDefinition = 0.5f;
    m_params.armWidth = 1.06f;
    m_params.legLength = 1.04f;
    m_params.calfWidth = 1.05f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم تطبيق البنية الرياضية"));
}

void BodyEditor::aiTallEffect()
{
    m_params.height = 1.06f;
    m_params.legLength = 1.08f;
    m_params.torsoLength = 1.03f;
    m_params.neckLength = 1.05f;
    m_params.bodyProportion = 1.05f;
    
    m_adjustmentsPanel->setValues(m_params);
    applyAdjustments();
    
    emit statusMessage(tr("تم تطبيق تأثير الطول"));
}

void BodyEditor::onCanvasMousePress(const QPoint &pos)
{
    m_isDrawing = true;
    m_lastPos = pos;
}

void BodyEditor::onCanvasMouseMove(const QPoint &pos)
{
    if (!m_isDrawing) return;
    
    if (m_currentTool == "brush") {
        // Apply liquify brush
        float strength = (m_brushIntensity - 50) / 50.0f * 0.2f;
        m_currentImage = liquifyArea(m_currentImage, pos, m_brushSize, strength);
        m_canvas->setImage(m_currentImage);
    }
    
    m_lastPos = pos;
}

void BodyEditor::onCanvasMouseRelease(const QPoint &pos)
{
    m_isDrawing = false;
    m_isModified = true;
    emit imageModified(true);
}

void BodyEditor::onPresetSelected(const QString &preset)
{
    applyPreset(preset);
}

void BodyEditor::applyPreset(const QString &presetName)
{
    QMap<QString, BodyAdjustParams> presets;
    
    // Define presets
    BodyAdjustParams slim;
    slim.slimness = 0.3f;
    slim.waistWidth = 0.85f;
    presets["نحيف"] = slim;
    
    BodyAdjustParams athletic;
    athletic.shoulderWidth = 1.10f;
    athletic.chestWidth = 1.08f;
    athletic.muscleDefinition = 0.5f;
    presets["رياضي"] = athletic;
    
    BodyAdjustParams hourglass;
    hourglass.shoulderWidth = 1.06f;
    hourglass.waistWidth = 0.78f;
    hourglass.hipWidth = 1.10f;
    presets["ساعة رملية"] = hourglass;
    
    BodyAdjustParams tall;
    tall.height = 1.06f;
    tall.legLength = 1.08f;
    presets["طويل"] = tall;
    
    if (presets.contains(presetName)) {
        m_params = presets[presetName];
        m_adjustmentsPanel->setValues(m_params);
        applyAdjustments();
        emit statusMessage(tr("تم تطبيق القالب: %1").arg(presetName));
    }
}

void BodyEditor::setProcessingProgress(float progress)
{
    m_processingProgress = progress;
    m_progressBar->setValue(int(progress * 100));
}

void BodyEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_O: {
            QString path = QFileDialog::getOpenFileName(this, tr("فتح صورة"), QString(),
                tr("الصور (*.png *.jpg *.jpeg *.bmp)"));
            if (!path.isEmpty()) openImage(path);
            return;
        }
        case Qt::Key_S:
            saveImage();
            return;
        case Qt::Key_E:
            exportImage();
            return;
        case Qt::Key_R:
            resetAllAdjustments();
            return;
        }
    }
    
    QWidget::keyPressEvent(event);
}

void BodyEditor::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

// ==================== BodyCanvas Implementation ====================

BodyCanvas::BodyCanvas(QWidget *parent)
    : QWidget(parent)
    , m_selectedPart(-1)
    , m_zoom(1.0f)
    , m_showOverlay(true)
    , m_showGrid(false)
    , m_showMeasurements(false)
    , m_isDragging(false)
{
    setMouseTracking(true);
    setMinimumSize(300, 400);
}

void BodyCanvas::setImage(const QImage &image)
{
    m_image = image;
    update();
}

void BodyCanvas::setBodyParts(const QVector<BodyPart> &parts)
{
    m_bodyParts = parts;
    update();
}

void BodyCanvas::setSelectedPart(int index)
{
    m_selectedPart = index;
    update();
}

void BodyCanvas::setShowOverlay(bool show)
{
    m_showOverlay = show;
    update();
}

void BodyCanvas::setShowGrid(bool show)
{
    m_showGrid = show;
    update();
}

void BodyCanvas::setShowMeasurements(bool show)
{
    m_showMeasurements = show;
    update();
}

void BodyCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Background
    painter.fillRect(rect(), QColor(15, 15, 25));
    
    // Grid
    if (m_showGrid) drawGrid(painter);
    
    // Image
    drawImage(painter);
    
    // Body parts overlay
    if (m_showOverlay && !m_bodyParts.isEmpty()) {
        drawBodyParts(painter);
    }
    
    // Measurements
    if (m_showMeasurements) drawMeasurements(painter);
    
    // Overlay info
    drawOverlay(painter);
}

void BodyCanvas::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor(255, 255, 255, 20), 1));
    int gridSize = 50;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }
}

void BodyCanvas::drawImage(QPainter &painter)
{
    if (m_image.isNull()) {
        painter.setPen(QPen(QColor(100, 100, 100)));
        painter.drawText(rect(), Qt::AlignCenter, tr("افتح صورة للبدء"));
        return;
    }
    
    QRect imgRect = imageRect();
    painter.drawImage(imgRect, m_image);
}

void BodyCanvas::drawBodyParts(QPainter &painter)
{
    for (int i = 0; i < m_bodyParts.size(); ++i) {
        const BodyPart &part = m_bodyParts[i];
        
        QColor color = (i == m_selectedPart) ? CP_ACCENT_PINK : CP_ACCENT_CYAN;
        color.setAlpha(80);
        
        painter.setPen(QPen(color, 2));
        painter.setBrush(QBrush(color, Qt::Dense4Pattern));
        
        QRect rect = part.boundingBox;
        // Scale to canvas
        QRect imgRect = imageRect();
        float scaleX = float(imgRect.width()) / m_image.width();
        float scaleY = float(imgRect.height()) / m_image.height();
        
        QRect scaledRect(
            imgRect.left() + int(rect.left() * scaleX),
            imgRect.top() + int(rect.top() * scaleY),
            int(rect.width() * scaleX),
            int(rect.height() * scaleY)
        );
        
        painter.drawRoundedRect(scaledRect, 8, 8);
        
        // Part name
        painter.setPen(QPen(Qt::white));
        painter.drawText(scaledRect.topLeft() + QPoint(5, -5), part.name);
    }
}

void BodyCanvas::drawMeasurements(QPainter &painter)
{
    // Draw measurement lines
    painter.setPen(QPen(CP_ACCENT_GREEN, 1, Qt::DashLine));
    
    // Shoulder line
    // Waist line
    // Hip line
}

void BodyCanvas::drawOverlay(QPainter &painter)
{
    // Zoom level
    painter.setPen(QPen(QColor(200, 200, 200)));
    painter.drawText(10, 20, QString("%1%").arg(int(m_zoom * 100)));
}

QRect BodyCanvas::imageRect() const
{
    if (m_image.isNull()) return QRect();
    
    float imgAspect = float(m_image.width()) / m_image.height();
    float canvasAspect = float(width()) / height();
    
    QRect rect;
    if (imgAspect > canvasAspect) {
        int h = int(width() / imgAspect);
        rect = QRect(0, (height() - h) / 2, width(), h);
    } else {
        int w = int(height() * imgAspect);
        rect = QRect((width() - w) / 2, 0, w, height());
    }
    
    return rect;
}

void BodyCanvas::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event->pos());
    
    // Check if clicked on a body part
    for (int i = 0; i < m_bodyParts.size(); ++i) {
        if (m_bodyParts[i].boundingBox.contains(event->pos())) {
            m_selectedPart = i;
            emit partClicked(i);
            update();
            break;
        }
    }
}

void BodyCanvas::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event->pos());
}

void BodyCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event->pos());
}

void BodyCanvas::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() > 0 ? 1.1f : 0.9f;
    m_zoom *= delta;
    m_zoom = qBound(0.1f, m_zoom, 5.0f);
    update();
}

// ==================== BodyAdjustmentsPanel Implementation ====================

BodyAdjustmentsPanel::BodyAdjustmentsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void BodyAdjustmentsPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    // Auto enhance button
    GlassButton *autoBtn = new GlassButton(tr("✨ تحسين تلقائي"), this);
    autoBtn->setFixedHeight(40);
    autoBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(autoBtn, &GlassButton::clicked, this, &BodyAdjustmentsPanel::autoEnhanceClicked);
    mainLayout->addWidget(autoBtn);
    
    mainLayout->addSpacing(10);
    
    // Scroll area for adjustments
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(15);
    
    // Upper body group
    QGroupBox *upperGroup = new QGroupBox(tr("الجزء العلوي"), this);
    upperGroup->setStyleSheet(
        "QGroupBox { color: #FFFFFF; font-size: 12px; font-weight: bold; "
        "border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; margin-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
    );
    QVBoxLayout *upperLayout = new QVBoxLayout(upperGroup);
    
    createSlider("shoulderWidth", tr("عرض الكتفين"), 0.7f, 1.3f, 1.0f, "upper");
    createSlider("chestWidth", tr("عرض الصدر"), 0.7f, 1.3f, 1.0f, "upper");
    createSlider("armWidth", tr("عرض الذراعين"), 0.7f, 1.3f, 1.0f, "upper");
    createSlider("armLength", tr("طول الذراعين"), 0.8f, 1.2f, 1.0f, "upper");
    createSlider("neckWidth", tr("عرض الرقبة"), 0.7f, 1.3f, 1.0f, "upper");
    createSlider("neckLength", tr("طول الرقبة"), 0.8f, 1.2f, 1.0f, "upper");
    
    upperLayout->addWidget(m_groups["upper"]);
    containerLayout->addWidget(upperGroup);
    
    // Mid body group
    QGroupBox *midGroup = new QGroupBox(tr("الجزء الأوسط"), this);
    midGroup->setStyleSheet(upperGroup->styleSheet());
    QVBoxLayout *midLayout = new QVBoxLayout(midGroup);
    
    createSlider("waistWidth", tr("عرض الخصر"), 0.6f, 1.4f, 1.0f, "mid");
    createSlider("torsoLength", tr("طول الجذع"), 0.8f, 1.2f, 1.0f, "mid");
    createSlider("stomachFlatness", tr("تسطيح البطن"), 0.0f, 1.0f, 0.0f, "mid");
    
    midLayout->addWidget(m_groups["mid"]);
    containerLayout->addWidget(midGroup);
    
    // Lower body group
    QGroupBox *lowerGroup = new QGroupBox(tr("الجزء السفلي"), this);
    lowerGroup->setStyleSheet(upperGroup->styleSheet());
    QVBoxLayout *lowerLayout = new QVBoxLayout(lowerGroup);
    
    createSlider("hipWidth", tr("عرض الوركين"), 0.7f, 1.3f, 1.0f, "lower");
    createSlider("legLength", tr("طول الساقين"), 0.8f, 1.2f, 1.0f, "lower");
    createSlider("thighWidth", tr("عرض الفخذين"), 0.7f, 1.3f, 1.0f, "lower");
    createSlider("calfWidth", tr("عرض الساقين"), 0.7f, 1.3f, 1.0f, "lower");
    createSlider("buttockSize", tr("حجم الأرداف"), 0.7f, 1.3f, 1.0f, "lower");
    
    lowerLayout->addWidget(m_groups["lower"]);
    containerLayout->addWidget(lowerGroup);
    
    // Overall group
    QGroupBox *overallGroup = new QGroupBox(tr("عام"), this);
    overallGroup->setStyleSheet(upperGroup->styleSheet());
    QVBoxLayout *overallLayout = new QVBoxLayout(overallGroup);
    
    createSlider("height", tr("الطول"), 0.9f, 1.15f, 1.0f, "overall");
    createSlider("bodyProportion", tr("تناسب الجسم"), 0.9f, 1.15f, 1.0f, "overall");
    createSlider("slimness", tr("النحافة"), 0.0f, 1.0f, 0.0f, "overall");
    createSlider("muscleDefinition", tr("تعريف العضلات"), 0.0f, 1.0f, 0.0f, "overall");
    
    overallLayout->addWidget(m_groups["overall"]);
    containerLayout->addWidget(overallGroup);
    
    containerLayout->addStretch();
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea, 1);
    
    // Reset button
    GlassButton *resetBtn = new GlassButton(tr("إعادة ضبط الكل"), this);
    resetBtn->setFixedHeight(35);
    connect(resetBtn, &GlassButton::clicked, this, &BodyAdjustmentsPanel::resetClicked);
    mainLayout->addWidget(resetBtn);
}

QSlider *BodyAdjustmentsPanel::createSlider(const QString &name, const QString &label, 
                                            float min, float max, float defaultValue, 
                                            const QString &group)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(3);
    
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setStyleSheet("color: #CCCCCC; font-size: 11px;");
    layout->addWidget(labelWidget);
    
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(int(min * 100), int(max * 100));
    slider->setValue(int(defaultValue * 100));
    slider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: rgba(0, 0, 0, 0.3); border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 14px; height: 14px; background: #00FFFF; "
        "border-radius: 7px; margin: -4px 0; }"
        "QSlider::sub-page:horizontal { background: #00FFFF; border-radius: 3px; }"
    );
    
    QLabel *valueLabel = new QLabel(QString::number(defaultValue, 'f', 2));
    valueLabel->setStyleSheet("color: #00FFFF; font-size: 10px; min-width: 35px;");
    valueLabel->setAlignment(Qt::AlignRight);
    
    connect(slider, &QSlider::valueChanged, this, [this, name, valueLabel](int value) {
        float fvalue = value / 100.0f;
        valueLabel->setText(QString::number(fvalue, 'f', 2));
        emit adjustmentChanged(name, fvalue);
    });
    
    sliderLayout->addWidget(slider, 1);
    sliderLayout->addWidget(valueLabel);
    layout->addLayout(sliderLayout);
    
    m_sliders[name] = slider;
    m_valueLabels[name] = valueLabel;
    
    // Add to group layout
    if (m_groups.contains(group)) {
        m_groups[group]->layout()->addItem(layout);
    }
    
    return slider;
}

void BodyAdjustmentsPanel::setValues(const BodyAdjustParams &params)
{
    m_sliders["shoulderWidth"]->setValue(int(params.shoulderWidth * 100));
    m_sliders["chestWidth"]->setValue(int(params.chestWidth * 100));
    m_sliders["armWidth"]->setValue(int(params.armWidth * 100));
    m_sliders["armLength"]->setValue(int(params.armLength * 100));
    m_sliders["neckWidth"]->setValue(int(params.neckWidth * 100));
    m_sliders["neckLength"]->setValue(int(params.neckLength * 100));
    m_sliders["waistWidth"]->setValue(int(params.waistWidth * 100));
    m_sliders["torsoLength"]->setValue(int(params.torsoLength * 100));
    m_sliders["stomachFlatness"]->setValue(int(params.stomachFlatness * 100));
    m_sliders["hipWidth"]->setValue(int(params.hipWidth * 100));
    m_sliders["legLength"]->setValue(int(params.legLength * 100));
    m_sliders["thighWidth"]->setValue(int(params.thighWidth * 100));
    m_sliders["calfWidth"]->setValue(int(params.calfWidth * 100));
    m_sliders["buttockSize"]->setValue(int(params.buttockSize * 100));
    m_sliders["height"]->setValue(int(params.height * 100));
    m_sliders["bodyProportion"]->setValue(int(params.bodyProportion * 100));
    m_sliders["slimness"]->setValue(int(params.slimness * 100));
    m_sliders["muscleDefinition"]->setValue(int(params.muscleDefinition * 100));
}

BodyAdjustParams BodyAdjustmentsPanel::values() const
{
    BodyAdjustParams params;
    params.shoulderWidth = m_sliders["shoulderWidth"]->value() / 100.0f;
    params.chestWidth = m_sliders["chestWidth"]->value() / 100.0f;
    params.armWidth = m_sliders["armWidth"]->value() / 100.0f;
    params.armLength = m_sliders["armLength"]->value() / 100.0f;
    params.neckWidth = m_sliders["neckWidth"]->value() / 100.0f;
    params.neckLength = m_sliders["neckLength"]->value() / 100.0f;
    params.waistWidth = m_sliders["waistWidth"]->value() / 100.0f;
    params.torsoLength = m_sliders["torsoLength"]->value() / 100.0f;
    params.stomachFlatness = m_sliders["stomachFlatness"]->value() / 100.0f;
    params.hipWidth = m_sliders["hipWidth"]->value() / 100.0f;
    params.legLength = m_sliders["legLength"]->value() / 100.0f;
    params.thighWidth = m_sliders["thighWidth"]->value() / 100.0f;
    params.calfWidth = m_sliders["calfWidth"]->value() / 100.0f;
    params.buttockSize = m_sliders["buttockSize"]->value() / 100.0f;
    params.height = m_sliders["height"]->value() / 100.0f;
    params.bodyProportion = m_sliders["bodyProportion"]->value() / 100.0f;
    params.slimness = m_sliders["slimness"]->value() / 100.0f;
    params.muscleDefinition = m_sliders["muscleDefinition"]->value() / 100.0f;
    return params;
}

void BodyAdjustmentsPanel::resetAll()
{
    for (auto it = m_sliders.begin(); it != m_sliders.end(); ++it) {
        it.value()->setValue(100);
    }
}

// ==================== BodyPresetsPanel Implementation ====================

BodyPresetsPanel::BodyPresetsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadPresets();
}

void BodyPresetsPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    QLabel *titleLabel = new QLabel(tr("قوالب الجسم"));
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);
    
    // Presets container
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    m_presetsContainer = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(m_presetsContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);
    containerLayout->addStretch();
    
    scrollArea->setWidget(m_presetsContainer);
    layout->addWidget(scrollArea, 1);
    
    // Save current button
    GlassButton *saveBtn = new GlassButton(tr("💾 حفظ الحالي كقالب"), this);
    saveBtn->setFixedHeight(40);
    connect(saveBtn, &GlassButton::clicked, this, [this]() {
        bool ok;
        QString name = QInputDialog::getText(this, tr("حفظ قالب"), 
                                             tr("اسم القالب:"), QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            savePreset(name, BodyAdjustParams());
            emit presetSaved(name);
        }
    });
    layout->addWidget(saveBtn);
}

void BodyPresetsPanel::loadPresets()
{
    // Create default presets
    createPresetCard(tr("نحيف"), QImage());
    createPresetCard(tr("رياضي"), QImage());
    createPresetCard(tr("ساعة رملية"), QImage());
    createPresetCard(tr("طويل"), QImage());
    createPresetCard(tr("عضلي"), QImage());
    createPresetCard(tr("متناسق"), QImage());
}

void BodyPresetsPanel::createPresetCard(const QString &name, const QImage &thumbnail)
{
    GlassPanel *card = new GlassPanel(m_presetsContainer);
    card->setFixedHeight(80);
    card->setGlassOpacity(0.1);
    card->setAccentColor(CP_ACCENT_CYAN);
    card->setCursor(Qt::PointingHandCursor);
    
    QHBoxLayout *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(15, 10, 15, 10);
    
    // Thumbnail placeholder
    QLabel *thumbLabel = new QLabel();
    thumbLabel->setFixedSize(60, 60);
    thumbLabel->setStyleSheet("background: rgba(0,0,0,0.3); border-radius: 8px;");
    cardLayout->addWidget(thumbLabel);
    
    // Name
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    cardLayout->addWidget(nameLabel, 1);
    
    // Click handler
    card->setMouseTracking(true);
    connect(card, &GlassPanel::mousePressEvent, this, [this, name]() {
        emit presetSelected(name);
    });
    
    QVBoxLayout *containerLayout = qobject_cast<QVBoxLayout*>(m_presetsContainer->layout());
    if (containerLayout) {
        containerLayout->insertWidget(containerLayout->count() - 1, card);
    }
}

void BodyPresetsPanel::savePreset(const QString &name, const BodyAdjustParams &params)
{
    m_presets[name] = params;
    createPresetCard(name, QImage());
}

void BodyPresetsPanel::deletePreset(const QString &name)
{
    m_presets.remove(name);
}

// ==================== BodyAIModelsPanel Implementation ====================

BodyAIModelsPanel::BodyAIModelsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void BodyAIModelsPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    QLabel *titleLabel = new QLabel(tr("🤖 أدوات AI"));
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);
    
    // AI buttons
    createAIButton("detect", tr("🔍 كشف الجسم"), 
                   tr("كشف تلقائي لأجزاء الجسم"), CP_ACCENT_CYAN);
    createAIButton("enhance", tr("✨ تحسين تلقائي"), 
                   tr("تحسين شامل للجسم"), CP_ACCENT_GREEN);
    createAIButton("slim", tr("🎯 نحت الجسم"), 
                   tr("نحت ونحافة الجسم"), CP_ACCENT_PINK);
    createAIButton("muscles", tr("💪 تعزيز العضلات"), 
                   tr("تعزيز وتعريف العضلات"), CP_ACCENT_ORANGE);
    createAIButton("proportions", tr("📐 نسب مثالية"), 
                   tr("تصحيح نسب الجسم"), CP_ACCENT_PURPLE);
    createAIButton("hourglass", tr("⏳ ساعة رملية"), 
                   tr("قوام الساعة الرملية"), QColor(255, 100, 150));
    createAIButton("athletic", tr("🏃 بنية رياضية"), 
                   tr("بنية رياضية متناسقة"), QColor(100, 200, 100));
    createAIButton("tall", tr("📏 تأثير الطول"), 
                   tr("إضفاء مظهر أطول"), QColor(100, 150, 255));
    
    layout->addStretch();
}

void BodyAIModelsPanel::createAIButton(const QString &id, const QString &title, 
                                        const QString &description, const QColor &accent)
{
    GlassPanel *panel = new GlassPanel(this);
    panel->setFixedHeight(70);
    panel->setGlassOpacity(0.1);
    panel->setAccentColor(accent);
    panel->setCursor(Qt::PointingHandCursor);
    
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(15, 10, 15, 10);
    panelLayout->setSpacing(3);
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: bold;").arg(accent.name()));
    panelLayout->addWidget(titleLabel);
    
    QLabel *descLabel = new QLabel(description);
    descLabel->setStyleSheet("color: #AAAAAA; font-size: 10px;");
    panelLayout->addWidget(descLabel);
    
    // Click handler
    connect(panel, &GlassPanel::mousePressEvent, this, [this, id]() {
        if (id == "detect") emit detectBodyPartsClicked();
        else if (id == "enhance") emit autoEnhanceClicked();
        else if (id == "slim") emit slimBodyClicked();
        else if (id == "muscles") emit enhanceMusclesClicked();
        else if (id == "proportions") emit perfectProportionsClicked();
        else if (id == "hourglass") emit hourglassFigureClicked();
        else if (id == "athletic") emit athleticBuildClicked();
        else if (id == "tall") emit tallEffectClicked();
    });
    
    this->layout()->addWidget(panel);
}

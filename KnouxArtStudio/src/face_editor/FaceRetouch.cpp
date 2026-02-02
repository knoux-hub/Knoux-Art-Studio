#include "FaceRetouch.h"
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
#include <QColorDialog>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QtMath>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)
#define CP_ACCENT_GREEN QColor(0, 255, 128)
#define CP_ACCENT_ORANGE QColor(255, 128, 0)

FaceRetouch::FaceRetouch(QWidget *parent)
    : QWidget(parent)
    , m_canvas(nullptr)
    , m_featuresPanel(nullptr)
    , m_skinPanel(nullptr)
    , m_makeupPanel(nullptr)
    , m_aiPanel(nullptr)
    , m_isModified(false)
    , m_faceDetected(false)
    , m_brushSize(30)
    , m_isDrawing(false)
    , m_processingProgress(0.0f)
    , m_isProcessing(false)
    , m_previewTimer(nullptr)
{
    setupUI();
    setupConnections();
    
    m_defaultParams = m_params;
    
    m_previewTimer = new QTimer(this);
    m_previewTimer->setSingleShot(true);
    m_previewTimer->setInterval(100);
    connect(m_previewTimer, &QTimer::timeout, this, &FaceRetouch::applyAdjustments);
}

FaceRetouch::~FaceRetouch()
{
}

void FaceRetouch::setupUI()
{
    setObjectName("faceRetouch");
    
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
    
    m_detectBtn = new GlassButton("🔍 " + tr("كشف الوجه"), m_topToolbar);
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

void FaceRetouch::createToolbar()
{
    m_leftToolbar = new GlassPanel(this);
    m_leftToolbar->setFixedWidth(60);
    m_leftToolbar->setGlassOpacity(0.08);
    m_leftToolbar->setAccentColor(CP_ACCENT_CYAN);
    
    QVBoxLayout *layout = new QVBoxLayout(m_leftToolbar);
    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    QStringList tools = {"move", "select", "brush", "stamp", "heal"};
    QStringList icons = {"↖", "▭", "🖌", "⚫", "✚"};
    
    for (int i = 0; i < tools.size(); ++i) {
        GlassButton *btn = new GlassButton(icons[i], m_leftToolbar);
        btn->setFixedSize(45, 45);
        btn->setCheckable(true);
        btn->setToolTip(tools[i]);
        btn->setProperty("tool", tools[i]);
        if (i == 0) btn->setChecked(true);
        layout->addWidget(btn);
    }
    
    layout->addStretch();
}

void FaceRetouch::createCanvas()
{
    m_canvas = new FaceCanvas(this);
    m_canvas->setMinimumSize(400, 400);
}

void FaceRetouch::createSidePanels()
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
    
    GlassButton *featuresTab = new GlassButton(tr("ملامح"), m_rightPanel);
    featuresTab->setCheckable(true);
    featuresTab->setChecked(true);
    featuresTab->setFixedHeight(35);
    tabLayout->addWidget(featuresTab);
    
    GlassButton *skinTab = new GlassButton(tr("بشرة"), m_rightPanel);
    skinTab->setCheckable(true);
    skinTab->setFixedHeight(35);
    tabLayout->addWidget(skinTab);
    
    GlassButton *makeupTab = new GlassButton(tr("مكياج"), m_rightPanel);
    makeupTab->setCheckable(true);
    makeupTab->setFixedHeight(35);
    tabLayout->addWidget(makeupTab);
    
    GlassButton *aiTab = new GlassButton(tr("AI"), m_rightPanel);
    aiTab->setCheckable(true);
    aiTab->setFixedHeight(35);
    tabLayout->addWidget(aiTab);
    
    layout->addLayout(tabLayout);
    
    // Stacked panels
    QStackedWidget *stack = new QStackedWidget(m_rightPanel);
    
    m_featuresPanel = new FaceFeaturesPanel();
    m_skinPanel = new FaceSkinPanel();
    m_makeupPanel = new FaceMakeupPanel();
    m_aiPanel = new FaceAIModelsPanel();
    
    stack->addWidget(m_featuresPanel);
    stack->addWidget(m_skinPanel);
    stack->addWidget(m_makeupPanel);
    stack->addWidget(m_aiPanel);
    
    layout->addWidget(stack, 1);
    
    // Tab connections
    connect(featuresTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(0); });
    connect(skinTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(1); });
    connect(makeupTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(2); });
    connect(aiTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(3); });
}

void FaceRetouch::setupConnections()
{
    connect(m_undoBtn, &GlassButton::clicked, this, &FaceRetouch::resetAllAdjustments);
    connect(m_saveBtn, &GlassButton::clicked, this, &FaceRetouch::saveImage);
    connect(m_exportBtn, &GlassButton::clicked, this, &FaceRetouch::exportImage);
    connect(m_resetBtn, &GlassButton::clicked, this, &FaceRetouch::resetAllAdjustments);
    connect(m_detectBtn, &GlassButton::clicked, this, &FaceRetouch::aiDetectFace);
    
    connect(m_canvas, &FaceCanvas::mousePressed, this, &FaceRetouch::onCanvasMousePress);
    connect(m_canvas, &FaceCanvas::mouseMoved, this, &FaceRetouch::onCanvasMouseMove);
    connect(m_canvas, &FaceCanvas::mouseReleased, this, &FaceRetouch::onCanvasMouseRelease);
    
    connect(m_featuresPanel, &FaceFeaturesPanel::adjustmentChanged,
            this, &FaceRetouch::onAdjustmentChanged);
    connect(m_skinPanel, &FaceSkinPanel::adjustmentChanged,
            this, &FaceRetouch::onAdjustmentChanged);
    
    // AI Panel
    connect(m_aiPanel, &FaceAIModelsPanel::detectFaceClicked, this, &FaceRetouch::aiDetectFace);
    connect(m_aiPanel, &FaceAIModelsPanel::autoRetouchClicked, this, &FaceRetouch::aiAutoRetouch);
    connect(m_aiPanel, &FaceAIModelsPanel::smoothSkinClicked, this, &FaceRetouch::aiSmoothSkin);
    connect(m_aiPanel, &FaceAIModelsPanel::removeBlemishesClicked, this, &FaceRetouch::aiRemoveBlemishes);
    connect(m_aiPanel, &FaceAIModelsPanel::removeWrinklesClicked, this, &FaceRetouch::aiRemoveWrinkles);
    connect(m_aiPanel, &FaceAIModelsPanel::brightenEyesClicked, this, &FaceRetouch::aiBrightenEyes);
    connect(m_aiPanel, &FaceAIModelsPanel::whitenTeethClicked, this, &FaceRetouch::aiWhitenTeeth);
    connect(m_aiPanel, &FaceAIModelsPanel::sculptFaceClicked, this, &FaceRetouch::aiSculptFace);
    connect(m_aiPanel, &FaceAIModelsPanel::perfectPortraitClicked, this, &FaceRetouch::aiPerfectPortrait);
    connect(m_aiPanel, &FaceAIModelsPanel::naturalBeautyClicked, this, &FaceRetouch::aiNaturalBeauty);
    connect(m_aiPanel, &FaceAIModelsPanel::glamourLookClicked, this, &FaceRetouch::aiGlamourLook);
    connect(m_aiPanel, &FaceAIModelsPanel::professionalHeadshotClicked, this, &FaceRetouch::aiProfessionalHeadshot);
}

void FaceRetouch::openImage(const QString &path)
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
    m_faceDetected = false;
    
    m_canvas->setImage(m_currentImage);
    
    m_params = m_defaultParams;
    m_featuresPanel->resetAll();
    m_skinPanel->resetAll();
    
    emit statusMessage(tr("تم فتح الصورة - اضغط 'كشف الوجه' للبدء"));
}

void FaceRetouch::saveImage()
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

void FaceRetouch::exportImage()
{
    QString path = QFileDialog::getSaveFileName(this, tr("تصدير الصورة"), QString(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;WebP (*.webp)"));
    
    if (!path.isEmpty() && m_currentImage.save(path)) {
        emit statusMessage(tr("تم التصدير: %1").arg(path));
    }
}

void FaceRetouch::resetAllAdjustments()
{
    m_params = m_defaultParams;
    m_featuresPanel->resetAll();
    m_skinPanel->resetAll();
    applyAdjustments();
    emit statusMessage(tr("تم إعادة ضبط جميع التعديلات"));
}

// Adjustment setters
void FaceRetouch::setEyeSize(float value) { m_params.eyeSize = value; m_previewTimer->start(); }
void FaceRetouch::setEyeWidth(float value) { m_params.eyeWidth = value; m_previewTimer->start(); }
void FaceRetouch::setEyeHeight(float value) { m_params.eyeHeight = value; m_previewTimer->start(); }
void FaceRetouch::setEyeDistance(float value) { m_params.eyeDistance = value; m_previewTimer->start(); }
void FaceRetouch::setEyeTilt(float value) { m_params.eyeTilt = value; m_previewTimer->start(); }
void FaceRetouch::setEyeOpenness(float value) { m_params.eyeOpenness = value; m_previewTimer->start(); }
void FaceRetouch::setRemoveDarkCircles(float value) { m_params.removeDarkCircles = value; m_previewTimer->start(); }
void FaceRetouch::setBrightenEyes(float value) { m_params.brightenEyes = value; m_previewTimer->start(); }
void FaceRetouch::setEyebrowThickness(float value) { m_params.eyebrowThickness = value; m_previewTimer->start(); }
void FaceRetouch::setEyebrowHeight(float value) { m_params.eyebrowHeight = value; m_previewTimer->start(); }
void FaceRetouch::setEyebrowShape(float value) { m_params.eyebrowShape = value; m_previewTimer->start(); }
void FaceRetouch::setEyebrowArch(float value) { m_params.eyebrowArch = value; m_previewTimer->start(); }
void FaceRetouch::setNoseWidth(float value) { m_params.noseWidth = value; m_previewTimer->start(); }
void FaceRetouch::setNoseHeight(float value) { m_params.noseHeight = value; m_previewTimer->start(); }
void FaceRetouch::setNoseBridge(float value) { m_params.noseBridge = value; m_previewTimer->start(); }
void FaceRetouch::setNoseTip(float value) { m_params.noseTip = value; m_previewTimer->start(); }
void FaceRetouch::setNostrilSize(float value) { m_params.nostrilSize = value; m_previewTimer->start(); }
void FaceRetouch::setMouthWidth(float value) { m_params.mouthWidth = value; m_previewTimer->start(); }
void FaceRetouch::setLipThickness(float value) { m_params.lipThickness = value; m_previewTimer->start(); }
void FaceRetouch::setLipShape(float value) { m_params.lipShape = value; m_previewTimer->start(); }
void FaceRetouch::setSmile(float value) { m_params.smile = value; m_previewTimer->start(); }
void FaceRetouch::setTeethWhiteness(float value) { m_params.teethWhiteness = value; m_previewTimer->start(); }
void FaceRetouch::setFaceWidth(float value) { m_params.faceWidth = value; m_previewTimer->start(); }
void FaceRetouch::setFaceLength(float value) { m_params.faceLength = value; m_previewTimer->start(); }
void FaceRetouch::setJawWidth(float value) { m_params.jawWidth = value; m_previewTimer->start(); }
void FaceRetouch::setChinLength(float value) { m_params.chinLength = value; m_previewTimer->start(); }
void FaceRetouch::setCheekboneWidth(float value) { m_params.cheekboneWidth = value; m_previewTimer->start(); }
void FaceRetouch::setForeheadWidth(float value) { m_params.foreheadWidth = value; m_previewTimer->start(); }
void FaceRetouch::setSmoothness(float value) { m_params.smoothness = value; m_previewTimer->start(); }
void FaceRetouch::setFairness(float value) { m_params.fairness = value; m_previewTimer->start(); }
void FaceRetouch::setRemoveBlemishes(float value) { m_params.removeBlemishes = value; m_previewTimer->start(); }
void FaceRetouch::setRemoveWrinkles(float value) { m_params.removeWrinkles = value; m_previewTimer->start(); }
void FaceRetouch::setPoreRefinement(float value) { m_params.poreRefinement = value; m_previewTimer->start(); }
void FaceRetouch::setShineReduction(float value) { m_params.shineReduction = value; m_previewTimer->start(); }

void FaceRetouch::onAdjustmentChanged(const QString &name, float value)
{
    // Map name to setter
    if (name == "eyeSize") setEyeSize(value);
    else if (name == "eyeWidth") setEyeWidth(value);
    else if (name == "eyeHeight") setEyeHeight(value);
    else if (name == "eyeDistance") setEyeDistance(value);
    else if (name == "eyeTilt") setEyeTilt(value);
    else if (name == "eyeOpenness") setEyeOpenness(value);
    else if (name == "removeDarkCircles") setRemoveDarkCircles(value);
    else if (name == "brightenEyes") setBrightenEyes(value);
    else if (name == "eyebrowThickness") setEyebrowThickness(value);
    else if (name == "eyebrowHeight") setEyebrowHeight(value);
    else if (name == "eyebrowShape") setEyebrowShape(value);
    else if (name == "eyebrowArch") setEyebrowArch(value);
    else if (name == "noseWidth") setNoseWidth(value);
    else if (name == "noseHeight") setNoseHeight(value);
    else if (name == "noseBridge") setNoseBridge(value);
    else if (name == "noseTip") setNoseTip(value);
    else if (name == "nostrilSize") setNostrilSize(value);
    else if (name == "mouthWidth") setMouthWidth(value);
    else if (name == "lipThickness") setLipThickness(value);
    else if (name == "lipShape") setLipShape(value);
    else if (name == "smile") setSmile(value);
    else if (name == "teethWhiteness") setTeethWhiteness(value);
    else if (name == "faceWidth") setFaceWidth(value);
    else if (name == "faceLength") setFaceLength(value);
    else if (name == "jawWidth") setJawWidth(value);
    else if (name == "chinLength") setChinLength(value);
    else if (name == "cheekboneWidth") setCheekboneWidth(value);
    else if (name == "foreheadWidth") setForeheadWidth(value);
    else if (name == "smoothness") setSmoothness(value);
    else if (name == "fairness") setFairness(value);
    else if (name == "removeBlemishes") setRemoveBlemishes(value);
    else if (name == "removeWrinkles") setRemoveWrinkles(value);
    else if (name == "poreRefinement") setPoreRefinement(value);
    else if (name == "shineReduction") setShineReduction(value);
}

void FaceRetouch::applyAdjustments()
{
    if (m_originalImage.isNull()) return;
    
    m_currentImage = applyFaceTransformations(m_originalImage);
    m_canvas->setImage(m_currentImage);
    m_isModified = true;
    
    emit imageModified(true);
}

QImage FaceRetouch::applyFaceTransformations(const QImage &input)
{
    QImage result = input.copy();
    
    // Apply skin smoothing
    if (m_params.smoothness > 0) {
        result = smoothSkinInternal(result);
    }
    
    // Apply skin tone adjustment
    if (m_params.fairness > 0) {
        result = applySkinTone(result);
    }
    
    // Remove blemishes
    if (m_params.removeBlemishes > 0) {
        result = removeBlemishesInternal(result);
    }
    
    // Remove wrinkles
    if (m_params.removeWrinkles > 0) {
        result = removeWrinklesInternal(result);
    }
    
    // Brighten eyes
    if (m_params.brightenEyes > 0) {
        result = brightenEyesInternal(result);
    }
    
    // Whiten teeth
    if (m_params.teethWhiteness > 0) {
        result = whitenTeethInternal(result);
    }
    
    return result;
}

QImage FaceRetouch::smoothSkinInternal(const QImage &input)
{
    QImage result(input.size(), QImage::Format_ARGB32);
    
    int radius = int(m_params.smoothness * 5) + 1;
    
    for (int y = radius; y < input.height() - radius; ++y) {
        for (int x = radius; x < input.width() - radius; ++x) {
            int r = 0, g = 0, b = 0, count = 0;
            
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    QColor c = input.pixelColor(x + dx, y + dy);
                    r += c.red();
                    g += c.green();
                    b += c.blue();
                    count++;
                }
            }
            
            result.setPixelColor(x, y, QColor(r / count, g / count, b / count));
        }
    }
    
    return result;
}

QImage FaceRetouch::removeBlemishesInternal(const QImage &input)
{
    QImage result = input.copy();
    
    // Simple blemish detection and removal
    for (int y = 2; y < input.height() - 2; ++y) {
        for (int x = 2; x < input.width() - 2; ++x) {
            QColor c = input.pixelColor(x, y);
            
            // Check for dark spots
            int avg = (c.red() + c.green() + c.blue()) / 3;
            
            QColor left = input.pixelColor(x - 1, y);
            QColor right = input.pixelColor(x + 1, y);
            QColor up = input.pixelColor(x, y - 1);
            QColor down = input.pixelColor(x, y + 1);
            
            int surroundingAvg = (left.lightness() + right.lightness() + 
                                  up.lightness() + down.lightness()) / 4;
            
            if (surroundingAvg - avg > 30 * m_params.removeBlemishes) {
                // Replace with surrounding average
                int r = (left.red() + right.red() + up.red() + down.red()) / 4;
                int g = (left.green() + right.green() + up.green() + down.green()) / 4;
                int b = (left.blue() + right.blue() + up.blue() + down.blue()) / 4;
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

QImage FaceRetouch::removeWrinklesInternal(const QImage &input)
{
    // Similar to smooth but more targeted
    return smoothSkinInternal(input);
}

QImage FaceRetouch::brightenEyesInternal(const QImage &input)
{
    QImage result = input.copy();
    
    if (!m_faceDetected) return result;
    
    // Brighten eye areas
    QPointF leftEye = m_landmarks.leftEyeCenter;
    QPointF rightEye = m_landmarks.rightEyeCenter;
    
    int radius = 30;
    int brightness = int(m_params.brightenEyes * 30);
    
    for (const QPointF &eye : {leftEye, rightEye}) {
        for (int y = int(eye.y()) - radius; y < int(eye.y()) + radius; ++y) {
            for (int x = int(eye.x()) - radius; x < int(eye.x()) + radius; ++x) {
                if (x < 0 || x >= input.width() || y < 0 || y >= input.height()) continue;
                
                float dist = std::sqrt(std::pow(x - eye.x(), 2) + std::pow(y - eye.y(), 2));
                if (dist > radius) continue;
                
                float factor = 1.0f - (dist / radius);
                factor = factor * factor;
                
                QColor c = input.pixelColor(x, y);
                int r = qMin(255, c.red() + int(brightness * factor));
                int g = qMin(255, c.green() + int(brightness * factor));
                int b = qMin(255, c.blue() + int(brightness * factor));
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

QImage FaceRetouch::whitenTeethInternal(const QImage &input)
{
    QImage result = input.copy();
    
    if (!m_faceDetected) return result;
    
    // Whiten teeth area (mouth region)
    QPointF mouth = m_landmarks.mouthCenter;
    int radius = 25;
    int whiteness = int(m_params.teethWhiteness * 40);
    
    for (int y = int(mouth.y()) - radius; y < int(mouth.y()) + radius; ++y) {
        for (int x = int(mouth.x()) - radius; x < int(mouth.x()) + radius; ++x) {
            if (x < 0 || x >= input.width() || y < 0 || y >= input.height()) continue;
            
            QColor c = input.pixelColor(x, y);
            
            // Detect teeth (bright areas in mouth)
            if (c.lightness() > 180 && c.saturation() < 30) {
                int r = qMin(255, c.red() + whiteness);
                int g = qMin(255, c.green() + whiteness);
                int b = qMin(255, c.blue() + whiteness);
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }
    
    return result;
}

QImage FaceRetouch::applySkinTone(const QImage &input)
{
    QImage result(input.size(), QImage::Format_ARGB32);
    
    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            QColor c = input.pixelColor(x, y);
            
            // Adjust towards fairer tone
            int adjust = int(m_params.fairness * 20);
            int r = qMin(255, c.red() + adjust);
            int g = qMin(255, c.green() + adjust);
            int b = qMin(255, c.blue() + adjust);
            
            result.setPixelColor(x, y, QColor(r, g, b));
        }
    }
    
    return result;
}

// AI Operations
void FaceRetouch::aiDetectFace()
{
    if (m_originalImage.isNull()) {
        emit statusMessage(tr("افتح صورة أولاً"));
        return;
    }
    
    emit processingStarted(tr("كشف الوجه"));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_isProcessing = true;
    
    QTimer *timer = new QTimer(this);
    int progress = 0;
    connect(timer, &QTimer::timeout, this, [this, &progress, timer]() {
        progress += 5;
        m_progressBar->setValue(progress);
        emit processingProgressChanged(progress);
        
        if (progress >= 100) {
            timer->stop();
            timer->deleteLater();
            
            detectFaceInternal();
            
            m_progressBar->setVisible(false);
            m_isProcessing = false;
            m_faceDetected = true;
            
            emit processingFinished(tr("تم كشف الوجه"));
            emit faceDetected(m_landmarks);
            emit statusMessage(tr("تم كشف الوجه - يمكنك الآن التعديل"));
        }
    });
    timer->start(50);
}

void FaceRetouch::detectFaceInternal()
{
    int w = m_originalImage.width();
    int h = m_originalImage.height();
    
    // Simulate face landmarks detection
    // Face center
    m_landmarks.faceCenter = QPointF(w * 0.5, h * 0.4);
    
    // Eyes
    m_landmarks.leftEyeCenter = QPointF(w * 0.4, h * 0.35);
    m_landmarks.rightEyeCenter = QPointF(w * 0.6, h * 0.35);
    m_landmarks.leftEyeOpenness = 0.8f;
    m_landmarks.rightEyeOpenness = 0.8f;
    
    // Eyebrows
    m_landmarks.leftEyebrow = {
        QPointF(w * 0.35, h * 0.30),
        QPointF(w * 0.40, h * 0.28),
        QPointF(w * 0.45, h * 0.30)
    };
    m_landmarks.rightEyebrow = {
        QPointF(w * 0.55, h * 0.30),
        QPointF(w * 0.60, h * 0.28),
        QPointF(w * 0.65, h * 0.30)
    };
    
    // Nose
    m_landmarks.noseTip = QPointF(w * 0.5, h * 0.48);
    
    // Mouth
    m_landmarks.mouthCenter = QPointF(w * 0.5, h * 0.58);
    m_landmarks.mouthOpenness = 0.0f;
    m_landmarks.smileFactor = 0.3f;
    
    // Face contour
    m_landmarks.faceContour = {
        QPointF(w * 0.35, h * 0.25),
        QPointF(w * 0.65, h * 0.25),
        QPointF(w * 0.70, h * 0.40),
        QPointF(w * 0.65, h * 0.65),
        QPointF(w * 0.50, h * 0.75),
        QPointF(w * 0.35, h * 0.65),
        QPointF(w * 0.30, h * 0.40)
    };
    
    m_canvas->setLandmarks(m_landmarks);
}

void FaceRetouch::aiAutoRetouch()
{
    if (!m_faceDetected) {
        aiDetectFace();
        return;
    }
    
    emit processingStarted(tr("ريتوش تلقائي"));
    
    m_params.smoothness = 0.4f;
    m_params.removeBlemishes = 0.6f;
    m_params.removeWrinkles = 0.3f;
    m_params.brightenEyes = 0.4f;
    m_params.teethWhiteness = 0.3f;
    
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    
    emit processingFinished(tr("تم الريتوش"));
    emit statusMessage(tr("تم الريتوش التلقائي"));
}

void FaceRetouch::aiSmoothSkin()
{
    m_params.smoothness = 0.5f;
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تنعيم البشرة"));
}

void FaceRetouch::aiRemoveBlemishes()
{
    m_params.removeBlemishes = 0.7f;
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم إزالة العيوب"));
}

void FaceRetouch::aiRemoveWrinkles()
{
    m_params.removeWrinkles = 0.6f;
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم إزالة التجاعيد"));
}

void FaceRetouch::aiBrightenEyes()
{
    m_params.brightenEyes = 0.5f;
    m_params.removeDarkCircles = 0.5f;
    m_featuresPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تفتيح العينين"));
}

void FaceRetouch::aiWhitenTeeth()
{
    m_params.teethWhiteness = 0.6f;
    m_featuresPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تبييض الأسنان"));
}

void FaceRetouch::aiSculptFace()
{
    m_params.faceWidth = 0.95f;
    m_params.jawWidth = 0.92f;
    m_params.cheekboneWidth = 1.03f;
    m_params.chinLength = 0.98f;
    m_featuresPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم نحت الوجه"));
}

void FaceRetouch::aiPerfectPortrait()
{
    // Apply all enhancements
    m_params.smoothness = 0.4f;
    m_params.removeBlemishes = 0.6f;
    m_params.removeWrinkles = 0.3f;
    m_params.brightenEyes = 0.4f;
    m_params.teethWhiteness = 0.3f;
    m_params.faceWidth = 0.95f;
    m_params.jawWidth = 0.92f;
    m_params.eyeSize = 1.05f;
    
    m_featuresPanel->setValues(m_params);
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تطبيق البورتريه المثالي"));
}

void FaceRetouch::aiNaturalBeauty()
{
    m_params.smoothness = 0.25f;
    m_params.removeBlemishes = 0.4f;
    m_params.brightenEyes = 0.25f;
    m_params.teethWhiteness = 0.15f;
    m_params.lipThickness = 1.05f;
    
    m_featuresPanel->setValues(m_params);
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تطبيق الجمال الطبيعي"));
}

void FaceRetouch::aiGlamourLook()
{
    m_params.smoothness = 0.5f;
    m_params.fairness = 0.2f;
    m_params.brightenEyes = 0.6f;
    m_params.teethWhiteness = 0.5f;
    m_params.eyeSize = 1.1f;
    m_params.lipThickness = 1.15f;
    m_params.cheekboneWidth = 1.08f;
    
    m_featuresPanel->setValues(m_params);
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تطبيق مظهر الجلامور"));
}

void FaceRetouch::aiProfessionalHeadshot()
{
    m_params.smoothness = 0.3f;
    m_params.removeBlemishes = 0.5f;
    m_params.brightenEyes = 0.35f;
    m_params.teethWhiteness = 0.25f;
    m_params.shineReduction = 0.5f;
    m_params.faceWidth = 0.98f;
    
    m_featuresPanel->setValues(m_params);
    m_skinPanel->setValues(m_params);
    applyAdjustments();
    emit statusMessage(tr("تم تطبيق صورة الاحترافية"));
}

void FaceRetouch::onCanvasMousePress(const QPoint &pos)
{
    m_isDrawing = true;
    m_lastPos = pos;
}

void FaceRetouch::onCanvasMouseMove(const QPoint &pos)
{
    if (!m_isDrawing) return;
    m_lastPos = pos;
}

void FaceRetouch::onCanvasMouseRelease(const QPoint &pos)
{
    m_isDrawing = false;
}

void FaceRetouch::setProcessingProgress(float progress)
{
    m_processingProgress = progress;
    m_progressBar->setValue(int(progress * 100));
}

void FaceRetouch::keyPressEvent(QKeyEvent *event)
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

void FaceRetouch::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

// ==================== FaceCanvas Implementation ====================

FaceCanvas::FaceCanvas(QWidget *parent)
    : QWidget(parent)
    , m_zoom(1.0f)
    , m_showOverlay(true)
    , m_showGrid(false)
    , m_showLandmarks(true)
{
    setMouseTracking(true);
    setMinimumSize(400, 400);
}

void FaceCanvas::setImage(const QImage &image)
{
    m_image = image;
    update();
}

void FaceCanvas::setLandmarks(const FaceLandmarks &landmarks)
{
    m_landmarks = landmarks;
    update();
}

void FaceCanvas::setFeatures(const QVector<FaceFeature> &features)
{
    m_features = features;
    update();
}

void FaceCanvas::setShowOverlay(bool show)
{
    m_showOverlay = show;
    update();
}

void FaceCanvas::setShowGrid(bool show)
{
    m_showGrid = show;
    update();
}

void FaceCanvas::setShowLandmarks(bool show)
{
    m_showLandmarks = show;
    update();
}

void FaceCanvas::setSelectedFeature(const QString &feature)
{
    m_selectedFeature = feature;
    update();
}

void FaceCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), QColor(15, 15, 25));
    
    if (m_showGrid) {
        painter.setPen(QPen(QColor(255, 255, 255, 20), 1));
        for (int x = 0; x < width(); x += 50) painter.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += 50) painter.drawLine(0, y, width(), y);
    }
    
    drawImage(painter);
    
    if (m_showOverlay && m_showLandmarks) {
        drawLandmarks(painter);
    }
    
    drawOverlay(painter);
}

void FaceCanvas::drawImage(QPainter &painter)
{
    if (m_image.isNull()) {
        painter.setPen(QPen(QColor(100, 100, 100)));
        painter.drawText(rect(), Qt::AlignCenter, tr("افتح صورة للبدء"));
        return;
    }
    
    QRect imgRect = imageRect();
    painter.drawImage(imgRect, m_image);
}

void FaceCanvas::drawLandmarks(QPainter &painter)
{
    if (!m_showLandmarks) return;
    
    painter.setPen(QPen(CP_ACCENT_CYAN, 2));
    painter.setBrush(Qt::NoBrush);
    
    // Draw face contour
    if (!m_landmarks.faceContour.isEmpty()) {
        QPolygonF contour;
        for (const QPointF &p : m_landmarks.faceContour) {
            contour.append(mapToImage(p));
        }
        painter.drawPolygon(contour);
    }
    
    // Draw eyes
    painter.setBrush(CP_ACCENT_CYAN);
    QPointF leftEye = mapToImage(m_landmarks.leftEyeCenter);
    QPointF rightEye = mapToImage(m_landmarks.rightEyeCenter);
    painter.drawEllipse(leftEye, 5, 5);
    painter.drawEllipse(rightEye, 5, 5);
    
    // Draw nose tip
    QPointF nose = mapToImage(m_landmarks.noseTip);
    painter.setBrush(CP_ACCENT_PINK);
    painter.drawEllipse(nose, 5, 5);
    
    // Draw mouth
    QPointF mouth = mapToImage(m_landmarks.mouthCenter);
    painter.setBrush(CP_ACCENT_GREEN);
    painter.drawEllipse(mouth, 5, 5);
}

void FaceCanvas::drawOverlay(QPainter &painter)
{
    painter.setPen(QPen(QColor(200, 200, 200)));
    painter.drawText(10, 20, QString("%1%").arg(int(m_zoom * 100)));
}

QRect FaceCanvas::imageRect() const
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

QPointF FaceCanvas::mapToImage(const QPointF &widgetPos) const
{
    QRect imgRect = imageRect();
    float scaleX = float(imgRect.width()) / m_image.width();
    float scaleY = float(imgRect.height()) / m_image.height();
    
    return QPointF(
        imgRect.left() + widgetPos.x() * scaleX,
        imgRect.top() + widgetPos.y() * scaleY
    );
}

void FaceCanvas::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event->pos());
}

void FaceCanvas::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event->pos());
}

void FaceCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event->pos());
}

void FaceCanvas::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() > 0 ? 1.1f : 0.9f;
    m_zoom *= delta;
    m_zoom = qBound(0.1f, m_zoom, 5.0f);
    update();
}

// ==================== FaceFeaturesPanel Implementation ====================

FaceFeaturesPanel::FaceFeaturesPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void FaceFeaturesPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    // Auto enhance button
    GlassButton *autoBtn = new GlassButton(tr("✨ تحسين تلقائي"), this);
    autoBtn->setFixedHeight(40);
    autoBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(autoBtn, &GlassButton::clicked, this, &FaceFeaturesPanel::autoEnhanceClicked);
    mainLayout->addWidget(autoBtn);
    
    mainLayout->addSpacing(10);
    
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(15);
    
    // Eyes section
    QGroupBox *eyesGroup = new QGroupBox(tr("العينين"), this);
    QVBoxLayout *eyesLayout = new QVBoxLayout(eyesGroup);
    createSlider("eyeSize", tr("الحجم"), 0.8f, 1.2f, 1.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["eyeSize"]);
    createSlider("eyeWidth", tr("العرض"), 0.8f, 1.2f, 1.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["eyeWidth"]);
    createSlider("eyeHeight", tr("الارتفاع"), 0.8f, 1.2f, 1.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["eyeHeight"]);
    createSlider("eyeDistance", tr("المسافة"), 0.8f, 1.2f, 1.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["eyeDistance"]);
    createSlider("eyeOpenness", tr("الانفتاح"), 0.8f, 1.2f, 1.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["eyeOpenness"]);
    createSlider("brightenEyes", tr("التفتيح"), 0.0f, 1.0f, 0.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["brightenEyes"]);
    createSlider("removeDarkCircles", tr("إزالة الهالات"), 0.0f, 1.0f, 0.0f)->setParent(eyesGroup);
    eyesLayout->addWidget(m_sliders["removeDarkCircles"]);
    containerLayout->addWidget(eyesGroup);
    
    // Nose section
    QGroupBox *noseGroup = new QGroupBox(tr("الأنف"), this);
    QVBoxLayout *noseLayout = new QVBoxLayout(noseGroup);
    createSlider("noseWidth", tr("العرض"), 0.7f, 1.3f, 1.0f)->setParent(noseGroup);
    noseLayout->addWidget(m_sliders["noseWidth"]);
    createSlider("noseHeight", tr("الطول"), 0.8f, 1.2f, 1.0f)->setParent(noseGroup);
    noseLayout->addWidget(m_sliders["noseHeight"]);
    createSlider("noseBridge", tr("الجسر"), 0.8f, 1.2f, 1.0f)->setParent(noseGroup);
    noseLayout->addWidget(m_sliders["noseBridge"]);
    createSlider("noseTip", tr("ال tip"), 0.8f, 1.2f, 1.0f)->setParent(noseGroup);
    noseLayout->addWidget(m_sliders["noseTip"]);
    containerLayout->addWidget(noseGroup);
    
    // Mouth section
    QGroupBox *mouthGroup = new QGroupBox(tr("الفم"), this);
    QVBoxLayout *mouthLayout = new QVBoxLayout(mouthGroup);
    createSlider("mouthWidth", tr("العرض"), 0.8f, 1.2f, 1.0f)->setParent(mouthGroup);
    mouthLayout->addWidget(m_sliders["mouthWidth"]);
    createSlider("lipThickness", tr("سمك الشفاه"), 0.7f, 1.4f, 1.0f)->setParent(mouthGroup);
    mouthLayout->addWidget(m_sliders["lipThickness"]);
    createSlider("smile", tr("الابتسامة"), 0.0f, 1.0f, 0.0f)->setParent(mouthGroup);
    mouthLayout->addWidget(m_sliders["smile"]);
    createSlider("teethWhiteness", tr("تبييض الأسنان"), 0.0f, 1.0f, 0.0f)->setParent(mouthGroup);
    mouthLayout->addWidget(m_sliders["teethWhiteness"]);
    containerLayout->addWidget(mouthGroup);
    
    // Face shape section
    QGroupBox *faceGroup = new QGroupBox(tr("شكل الوجه"), this);
    QVBoxLayout *faceLayout = new QVBoxLayout(faceGroup);
    createSlider("faceWidth", tr("عرض الوجه"), 0.8f, 1.2f, 1.0f)->setParent(faceGroup);
    faceLayout->addWidget(m_sliders["faceWidth"]);
    createSlider("faceLength", tr("طول الوجه"), 0.9f, 1.15f, 1.0f)->setParent(faceGroup);
    faceLayout->addWidget(m_sliders["faceLength"]);
    createSlider("jawWidth", tr("عرض الفك"), 0.8f, 1.2f, 1.0f)->setParent(faceGroup);
    faceLayout->addWidget(m_sliders["jawWidth"]);
    createSlider("chinLength", tr("طول الذقن"), 0.8f, 1.2f, 1.0f)->setParent(faceGroup);
    faceLayout->addWidget(m_sliders["chinLength"]);
    createSlider("cheekboneWidth", tr("عرض عظام الخد"), 0.8f, 1.2f, 1.0f)->setParent(faceGroup);
    faceLayout->addWidget(m_sliders["cheekboneWidth"]);
    containerLayout->addWidget(faceGroup);
    
    containerLayout->addStretch();
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea, 1);
    
    // Reset button
    GlassButton *resetBtn = new GlassButton(tr("إعادة ضبط"), this);
    resetBtn->setFixedHeight(35);
    connect(resetBtn, &GlassButton::clicked, this, &FaceFeaturesPanel::resetClicked);
    mainLayout->addWidget(resetBtn);
}

QSlider *FaceFeaturesPanel::createSlider(const QString &name, const QString &label, 
                                          float min, float max, float defaultValue)
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
    
    return slider;
}

void FaceFeaturesPanel::setValues(const FaceAdjustParams &params)
{
    m_sliders["eyeSize"]->setValue(int(params.eyeSize * 100));
    m_sliders["eyeWidth"]->setValue(int(params.eyeWidth * 100));
    m_sliders["eyeHeight"]->setValue(int(params.eyeHeight * 100));
    m_sliders["eyeDistance"]->setValue(int(params.eyeDistance * 100));
    m_sliders["eyeOpenness"]->setValue(int(params.eyeOpenness * 100));
    m_sliders["brightenEyes"]->setValue(int(params.brightenEyes * 100));
    m_sliders["removeDarkCircles"]->setValue(int(params.removeDarkCircles * 100));
    m_sliders["noseWidth"]->setValue(int(params.noseWidth * 100));
    m_sliders["noseHeight"]->setValue(int(params.noseHeight * 100));
    m_sliders["noseBridge"]->setValue(int(params.noseBridge * 100));
    m_sliders["noseTip"]->setValue(int(params.noseTip * 100));
    m_sliders["mouthWidth"]->setValue(int(params.mouthWidth * 100));
    m_sliders["lipThickness"]->setValue(int(params.lipThickness * 100));
    m_sliders["smile"]->setValue(int(params.smile * 100));
    m_sliders["teethWhiteness"]->setValue(int(params.teethWhiteness * 100));
    m_sliders["faceWidth"]->setValue(int(params.faceWidth * 100));
    m_sliders["faceLength"]->setValue(int(params.faceLength * 100));
    m_sliders["jawWidth"]->setValue(int(params.jawWidth * 100));
    m_sliders["chinLength"]->setValue(int(params.chinLength * 100));
    m_sliders["cheekboneWidth"]->setValue(int(params.cheekboneWidth * 100));
}

FaceAdjustParams FaceFeaturesPanel::values() const
{
    FaceAdjustParams params;
    params.eyeSize = m_sliders["eyeSize"]->value() / 100.0f;
    params.eyeWidth = m_sliders["eyeWidth"]->value() / 100.0f;
    params.eyeHeight = m_sliders["eyeHeight"]->value() / 100.0f;
    params.eyeDistance = m_sliders["eyeDistance"]->value() / 100.0f;
    params.eyeOpenness = m_sliders["eyeOpenness"]->value() / 100.0f;
    params.brightenEyes = m_sliders["brightenEyes"]->value() / 100.0f;
    params.removeDarkCircles = m_sliders["removeDarkCircles"]->value() / 100.0f;
    params.noseWidth = m_sliders["noseWidth"]->value() / 100.0f;
    params.noseHeight = m_sliders["noseHeight"]->value() / 100.0f;
    params.noseBridge = m_sliders["noseBridge"]->value() / 100.0f;
    params.noseTip = m_sliders["noseTip"]->value() / 100.0f;
    params.mouthWidth = m_sliders["mouthWidth"]->value() / 100.0f;
    params.lipThickness = m_sliders["lipThickness"]->value() / 100.0f;
    params.smile = m_sliders["smile"]->value() / 100.0f;
    params.teethWhiteness = m_sliders["teethWhiteness"]->value() / 100.0f;
    params.faceWidth = m_sliders["faceWidth"]->value() / 100.0f;
    params.faceLength = m_sliders["faceLength"]->value() / 100.0f;
    params.jawWidth = m_sliders["jawWidth"]->value() / 100.0f;
    params.chinLength = m_sliders["chinLength"]->value() / 100.0f;
    params.cheekboneWidth = m_sliders["cheekboneWidth"]->value() / 100.0f;
    return params;
}

void FaceFeaturesPanel::resetAll()
{
    for (auto it = m_sliders.begin(); it != m_sliders.end(); ++it) {
        it.value()->setValue(100);
    }
}

// ==================== FaceSkinPanel Implementation ====================

FaceSkinPanel::FaceSkinPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void FaceSkinPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(15);
    
    // Skin smoothing section
    QGroupBox *smoothGroup = new QGroupBox(tr("تنعيم البشرة"), this);
    QVBoxLayout *smoothLayout = new QVBoxLayout(smoothGroup);
    createSlider("smoothness", tr("درجة التنعيم"), 0.0f, 1.0f, 0.0f)->setParent(smoothGroup);
    smoothLayout->addWidget(m_sliders["smoothness"]);
    createSlider("poreRefinement", tr("تصغير المسام"), 0.0f, 1.0f, 0.0f)->setParent(smoothGroup);
    smoothLayout->addWidget(m_sliders["poreRefinement"]);
    containerLayout->addWidget(smoothGroup);
    
    // Blemishes section
    QGroupBox *blemishGroup = new QGroupBox(tr("إزالة العيوب"), this);
    QVBoxLayout *blemishLayout = new QVBoxLayout(blemishGroup);
    createSlider("removeBlemishes", tr("إزالة البقع"), 0.0f, 1.0f, 0.0f)->setParent(blemishGroup);
    blemishLayout->addWidget(m_sliders["removeBlemishes"]);
    createSlider("removeWrinkles", tr("إزالة التجاعيد"), 0.0f, 1.0f, 0.0f)->setParent(blemishGroup);
    blemishLayout->addWidget(m_sliders["removeWrinkles"]);
    containerLayout->addWidget(blemishGroup);
    
    // Tone section
    QGroupBox *toneGroup = new QGroupBox(tr("نغمة البشرة"), this);
    QVBoxLayout *toneLayout = new QVBoxLayout(toneGroup);
    createSlider("fairness", tr("تفتيح"), 0.0f, 0.5f, 0.0f)->setParent(toneGroup);
    toneLayout->addWidget(m_sliders["fairness"]);
    createSlider("shineReduction", tr("تقليل اللمعان"), 0.0f, 1.0f, 0.0f)->setParent(toneGroup);
    toneLayout->addWidget(m_sliders["shineReduction"]);
    containerLayout->addWidget(toneGroup);
    
    containerLayout->addStretch();
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea, 1);
    
    // Reset button
    GlassButton *resetBtn = new GlassButton(tr("إعادة ضبط"), this);
    resetBtn->setFixedHeight(35);
    connect(resetBtn, &GlassButton::clicked, this, &FaceSkinPanel::resetClicked);
    mainLayout->addWidget(resetBtn);
}

QSlider *FaceSkinPanel::createSlider(const QString &name, const QString &label, 
                                      float min, float max, float defaultValue)
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
    
    return slider;
}

void FaceSkinPanel::setValues(const FaceAdjustParams &params)
{
    m_sliders["smoothness"]->setValue(int(params.smoothness * 100));
    m_sliders["removeBlemishes"]->setValue(int(params.removeBlemishes * 100));
    m_sliders["removeWrinkles"]->setValue(int(params.removeWrinkles * 100));
    m_sliders["fairness"]->setValue(int(params.fairness * 100));
    m_sliders["poreRefinement"]->setValue(int(params.poreRefinement * 100));
    m_sliders["shineReduction"]->setValue(int(params.shineReduction * 100));
}

void FaceSkinPanel::resetAll()
{
    for (auto it = m_sliders.begin(); it != m_sliders.end(); ++it) {
        it.value()->setValue(0);
    }
}

// ==================== FaceMakeupPanel Implementation ====================

FaceMakeupPanel::FaceMakeupPanel(QWidget *parent)
    : QWidget(parent)
    , m_lipstickColor(QColor(200, 50, 80))
    , m_blushColor(QColor(255, 150, 150))
    , m_eyeshadowColor(QColor(150, 100, 80))
{
    setupUI();
}

void FaceMakeupPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(15);
    
    // Lipstick section
    QGroupBox *lipstickGroup = new QGroupBox(tr("أحمر الشفاه"), this);
    QVBoxLayout *lipstickLayout = new QVBoxLayout(lipstickGroup);
    
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel(tr("اللون:")));
    QPushButton *colorBtn = new QPushButton();
    colorBtn->setFixedSize(40, 25);
    colorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(m_lipstickColor.name()));
    connect(colorBtn, &QPushButton::clicked, this, [this, colorBtn]() {
        QColor c = QColorDialog::getColor(m_lipstickColor, this, tr("اختيار لون"));
        if (c.isValid()) {
            m_lipstickColor = c;
            colorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(c.name()));
            emit lipstickColorChanged(c);
        }
    });
    colorLayout->addWidget(colorBtn);
    colorLayout->addStretch();
    lipstickLayout->addLayout(colorLayout);
    
    QSlider *lipstickSlider = new QSlider(Qt::Horizontal);
    lipstickSlider->setRange(0, 100);
    lipstickSlider->setValue(0);
    connect(lipstickSlider, &QSlider::valueChanged, this, &FaceMakeupPanel::lipstickIntensityChanged);
    lipstickLayout->addWidget(lipstickSlider);
    
    containerLayout->addWidget(lipstickGroup);
    
    // Blush section
    QGroupBox *blushGroup = new QGroupBox(tr("أحمر الخدود"), this);
    QVBoxLayout *blushLayout = new QVBoxLayout(blushGroup);
    
    QHBoxLayout *blushColorLayout = new QHBoxLayout();
    blushColorLayout->addWidget(new QLabel(tr("اللون:")));
    QPushButton *blushColorBtn = new QPushButton();
    blushColorBtn->setFixedSize(40, 25);
    blushColorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(m_blushColor.name()));
    connect(blushColorBtn, &QPushButton::clicked, this, [this, blushColorBtn]() {
        QColor c = QColorDialog::getColor(m_blushColor, this, tr("اختيار لون"));
        if (c.isValid()) {
            m_blushColor = c;
            blushColorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(c.name()));
            emit blushColorChanged(c);
        }
    });
    blushColorLayout->addWidget(blushColorBtn);
    blushColorLayout->addStretch();
    blushLayout->addLayout(blushColorLayout);
    
    QSlider *blushSlider = new QSlider(Qt::Horizontal);
    blushSlider->setRange(0, 100);
    blushSlider->setValue(0);
    connect(blushSlider, &QSlider::valueChanged, this, &FaceMakeupPanel::blushIntensityChanged);
    blushLayout->addWidget(blushSlider);
    
    containerLayout->addWidget(blushGroup);
    
    // Eyeshadow section
    QGroupBox *shadowGroup = new QGroupBox(tr("ظلال العيون"), this);
    QVBoxLayout *shadowLayout = new QVBoxLayout(shadowGroup);
    
    QHBoxLayout *shadowColorLayout = new QHBoxLayout();
    shadowColorLayout->addWidget(new QLabel(tr("اللون:")));
    QPushButton *shadowColorBtn = new QPushButton();
    shadowColorBtn->setFixedSize(40, 25);
    shadowColorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(m_eyeshadowColor.name()));
    connect(shadowColorBtn, &QPushButton::clicked, this, [this, shadowColorBtn]() {
        QColor c = QColorDialog::getColor(m_eyeshadowColor, this, tr("اختيار لون"));
        if (c.isValid()) {
            m_eyeshadowColor = c;
            shadowColorBtn->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(c.name()));
            emit eyeshadowColorChanged(c);
        }
    });
    shadowColorLayout->addWidget(shadowColorBtn);
    shadowColorLayout->addStretch();
    shadowLayout->addLayout(shadowColorLayout);
    
    QSlider *shadowSlider = new QSlider(Qt::Horizontal);
    shadowSlider->setRange(0, 100);
    shadowSlider->setValue(0);
    connect(shadowSlider, &QSlider::valueChanged, this, &FaceMakeupPanel::eyeshadowIntensityChanged);
    shadowLayout->addWidget(shadowSlider);
    
    containerLayout->addWidget(shadowGroup);
    
    // Eyeliner section
    QGroupBox *linerGroup = new QGroupBox(tr("آي لاينر"), this);
    QVBoxLayout *linerLayout = new QVBoxLayout(linerGroup);
    QSlider *linerSlider = new QSlider(Qt::Horizontal);
    linerSlider->setRange(0, 100);
    linerSlider->setValue(0);
    connect(linerSlider, &QSlider::valueChanged, this, &FaceMakeupPanel::eyelinerIntensityChanged);
    linerLayout->addWidget(linerSlider);
    containerLayout->addWidget(linerGroup);
    
    // Mascara section
    QGroupBox *mascaraGroup = new QGroupBox(tr("ماسكارا"), this);
    QVBoxLayout *mascaraLayout = new QVBoxLayout(mascaraGroup);
    QSlider *mascaraSlider = new QSlider(Qt::Horizontal);
    mascaraSlider->setRange(0, 100);
    mascaraSlider->setValue(0);
    connect(mascaraSlider, &QSlider::valueChanged, this, &FaceMakeupPanel::mascaraIntensityChanged);
    mascaraLayout->addWidget(mascaraSlider);
    containerLayout->addWidget(mascaraGroup);
    
    containerLayout->addStretch();
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea, 1);
}

// ==================== FaceAIModelsPanel Implementation ====================

FaceAIModelsPanel::FaceAIModelsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void FaceAIModelsPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);
    
    QLabel *titleLabel = new QLabel(tr("🤖 أدوات AI"));
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);
    
    createAIButton("detect", tr("🔍 كشف الوجه"), tr("كشف تلقائي لمعالم الوجه"), CP_ACCENT_CYAN);
    createAIButton("autoretouch", tr("✨ ريتوش تلقائي"), tr("ريتوش شامل تلقائي"), CP_ACCENT_GREEN);
    createAIButton("smooth", tr("🧴 تنعيم البشرة"), tr("تنعيم ناعم للبشرة"), QColor(255, 200, 150));
    createAIButton("blemishes", tr("🎯 إزالة العيوب"), tr("إزالة البقع والشوائب"), CP_ACCENT_PINK);
    createAIButton("wrinkles", tr("👤 إزالة التجاعيد"), tr("إزالة خطوط التجاعيد"), QColor(200, 150, 255));
    createAIButton("brighten", tr("✨ تفتيح العينين"), tr("تفتيح وإشراق العينين"), QColor(255, 255, 150));
    createAIButton("whiten", tr("🦷 تبييض الأسنان"), tr("تبييض الأسنان"), QColor(240, 240, 255));
    createAIButton("sculpt", tr("🎨 نحت الوجه"), tr("نحت وتعديل شكل الوجه"), CP_ACCENT_ORANGE);
    createAIButton("portrait", tr("📸 بورتريه مثالي"), tr("بورتريه احترافي متكامل"), CP_ACCENT_PURPLE);
    createAIButton("natural", tr("🌸 جمال طبيعي"), tr("مظهر طبيعي متناسق"), QColor(150, 255, 150));
    createAIButton("glamour", tr("💎 جلامور"), tr("مظهر جلامور أنيق"), QColor(255, 150, 200));
    createAIButton("headshot", tr("👔 صورة احترافية"), tr("صورة رسمية احترافية"), QColor(150, 200, 255));
    
    layout->addStretch();
}

void FaceAIModelsPanel::createAIButton(const QString &id, const QString &title, 
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
    
    connect(panel, &GlassPanel::mousePressEvent, this, [this, id]() {
        if (id == "detect") emit detectFaceClicked();
        else if (id == "autoretouch") emit autoRetouchClicked();
        else if (id == "smooth") emit smoothSkinClicked();
        else if (id == "blemishes") emit removeBlemishesClicked();
        else if (id == "wrinkles") emit removeWrinklesClicked();
        else if (id == "brighten") emit brightenEyesClicked();
        else if (id == "whiten") emit whitenTeethClicked();
        else if (id == "sculpt") emit sculptFaceClicked();
        else if (id == "portrait") emit perfectPortraitClicked();
        else if (id == "natural") emit naturalBeautyClicked();
        else if (id == "glamour") emit glamourLookClicked();
        else if (id == "headshot") emit professionalHeadshotClicked();
    });
    
    this->layout()->addWidget(panel);
}

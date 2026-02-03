#include "PhotoEditor.h"
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
#include <QClipboard>
#include <QMimeData>
#include <QImageReader>
#include <QImageWriter>
#include <QBuffer>
#include <QtMath>
#include <QDebug>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)

PhotoEditor::PhotoEditor(QWidget *parent)
    : QWidget(parent)
    , m_zoomLevel(1.0f)
    , m_brushSize(20)
    , m_brushHardness(80)
    , m_brushShape("circle")
    , m_currentTool("brush")
    , m_primaryColor(Qt::black)
    , m_secondaryColor(Qt::white)
    , m_currentLayerIndex(0)
    , m_isModified(false)
    , m_isDrawing(false)
    , m_hasSelection(false)
    , m_isAIProcessing(false)
    , m_canvas(nullptr)
    , m_layersPanel(nullptr)
    , m_adjustmentsPanel(nullptr)
    , m_toolsPanel(nullptr)
    , m_aiPanel(nullptr)
    , m_aiProgressTimer(nullptr)
{
    setupUI();
    setupConnections();
    setupShortcuts();

    // Add initial layer
    addLayer(tr("الخلفية"));
}

PhotoEditor::~PhotoEditor()
{
}

void PhotoEditor::setupUI()
{
    setObjectName("photoEditor");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Left toolbar
    createToolbar();
    mainLayout->addWidget(m_leftToolbar, 0);

    // Center area with canvas
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
    m_undoBtn->setToolTip(tr("تراجع (Ctrl+Z)"));
    topLayout->addWidget(m_undoBtn);

    m_redoBtn = new GlassButton("↪", m_topToolbar);
    m_redoBtn->setFixedSize(40, 40);
    m_redoBtn->setToolTip(tr("إعادة (Ctrl+Y)"));
    topLayout->addWidget(m_redoBtn);

    topLayout->addSpacing(20);

    m_saveBtn = new GlassButton("💾", m_topToolbar);
    m_saveBtn->setFixedSize(40, 40);
    m_saveBtn->setToolTip(tr("حفظ (Ctrl+S)"));
    topLayout->addWidget(m_saveBtn);

    m_exportBtn = new GlassButton("📤", m_topToolbar);
    m_exportBtn->setFixedSize(40, 40);
    m_exportBtn->setToolTip(tr("تصدير (Ctrl+E)"));
    topLayout->addWidget(m_exportBtn);

    topLayout->addStretch();

    // Zoom controls
    m_zoomOutBtn = new GlassButton("−", m_topToolbar);
    m_zoomOutBtn->setFixedSize(35, 35);
    topLayout->addWidget(m_zoomOutBtn);

    m_zoomLabel = new QLabel("100%", m_topToolbar);
    m_zoomLabel->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    m_zoomLabel->setFixedWidth(50);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(m_zoomLabel);

    m_zoomInBtn = new GlassButton("+", m_topToolbar);
    m_zoomInBtn->setFixedSize(35, 35);
    topLayout->addWidget(m_zoomInBtn);

    m_zoomFitBtn = new GlassButton("⬚", m_topToolbar);
    m_zoomFitBtn->setFixedSize(35, 35);
    m_zoomFitBtn->setToolTip(tr("ملاءمة الشاشة"));
    topLayout->addWidget(m_zoomFitBtn);

    topLayout->addSpacing(20);

    m_aiProgressBar = new QProgressBar(m_topToolbar);
    m_aiProgressBar->setFixedSize(150, 20);
    m_aiProgressBar->setRange(0, 100);
    m_aiProgressBar->setValue(0);
    m_aiProgressBar->setVisible(false);
    m_aiProgressBar->setStyleSheet(
        "QProgressBar { background: rgba(0,0,0,0.3); border-radius: 10px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 10px; }"
    );
    topLayout->addWidget(m_aiProgressBar);

    centerLayout->addWidget(m_topToolbar, 0);

    // Canvas area
    createCanvas();
    centerLayout->addWidget(m_canvas, 1);

    // Bottom bar
    m_bottomBar = new GlassPanel(this);
    m_bottomBar->setFixedHeight(35);
    m_bottomBar->setGlassOpacity(0.08);

    QHBoxLayout *bottomLayout = new QHBoxLayout(m_bottomBar);
    bottomLayout->setContentsMargins(15, 5, 15, 5);

    QLabel *infoLabel = new QLabel(tr("جاهز"), m_bottomBar);
    infoLabel->setObjectName("statusLabel");
    infoLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    bottomLayout->addWidget(infoLabel);

    bottomLayout->addStretch();

    QLabel *dimLabel = new QLabel("0 x 0", m_bottomBar);
    dimLabel->setObjectName("dimLabel");
    dimLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    bottomLayout->addWidget(dimLabel);

    centerLayout->addWidget(m_bottomBar, 0);
    mainLayout->addLayout(centerLayout, 1);

    // Right panels
    createSidePanels();
    mainLayout->addWidget(m_rightPanel, 0);
}

void PhotoEditor::createToolbar()
{
    m_leftToolbar = new GlassPanel(this);
    m_leftToolbar->setFixedWidth(60);
    m_leftToolbar->setGlassOpacity(0.08);
    m_leftToolbar->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_leftToolbar);
    layout->setContentsMargins(5, 15, 5, 15);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Tool buttons
    QStringList tools = {
        "move", "select", "crop", "brush", "eraser",
        "clone", "heal", "gradient", "text", "shape",
        "eyedropper", "hand"
    };

    QStringList icons = {
        "↖", "▭", "✂", "🖌", "◯",
        "⚫", "✚", "▓", "T", "⬡",
        "💧", "✋"
    };

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

void PhotoEditor::createCanvas()
{
    m_canvas = new CanvasWidget(this);
    m_canvas->setZoom(m_zoomLevel);
    m_canvas->setStyleSheet("background: transparent;");
}

void PhotoEditor::createSidePanels()
{
    m_rightPanel = new GlassPanel(this);
    m_rightPanel->setFixedWidth(300);
    m_rightPanel->setGlassOpacity(0.1);
    m_rightPanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(10, 15, 10, 15);
    layout->setSpacing(15);

    // Tabs for different panels
    QHBoxLayout *tabLayout = new QHBoxLayout();

    GlassButton *layersTab = new GlassButton(tr("الطبقات"), m_rightPanel);
    layersTab->setCheckable(true);
    layersTab->setChecked(true);
    layersTab->setFixedHeight(35);
    tabLayout->addWidget(layersTab);

    GlassButton *adjustTab = new GlassButton(tr("تعديلات"), m_rightPanel);
    adjustTab->setCheckable(true);
    adjustTab->setFixedHeight(35);
    tabLayout->addWidget(adjustTab);

    GlassButton *aiTab = new GlassButton(tr("AI"), m_rightPanel);
    aiTab->setCheckable(true);
    aiTab->setFixedHeight(35);
    tabLayout->addWidget(aiTab);

    layout->addLayout(tabLayout);

    // Panels container
    QStackedWidget *panelsStack = new QStackedWidget(m_rightPanel);

    m_layersPanel = new LayersPanel(panelsStack);
    m_adjustmentsPanel = new AdjustmentsPanel(panelsStack);
    m_aiPanel = new AIPanel(panelsStack);

    panelsStack->addWidget(m_layersPanel);
    panelsStack->addWidget(m_adjustmentsPanel);
    panelsStack->addWidget(m_aiPanel);

    layout->addWidget(panelsStack, 1);

    // Tab connections
    connect(layersTab, &GlassButton::clicked, [panelsStack]() { panelsStack->setCurrentIndex(0); });
    connect(adjustTab, &GlassButton::clicked, [panelsStack]() { panelsStack->setCurrentIndex(1); });
    connect(aiTab, &GlassButton::clicked, [panelsStack]() { panelsStack->setCurrentIndex(2); });
}

void PhotoEditor::setupConnections()
{
    // Toolbar connections
    connect(m_undoBtn, &GlassButton::clicked, this, &PhotoEditor::undo);
    connect(m_redoBtn, &GlassButton::clicked, this, &PhotoEditor::redo);
    connect(m_saveBtn, &GlassButton::clicked, this, &PhotoEditor::saveImage);
    connect(m_exportBtn, &GlassButton::clicked, this, &PhotoEditor::exportImage);
    connect(m_zoomInBtn, &GlassButton::clicked, this, &PhotoEditor::zoomIn);
    connect(m_zoomOutBtn, &GlassButton::clicked, this, &PhotoEditor::zoomOut);
    connect(m_zoomFitBtn, &GlassButton::clicked, this, &PhotoEditor::zoomToFit);

    // Canvas connections
    connect(m_canvas, &CanvasWidget::mousePressed, this, &PhotoEditor::onCanvasMousePress);
    connect(m_canvas, &CanvasWidget::mouseMoved, this, &PhotoEditor::onCanvasMouseMove);
    connect(m_canvas, &CanvasWidget::mouseReleased, this, &PhotoEditor::onCanvasMouseRelease);
    connect(m_canvas, &CanvasWidget::wheelScrolled, this, &PhotoEditor::onCanvasWheel);

    // Panel connections
    connect(m_layersPanel, &LayersPanel::layerSelected, this, &PhotoEditor::onLayerSelected);
    connect(m_adjustmentsPanel, &AdjustmentsPanel::adjustmentChanged, this, &PhotoEditor::onAdjustmentChanged);
    connect(m_aiPanel, &AIPanel::autoEnhanceClicked, this, &PhotoEditor::aiAutoEnhance);
    connect(m_aiPanel, &AIPanel::removeBackgroundClicked, this, &PhotoEditor::aiRemoveBackground);
    connect(m_aiPanel, &AIPanel::upscaleClicked, this, &PhotoEditor::aiUpscale);
    connect(m_aiPanel, &AIPanel::portraitEnhanceClicked, this, &PhotoEditor::aiPortraitEnhance);
}

void PhotoEditor::setupShortcuts()
{
    // Shortcuts are handled in keyPressEvent
}

void PhotoEditor::openImage(const QString &path)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);

    QImage image = reader.read();
    if (image.isNull()) {
        emit statusMessage(tr("فشل تحميل الصورة: %1").arg(reader.errorString()));
        return;
    }

    m_originalImage = image;
    m_currentImage = image;
    m_currentPath = path;
    m_isModified = false;

    // Clear history
    m_undoStack.clear();
    m_redoStack.clear();

    // Update layer
    if (!m_layers.isEmpty()) {
        m_layers[0].image = image;
    }

    // Update canvas
    m_canvas->setImage(m_currentImage);
    updateCanvas();

    // Update status
    emit statusMessage(tr("تم فتح: %1").arg(QFileInfo(path).fileName()));
    emit imageModified(false);

    // Update dimension label
    QLabel *dimLabel = findChild<QLabel*>("dimLabel");
    if (dimLabel) {
        dimLabel->setText(QString("%1 x %2").arg(image.width()).arg(image.height()));
    }

    // Add to history
    addHistoryState(tr("فتح صورة"));
}

void PhotoEditor::saveImage()
{
    if (m_currentPath.isEmpty()) {
        QString path = QFileDialog::getSaveFileName(this, tr("حفظ الصورة"), QString(),
            tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;TIFF (*.tiff);;BMP (*.bmp)"));
        if (path.isEmpty()) return;
        m_currentPath = path;
    }

    if (m_currentImage.save(m_currentPath)) {
        m_isModified = false;
        emit statusMessage(tr("تم الحفظ: %1").arg(m_currentPath));
        emit imageModified(false);
    } else {
        emit statusMessage(tr("فشل الحفظ"));
    }
}

void PhotoEditor::exportImage()
{
    QString path = QFileDialog::getSaveFileName(this, tr("تصدير الصورة"), QString(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;TIFF (*.tiff);;WebP (*.webp)"));
    if (path.isEmpty()) return;

    exportImage(path, QFileInfo(path).suffix().toUpper());
}

void PhotoEditor::exportImage(const QString &path, const QString &format)
{
    QString fmt = format.toLower();
    if (fmt == "jpg") fmt = "jpeg";

    if (m_currentImage.save(path, fmt.toUtf8().constData())) {
        emit statusMessage(tr("تم التصدير: %1").arg(path));
    } else {
        emit statusMessage(tr("فشل التصدير"));
    }
}

void PhotoEditor::undo()
{
    if (m_undoStack.isEmpty()) return;

    EditState state = m_undoStack.pop();
    m_redoStack.push(state);

    // Restore image
    if (!m_undoStack.isEmpty()) {
        m_currentImage = m_undoStack.top().image;
    } else {
        m_currentImage = m_originalImage;
    }

    m_canvas->setImage(m_currentImage);
    updateCanvas();

    emit statusMessage(tr("تراجع: %1").arg(state.action));
    emit historyChanged(!m_undoStack.isEmpty(), !m_redoStack.isEmpty());
}

void PhotoEditor::redo()
{
    if (m_redoStack.isEmpty()) return;

    EditState state = m_redoStack.pop();
    m_undoStack.push(state);

    m_currentImage = state.image;
    m_canvas->setImage(m_currentImage);
    updateCanvas();

    emit statusMessage(tr("إعادة: %1").arg(state.action));
    emit historyChanged(!m_undoStack.isEmpty(), !m_redoStack.isEmpty());
}

void PhotoEditor::addHistoryState(const QString &action)
{
    EditState state;
    state.image = m_currentImage;
    state.action = action;
    state.timestamp = QDateTime::currentDateTime();

    m_undoStack.push(state);

    // Clear redo stack on new action
    m_redoStack.clear();

    // Limit history size
    while (m_undoStack.size() > MAX_HISTORY_SIZE) {
        m_undoStack.removeFirst();
    }

    m_isModified = true;
    emit imageModified(true);
    emit historyChanged(true, false);
}

void PhotoEditor::setZoomLevel(float zoom)
{
    m_zoomLevel = qBound(0.1f, zoom, 10.0f);
    m_canvas->setZoom(m_zoomLevel);
    m_zoomLabel->setText(QString("%1%").arg(int(m_zoomLevel * 100)));
    emit zoomChanged(m_zoomLevel);
}

void PhotoEditor::zoomIn()
{
    setZoomLevel(m_zoomLevel * 1.25f);
}

void PhotoEditor::zoomOut()
{
    setZoomLevel(m_zoomLevel * 0.8f);
}

void PhotoEditor::zoomToFit()
{
    if (m_currentImage.isNull()) return;

    QSize canvasSize = m_canvas->size();
    float scaleX = float(canvasSize.width()) / m_currentImage.width();
    float scaleY = float(canvasSize.height()) / m_currentImage.height();
    setZoomLevel(qMin(scaleX, scaleY) * 0.95f);
}

void PhotoEditor::zoomTo100()
{
    setZoomLevel(1.0f);
}

void PhotoEditor::rotate(int degrees)
{
    if (m_currentImage.isNull()) return;

    QTransform transform;
    transform.rotate(degrees);
    m_currentImage = m_currentImage.transformed(transform, Qt::SmoothTransformation);

    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("تدوير %1°").arg(degrees));
}

void PhotoEditor::flipHorizontal()
{
    if (m_currentImage.isNull()) return;

    m_currentImage = m_currentImage.mirrored(true, false);
    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("انعكاس أفقي"));
}

void PhotoEditor::flipVertical()
{
    if (m_currentImage.isNull()) return;

    m_currentImage = m_currentImage.mirrored(false, true);
    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("انعكاس رأسي"));
}

void PhotoEditor::crop(const QRect &rect)
{
    if (m_currentImage.isNull() || rect.isEmpty()) return;

    m_currentImage = m_currentImage.copy(rect);
    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("قص"));
}

void PhotoEditor::resize(int width, int height)
{
    if (m_currentImage.isNull() || width <= 0 || height <= 0) return;

    m_currentImage = m_currentImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("تغيير الحجم"));
}

void PhotoEditor::setBrightness(int value)
{
    m_adjustments.brightness = value;
    applyAdjustments();
}

void PhotoEditor::setContrast(int value)
{
    m_adjustments.contrast = value;
    applyAdjustments();
}

void PhotoEditor::setSaturation(int value)
{
    m_adjustments.saturation = value;
    applyAdjustments();
}

void PhotoEditor::applyAdjustments()
{
    if (m_originalImage.isNull()) return;

    // Apply adjustments to a copy
    m_currentImage = m_originalImage.copy();

    // Apply brightness and contrast
    if (m_adjustments.brightness != 0 || m_adjustments.contrast != 0) {
        int brightness = m_adjustments.brightness * 255 / 100;
        float contrast = (m_adjustments.contrast + 100.0f) / 100.0f;

        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor c = m_currentImage.pixelColor(x, y);
                int r = qBound(0, int((c.red() + brightness - 128) * contrast + 128), 255);
                int g = qBound(0, int((c.green() + brightness - 128) * contrast + 128), 255);
                int b = qBound(0, int((c.blue() + brightness - 128) * contrast + 128), 255);
                m_currentImage.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
            }
        }
    }

    // Apply saturation
    if (m_adjustments.saturation != 0) {
        float saturation = (m_adjustments.saturation + 100.0f) / 100.0f;
        for (int y = 0; y < m_currentImage.height(); ++y) {
            for (int x = 0; x < m_currentImage.width(); ++x) {
                QColor c = m_currentImage.pixelColor(x, y);
                int h, s, v;
                c.getHsv(&h, &s, &v);
                s = qBound(0, int(s * saturation), 255);
                c.setHsv(h, s, v);
                m_currentImage.setPixelColor(x, y, c);
            }
        }
    }

    m_canvas->setImage(m_currentImage);
    updateCanvas();
}

void PhotoEditor::applyGrayscale()
{
    if (m_currentImage.isNull()) return;

    for (int y = 0; y < m_currentImage.height(); ++y) {
        for (int x = 0; x < m_currentImage.width(); ++x) {
            QColor c = m_currentImage.pixelColor(x, y);
            int gray = qGray(c.rgb());
            m_currentImage.setPixelColor(x, y, QColor(gray, gray, gray, c.alpha()));
        }
    }

    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("تدرج رمادي"));
}

void PhotoEditor::applySepia()
{
    if (m_currentImage.isNull()) return;

    for (int y = 0; y < m_currentImage.height(); ++y) {
        for (int x = 0; x < m_currentImage.width(); ++x) {
            QColor c = m_currentImage.pixelColor(x, y);
            int r = c.red();
            int g = c.green();
            int b = c.blue();

            int tr = qBound(0, int(0.393 * r + 0.769 * g + 0.189 * b), 255);
            int tg = qBound(0, int(0.349 * r + 0.686 * g + 0.168 * b), 255);
            int tb = qBound(0, int(0.272 * r + 0.534 * g + 0.131 * b), 255);

            m_currentImage.setPixelColor(x, y, QColor(tr, tg, tb, c.alpha()));
        }
    }

    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("سيبيا"));
}

void PhotoEditor::addLayer(const QString &name)
{
    Layer layer;
    layer.name = name.isEmpty() ? tr("طبقة %1").arg(m_layers.size() + 1) : name;
    layer.opacity = 1.0f;
    layer.visible = true;
    layer.locked = false;
    layer.blendMode = QPainter::CompositionMode_SourceOver;

    if (!m_currentImage.isNull()) {
        layer.image = QImage(m_currentImage.size(), QImage::Format_ARGB32);
        layer.image.fill(Qt::transparent);
    }

    m_layers.insert(m_currentLayerIndex, layer);
    m_layersPanel->setLayers(m_layers);

    emit statusMessage(tr("تمت إضافة طبقة: %1").arg(layer.name));
}

void PhotoEditor::deleteLayer(int index)
{
    if (index < 0 || index >= m_layers.size() || m_layers.size() <= 1) return;

    QString name = m_layers[index].name;
    m_layers.removeAt(index);

    if (m_currentLayerIndex >= m_layers.size()) {
        m_currentLayerIndex = m_layers.size() - 1;
    }

    m_layersPanel->setLayers(m_layers);
    emit statusMessage(tr("تم حذف الطبقة: %1").arg(name));
}

void PhotoEditor::mergeLayerDown(int index)
{
    if (index <= 0 || index >= m_layers.size()) return;

    Layer &top = m_layers[index];
    Layer &bottom = m_layers[index - 1];

    QPainter painter(&bottom.image);
    painter.setOpacity(top.opacity);
    painter.setCompositionMode(top.blendMode);
    painter.drawImage(0, 0, top.image);
    painter.end();

    m_layers.removeAt(index);
    m_currentLayerIndex--;

    m_layersPanel->setLayers(m_layers);
    renderLayers();
    addHistoryState(tr("دمج الطبقة"));
}

void PhotoEditor::renderLayers()
{
    if (m_layers.isEmpty()) return;

    // Start with transparent image
    QImage result(m_layers[0].image.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);

    // Render each visible layer
    for (const Layer &layer : m_layers) {
        if (!layer.visible) continue;

        painter.setOpacity(layer.opacity);
        painter.setCompositionMode(layer.blendMode);
        painter.drawImage(0, 0, layer.image);
    }

    painter.end();

    m_currentImage = result;
    m_canvas->setImage(m_currentImage);
    updateCanvas();
}

void PhotoEditor::selectTool(const QString &toolName)
{
    m_currentTool = toolName;
    emit statusMessage(tr("أداة محددة: %1").arg(toolName));
}

void PhotoEditor::setBrushSize(int size)
{
    m_brushSize = qBound(1, size, 500);
}

void PhotoEditor::onCanvasMousePress(const QPoint &pos)
{
    m_isDrawing = true;
    m_lastPos = pos;

    if (m_currentLayerIndex >= 0 && m_currentLayerIndex < m_layers.size()) {
        m_layerBeforeStroke = m_layers[m_currentLayerIndex].image.copy();
    }

    applyTool(pos);
}

void PhotoEditor::onCanvasMouseMove(const QPoint &pos)
{
    if (!m_isDrawing) return;

    if (m_currentTool == "brush" || m_currentTool == "eraser") {
        drawLine(m_lastPos, pos);
    }

    m_lastPos = pos;
    updateCanvas();
}

void PhotoEditor::onCanvasMouseRelease(const QPoint &pos)
{
    if (!m_isDrawing) return;

    m_isDrawing = false;

    if (m_currentTool == "brush" || m_currentTool == "eraser") {
        addHistoryState(m_currentTool == "brush" ? tr("رسم") : tr("ممحاة"));
    }
}

void PhotoEditor::onCanvasWheel(int delta)
{
    if (delta > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}

void PhotoEditor::onLayerSelected(int index)
{
    if (index >= 0 && index < m_layers.size()) {
        m_currentLayerIndex = index;
        emit layerChanged(index);
    }
}

void PhotoEditor::onAdjustmentChanged(const QString &name, int value)
{
    if (name == "brightness") setBrightness(value);
    else if (name == "contrast") setContrast(value);
    else if (name == "saturation") setSaturation(value);
}

void PhotoEditor::applyTool(const QPoint &pos)
{
    if (m_currentTool == "brush" || m_currentTool == "eraser") {
        drawBrush(pos);
    } else if (m_currentTool == "eyedropper") {
        pickColor(pos);
    }
}

void PhotoEditor::drawBrush(const QPoint &pos)
{
    if (m_currentLayerIndex < 0 || m_currentLayerIndex >= m_layers.size()) return;

    Layer &layer = m_layers[m_currentLayerIndex];
    if (layer.locked || !layer.visible) return;

    QPainter painter(&layer.image);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_currentTool == "eraser") {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
    }

    QColor color = m_currentTool == "eraser" ? Qt::transparent : m_primaryColor;
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);

    painter.drawEllipse(pos, m_brushSize / 2, m_brushSize / 2);
    painter.end();

    renderLayers();
}

void PhotoEditor::drawLine(const QPoint &from, const QPoint &to)
{
    if (m_currentLayerIndex < 0 || m_currentLayerIndex >= m_layers.size()) return;

    Layer &layer = m_layers[m_currentLayerIndex];
    if (layer.locked || !layer.visible) return;

    QPainter painter(&layer.image);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_currentTool == "eraser") {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
    }

    QColor color = m_currentTool == "eraser" ? Qt::transparent : m_primaryColor;
    QPen pen(color, m_brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    painter.drawLine(from, to);
    painter.end();

    renderLayers();
}

void PhotoEditor::pickColor(const QPoint &pos)
{
    if (m_currentImage.isNull()) return;
    if (pos.x() < 0 || pos.x() >= m_currentImage.width()) return;
    if (pos.y() < 0 || pos.y() >= m_currentImage.height()) return;

    m_primaryColor = m_currentImage.pixelColor(pos);
    emit statusMessage(tr("لون مختار: %1").arg(m_primaryColor.name()));
}

void PhotoEditor::updateCanvas()
{
    m_canvas->update();
}

void PhotoEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_Z: undo(); return;
        case Qt::Key_Y: redo(); return;
        case Qt::Key_S: saveImage(); return;
        case Qt::Key_E: exportImage(); return;
        case Qt::Key_O: {
            QString path = QFileDialog::getOpenFileName(this, tr("فتح صورة"), QString(),
                tr("الصور (*.png *.jpg *.jpeg *.bmp *.tiff);;كل الملفات (*)"));
            if (!path.isEmpty()) openImage(path);
            return;
        }
        case Qt::Key_Plus:
        case Qt::Key_Equal: zoomIn(); return;
        case Qt::Key_Minus: zoomOut(); return;
        case Qt::Key_0: zoomToFit(); return;
        case Qt::Key_1: zoomTo100(); return;
        }
    }

    QWidget::keyPressEvent(event);
}

void PhotoEditor::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void PhotoEditor::setBrushSize(int size)
{
    m_brushSize = qBound(1, size, 500);
}

void PhotoEditor::cut()
{
    copy();
    // Clear selection area
    if (m_hasSelection) {
        // Implementation for cut
    }
}

void PhotoEditor::copy()
{
    if (m_currentImage.isNull()) return;

    QImage copyImage = m_hasSelection ? m_currentImage.copy(m_selection) : m_currentImage;
    QApplication::clipboard()->setImage(copyImage);
    emit statusMessage(tr("تم النسخ"));
}

void PhotoEditor::paste()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        if (!image.isNull()) {
            // Create new layer with pasted image
            addLayer(tr("لصق"));
            m_layers[m_currentLayerIndex].image = image;
            renderLayers();
            addHistoryState(tr("لصق"));
        }
    }
}

void PhotoEditor::selectAll()
{
    if (m_currentImage.isNull()) return;
    m_selection = m_currentImage.rect();
    m_hasSelection = true;
    m_canvas->setSelection(m_selection);
    emit selectionChanged(m_selection);
}

void PhotoEditor::deselect()
{
    m_hasSelection = false;
    m_selection = QRect();
    m_canvas->clearSelection();
    emit selectionChanged(m_selection);
}

void PhotoEditor::invertSelection()
{
    if (m_currentImage.isNull()) return;
    m_selection = m_currentImage.rect().adjusted(
        m_selection.left(), m_selection.top(),
        -m_selection.right(), -m_selection.bottom()
    );
    m_canvas->setSelection(m_selection);
    emit selectionChanged(m_selection);
}

void PhotoEditor::setHue(int value) { m_adjustments.hue = value; applyAdjustments(); }
void PhotoEditor::setExposure(int value) { m_adjustments.exposure = value; applyAdjustments(); }
void PhotoEditor::setHighlights(int value) { m_adjustments.highlights = value; applyAdjustments(); }
void PhotoEditor::setShadows(int value) { m_adjustments.shadows = value; applyAdjustments(); }
void PhotoEditor::setSharpness(int value) { m_adjustments.sharpness = value; applyAdjustments(); }
void PhotoEditor::setBlur(int value) { m_adjustments.blur = value; applyAdjustments(); }
void PhotoEditor::setVignette(int value) { m_adjustments.vignette = value; applyAdjustments(); }
void PhotoEditor::setTemperature(int value) { m_adjustments.temperature = value; applyAdjustments(); }
void PhotoEditor::setTint(int value) { m_adjustments.tint = value; applyAdjustments(); }

void PhotoEditor::applyFilter(const QString &filterName)
{
    if (filterName == "grayscale") applyGrayscale();
    else if (filterName == "sepia") applySepia();
    else if (filterName == "invert") applyInvert();
}

void PhotoEditor::applyInvert()
{
    if (m_currentImage.isNull()) return;

    m_currentImage.invertPixels();
    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("عكس الألوان"));
}

void PhotoEditor::applyPosterize(int levels)
{
    if (m_currentImage.isNull() || levels < 2) return;

    int step = 256 / levels;
    for (int y = 0; y < m_currentImage.height(); ++y) {
        for (int x = 0; x < m_currentImage.width(); ++x) {
            QColor c = m_currentImage.pixelColor(x, y);
            int r = (c.red() / step) * step;
            int g = (c.green() / step) * step;
            int b = (c.blue() / step) * step;
            m_currentImage.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }

    m_canvas->setImage(m_currentImage);
    updateCanvas();
    addHistoryState(tr("بوسترايز"));
}

void PhotoEditor::duplicateLayer(int index)
{
    if (index < 0 || index >= m_layers.size()) return;

    Layer copy = m_layers[index];
    copy.name = copy.name + tr(" (نسخة)");
    m_layers.insert(index + 1, copy);
    m_layersPanel->setLayers(m_layers);
    emit statusMessage(tr("تم تكرار الطبقة"));
}

void PhotoEditor::mergeAllLayers()
{
    if (m_layers.size() <= 1) return;

    while (m_layers.size() > 1) {
        mergeLayerDown(m_layers.size() - 1);
    }

    m_layers[0].name = tr("دمج الكل");
    m_layersPanel->setLayers(m_layers);
}

void PhotoEditor::moveLayer(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= m_layers.size()) return;
    if (toIndex < 0 || toIndex >= m_layers.size()) return;

    Layer layer = m_layers.takeAt(fromIndex);
    m_layers.insert(toIndex, layer);
    m_currentLayerIndex = toIndex;

    m_layersPanel->setLayers(m_layers);
    renderLayers();
}

void PhotoEditor::setLayerOpacity(int index, float opacity)
{
    if (index < 0 || index >= m_layers.size()) return;

    m_layers[index].opacity = qBound(0.0f, opacity, 1.0f);
    renderLayers();
}

void PhotoEditor::setLayerVisibility(int index, bool visible)
{
    if (index < 0 || index >= m_layers.size()) return;

    m_layers[index].visible = visible;
    renderLayers();
}

void PhotoEditor::setLayerBlendMode(int index, QPainter::CompositionMode mode)
{
    if (index < 0 || index >= m_layers.size()) return;

    m_layers[index].blendMode = mode;
    renderLayers();
}

void PhotoEditor::setPrimaryColor(const QColor &color)
{
    m_primaryColor = color;
}

void PhotoEditor::setSecondaryColor(const QColor &color)
{
    m_secondaryColor = color;
}

// ==================== AI Operations ====================

void PhotoEditor::aiAutoEnhance()
{
    if (m_currentImage.isNull()) return;

    emit aiProcessingStarted(tr("تحسين تلقائي"));
    m_aiProgressBar->setVisible(true);
    m_aiProgressBar->setValue(0);

    // Simulate AI processing with progress
    m_aiProgressTimer = new QTimer(this);
    int progress = 0;
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress]() {
        progress += 5;
        m_aiProgressBar->setValue(progress);
        emit aiProcessingProgress(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            // Apply enhancement
            m_currentImage = processAIAutoEnhance(m_currentImage);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("تحسين AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تم التحسين بنجاح"));
        }
    });
    m_aiProgressTimer->start(50);
}

void PhotoEditor::aiRemoveBackground()
{
    if (m_currentImage.isNull()) return;

    emit aiProcessingStarted(tr("إزالة الخلفية"));
    m_aiProgressBar->setVisible(true);
    m_aiProgressBar->setValue(0);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress]() {
        progress += 3;
        m_aiProgressBar->setValue(progress);
        emit aiProcessingProgress(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIRemoveBackground(m_currentImage);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("إزالة خلفية AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تمت إزالة الخلفية"));
        }
    });
    m_aiProgressTimer->start(80);
}

void PhotoEditor::aiUpscale(int scale)
{
    if (m_currentImage.isNull()) return;
    scale = qBound(2, scale, 4);

    emit aiProcessingStarted(tr("تكبير %1x").arg(scale));
    m_aiProgressBar->setVisible(true);
    m_aiProgressBar->setValue(0);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress, scale]() {
        progress += 2;
        m_aiProgressBar->setValue(progress);
        emit aiProcessingProgress(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIUpscale(m_currentImage, scale);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("تكبير AI %1x").arg(scale));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تم التكبير بنجاح"));
        }
    });
    m_aiProgressTimer->start(100);
}

void PhotoEditor::aiPortraitEnhance()
{
    if (m_currentImage.isNull()) return;

    emit aiProcessingStarted(tr("تحسين البورتريه"));
    m_aiProgressBar->setVisible(true);
    m_aiProgressBar->setValue(0);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress]() {
        progress += 4;
        m_aiProgressBar->setValue(progress);
        emit aiProcessingProgress(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIPortraitEnhance(m_currentImage);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("تحسين بورتريه AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تم تحسين البورتريه"));
        }
    });
    m_aiProgressTimer->start(60);
}

void PhotoEditor::aiColorMatch(const QString &referencePath)
{
    if (m_currentImage.isNull()) return;

    QImage reference(referencePath);
    if (reference.isNull()) {
        emit statusMessage(tr("فشل تحميل صورة المرجع"));
        return;
    }

    emit aiProcessingStarted(tr("مطابقة الألوان"));
    m_aiProgressBar->setVisible(true);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress, reference]() {
        progress += 3;
        m_aiProgressBar->setValue(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIColorMatch(m_currentImage, reference);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("مطابقة ألوان AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تمت مطابقة الألوان"));
        }
    });
    m_aiProgressTimer->start(70);
}

void PhotoEditor::aiStyleTransfer(const QString &stylePath)
{
    if (m_currentImage.isNull()) return;

    QImage style(stylePath);
    if (style.isNull()) {
        emit statusMessage(tr("فشل تحميل نمط التحويل"));
        return;
    }

    emit aiProcessingStarted(tr("نقل النمط"));
    m_aiProgressBar->setVisible(true);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress, style]() {
        progress += 2;
        m_aiProgressBar->setValue(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIStyleTransfer(m_currentImage, style);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("نقل نمط AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تم نقل النمط"));
        }
    });
    m_aiProgressTimer->start(120);
}

void PhotoEditor::aiGenerateMask(const QString &prompt)
{
    if (m_currentImage.isNull()) return;

    emit aiProcessingStarted(tr("توليد قناع: %1").arg(prompt));
    m_aiProgressBar->setVisible(true);

    int progress = 0;
    m_aiProgressTimer = new QTimer(this);
    connect(m_aiProgressTimer, &QTimer::timeout, this, [this, &progress, prompt]() {
        progress += 2;
        m_aiProgressBar->setValue(progress);

        if (progress >= 100) {
            m_aiProgressTimer->stop();
            m_aiProgressTimer->deleteLater();

            m_currentImage = processAIGenerateMask(m_currentImage, prompt);
            m_canvas->setImage(m_currentImage);
            updateCanvas();
            addHistoryState(tr("قناع AI"));

            m_aiProgressBar->setVisible(false);
            emit aiProcessingFinished(tr("تم توليد القناع"));
        }
    });
    m_aiProgressTimer->start(100);
}

// ==================== AI Processing Functions ====================

QImage PhotoEditor::processAIAutoEnhance(const QImage &input)
{
    QImage output = input.copy();

    // Auto-levels implementation
    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;

    // Find min/max values
    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor c = output.pixelColor(x, y);
            minR = qMin(minR, c.red());
            minG = qMin(minG, c.green());
            minB = qMin(minB, c.blue());
            maxR = qMax(maxR, c.red());
            maxG = qMax(maxG, c.green());
            maxB = qMax(maxB, c.blue());
        }
    }

    // Apply auto-levels
    float scaleR = 255.0f / (maxR - minR + 1);
    float scaleG = 255.0f / (maxG - minG + 1);
    float scaleB = 255.0f / (maxB - minB + 1);

    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor c = output.pixelColor(x, y);
            int r = qBound(0, int((c.red() - minR) * scaleR), 255);
            int g = qBound(0, int((c.green() - minG) * scaleG), 255);
            int b = qBound(0, int((c.blue() - minB) * scaleB), 255);
            output.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }

    // Slight saturation boost
    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor c = output.pixelColor(x, y);
            int h, s, v;
            c.getHsv(&h, &s, &v);
            s = qMin(255, int(s * 1.1));
            c.setHsv(h, s, v);
            output.setPixelColor(x, y, c);
        }
    }

    return output;
}

QImage PhotoEditor::processAIRemoveBackground(const QImage &input)
{
    // Simplified background removal using edge detection and flood fill
    QImage output(input.size(), QImage::Format_ARGB32);
    output.fill(Qt::transparent);

    // Create a simple mask based on brightness difference from edges
    int threshold = 30;

    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            QColor c = input.pixelColor(x, y);

            // Simple edge detection
            bool isEdge = false;
            if (x > 0 && x < input.width() - 1 && y > 0 && y < input.height() - 1) {
                QColor left = input.pixelColor(x - 1, y);
                QColor right = input.pixelColor(x + 1, y);
                QColor top = input.pixelColor(x, y - 1);
                QColor bottom = input.pixelColor(x, y + 1);

                int diff = qAbs(c.lightness() - left.lightness()) +
                          qAbs(c.lightness() - right.lightness()) +
                          qAbs(c.lightness() - top.lightness()) +
                          qAbs(c.lightness() - bottom.lightness());

                isEdge = diff > threshold * 4;
            }

            // Keep pixels that are part of edges or interior
            if (isEdge || (x > input.width() * 0.1 && x < input.width() * 0.9 &&
                          y > input.height() * 0.1 && y < input.height() * 0.9)) {
                output.setPixelColor(x, y, c);
            }
        }
    }

    return output;
}

QImage PhotoEditor::processAIUpscale(const QImage &input, int scale)
{
    // Use high-quality scaling
    int newWidth = input.width() * scale;
    int newHeight = input.height() * scale;

    return input.scaled(newWidth, newHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QImage PhotoEditor::processAIPortraitEnhance(const QImage &input)
{
    QImage output = input.copy();

    // Skin smoothing simulation
    for (int y = 1; y < output.height() - 1; ++y) {
        for (int x = 1; x < output.width() - 1; ++x) {
            QColor c = output.pixelColor(x, y);

            // Detect skin tones (simplified)
            bool isSkin = (c.red() > 60 && c.red() < 255 &&
                          c.green() > 40 && c.green() < 220 &&
                          c.blue() > 20 && c.blue() < 170 &&
                          c.red() > c.green() && c.green() > c.blue());

            if (isSkin) {
                // Apply slight blur to skin areas
                int r = 0, g = 0, b = 0, a = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        QColor nc = input.pixelColor(x + dx, y + dy);
                        r += nc.red();
                        g += nc.green();
                        b += nc.blue();
                        a += nc.alpha();
                    }
                }
                output.setPixelColor(x, y, QColor(r / 9, g / 9, b / 9, a / 9));
            }
        }
    }

    // Eye brightening (simplified - brighten upper portion)
    int eyeY = output.height() / 3;
    for (int y = eyeY - 20; y < eyeY + 20; ++y) {
        for (int x = output.width() / 4; x < output.width() * 3 / 4; ++x) {
            if (y >= 0 && y < output.height() && x >= 0 && x < output.width()) {
                QColor c = output.pixelColor(x, y);
                // Brighten slightly
                c.setRgb(qMin(255, c.red() + 10),
                        qMin(255, c.green() + 10),
                        qMin(255, c.blue() + 10),
                        c.alpha());
                output.setPixelColor(x, y, c);
            }
        }
    }

    return output;
}

QImage PhotoEditor::processAIColorMatch(const QImage &input, const QImage &reference)
{
    QImage output = input.copy();

    // Calculate mean colors
    qint64 refR = 0, refG = 0, refB = 0;
    qint64 inR = 0, inG = 0, inB = 0;
    int refCount = 0, inCount = 0;

    for (int y = 0; y < reference.height(); ++y) {
        for (int x = 0; x < reference.width(); ++x) {
            QColor c = reference.pixelColor(x, y);
            refR += c.red();
            refG += c.green();
            refB += c.blue();
            refCount++;
        }
    }

    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            QColor c = input.pixelColor(x, y);
            inR += c.red();
            inG += c.green();
            inB += c.blue();
            inCount++;
        }
    }

    float ratioR = (refR / refCount) / (inR / inCount);
    float ratioG = (refG / refCount) / (inG / inCount);
    float ratioB = (refB / refCount) / (inB / inCount);

    // Apply color matching
    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor c = output.pixelColor(x, y);
            int r = qBound(0, int(c.red() * ratioR), 255);
            int g = qBound(0, int(c.green() * ratioG), 255);
            int b = qBound(0, int(c.blue() * ratioB), 255);
            output.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }

    return output;
}

QImage PhotoEditor::processAIStyleTransfer(const QImage &input, const QImage &style)
{
    // Simplified style transfer - blend with style colors
    QImage output = input.copy();
    QImage scaledStyle = style.scaled(input.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor inColor = input.pixelColor(x, y);
            QColor styleColor = scaledStyle.pixelColor(x, y);

            // Blend based on style intensity
            float blend = 0.3f;
            int r = int(inColor.red() * (1 - blend) + styleColor.red() * blend);
            int g = int(inColor.green() * (1 - blend) + styleColor.green() * blend);
            int b = int(inColor.blue() * (1 - blend) + styleColor.blue() * blend);

            output.setPixelColor(x, y, QColor(r, g, b, inColor.alpha()));
        }
    }

    return output;
}

QImage PhotoEditor::processAIGenerateMask(const QImage &input, const QString &prompt)
{
    // Simplified mask generation - create a gradient mask
    QImage mask(input.size(), QImage::Format_ARGB32);

    // Parse prompt for keywords (simplified)
    bool isCenter = prompt.contains("center") || prompt.contains("center");
    bool isTop = prompt.contains("top") || prompt.contains("top");

    for (int y = 0; y < mask.height(); ++y) {
        for (int x = 0; x < mask.width(); ++x) {
            float alpha = 1.0f;

            if (isCenter) {
                float dx = (x - mask.width() / 2.0f) / (mask.width() / 2.0f);
                float dy = (y - mask.height() / 2.0f) / (mask.height() / 2.0f);
                alpha = 1.0f - qMin(1.0f, std::sqrt(dx * dx + dy * dy));
            } else if (isTop) {
                alpha = 1.0f - (float(y) / mask.height());
            } else {
                // Default gradient from left to right
                alpha = float(x) / mask.width();
            }

            int a = int(alpha * 255);
            mask.setPixelColor(x, y, QColor(a, a, a, 255));
        }
    }

    // Apply mask to input
    QImage output(input.size(), QImage::Format_ARGB32);
    for (int y = 0; y < output.height(); ++y) {
        for (int x = 0; x < output.width(); ++x) {
            QColor inColor = input.pixelColor(x, y);
            int maskAlpha = mask.pixelColor(x, y).red();
            inColor.setAlpha(maskAlpha);
            output.setPixelColor(x, y, inColor);
        }
    }

    return output;
}

// ==================== CanvasWidget Implementation ====================

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
    , m_zoom(1.0f)
    , m_hasSelection(false)
    , m_isDragging(false)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void CanvasWidget::setImage(const QImage &image)
{
    m_image = image;
    update();
}

void CanvasWidget::setZoom(float zoom)
{
    m_zoom = zoom;
    update();
}

void CanvasWidget::setOffset(const QPoint &offset)
{
    m_offset = offset;
    update();
}

void CanvasWidget::setSelection(const QRect &selection)
{
    m_selection = selection;
    m_hasSelection = true;
    update();
}

void CanvasWidget::clearSelection()
{
    m_hasSelection = false;
    m_selection = QRect();
    update();
}

QPoint CanvasWidget::imagePosFromWidget(const QPoint &widgetPos) const
{
    int imgX = int((widgetPos.x() - m_offset.x() - width() / 2 + m_image.width() * m_zoom / 2) / m_zoom);
    int imgY = int((widgetPos.y() - m_offset.y() - height() / 2 + m_image.height() * m_zoom / 2) / m_zoom);
    return QPoint(imgX, imgY);
}

QRect CanvasWidget::visibleImageRect() const
{
    if (m_image.isNull()) return QRect();

    int x = m_offset.x() + width() / 2 - int(m_image.width() * m_zoom / 2);
    int y = m_offset.y() + height() / 2 - int(m_image.height() * m_zoom / 2);
    int w = int(m_image.width() * m_zoom);
    int h = int(m_image.height() * m_zoom);

    return QRect(x, y, w, h);
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Transparent background pattern
    drawGrid(painter);

    // Draw image
    drawImage(painter);

    // Draw selection
    if (m_hasSelection) {
        drawSelection(painter);
    }

    // Draw overlay
    drawOverlay(painter);
}

void CanvasWidget::drawGrid(QPainter &painter)
{
    // Checkerboard pattern for transparency
    int checkerSize = 10;
    for (int y = 0; y < height(); y += checkerSize) {
        for (int x = 0; x < width(); x += checkerSize) {
            QColor color = ((x / checkerSize + y / checkerSize) % 2 == 0)
                          ? QColor(40, 40, 50) : QColor(50, 50, 60);
            painter.fillRect(x, y, checkerSize, checkerSize, color);
        }
    }
}

void CanvasWidget::drawImage(QPainter &painter)
{
    if (m_image.isNull()) {
        // Draw placeholder text
        painter.setPen(QPen(QColor(150, 150, 150)));
        painter.drawText(rect(), Qt::AlignCenter, tr("افتح صورة للبدء"));
        return;
    }

    QRect imgRect = visibleImageRect();
    painter.drawImage(imgRect, m_image);
}

void CanvasWidget::drawSelection(QPainter &painter)
{
    if (!m_hasSelection || m_image.isNull()) return;

    QRect imgRect = visibleImageRect();
    QRect selRect(
        imgRect.left() + int(m_selection.left() * m_zoom),
        imgRect.top() + int(m_selection.top() * m_zoom),
        int(m_selection.width() * m_zoom),
        int(m_selection.height() * m_zoom)
    );

    // Draw marching ants border
    QPen pen(CP_ACCENT_CYAN, 1, Qt::DashLine);
    pen.setDashPattern({4, 4});
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(selRect);

    // Draw handles
    painter.setBrush(CP_ACCENT_CYAN);
    int handleSize = 6;
    painter.drawRect(selRect.left() - handleSize/2, selRect.top() - handleSize/2, handleSize, handleSize);
    painter.drawRect(selRect.right() - handleSize/2, selRect.top() - handleSize/2, handleSize, handleSize);
    painter.drawRect(selRect.left() - handleSize/2, selRect.bottom() - handleSize/2, handleSize, handleSize);
    painter.drawRect(selRect.right() - handleSize/2, selRect.bottom() - handleSize/2, handleSize, handleSize);
}

void CanvasWidget::drawOverlay(QPainter &painter)
{
    // Draw zoom level indicator
    painter.setPen(QPen(QColor(200, 200, 200)));
    QFont font;
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(10, 20, QString("%1%").arg(int(m_zoom * 100)));
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && QApplication::keyboardModifiers() & Qt::SpaceModifier)) {
        m_isDragging = true;
        m_dragStart = event->pos();
        m_lastOffset = m_offset;
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPoint imgPos = imagePosFromWidget(event->pos());
        emit mousePressed(imgPos);
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        QPoint delta = event->pos() - m_dragStart;
        m_offset = m_lastOffset + delta;
        update();
        return;
    }

    QPoint imgPos = imagePosFromWidget(event->pos());
    emit mouseMoved(imgPos);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPoint imgPos = imagePosFromWidget(event->pos());
        emit mouseReleased(imgPos);
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event)
{
    emit wheelScrolled(event->angleDelta().y());
}

// ==================== LayersPanel Implementation ====================

LayersPanel::LayersPanel(QWidget *parent)
    : QWidget(parent)
    , m_currentLayerIndex(0)
{
    setupUI();
}

void LayersPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Layer controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();

    GlassButton *addBtn = new GlassButton("+", this);
    addBtn->setFixedSize(35, 35);
    addBtn->setToolTip(tr("إضافة طبقة"));
    connect(addBtn, &GlassButton::clicked, this, &LayersPanel::addLayerClicked);
    controlsLayout->addWidget(addBtn);

    GlassButton *deleteBtn = new GlassButton("−", this);
    deleteBtn->setFixedSize(35, 35);
    deleteBtn->setToolTip(tr("حذف طبقة"));
    connect(deleteBtn, &GlassButton::clicked, this, [this]() {
        emit deleteLayerClicked(m_currentLayerIndex);
    });
    controlsLayout->addWidget(deleteBtn);

    GlassButton *dupBtn = new GlassButton("⎘", this);
    dupBtn->setFixedSize(35, 35);
    dupBtn->setToolTip(tr("تكرار طبقة"));
    connect(dupBtn, &GlassButton::clicked, this, [this]() {
        emit duplicateLayerClicked(m_currentLayerIndex);
    });
    controlsLayout->addWidget(dupBtn);

    GlassButton *mergeBtn = new GlassButton("⮣", this);
    mergeBtn->setFixedSize(35, 35);
    mergeBtn->setToolTip(tr("دمج للأسفل"));
    connect(mergeBtn, &GlassButton::clicked, this, [this]() {
        emit mergeLayerClicked(m_currentLayerIndex);
    });
    controlsLayout->addWidget(mergeBtn);

    controlsLayout->addStretch();
    mainLayout->addLayout(controlsLayout);

    // Layers container
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_layersContainer = new QWidget();
    m_layersLayout = new QVBoxLayout(m_layersContainer);
    m_layersLayout->setContentsMargins(0, 0, 0, 0);
    m_layersLayout->setSpacing(5);
    m_layersLayout->addStretch();

    scrollArea->setWidget(m_layersContainer);
    mainLayout->addWidget(scrollArea, 1);
}

void LayersPanel::setLayers(const QVector<Layer> &layers)
{
    // Clear existing items
    QLayoutItem *child;
    while ((child = m_layersLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    // Add layer items
    for (int i = 0; i < layers.size(); ++i) {
        createLayerItem(i, layers[i]);
    }

    m_layersLayout->addStretch();
}

void LayersPanel::createLayerItem(int index, const Layer &layer)
{
    GlassPanel *item = new GlassPanel(m_layersContainer);
    item->setFixedHeight(60);
    item->setGlassOpacity(index == m_currentLayerIndex ? 0.2f : 0.08f);
    item->setAccentColor(index == m_currentLayerIndex ? CP_ACCENT_CYAN : QColor(255, 255, 255));
    item->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);

    // Visibility toggle
    GlassButton *visBtn = new GlassButton(layer.visible ? "👁" : "", item);
    visBtn->setFixedSize(30, 30);
    connect(visBtn, &GlassButton::clicked, this, [this, index]() {
        emit layerVisibilityChanged(index, true); // Toggle
    });
    layout->addWidget(visBtn);

    // Thumbnail
    QLabel *thumbLabel = new QLabel(item);
    thumbLabel->setFixedSize(40, 40);
    if (!layer.image.isNull()) {
        QPixmap thumb = QPixmap::fromImage(layer.image.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        thumbLabel->setPixmap(thumb);
    }
    thumbLabel->setStyleSheet("background: rgba(0,0,0,0.3); border-radius: 4px;");
    layout->addWidget(thumbLabel);

    // Name
    QLabel *nameLabel = new QLabel(layer.name, item);
    nameLabel->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    layout->addWidget(nameLabel, 1);

    // Opacity slider
    QSlider *opacitySlider = new QSlider(Qt::Horizontal, item);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(int(layer.opacity * 100));
    opacitySlider->setFixedWidth(60);
    connect(opacitySlider, &QSlider::valueChanged, this, [this, index](int value) {
        emit layerOpacityChanged(index, value / 100.0f);
    });
    layout->addWidget(opacitySlider);

    // Click to select
    connect(item, &GlassPanel::mousePressEvent, this, [this, index]() {
        emit layerSelected(index);
    });

    m_layersLayout->insertWidget(0, item);
}

void LayersPanel::setCurrentLayer(int index)
{
    m_currentLayerIndex = index;
}

void LayersPanel::updateLayer(int index, const Layer &layer)
{
    // Update specific layer UI
}

// ==================== AdjustmentsPanel Implementation ====================

AdjustmentsPanel::AdjustmentsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AdjustmentsPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel(tr("التعديلات"), this);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Auto enhance button
    GlassButton *autoBtn = new GlassButton(tr("✨ تحسين تلقائي"), this);
    autoBtn->setFixedHeight(40);
    autoBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(autoBtn, &GlassButton::clicked, this, &AdjustmentsPanel::autoEnhanceClicked);
    mainLayout->addWidget(autoBtn);

    mainLayout->addSpacing(10);

    // Sliders
    QStringList labels = {
        tr("السطوع"), tr("التباين"), tr("التشبع"),
        tr("درجة اللون"), tr("التعريض"), tr("الإضاءة العالية"),
        tr("الظلال"), tr("الحدة"), tr("الضبابية")
    };

    QStringList names = {
        "brightness", "contrast", "saturation",
        "hue", "exposure", "highlights",
        "shadows", "sharpness", "blur"
    };

    for (int i = 0; i < labels.size(); ++i) {
        mainLayout->addLayout(createSlider(names[i], -100, 100, 0));
    }

    // Reset button
    GlassButton *resetBtn = new GlassButton(tr("إعادة ضبط الكل"), this);
    resetBtn->setFixedHeight(35);
    connect(resetBtn, &GlassButton::clicked, this, &AdjustmentsPanel::resetAll);
    mainLayout->addWidget(resetBtn);

    mainLayout->addStretch();
}

QSlider *AdjustmentsPanel::createSlider(const QString &name, int min, int max, int defaultValue)
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel(name);
    label->setStyleSheet("color: #CCCCCC; font-size: 11px;");
    layout->addWidget(label);

    QHBoxLayout *sliderLayout = new QHBoxLayout();

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    slider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: rgba(0,0,0,0.3); border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 14px; height: 14px; background: #00FFFF; "
        "border-radius: 7px; margin: -4px 0; }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 3px; }"
    );
    sliderLayout->addWidget(slider, 1);

    QLabel *valueLabel = new QLabel("0");
    valueLabel->setStyleSheet("color: #00FFFF; font-size: 10px; min-width: 25px;");
    valueLabel->setAlignment(Qt::AlignRight);
    sliderLayout->addWidget(valueLabel);

    m_sliders[name] = slider;
    m_valueLabels[name] = valueLabel;

    connect(slider, &QSlider::valueChanged, this, [this, name, valueLabel](int value) {
        valueLabel->setText(QString::number(value));
        emit adjustmentChanged(name, value);
    });

    qobject_cast<QVBoxLayout*>(layout())->addLayout(sliderLayout);

    // Add to parent layout
    if (QVBoxLayout *parentLayout = qobject_cast<QVBoxLayout*>(this->layout())) {
        parentLayout->addLayout(layout);
    }

    return slider;
}

void AdjustmentsPanel::resetAll()
{
    for (auto it = m_sliders.begin(); it != m_sliders.end(); ++it) {
        it.value()->setValue(0);
    }
}

void AdjustmentsPanel::setAdjustment(const QString &name, int value)
{
    if (m_sliders.contains(name)) {
        m_sliders[name]->setValue(value);
    }
}

// ==================== ToolsPanel Implementation ====================

ToolsPanel::ToolsPanel(QWidget *parent)
    : QWidget(parent)
    , m_currentTool("brush")
{
    setupUI();
}

void ToolsPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel(tr("الأدوات"), this);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Tools grid
    QGridLayout *toolsGrid = new QGridLayout();
    toolsGrid->setSpacing(5);

    QStringList tools = {
        "move", "select", "crop", "brush", "eraser",
        "clone", "heal", "gradient", "text", "shape"
    };

    QStringList icons = {
        "↖", "▭", "✂", "🖌", "◯",
        "⚫", "✚", "▓", "T", "⬡"
    };

    for (int i = 0; i < tools.size(); ++i) {
        int row = i / 2;
        int col = i % 2;
        createToolButton(tools[i], icons[i], tools[i]);
        if (m_toolButtons.contains(tools[i])) {
            toolsGrid->addWidget(m_toolButtons[tools[i]], row, col);
        }
    }

    mainLayout->addLayout(toolsGrid);

    // Brush settings
    mainLayout->addSpacing(20);

    QLabel *brushLabel = new QLabel(tr("حجم الفرشاة"), this);
    brushLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    mainLayout->addWidget(brushLabel);

    m_brushSizeSlider = new QSlider(Qt::Horizontal, this);
    m_brushSizeSlider->setRange(1, 200);
    m_brushSizeSlider->setValue(20);
    connect(m_brushSizeSlider, &QSlider::valueChanged, this, &ToolsPanel::brushSizeChanged);
    mainLayout->addWidget(m_brushSizeSlider);

    QLabel *hardnessLabel = new QLabel(tr("الصلابة"), this);
    hardnessLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    mainLayout->addWidget(hardnessLabel);

    m_brushHardnessSlider = new QSlider(Qt::Horizontal, this);
    m_brushHardnessSlider->setRange(0, 100);
    m_brushHardnessSlider->setValue(80);
    connect(m_brushHardnessSlider, &QSlider::valueChanged, this, &ToolsPanel::brushHardnessChanged);
    mainLayout->addWidget(m_brushHardnessSlider);

    mainLayout->addStretch();
}

void ToolsPanel::createToolButton(const QString &tool, const QString &icon, const QString &tooltip)
{
    GlassButton *btn = new GlassButton(icon, this);
    btn->setFixedSize(50, 50);
    btn->setCheckable(true);
    btn->setToolTip(tooltip);
    btn->setProperty("tool", tool);

    connect(btn, &GlassButton::clicked, this, [this, tool]() {
        setCurrentTool(tool);
        emit toolSelected(tool);
    });

    m_toolButtons[tool] = btn;
}

void ToolsPanel::setCurrentTool(const QString &tool)
{
    m_currentTool = tool;

    for (auto it = m_toolButtons.begin(); it != m_toolButtons.end(); ++it) {
        it.value()->setChecked(it.key() == tool);
    }
}

void ToolsPanel::setBrushSize(int size)
{
    m_brushSizeSlider->setValue(size);
}

void ToolsPanel::setBrushHardness(int hardness)
{
    m_brushHardnessSlider->setValue(hardness);
}

// ==================== AIPanel Implementation ====================

AIPanel::AIPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AIPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel(tr("🤖 الذكاء الاصطناعي"), this);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { background: rgba(0,0,0,0.3); border-radius: 10px; height: 20px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 10px; }"
    );
    mainLayout->addWidget(m_progressBar);

    // Status label
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    m_statusLabel->setVisible(false);
    mainLayout->addWidget(m_statusLabel);

    mainLayout->addSpacing(10);

    // AI operation buttons
    createAIButton("auto", tr("✨ تحسين تلقائي"),
                   tr("تحسين تلقائي للألوان والتباين"), CP_ACCENT_CYAN);
    createAIButton("bg", tr("🎯 إزالة الخلفية"),
                   tr("إزالة الخلفية تلقائياً"), CP_ACCENT_PINK);
    createAIButton("upscale", tr("📈 تكبير 2x/4x"),
                   tr("تكبير الصورة بدون فقدان الجودة"), CP_ACCENT_PURPLE);
    createAIButton("portrait", tr("👤 تحسين البورتريه"),
                   tr("تحسين الوجوه والبشرة"), QColor(255, 200, 0));
    createAIButton("color", tr("🎨 مطابقة الألوان"),
                   tr("مطابقة ألوان صورة مرجعية"), QColor(0, 255, 128));
    createAIButton("style", tr("🖼 نقل النمط"),
                   tr("تطبيق نمط فني على الصورة"), QColor(255, 128, 0));

    mainLayout->addStretch();
}

void AIPanel::createAIButton(const QString &id, const QString &title,
                              const QString &description, const QColor &accent)
{
    GlassPanel *panel = new GlassPanel(this);
    panel->setFixedHeight(70);
    panel->setGlassOpacity(0.1);
    panel->setAccentColor(accent);
    panel->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(3);

    QLabel *titleLabel = new QLabel(title, panel);
    titleLabel->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: bold;").arg(accent.name()));
    layout->addWidget(titleLabel);

    QLabel *descLabel = new QLabel(description, panel);
    descLabel->setStyleSheet("color: #AAAAAA; font-size: 10px;");
    layout->addWidget(descLabel);

    // Connect click
    panel->setMouseTracking(true);
    connect(panel, &GlassPanel::mousePressEvent, this, [this, id]() {
        if (id == "auto") emit autoEnhanceClicked();
        else if (id == "bg") emit removeBackgroundClicked();
        else if (id == "upscale") emit upscaleClicked(2);
        else if (id == "portrait") emit portraitEnhanceClicked();
        else if (id == "color") emit colorMatchClicked();
        else if (id == "style") emit styleTransferClicked();
    });

    this->layout()->addWidget(panel);
}

void AIPanel::setProcessing(bool processing)
{
    m_progressBar->setVisible(processing);
    m_statusLabel->setVisible(processing);
}

void AIPanel::setProgress(int percent)
{
    m_progressBar->setValue(percent);
}

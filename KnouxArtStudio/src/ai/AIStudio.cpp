#include "AIStudio.h"
#include "../ui/GlassButton.h"
#include "../ui/GlassPanel.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImageReader>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)

// ==================== AIWorker Implementation ====================

AIWorker::AIWorker(QObject *parent)
    : QThread(parent)
    , m_cancelled(false)
{
}

void AIWorker::setTask(const QString &type, const QImage &input, const QVariantMap &params)
{
    m_taskType = type;
    m_inputImage = input;
    m_params = params;
    m_cancelled = false;
}

void AIWorker::cancel()
{
    m_cancelled = true;
}

void AIWorker::run()
{
    QImage result;

    if (m_taskType == "text2image") {
        result = processTextToImage();
    } else if (m_taskType == "image2image") {
        result = processImageToImage();
    } else if (m_taskType == "upscale") {
        result = processUpscale();
    } else if (m_taskType == "removebg") {
        result = processRemoveBackground();
    } else if (m_taskType == "portrait") {
        result = processPortraitEnhance();
    } else if (m_taskType == "colorize") {
        result = processColorize();
    } else if (m_taskType == "style") {
        result = processStyleTransfer();
    }

    if (!m_cancelled) {
        emit resultReady(result);
    }
}

QImage AIWorker::processTextToImage()
{
    // Simulate text-to-image generation with progress
    int steps = m_params.value("steps", 30).toInt();

    QImage result(512, 512, QImage::Format_ARGB32);

    for (int step = 0; step < steps; ++step) {
        if (m_cancelled) return QImage();

        // Simulate processing
        QThread::msleep(50);

        int progress = (step + 1) * 100 / steps;
        emit progressUpdated(progress);

        // Generate noise pattern that gradually becomes more structured
        for (int y = 0; y < result.height(); ++y) {
            for (int x = 0; x < result.width(); ++x) {
                float t = float(step) / steps;
                int noise = QRandomGenerator::global()->bounded(256);

                // Gradient background
                int r = int(50 + 100 * t + noise * (1 - t) * 0.3);
                int g = int(30 + 80 * t + noise * (1 - t) * 0.3);
                int b = int(80 + 120 * t + noise * (1 - t) * 0.3);

                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
    }

    // Add some pattern based on prompt (simplified)
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw abstract shapes
    for (int i = 0; i < 10; ++i) {
        QColor color = QColor(
            QRandomGenerator::global()->bounded(100, 255),
            QRandomGenerator::global()->bounded(100, 255),
            QRandomGenerator::global()->bounded(100, 255),
            100
        );
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);

        int x = QRandomGenerator::global()->bounded(result.width());
        int y = QRandomGenerator::global()->bounded(result.height());
        int r = QRandomGenerator::global()->bounded(50, 150);

        painter.drawEllipse(x - r, y - r, r * 2, r * 2);
    }

    return result;
}

QImage AIWorker::processImageToImage()
{
    if (m_inputImage.isNull()) return QImage();

    int steps = m_params.value("steps", 20).toInt();
    float strength = m_params.value("strength", 0.75f).toFloat();

    QImage result = m_inputImage.copy();

    for (int step = 0; step < steps; ++step) {
        if (m_cancelled) return QImage();

        QThread::msleep(30);
        emit progressUpdated((step + 1) * 100 / steps);
    }

    // Apply transformation based on strength
    QPainter painter(&result);
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.fillRect(result.rect(), QColor(100, 150, 200, int(strength * 100)));

    return result;
}

QImage AIWorker::processUpscale()
{
    if (m_inputImage.isNull()) return QImage();

    int scale = m_params.value("scale", 2).toInt();

    emit progressUpdated(10);
    QThread::msleep(100);

    QImage result = m_inputImage.scaled(
        m_inputImage.width() * scale,
        m_inputImage.height() * scale,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    );

    emit progressUpdated(50);
    QThread::msleep(100);

    // Apply sharpening
    // Simplified sharpening filter
    emit progressUpdated(100);

    return result;
}

QImage AIWorker::processRemoveBackground()
{
    if (m_inputImage.isNull()) return QImage();

    emit progressUpdated(20);
    QThread::msleep(200);

    QImage result(m_inputImage.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    // Simple edge-based background removal simulation
    int threshold = 30;

    for (int y = 0; y < m_inputImage.height(); ++y) {
        emit progressUpdated(20 + (y * 70 / m_inputImage.height()));

        for (int x = 0; x < m_inputImage.width(); ++x) {
            QColor c = m_inputImage.pixelColor(x, y);

            // Detect edges
            bool isEdge = false;
            if (x > 0 && x < m_inputImage.width() - 1 && y > 0 && y < m_inputImage.height() - 1) {
                QColor left = m_inputImage.pixelColor(x - 1, y);
                QColor right = m_inputImage.pixelColor(x + 1, y);
                QColor top = m_inputImage.pixelColor(x, y - 1);
                QColor bottom = m_inputImage.pixelColor(x, y + 1);

                int diff = qAbs(c.lightness() - left.lightness()) +
                          qAbs(c.lightness() - right.lightness()) +
                          qAbs(c.lightness() - top.lightness()) +
                          qAbs(c.lightness() - bottom.lightness());

                isEdge = diff > threshold * 4;
            }

            // Keep pixels that are edges or in center area
            if (isEdge || (x > m_inputImage.width() * 0.1 && x < m_inputImage.width() * 0.9 &&
                          y > m_inputImage.height() * 0.1 && y < m_inputImage.height() * 0.9)) {
                result.setPixelColor(x, y, c);
            }
        }
    }

    emit progressUpdated(100);
    return result;
}

QImage AIWorker::processPortraitEnhance()
{
    if (m_inputImage.isNull()) return QImage();

    emit progressUpdated(10);
    QThread::msleep(100);

    QImage result = m_inputImage.copy();

    // Skin smoothing simulation
    for (int y = 1; y < result.height() - 1; ++y) {
        emit progressUpdated(10 + (y * 80 / result.height()));

        for (int x = 1; x < result.width() - 1; ++x) {
            QColor c = result.pixelColor(x, y);

            // Detect skin tones
            bool isSkin = (c.red() > 60 && c.red() < 255 &&
                          c.green() > 40 && c.green() < 220 &&
                          c.blue() > 20 && c.blue() < 170 &&
                          c.red() > c.green() && c.green() > c.blue());

            if (isSkin) {
                // Apply slight blur
                int r = 0, g = 0, b = 0, a = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        QColor nc = m_inputImage.pixelColor(x + dx, y + dy);
                        r += nc.red();
                        g += nc.green();
                        b += nc.blue();
                        a += nc.alpha();
                    }
                }
                result.setPixelColor(x, y, QColor(r / 9, g / 9, b / 9, a / 9));
            }
        }
    }

    emit progressUpdated(100);
    return result;
}

QImage AIWorker::processColorize()
{
    if (m_inputImage.isNull()) return QImage();

    emit progressUpdated(10);
    QThread::msleep(100);

    QImage result = m_inputImage.copy();

    for (int y = 0; y < result.height(); ++y) {
        emit progressUpdated(10 + (y * 80 / result.height()));

        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int gray = qGray(c.rgb());

            // Apply colorization (simplified - using color temperature)
            int r = qMin(255, gray + 20);
            int g = gray;
            int b = qMax(0, gray - 20);

            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }

    emit progressUpdated(100);
    return result;
}

QImage AIWorker::processStyleTransfer()
{
    if (m_inputImage.isNull()) return QImage();

    emit progressUpdated(10);
    QThread::msleep(200);

    QImage result = m_inputImage.copy();

    // Apply artistic effect based on style
    QString style = m_params.value("style", "abstract").toString();

    if (style == "oil") {
        // Oil painting effect simulation
        for (int y = 1; y < result.height() - 1; y += 2) {
            emit progressUpdated(10 + (y * 80 / result.height()));

            for (int x = 1; x < result.width() - 1; x += 2) {
                QColor c = m_inputImage.pixelColor(x, y);
                result.setPixelColor(x, y, c);
                result.setPixelColor(x + 1, y, c);
                result.setPixelColor(x, y + 1, c);
                result.setPixelColor(x + 1, y + 1, c);
            }
        }
    } else {
        // Abstract style - add color overlay
        QPainter painter(&result);
        painter.setCompositionMode(QPainter::CompositionMode_Overlay);
        painter.fillRect(result.rect(), QColor(100, 150, 200, 50));
    }

    emit progressUpdated(100);
    return result;
}

// ==================== AIStudio Implementation ====================

AIStudio::AIStudio(QWidget *parent)
    : QWidget(parent)
    , m_generationProgress(0.0f)
    , m_isGenerating(false)
    , m_worker(nullptr)
{
    setupUI();
    setupConnections();

    // Initialize models
    m_models = {
        {"sdxl", "Stable Diffusion XL", tr("نموذج توليد صور عالي الجودة"), "text2image", 7000000000, true, false, 1.0f},
        {"realistic", "Realistic Vision", tr("نموذج للصور الواقعية"), "text2image", 4000000000, true, false, 1.0f},
        {"anime", "Anime Diffusion", tr("نموذج لصور الأنمي"), "text2image", 2000000000, true, false, 1.0f},
        {"realesrgan", "Real-ESRGAN", tr("نموذج تكبير الصور"), "upscale", 60000000, true, false, 1.0f},
        {"rembg", "U-2-Net", tr("نموذج إزالة الخلفية"), "segmentation", 170000000, true, false, 1.0f},
        {"gfpgan", "GFPGAN", tr("نموذج تحسين الوجوه"), "enhance", 400000000, false, false, 0.0f}
    };

    // Populate model combo
    for (const AIModel &model : m_models) {
        if (model.category == "text2image") {
            m_modelCombo->addItem(model.name, model.id);
        }
    }
}

AIStudio::~AIStudio()
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->cancel();
        m_worker->wait(5000);
    }
}

void AIStudio::setupUI()
{
    setObjectName("aiStudio");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Left panel - Prompt and settings
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(15);

    createPromptPanel();
    leftLayout->addWidget(m_promptPanel);

    createSettingsPanel();
    leftLayout->addWidget(m_settingsPanel, 1);

    mainLayout->addLayout(leftLayout, 1);

    // Center panel - Preview
    createPreviewPanel();
    mainLayout->addWidget(m_previewPanel, 2);

    // Right panel - History and models
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(15);

    createHistoryPanel();
    rightLayout->addWidget(m_historyPanel, 1);

    createModelsPanel();
    rightLayout->addWidget(m_modelsPanel, 1);

    mainLayout->addLayout(rightLayout, 1);
}

void AIStudio::createPromptPanel()
{
    m_promptPanel = new GlassPanel(this);
    m_promptPanel->setGlassOpacity(0.1);
    m_promptPanel->setAccentColor(CP_ACCENT_CYAN);
    m_promptPanel->setFixedHeight(200);

    QVBoxLayout *layout = new QVBoxLayout(m_promptPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel(tr("🎨 وصف الصورة"), m_promptPanel);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);

    m_promptEdit = new QTextEdit(m_promptPanel);
    m_promptEdit->setPlaceholderText(tr("صف الصورة التي تريد توليدها...\nمثال: منظر طبيعي لجبل مغطى بالثلج عند غروب الشمس"));
    m_promptEdit->setStyleSheet(
        "QTextEdit { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 8px; color: #FFFFFF; padding: 10px; font-size: 13px; }"
        "QTextEdit:focus { border: 1px solid #00FFFF; }"
    );
    m_promptEdit->setFixedHeight(80);
    layout->addWidget(m_promptEdit);

    // Negative prompt
    m_negativePromptEdit = new QTextEdit(m_promptPanel);
    m_negativePromptEdit->setPlaceholderText(tr("ما تريد تجنبه (اختياري)..."));
    m_negativePromptEdit->setStyleSheet(
        "QTextEdit { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 8px; color: #AAAAAA; padding: 8px; font-size: 11px; }"
    );
    m_negativePromptEdit->setFixedHeight(50);
    layout->addWidget(m_negativePromptEdit);

    // Generate button
    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_generateBtn = new GlassButton(tr("✨ توليد"), m_promptPanel);
    m_generateBtn->setFixedHeight(45);
    m_generateBtn->setAccentColor(CP_ACCENT_CYAN);
    btnLayout->addWidget(m_generateBtn);

    m_cancelBtn = new GlassButton(tr("إلغاء"), m_promptPanel);
    m_cancelBtn->setFixedHeight(45);
    m_cancelBtn->setVisible(false);
    m_cancelBtn->setAccentColor(CP_ACCENT_PINK);
    btnLayout->addWidget(m_cancelBtn);

    layout->addLayout(btnLayout);
}

void AIStudio::createPreviewPanel()
{
    m_previewPanel = new GlassPanel(this);
    m_previewPanel->setGlassOpacity(0.08);
    m_previewPanel->setAccentColor(CP_ACCENT_CYAN);
    m_previewPanel->setAcceptDrops(true);

    QVBoxLayout *layout = new QVBoxLayout(m_previewPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    // Title
    QLabel *titleLabel = new QLabel(tr("المعاينة"), m_previewPanel);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // Preview area
    m_previewLabel = new QLabel(m_previewPanel);
    m_previewLabel->setMinimumSize(400, 400);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet(
        "QLabel { background: rgba(0, 0, 0, 0.3); border: 2px dashed rgba(255, 255, 255, 0.2); "
        "border-radius: 12px; color: #888888; }"
    );
    m_previewLabel->setText(tr("اسحب صورة هنا أو اكتب وصفاً للتوليد"));
    layout->addWidget(m_previewLabel, 1);

    // Progress bar
    m_progressBar = new QProgressBar(m_previewPanel);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { background: rgba(0, 0, 0, 0.3); border-radius: 8px; height: 20px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 8px; }"
    );
    layout->addWidget(m_progressBar);

    // Info label
    m_infoLabel = new QLabel(m_previewPanel);
    m_infoLabel->setStyleSheet("color: #888888; font-size: 11px;");
    m_infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_infoLabel);

    // Action buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_saveBtn = new GlassButton(tr("💾 حفظ"), m_previewPanel);
    m_saveBtn->setFixedHeight(40);
    m_saveBtn->setEnabled(false);
    btnLayout->addWidget(m_saveBtn);

    m_useAsInputBtn = new GlassButton(tr("استخدام كمدخل"), m_previewPanel);
    m_useAsInputBtn->setFixedHeight(40);
    m_useAsInputBtn->setEnabled(false);
    btnLayout->addWidget(m_useAsInputBtn);

    layout->addLayout(btnLayout);
}

void AIStudio::createSettingsPanel()
{
    m_settingsPanel = new GlassPanel(this);
    m_settingsPanel->setGlassOpacity(0.1);
    m_settingsPanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_settingsPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    QLabel *titleLabel = new QLabel(tr("⚙ الإعدادات"), m_settingsPanel);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // Model selection
    QLabel *modelLabel = new QLabel(tr("النموذج:"), m_settingsPanel);
    modelLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    layout->addWidget(modelLabel);

    m_modelCombo = new QComboBox(m_settingsPanel);
    m_modelCombo->setStyleSheet(
        "QComboBox { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 6px; color: #FFFFFF; padding: 8px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: #1a1a2e; color: #FFFFFF; selection-background-color: #00FFFF; }"
    );
    layout->addWidget(m_modelCombo);

    // Style selection
    QLabel *styleLabel = new QLabel(tr("النمط:"), m_settingsPanel);
    styleLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    layout->addWidget(styleLabel);

    m_styleCombo = new QComboBox(m_settingsPanel);
    m_styleCombo->addItems({
        tr("افتراضي"), tr("واقعي"), tr("أنمي"),
        tr("فني"), tr("سينمائي"), tr("ثلاثي الأبعاد")
    });
    m_styleCombo->setStyleSheet(m_modelCombo->styleSheet());
    layout->addWidget(m_styleCombo);

    // Strength slider
    QLabel *strengthLabel = new QLabel(tr("قوة التأثير: 75%"), m_settingsPanel);
    strengthLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    layout->addWidget(strengthLabel);

    m_strengthSlider = new QSlider(Qt::Horizontal, m_settingsPanel);
    m_strengthSlider->setRange(0, 100);
    m_strengthSlider->setValue(75);
    m_strengthSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: rgba(0, 0, 0, 0.3); border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 16px; height: 16px; background: #00FFFF; "
        "border-radius: 8px; margin: -5px 0; }"
        "QSlider::sub-page:horizontal { background: #00FFFF; border-radius: 3px; }"
    );
    layout->addWidget(m_strengthSlider);

    // Steps slider
    QLabel *stepsLabel = new QLabel(tr("خطوات التوليد: 30"), m_settingsPanel);
    stepsLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    layout->addWidget(stepsLabel);

    m_stepsSlider = new QSlider(Qt::Horizontal, m_settingsPanel);
    m_stepsSlider->setRange(10, 100);
    m_stepsSlider->setValue(30);
    m_stepsSlider->setStyleSheet(m_strengthSlider->styleSheet());
    layout->addWidget(m_stepsSlider);

    // Guidance scale
    QLabel *guidanceLabel = new QLabel(tr("التوجيه: 7.5"), m_settingsPanel);
    guidanceLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    layout->addWidget(guidanceLabel);

    m_guidanceSlider = new QSlider(Qt::Horizontal, m_settingsPanel);
    m_guidanceSlider->setRange(10, 200);
    m_guidanceSlider->setValue(75);
    m_guidanceSlider->setStyleSheet(m_strengthSlider->styleSheet());
    layout->addWidget(m_guidanceSlider);

    // Seed
    QHBoxLayout *seedLayout = new QHBoxLayout();

    QLabel *seedLabel = new QLabel(tr("البذرة:"), m_settingsPanel);
    seedLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    seedLayout->addWidget(seedLabel);

    m_seedEdit = new QLineEdit(m_settingsPanel);
    m_seedEdit->setText(generateRandomSeed());
    m_seedEdit->setStyleSheet(
        "QLineEdit { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 6px; color: #FFFFFF; padding: 8px; }"
    );
    seedLayout->addWidget(m_seedEdit);

    m_randomSeedBtn = new GlassButton("🎲", m_settingsPanel);
    m_randomSeedBtn->setFixedSize(35, 35);
    m_randomSeedBtn->setToolTip(tr("بذرة عشوائية"));
    seedLayout->addWidget(m_randomSeedBtn);

    layout->addLayout(seedLayout);

    layout->addStretch();
}

void AIStudio::createHistoryPanel()
{
    m_historyPanel = new GlassPanel(this);
    m_historyPanel->setGlassOpacity(0.1);
    m_historyPanel->setAccentColor(CP_ACCENT_CYAN);
    m_historyPanel->setFixedHeight(250);

    QVBoxLayout *layout = new QVBoxLayout(m_historyPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel(tr("📜 السجل"), m_historyPanel);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);

    headerLayout->addStretch();

    GlassButton *clearBtn = new GlassButton(tr("مسح"), m_historyPanel);
    clearBtn->setFixedSize(60, 30);
    connect(clearBtn, &GlassButton::clicked, this, &AIStudio::clearHistory);
    headerLayout->addWidget(clearBtn);

    layout->addLayout(headerLayout);

    // History container
    QScrollArea *scrollArea = new QScrollArea(m_historyPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_historyContainer = new QWidget();
    QHBoxLayout *historyLayout = new QHBoxLayout(m_historyContainer);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    historyLayout->setSpacing(10);
    historyLayout->addStretch();

    scrollArea->setWidget(m_historyContainer);
    layout->addWidget(scrollArea);
}

void AIStudio::createModelsPanel()
{
    m_modelsPanel = new GlassPanel(this);
    m_modelsPanel->setGlassOpacity(0.1);
    m_modelsPanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_modelsPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel(tr("🤖 النماذج"), m_modelsPanel);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // Models list
    QScrollArea *scrollArea = new QScrollArea(m_modelsPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *modelsContainer = new QWidget();
    QVBoxLayout *modelsLayout = new QVBoxLayout(modelsContainer);
    modelsLayout->setContentsMargins(0, 0, 0, 0);
    modelsLayout->setSpacing(10);

    for (const AIModel &model : m_models) {
        ModelCard *card = new ModelCard(model, modelsContainer);
        modelsLayout->addWidget(card);
    }

    modelsLayout->addStretch();
    scrollArea->setWidget(modelsContainer);
    layout->addWidget(scrollArea);
}

void AIStudio::setupConnections()
{
    connect(m_generateBtn, &GlassButton::clicked, this, &AIStudio::onGenerateClicked);
    connect(m_cancelBtn, &GlassButton::clicked, this, &AIStudio::onCancelGenerationClicked);
    connect(m_saveBtn, &GlassButton::clicked, this, &AIStudio::onSaveImageClicked);
    connect(m_useAsInputBtn, &GlassButton::clicked, this, &AIStudio::onUseAsInputClicked);
    connect(m_randomSeedBtn, &GlassButton::clicked, this, &AIStudio::onRandomSeedClicked);

    connect(m_strengthSlider, &QSlider::valueChanged, this, &AIStudio::onStrengthChanged);
    connect(m_stepsSlider, &QSlider::valueChanged, this, &AIStudio::onStepsChanged);
    connect(m_guidanceSlider, &QSlider::valueChanged, this, &AIStudio::onGuidanceChanged);
}

void AIStudio::onGenerateClicked()
{
    QString prompt = m_promptEdit->toPlainText().trimmed();
    if (prompt.isEmpty()) {
        m_infoLabel->setText(tr("الرجاء إدخال وصف للصورة"));
        return;
    }

    generateImage(prompt);
}

void AIStudio::generateImage(const QString &prompt)
{
    QVariantMap params;
    params["steps"] = m_stepsSlider->value();
    params["strength"] = m_strengthSlider->value() / 100.0f;
    params["guidance"] = m_guidanceSlider->value() / 10.0f;
    params["seed"] = m_seedEdit->text();
    params["style"] = m_styleCombo->currentText();

    generateImage(prompt, params);
}

void AIStudio::generateImage(const QString &prompt, const QVariantMap &params)
{
    m_currentPrompt = prompt;
    m_isGenerating = true;
    setProcessingState(true);

    emit processingStarted(tr("توليد الصورة"));

    // Create and start worker
    if (m_worker) {
        m_worker->cancel();
        m_worker->wait();
        delete m_worker;
    }

    m_worker = new AIWorker(this);
    m_worker->setTask("text2image", QImage(), params);

    connect(m_worker, &AIWorker::progressUpdated, this, &AIStudio::onWorkerProgress);
    connect(m_worker, &AIWorker::resultReady, this, &AIStudio::onWorkerFinished);
    connect(m_worker, &AIWorker::errorOccurred, this, &AIStudio::onWorkerError);

    m_worker->start();
}

void AIStudio::onWorkerProgress(int percent)
{
    m_progressBar->setValue(percent);
    emit processingProgress(percent);
}

void AIStudio::onWorkerFinished(const QImage &result)
{
    m_isGenerating = false;
    setProcessingState(false);

    m_currentImage = result;

    // Update preview
    QPixmap pixmap = QPixmap::fromImage(result.scaled(
        m_previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_previewLabel->setPixmap(pixmap);
    m_previewLabel->setText("");

    // Enable buttons
    m_saveBtn->setEnabled(true);
    m_useAsInputBtn->setEnabled(true);

    // Add to history
    addToHistory(result, m_currentPrompt);

    m_infoLabel->setText(tr("تم التوليد بنجاح"));
    emit processingFinished(tr("تم توليد الصورة"));
    emit imageGenerated(result);
}

void AIStudio::onWorkerError(const QString &error)
{
    m_isGenerating = false;
    setProcessingState(false);
    m_infoLabel->setText(tr("خطأ: %1").arg(error));
    emit errorOccurred(error);
}

void AIStudio::onCancelGenerationClicked()
{
    if (m_worker) {
        m_worker->cancel();
    }

    m_isGenerating = false;
    setProcessingState(false);
    m_infoLabel->setText(tr("تم إلغاء التوليد"));
}

void AIStudio::setProcessingState(bool processing)
{
    m_generateBtn->setVisible(!processing);
    m_cancelBtn->setVisible(processing);
    m_progressBar->setVisible(processing);

    if (processing) {
        m_progressBar->setValue(0);
        m_infoLabel->setText(tr("جاري التوليد..."));
    }
}

void AIStudio::onSaveImageClicked()
{
    if (m_currentImage.isNull()) return;

    QString path = QFileDialog::getSaveFileName(this, tr("حفظ الصورة"), QString(),
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;WebP (*.webp)"));

    if (!path.isEmpty()) {
        if (m_currentImage.save(path)) {
            m_infoLabel->setText(tr("تم الحفظ: %1").arg(path));
        } else {
            m_infoLabel->setText(tr("فشل الحفظ"));
        }
    }
}

void AIStudio::onUseAsInputClicked()
{
    if (m_currentImage.isNull()) return;

    m_inputImage = m_currentImage;

    // Switch to image-to-image mode
    m_promptEdit->setPlaceholderText(tr("صف التعديلات التي تريد تطبيقها..."));
    m_infoLabel->setText(tr("تم تعيين الصورة كمدخل"));
}

void AIStudio::onRandomSeedClicked()
{
    m_seedEdit->setText(generateRandomSeed());
}

QString AIStudio::generateRandomSeed()
{
    return QString::number(QRandomGenerator::global()->bounded(1000000000));
}

void AIStudio::onStrengthChanged(int value)
{
    QLabel *label = findChild<QLabel*>("strengthLabel");
    if (label) {
        label->setText(tr("قوة التأثير: %1%").arg(value));
    }
}

void AIStudio::onStepsChanged(int value)
{
    QLabel *label = findChild<QLabel*>("stepsLabel");
    if (label) {
        label->setText(tr("خطوات التوليد: %1").arg(value));
    }
}

void AIStudio::onGuidanceChanged(int value)
{
    QLabel *label = findChild<QLabel*>("guidanceLabel");
    if (label) {
        label->setText(tr("التوجيه: %1").arg(value / 10.0));
    }
}

void AIStudio::addToHistory(const QImage &image, const QString &prompt)
{
    m_historyImages.append(image);
    m_historyPrompts.append(prompt);

    // Create thumbnail
    HistoryThumbnail *thumb = new HistoryThumbnail(image, prompt, m_historyImages.size() - 1, m_historyContainer);
    connect(thumb, &HistoryThumbnail::clicked, this, &AIStudio::onHistoryItemClicked);

    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(m_historyContainer->layout());
    if (layout) {
        layout->insertWidget(layout->count() - 1, thumb);
    }
}

void AIStudio::onHistoryItemClicked(int index)
{
    if (index >= 0 && index < m_historyImages.size()) {
        m_currentImage = m_historyImages[index];

        QPixmap pixmap = QPixmap::fromImage(m_currentImage.scaled(
            m_previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_previewLabel->setPixmap(pixmap);
        m_previewLabel->setText("");

        m_promptEdit->setPlainText(m_historyPrompts[index]);
    }
}

void AIStudio::clearHistory()
{
    m_historyImages.clear();
    m_historyPrompts.clear();

    // Clear UI
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(m_historyContainer->layout());
    if (layout) {
        while (layout->count() > 1) {
            QLayoutItem *item = layout->takeAt(0);
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }
}

void AIStudio::setGenerationProgress(float progress)
{
    m_generationProgress = progress;
    m_progressBar->setValue(int(progress * 100));
}

void AIStudio::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Update preview if image exists
    if (!m_currentImage.isNull() && m_previewLabel->pixmap()) {
        QPixmap pixmap = QPixmap::fromImage(m_currentImage.scaled(
            m_previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_previewLabel->setPixmap(pixmap);
    }
}

// ==================== ModelCard Implementation ====================

ModelCard::ModelCard(const AIModel &model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
{
    setupUI();
}

void ModelCard::setupUI()
{
    setFixedHeight(80);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->setSpacing(10);

    // Info
    QVBoxLayout *infoLayout = new QVBoxLayout();

    m_nameLabel = new QLabel(m_model.name);
    m_nameLabel->setStyleSheet("color: #FFFFFF; font-size: 13px; font-weight: bold;");
    infoLayout->addWidget(m_nameLabel);

    m_descLabel = new QLabel(m_model.description);
    m_descLabel->setStyleSheet("color: #AAAAAA; font-size: 10px;");
    infoLayout->addWidget(m_descLabel);

    // Size
    QString sizeStr;
    if (m_model.size > 1000000000) {
        sizeStr = QString("%1 GB").arg(m_model.size / 1000000000.0, 0, 'f', 1);
    } else {
        sizeStr = QString("%1 MB").arg(m_model.size / 1000000.0, 0, 'f', 1);
    }
    m_sizeLabel = new QLabel(sizeStr);
    m_sizeLabel->setStyleSheet("color: #888888; font-size: 9px;");
    infoLayout->addWidget(m_sizeLabel);

    layout->addLayout(infoLayout, 1);

    // Download progress
    m_downloadProgress = new QProgressBar();
    m_downloadProgress->setRange(0, 100);
    m_downloadProgress->setValue(int(m_model.progress * 100));
    m_downloadProgress->setVisible(!m_model.isDownloaded);
    m_downloadProgress->setFixedWidth(80);
    m_downloadProgress->setStyleSheet(
        "QProgressBar { background: rgba(0, 0, 0, 0.3); border-radius: 4px; height: 8px; }"
        "QProgressBar::chunk { background: #00FFFF; border-radius: 4px; }"
    );
    layout->addWidget(m_downloadProgress);

    // Action button
    m_actionBtn = new GlassButton();
    m_actionBtn->setFixedSize(70, 35);
    updateState();
    layout->addWidget(m_actionBtn);
}

void ModelCard::updateState()
{
    if (!m_model.isDownloaded) {
        m_actionBtn->setText(tr("تحميل"));
        m_actionBtn->setAccentColor(CP_ACCENT_CYAN);
    } else if (!m_model.isLoaded) {
        m_actionBtn->setText(tr("تحميل"));
        m_actionBtn->setAccentColor(CP_ACCENT_CYAN);
    } else {
        m_actionBtn->setText(tr("إلغاء"));
        m_actionBtn->setAccentColor(CP_ACCENT_PINK);
    }
}

// ==================== HistoryThumbnail Implementation ====================

HistoryThumbnail::HistoryThumbnail(const QImage &image, const QString &prompt, int index, QWidget *parent)
    : QWidget(parent)
    , m_prompt(prompt)
    , m_index(index)
    , m_isHovered(false)
{
    setFixedSize(80, 80);
    setCursor(Qt::PointingHandCursor);

    m_thumbnail = image.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void HistoryThumbnail::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    QRect rect = this->rect().adjusted(2, 2, -2, -2);
    painter.fillRect(rect, QColor(40, 40, 60));

    // Thumbnail
    QRect thumbRect((width() - m_thumbnail.width()) / 2,
                    (height() - m_thumbnail.height()) / 2,
                    m_thumbnail.width(), m_thumbnail.height());
    painter.drawImage(thumbRect, m_thumbnail);

    // Border
    if (m_isHovered) {
        painter.setPen(QPen(CP_ACCENT_CYAN, 2));
        painter.drawRect(rect);
    }
}

void HistoryThumbnail::mousePressEvent(QMouseEvent *event)
{
    emit clicked(m_index);
}

void HistoryThumbnail::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    update();
    setToolTip(m_prompt);
}

void HistoryThumbnail::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    update();
}

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../ui/GlassButton.h"
#include "../ui/GlassPanel.h"
#include "../ui/CyberpunkSplash.h"
#include "../ui/HomeScreen.h"
#include "../ui/NotificationManager.h"
#include "../photo_editor/PhotoEditor.h"
#include "../video_editor/VideoEditor.h"
#include "../ai/AIStudio.h"
#include "../ui/SettingsPanel.h"
#include "../body_editor/BodyEditor.h"
#include "../face_editor/FaceRetouch.h"
#include "../makeup/MakeupStudio.h"

#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

// Cyberpunk color palette
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_BG_MID       QColor(20, 20, 45)
#define CP_BG_LIGHT     QColor(35, 35, 70)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)
#define CP_GLOW_CYAN    QColor(0, 255, 255, 80)
#define CP_GLOW_PINK    QColor(255, 0, 128, 80)
#define CP_TEXT_BRIGHT  QColor(255, 255, 255)
#define CP_TEXT_DIM     QColor(180, 180, 200)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_centralStack(nullptr)
    , m_splashScreen(nullptr)
    , m_homeScreen(nullptr)
    , m_photoEditor(nullptr)
    , m_videoEditor(nullptr)
    , m_aiStudio(nullptr)
    , m_bodyEditor(nullptr)
    , m_faceRetouch(nullptr)
    , m_makeupStudio(nullptr)
    , m_settingsPanel(nullptr)
    , m_notificationManager(nullptr)
    , m_sidebar(nullptr)
    , m_topBar(nullptr)
    , m_bottomBar(nullptr)
    , m_homeBtn(nullptr)
    , m_photoBtn(nullptr)
    , m_videoBtn(nullptr)
    , m_aiBtn(nullptr)
    , m_bodyBtn(nullptr)
    , m_faceBtn(nullptr)
    , m_makeupBtn(nullptr)
    , m_settingsBtn(nullptr)
    , m_menuToggleBtn(nullptr)
    , m_glowAnimation(nullptr)
    , m_opacityAnimation(nullptr)
    , m_transitionAnimation(nullptr)
    , m_currentMode(Mode_Home)
    , m_glassOpacity(0.15f)
    , m_glowIntensity(20)
    , m_sidebarExpanded(true)
    , m_glowEffect(nullptr)
    , m_pulseTimer(nullptr)
{
    ui->setupUi(this);

    // Remove default window frame for custom styling
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();
    setupGlassmorphism();
    setupAnimations();
    setupConnections();
    applyCyberpunkTheme();

    // Show splash screen first
    showSplashScreen();

    // Initialize with home mode
    QTimer::singleShot(2500, this, [this]() {
        hideSplashScreen();
        switchMode(Mode_Home);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setMinimumSize(1400, 900);
    resize(1600, 1000);

    // Central widget with transparent background
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create top bar
    createTopBar();
    mainLayout->addWidget(m_topBar, 0);

    // Create content area with sidebar and central stack
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    createSidebar();
    contentLayout->addWidget(m_sidebar, 0);

    createCentralStack();
    contentLayout->addWidget(m_centralStack, 1);

    mainLayout->addLayout(contentLayout, 1);

    // Create bottom bar
    m_bottomBar = new GlassPanel(this);
    m_bottomBar->setFixedHeight(40);
    m_bottomBar->setGlassOpacity(0.1);
    m_bottomBar->setAccentColor(CP_ACCENT_CYAN);
    mainLayout->addWidget(m_bottomBar, 0);

    // Notification manager
    m_notificationManager = new NotificationManager(this);
    m_notificationManager->setParent(centralWidget);
    m_notificationManager->setGeometry(width() - 420, 80, 400, 600);
}

void MainWindow::createTopBar()
{
    m_topBar = new GlassPanel(this);
    m_topBar->setFixedHeight(60);
    m_topBar->setGlassOpacity(0.12);
    m_topBar->setAccentColor(CP_ACCENT_CYAN);
    m_topBar->setGlowIntensity(15);

    QHBoxLayout *layout = new QHBoxLayout(m_topBar);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setSpacing(15);

    // Logo and title
    QLabel *logoLabel = new QLabel("◈", m_topBar);
    logoLabel->setStyleSheet("color: #00FFFF; font-size: 24px; font-weight: bold;");
    layout->addWidget(logoLabel);

    QLabel *titleLabel = new QLabel("KNOUX", m_topBar);
    titleLabel->setStyleSheet(
        "color: #FFFFFF; font-size: 18px; font-weight: bold;"
        "font-family: 'Orbitron', 'Dubai', sans-serif;"
        "text-shadow: 0 0 10px #00FFFF;"
    );
    layout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("ART STUDIO", m_topBar);
    subtitleLabel->setStyleSheet(
        "color: #FF0080; font-size: 12px; font-weight: bold;"
        "font-family: 'Orbitron', sans-serif; margin-left: 5px;"
    );
    layout->addWidget(subtitleLabel);

    layout->addStretch();

    // Window controls
    GlassButton *minBtn = new GlassButton("−", m_topBar);
    minBtn->setFixedSize(40, 30);
    minBtn->setGlassOpacity(0.2);
    connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    layout->addWidget(minBtn);

    GlassButton *maxBtn = new GlassButton("□", m_topBar);
    maxBtn->setFixedSize(40, 30);
    maxBtn->setGlassOpacity(0.2);
    connect(maxBtn, &QPushButton::clicked, this, [this]() {
        isMaximized() ? showNormal() : showMaximized();
    });
    layout->addWidget(maxBtn);

    GlassButton *closeBtn = new GlassButton("×", m_topBar);
    closeBtn->setFixedSize(40, 30);
    closeBtn->setGlassOpacity(0.3);
    closeBtn->setAccentColor(CP_ACCENT_PINK);
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    layout->addWidget(closeBtn);
}

void MainWindow::createSidebar()
{
    m_sidebar = new GlassPanel(this);
    m_sidebar->setFixedWidth(80);
    m_sidebar->setGlassOpacity(0.08);
    m_sidebar->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_sidebar);
    layout->setContentsMargins(10, 20, 10, 20);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Menu toggle button
    m_menuToggleBtn = new GlassButton("☰", m_sidebar);
    m_menuToggleBtn->setFixedSize(50, 50);
    m_menuToggleBtn->setToolTip(tr("تبديل القائمة"));
    connect(m_menuToggleBtn, &QPushButton::clicked, this, &MainWindow::onMenuToggle);
    layout->addWidget(m_menuToggleBtn);

    layout->addSpacing(20);

    // Navigation buttons with icons
    m_homeBtn = new GlassButton("🏠", m_sidebar);
    m_homeBtn->setFixedSize(60, 60);
    m_homeBtn->setCheckable(true);
    m_homeBtn->setToolTip(tr("الرئيسية"));
    connect(m_homeBtn, &QPushButton::clicked, this, &MainWindow::onHomeClicked);
    layout->addWidget(m_homeBtn);

    m_photoBtn = new GlassButton("📷", m_sidebar);
    m_photoBtn->setFixedSize(60, 60);
    m_photoBtn->setCheckable(true);
    m_photoBtn->setToolTip(tr("محرر الصور"));
    connect(m_photoBtn, &QPushButton::clicked, this, &MainWindow::onPhotoClicked);
    layout->addWidget(m_photoBtn);

    m_videoBtn = new GlassButton("🎬", m_sidebar);
    m_videoBtn->setFixedSize(60, 60);
    m_videoBtn->setCheckable(true);
    m_videoBtn->setToolTip(tr("محرر الفيديو"));
    connect(m_videoBtn, &QPushButton::clicked, this, &MainWindow::onVideoClicked);
    layout->addWidget(m_videoBtn);

    m_aiBtn = new GlassButton("🤖", m_sidebar);
    m_aiBtn->setFixedSize(60, 60);
    m_aiBtn->setCheckable(true);
    m_aiBtn->setToolTip(tr("استوديو الذكاء الاصطناعي"));
    connect(m_aiBtn, &QPushButton::clicked, this, &MainWindow::onAIClicked);
    layout->addWidget(m_aiBtn);

    m_bodyBtn = new GlassButton("👤", m_sidebar);
    m_bodyBtn->setFixedSize(60, 60);
    m_bodyBtn->setCheckable(true);
    m_bodyBtn->setToolTip(tr("تعديل الجسم"));
    connect(m_bodyBtn, &QPushButton::clicked, this, &MainWindow::onBodyClicked);
    layout->addWidget(m_bodyBtn);

    m_faceBtn = new GlassButton("😊", m_sidebar);
    m_faceBtn->setFixedSize(60, 60);
    m_faceBtn->setCheckable(true);
    m_faceBtn->setToolTip(tr("ريتوش الوجه"));
    connect(m_faceBtn, &QPushButton::clicked, this, &MainWindow::onFaceClicked);
    layout->addWidget(m_faceBtn);

    m_makeupBtn = new GlassButton("💄", m_sidebar);
    m_makeupBtn->setFixedSize(60, 60);
    m_makeupBtn->setCheckable(true);
    m_makeupBtn->setToolTip(tr("استوديو المكياج"));
    connect(m_makeupBtn, &QPushButton::clicked, this, &MainWindow::onMakeupClicked);
    layout->addWidget(m_makeupBtn);

    layout->addStretch();

    m_settingsBtn = new GlassButton("⚙", m_sidebar);
    m_settingsBtn->setFixedSize(50, 50);
    m_settingsBtn->setToolTip(tr("الإعدادات"));
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    layout->addWidget(m_settingsBtn);
}

void MainWindow::createCentralStack()
{
    m_centralStack = new QStackedWidget(this);
    m_centralStack->setObjectName("centralStack");

    // Create all screens
    m_homeScreen = new HomeScreen(this);
    m_photoEditor = new PhotoEditor(this);
    m_videoEditor = new VideoEditor(this);
    m_aiStudio = new AIStudio(this);
    m_bodyEditor = new BodyEditor(this);
    m_faceRetouch = new FaceRetouch(this);
    m_makeupStudio = new MakeupStudio(this);
    m_settingsPanel = new SettingsPanel(this);

    // Add to stack
    m_centralStack->addWidget(m_homeScreen);
    m_centralStack->addWidget(m_photoEditor);
    m_centralStack->addWidget(m_videoEditor);
    m_centralStack->addWidget(m_aiStudio);
    m_centralStack->addWidget(m_bodyEditor);
    m_centralStack->addWidget(m_faceRetouch);
    m_centralStack->addWidget(m_makeupStudio);
    m_centralStack->addWidget(m_settingsPanel);

    // Connect signals
    connect(m_homeScreen, &HomeScreen::openPhotoEditor, this, &MainWindow::onPhotoClicked);
    connect(m_homeScreen, &HomeScreen::openVideoEditor, this, &MainWindow::onVideoClicked);
    connect(m_homeScreen, &HomeScreen::openAIStudio, this, &MainWindow::onAIClicked);
    connect(m_homeScreen, &HomeScreen::openBodyEditor, this, &MainWindow::onBodyClicked);
    connect(m_homeScreen, &HomeScreen::openFaceRetouch, this, &MainWindow::onFaceClicked);
    connect(m_homeScreen, &HomeScreen::openMakeupStudio, this, &MainWindow::onMakeupClicked);

    connect(m_photoEditor, &PhotoEditor::statusMessage, this, [this](const QString& msg) {
        showNotification(tr("محرر الصور"), msg, 0);
    });

    connect(m_aiStudio, &AIStudio::processingStarted, this, &MainWindow::onAIProcessingStarted);
    connect(m_aiStudio, &AIStudio::processingProgress, this, &MainWindow::onAIProcessingProgress);
    connect(m_aiStudio, &AIStudio::processingFinished, this, &MainWindow::onAIProcessingFinished);
}

void MainWindow::setupGlassmorphism()
{
    // Apply glassmorphism effect to all panels
    QList<GlassPanel*> panels = findChildren<GlassPanel*>();
    for (GlassPanel* panel : panels) {
        panel->setBlurRadius(20);
        panel->setGlassOpacity(m_glassOpacity);
        panel->setBorderRadius(12);
        panel->setBorderWidth(1);
        panel->setBorderColor(QColor(255, 255, 255, 30));
    }
}

void MainWindow::setupAnimations()
{
    // Glow intensity animation
    m_glowAnimation = new QPropertyAnimation(this, "glowIntensity", this);
    m_glowAnimation->setDuration(2000);
    m_glowAnimation->setStartValue(15);
    m_glowAnimation->setEndValue(35);
    m_glowAnimation->setEasingCurve(QEasingCurve::InOutSine);

    // Pulse timer for continuous glow effect
    m_pulseTimer = new QTimer(this);
    connect(m_pulseTimer, &QTimer::timeout, this, &MainWindow::pulseGlowEffect);
    m_pulseTimer->start(100);

    // Transition animation group
    m_transitionAnimation = new QParallelAnimationGroup(this);
}

void MainWindow::setupConnections()
{
    // Install event filter for global shortcuts
    qApp->installEventFilter(this);
}

void MainWindow::applyCyberpunkTheme()
{
    // Set application-wide stylesheet
    QString cyberpunkStyle = R"(
        QWidget {
            font-family: 'Dubai', 'Segoe UI', sans-serif;
            color: #FFFFFF;
        }

        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #0A0A19,
                stop:0.5 #14142D,
                stop:1 #1E1E3D);
        }

        QToolTip {
            background-color: rgba(20, 20, 45, 220);
            color: #00FFFF;
            border: 1px solid #00FFFF;
            border-radius: 6px;
            padding: 8px;
            font-size: 12px;
        }

        QScrollBar:vertical {
            background: rgba(0, 0, 0, 50);
            width: 10px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00FFFF,
                stop:1 #FF0080);
            border-radius: 5px;
            min-height: 30px;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background: rgba(0, 0, 0, 50);
            height: 10px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #00FFFF,
                stop:1 #FF0080);
            border-radius: 5px;
            min-width: 30px;
        }
    )";

    setStyleSheet(cyberpunkStyle);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw cyberpunk background gradient
    QLinearGradient bgGradient(0, 0, width(), height());
    bgGradient.setColorAt(0, CP_BG_DARK);
    bgGradient.setColorAt(0.5, CP_BG_MID);
    bgGradient.setColorAt(1, QColor(25, 25, 55));

    painter.fillRect(rect(), bgGradient);

    // Draw subtle grid pattern
    painter.setPen(QPen(QColor(0, 255, 255, 8), 1));
    int gridSize = 50;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }

    // Draw corner accents
    int cornerSize = 30;
    painter.setPen(QPen(CP_ACCENT_CYAN, 2));

    // Top-left corner
    painter.drawLine(0, cornerSize, 0, 0);
    painter.drawLine(0, 0, cornerSize, 0);

    // Top-right corner
    painter.drawLine(width() - cornerSize, 0, width(), 0);
    painter.drawLine(width(), 0, width(), cornerSize);

    // Bottom-left corner
    painter.drawLine(0, height() - cornerSize, 0, height());
    painter.drawLine(0, height(), cornerSize, height());

    // Bottom-right corner
    painter.drawLine(width() - cornerSize, height(), width(), height());
    painter.drawLine(width(), height() - cornerSize, width(), height());

    QMainWindow::paintEvent(event);
}

void MainWindow::switchMode(AppMode mode)
{
    if (m_currentMode == mode) return;

    // Uncheck all buttons
    m_homeBtn->setChecked(false);
    m_photoBtn->setChecked(false);
    m_videoBtn->setChecked(false);
    m_aiBtn->setChecked(false);
    m_bodyBtn->setChecked(false);
    m_faceBtn->setChecked(false);
    m_makeupBtn->setChecked(false);
    m_settingsBtn->setChecked(false);

    // Check active button
    switch (mode) {
        case Mode_Home: m_homeBtn->setChecked(true); break;
        case Mode_Photo: m_photoBtn->setChecked(true); break;
        case Mode_Video: m_videoBtn->setChecked(true); break;
        case Mode_AI: m_aiBtn->setChecked(true); break;
        case Mode_Body: m_bodyBtn->setChecked(true); break;
        case Mode_Face: m_faceBtn->setChecked(true); break;
        case Mode_Makeup: m_makeupBtn->setChecked(true); break;
        case Mode_Settings: m_settingsBtn->setChecked(true); break;
    }

    // Animate transition
    QWidget *currentWidget = m_centralStack->currentWidget();

    QPropertyAnimation *fadeOut = new QPropertyAnimation(currentWidget, "windowOpacity");
    fadeOut->setDuration(150);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this, mode]() {
        m_centralStack->setCurrentIndex(static_cast<int>(mode));

        QWidget *newWidget = m_centralStack->currentWidget();
        newWidget->setWindowOpacity(0.0);

        QPropertyAnimation *fadeIn = new QPropertyAnimation(newWidget, "windowOpacity");
        fadeIn->setDuration(200);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);

    m_currentMode = mode;
    emit modeChanged(mode);

    showNotification(tr("وضع جديد"), tr("تم التبديل إلى: %1").arg(
        mode == Mode_Home ? tr("الرئيسية") :
        mode == Mode_Photo ? tr("محرر الصور") :
        mode == Mode_Video ? tr("محرر الفيديو") :
        mode == Mode_AI ? tr("استوديو AI") :
        mode == Mode_Body ? tr("تعديل الجسم") :
        mode == Mode_Face ? tr("ريتوش الوجه") :
        mode == Mode_Makeup ? tr("استوديو المكياج") : tr("الإعدادات")
    ), 0);
}

void MainWindow::showSplashScreen()
{
    if (!m_splashScreen) {
        m_splashScreen = new CyberpunkSplash(this);
    }
    m_splashScreen->show();
    m_splashScreen->raise();
    m_splashScreen->activateWindow();
}

void MainWindow::hideSplashScreen()
{
    if (m_splashScreen) {
        QPropertyAnimation *fadeOut = new QPropertyAnimation(m_splashScreen, "windowOpacity");
        fadeOut->setDuration(500);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, m_splashScreen, &QWidget::hide);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void MainWindow::showNotification(const QString& title, const QString& message, int type)
{
    if (m_notificationManager) {
        m_notificationManager->showNotification(title, message, type);
    }
}

void MainWindow::onAIProcessingStarted(const QString& operation)
{
    showNotification(tr("AI Processing"), tr("بدء: %1").arg(operation), 1);
    m_aiBtn->setProperty("processing", true);
    m_aiBtn->style()->unpolish(m_aiBtn);
    m_aiBtn->style()->polish(m_aiBtn);
}

void MainWindow::onAIProcessingProgress(int percent)
{
    // Update progress indicator
    m_bottomBar->setProperty("progress", percent);
}

void MainWindow::onAIProcessingFinished(const QString& result)
{
    showNotification(tr("AI Complete"), result, 0);
    m_aiBtn->setProperty("processing", false);
    m_aiBtn->style()->unpolish(m_aiBtn);
    m_aiBtn->style()->polish(m_aiBtn);
}

void MainWindow::onOpenFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("فتح ملف"), QString(),
        tr("الصور (*.png *.jpg *.jpeg *.bmp *.tiff);;الفيديو (*.mp4 *.mov *.avi);;كل الملفات (*)"));

    if (!path.isEmpty()) {
        if (m_currentMode == Mode_Photo) {
            m_photoEditor->openImage(path);
        } else if (m_currentMode == Mode_Video) {
            m_videoEditor->openVideo(path);
        }
    }
}

void MainWindow::onSaveFile()
{
    if (m_currentMode == Mode_Photo) {
        m_photoEditor->saveImage();
    } else if (m_currentMode == Mode_Video) {
        m_videoEditor->saveProject();
    }
}

void MainWindow::onExportFile()
{
    if (m_currentMode == Mode_Photo) {
        m_photoEditor->exportImage();
    } else if (m_currentMode == Mode_Video) {
        m_videoEditor->exportVideo();
    }
}

void MainWindow::onHomeClicked() { switchMode(Mode_Home); }
void MainWindow::onPhotoClicked() { switchMode(Mode_Photo); }
void MainWindow::onVideoClicked() { switchMode(Mode_Video); }
void MainWindow::onAIClicked() { switchMode(Mode_AI); }
void MainWindow::onBodyClicked() { switchMode(Mode_Body); }
void MainWindow::onFaceClicked() { switchMode(Mode_Face); }
void MainWindow::onMakeupClicked() { switchMode(Mode_Makeup); }
void MainWindow::onSettingsClicked() { switchMode(Mode_Settings); }

void MainWindow::onMenuToggle()
{
    m_sidebarExpanded = !m_sidebarExpanded;

    QPropertyAnimation *widthAnim = new QPropertyAnimation(m_sidebar, "minimumWidth");
    widthAnim->setDuration(250);
    widthAnim->setEasingCurve(QEasingCurve::InOutCubic);
    widthAnim->setStartValue(m_sidebar->width());
    widthAnim->setEndValue(m_sidebarExpanded ? 80 : 50);
    widthAnim->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *widthAnim2 = new QPropertyAnimation(m_sidebar, "maximumWidth");
    widthAnim2->setDuration(250);
    widthAnim2->setEasingCurve(QEasingCurve::InOutCubic);
    widthAnim2->setStartValue(m_sidebar->width());
    widthAnim2->setEndValue(m_sidebarExpanded ? 80 : 50);
    widthAnim2->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::pulseGlowEffect()
{
    static bool increasing = true;
    static int intensity = 15;

    if (increasing) {
        intensity += 1;
        if (intensity >= 35) increasing = false;
    } else {
        intensity -= 1;
        if (intensity <= 15) increasing = true;
    }

    setGlowIntensity(intensity);
}

void MainWindow::setGlassOpacity(float opacity)
{
    m_glassOpacity = opacity;
    QList<GlassPanel*> panels = findChildren<GlassPanel*>();
    for (GlassPanel* panel : panels) {
        panel->setGlassOpacity(opacity);
    }
}

void MainWindow::setGlowIntensity(int intensity)
{
    m_glowIntensity = intensity;
    if (m_sidebar) m_sidebar->setGlowIntensity(intensity);
    if (m_topBar) m_topBar->setGlowIntensity(intensity / 2);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_notificationManager) {
        m_notificationManager->setGeometry(width() - 420, 80, 400, height() - 160);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_F11:
            isFullScreen() ? showNormal() : showFullScreen();
            break;
        case Qt::Key_Escape:
            if (isFullScreen()) showNormal();
            break;
        case Qt::Key_O:
            if (event->modifiers() & Qt::ControlModifier) onOpenFile();
            break;
        case Qt::Key_S:
            if (event->modifiers() & Qt::ControlModifier) onSaveFile();
            break;
        case Qt::Key_E:
            if (event->modifiers() & Qt::ControlModifier) onExportFile();
            break;
        default:
            QMainWindow::keyPressEvent(event);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Global shortcuts
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Ctrl+1-8 for mode switching
        if (keyEvent->modifiers() & Qt::ControlModifier) {
            switch (keyEvent->key()) {
                case Qt::Key_1: onHomeClicked(); return true;
                case Qt::Key_2: onPhotoClicked(); return true;
                case Qt::Key_3: onVideoClicked(); return true;
                case Qt::Key_4: onAIClicked(); return true;
                case Qt::Key_5: onBodyClicked(); return true;
                case Qt::Key_6: onFaceClicked(); return true;
                case Qt::Key_7: onMakeupClicked(); return true;
                case Qt::Key_8: onSettingsClicked(); return true;
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

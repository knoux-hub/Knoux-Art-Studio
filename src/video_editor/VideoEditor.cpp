#include "VideoEditor.h"
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
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QHeaderView>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)

VideoEditor::VideoEditor(QWidget *parent)
    : QWidget(parent)
    , m_mediaPlayer(nullptr)
    , m_audioOutput(nullptr)
    , m_videoWidget(nullptr)
    , m_videoPreview(nullptr)
    , m_timeline(nullptr)
    , m_effectsList(nullptr)
    , m_transitionsList(nullptr)
    , m_playheadPosition(0)
    , m_totalDuration(0)
    , m_zoomLevel(1.0)
    , m_currentTrack(0)
    , m_isModified(false)
    , m_isPlaying(false)
    , m_isExporting(false)
    , m_isAIProcessing(false)
    , m_aiProgressTimer(nullptr)
{
    setupUI();
    setupConnections();
    setupShortcuts();

    // Initialize media player
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.7);
}

VideoEditor::~VideoEditor()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
}

void VideoEditor::setupUI()
{
    setObjectName("videoEditor");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Top toolbar
    createTopToolbar();
    mainLayout->addWidget(m_topToolbar, 0);

    // Main content area
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Preview area
    createPreviewArea();
    contentLayout->addWidget(m_previewPanel, 1);

    // Side panels
    createSidePanels();
    contentLayout->addWidget(m_propertiesPanel, 0);

    mainLayout->addLayout(contentLayout, 1);

    // Timeline
    createTimeline();
    mainLayout->addWidget(m_timelinePanel, 0);

    // Bottom bar
    createBottomBar();
    mainLayout->addWidget(m_bottomBar, 0);
}

void VideoEditor::createTopToolbar()
{
    m_topToolbar = new GlassPanel(this);
    m_topToolbar->setFixedHeight(60);
    m_topToolbar->setGlassOpacity(0.1);
    m_topToolbar->setAccentColor(CP_ACCENT_CYAN);

    QHBoxLayout *layout = new QHBoxLayout(m_topToolbar);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(10);

    // File operations
    m_importBtn = new GlassButton("📁", m_topToolbar);
    m_importBtn->setFixedSize(40, 40);
    m_importBtn->setToolTip(tr("استيراد وسائط"));
    connect(m_importBtn, &GlassButton::clicked, this, &VideoEditor::importVideo);
    layout->addWidget(m_importBtn);

    m_saveBtn = new GlassButton("💾", m_topToolbar);
    m_saveBtn->setFixedSize(40, 40);
    m_saveBtn->setToolTip(tr("حفظ المشروع (Ctrl+S)"));
    connect(m_saveBtn, &GlassButton::clicked, this, &VideoEditor::saveProject);
    layout->addWidget(m_saveBtn);

    layout->addSpacing(20);

    // Undo/Redo
    m_undoBtn = new GlassButton("↩", m_topToolbar);
    m_undoBtn->setFixedSize(40, 40);
    m_undoBtn->setToolTip(tr("تراجع"));
    layout->addWidget(m_undoBtn);

    m_redoBtn = new GlassButton("↪", m_topToolbar);
    m_redoBtn->setFixedSize(40, 40);
    m_redoBtn->setToolTip(tr("إعادة"));
    layout->addWidget(m_redoBtn);

    layout->addSpacing(20);

    // Playback controls
    m_goStartBtn = new GlassButton("⏮", m_topToolbar);
    m_goStartBtn->setFixedSize(40, 40);
    m_goStartBtn->setToolTip(tr("الذهاب للبداية"));
    connect(m_goStartBtn, &GlassButton::clicked, this, &VideoEditor::goToStart);
    layout->addWidget(m_goStartBtn);

    m_prevFrameBtn = new GlassButton("⏪", m_topToolbar);
    m_prevFrameBtn->setFixedSize(40, 40);
    m_prevFrameBtn->setToolTip(tr("إطار سابق"));
    connect(m_prevFrameBtn, &GlassButton::clicked, this, &VideoEditor::frameBack);
    layout->addWidget(m_prevFrameBtn);

    m_playBtn = new GlassButton("▶", m_topToolbar);
    m_playBtn->setFixedSize(50, 50);
    m_playBtn->setToolTip(tr("تشغيل (Space)"));
    m_playBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(m_playBtn, &GlassButton::clicked, this, &VideoEditor::play);
    layout->addWidget(m_playBtn);

    m_pauseBtn = new GlassButton("⏸", m_topToolbar);
    m_pauseBtn->setFixedSize(50, 50);
    m_pauseBtn->setToolTip(tr("إيقاف مؤقت"));
    m_pauseBtn->setVisible(false);
    connect(m_pauseBtn, &GlassButton::clicked, this, &VideoEditor::pause);
    layout->addWidget(m_pauseBtn);

    m_stopBtn = new GlassButton("⏹", m_topToolbar);
    m_stopBtn->setFixedSize(40, 40);
    m_stopBtn->setToolTip(tr("إيقاف"));
    connect(m_stopBtn, &GlassButton::clicked, this, &VideoEditor::stop);
    layout->addWidget(m_stopBtn);

    m_nextFrameBtn = new GlassButton("⏩", m_topToolbar);
    m_nextFrameBtn->setFixedSize(40, 40);
    m_nextFrameBtn->setToolTip(tr("إطار تالي"));
    connect(m_nextFrameBtn, &GlassButton::clicked, this, &VideoEditor::frameStep);
    layout->addWidget(m_nextFrameBtn);

    m_goEndBtn = new GlassButton("⏭", m_topToolbar);
    m_goEndBtn->setFixedSize(40, 40);
    m_goEndBtn->setToolTip(tr("الذهاب للنهاية"));
    connect(m_goEndBtn, &GlassButton::clicked, this, &VideoEditor::goToEnd);
    layout->addWidget(m_goEndBtn);

    layout->addSpacing(20);

    // Edit controls
    m_splitBtn = new GlassButton("✂", m_topToolbar);
    m_splitBtn->setFixedSize(40, 40);
    m_splitBtn->setToolTip(tr("قص عند المؤشر (C)"));
    connect(m_splitBtn, &GlassButton::clicked, this, &VideoEditor::splitClipAtPlayhead);
    layout->addWidget(m_splitBtn);

    m_deleteBtn = new GlassButton("🗑", m_topToolbar);
    m_deleteBtn->setFixedSize(40, 40);
    m_deleteBtn->setToolTip(tr("حذف المحدد (Delete)"));
    connect(m_deleteBtn, &GlassButton::clicked, this, &VideoEditor::deleteSelected);
    layout->addWidget(m_deleteBtn);

    m_markerBtn = new GlassButton("🔖", m_topToolbar);
    m_markerBtn->setFixedSize(40, 40);
    m_markerBtn->setToolTip(tr("إضافة علامة (M)"));
    connect(m_markerBtn, &GlassButton::clicked, this, &VideoEditor::addMarker);
    layout->addWidget(m_markerBtn);

    layout->addStretch();

    // Export
    m_exportBtn = new GlassButton("📤", m_topToolbar);
    m_exportBtn->setFixedSize(45, 45);
    m_exportBtn->setToolTip(tr("تصدير الفيديو (Ctrl+E)"));
    m_exportBtn->setAccentColor(CP_ACCENT_PINK);
    connect(m_exportBtn, &GlassButton::clicked, this, &VideoEditor::exportVideo);
    layout->addWidget(m_exportBtn);
}

void VideoEditor::createPreviewArea()
{
    m_previewPanel = new GlassPanel(this);
    m_previewPanel->setGlassOpacity(0.05);
    m_previewPanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_previewPanel);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    // Video preview
    m_videoPreview = new VideoPreview(m_previewPanel);
    m_videoPreview->setMinimumSize(480, 270);
    m_videoPreview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create video widget
    m_videoWidget = new QVideoWidget(m_videoPreview);
    m_videoWidget->setGeometry(m_videoPreview->rect());

    if (m_mediaPlayer) {
        m_mediaPlayer->setVideoOutput(m_videoWidget);
    }

    layout->addWidget(m_videoPreview, 1);

    // Timecode display
    QHBoxLayout *timeLayout = new QHBoxLayout();

    m_timecodeLabel = new QLabel("00:00:00:00", m_previewPanel);
    m_timecodeLabel->setStyleSheet(
        "color: #00FFFF; font-size: 24px; font-family: 'Consolas', monospace;"
        "font-weight: bold; padding: 5px 15px;"
        "background: rgba(0, 0, 0, 0.5); border-radius: 8px;"
    );
    timeLayout->addWidget(m_timecodeLabel);

    timeLayout->addStretch();

    m_durationLabel = new QLabel("/ 00:00:00:00", m_previewPanel);
    m_durationLabel->setStyleSheet(
        "color: #888888; font-size: 18px; font-family: 'Consolas', monospace;"
    );
    timeLayout->addWidget(m_durationLabel);

    layout->addLayout(timeLayout);
}

void VideoEditor::createTimeline()
{
    m_timelinePanel = new GlassPanel(this);
    m_timelinePanel->setFixedHeight(280);
    m_timelinePanel->setGlassOpacity(0.08);
    m_timelinePanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_timelinePanel);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);

    // Timeline header with zoom controls
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *timelineLabel = new QLabel(tr("الخط الزمني"), m_timelinePanel);
    timelineLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    headerLayout->addWidget(timelineLabel);

    headerLayout->addStretch();

    QLabel *zoomLabel = new QLabel(tr("تكبير:"), m_timelinePanel);
    zoomLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    headerLayout->addWidget(zoomLabel);

    m_zoomSlider = new QSlider(Qt::Horizontal, m_timelinePanel);
    m_zoomSlider->setRange(10, 500);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setFixedWidth(150);
    m_zoomSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: rgba(0,0,0,0.3); border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 12px; height: 12px; background: #00FFFF; "
        "border-radius: 6px; margin: -4px 0; }"
    );
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int value) {
        setZoomLevel(value / 100.0);
    });
    headerLayout->addWidget(m_zoomSlider);

    layout->addLayout(headerLayout);

    // Timeline widget
    m_timeline = new TimelineWidget(m_timelinePanel);
    m_timeline->setDuration(0);
    layout->addWidget(m_timeline, 1);

    // Export progress bar (hidden by default)
    m_exportProgressBar = new QProgressBar(m_timelinePanel);
    m_exportProgressBar->setRange(0, 100);
    m_exportProgressBar->setValue(0);
    m_exportProgressBar->setVisible(false);
    m_exportProgressBar->setStyleSheet(
        "QProgressBar { background: rgba(0,0,0,0.3); border-radius: 4px; height: 20px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #00FFFF, stop:1 #FF0080); border-radius: 4px; }"
    );
    layout->addWidget(m_exportProgressBar);
}

void VideoEditor::createSidePanels()
{
    m_propertiesPanel = new GlassPanel(this);
    m_propertiesPanel->setFixedWidth(280);
    m_propertiesPanel->setGlassOpacity(0.1);
    m_propertiesPanel->setAccentColor(CP_ACCENT_CYAN);

    QVBoxLayout *layout = new QVBoxLayout(m_propertiesPanel);
    layout->setContentsMargins(10, 15, 10, 15);
    layout->setSpacing(15);

    // Tabs
    QHBoxLayout *tabLayout = new QHBoxLayout();

    GlassButton *propertiesTab = new GlassButton(tr("خصائص"), m_propertiesPanel);
    propertiesTab->setCheckable(true);
    propertiesTab->setChecked(true);
    propertiesTab->setFixedHeight(35);
    tabLayout->addWidget(propertiesTab);

    GlassButton *effectsTab = new GlassButton(tr("تأثيرات"), m_propertiesPanel);
    effectsTab->setCheckable(true);
    effectsTab->setFixedHeight(35);
    tabLayout->addWidget(effectsTab);

    GlassButton *aiTab = new GlassButton(tr("AI"), m_propertiesPanel);
    aiTab->setCheckable(true);
    aiTab->setFixedHeight(35);
    tabLayout->addWidget(aiTab);

    layout->addLayout(tabLayout);

    // Stacked panels
    QStackedWidget *stack = new QStackedWidget(m_propertiesPanel);

    // Properties panel
    QWidget *propertiesWidget = new QWidget();
    QVBoxLayout *propsLayout = new QVBoxLayout(propertiesWidget);
    propsLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *clipPropsLabel = new QLabel(tr("خصائص المقطع"));
    clipPropsLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    propsLayout->addWidget(clipPropsLabel);

    // Position
    QGridLayout *posLayout = new QGridLayout();
    posLayout->addWidget(new QLabel(tr("البداية:")), 0, 0);
    posLayout->addWidget(new QLineEdit("00:00:00:00"), 0, 1);
    posLayout->addWidget(new QLabel(tr("المدة:")), 1, 0);
    posLayout->addWidget(new QLineEdit("00:00:05:00"), 1, 1);
    propsLayout->addLayout(posLayout);

    propsLayout->addSpacing(15);

    // Speed
    QLabel *speedLabel = new QLabel(tr("السرعة: 100%"));
    speedLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    propsLayout->addWidget(speedLabel);

    QSlider *speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(25, 400);
    speedSlider->setValue(100);
    propsLayout->addWidget(speedSlider);

    propsLayout->addSpacing(15);

    // Opacity
    QLabel *opacityLabel = new QLabel(tr("الشفافية: 100%"));
    opacityLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    propsLayout->addWidget(opacityLabel);

    QSlider *opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setRange(0, 100);
    opacitySlider->setValue(100);
    propsLayout->addWidget(opacitySlider);

    propsLayout->addSpacing(15);

    // Volume
    QLabel *volumeLabel = new QLabel(tr("الصوت: 100%"));
    volumeLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    propsLayout->addWidget(volumeLabel);

    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &VideoEditor::onVolumeChanged);
    propsLayout->addWidget(m_volumeSlider);

    propsLayout->addStretch();

    stack->addWidget(propertiesWidget);

    // Effects panel
    m_effectsList = new EffectsPanel();
    stack->addWidget(m_effectsList);

    // AI panel
    QWidget *aiWidget = new QWidget();
    QVBoxLayout *aiLayout = new QVBoxLayout(aiWidget);
    aiLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *aiTitle = new QLabel(tr("🤖 عمليات AI"));
    aiTitle->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    aiLayout->addWidget(aiTitle);

    QStringList aiOps = {
        tr("✂ قص تلقائي"), tr("📹 تثبيت"), tr("🎨 تصحيح الألوان"),
        tr("🎯 إزالة خلفية"), tr("✨ تحسين"), tr("📝 توليد ترجمة")
    };

    for (const QString &op : aiOps) {
        GlassButton *btn = new GlassButton(op, aiWidget);
        btn->setFixedHeight(45);
        aiLayout->addWidget(btn);
    }

    aiLayout->addStretch();
    stack->addWidget(aiWidget);

    layout->addWidget(stack, 1);

    // Tab connections
    connect(propertiesTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(0); });
    connect(effectsTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(1); });
    connect(aiTab, &GlassButton::clicked, [stack]() { stack->setCurrentIndex(2); });
}

void VideoEditor::createBottomBar()
{
    m_bottomBar = new GlassPanel(this);
    m_bottomBar->setFixedHeight(35);
    m_bottomBar->setGlassOpacity(0.08);

    QHBoxLayout *layout = new QHBoxLayout(m_bottomBar);
    layout->setContentsMargins(15, 5, 15, 5);

    m_statusLabel = new QLabel(tr("جاهز"), m_bottomBar);
    m_statusLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    layout->addWidget(m_statusLabel);

    layout->addStretch();

    QLabel *infoLabel = new QLabel("0 مقاطع | 0:00:00", m_bottomBar);
    infoLabel->setObjectName("infoLabel");
    infoLabel->setStyleSheet("color: #AAAAAA; font-size: 11px;");
    layout->addWidget(infoLabel);
}

void VideoEditor::setupConnections()
{
    // Media player connections
    if (m_mediaPlayer) {
        connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged,
                this, &VideoEditor::onMediaStatusChanged);
        connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged,
                this, &VideoEditor::onPlaybackStateChanged);
        connect(m_mediaPlayer, &QMediaPlayer::positionChanged,
                this, &VideoEditor::onPositionChanged);
        connect(m_mediaPlayer, &QMediaPlayer::durationChanged,
                this, &VideoEditor::onDurationChanged);
    }

    // Timeline connections
    connect(m_timeline, &TimelineWidget::playheadChanged,
            this, &VideoEditor::seek);
    connect(m_timeline, &TimelineWidget::clipClicked,
            this, &VideoEditor::onClipClicked);
    connect(m_timeline, &TimelineWidget::timelineClicked,
            this, &VideoEditor::onTimelineClicked);
}

void VideoEditor::setupShortcuts()
{
    // Shortcuts handled in keyPressEvent
}

void VideoEditor::openVideo(const QString &path)
{
    if (m_mediaPlayer) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(path));

        // Create clip from video
        VideoClip clip;
        clip.id = QUuid::createUuid().toString();
        clip.filePath = path;
        clip.name = QFileInfo(path).fileName();
        clip.startTime = 0;
        clip.timelineStart = 0;
        clip.track = 0;
        clip.speed = 1.0;
        clip.opacity = 1.0;
        clip.volume = 100;
        clip.muted = false;

        m_videoClips.append(clip);
        m_timeline->setVideoClips(m_videoClips);

        emit statusMessage(tr("تم فتح: %1").arg(clip.name));
    }
}

void VideoEditor::importVideo()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("استيراد فيديو"), QString(),
        tr("الفيديو (*.mp4 *.mov *.avi *.mkv *.webm);;كل الملفات (*)"));

    for (const QString &path : paths) {
        openVideo(path);
    }
}

void VideoEditor::importAudio()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("استيراد صوت"), QString(),
        tr("الصوت (*.mp3 *.wav *.aac *.flac *.ogg);;كل الملفات (*)"));

    for (const QString &path : paths) {
        AudioClip clip;
        clip.id = QUuid::createUuid().toString();
        clip.filePath = path;
        clip.name = QFileInfo(path).fileName();
        clip.startTime = 0;
        clip.timelineStart = 0;
        clip.track = 0;
        clip.volume = 100;
        clip.muted = false;

        m_audioClips.append(clip);
    }

    m_timeline->setAudioClips(m_audioClips);
}

void VideoEditor::importImage()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("استيراد صورة"), QString(),
        tr("الصور (*.png *.jpg *.jpeg *.bmp *.tiff);;كل الملفات (*)"));

    for (const QString &path : paths) {
        VideoClip clip;
        clip.id = QUuid::createUuid().toString();
        clip.filePath = path;
        clip.name = QFileInfo(path).fileName();
        clip.startTime = 0;
        clip.duration = 5000; // 5 seconds default for images
        clip.timelineStart = m_playheadPosition;
        clip.track = 0;
        clip.speed = 1.0;
        clip.opacity = 1.0;

        m_videoClips.append(clip);
    }

    m_timeline->setVideoClips(m_videoClips);
}

void VideoEditor::play()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->play();
        m_isPlaying = true;
        m_playBtn->setVisible(false);
        m_pauseBtn->setVisible(true);
        emit playbackStateChanged(true);
    }
}

void VideoEditor::pause()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->pause();
        m_isPlaying = false;
        m_playBtn->setVisible(true);
        m_pauseBtn->setVisible(false);
        emit playbackStateChanged(false);
    }
}

void VideoEditor::stop()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_isPlaying = false;
        m_playBtn->setVisible(true);
        m_pauseBtn->setVisible(false);
        emit playbackStateChanged(false);
    }
}

void VideoEditor::seek(qint64 position)
{
    m_playheadPosition = position;

    if (m_mediaPlayer) {
        m_mediaPlayer->setPosition(position);
    }

    m_timeline->setPlayheadPosition(position);
    updateTimecode();

    emit playheadChanged(position);
}

void VideoEditor::frameStep()
{
    qint64 frameTime = 1000 / 30; // Assume 30fps
    seek(m_playheadPosition + frameTime);
}

void VideoEditor::frameBack()
{
    qint64 frameTime = 1000 / 30;
    seek(qMax(0LL, m_playheadPosition - frameTime));
}

void VideoEditor::goToStart()
{
    seek(0);
}

void VideoEditor::goToEnd()
{
    seek(m_totalDuration);
}

void VideoEditor::splitClipAtPlayhead()
{
    if (m_selectedClipId.isEmpty()) return;

    // Find clip at playhead
    for (int i = 0; i < m_videoClips.size(); ++i) {
        VideoClip &clip = m_videoClips[i];
        if (clip.id == m_selectedClipId) {
            if (m_playheadPosition > clip.timelineStart &&
                m_playheadPosition < clip.timelineStart + clip.duration) {

                // Create second part
                VideoClip newClip = clip;
                newClip.id = QUuid::createUuid().toString();
                newClip.startTime += (m_playheadPosition - clip.timelineStart);
                newClip.duration = clip.timelineStart + clip.duration - m_playheadPosition;
                newClip.timelineStart = m_playheadPosition;

                // Adjust first part
                clip.duration = m_playheadPosition - clip.timelineStart;

                m_videoClips.insert(i + 1, newClip);
                m_timeline->setVideoClips(m_videoClips);

                emit statusMessage(tr("تم قص المقطع"));
                break;
            }
        }
    }
}

void VideoEditor::deleteSelected()
{
    if (m_selectedClipId.isEmpty()) return;

    for (int i = 0; i < m_videoClips.size(); ++i) {
        if (m_videoClips[i].id == m_selectedClipId) {
            m_videoClips.removeAt(i);
            break;
        }
    }

    m_selectedClipId.clear();
    m_timeline->setSelectedClip(QString());
    m_timeline->setVideoClips(m_videoClips);

    emit statusMessage(tr("تم حذف المقطع"));
}

void VideoEditor::saveProject()
{
    if (m_projectPath.isEmpty()) {
        QString path = QFileDialog::getSaveFileName(this, tr("حفظ المشروع"), QString(),
            tr("مشروع Knoux (*.knoux);;كل الملفات (*)"));
        if (path.isEmpty()) return;
        m_projectPath = path;
    }

    // Save project data
    QSettings settings(m_projectPath, QSettings::IniFormat);

    settings.setValue("project/name", m_projectName);
    settings.setValue("project/duration", m_totalDuration);

    // Save clips
    settings.beginWriteArray("videoClips");
    for (int i = 0; i < m_videoClips.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", m_videoClips[i].id);
        settings.setValue("path", m_videoClips[i].filePath);
        settings.setValue("startTime", m_videoClips[i].startTime);
        settings.setValue("duration", m_videoClips[i].duration);
        settings.setValue("timelineStart", m_videoClips[i].timelineStart);
    }
    settings.endArray();

    m_isModified = false;
    emit statusMessage(tr("تم حفظ المشروع"));
    emit projectModified(false);
}

void VideoEditor::exportVideo()
{
    ExportDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;

    QString outputPath = dialog.outputPath();
    if (outputPath.isEmpty()) return;

    m_exportSettings.format = dialog.format();
    m_exportSettings.width = dialog.width();
    m_exportSettings.height = dialog.height();
    m_exportSettings.fps = dialog.fps();

    m_isExporting = true;
    m_exportProgressBar->setVisible(true);
    m_exportProgressBar->setValue(0);

    emit statusMessage(tr("جاري التصدير..."));

    // Simulate export progress
    QTimer *exportTimer = new QTimer(this);
    int progress = 0;
    connect(exportTimer, &QTimer::timeout, this, [this, &progress, outputPath, exportTimer]() {
        progress += 2;
        m_exportProgressBar->setValue(progress);
        emit exportProgress(progress);

        if (progress >= 100) {
            exportTimer->stop();
            exportTimer->deleteLater();

            m_isExporting = false;
            m_exportProgressBar->setVisible(false);

            emit statusMessage(tr("تم التصدير: %1").arg(outputPath));
            emit exportFinished(outputPath);
        }
    });
    exportTimer->start(100);
}

void VideoEditor::addMarker()
{
    m_markers.append(m_playheadPosition);
    m_timeline->setMarkers(m_markers);
    emit statusMessage(tr("تمت إضافة علامة"));
}

void VideoEditor::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::LoadedMedia:
        emit statusMessage(tr("تم تحميل الوسائط"));
        break;
    case QMediaPlayer::InvalidMedia:
        emit statusMessage(tr("خطأ في تحميل الوسائط"));
        break;
    default:
        break;
    }
}

void VideoEditor::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState) {
        m_isPlaying = false;
        m_playBtn->setVisible(true);
        m_pauseBtn->setVisible(false);
    }
}

void VideoEditor::onPositionChanged(qint64 position)
{
    m_playheadPosition = position;
    m_timeline->setPlayheadPosition(position);
    updateTimecode();
}

void VideoEditor::onDurationChanged(qint64 duration)
{
    m_totalDuration = duration;
    m_timeline->setDuration(duration);

    // Update clip duration if loading new media
    if (!m_videoClips.isEmpty() && m_videoClips.last().duration == 0) {
        m_videoClips.last().duration = duration;
        m_timeline->setVideoClips(m_videoClips);
    }

    m_durationLabel->setText("/ " + formatTimecode(duration));
}

void VideoEditor::onTimelineClicked(qint64 position)
{
    seek(position);
}

void VideoEditor::onClipClicked(const QString &clipId)
{
    m_selectedClipId = clipId;
    m_timeline->setSelectedClip(clipId);
    emit clipSelected(clipId);

    // Find clip info
    for (const VideoClip &clip : m_videoClips) {
        if (clip.id == clipId) {
            emit statusMessage(tr("محدد: %1").arg(clip.name));
            break;
        }
    }
}

void VideoEditor::onVolumeChanged(int volume)
{
    if (m_audioOutput) {
        m_audioOutput->setVolume(volume / 100.0);
    }
}

void VideoEditor::updateTimecode()
{
    m_timecodeLabel->setText(formatTimecode(m_playheadPosition));
}

QString VideoEditor::formatTimecode(qint64 ms)
{
    int hours = ms / 3600000;
    int minutes = (ms % 3600000) / 60000;
    int seconds = (ms % 60000) / 1000;
    int frames = (ms % 1000) / (1000 / 30);

    return QString("%1:%2:%3:%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(frames, 2, 10, QChar('0'));
}

void VideoEditor::setPlayheadPosition(qint64 position)
{
    m_playheadPosition = position;
    updateTimecode();
}

void VideoEditor::setZoomLevel(double zoom)
{
    m_zoomLevel = qBound(0.1, zoom, 5.0);
    m_timeline->setZoomLevel(m_zoomLevel);
}

void VideoEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        m_isPlaying ? pause() : play();
        break;
    case Qt::Key_Left:
        frameBack();
        break;
    case Qt::Key_Right:
        frameStep();
        break;
    case Qt::Key_Home:
        goToStart();
        break;
    case Qt::Key_End:
        goToEnd();
        break;
    case Qt::Key_C:
        splitClipAtPlayhead();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        deleteSelected();
        break;
    case Qt::Key_M:
        addMarker();
        break;
    default:
        if (event->modifiers() & Qt::ControlModifier) {
            switch (event->key()) {
            case Qt::Key_S:
                saveProject();
                return;
            case Qt::Key_E:
                exportVideo();
                return;
            case Qt::Key_I:
                importVideo();
                return;
            }
        }
        QWidget::keyPressEvent(event);
    }
}

void VideoEditor::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_videoWidget) {
        m_videoWidget->setGeometry(m_videoPreview->rect());
    }
}

// AI Operations
void VideoEditor::aiAutoCut()
{
    emit aiProcessingStarted(tr("القص التلقائي"));
    emit statusMessage(tr("جاري تحليل الفيديو للقص التلقائي..."));

    // Simulate AI processing
    QTimer::singleShot(2000, this, [this]() {
        emit aiProcessingFinished(tr("تم القص التلقائي"));
        emit statusMessage(tr("تم القص التلقائي - تم العثور على 5 نقاط قص"));
    });
}

void VideoEditor::aiStabilize()
{
    emit aiProcessingStarted(tr("تثبيت الفيديو"));
    emit statusMessage(tr("جاري تثبيت الفيديو..."));

    QTimer::singleShot(3000, this, [this]() {
        emit aiProcessingFinished(tr("تم التثبيت"));
        emit statusMessage(tr("تم تثبيت الفيديو بنجاح"));
    });
}

void VideoEditor::aiColorGrade()
{
    emit aiProcessingStarted(tr("تصحيح الألوان"));
    emit statusMessage(tr("جاري تصحيح الألوان تلقائياً..."));

    QTimer::singleShot(2500, this, [this]() {
        emit aiProcessingFinished(tr("تم تصحيح الألوان"));
        emit statusMessage(tr("تم تصحيح الألوان بنجاح"));
    });
}

void VideoEditor::aiRemoveBackground()
{
    emit aiProcessingStarted(tr("إزالة الخلفية"));
    emit statusMessage(tr("جاري إزالة الخلفية..."));

    QTimer::singleShot(4000, this, [this]() {
        emit aiProcessingFinished(tr("تمت إزالة الخلفية"));
        emit statusMessage(tr("تمت إزالة الخلفية بنجاح"));
    });
}

void VideoEditor::aiEnhance()
{
    emit aiProcessingStarted(tr("تحسين الفيديو"));
    emit statusMessage(tr("جاري تحسين جودة الفيديو..."));

    QTimer::singleShot(5000, this, [this]() {
        emit aiProcessingFinished(tr("تم التحسين"));
        emit statusMessage(tr("تم تحسين الفيديو بنجاح"));
    });
}

void VideoEditor::aiGenerateSubtitles()
{
    emit aiProcessingStarted(tr("توليد الترجمة"));
    emit statusMessage(tr("جاري توليد الترجمة التلقائية..."));

    QTimer::singleShot(6000, this, [this]() {
        emit aiProcessingFinished(tr("تم توليد الترجمة"));
        emit statusMessage(tr("تم توليد الترجمة بنجاح"));
    });
}

// ==================== VideoPreview Implementation ====================

VideoPreview::VideoPreview(QWidget *parent)
    : QWidget(parent)
    , m_videoWidget(nullptr)
    , m_showSafeAreas(false)
    , m_showGrid(false)
{
    setMinimumSize(320, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void VideoPreview::setVideoWidget(QVideoWidget *widget)
{
    m_videoWidget = widget;
    if (m_videoWidget) {
        m_videoWidget->setParent(this);
        m_videoWidget->setGeometry(rect());
    }
}

void VideoPreview::setOverlayText(const QString &text)
{
    m_overlayText = text;
    update();
}

void VideoPreview::showSafeAreas(bool show)
{
    m_showSafeAreas = show;
    update();
}

void VideoPreview::showGrid(bool show)
{
    m_showGrid = show;
    update();
}

void VideoPreview::paintEvent(QPaintEvent *event)
{
    // Background
    QPainter painter(this);
    painter.fillRect(rect(), QColor(20, 20, 30));

    // Grid
    if (m_showGrid) {
        drawGrid(painter);
    }

    // Safe areas
    if (m_showSafeAreas) {
        drawSafeAreas(painter);
    }

    // Overlay
    if (!m_overlayText.isEmpty()) {
        drawOverlay(painter);
    }
}

void VideoPreview::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(QColor(255, 255, 255, 20), 1));

    int gridSize = 40;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }

    // Rule of thirds
    painter.setPen(QPen(QColor(255, 255, 255, 30), 1, Qt::DashLine));
    painter.drawLine(width() / 3, 0, width() / 3, height());
    painter.drawLine(width() * 2 / 3, 0, width() * 2 / 3, height());
    painter.drawLine(0, height() / 3, width(), height() / 3);
    painter.drawLine(0, height() * 2 / 3, width(), height() * 2 / 3);
}

void VideoPreview::drawSafeAreas(QPainter &painter)
{
    // 90% safe area
    painter.setPen(QPen(QColor(0, 255, 0, 100), 1, Qt::DashLine));
    QRect safe90(width() * 0.05, height() * 0.05, width() * 0.9, height() * 0.9);
    painter.drawRect(safe90);

    // 80% safe area
    painter.setPen(QPen(QColor(255, 255, 0, 100), 1, Qt::DashLine));
    QRect safe80(width() * 0.1, height() * 0.1, width() * 0.8, height() * 0.8);
    painter.drawRect(safe80);
}

void VideoPreview::drawOverlay(QPainter &painter)
{
    painter.setPen(QPen(Qt::white));
    QFont font;
    font.setPointSize(14);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, m_overlayText);
}

void VideoPreview::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_videoWidget) {
        m_videoWidget->setGeometry(rect());
    }
}

// ==================== TimelineWidget Implementation ====================

TimelineWidget::TimelineWidget(QWidget *parent)
    : QWidget(parent)
    , m_playheadPosition(0)
    , m_duration(0)
    , m_zoomLevel(1.0)
    , m_isDragging(false)
    , m_isTrimming(false)
    , m_isResizing(false)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
}

void TimelineWidget::setVideoClips(const QList<VideoClip> &clips)
{
    m_videoClips = clips;
    update();
}

void TimelineWidget::setAudioClips(const QList<AudioClip> &clips)
{
    m_audioClips = clips;
    update();
}

void TimelineWidget::setMarkers(const QList<qint64> &markers)
{
    m_markers = markers;
    update();
}

void TimelineWidget::setDuration(qint64 duration)
{
    m_duration = duration;
    update();
}

void TimelineWidget::setPlayheadPosition(qint64 position)
{
    m_playheadPosition = position;
    update();
}

void TimelineWidget::setZoomLevel(double zoom)
{
    m_zoomLevel = zoom;
    update();
}

void TimelineWidget::setSelectedClip(const QString &clipId)
{
    m_selectedClipId = clipId;
    update();
}

void TimelineWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), QColor(15, 15, 25));

    // Ruler
    drawRuler(painter);

    // Tracks
    drawTracks(painter);

    // Clips
    drawClips(painter);

    // Audio clips
    drawAudioClips(painter);

    // Markers
    drawMarkers(painter);

    // Playhead
    drawPlayhead(painter);
}

void TimelineWidget::drawRuler(QPainter &painter)
{
    QRect rulerRect(HEADER_WIDTH, 0, width() - HEADER_WIDTH, RULER_HEIGHT);
    painter.fillRect(rulerRect, QColor(25, 25, 40));

    painter.setPen(QPen(QColor(100, 100, 120), 1));

    // Draw time markers
    int interval = 1000; // 1 second
    if (m_zoomLevel < 0.5) interval = 5000;
    if (m_zoomLevel > 2.0) interval = 500;

    for (qint64 t = 0; t <= m_duration; t += interval) {
        int x = xFromPosition(t);
        if (x < HEADER_WIDTH) continue;

        painter.drawLine(x, RULER_HEIGHT - 10, x, RULER_HEIGHT);

        // Time label
        int seconds = t / 1000;
        int minutes = seconds / 60;
        seconds %= 60;
        QString time = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));

        painter.setPen(QPen(QColor(150, 150, 170)));
        painter.drawText(x + 3, RULER_HEIGHT - 12, time);
        painter.setPen(QPen(QColor(100, 100, 120), 1));
    }
}

void TimelineWidget::drawTracks(QPainter &painter)
{
    int y = RULER_HEIGHT;

    // Video tracks
    for (int i = 0; i < 4; ++i) {
        QRect trackRect(0, y, width(), TRACK_HEIGHT);
        painter.fillRect(trackRect, i % 2 == 0 ? QColor(20, 20, 30) : QColor(25, 25, 35));

        // Track header
        QRect headerRect(0, y, HEADER_WIDTH, TRACK_HEIGHT);
        painter.fillRect(headerRect, QColor(35, 35, 50));

        painter.setPen(QPen(QColor(150, 150, 170)));
        painter.drawText(10, y + TRACK_HEIGHT / 2 + 5, tr("فيديو %1").arg(i + 1));

        y += TRACK_HEIGHT;
    }

    // Separator
    painter.setPen(QPen(QColor(50, 50, 70), 2));
    painter.drawLine(0, y, width(), y);

    // Audio tracks
    for (int i = 0; i < 3; ++i) {
        QRect trackRect(0, y, width(), TRACK_HEIGHT);
        painter.fillRect(trackRect, i % 2 == 0 ? QColor(25, 25, 35) : QColor(30, 30, 40));

        // Track header
        QRect headerRect(0, y, HEADER_WIDTH, TRACK_HEIGHT);
        painter.fillRect(headerRect, QColor(40, 40, 55));

        painter.setPen(QPen(QColor(150, 150, 170)));
        painter.drawText(10, y + TRACK_HEIGHT / 2 + 5, tr("صوت %1").arg(i + 1));

        y += TRACK_HEIGHT;
    }
}

void TimelineWidget::drawClips(QPainter &painter)
{
    for (const VideoClip &clip : m_videoClips) {
        int x = xFromPosition(clip.timelineStart);
        int w = int(clip.duration * m_zoomLevel / 10);
        int y = RULER_HEIGHT + clip.track * TRACK_HEIGHT;

        if (x + w < HEADER_WIDTH || x > width()) continue;

        // Clip background
        QColor clipColor = (clip.id == m_selectedClipId) ? CP_ACCENT_CYAN : QColor(60, 100, 140);
        QRect clipRect(x, y + 2, w, TRACK_HEIGHT - 4);
        painter.fillRect(clipRect, clipColor);

        // Clip border
        painter.setPen(QPen((clip.id == m_selectedClipId) ? CP_ACCENT_PINK : QColor(80, 120, 160), 2));
        painter.drawRect(clipRect);

        // Clip name
        painter.setPen(QPen(Qt::white));
        QFont font;
        font.setPointSize(9);
        painter.setFont(font);
        painter.drawText(clipRect.adjusted(5, 5, -5, -5), Qt::AlignLeft | Qt::AlignTop,
                        painter.fontMetrics().elidedText(clip.name, Qt::ElideRight, w - 10));

        // Thumbnail (simplified)
        if (!clip.thumbnail.isNull()) {
            QRect thumbRect(x + 5, y + 20, 40, 25);
            painter.drawImage(thumbRect, clip.thumbnail.scaled(40, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void TimelineWidget::drawAudioClips(QPainter &painter)
{
    // Simplified audio clip visualization
    for (const AudioClip &clip : m_audioClips) {
        int x = xFromPosition(clip.timelineStart);
        int w = int(clip.duration * m_zoomLevel / 10);
        int y = RULER_HEIGHT + 4 * TRACK_HEIGHT + clip.track * TRACK_HEIGHT;

        if (x + w < HEADER_WIDTH || x > width()) continue;

        QColor clipColor = QColor(100, 140, 80);
        QRect clipRect(x, y + 2, w, TRACK_HEIGHT - 4);
        painter.fillRect(clipRect, clipColor);

        painter.setPen(QPen(QColor(120, 160, 100), 1));
        painter.drawRect(clipRect);

        // Waveform placeholder
        painter.setPen(QPen(QColor(150, 190, 130), 1));
        int centerY = y + TRACK_HEIGHT / 2;
        for (int i = x; i < x + w; i += 4) {
            int h = qrand() % (TRACK_HEIGHT - 10);
            painter.drawLine(i, centerY - h / 2, i, centerY + h / 2);
        }
    }
}

void TimelineWidget::drawPlayhead(QPainter &painter)
{
    int x = xFromPosition(m_playheadPosition);

    // Playhead line
    painter.setPen(QPen(CP_ACCENT_PINK, 2));
    painter.drawLine(x, 0, x, height());

    // Playhead handle
    QPolygonF handle;
    handle << QPointF(x - 6, 0) << QPointF(x + 6, 0) << QPointF(x, 8);
    painter.setBrush(CP_ACCENT_PINK);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(handle);
}

void TimelineWidget::drawMarkers(QPainter &painter)
{
    painter.setPen(QPen(QColor(255, 200, 0), 2));

    for (qint64 marker : m_markers) {
        int x = xFromPosition(marker);
        if (x < HEADER_WIDTH || x > width()) continue;

        // Marker triangle
        QPolygonF markerShape;
        markerShape << QPointF(x - 5, 0) << QPointF(x + 5, 0) << QPointF(x, 8);
        painter.setBrush(QColor(255, 200, 0));
        painter.drawPolygon(markerShape);
    }
}

void TimelineWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x();

        if (x >= HEADER_WIDTH) {
            qint64 position = positionFromX(x);
            emit timelineClicked(position);
            emit playheadChanged(position);
        }
    }
}

void TimelineWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int x = event->pos().x();
        if (x >= HEADER_WIDTH) {
            qint64 position = positionFromX(x);
            emit playheadChanged(position);
        }
    }
}

void TimelineWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    m_isTrimming = false;
    m_isResizing = false;
}

void TimelineWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom
        double delta = event->angleDelta().y() > 0 ? 1.1 : 0.9;
        setZoomLevel(m_zoomLevel * delta);
    } else {
        // Scroll
        // Implementation for scrolling
    }
}

void TimelineWidget::contextMenuEvent(QContextMenuEvent *event)
{
    // Context menu for clips
}

qint64 TimelineWidget::positionFromX(int x) const
{
    return qint64((x - HEADER_WIDTH) * 10 / m_zoomLevel);
}

int TimelineWidget::xFromPosition(qint64 position) const
{
    return HEADER_WIDTH + int(position * m_zoomLevel / 10);
}

// ==================== EffectsPanel Implementation ====================

EffectsPanel::EffectsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void EffectsPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // Categories
    createEffectCategory(tr("تصحيح اللون"), {
        tr("سطوع/تباين"), tr("التشبع"), tr("درجة اللون"),
        tr("منحنيات"), tr("مستويات"), tr("توازن اللون الأبيض")
    });

    createEffectCategory(tr("تأثيرات"), {
        tr("ضبابية"), tr("حدة"), tr("ضوضاء"),
        tr("حبوب فيلم"), tr("فينيت"), tr("عدسة عين السمكة")
    });

    createEffectCategory(tr("تحويل"), {
        tr("قص"), tr("تدوير"), tr("تحجيم"),
        tr("انعكاس"), tr("تمويه"), tr("تموج")
    });

    layout->addStretch();
}

void EffectsPanel::createEffectCategory(const QString &title, const QStringList &effects)
{
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 13px; font-weight: bold; margin-top: 10px;");
    layout()->addWidget(titleLabel);

    for (const QString &effect : effects) {
        GlassButton *btn = new GlassButton(effect, this);
        btn->setFixedHeight(35);
        btn->setGlassOpacity(0.1);
        connect(btn, &GlassButton::clicked, this, [this, effect]() {
            emit effectSelected(effect);
        });
        layout()->addWidget(btn);
    }
}

// ==================== TransitionsPanel Implementation ====================

TransitionsPanel::TransitionsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void TransitionsPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel(tr("الانتقالات"));
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    layout->addWidget(titleLabel);

    QStringList transitions = {
        tr("دمج"), tr("انزلاق"), tr("دفع"),
        tr("تكبير"), tr("دائري"), tr("مؤقت"),
        tr("ستارة"), tr("تمويه"), tr("تموج")
    };

    for (const QString &trans : transitions) {
        GlassButton *btn = new GlassButton(trans, this);
        btn->setFixedHeight(45);
        btn->setGlassOpacity(0.1);
        connect(btn, &GlassButton::clicked, this, [this, trans]() {
            emit transitionSelected(trans);
        });
        layout->addWidget(btn);
    }

    layout->addStretch();
}

// ==================== ExportDialog Implementation ====================

ExportDialog::ExportDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle(tr("تصدير الفيديو"));
    setMinimumSize(450, 400);
}

void ExportDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Output path
    QGroupBox *pathGroup = new QGroupBox(tr("موقع الحفظ"), this);
    QHBoxLayout *pathLayout = new QHBoxLayout(pathGroup);

    m_pathEdit = new QLineEdit(this);
    pathLayout->addWidget(m_pathEdit);

    QPushButton *browseBtn = new QPushButton(tr("استعراض..."), this);
    connect(browseBtn, &QPushButton::clicked, this, &ExportDialog::browseOutputPath);
    pathLayout->addWidget(browseBtn);

    mainLayout->addWidget(pathGroup);

    // Format
    QGroupBox *formatGroup = new QGroupBox(tr("الصيغة"), this);
    QVBoxLayout *formatLayout = new QVBoxLayout(formatGroup);

    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItems({"MP4", "MOV", "AVI", "MKV", "WebM"});
    formatLayout->addWidget(m_formatCombo);

    mainLayout->addWidget(formatGroup);

    // Preset
    QGroupBox *presetGroup = new QGroupBox(tr("الإعداد المسبق"), this);
    QVBoxLayout *presetLayout = new QVBoxLayout(presetGroup);

    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItems({
        tr("YouTube 4K"), tr("YouTube 1080p"), tr("YouTube 720p"),
        tr("Instagram"), tr("TikTok"), tr("جودة عالية"), tr("حجم صغير")
    });
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ExportDialog::updatePresets);
    presetLayout->addWidget(m_presetCombo);

    mainLayout->addWidget(presetGroup);

    // Resolution
    QGroupBox *resGroup = new QGroupBox(tr("الدقة"), this);
    QHBoxLayout *resLayout = new QHBoxLayout(resGroup);

    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(320, 7680);
    m_widthSpin->setValue(1920);
    m_widthSpin->setSuffix(" px");
    resLayout->addWidget(new QLabel(tr("العرض:")));
    resLayout->addWidget(m_widthSpin);

    m_heightSpin = new QSpinBox(this);
    m_heightSpin->setRange(240, 4320);
    m_heightSpin->setValue(1080);
    m_heightSpin->setSuffix(" px");
    resLayout->addWidget(new QLabel(tr("الارتفاع:")));
    resLayout->addWidget(m_heightSpin);

    mainLayout->addWidget(resGroup);

    // Frame rate
    QGroupBox *fpsGroup = new QGroupBox(tr("معدل الإطارات"), this);
    QHBoxLayout *fpsLayout = new QHBoxLayout(fpsGroup);

    m_fpsSpin = new QSpinBox(this);
    m_fpsSpin->setRange(1, 240);
    m_fpsSpin->setValue(30);
    m_fpsSpin->setSuffix(" fps");
    fpsLayout->addWidget(m_fpsSpin);

    mainLayout->addWidget(fpsGroup);

    // Quality
    QGroupBox *qualityGroup = new QGroupBox(tr("الجودة"), this);
    QVBoxLayout *qualityLayout = new QVBoxLayout(qualityGroup);

    m_qualitySlider = new QSlider(Qt::Horizontal, this);
    m_qualitySlider->setRange(1, 100);
    m_qualitySlider->setValue(80);
    qualityLayout->addWidget(m_qualitySlider);

    m_qualityLabel = new QLabel(tr("جودة عالية"), this);
    m_qualityLabel->setAlignment(Qt::AlignCenter);
    qualityLayout->addWidget(m_qualityLabel);

    mainLayout->addWidget(qualityGroup);

    // Buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void ExportDialog::browseOutputPath()
{
    QString path = QFileDialog::getSaveFileName(this, tr("حفظ الفيديو"), QString(),
        tr("MP4 (*.mp4);;MOV (*.mov);;AVI (*.avi);;MKV (*.mkv)"));
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void ExportDialog::updatePresets()
{
    switch (m_presetCombo->currentIndex()) {
    case 0: // YouTube 4K
        m_widthSpin->setValue(3840);
        m_heightSpin->setValue(2160);
        m_fpsSpin->setValue(60);
        break;
    case 1: // YouTube 1080p
        m_widthSpin->setValue(1920);
        m_heightSpin->setValue(1080);
        m_fpsSpin->setValue(30);
        break;
    case 2: // YouTube 720p
        m_widthSpin->setValue(1280);
        m_heightSpin->setValue(720);
        m_fpsSpin->setValue(30);
        break;
    case 3: // Instagram
        m_widthSpin->setValue(1080);
        m_heightSpin->setValue(1080);
        m_fpsSpin->setValue(30);
        break;
    case 4: // TikTok
        m_widthSpin->setValue(1080);
        m_heightSpin->setValue(1920);
        m_fpsSpin->setValue(30);
        break;
    }
}

QString ExportDialog::outputPath() const
{
    return m_pathEdit->text();
}

QString ExportDialog::format() const
{
    return m_formatCombo->currentText().toLower();
}

int ExportDialog::quality() const
{
    return m_qualitySlider->value();
}

int ExportDialog::width() const
{
    return m_widthSpin->value();
}

int ExportDialog::height() const
{
    return m_heightSpin->value();
}

int ExportDialog::fps() const
{
    return m_fpsSpin->value();
}

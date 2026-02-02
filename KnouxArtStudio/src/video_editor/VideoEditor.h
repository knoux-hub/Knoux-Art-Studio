#ifndef VIDEOEDITOR_H
#define VIDEOEDITOR_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QTimer>
#include <QPropertyAnimation>
#include <QThread>

class GlassButton;
class GlassPanel;
class TimelineWidget;
class VideoPreview;
class EffectsPanel;
class TransitionsPanel;
class ExportDialog;
class QLabel;
class QSlider;
class QProgressBar;

// Video clip data structure
struct VideoClip {
    QString id;
    QString filePath;
    QString name;
    qint64 startTime;      // Start time in milliseconds within the source file
    qint64 duration;       // Duration in milliseconds
    qint64 timelineStart;  // Start position on timeline in milliseconds
    int track;             // Track number (0 = video, 1 = overlay, etc.)
    QImage thumbnail;
    double speed;
    double opacity;
    int volume;
    bool muted;
    QList<QString> effects;
};

// Audio clip data structure
struct AudioClip {
    QString id;
    QString filePath;
    QString name;
    qint64 startTime;
    qint64 duration;
    qint64 timelineStart;
    int track;
    int volume;
    bool muted;
    QList<QString> effects;
};

// Effect data structure
struct VideoEffect {
    QString name;
    QString displayName;
    QMap<QString, QVariant> parameters;
    qint64 startTime;
    qint64 duration;
};

// Transition data structure
struct Transition {
    QString type;
    QString name;
    qint64 duration;
    QString fromClip;
    QString toClip;
};

class VideoEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qint64 playheadPosition READ playheadPosition WRITE setPlayheadPosition)
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    explicit VideoEditor(QWidget *parent = nullptr);
    ~VideoEditor();

    qint64 playheadPosition() const { return m_playheadPosition; }
    void setPlayheadPosition(qint64 position);

    double zoomLevel() const { return m_zoomLevel; }
    void setZoomLevel(double zoom);

public slots:
    // File operations
    void openVideo(const QString &path);
    void saveProject();
    void saveProjectAs(const QString &path);
    void loadProject(const QString &path);
    void exportVideo();
    void exportVideo(const QString &path, const QString &format, int quality);

    // Media import
    void importVideo();
    void importAudio();
    void importImage();

    // Playback control
    void play();
    void pause();
    void stop();
    void seek(qint64 position);
    void frameStep();
    void frameBack();
    void goToStart();
    void goToEnd();

    // Timeline operations
    void addClipToTimeline(const VideoClip &clip);
    void removeClip(const QString &clipId);
    void moveClip(const QString &clipId, qint64 newStart, int newTrack);
    void trimClip(const QString &clipId, qint64 newStart, qint64 newDuration);
    void splitClipAtPlayhead();
    void deleteSelected();

    // Clip properties
    void setClipSpeed(const QString &clipId, double speed);
    void setClipOpacity(const QString &clipId, double opacity);
    void setClipVolume(const QString &clipId, int volume);
    void muteClip(const QString &clipId, bool muted);

    // Effects
    void addEffect(const QString &clipId, const QString &effectName);
    void removeEffect(const QString &clipId, int effectIndex);
    void setEffectParameter(const QString &clipId, int effectIndex,
                            const QString &param, const QVariant &value);

    // Transitions
    void addTransition(const QString &fromClip, const QString &toClip,
                       const QString &type, qint64 duration);
    void removeTransition(int index);

    // AI operations
    void aiAutoCut();
    void aiStabilize();
    void aiColorGrade();
    void aiRemoveBackground();
    void aiEnhance();
    void aiGenerateSubtitles();

    // Markers
    void addMarker();
    void removeMarker(int index);
    void goToNextMarker();
    void goToPreviousMarker();

signals:
    void statusMessage(const QString &message);
    void projectModified(bool modified);
    void playbackStateChanged(bool playing);
    void playheadChanged(qint64 position);
    void durationChanged(qint64 duration);
    void clipSelected(const QString &clipId);
    void exportProgress(int percent);
    void exportFinished(const QString &path);
    void aiProcessingStarted(const QString &operation);
    void aiProcessingProgress(int percent);
    void aiProcessingFinished(const QString &result);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onTimelineClicked(qint64 position);
    void onClipClicked(const QString &clipId);
    void onVolumeChanged(int volume);
    void updateTimecode();
    void updatePreview();

private:
    void setupUI();
    void createTopToolbar();
    void createPreviewArea();
    void createTimeline();
    void createSidePanels();
    void createBottomBar();
    void setupConnections();
    void setupShortcuts();

    void updateTimelineDisplay();
    void updateClipThumbnails();
    void renderFrame();
    void processVideoExport(const QString &outputPath, const QString &format);

    QString formatTimecode(qint64 ms);
    qint64 timecodeToMs(const QString &timecode);

    // UI Components
    GlassPanel *m_topToolbar;
    GlassPanel *m_previewPanel;
    GlassPanel *m_timelinePanel;
    GlassPanel *m_propertiesPanel;
    GlassPanel *m_effectsPanel;
    GlassPanel *m_bottomBar;

    VideoPreview *m_videoPreview;
    TimelineWidget *m_timeline;
    EffectsPanel *m_effectsList;
    TransitionsPanel *m_transitionsList;

    // Toolbar buttons
    GlassButton *m_playBtn;
    GlassButton *m_pauseBtn;
    GlassButton *m_stopBtn;
    GlassButton *m_prevFrameBtn;
    GlassButton *m_nextFrameBtn;
    GlassButton *m_goStartBtn;
    GlassButton *m_goEndBtn;
    GlassButton *m_splitBtn;
    GlassButton *m_deleteBtn;
    GlassButton *m_undoBtn;
    GlassButton *m_redoBtn;
    GlassButton *m_saveBtn;
    GlassButton *m_exportBtn;
    GlassButton *m_importBtn;
    GlassButton *m_markerBtn;

    // Media player
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QVideoWidget *m_videoWidget;

    // Labels and displays
    QLabel *m_timecodeLabel;
    QLabel *m_durationLabel;
    QLabel *m_statusLabel;
    QSlider *m_volumeSlider;
    QSlider *m_zoomSlider;
    QProgressBar *m_exportProgressBar;

    // Project data
    QString m_projectPath;
    QString m_projectName;
    bool m_isModified;

    QList<VideoClip> m_videoClips;
    QList<AudioClip> m_audioClips;
    QList<Transition> m_transitions;
    QList<qint64> m_markers;

    // State
    qint64 m_playheadPosition;
    qint64 m_totalDuration;
    double m_zoomLevel;
    int m_currentTrack;
    QString m_selectedClipId;
    bool m_isPlaying;
    bool m_isExporting;

    // History for undo/redo
    QStack<QByteArray> m_undoStack;
    QStack<QByteArray> m_redoStack;
    static const int MAX_HISTORY_SIZE = 50;

    // Export settings
    struct ExportSettings {
        QString format = "mp4";
        int width = 1920;
        int height = 1080;
        int fps = 30;
        int videoBitrate = 8000;
        int audioBitrate = 128;
        QString codec = "h264";
    } m_exportSettings;

    // AI processing
    bool m_isAIProcessing;
    QTimer *m_aiProgressTimer;
};

// Video Preview Widget
class VideoPreview : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPreview(QWidget *parent = nullptr);

    void setVideoWidget(QVideoWidget *widget);
    void setOverlayText(const QString &text);
    void showSafeAreas(bool show);
    void showGrid(bool show);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawSafeAreas(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawOverlay(QPainter &painter);

    QVideoWidget *m_videoWidget;
    QString m_overlayText;
    bool m_showSafeAreas;
    bool m_showGrid;
};

// Timeline Widget
class TimelineWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qint64 playheadPosition READ playheadPosition WRITE setPlayheadPosition)
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    explicit TimelineWidget(QWidget *parent = nullptr);

    qint64 playheadPosition() const { return m_playheadPosition; }
    void setPlayheadPosition(qint64 position);

    double zoomLevel() const { return m_zoomLevel; }
    void setZoomLevel(double zoom);

    void setVideoClips(const QList<VideoClip> &clips);
    void setAudioClips(const QList<AudioClip> &clips);
    void setMarkers(const QList<qint64> &markers);
    void setDuration(qint64 duration);
    void setSelectedClip(const QString &clipId);

    QString clipAt(qint64 time, int track) const;
    QRect clipRect(const QString &clipId) const;

signals:
    void playheadChanged(qint64 position);
    void clipClicked(const QString &clipId);
    void clipMoved(const QString &clipId, qint64 newStart, int newTrack);
    void clipTrimmed(const QString &clipId, qint64 newStart, qint64 newDuration);
    void timelineClicked(qint64 position);
    void contextMenuRequested(const QString &clipId, const QPoint &pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void drawRuler(QPainter &painter);
    void drawTracks(QPainter &painter);
    void drawClips(QPainter &painter);
    void drawAudioClips(QPainter &painter);
    void drawPlayhead(QPainter &painter);
    void drawMarkers(QPainter &painter);
    void drawSelection(QPainter &painter);

    qint64 positionFromX(int x) const;
    int xFromPosition(qint64 position) const;
    int trackFromY(int y) const;
    int yFromTrack(int track) const;

    QList<VideoClip> m_videoClips;
    QList<AudioClip> m_audioClips;
    QList<qint64> m_markers;
    QString m_selectedClipId;

    qint64 m_playheadPosition;
    qint64 m_duration;
    double m_zoomLevel;

    bool m_isDragging;
    bool m_isTrimming;
    bool m_isResizing;
    QString m_draggedClipId;
    QPoint m_dragStart;
    qint64 m_dragStartTime;
    int m_dragStartTrack;

    static constexpr int TRACK_HEIGHT = 50;
    static constexpr int RULER_HEIGHT = 30;
    static constexpr int HEADER_WIDTH = 100;
};

// Effects Panel
class EffectsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EffectsPanel(QWidget *parent = nullptr);

signals:
    void effectSelected(const QString &effectName);

private:
    void setupUI();
    void createEffectCategory(const QString &title, const QStringList &effects);
};

// Transitions Panel
class TransitionsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TransitionsPanel(QWidget *parent = nullptr);

signals:
    void transitionSelected(const QString &type);

private:
    void setupUI();
};

// Export Dialog
class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);

    QString outputPath() const;
    QString format() const;
    int quality() const;
    int width() const;
    int height() const;
    int fps() const;

private:
    void setupUI();
    void browseOutputPath();
    void updatePresets();

    QLineEdit *m_pathEdit;
    QComboBox *m_formatCombo;
    QComboBox *m_presetCombo;
    QSpinBox *m_widthSpin;
    QSpinBox *m_heightSpin;
    QSpinBox *m_fpsSpin;
    QSlider *m_qualitySlider;
    QLabel *m_qualityLabel;
};

#endif // VIDEOEDITOR_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GlassButton;
class GlassPanel;
class CyberpunkSplash;
class HomeScreen;
class PhotoEditor;
class VideoEditor;
class AIStudio;
class SettingsPanel;
class NotificationManager;
class BodyEditor;
class FaceRetouch;
class MakeupStudio;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(float glassOpacity READ glassOpacity WRITE setGlassOpacity)
    Q_PROPERTY(int glowIntensity READ glowIntensity WRITE setGlowIntensity)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum AppMode {
        Mode_Home,
        Mode_Photo,
        Mode_Video,
        Mode_AI,
        Mode_Body,
        Mode_Face,
        Mode_Makeup,
        Mode_Settings
    };

    float glassOpacity() const { return m_glassOpacity; }
    void setGlassOpacity(float opacity);

    int glowIntensity() const { return m_glowIntensity; }
    void setGlowIntensity(int intensity);

public slots:
    void switchMode(AppMode mode);
    void showSplashScreen();
    void hideSplashScreen();
    void showNotification(const QString& title, const QString& message, int type = 0);

    // AI Processing slots
    void onAIProcessingStarted(const QString& operation);
    void onAIProcessingProgress(int percent);
    void onAIProcessingFinished(const QString& result);

    // File operations
    void onOpenFile();
    void onSaveFile();
    void onExportFile();

signals:
    void modeChanged(AppMode newMode);
    void glassOpacityChanged(float opacity);
    void glowIntensityChanged(int intensity);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onHomeClicked();
    void onPhotoClicked();
    void onVideoClicked();
    void onAIClicked();
    void onBodyClicked();
    void onFaceClicked();
    void onMakeupClicked();
    void onSettingsClicked();
    void onMenuToggle();
    void updateAnimations();

private:
    void setupUI();
    void setupGlassmorphism();
    void setupAnimations();
    void setupConnections();
    void createSidebar();
    void createTopBar();
    void createCentralStack();
    void createFloatingPanels();
    void applyCyberpunkTheme();

    void animatePanelEntry(QWidget* panel);
    void animatePanelExit(QWidget* panel);
    void pulseGlowEffect();

    Ui::MainWindow *ui;

    // Core components
    QStackedWidget *m_centralStack;
    CyberpunkSplash *m_splashScreen;
    HomeScreen *m_homeScreen;
    PhotoEditor *m_photoEditor;
    VideoEditor *m_videoEditor;
    AIStudio *m_aiStudio;
    BodyEditor *m_bodyEditor;
    FaceRetouch *m_faceRetouch;
    MakeupStudio *m_makeupStudio;
    SettingsPanel *m_settingsPanel;
    NotificationManager *m_notificationManager;

    // Glassmorphism panels
    GlassPanel *m_sidebar;
    GlassPanel *m_topBar;
    GlassPanel *m_bottomBar;

    // Navigation buttons
    GlassButton *m_homeBtn;
    GlassButton *m_photoBtn;
    GlassButton *m_videoBtn;
    GlassButton *m_aiBtn;
    GlassButton *m_bodyBtn;
    GlassButton *m_faceBtn;
    GlassButton *m_makeupBtn;
    GlassButton *m_settingsBtn;
    GlassButton *m_menuToggleBtn;

    // Animations
    QPropertyAnimation *m_glowAnimation;
    QPropertyAnimation *m_opacityAnimation;
    QParallelAnimationGroup *m_transitionAnimation;

    // State
    AppMode m_currentMode;
    float m_glassOpacity;
    int m_glowIntensity;
    QPoint m_dragPosition;
    bool m_sidebarExpanded;

    // Cyberpunk effects
    QGraphicsDropShadowEffect *m_glowEffect;
    QTimer *m_pulseTimer;
};

#endif // MAINWINDOW_H

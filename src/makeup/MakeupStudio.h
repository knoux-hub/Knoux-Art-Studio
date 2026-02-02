#ifndef MAKEUPSTUDIO_H
#define MAKEUPSTUDIO_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QMap>
#include <QPropertyAnimation>

class GlassButton;
class GlassPanel;
class MakeupCanvas;
class LipstickPanel;
class EyeshadowPanel;
class BlushPanel;
class ContourPanel;
class MakeupAIModelsPanel;
class QLabel;
class QSlider;
class QProgressBar;

// Makeup product structure
struct MakeupProduct {
    QString id;
    QString name;
    QString category;
    QColor color;
    float opacity;
    float shine;
    float texture;
    QImage preview;
};

// Applied makeup structure
struct AppliedMakeup {
    QString productId;
    QColor color;
    int intensity;
    QRect area;
    QPolygon mask;
    float blendMode;
};

// Face regions for makeup
struct FaceRegions {
    QPolygon lips;
    QPolygon leftEye;
    QPolygon rightEye;
    QPolygon leftEyebrow;
    QPolygon rightEyebrow;
    QPolygon leftCheek;
    QPolygon rightCheek;
    QPolygon forehead;
    QPolygon nose;
    QPolygon chin;
    QPolygon jawline;
};

class MakeupStudio : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float processingProgress READ processingProgress WRITE setProcessingProgress)

public:
    explicit MakeupStudio(QWidget *parent = nullptr);
    ~MakeupStudio();

    float processingProgress() const { return m_processingProgress; }
    void setProcessingProgress(float progress);

public slots:
    void openImage(const QString &path);
    void saveImage();
    void exportImage();
    void resetAllMakeup();
    
    // Lipstick
    void setLipstickColor(const QColor &color);
    void setLipstickIntensity(int value);
    void setLipstickGloss(int value);
    void setLipstickType(const QString &type);
    
    // Eyeshadow
    void setEyeshadowColor(const QColor &color);
    void setEyeshadowIntensity(int value);
    void setEyeshadowStyle(const QString &style);
    void setEyeshadowShimmer(int value);
    
    // Eyeliner
    void setEyelinerColor(const QColor &color);
    void setEyelinerIntensity(int value);
    void setEyelinerStyle(const QString &style);
    void setEyelinerWing(int value);
    
    // Mascara
    void setMascaraIntensity(int value);
    void setMascaraLength(int value);
    void setMascaraVolume(int value);
    
    // Blush
    void setBlushColor(const QColor &color);
    void setBlushIntensity(int value);
    void setBlushStyle(const QString &style);
    
    // Contour
    void setContourIntensity(int value);
    void setHighlightIntensity(int value);
    void setBronzerIntensity(int value);
    
    // Foundation
    void setFoundationColor(const QColor &color);
    void setFoundationIntensity(int value);
    void setFoundationCoverage(int value);
    
    // AI Operations
    void aiDetectFace();
    void aiAutoMakeup();
    void aiNaturalLook();
    void aiEveningLook();
    void aiBridalLook();
    void aiCasualLook();
    void aiGlamLook();
    void aiProfessionalLook();
    void aiSuggestColors();

signals:
    void statusMessage(const QString &message);
    void imageModified(bool modified);
    void processingStarted(const QString &operation);
    void processingProgressChanged(int percent);
    void processingFinished(const QString &result);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCanvasMousePress(const QPoint &pos);
    void onCanvasMouseMove(const QPoint &pos);
    void onCanvasMouseRelease(const QPoint &pos);
    void onProductSelected(const MakeupProduct &product);
    void onIntensityChanged(const QString &category, int value);
    void onColorChanged(const QString &category, const QColor &color);
    void onAIOperationClicked(const QString &operation);
    void updatePreview();
    void applyMakeup();

private:
    void setupUI();
    void createToolbar();
    void createCanvas();
    void createSidePanels();
    void createBottomBar();
    void setupConnections();
    
    void detectFaceInternal();
    QImage applyMakeupToImage(const QImage &input);
    QImage applyLipstick(const QImage &input);
    QImage applyEyeshadow(const QImage &input);
    QImage applyEyeliner(const QImage &input);
    QImage applyMascara(const QImage &input);
    QImage applyBlush(const QImage &input);
    QImage applyContour(const QImage &input);
    QImage applyFoundation(const QImage &input);
    QImage blendColor(const QImage &input, const QPolygon &area, const QColor &color, float intensity);
    
    void loadProductPresets();
    void addHistoryState(const QString &action);
    
    // UI Components
    GlassPanel *m_topToolbar;
    GlassPanel *m_leftToolbar;
    GlassPanel *m_rightPanel;
    GlassPanel *m_bottomBar;
    MakeupCanvas *m_canvas;
    LipstickPanel *m_lipstickPanel;
    EyeshadowPanel *m_eyeshadowPanel;
    BlushPanel *m_blushPanel;
    ContourPanel *m_contourPanel;
    MakeupAIModelsPanel *m_aiPanel;
    
    // Toolbar buttons
    GlassButton *m_undoBtn;
    GlassButton *m_redoBtn;
    GlassButton *m_saveBtn;
    GlassButton *m_exportBtn;
    GlassButton *m_resetBtn;
    GlassButton *m_detectBtn;
    QProgressBar *m_progressBar;
    
    // Image data
    QImage m_originalImage;
    QImage m_currentImage;
    QImage m_previewImage;
    QString m_currentPath;
    bool m_isModified;
    
    // Face detection
    FaceRegions m_regions;
    bool m_faceDetected;
    
    // Makeup state
    struct MakeupState {
        // Lipstick
        QColor lipstickColor = QColor(200, 50, 80);
        int lipstickIntensity = 0;
        int lipstickGloss = 30;
        QString lipstickType = "matte";
        
        // Eyeshadow
        QColor eyeshadowColor = QColor(150, 100, 80);
        int eyeshadowIntensity = 0;
        int eyeshadowShimmer = 20;
        QString eyeshadowStyle = "natural";
        
        // Eyeliner
        QColor eyelinerColor = QColor(20, 20, 20);
        int eyelinerIntensity = 0;
        int eyelinerWing = 0;
        QString eyelinerStyle = "thin";
        
        // Mascara
        int mascaraIntensity = 0;
        int mascaraLength = 50;
        int mascaraVolume = 50;
        
        // Blush
        QColor blushColor = QColor(255, 150, 150);
        int blushIntensity = 0;
        QString blushStyle = "natural";
        
        // Contour
        int contourIntensity = 0;
        int highlightIntensity = 0;
        int bronzerIntensity = 0;
        
        // Foundation
        QColor foundationColor = QColor(240, 210, 190);
        int foundationIntensity = 0;
        int foundationCoverage = 50;
    } m_makeup;
    
    MakeupState m_defaultMakeup;
    
    // Products
    QVector<MakeupProduct> m_products;
    QVector<AppliedMakeup> m_applied;
    
    // History
    QStack<QPair<QImage, MakeupState>> m_undoStack;
    QStack<QPair<QImage, MakeupState>> m_redoStack;
    static const int MAX_HISTORY_SIZE = 30;
    
    // Tools
    QString m_currentTool;
    bool m_isDrawing;
    QPoint m_lastPos;
    
    // State
    float m_processingProgress;
    bool m_isProcessing;
    QTimer *m_previewTimer;
};

// Makeup Canvas Widget
class MakeupCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit MakeupCanvas(QWidget *parent = nullptr);
    
    void setImage(const QImage &image);
    void setRegions(const FaceRegions &regions);
    void setShowOverlay(bool show);
    void setShowRegions(bool show);
    void setSelectedRegion(const QString &region);
    
    QPoint imagePosFromWidget(const QPoint &widgetPos) const;
    QString regionAt(const QPoint &pos) const;

signals:
    void mousePressed(const QPoint &imagePos);
    void mouseMoved(const QPoint &imagePos);
    void mouseReleased(const QPoint &imagePos);
    void regionClicked(const QString &regionName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawImage(QPainter &painter);
    void drawRegions(QPainter &painter);
    void drawOverlay(QPainter &painter);
    
    QRect imageRect() const;
    QPointF mapToImage(const QPointF &widgetPos) const;
    
    QImage m_image;
    FaceRegions m_regions;
    QString m_selectedRegion;
    float m_zoom;
    QPoint m_offset;
    bool m_showOverlay;
    bool m_showRegions;
};

// Lipstick Panel
class LipstickPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LipstickPanel(QWidget *parent = nullptr);

signals:
    void colorChanged(const QColor &color);
    void intensityChanged(int value);
    void glossChanged(int value);
    void typeChanged(const QString &type);

private:
    void setupUI();
    void createColorPalette();
    void createTypeSelector();
    
    QColor m_currentColor;
};

// Eyeshadow Panel
class EyeshadowPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EyeshadowPanel(QWidget *parent = nullptr);

signals:
    void colorChanged(const QColor &color);
    void intensityChanged(int value);
    void shimmerChanged(int value);
    void styleChanged(const QString &style);

private:
    void setupUI();
    
    QColor m_currentColor;
};

// Blush Panel
class BlushPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BlushPanel(QWidget *parent = nullptr);

signals:
    void colorChanged(const QColor &color);
    void intensityChanged(int value);
    void styleChanged(const QString &style);

private:
    void setupUI();
    
    QColor m_currentColor;
};

// Contour Panel
class ContourPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ContourPanel(QWidget *parent = nullptr);

signals:
    void contourIntensityChanged(int value);
    void highlightIntensityChanged(int value);
    void bronzerIntensityChanged(int value);

private:
    void setupUI();
};

// Makeup AI Models Panel
class MakeupAIModelsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MakeupAIModelsPanel(QWidget *parent = nullptr);

signals:
    void detectFaceClicked();
    void autoMakeupClicked();
    void naturalLookClicked();
    void eveningLookClicked();
    void bridalLookClicked();
    void casualLookClicked();
    void glamLookClicked();
    void professionalLookClicked();
    void suggestColorsClicked();

private:
    void setupUI();
    void createAIButton(const QString &id, const QString &title, const QString &description, const QColor &accent);
};

#endif // MAKEUPSTUDIO_H

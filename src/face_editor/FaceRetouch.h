#ifndef FACERETOUCH_H
#define FACERETOUCH_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QPropertyAnimation>

class GlassButton;
class GlassPanel;
class FaceCanvas;
class FaceFeaturesPanel;
class FaceSkinPanel;
class FaceMakeupPanel;
class FaceAIModelsPanel;
class QLabel;
class QSlider;
class QProgressBar;

// Face feature detection structure
struct FaceFeature {
    QString name;
    QPointF position;
    float confidence;
    QPolygon contour;
    QRect boundingBox;
};

// Face landmarks structure
struct FaceLandmarks {
    // Jaw
    QVector<QPointF> jawPoints;
    
    // Eyes
    QVector<QPointF> leftEye;
    QVector<QPointF> rightEye;
    QPointF leftEyeCenter;
    QPointF rightEyeCenter;
    float leftEyeOpenness;
    float rightEyeOpenness;
    
    // Eyebrows
    QVector<QPointF> leftEyebrow;
    QVector<QPointF> rightEyebrow;
    
    // Nose
    QVector<QPointF> noseBridge;
    QVector<QPointF> noseBottom;
    QPointF noseTip;
    
    // Mouth
    QVector<QPointF> outerLips;
    QVector<QPointF> innerLips;
    QPointF mouthCenter;
    float mouthOpenness;
    float smileFactor;
    
    // Face contour
    QVector<QPointF> faceContour;
    QPointF faceCenter;
    float faceRotation;
    float faceScale;
};

// Face adjustment parameters
struct FaceAdjustParams {
    // Eyes
    float eyeSize = 1.0f;
    float eyeWidth = 1.0f;
    float eyeHeight = 1.0f;
    float eyeDistance = 1.0f;
    float eyeTilt = 0.0f;
    float eyeOpenness = 1.0f;
    float removeDarkCircles = 0.0f;
    float brightenEyes = 0.0f;
    
    // Eyebrows
    float eyebrowThickness = 1.0f;
    float eyebrowHeight = 1.0f;
    float eyebrowShape = 0.0f;
    float eyebrowArch = 0.0f;
    
    // Nose
    float noseWidth = 1.0f;
    float noseHeight = 1.0f;
    float noseBridge = 1.0f;
    float noseTip = 1.0f;
    float nostrilSize = 1.0f;
    
    // Mouth
    float mouthWidth = 1.0f;
    float lipThickness = 1.0f;
    float lipShape = 0.0f;
    float smile = 0.0f;
    float teethWhiteness = 0.0f;
    
    // Face shape
    float faceWidth = 1.0f;
    float faceLength = 1.0f;
    float jawWidth = 1.0f;
    float chinLength = 1.0f;
    float cheekboneWidth = 1.0f;
    float foreheadWidth = 1.0f;
    
    // Skin
    float smoothness = 0.0f;
    float fairness = 0.0f;
    float removeBlemishes = 0.0f;
    float removeWrinkles = 0.0f;
    float poreRefinement = 0.0f;
    float shineReduction = 0.0f;
};

class FaceRetouch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float processingProgress READ processingProgress WRITE setProcessingProgress)

public:
    explicit FaceRetouch(QWidget *parent = nullptr);
    ~FaceRetouch();

    float processingProgress() const { return m_processingProgress; }
    void setProcessingProgress(float progress);

public slots:
    void openImage(const QString &path);
    void saveImage();
    void exportImage();
    void resetAllAdjustments();
    
    // Eye adjustments
    void setEyeSize(float value);
    void setEyeWidth(float value);
    void setEyeHeight(float value);
    void setEyeDistance(float value);
    void setEyeTilt(float value);
    void setEyeOpenness(float value);
    void setRemoveDarkCircles(float value);
    void setBrightenEyes(float value);
    
    // Eyebrow adjustments
    void setEyebrowThickness(float value);
    void setEyebrowHeight(float value);
    void setEyebrowShape(float value);
    void setEyebrowArch(float value);
    
    // Nose adjustments
    void setNoseWidth(float value);
    void setNoseHeight(float value);
    void setNoseBridge(float value);
    void setNoseTip(float value);
    void setNostrilSize(float value);
    
    // Mouth adjustments
    void setMouthWidth(float value);
    void setLipThickness(float value);
    void setLipShape(float value);
    void setSmile(float value);
    void setTeethWhiteness(float value);
    
    // Face shape adjustments
    void setFaceWidth(float value);
    void setFaceLength(float value);
    void setJawWidth(float value);
    void setChinLength(float value);
    void setCheekboneWidth(float value);
    void setForeheadWidth(float value);
    
    // Skin adjustments
    void setSmoothness(float value);
    void setFairness(float value);
    void setRemoveBlemishes(float value);
    void setRemoveWrinkles(float value);
    void setPoreRefinement(float value);
    void setShineReduction(float value);
    
    // AI Operations
    void aiDetectFace();
    void aiAutoRetouch();
    void aiSmoothSkin();
    void aiRemoveBlemishes();
    void aiRemoveWrinkles();
    void aiBrightenEyes();
    void aiWhitenTeeth();
    void aiSculptFace();
    void aiPerfectPortrait();
    void aiNaturalBeauty();
    void aiGlamourLook();
    void aiProfessionalHeadshot();

signals:
    void statusMessage(const QString &message);
    void imageModified(bool modified);
    void processingStarted(const QString &operation);
    void processingProgressChanged(int percent);
    void processingFinished(const QString &result);
    void faceDetected(const FaceLandmarks &landmarks);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCanvasMousePress(const QPoint &pos);
    void onCanvasMouseMove(const QPoint &pos);
    void onCanvasMouseRelease(const QPoint &pos);
    void onAdjustmentChanged(const QString &name, float value);
    void onTabChanged(int index);
    void onAIOperationClicked(const QString &operation);
    void updatePreview();
    void applyAdjustments();

private:
    void setupUI();
    void createToolbar();
    void createCanvas();
    void createSidePanels();
    void createBottomBar();
    void setupConnections();
    
    void detectFaceInternal();
    QImage applyFaceTransformations(const QImage &input);
    QImage warpFeature(const QImage &input, const FaceFeature &feature, float scaleX, float scaleY);
    QImage smoothSkinInternal(const QImage &input);
    QImage removeBlemishesInternal(const QImage &input);
    QImage removeWrinklesInternal(const QImage &input);
    QImage brightenEyesInternal(const QImage &input);
    QImage whitenTeethInternal(const QImage &input);
    QImage applySkinTone(const QImage &input);
    
    void addHistoryState(const QString &action);
    
    // UI Components
    GlassPanel *m_topToolbar;
    GlassPanel *m_leftToolbar;
    GlassPanel *m_rightPanel;
    GlassPanel *m_bottomBar;
    FaceCanvas *m_canvas;
    FaceFeaturesPanel *m_featuresPanel;
    FaceSkinPanel *m_skinPanel;
    FaceMakeupPanel *m_makeupPanel;
    FaceAIModelsPanel *m_aiPanel;
    
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
    FaceLandmarks m_landmarks;
    QVector<FaceFeature> m_features;
    bool m_faceDetected;
    
    // Adjustments
    FaceAdjustParams m_params;
    FaceAdjustParams m_defaultParams;
    
    // History
    QStack<QPair<QImage, FaceAdjustParams>> m_undoStack;
    QStack<QPair<QImage, FaceAdjustParams>> m_redoStack;
    static const int MAX_HISTORY_SIZE = 30;
    
    // Tools
    QString m_currentTool;
    int m_brushSize;
    bool m_isDrawing;
    QPoint m_lastPos;
    
    // State
    float m_processingProgress;
    bool m_isProcessing;
    QTimer *m_previewTimer;
};

// Face Canvas Widget
class FaceCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit FaceCanvas(QWidget *parent = nullptr);
    
    void setImage(const QImage &image);
    void setLandmarks(const FaceLandmarks &landmarks);
    void setFeatures(const QVector<FaceFeature> &features);
    void setShowOverlay(bool show);
    void setShowGrid(bool show);
    void setShowLandmarks(bool show);
    void setSelectedFeature(const QString &feature);
    
    QPoint imagePosFromWidget(const QPoint &widgetPos) const;
    QString featureAt(const QPoint &pos) const;

signals:
    void mousePressed(const QPoint &imagePos);
    void mouseMoved(const QPoint &imagePos);
    void mouseReleased(const QPoint &imagePos);
    void featureClicked(const QString &featureName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawImage(QPainter &painter);
    void drawLandmarks(QPainter &painter);
    void drawFeatures(QPainter &painter);
    void drawOverlay(QPainter &painter);
    
    QRect imageRect() const;
    QPointF mapToImage(const QPointF &widgetPos) const;
    
    QImage m_image;
    FaceLandmarks m_landmarks;
    QVector<FaceFeature> m_features;
    QString m_selectedFeature;
    float m_zoom;
    QPoint m_offset;
    bool m_showOverlay;
    bool m_showGrid;
    bool m_showLandmarks;
};

// Face Features Panel
class FaceFeaturesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FaceFeaturesPanel(QWidget *parent = nullptr);
    
    void setValues(const FaceAdjustParams &params);
    FaceAdjustParams values() const;
    void resetAll();

signals:
    void adjustmentChanged(const QString &name, float value);
    void resetClicked();
    void autoEnhanceClicked();

private:
    void setupUI();
    void createEyeSection();
    void createEyebrowSection();
    void createNoseSection();
    void createMouthSection();
    void createFaceShapeSection();
    QSlider *createSlider(const QString &name, const QString &label, float min, float max, float defaultValue);
    
    QMap<QString, QSlider*> m_sliders;
    QMap<QString, QLabel*> m_valueLabels;
};

// Face Skin Panel
class FaceSkinPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FaceSkinPanel(QWidget *parent = nullptr);
    
    void setValues(const FaceAdjustParams &params);
    void resetAll();

signals:
    void adjustmentChanged(const QString &name, float value);
    void resetClicked();
    void smoothSkinClicked();
    void removeBlemishesClicked();
    void removeWrinklesClicked();

private:
    void setupUI();
    QSlider *createSlider(const QString &name, const QString &label, float min, float max, float defaultValue);
    
    QMap<QString, QSlider*> m_sliders;
};

// Face Makeup Panel
class FaceMakeupPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FaceMakeupPanel(QWidget *parent = nullptr);

signals:
    void lipstickColorChanged(const QColor &color);
    void lipstickIntensityChanged(int value);
    void blushColorChanged(const QColor &color);
    void blushIntensityChanged(int value);
    void eyeshadowColorChanged(const QColor &color);
    void eyeshadowIntensityChanged(int value);
    void eyelinerIntensityChanged(int value);
    void mascaraIntensityChanged(int value);
    void contourIntensityChanged(int value);
    void highlightIntensityChanged(int value);

private:
    void setupUI();
    
    QColor m_lipstickColor;
    QColor m_blushColor;
    QColor m_eyeshadowColor;
};

// Face AI Models Panel
class FaceAIModelsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FaceAIModelsPanel(QWidget *parent = nullptr);

signals:
    void detectFaceClicked();
    void autoRetouchClicked();
    void smoothSkinClicked();
    void removeBlemishesClicked();
    void removeWrinklesClicked();
    void brightenEyesClicked();
    void whitenTeethClicked();
    void sculptFaceClicked();
    void perfectPortraitClicked();
    void naturalBeautyClicked();
    void glamourLookClicked();
    void professionalHeadshotClicked();

private:
    void setupUI();
    void createAIButton(const QString &id, const QString &title, const QString &description, const QColor &accent);
};

#endif // FACERETOUCH_H

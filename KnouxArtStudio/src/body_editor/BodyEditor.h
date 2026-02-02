#ifndef BODYEDITOR_H
#define BODYEDITOR_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QPropertyAnimation>

class GlassButton;
class GlassPanel;
class BodyCanvas;
class BodyAdjustmentsPanel;
class BodyPresetsPanel;
class BodyAIModelsPanel;
class QLabel;
class QSlider;
class QProgressBar;
class QComboBox;

// Body part detection structure
struct BodyPart {
    QString name;
    QRect boundingBox;
    QPolygon contour;
    float confidence;
    QPointF center;
    float scale;
    float rotation;
};

// Body measurements structure
struct BodyMeasurements {
    float shoulderWidth;
    float waistWidth;
    float hipWidth;
    float legLength;
    float armLength;
    float torsoLength;
    float neckWidth;
    float headToBodyRatio;
};

// Body adjustment parameters
struct BodyAdjustParams {
    // Upper body
    float shoulderWidth = 1.0f;
    float chestWidth = 1.0f;
    float armWidth = 1.0f;
    float armLength = 1.0f;
    float neckWidth = 1.0f;
    float neckLength = 1.0f;
    
    // Mid body
    float waistWidth = 1.0f;
    float torsoLength = 1.0f;
    float stomachFlatness = 0.0f;
    
    // Lower body
    float hipWidth = 1.0f;
    float legLength = 1.0f;
    float thighWidth = 1.0f;
    float calfWidth = 1.0f;
    float buttockSize = 1.0f;
    
    // Overall
    float height = 1.0f;
    float bodyProportion = 1.0f;
    float slimness = 0.0f;
    float muscleDefinition = 0.0f;
};

class BodyEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float processingProgress READ processingProgress WRITE setProcessingProgress)

public:
    explicit BodyEditor(QWidget *parent = nullptr);
    ~BodyEditor();

    float processingProgress() const { return m_processingProgress; }
    void setProcessingProgress(float progress);

public slots:
    void openImage(const QString &path);
    void saveImage();
    void exportImage();
    void resetAllAdjustments();
    
    // Body part adjustments
    void setShoulderWidth(float value);
    void setChestWidth(float value);
    void setArmWidth(float value);
    void setArmLength(float value);
    void setNeckWidth(float value);
    void setNeckLength(float value);
    void setWaistWidth(float value);
    void setTorsoLength(float value);
    void setStomachFlatness(float value);
    void setHipWidth(float value);
    void setLegLength(float value);
    void setThighWidth(float value);
    void setCalfWidth(float value);
    void setButtockSize(float value);
    void setHeight(float value);
    void setBodyProportion(float value);
    void setSlimness(float value);
    void setMuscleDefinition(float value);
    
    // Presets
    void applyPreset(const QString &presetName);
    void saveCurrentAsPreset(const QString &name);
    void deletePreset(const QString &name);
    
    // AI Operations
    void aiDetectBodyParts();
    void aiAutoEnhanceBody();
    void aiSlimBody();
    void aiEnhanceMuscles();
    void aiPerfectProportions();
    void aiHourglassFigure();
    void aiAthleticBuild();
    void aiTallEffect();
    
    // Tools
    void selectTool(const QString &toolName);
    void setBrushSize(int size);
    void setBrushIntensity(int intensity);

signals:
    void statusMessage(const QString &message);
    void imageModified(bool modified);
    void processingStarted(const QString &operation);
    void processingProgressChanged(int percent);
    void processingFinished(const QString &result);
    void bodyPartsDetected(const QVector<BodyPart> &parts);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCanvasMousePress(const QPoint &pos);
    void onCanvasMouseMove(const QPoint &pos);
    void onCanvasMouseRelease(const QPoint &pos);
    void onAdjustmentChanged(const QString &name, float value);
    void onPresetSelected(const QString &preset);
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
    
    void detectBodyPartsInternal();
    QImage applyBodyTransformations(const QImage &input);
    QImage warpBodyPart(const QImage &input, const BodyPart &part, float scaleX, float scaleY);
    QImage liquifyArea(const QImage &input, const QPoint &center, float radius, float strength);
    QImage smoothBodyContours(const QImage &input);
    QImage enhanceMusclesInternal(const QImage &input);
    
    void addHistoryState(const QString &action);
    void updateMeasurements();
    
    // UI Components
    GlassPanel *m_topToolbar;
    GlassPanel *m_leftToolbar;
    GlassPanel *m_rightPanel;
    GlassPanel *m_bottomBar;
    BodyCanvas *m_canvas;
    BodyAdjustmentsPanel *m_adjustmentsPanel;
    BodyPresetsPanel *m_presetsPanel;
    BodyAIModelsPanel *m_aiPanel;
    
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
    
    // Body detection
    QVector<BodyPart> m_detectedParts;
    BodyMeasurements m_measurements;
    bool m_bodyDetected;
    
    // Adjustments
    BodyAdjustParams m_params;
    BodyAdjustParams m_defaultParams;
    
    // History
    QStack<QPair<QImage, BodyAdjustParams>> m_undoStack;
    QStack<QPair<QImage, BodyAdjustParams>> m_redoStack;
    static const int MAX_HISTORY_SIZE = 30;
    
    // Tools
    QString m_currentTool;
    int m_brushSize;
    int m_brushIntensity;
    QPoint m_lastPos;
    bool m_isDrawing;
    
    // State
    float m_processingProgress;
    bool m_isProcessing;
    QTimer *m_previewTimer;
};

// Body Canvas Widget
class BodyCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit BodyCanvas(QWidget *parent = nullptr);
    
    void setImage(const QImage &image);
    void setBodyParts(const QVector<BodyPart> &parts);
    void setSelectedPart(int index);
    void setShowOverlay(bool show);
    void setShowGrid(bool show);
    void setShowMeasurements(bool show);
    
    QPoint imagePosFromWidget(const QPoint &widgetPos) const;
    int partAt(const QPoint &pos) const;

signals:
    void mousePressed(const QPoint &imagePos);
    void mouseMoved(const QPoint &imagePos);
    void mouseReleased(const QPoint &imagePos);
    void partClicked(int partIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawImage(QPainter &painter);
    void drawBodyParts(QPainter &painter);
    void drawMeasurements(QPainter &painter);
    void drawOverlay(QPainter &painter);
    
    QRect imageRect() const;
    
    QImage m_image;
    QVector<BodyPart> m_bodyParts;
    int m_selectedPart;
    float m_zoom;
    QPoint m_offset;
    bool m_showOverlay;
    bool m_showGrid;
    bool m_showMeasurements;
    bool m_isDragging;
    QPoint m_dragStart;
};

// Body Adjustments Panel
class BodyAdjustmentsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BodyAdjustmentsPanel(QWidget *parent = nullptr);
    
    void setValues(const BodyAdjustParams &params);
    BodyAdjustParams values() const;
    void resetAll();

signals:
    void adjustmentChanged(const QString &name, float value);
    void resetClicked();
    void autoEnhanceClicked();

private:
    void setupUI();
    QSlider *createSlider(const QString &name, const QString &label, float min, float max, float defaultValue, const QString &group);
    
    QMap<QString, QSlider*> m_sliders;
    QMap<QString, QLabel*> m_valueLabels;
    QMap<QString, QGroupBox*> m_groups;
};

// Body Presets Panel
class BodyPresetsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BodyPresetsPanel(QWidget *parent = nullptr);
    
    void loadPresets();
    void savePreset(const QString &name, const BodyAdjustParams &params);
    void deletePreset(const QString &name);

signals:
    void presetSelected(const QString &name);
    void presetSaved(const QString &name);
    void presetDeleted(const QString &name);

private:
    void setupUI();
    void createPresetCard(const QString &name, const QImage &thumbnail);
    
    QMap<QString, BodyAdjustParams> m_presets;
    QWidget *m_presetsContainer;
};

// Body AI Models Panel
class BodyAIModelsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BodyAIModelsPanel(QWidget *parent = nullptr);

signals:
    void detectBodyPartsClicked();
    void autoEnhanceClicked();
    void slimBodyClicked();
    void enhanceMusclesClicked();
    void perfectProportionsClicked();
    void hourglassFigureClicked();
    void athleticBuildClicked();
    void tallEffectClicked();

private:
    void setupUI();
    void createAIButton(const QString &id, const QString &title, const QString &description, const QColor &accent);
};

#endif // BODYEDITOR_H

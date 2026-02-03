#ifndef PHOTOEDITOR_H
#define PHOTOEDITOR_H

#include <QWidget>
#include <QImage>
#include <QStack>
#include <QTimer>
#include <QPropertyAnimation>

class CanvasWidget;
class LayersPanel;
class AdjustmentsPanel;
class ToolsPanel;
class AIPanel;
class GlassButton;
class GlassPanel;
class QLabel;
class QSlider;
class QProgressBar;

struct EditState {
    QImage image;
    QString action;
    QDateTime timestamp;
};

struct Layer {
    QString name;
    QImage image;
    float opacity;
    bool visible;
    bool locked;
    QPainter::CompositionMode blendMode;
};

class PhotoEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float zoomLevel READ zoomLevel WRITE setZoomLevel)
    Q_PROPERTY(int brushSize READ brushSize WRITE setBrushSize)

public:
    explicit PhotoEditor(QWidget *parent = nullptr);
    ~PhotoEditor();

    float zoomLevel() const { return m_zoomLevel; }
    void setZoomLevel(float zoom);

    int brushSize() const { return m_brushSize; }
    void setBrushSize(int size);

public slots:
    void openImage(const QString &path);
    void saveImage();
    void exportImage();
    void exportImage(const QString &path, const QString &format);

    // Edit operations
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    // Transform operations
    void rotate(int degrees);
    void flipHorizontal();
    void flipVertical();
    void crop(const QRect &rect);
    void resize(int width, int height);

    // Adjustments
    void setBrightness(int value);
    void setContrast(int value);
    void setSaturation(int value);
    void setHue(int value);
    void setExposure(int value);
    void setHighlights(int value);
    void setShadows(int value);
    void setSharpness(int value);
    void setBlur(int value);
    void setVignette(int value);
    void setTemperature(int value);
    void setTint(int value);

    // Filters
    void applyFilter(const QString &filterName);
    void applyGrayscale();
    void applySepia();
    void applyInvert();
    void applyPosterize(int levels);

    // AI operations
    void aiAutoEnhance();
    void aiRemoveBackground();
    void aiUpscale(int scale);
    void aiPortraitEnhance();
    void aiColorMatch(const QString &referencePath);
    void aiStyleTransfer(const QString &stylePath);
    void aiGenerateMask(const QString &prompt);

    // Layer operations
    void addLayer(const QString &name = QString());
    void deleteLayer(int index);
    void duplicateLayer(int index);
    void mergeLayerDown(int index);
    void mergeAllLayers();
    void moveLayer(int fromIndex, int toIndex);
    void setLayerOpacity(int index, float opacity);
    void setLayerVisibility(int index, bool visible);
    void setLayerBlendMode(int index, QPainter::CompositionMode mode);

    // Tool selection
    void selectTool(const QString &toolName);
    void setPrimaryColor(const QColor &color);
    void setSecondaryColor(const QColor &color);

    // Zoom
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomTo100();

    // Selection
    void selectAll();
    void deselect();
    void invertSelection();

signals:
    void statusMessage(const QString &message);
    void imageModified(bool modified);
    void zoomChanged(float zoom);
    void selectionChanged(const QRect &selection);
    void layerChanged(int currentLayer);
    void historyChanged(bool canUndo, bool canRedo);
    void aiProcessingStarted(const QString &operation);
    void aiProcessingProgress(int percent);
    void aiProcessingFinished(const QString &result);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCanvasMousePress(const QPoint &pos);
    void onCanvasMouseMove(const QPoint &pos);
    void onCanvasMouseRelease(const QPoint &pos);
    void onCanvasWheel(int delta);
    void onLayerSelected(int index);
    void onAdjustmentChanged(const QString &name, int value);
    void onToolSelected(const QString &tool);
    void onAIOperationClicked(const QString &operation);
    void updateCanvas();
    void renderLayers();
    void addHistoryState(const QString &action);

private:
    void setupUI();
    void createToolbar();
    void createCanvas();
    void createSidePanels();
    void createBottomBar();
    void setupConnections();
    void setupShortcuts();

    void applyAdjustments();
    void applyTool(const QPoint &pos);
    void drawBrush(const QPoint &pos);
    void drawLine(const QPoint &from, const QPoint &to);
    void fillArea(const QPoint &pos);
    void pickColor(const QPoint &pos);

    QImage processAIAutoEnhance(const QImage &input);
    QImage processAIRemoveBackground(const QImage &input);
    QImage processAIUpscale(const QImage &input, int scale);
    QImage processAIPortraitEnhance(const QImage &input);
    QImage processAIColorMatch(const QImage &input, const QImage &reference);
    QImage processAIStyleTransfer(const QImage &input, const QImage &style);
    QImage processAIGenerateMask(const QImage &input, const QString &prompt);

    // UI Components
    GlassPanel *m_topToolbar;
    GlassPanel *m_leftToolbar;
    GlassPanel *m_rightPanel;
    GlassPanel *m_bottomBar;
    CanvasWidget *m_canvas;
    LayersPanel *m_layersPanel;
    AdjustmentsPanel *m_adjustmentsPanel;
    ToolsPanel *m_toolsPanel;
    AIPanel *m_aiPanel;

    // Toolbar buttons
    GlassButton *m_undoBtn;
    GlassButton *m_redoBtn;
    GlassButton *m_saveBtn;
    GlassButton *m_exportBtn;
    GlassButton *m_zoomInBtn;
    GlassButton *m_zoomOutBtn;
    GlassButton *m_zoomFitBtn;
    QLabel *m_zoomLabel;
    QProgressBar *m_aiProgressBar;

    // Image data
    QImage m_originalImage;
    QImage m_currentImage;
    QImage m_previewImage;
    QString m_currentPath;
    bool m_isModified;

    // Layers
    QVector<Layer> m_layers;
    int m_currentLayerIndex;

    // History
    QStack<EditState> m_undoStack;
    QStack<EditState> m_redoStack;
    static const int MAX_HISTORY_SIZE = 50;

    // Tools
    QString m_currentTool;
    QColor m_primaryColor;
    QColor m_secondaryColor;
    int m_brushSize;
    int m_brushHardness;
    QString m_brushShape;

    // View state
    float m_zoomLevel;
    QPoint m_canvasOffset;
    QRect m_selection;
    bool m_hasSelection;

    // Adjustments
    struct Adjustments {
        int brightness = 0;
        int contrast = 0;
        int saturation = 0;
        int hue = 0;
        int exposure = 0;
        int highlights = 0;
        int shadows = 0;
        int sharpness = 0;
        int blur = 0;
        int vignette = 0;
        int temperature = 0;
        int tint = 0;
    } m_adjustments;

    // Drawing state
    bool m_isDrawing;
    QPoint m_lastPos;
    QImage m_layerBeforeStroke;

    // AI state
    bool m_isAIProcessing;
    QTimer *m_aiProgressTimer;
};

// Canvas Widget for image display and interaction
class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void setZoom(float zoom);
    void setOffset(const QPoint &offset);
    void setSelection(const QRect &selection);
    void clearSelection();

    QPoint imagePosFromWidget(const QPoint &widgetPos) const;
    QRect visibleImageRect() const;

signals:
    void mousePressed(const QPoint &imagePos);
    void mouseMoved(const QPoint &imagePos);
    void mouseReleased(const QPoint &imagePos);
    void wheelScrolled(int delta);
    void selectionChanged(const QRect &selection);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawImage(QPainter &painter);
    void drawSelection(QPainter &painter);
    void drawOverlay(QPainter &painter);

    QImage m_image;
    float m_zoom;
    QPoint m_offset;
    QRect m_selection;
    bool m_hasSelection;
    bool m_isDragging;
    QPoint m_dragStart;
    QPoint m_lastOffset;
};

// Layers Panel
class LayersPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayersPanel(QWidget *parent = nullptr);

    void setLayers(const QVector<Layer> &layers);
    void setCurrentLayer(int index);
    void updateLayer(int index, const Layer &layer);

signals:
    void layerSelected(int index);
    void layerVisibilityChanged(int index, bool visible);
    void layerOpacityChanged(int index, float opacity);
    void layerBlendModeChanged(int index, QPainter::CompositionMode mode);
    void addLayerClicked();
    void deleteLayerClicked(int index);
    void duplicateLayerClicked(int index);
    void mergeLayerClicked(int index);
    void moveLayerClicked(int from, int to);

private:
    void setupUI();
    void createLayerItem(int index, const Layer &layer);

    QWidget *m_layersContainer;
    QVBoxLayout *m_layersLayout;
    int m_currentLayerIndex;
};

// Adjustments Panel
class AdjustmentsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AdjustmentsPanel(QWidget *parent = nullptr);

    void resetAll();
    void setAdjustment(const QString &name, int value);

signals:
    void adjustmentChanged(const QString &name, int value);
    void resetClicked(const QString &name);
    void autoEnhanceClicked();

private:
    void setupUI();
    QSlider *createSlider(const QString &name, int min, int max, int defaultValue);

    QMap<QString, QSlider*> m_sliders;
    QMap<QString, QLabel*> m_valueLabels;
};

// Tools Panel
class ToolsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ToolsPanel(QWidget *parent = nullptr);

    void setCurrentTool(const QString &tool);
    void setBrushSize(int size);
    void setBrushHardness(int hardness);

signals:
    void toolSelected(const QString &tool);
    void brushSizeChanged(int size);
    void brushHardnessChanged(int hardness);
    void primaryColorChanged(const QColor &color);
    void secondaryColorChanged(const QColor &color);

private:
    void setupUI();
    void createToolButton(const QString &tool, const QString &icon, const QString &tooltip);

    QString m_currentTool;
    QMap<QString, GlassButton*> m_toolButtons;
    QSlider *m_brushSizeSlider;
    QSlider *m_brushHardnessSlider;
};

// AI Panel
class AIPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AIPanel(QWidget *parent = nullptr);

    void setProcessing(bool processing);
    void setProgress(int percent);

signals:
    void autoEnhanceClicked();
    void removeBackgroundClicked();
    void upscaleClicked(int scale);
    void portraitEnhanceClicked();
    void colorMatchClicked();
    void styleTransferClicked();
    void generateMaskClicked(const QString &prompt);

private:
    void setupUI();
    void createAIButton(const QString &id, const QString &title, 
                        const QString &description, const QColor &accent);

    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QMap<QString, GlassButton*> m_aiButtons;
};

#endif // PHOTOEDITOR_H

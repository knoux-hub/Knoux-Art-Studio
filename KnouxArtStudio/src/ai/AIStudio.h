#ifndef AISTUDIO_H
#define AISTUDIO_H

#include <QWidget>
#include <QThread>
#include <QPropertyAnimation>
#include <QTimer>

class GlassButton;
class GlassPanel;
class GlassCard;
class QProgressBar;
class QLabel;
class QTextEdit;
class QLineEdit;
class QComboBox;
class QSlider;
class QSpinBox;

// AI Model information structure
struct AIModel {
    QString id;
    QString name;
    QString description;
    QString category;
    qint64 size;
    bool isDownloaded;
    bool isLoaded;
    float progress;
};

// AI Task structure
struct AITask {
    QString id;
    QString type;
    QString status;
    int progress;
    QString input;
    QString output;
    QDateTime startTime;
    QDateTime endTime;
};

class AIWorker : public QThread
{
    Q_OBJECT

public:
    explicit AIWorker(QObject *parent = nullptr);

    void setTask(const QString &type, const QImage &input, const QVariantMap &params);
    void cancel();

signals:
    void progressUpdated(int percent);
    void resultReady(const QImage &result);
    void errorOccurred(const QString &error);

protected:
    void run() override;

private:
    QString m_taskType;
    QImage m_inputImage;
    QVariantMap m_params;
    bool m_cancelled;

    QImage processTextToImage();
    QImage processImageToImage();
    QImage processUpscale();
    QImage processRemoveBackground();
    QImage processPortraitEnhance();
    QImage processColorize();
    QImage processStyleTransfer();
};

class AIStudio : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float generationProgress READ generationProgress WRITE setGenerationProgress)

public:
    explicit AIStudio(QWidget *parent = nullptr);
    ~AIStudio();

    float generationProgress() const { return m_generationProgress; }
    void setGenerationProgress(float progress);

public slots:
    // Text to Image
    void generateImage(const QString &prompt);
    void generateImage(const QString &prompt, const QVariantMap &params);

    // Image to Image
    void transformImage(const QImage &input, const QString &prompt);
    void enhanceImage(const QImage &input);
    void upscaleImage(const QImage &input, int scale);
    void removeBackground(const QImage &input);
    void colorizeImage(const QImage &input);
    void applyStyleTransfer(const QImage &input, const QString &style);

    // Batch processing
    void batchProcess(const QStringList &paths, const QString &operation);

    // Model management
    void loadModel(const QString &modelId);
    void unloadModel(const QString &modelId);
    void downloadModel(const QString &modelId);
    void deleteModel(const QString &modelId);

    // History
    void clearHistory();
    void exportHistory(const QString &path);

signals:
    void processingStarted(const QString &operation);
    void processingProgress(int percent);
    void processingFinished(const QString &result);
    void imageGenerated(const QImage &image);
    void errorOccurred(const QString &error);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onGenerateClicked();
    void onPromptChanged();
    void onModelSelected(int index);
    void onStyleSelected(int index);
    void onStrengthChanged(int value);
    void onStepsChanged(int value);
    void onGuidanceChanged(int value);
    void onNegativePromptChanged();
    void onBatchSizeChanged(int value);
    void onSeedChanged();
    void onRandomSeedClicked();
    void onImageDropped(const QImage &image);
    void onHistoryItemClicked(int index);
    void onSaveImageClicked();
    void onUseAsInputClicked();
    void onCancelGenerationClicked();
    void onWorkerProgress(int percent);
    void onWorkerFinished(const QImage &result);
    void onWorkerError(const QString &error);
    void updatePreview();

private:
    void setupUI();
    void createPromptPanel();
    void createPreviewPanel();
    void createSettingsPanel();
    void createHistoryPanel();
    void createModelsPanel();
    void setupConnections();

    void generateWithSettings();
    void updateHistoryDisplay();
    void addToHistory(const QImage &image, const QString &prompt);
    QString generateRandomSeed();
    void setProcessingState(bool processing);

    // UI Components
    GlassPanel *m_promptPanel;
    GlassPanel *m_previewPanel;
    GlassPanel *m_settingsPanel;
    GlassPanel *m_historyPanel;
    GlassPanel *m_modelsPanel;

    // Prompt area
    QTextEdit *m_promptEdit;
    QTextEdit *m_negativePromptEdit;
    GlassButton *m_generateBtn;
    GlassButton *m_cancelBtn;

    // Preview area
    QLabel *m_previewLabel;
    QLabel *m_infoLabel;
    QProgressBar *m_progressBar;
    GlassButton *m_saveBtn;
    GlassButton *m_useAsInputBtn;

    // Settings
    QComboBox *m_modelCombo;
    QComboBox *m_styleCombo;
    QSlider *m_strengthSlider;
    QSlider *m_stepsSlider;
    QSlider *m_guidanceSlider;
    QSpinBox *m_batchSizeSpin;
    QLineEdit *m_seedEdit;
    GlassButton *m_randomSeedBtn;

    // History
    QWidget *m_historyContainer;
    QVector<QImage> m_historyImages;
    QVector<QString> m_historyPrompts;

    // Models
    QVector<AIModel> m_models;

    // State
    QImage m_currentImage;
    QImage m_inputImage;
    float m_generationProgress;
    bool m_isGenerating;
    QString m_currentPrompt;

    // Worker thread
    AIWorker *m_worker;

    // Generation parameters
    struct GenParams {
        QString model;
        QString style;
        float strength = 0.75f;
        int steps = 30;
        float guidance = 7.5f;
        int batchSize = 1;
        QString seed;
    } m_params;
};

// Model Card Widget
class ModelCard : public QWidget
{
    Q_OBJECT

public:
    explicit ModelCard(const AIModel &model, QWidget *parent = nullptr);

signals:
    void loadClicked(const QString &modelId);
    void unloadClicked(const QString &modelId);
    void downloadClicked(const QString &modelId);
    void deleteClicked(const QString &modelId);

private:
    void setupUI();
    void updateState();

    AIModel m_model;
    QLabel *m_nameLabel;
    QLabel *m_descLabel;
    QLabel *m_sizeLabel;
    QProgressBar *m_downloadProgress;
    GlassButton *m_actionBtn;
};

// History Thumbnail Widget
class HistoryThumbnail : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryThumbnail(const QImage &image, const QString &prompt, int index, QWidget *parent = nullptr);

signals:
    void clicked(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QImage m_thumbnail;
    QString m_prompt;
    int m_index;
    bool m_isHovered;
};

#endif // AISTUDIO_H

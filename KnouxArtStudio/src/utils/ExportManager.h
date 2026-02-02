#ifndef EXPORTMANAGER_H
#define EXPORTMANAGER_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <QHash>

namespace Knoux {
namespace Utils {

/**
 * @brief Export format settings
 */
struct ExportFormat {
    QString name;
    QString extension;
    QString mimeType;
    int quality;  // For lossy formats (0-100)
    bool supportsAlpha;
    bool supportsAnimation;
};

/**
 * @brief Export preset for quick settings
 */
struct ExportPreset {
    QString name;
    QSize resolution;
    int quality;
    QString format;
    bool maintainAspectRatio;
    bool applyWatermark;
    QString watermarkPath;
    float watermarkOpacity;
};

/**
 * @brief Batch export task
 */
struct BatchExportTask {
    QString inputPath;
    QString outputPath;
    ExportPreset preset;
    QHash<QString, QVariant> customSettings;
};

/**
 * @brief Export Manager - Handles all export operations
 */
class ExportManager : public QObject {
    Q_OBJECT

public:
    explicit ExportManager(QObject *parent = nullptr);
    ~ExportManager();

    // Supported formats
    static QVector<ExportFormat> supportedImageFormats();
    static QVector<ExportFormat> supportedVideoFormats();
    static ExportFormat formatByExtension(const QString &ext);
    
    // Export presets
    void addPreset(const ExportPreset &preset);
    void removePreset(const QString &name);
    ExportPreset preset(const QString &name) const;
    QVector<ExportPreset> presets() const;
    
    // Default presets
    void loadDefaultPresets();
    
    // Single image export
    bool exportImage(const QImage &image, const QString &path, const ExportFormat &format);
    bool exportImage(const QImage &image, const QString &path, const ExportPreset &preset);
    
    // Batch export
    void startBatchExport(const QVector<BatchExportTask> &tasks);
    void cancelBatchExport();
    bool isBatchExporting() const;
    
    // Export settings
    void setDefaultExportPath(const QString &path);
    QString defaultExportPath() const;
    
    void setOverwriteExisting(bool overwrite);
    bool overwriteExisting() const;
    
    // Metadata
    void setMetadata(const QHash<QString, QString> &metadata);
    QHash<QString, QString> metadata() const;
    
    // Watermark
    void setWatermark(const QString &imagePath, Qt::Alignment alignment, float opacity);
    void removeWatermark();
    bool hasWatermark() const;

signals:
    void exportStarted(const QString &path);
    void exportProgress(int percent);
    void exportCompleted(const QString &path);
    void exportError(const QString &path, const QString &error);
    
    void batchExportStarted(int totalTasks);
    void batchExportProgress(int currentTask, int totalTasks, int taskPercent);
    void batchExportCompleted(int successCount, int failCount);
    void batchExportCancelled();

private:
    class Impl;
    std::unique_ptr<Impl> d;
    
    QImage applyPreset(const QImage &image, const ExportPreset &preset);
    QImage applyWatermark(const QImage &image);
};

} // namespace Utils
} // namespace Knoux

#endif // EXPORTMANAGER_H

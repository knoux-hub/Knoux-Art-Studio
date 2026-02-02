#include "ExportManager.h"
#include "ImageProcessor.h"
#include <QPainter>
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent>

namespace Knoux {
namespace Utils {

// ============================================================================
// Private Implementation
// ============================================================================

class ExportManager::Impl {
public:
    QVector<ExportPreset> exportPresets;
    QString defaultPath;
    bool overwrite = false;
    bool batchRunning = false;
    bool cancelRequested = false;
    
    QHash<QString, QString> imageMetadata;
    
    // Watermark settings
    QString watermarkPath;
    Qt::Alignment watermarkAlignment = Qt::AlignBottom | Qt::AlignRight;
    float watermarkOpacity = 0.5f;
    bool useWatermark = false;
    
    void initializePresets() {
        // Web preset
        ExportPreset web;
        web.name = tr("ويب");
        web.resolution = QSize(1920, 1080);
        web.quality = 85;
        web.format = "jpg";
        web.maintainAspectRatio = true;
        web.applyWatermark = false;
        exportPresets.append(web);
        
        // Social Media preset
        ExportPreset social;
        social.name = tr("وسائل التواصل");
        social.resolution = QSize(1080, 1080);
        social.quality = 90;
        social.format = "jpg";
        social.maintainAspectRatio = true;
        social.applyWatermark = true;
        social.watermarkOpacity = 0.3f;
        exportPresets.append(social);
        
        // Print preset
        ExportPreset print;
        print.name = tr("طباعة");
        print.resolution = QSize(3000, 2000);
        print.quality = 100;
        print.format = "tiff";
        print.maintainAspectRatio = true;
        print.applyWatermark = false;
        exportPresets.append(print);
        
        // High Quality preset
        ExportPreset hq;
        hq.name = tr("جودة عالية");
        hq.resolution = QSize(3840, 2160);
        hq.quality = 95;
        hq.format = "png";
        hq.maintainAspectRatio = true;
        hq.applyWatermark = false;
        exportPresets.append(hq);
        
        // Original preset
        ExportPreset original;
        original.name = tr("الأصلي");
        original.resolution = QSize(0, 0); // Keep original
        original.quality = 100;
        original.format = "png";
        original.maintainAspectRatio = true;
        original.applyWatermark = false;
        exportPresets.append(original);
    }
};

// ============================================================================
// Public Implementation
// ============================================================================

ExportManager::ExportManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
    d->initializePresets();
}

ExportManager::~ExportManager() = default;

QVector<ExportFormat> ExportManager::supportedImageFormats() {
    QVector<ExportFormat> formats;
    
    formats.append({tr("PNG"), "png", "image/png", 100, true, false});
    formats.append({tr("JPEG"), "jpg", "image/jpeg", 90, false, false});
    formats.append({tr("JPEG"), "jpeg", "image/jpeg", 90, false, false});
    formats.append({tr("BMP"), "bmp", "image/bmp", 100, true, false});
    formats.append({tr("TIFF"), "tiff", "image/tiff", 100, true, false});
    formats.append({tr("TIFF"), "tif", "image/tiff", 100, true, false});
    formats.append({tr("WebP"), "webp", "image/webp", 85, true, true});
    formats.append({tr("GIF"), "gif", "image/gif", 100, true, true});
    
    return formats;
}

QVector<ExportFormat> ExportManager::supportedVideoFormats() {
    QVector<ExportFormat> formats;
    
    formats.append({tr("MP4"), "mp4", "video/mp4", 90, false, true});
    formats.append({tr("AVI"), "avi", "video/avi", 90, false, true});
    formats.append({tr("MOV"), "mov", "video/quicktime", 95, false, true});
    formats.append({tr("MKV"), "mkv", "video/x-matroska", 95, false, true});
    formats.append({tr("WebM"), "webm", "video/webm", 85, false, true});
    
    return formats;
}

ExportFormat ExportManager::formatByExtension(const QString &ext) {
    QString lowerExt = ext.toLower();
    if (lowerExt.startsWith('.')) {
        lowerExt = lowerExt.mid(1);
    }
    
    for (const auto &format : supportedImageFormats()) {
        if (format.extension == lowerExt) {
            return format;
        }
    }
    
    for (const auto &format : supportedVideoFormats()) {
        if (format.extension == lowerExt) {
            return format;
        }
    }
    
    // Default to PNG
    return {tr("PNG"), "png", "image/png", 100, true, false};
}

void ExportManager::addPreset(const ExportPreset &preset) {
    // Remove existing preset with same name
    for (int i = 0; i < d->exportPresets.size(); ++i) {
        if (d->exportPresets[i].name == preset.name) {
            d->exportPresets.removeAt(i);
            break;
        }
    }
    d->exportPresets.append(preset);
}

void ExportManager::removePreset(const QString &name) {
    for (int i = 0; i < d->exportPresets.size(); ++i) {
        if (d->exportPresets[i].name == name) {
            d->exportPresets.removeAt(i);
            break;
        }
    }
}

ExportPreset ExportManager::preset(const QString &name) const {
    for (const auto &p : d->exportPresets) {
        if (p.name == name) {
            return p;
        }
    }
    return d->exportPresets.isEmpty() ? ExportPreset() : d->exportPresets.first();
}

QVector<ExportPreset> ExportManager::presets() const {
    return d->exportPresets;
}

void ExportManager::loadDefaultPresets() {
    d->exportPresets.clear();
    d->initializePresets();
}

bool ExportManager::exportImage(const QImage &image, const QString &path, const ExportFormat &format) {
    if (image.isNull()) {
        emit exportError(path, tr("صورة فارغة"));
        return false;
    }
    
    emit exportStarted(path);
    
    // Check if file exists
    if (QFile::exists(path) && !d->overwrite) {
        emit exportError(path, tr("الملف موجود بالفعل"));
        return false;
    }
    
    // Ensure directory exists
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Save image
    bool success = false;
    
    if (format.extension == "jpg" || format.extension == "jpeg") {
        success = image.save(path, "JPEG", format.quality);
    } else if (format.extension == "png") {
        success = image.save(path, "PNG");
    } else if (format.extension == "bmp") {
        success = image.save(path, "BMP");
    } else if (format.extension == "tiff" || format.extension == "tif") {
        success = image.save(path, "TIFF");
    } else if (format.extension == "webp") {
        success = image.save(path, "WebP", format.quality);
    } else if (format.extension == "gif") {
        success = image.save(path, "GIF");
    } else {
        success = image.save(path);
    }
    
    if (success) {
        emit exportCompleted(path);
    } else {
        emit exportError(path, tr("فشل في حفظ الصورة"));
    }
    
    return success;
}

bool ExportManager::exportImage(const QImage &image, const QString &path, const ExportPreset &preset) {
    QImage processed = applyPreset(image, preset);
    
    ExportFormat format = formatByExtension(preset.format);
    format.quality = preset.quality;
    
    return exportImage(processed, path, format);
}

void ExportManager::startBatchExport(const QVector<BatchExportTask> &tasks) {
    if (d->batchRunning) return;
    
    d->batchRunning = true;
    d->cancelRequested = false;
    
    emit batchExportStarted(tasks.size());
    
    int successCount = 0;
    int failCount = 0;
    
    for (int i = 0; i < tasks.size(); ++i) {
        if (d->cancelRequested) {
            emit batchExportCancelled();
            break;
        }
        
        const auto &task = tasks[i];
        
        QImage image(task.inputPath);
        if (image.isNull()) {
            emit exportError(task.inputPath, tr("فشل في تحميل الصورة"));
            failCount++;
            continue;
        }
        
        QImage processed = applyPreset(image, task.preset);
        
        ExportFormat format = formatByExtension(task.preset.format);
        format.quality = task.preset.quality;
        
        if (exportImage(processed, task.outputPath, format)) {
            successCount++;
        } else {
            failCount++;
        }
        
        int progress = (i + 1) * 100 / tasks.size();
        emit batchExportProgress(i + 1, tasks.size(), progress);
    }
    
    d->batchRunning = false;
    emit batchExportCompleted(successCount, failCount);
}

void ExportManager::cancelBatchExport() {
    d->cancelRequested = true;
}

bool ExportManager::isBatchExporting() const {
    return d->batchRunning;
}

void ExportManager::setDefaultExportPath(const QString &path) {
    d->defaultPath = path;
}

QString ExportManager::defaultExportPath() const {
    return d->defaultPath;
}

void ExportManager::setOverwriteExisting(bool overwrite) {
    d->overwrite = overwrite;
}

bool ExportManager::overwriteExisting() const {
    return d->overwrite;
}

void ExportManager::setMetadata(const QHash<QString, QString> &metadata) {
    d->imageMetadata = metadata;
}

QHash<QString, QString> ExportManager::metadata() const {
    return d->imageMetadata;
}

void ExportManager::setWatermark(const QString &imagePath, Qt::Alignment alignment, float opacity) {
    d->watermarkPath = imagePath;
    d->watermarkAlignment = alignment;
    d->watermarkOpacity = opacity;
    d->useWatermark = true;
}

void ExportManager::removeWatermark() {
    d->useWatermark = false;
    d->watermarkPath.clear();
}

bool ExportManager::hasWatermark() const {
    return d->useWatermark && !d->watermarkPath.isEmpty();
}

// ============================================================================
// Private Methods
// ============================================================================

QImage ExportManager::applyPreset(const QImage &image, const ExportPreset &preset) {
    QImage result = image.copy();
    
    // Apply resize if needed
    if (preset.resolution.width() > 0 && preset.resolution.height() > 0) {
        if (preset.maintainAspectRatio) {
            result = ImageProcessor::resize(result, preset.resolution, Qt::KeepAspectRatio);
        } else {
            result = ImageProcessor::resize(result, preset.resolution, Qt::IgnoreAspectRatio);
        }
    }
    
    // Apply watermark if enabled
    if (preset.applyWatermark && !preset.watermarkPath.isEmpty()) {
        QImage watermark(preset.watermarkPath);
        if (!watermark.isNull()) {
            QImage scaledWatermark = watermark.scaled(
                result.width() / 4,
                result.height() / 4,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
            
            QPainter painter(&result);
            painter.setOpacity(preset.watermarkOpacity);
            
            int x = result.width() - scaledWatermark.width() - 20;
            int y = result.height() - scaledWatermark.height() - 20;
            
            painter.drawImage(x, y, scaledWatermark);
            painter.end();
        }
    }
    
    return result;
}

QImage ExportManager::applyWatermark(const QImage &image) {
    if (!d->useWatermark || d->watermarkPath.isEmpty()) {
        return image;
    }
    
    QImage result = image.copy();
    QImage watermark(d->watermarkPath);
    
    if (watermark.isNull()) {
        return result;
    }
    
    // Scale watermark to be subtle
    QImage scaledWatermark = watermark.scaled(
        result.width() / 5,
        result.height() / 5,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    
    QPainter painter(&result);
    painter.setOpacity(d->watermarkOpacity);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    int x = 0, y = 0;
    int margin = 20;
    
    if (d->watermarkAlignment & Qt::AlignLeft) {
        x = margin;
    } else if (d->watermarkAlignment & Qt::AlignRight) {
        x = result.width() - scaledWatermark.width() - margin;
    } else {
        x = (result.width() - scaledWatermark.width()) / 2;
    }
    
    if (d->watermarkAlignment & Qt::AlignTop) {
        y = margin;
    } else if (d->watermarkAlignment & Qt::AlignBottom) {
        y = result.height() - scaledWatermark.height() - margin;
    } else {
        y = (result.height() - scaledWatermark.height()) / 2;
    }
    
    painter.drawImage(x, y, scaledWatermark);
    painter.end();
    
    return result;
}

} // namespace Utils
} // namespace Knoux

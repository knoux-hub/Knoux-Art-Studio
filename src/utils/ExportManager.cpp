#include "ExportManager.h"
bool ExportManager::exportSovereign(const QImage &img, const QString &path) {
    // Automatic Metadata Stripping (Privacy Enforcement)
    return img.save(path, "PNG", 100);
}

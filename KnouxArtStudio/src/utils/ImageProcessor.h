#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QColor>
#include <QVector>
#include <QPointF>
#include <functional>

namespace Knoux {
namespace Utils {

/**
 * @brief Image processing utilities
 */
class ImageProcessor {
public:
    // Basic filters
    static QImage applyBrightness(const QImage &input, int value);
    static QImage applyContrast(const QImage &input, float value);
    static QImage applySaturation(const QImage &input, float value);
    static QImage applyHueShift(const QImage &input, int degrees);
    
    // Color adjustments
    static QImage applyColorBalance(const QImage &input, int red, int green, int blue);
    static QImage applyColorTemperature(const QImage &input, int kelvin);
    static QImage applyTint(const QImage &input, int value);
    static QImage applyVibrance(const QImage &input, float value);
    
    // Tone adjustments
    static QImage applyExposure(const QImage &input, float value);
    static QImage applyHighlights(const QImage &input, float value);
    static QImage applyShadows(const QImage &input, float value);
    static QImage applyWhites(const QImage &input, float value);
    static QImage applyBlacks(const QImage &input, float value);
    
    // Detail adjustments
    static QImage applySharpness(const QImage &input, float value);
    static QImage applyClarity(const QImage &input, float value);
    static QImage applyDehaze(const QImage &input, float value);
    static QImage applyNoiseReduction(const QImage &input, float value);
    
    // Blur effects
    static QImage applyGaussianBlur(const QImage &input, float radius);
    static QImage applyMotionBlur(const QImage &input, float angle, float distance);
    static QImage applyRadialBlur(const QImage &input, QPointF center, float amount);
    static QImage applyLensBlur(const QImage &input, float radius);
    
    // Artistic filters
    static QImage applyVignette(const QImage &input, float amount, float feather);
    static QImage applyFilmGrain(const QImage &input, float amount);
    static QImage applyVintage(const QImage &input, float amount);
    static QImage applyBlackAndWhite(const QImage &input, float red, float green, float blue);
    static QImage applySepia(const QImage &input, float amount);
    
    // Transformations
    static QImage crop(const QImage &input, const QRect &rect);
    static QImage resize(const QImage &input, const QSize &size, Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
    static QImage rotate(const QImage &input, float degrees);
    static QImage flipHorizontal(const QImage &input);
    static QImage flipVertical(const QImage &input);
    
    // Advanced operations
    static QImage applyLUT(const QImage &input, const QVector<QColor> &lut);
    static QImage applyCurves(const QImage &input, const QVector<QPointF> &curve);
    static QImage applyLevels(const QImage &input, int black, int gamma, int white);
    
    // Masking
    static QImage applyMask(const QImage &input, const QImage &mask);
    static QImage createGradientMask(const QSize &size, const QPointF &start, const QPointF &end);
    static QImage createRadialMask(const QSize &size, const QPointF &center, float radius);
    
    // Blending modes
    static QImage blendNormal(const QImage &base, const QImage &blend, float opacity);
    static QImage blendMultiply(const QImage &base, const QImage &blend, float opacity);
    static QImage blendScreen(const QImage &base, const QImage &blend, float opacity);
    static QImage blendOverlay(const QImage &base, const QImage &blend, float opacity);
    static QImage blendSoftLight(const QImage &base, const QImage &blend, float opacity);
    static QImage blendHardLight(const QImage &base, const QImage &blend, float opacity);
    static QImage blendColorDodge(const QImage &base, const QImage &blend, float opacity);
    static QImage blendColorBurn(const QImage &base, const QImage &blend, float opacity);
    
    // Utility functions
    static QColor sampleColor(const QImage &image, const QPoint &pos, int radius = 1);
    static QVector<QColor> extractPalette(const QImage &image, int colorCount = 5);
    static QImage createHistogram(const QImage &image);
    
    // Parallel processing
    static QImage processParallel(const QImage &input, 
                                   std::function<QColor(const QColor&)> pixelFunc);
    
private:
    static float applyCurve(float value, const QVector<QPointF> &curve);
    static QColor blendColors(const QColor &base, const QColor &blend, float opacity);
};

} // namespace Utils
} // namespace Knoux

#endif // IMAGEPROCESSOR_H

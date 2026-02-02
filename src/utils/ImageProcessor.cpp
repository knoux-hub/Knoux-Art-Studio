#include "ImageProcessor.h"
#include <QPainter>
#include <QtMath>
#include <QtConcurrent>

namespace Knoux {
namespace Utils {

// ============================================================================
// Basic Filters
// ============================================================================

QImage ImageProcessor::applyBrightness(const QImage &input, int value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    int adjustment = value * 255 / 100;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, c.red() + adjustment, 255);
            int g = qBound(0, c.green() + adjustment, 255);
            int b = qBound(0, c.blue() + adjustment, 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyContrast(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = (value + 100.0f) / 100.0f;
    factor = factor * factor;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>((c.red() - 128) * factor + 128), 255);
            int g = qBound(0, static_cast<int>((c.green() - 128) * factor + 128), 255);
            int b = qBound(0, static_cast<int>((c.blue() - 128) * factor + 128), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applySaturation(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = (value + 100.0f) / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int gray = qGray(c.rgb());
            int r = qBound(0, static_cast<int>(gray + (c.red() - gray) * factor), 255);
            int g = qBound(0, static_cast<int>(gray + (c.green() - gray) * factor), 255);
            int b = qBound(0, static_cast<int>(gray + (c.blue() - gray) * factor), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyHueShift(const QImage &input, int degrees) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int h, s, v;
            c.getHsv(&h, &s, &v);
            h = (h + degrees) % 360;
            if (h < 0) h += 360;
            c.setHsv(h, s, v);
            result.setPixelColor(x, y, c);
        }
    }
    
    return result;
}

// ============================================================================
// Color Adjustments
// ============================================================================

QImage ImageProcessor::applyColorBalance(const QImage &input, int red, int green, int blue) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, c.red() + red, 255);
            int g = qBound(0, c.green() + green, 255);
            int b = qBound(0, c.blue() + blue, 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyColorTemperature(const QImage &input, int kelvin) {
    if (input.isNull()) return QImage();
    
    // Simple temperature adjustment
    QImage result = input.copy();
    float warmth = (kelvin - 6500) / 100.0f;
    int redAdjust = static_cast<int>(warmth * 2);
    int blueAdjust = static_cast<int>(-warmth * 2);
    
    return applyColorBalance(result, redAdjust, 0, blueAdjust);
}

QImage ImageProcessor::applyTint(const QImage &input, int value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    int greenAdjust = value / 2;
    int magentaAdjust = -value / 2;
    
    return applyColorBalance(result, magentaAdjust, greenAdjust, magentaAdjust);
}

QImage ImageProcessor::applyVibrance(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = (value + 100.0f) / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int gray = qGray(c.rgb());
            float saturation = (c.saturationF() + 1.0f) / 2.0f;
            float adjustment = factor * (1.0f - saturation);
            
            int r = qBound(0, static_cast<int>(gray + (c.red() - gray) * adjustment), 255);
            int g = qBound(0, static_cast<int>(gray + (c.green() - gray) * adjustment), 255);
            int b = qBound(0, static_cast<int>(gray + (c.blue() - gray) * adjustment), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

// ============================================================================
// Tone Adjustments
// ============================================================================

QImage ImageProcessor::applyExposure(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = std::pow(2.0f, value / 100.0f);
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>(c.red() * factor), 255);
            int g = qBound(0, static_cast<int>(c.green() * factor), 255);
            int b = qBound(0, static_cast<int>(c.blue() * factor), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyHighlights(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = (value + 100.0f) / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int brightness = qGray(c.rgb());
            
            if (brightness > 128) {
                float adjustment = (brightness - 128) / 128.0f * factor;
                int r = qBound(0, static_cast<int>(c.red() * (1 + adjustment)), 255);
                int g = qBound(0, static_cast<int>(c.green() * (1 + adjustment)), 255);
                int b = qBound(0, static_cast<int>(c.blue() * (1 + adjustment)), 255);
                result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
            }
        }
    }
    
    return result;
}

QImage ImageProcessor::applyShadows(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = (value + 100.0f) / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int brightness = qGray(c.rgb());
            
            if (brightness < 128) {
                float adjustment = (128 - brightness) / 128.0f * factor;
                int r = qBound(0, static_cast<int>(c.red() * (1 + adjustment)), 255);
                int g = qBound(0, static_cast<int>(c.green() * (1 + adjustment)), 255);
                int b = qBound(0, static_cast<int>(c.blue() * (1 + adjustment)), 255);
                result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
            }
        }
    }
    
    return result;
}

QImage ImageProcessor::applyWhites(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float whitePoint = 255.0f * (100.0f - value) / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>(c.red() * 255.0f / whitePoint), 255);
            int g = qBound(0, static_cast<int>(c.green() * 255.0f / whitePoint), 255);
            int b = qBound(0, static_cast<int>(c.blue() * 255.0f / whitePoint), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyBlacks(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float blackPoint = 255.0f * value / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, static_cast<int>((c.red() - blackPoint) * 255.0f / (255.0f - blackPoint)), 255);
            int g = qBound(0, static_cast<int>((c.green() - blackPoint) * 255.0f / (255.0f - blackPoint)), 255);
            int b = qBound(0, static_cast<int>((c.blue() - blackPoint) * 255.0f / (255.0f - blackPoint)), 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

// ============================================================================
// Detail Adjustments
// ============================================================================

QImage ImageProcessor::applySharpness(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    // Simple unsharp mask
    QImage blurred = applyGaussianBlur(input, 2.0f);
    QImage result = input.copy();
    
    float amount = value / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor original = input.pixelColor(x, y);
            QColor blur = blurred.pixelColor(x, y);
            
            int r = qBound(0, static_cast<int>(original.red() + (original.red() - blur.red()) * amount), 255);
            int g = qBound(0, static_cast<int>(original.green() + (original.green() - blur.green()) * amount), 255);
            int b = qBound(0, static_cast<int>(original.blue() + (original.blue() - blur.blue()) * amount), 255);
            
            result.setPixelColor(x, y, QColor(r, g, b, original.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyClarity(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    // Clarity is like local contrast
    QImage result = input.copy();
    float amount = value / 100.0f;
    int radius = 10;
    
    for (int y = radius; y < result.height() - radius; ++y) {
        for (int x = radius; x < result.width() - radius; ++x) {
            QColor center = input.pixelColor(x, y);
            
            // Calculate local average
            float sumR = 0, sumG = 0, sumB = 0;
            int count = 0;
            
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    QColor c = input.pixelColor(x + dx, y + dy);
                    sumR += c.red();
                    sumG += c.green();
                    sumB += c.blue();
                    count++;
                }
            }
            
            float avgR = sumR / count;
            float avgG = sumG / count;
            float avgB = sumB / count;
            
            int r = qBound(0, static_cast<int>(center.red() + (center.red() - avgR) * amount), 255);
            int g = qBound(0, static_cast<int>(center.green() + (center.green() - avgG) * amount), 255);
            int b = qBound(0, static_cast<int>(center.blue() + (center.blue() - avgB) * amount), 255);
            
            result.setPixelColor(x, y, QColor(r, g, b, center.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyDehaze(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float amount = value / 100.0f;
    
    // Simple dehaze: increase contrast and reduce blue
    result = applyContrast(result, value * 0.5f);
    result = applyColorBalance(result, 0, 0, static_cast<int>(-value * 0.3f));
    
    return result;
}

QImage ImageProcessor::applyNoiseReduction(const QImage &input, float value) {
    if (input.isNull()) return QImage();
    
    // Simple median filter
    QImage result = input.copy();
    int radius = static_cast<int>(value / 50.0f) + 1;
    
    for (int y = radius; y < result.height() - radius; ++y) {
        for (int x = radius; x < result.width() - radius; ++x) {
            QVector<int> reds, greens, blues;
            
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    QColor c = input.pixelColor(x + dx, y + dy);
                    reds.append(c.red());
                    greens.append(c.green());
                    blues.append(c.blue());
                }
            }
            
            std::sort(reds.begin(), reds.end());
            std::sort(greens.begin(), greens.end());
            std::sort(blues.begin(), blues.end());
            
            int medianIdx = reds.size() / 2;
            result.setPixelColor(x, y, QColor(reds[medianIdx], greens[medianIdx], blues[medianIdx]));
        }
    }
    
    return result;
}

// ============================================================================
// Blur Effects
// ============================================================================

QImage ImageProcessor::applyGaussianBlur(const QImage &input, float radius) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    if (radius < 0.5f) return result;
    
    int size = static_cast<int>(radius * 3) * 2 + 1;
    QVector<float> kernel(size);
    float sigma = radius;
    float sum = 0.0f;
    
    // Create Gaussian kernel
    for (int i = 0; i < size; ++i) {
        int x = i - size / 2;
        kernel[i] = std::exp(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }
    
    // Normalize
    for (float &k : kernel) {
        k /= sum;
    }
    
    // Horizontal pass
    QImage temp = input.copy();
    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            float r = 0, g = 0, b = 0, a = 0;
            
            for (int i = 0; i < size; ++i) {
                int px = qBound(0, x + i - size / 2, input.width() - 1);
                QColor c = input.pixelColor(px, y);
                r += c.redF() * kernel[i];
                g += c.greenF() * kernel[i];
                b += c.blueF() * kernel[i];
                a += c.alphaF() * kernel[i];
            }
            
            temp.setPixelColor(x, y, QColor::fromRgbF(r, g, b, a));
        }
    }
    
    // Vertical pass
    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            float r = 0, g = 0, b = 0, a = 0;
            
            for (int i = 0; i < size; ++i) {
                int py = qBound(0, y + i - size / 2, input.height() - 1);
                QColor c = temp.pixelColor(x, py);
                r += c.redF() * kernel[i];
                g += c.greenF() * kernel[i];
                b += c.blueF() * kernel[i];
                a += c.alphaF() * kernel[i];
            }
            
            result.setPixelColor(x, y, QColor::fromRgbF(r, g, b, a));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyMotionBlur(const QImage &input, float angle, float distance) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float rad = qDegreesToRadians(angle);
    int steps = static_cast<int>(distance);
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            float r = 0, g = 0, b = 0;
            int count = 0;
            
            for (int i = -steps / 2; i <= steps / 2; ++i) {
                int px = qBound(0, static_cast<int>(x + i * std::cos(rad)), input.width() - 1);
                int py = qBound(0, static_cast<int>(y + i * std::sin(rad)), input.height() - 1);
                
                QColor c = input.pixelColor(px, py);
                r += c.redF();
                g += c.greenF();
                b += c.blueF();
                count++;
            }
            
            result.setPixelColor(x, y, QColor::fromRgbF(r / count, g / count, b / count));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyRadialBlur(const QImage &input, QPointF center, float amount) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    int samples = static_cast<int>(amount / 5) + 3;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            float r = 0, g = 0, b = 0;
            
            float dx = x - center.x();
            float dy = y - center.y();
            float dist = std::sqrt(dx * dx + dy * dy);
            float angle = std::atan2(dy, dx);
            
            for (int i = 0; i < samples; ++i) {
                float t = i / static_cast<float>(samples);
                float sampleDist = dist * (1 - t * amount / 100);
                
                int px = qBound(0, static_cast<int>(center.x() + sampleDist * std::cos(angle)), input.width() - 1);
                int py = qBound(0, static_cast<int>(center.y() + sampleDist * std::sin(angle)), input.height() - 1);
                
                QColor c = input.pixelColor(px, py);
                r += c.redF();
                g += c.greenF();
                b += c.blueF();
            }
            
            result.setPixelColor(x, y, QColor::fromRgbF(r / samples, g / samples, b / samples));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyLensBlur(const QImage &input, float radius) {
    // Lens blur is similar to Gaussian but with bokeh effect
    // For simplicity, use Gaussian with slight modification
    return applyGaussianBlur(input, radius * 1.5f);
}

// ============================================================================
// Artistic Filters
// ============================================================================

QImage ImageProcessor::applyVignette(const QImage &input, float amount, float feather) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    QPointF center(input.width() / 2.0f, input.height() / 2.0f);
    float maxDist = std::sqrt(center.x() * center.x() + center.y() * center.y());
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            float dx = x - center.x();
            float dy = y - center.y();
            float dist = std::sqrt(dx * dx + dy * dy);
            
            float vignette = 1.0f;
            if (dist > maxDist * (1 - feather / 100)) {
                float t = (dist - maxDist * (1 - feather / 100)) / (maxDist * feather / 100);
                vignette = 1.0f - t * amount / 100;
                vignette = qMax(0.0f, vignette);
            }
            
            QColor c = result.pixelColor(x, y);
            int r = static_cast<int>(c.red() * vignette);
            int g = static_cast<int>(c.green() * vignette);
            int b = static_cast<int>(c.blue() * vignette);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyFilmGrain(const QImage &input, float amount) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    qsrand(QTime::currentTime().msec());
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            int noise = (qrand() % 256 - 128) * amount / 100;
            
            QColor c = result.pixelColor(x, y);
            int r = qBound(0, c.red() + noise, 255);
            int g = qBound(0, c.green() + noise, 255);
            int b = qBound(0, c.blue() + noise, 255);
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applyVintage(const QImage &input, float amount) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    
    // Reduce saturation
    result = applySaturation(result, -amount * 0.3f);
    
    // Warm color cast
    result = applyColorBalance(result, static_cast<int>(amount * 0.3f), 0, static_cast<int>(-amount * 0.2f));
    
    // Add vignette
    result = applyVignette(result, amount * 0.5f, 50);
    
    return result;
}

QImage ImageProcessor::applyBlackAndWhite(const QImage &input, float red, float green, float blue) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float total = red + green + blue;
    float rWeight = red / total;
    float gWeight = green / total;
    float bWeight = blue / total;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int gray = static_cast<int>(c.red() * rWeight + c.green() * gWeight + c.blue() * bWeight);
            result.setPixelColor(x, y, QColor(gray, gray, gray, c.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::applySepia(const QImage &input, float amount) {
    if (input.isNull()) return QImage();
    
    QImage result = input.copy();
    float factor = amount / 100.0f;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor c = result.pixelColor(x, y);
            int gray = qGray(c.rgb());
            
            int r = qBound(0, static_cast<int>(gray + 40 * factor), 255);
            int g = qBound(0, static_cast<int>(gray + 20 * factor), 255);
            int b = qBound(0, static_cast<int>(gray - 20 * factor), 255);
            
            result.setPixelColor(x, y, QColor(r, g, b, c.alpha()));
        }
    }
    
    return result;
}

// ============================================================================
// Transformations
// ============================================================================

QImage ImageProcessor::crop(const QImage &input, const QRect &rect) {
    if (input.isNull()) return QImage();
    return input.copy(rect.intersected(input.rect()));
}

QImage ImageProcessor::resize(const QImage &input, const QSize &size, Qt::AspectRatioMode mode) {
    if (input.isNull()) return QImage();
    return input.scaled(size, mode, Qt::SmoothTransformation);
}

QImage ImageProcessor::rotate(const QImage &input, float degrees) {
    if (input.isNull()) return QImage();
    
    QTransform transform;
    transform.rotate(degrees);
    return input.transformed(transform, Qt::SmoothTransformation);
}

QImage ImageProcessor::flipHorizontal(const QImage &input) {
    if (input.isNull()) return QImage();
    return input.mirrored(true, false);
}

QImage ImageProcessor::flipVertical(const QImage &input) {
    if (input.isNull()) return QImage();
    return input.mirrored(false, true);
}

// ============================================================================
// Blending Modes
// ============================================================================

QImage ImageProcessor::blendNormal(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            int r = static_cast<int>(b.red() * (1 - opacity) + l.red() * opacity);
            int g = static_cast<int>(b.green() * (1 - opacity) + l.green() * opacity);
            int b_ = static_cast<int>(b.blue() * (1 - opacity) + l.blue() * opacity);
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendMultiply(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            int r = static_cast<int>(b.red() * (l.red() / 255.0f) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(b.green() * (l.green() / 255.0f) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(b.blue() * (l.blue() / 255.0f) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendScreen(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            int r = static_cast<int>((255 - (255 - b.red()) * (255 - l.red()) / 255.0f) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>((255 - (255 - b.green()) * (255 - l.green()) / 255.0f) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>((255 - (255 - b.blue()) * (255 - l.blue()) / 255.0f) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendOverlay(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            auto overlay = [](int base, int blend) {
                if (base < 128) {
                    return 2 * base * blend / 255;
                } else {
                    return 255 - 2 * (255 - base) * (255 - blend) / 255;
                }
            };
            
            int r = static_cast<int>(overlay(b.red(), l.red()) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(overlay(b.green(), l.green()) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(overlay(b.blue(), l.blue()) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendSoftLight(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            auto softLight = [](int base, int blend) {
                float b = base / 255.0f;
                float l = blend / 255.0f;
                if (l < 0.5f) {
                    return static_cast<int>((2 * b * l + b * b * (1 - 2 * l)) * 255);
                } else {
                    return static_cast<int>((2 * b * (1 - l) + std::sqrt(b) * (2 * l - 1)) * 255);
                }
            };
            
            int r = static_cast<int>(softLight(b.red(), l.red()) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(softLight(b.green(), l.green()) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(softLight(b.blue(), l.blue()) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendHardLight(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            auto hardLight = [](int base, int blend) {
                if (blend < 128) {
                    return 2 * base * blend / 255;
                } else {
                    return 255 - 2 * (255 - base) * (255 - blend) / 255;
                }
            };
            
            int r = static_cast<int>(hardLight(b.red(), l.red()) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(hardLight(b.green(), l.green()) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(hardLight(b.blue(), l.blue()) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendColorDodge(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            auto colorDodge = [](int base, int blend) {
                if (blend == 255) return 255;
                return qMin(255, base * 255 / (255 - blend));
            };
            
            int r = static_cast<int>(colorDodge(b.red(), l.red()) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(colorDodge(b.green(), l.green()) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(colorDodge(b.blue(), l.blue()) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

QImage ImageProcessor::blendColorBurn(const QImage &base, const QImage &blend, float opacity) {
    if (base.isNull() || blend.isNull()) return base;
    
    QImage result = base.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QColor b = base.pixelColor(x, y);
            QColor l = blend.pixelColor(qMin(x, blend.width() - 1), qMin(y, blend.height() - 1));
            
            auto colorBurn = [](int base, int blend) {
                if (blend == 0) return 0;
                return qMax(0, 255 - (255 - base) * 255 / blend);
            };
            
            int r = static_cast<int>(colorBurn(b.red(), l.red()) * opacity + b.red() * (1 - opacity));
            int g = static_cast<int>(colorBurn(b.green(), l.green()) * opacity + b.green() * (1 - opacity));
            int b_ = static_cast<int>(colorBurn(b.blue(), l.blue()) * opacity + b.blue() * (1 - opacity));
            
            result.setPixelColor(x, y, QColor(r, g, b_, b.alpha()));
        }
    }
    
    return result;
}

// ============================================================================
// Utility Functions
// ============================================================================

QColor ImageProcessor::sampleColor(const QImage &image, const QPoint &pos, int radius) {
    if (image.isNull()) return QColor();
    
    float r = 0, g = 0, b = 0;
    int count = 0;
    
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int px = qBound(0, pos.x() + dx, image.width() - 1);
            int py = qBound(0, pos.y() + dy, image.height() - 1);
            
            QColor c = image.pixelColor(px, py);
            r += c.redF();
            g += c.greenF();
            b += c.blueF();
            count++;
        }
    }
    
    return QColor::fromRgbF(r / count, g / count, b / count);
}

QVector<QColor> ImageProcessor::extractPalette(const QImage &image, int colorCount) {
    if (image.isNull()) return {};
    
    // Simple color quantization
    QHash<int, int> colorHistogram;
    
    for (int y = 0; y < image.height(); y += 4) {
        for (int x = 0; x < image.width(); x += 4) {
            QColor c = image.pixelColor(x, y);
            // Quantize to reduce colors
            int r = (c.red() / 32) * 32;
            int g = (c.green() / 32) * 32;
            int b = (c.blue() / 32) * 32;
            int key = (r << 16) | (g << 8) | b;
            colorHistogram[key]++;
        }
    }
    
    // Get most frequent colors
    QVector<QPair<int, int>> sortedColors;
    for (auto it = colorHistogram.begin(); it != colorHistogram.end(); ++it) {
        sortedColors.append({it.value(), it.key()});
    }
    
    std::sort(sortedColors.begin(), sortedColors.end(), std::greater<QPair<int, int>>());
    
    QVector<QColor> palette;
    for (int i = 0; i < qMin(colorCount, sortedColors.size()); ++i) {
        int key = sortedColors[i].second;
        int r = (key >> 16) & 0xFF;
        int g = (key >> 8) & 0xFF;
        int b = key & 0xFF;
        palette.append(QColor(r, g, b));
    }
    
    return palette;
}

QImage ImageProcessor::createHistogram(const QImage &image) {
    if (image.isNull()) return QImage();
    
    const int width = 256;
    const int height = 100;
    QImage histogram(width, height, QImage::Format_ARGB32);
    histogram.fill(Qt::transparent);
    
    QVector<int> redHist(256, 0);
    QVector<int> greenHist(256, 0);
    QVector<int> blueHist(256, 0);
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor c = image.pixelColor(x, y);
            redHist[c.red()]++;
            greenHist[c.green()]++;
            blueHist[c.blue()]++;
        }
    }
    
    // Find max for normalization
    int maxVal = 0;
    for (int i = 0; i < 256; ++i) {
        maxVal = qMax(maxVal, redHist[i]);
        maxVal = qMax(maxVal, greenHist[i]);
        maxVal = qMax(maxVal, blueHist[i]);
    }
    
    QPainter painter(&histogram);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw histogram
    for (int i = 0; i < 256; ++i) {
        int rHeight = redHist[i] * height / maxVal;
        int gHeight = greenHist[i] * height / maxVal;
        int bHeight = blueHist[i] * height / maxVal;
        
        painter.setPen(QColor(255, 0, 0, 128));
        painter.drawLine(i, height, i, height - rHeight);
        painter.setPen(QColor(0, 255, 0, 128));
        painter.drawLine(i, height, i, height - gHeight);
        painter.setPen(QColor(0, 0, 255, 128));
        painter.drawLine(i, height, i, height - bHeight);
    }
    
    return histogram;
}

} // namespace Utils
} // namespace Knoux

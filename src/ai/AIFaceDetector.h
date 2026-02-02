#ifndef AIFACEDETECTOR_H
#define AIFACEDETECTOR_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QPointF>

namespace Knoux {
namespace AI {

/**
 * @brief Structure representing detected face with landmarks
 */
struct DetectedFace {
    QRect boundingBox;
    float confidence;
    QVector<QPointF> landmarks68;
    QPointF leftEyeCenter;
    QPointF rightEyeCenter;
    QPointF noseTip;
    QPointF mouthCenter;
    QPointF jawCenter;
    
    // Facial feature regions
    QVector<QPointF> leftEyeContour;
    QVector<QPointF> rightEyeContour;
    QVector<QPointF> noseContour;
    QVector<QPointF> mouthContour;
    QVector<QPointF> jawline;
    QVector<QPointF> leftEyebrow;
    QVector<QPointF> rightEyebrow;
};

/**
 * @brief AI Face Detection Model - Offline
 * 
 * Uses FaceNet-v2 architecture optimized for local inference
 * Detects faces and 68 facial landmarks without internet connection
 */
class AIFaceDetector : public QObject {
    Q_OBJECT

public:
    explicit AIFaceDetector(QObject *parent = nullptr);
    ~AIFaceDetector();

    // Model management
    bool loadModel(const QString &modelPath);
    bool isModelLoaded() const;
    void unloadModel();
    
    // Detection methods
    QVector<DetectedFace> detectFaces(const QImage &image);
    DetectedFace detectPrimaryFace(const QImage &image);
    
    // Landmark detection
    QVector<QPointF> detectLandmarks(const QImage &image, const QRect &faceRect);
    
    // Feature extraction
    QVector<float> extractFaceFeatures(const QImage &image, const DetectedFace &face);
    
    // Detection parameters
    void setConfidenceThreshold(float threshold);
    float confidenceThreshold() const;
    
    void setDetectionScale(float scale);
    float detectionScale() const;
    
    // Performance settings
    void enableGPUAcceleration(bool enable);
    bool isGPUAccelerationEnabled() const;
    
    void setThreadCount(int count);
    int threadCount() const;

signals:
    void detectionCompleted(const QVector<DetectedFace> &faces);
    void detectionProgress(int percent);
    void modelLoaded();
    void modelUnloaded();
    void error(const QString &message);

private:
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace AI
} // namespace Knoux

#endif // AIFACEDETECTOR_H

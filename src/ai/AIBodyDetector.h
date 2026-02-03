#ifndef AIBODYDETECTOR_H
#define AIBODYDETECTOR_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QPointF>
#include <QPolygon>

namespace Knoux {
namespace AI {

/**
 * @brief Body part detection result
 */
struct BodyPart {
    QString name;           // e.g., "head", "torso", "left_arm", etc.
    QRect boundingBox;
    QPolygon contour;
    float confidence;
    QPointF center;
    
    // Part-specific measurements
    float width;
    float height;
    float aspectRatio;
    
    // For limbs
    QPointF jointStart;
    QPointF jointEnd;
};

/**
 * @brief Full body pose detection result
 */
struct BodyPose {
    QVector<BodyPart> parts;
    
    // Key joints (for pose estimation)
    QPointF nose;
    QPointF neck;
    QPointF leftShoulder;
    QPointF rightShoulder;
    QPointF leftElbow;
    QPointF rightElbow;
    QPointF leftWrist;
    QPointF rightWrist;
    QPointF leftHip;
    QPointF rightHip;
    QPointF leftKnee;
    QPointF rightKnee;
    QPointF leftAnkle;
    QPointF rightAnkle;
    
    // Body measurements
    float totalHeight;
    float shoulderWidth;
    float hipWidth;
    float torsoLength;
    float legLength;
    float armLength;
};

/**
 * @brief AI Body Detection Model - Offline
 * 
 * Uses OpenPose-style architecture for body detection
 * Detects 18 key body points and segments body parts
 */
class AIBodyDetector : public QObject {
    Q_OBJECT

public:
    explicit AIBodyDetector(QObject *parent = nullptr);
    ~AIBodyDetector();

    // Model management
    bool loadModel(const QString &modelPath);
    bool isModelLoaded() const;
    void unloadModel();
    
    // Detection methods
    BodyPose detectPose(const QImage &image);
    QVector<BodyPart> detectBodyParts(const QImage &image);
    
    // Specific part detection
    BodyPart detectTorso(const QImage &image);
    BodyPart detectHead(const QImage &image);
    QVector<BodyPart> detectArms(const QImage &image);
    QVector<BodyPart> detectLegs(const QImage &image);
    
    // Body measurements
    float calculateShoulderWidth(const BodyPose &pose);
    float calculateWaistWidth(const BodyPose &pose);
    float calculateHipWidth(const BodyPose &pose);
    float calculateLegLength(const BodyPose &pose);
    float calculateArmLength(const BodyPose &pose);
    
    // Body proportions
    float calculateBodyRatio(const BodyPose &pose);
    float calculateWaistToHipRatio(const BodyPose &pose);
    
    // Detection parameters
    void setConfidenceThreshold(float threshold);
    float confidenceThreshold() const;
    
    void setInputSize(const QSize &size);
    QSize inputSize() const;

signals:
    void detectionCompleted(const BodyPose &pose);
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

#endif // AIBODYDETECTOR_H

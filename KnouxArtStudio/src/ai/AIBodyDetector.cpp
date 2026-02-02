#include "AIBodyDetector.h"
#include <QPainter>
#include <QtMath>

namespace Knoux {
namespace AI {

// ============================================================================
// Private Implementation
// ============================================================================

class AIBodyDetector::Impl {
public:
    bool modelLoaded = false;
    float confidenceThreshold = 0.5f;
    QSize inputSize = QSize(368, 368);
    
    // Body keypoint connections (OpenPose COCO format)
    struct KeypointConnection {
        int from;
        int to;
        QString name;
    };
    
    QVector<KeypointConnection> skeleton = {
        {0, 1, "neck"},           // Nose to Neck
        {1, 2, "right_shoulder"}, // Neck to R-Shoulder
        {1, 5, "left_shoulder"},  // Neck to L-Shoulder
        {2, 3, "right_arm"},      // R-Shoulder to R-Elbow
        {3, 4, "right_forearm"},  // R-Elbow to R-Wrist
        {5, 6, "left_arm"},       // L-Shoulder to L-Elbow
        {6, 7, "left_forearm"},   // L-Elbow to L-Wrist
        {1, 8, "right_torso"},    // Neck to R-Hip
        {1, 11, "left_torso"},    // Neck to L-Hip
        {8, 9, "right_thigh"},    // R-Hip to R-Knee
        {9, 10, "right_leg"},     // R-Knee to R-Ankle
        {11, 12, "left_thigh"},   // L-Hip to L-Knee
        {12, 13, "left_leg"},     // L-Knee to L-Ankle
    };
    
    // 18 keypoints in COCO format
    enum KeypointIndex {
        NOSE = 0,
        NECK = 1,
        RIGHT_SHOULDER = 2,
        RIGHT_ELBOW = 3,
        RIGHT_WRIST = 4,
        LEFT_SHOULDER = 5,
        LEFT_ELBOW = 6,
        LEFT_WRIST = 7,
        RIGHT_HIP = 8,
        RIGHT_KNEE = 9,
        RIGHT_ANKLE = 10,
        LEFT_HIP = 11,
        LEFT_KNEE = 12,
        LEFT_ANKLE = 13,
        RIGHT_EYE = 14,
        LEFT_EYE = 15,
        RIGHT_EAR = 16,
        LEFT_EAR = 17
    };
    
    void initializeModel() {
        // Initialize detection parameters
        // In real implementation, load neural network weights
    }
    
    QVector<QPointF> detectKeypoints(const QImage &image) {
        // Simulated keypoint detection
        // In real implementation, this would run the neural network
        
        QVector<QPointF> keypoints(18);
        
        int w = image.width();
        int h = image.height();
        
        // Estimate body position based on image analysis
        // This is a simplified estimation for demonstration
        
        QPointF bodyCenter(w / 2, h / 2);
        float bodyScale = qMin(w, h) * 0.4f;
        
        // Nose (top center)
        keypoints[NOSE] = QPointF(bodyCenter.x(), bodyCenter.y() - bodyScale * 0.8f);
        
        // Neck
        keypoints[NECK] = QPointF(bodyCenter.x(), bodyCenter.y() - bodyScale * 0.5f);
        
        // Shoulders
        keypoints[RIGHT_SHOULDER] = QPointF(bodyCenter.x() - bodyScale * 0.4f, bodyCenter.y() - bodyScale * 0.45f);
        keypoints[LEFT_SHOULDER] = QPointF(bodyCenter.x() + bodyScale * 0.4f, bodyCenter.y() - bodyScale * 0.45f);
        
        // Elbows
        keypoints[RIGHT_ELBOW] = QPointF(bodyCenter.x() - bodyScale * 0.6f, bodyCenter.y() - bodyScale * 0.1f);
        keypoints[LEFT_ELBOW] = QPointF(bodyCenter.x() + bodyScale * 0.6f, bodyCenter.y() - bodyScale * 0.1f);
        
        // Wrists
        keypoints[RIGHT_WRIST] = QPointF(bodyCenter.x() - bodyScale * 0.5f, bodyCenter.y() + bodyScale * 0.3f);
        keypoints[LEFT_WRIST] = QPointF(bodyCenter.x() + bodyScale * 0.5f, bodyCenter.y() + bodyScale * 0.3f);
        
        // Hips
        keypoints[RIGHT_HIP] = QPointF(bodyCenter.x() - bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.1f);
        keypoints[LEFT_HIP] = QPointF(bodyCenter.x() + bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.1f);
        
        // Knees
        keypoints[RIGHT_KNEE] = QPointF(bodyCenter.x() - bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.5f);
        keypoints[LEFT_KNEE] = QPointF(bodyCenter.x() + bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.5f);
        
        // Ankles
        keypoints[RIGHT_ANKLE] = QPointF(bodyCenter.x() - bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.9f);
        keypoints[LEFT_ANKLE] = QPointF(bodyCenter.x() + bodyScale * 0.25f, bodyCenter.y() + bodyScale * 0.9f);
        
        // Eyes
        keypoints[RIGHT_EYE] = QPointF(bodyCenter.x() - bodyScale * 0.1f, bodyCenter.y() - bodyScale * 0.75f);
        keypoints[LEFT_EYE] = QPointF(bodyCenter.x() + bodyScale * 0.1f, bodyCenter.y() - bodyScale * 0.75f);
        
        // Ears
        keypoints[RIGHT_EAR] = QPointF(bodyCenter.x() - bodyScale * 0.2f, bodyCenter.y() - bodyScale * 0.7f);
        keypoints[LEFT_EAR] = QPointF(bodyCenter.x() + bodyScale * 0.2f, bodyCenter.y() - bodyScale * 0.7f);
        
        return keypoints;
    }
    
    BodyPart createBodyPart(const QString &name, const QVector<QPointF> &keypoints, 
                            const QVector<int> &indices, float confidence) {
        BodyPart part;
        part.name = name;
        part.confidence = confidence;
        
        // Calculate bounding box
        if (!indices.isEmpty()) {
            float minX = keypoints[indices[0]].x();
            float maxX = keypoints[indices[0]].x();
            float minY = keypoints[indices[0]].y();
            float maxY = keypoints[indices[0]].y();
            
            for (int i = 1; i < indices.size(); ++i) {
                minX = qMin(minX, keypoints[indices[i]].x());
                maxX = qMax(maxX, keypoints[indices[i]].x());
                minY = qMin(minY, keypoints[indices[i]].y());
                maxY = qMax(maxY, keypoints[indices[i]].y());
            }
            
            part.boundingBox = QRect(
                static_cast<int>(minX),
                static_cast<int>(minY),
                static_cast<int>(maxX - minX),
                static_cast<int>(maxY - minY)
            );
            
            // Create contour from keypoints
            for (int idx : indices) {
                part.contour << keypoints[idx].toPoint();
            }
            
            // Calculate center
            part.center = QPointF((minX + maxX) / 2, (minY + maxY) / 2);
            
            // Calculate dimensions
            part.width = maxX - minX;
            part.height = maxY - minY;
            part.aspectRatio = part.height > 0 ? part.width / part.height : 1.0f;
        }
        
        return part;
    }
    
    BodyPose createBodyPose(const QVector<QPointF> &keypoints) {
        BodyPose pose;
        
        // Set key joints
        pose.nose = keypoints[NOSE];
        pose.neck = keypoints[NECK];
        pose.leftShoulder = keypoints[LEFT_SHOULDER];
        pose.rightShoulder = keypoints[RIGHT_SHOULDER];
        pose.leftElbow = keypoints[LEFT_ELBOW];
        pose.rightElbow = keypoints[RIGHT_ELBOW];
        pose.leftWrist = keypoints[LEFT_WRIST];
        pose.rightWrist = keypoints[RIGHT_WRIST];
        pose.leftHip = keypoints[LEFT_HIP];
        pose.rightHip = keypoints[RIGHT_HIP];
        pose.leftKnee = keypoints[LEFT_KNEE];
        pose.rightKnee = keypoints[RIGHT_KNEE];
        pose.leftAnkle = keypoints[LEFT_ANKLE];
        pose.rightAnkle = keypoints[RIGHT_ANKLE];
        
        // Calculate measurements
        pose.shoulderWidth = std::sqrt(
            std::pow(pose.leftShoulder.x() - pose.rightShoulder.x(), 2) +
            std::pow(pose.leftShoulder.y() - pose.rightShoulder.y(), 2)
        );
        
        pose.hipWidth = std::sqrt(
            std::pow(pose.leftHip.x() - pose.rightHip.x(), 2) +
            std::pow(pose.leftHip.y() - pose.rightHip.y(), 2)
        );
        
        pose.torsoLength = std::sqrt(
            std::pow(pose.neck.x() - (pose.leftHip.x() + pose.rightHip.x()) / 2, 2) +
            std::pow(pose.neck.y() - (pose.leftHip.y() + pose.rightHip.y()) / 2, 2)
        );
        
        pose.legLength = (
            std::sqrt(
                std::pow(pose.leftHip.x() - pose.leftAnkle.x(), 2) +
                std::pow(pose.leftHip.y() - pose.leftAnkle.y(), 2)
            ) +
            std::sqrt(
                std::pow(pose.rightHip.x() - pose.rightAnkle.x(), 2) +
                std::pow(pose.rightHip.y() - pose.rightAnkle.y(), 2)
            )
        ) / 2.0f;
        
        pose.armLength = (
            std::sqrt(
                std::pow(pose.leftShoulder.x() - pose.leftWrist.x(), 2) +
                std::pow(pose.leftShoulder.y() - pose.leftWrist.y(), 2)
            ) +
            std::sqrt(
                std::pow(pose.rightShoulder.x() - pose.rightWrist.x(), 2) +
                std::pow(pose.rightShoulder.y() - pose.rightWrist.y(), 2)
            )
        ) / 2.0f;
        
        pose.totalHeight = std::sqrt(
            std::pow(pose.nose.x() - (pose.leftAnkle.x() + pose.rightAnkle.x()) / 2, 2) +
            std::pow(pose.nose.y() - (pose.leftAnkle.y() + pose.rightAnkle.y()) / 2, 2)
        );
        
        // Create body parts
        pose.parts.append(createBodyPart("head", keypoints, {NOSE, RIGHT_EYE, LEFT_EYE, RIGHT_EAR, LEFT_EAR}, 0.9f));
        pose.parts.append(createBodyPart("torso", keypoints, {NECK, RIGHT_SHOULDER, LEFT_SHOULDER, RIGHT_HIP, LEFT_HIP}, 0.85f));
        pose.parts.append(createBodyPart("right_arm", keypoints, {RIGHT_SHOULDER, RIGHT_ELBOW, RIGHT_WRIST}, 0.8f));
        pose.parts.append(createBodyPart("left_arm", keypoints, {LEFT_SHOULDER, LEFT_ELBOW, LEFT_WRIST}, 0.8f));
        pose.parts.append(createBodyPart("right_leg", keypoints, {RIGHT_HIP, RIGHT_KNEE, RIGHT_ANKLE}, 0.85f));
        pose.parts.append(createBodyPart("left_leg", keypoints, {LEFT_HIP, LEFT_KNEE, LEFT_ANKLE}, 0.85f));
        
        return pose;
    }
    
    float estimateConfidence(const QImage &image, const QVector<QPointF> &keypoints) {
        // Estimate detection confidence based on image quality and keypoint distribution
        float confidence = 0.5f;
        
        // Check if keypoints are within image bounds
        int validPoints = 0;
        for (const QPointF &kp : keypoints) {
            if (kp.x() >= 0 && kp.x() < image.width() &&
                kp.y() >= 0 && kp.y() < image.height()) {
                validPoints++;
            }
        }
        
        confidence += (validPoints / 18.0f) * 0.3f;
        
        // Check body proportions
        if (keypoints.size() >= 18) {
            float shoulderWidth = std::abs(keypoints[LEFT_SHOULDER].x() - keypoints[RIGHT_SHOULDER].x());
            float hipWidth = std::abs(keypoints[LEFT_HIP].x() - keypoints[RIGHT_HIP].x());
            
            if (shoulderWidth > 0 && hipWidth > 0) {
                float ratio = shoulderWidth / hipWidth;
                if (ratio > 0.8f && ratio < 2.0f) {
                    confidence += 0.2f;
                }
            }
        }
        
        return qMin(1.0f, confidence);
    }
};

// ============================================================================
// Public Implementation
// ============================================================================

AIBodyDetector::AIBodyDetector(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
    d->initializeModel();
}

AIBodyDetector::~AIBodyDetector() = default;

bool AIBodyDetector::loadModel(const QString &modelPath) {
    Q_UNUSED(modelPath)
    d->modelLoaded = true;
    emit modelLoaded();
    return true;
}

bool AIBodyDetector::isModelLoaded() const {
    return d->modelLoaded;
}

void AIBodyDetector::unloadModel() {
    d->modelLoaded = false;
    emit modelUnloaded();
}

BodyPose AIBodyDetector::detectPose(const QImage &image) {
    if (image.isNull()) {
        emit error(tr("صورة فارغة"));
        return BodyPose();
    }
    
    emit detectionProgress(0);
    
    // Detect keypoints
    QVector<QPointF> keypoints = d->detectKeypoints(image);
    
    emit detectionProgress(50);
    
    // Create body pose from keypoints
    BodyPose pose = d->createBodyPose(keypoints);
    
    // Estimate confidence
    float confidence = d->estimateConfidence(image, keypoints);
    for (auto &part : pose.parts) {
        part.confidence *= confidence;
    }
    
    emit detectionProgress(100);
    emit detectionCompleted(pose);
    
    return pose;
}

QVector<BodyPart> AIBodyDetector::detectBodyParts(const QImage &image) {
    BodyPose pose = detectPose(image);
    return pose.parts;
}

BodyPart AIBodyDetector::detectTorso(const QImage &image) {
    QVector<BodyPart> parts = detectBodyParts(image);
    
    for (const auto &part : parts) {
        if (part.name == "torso") {
            return part;
        }
    }
    
    return BodyPart();
}

BodyPart AIBodyDetector::detectHead(const QImage &image) {
    QVector<BodyPart> parts = detectBodyParts(image);
    
    for (const auto &part : parts) {
        if (part.name == "head") {
            return part;
        }
    }
    
    return BodyPart();
}

QVector<BodyPart> AIBodyDetector::detectArms(const QImage &image) {
    QVector<BodyPart> parts = detectBodyParts(image);
    QVector<BodyPart> arms;
    
    for (const auto &part : parts) {
        if (part.name.contains("arm")) {
            arms.append(part);
        }
    }
    
    return arms;
}

QVector<BodyPart> AIBodyDetector::detectLegs(const QImage &image) {
    QVector<BodyPart> parts = detectBodyParts(image);
    QVector<BodyPart> legs;
    
    for (const auto &part : parts) {
        if (part.name.contains("leg")) {
            legs.append(part);
        }
    }
    
    return legs;
}

float AIBodyDetector::calculateShoulderWidth(const BodyPose &pose) {
    return pose.shoulderWidth;
}

float AIBodyDetector::calculateWaistWidth(const BodyPose &pose) {
    // Estimate waist as midpoint between shoulders and hips
    QPointF waistCenter(
        (pose.leftShoulder.x() + pose.rightShoulder.x() + pose.leftHip.x() + pose.rightHip.x()) / 4,
        (pose.leftShoulder.y() + pose.rightShoulder.y() + pose.leftHip.y() + pose.rightHip.y()) / 4
    );
    
    // Estimate waist width based on shoulder and hip proportions
    float estimatedWaist = pose.shoulderWidth * 0.7f;
    
    return estimatedWaist;
}

float AIBodyDetector::calculateHipWidth(const BodyPose &pose) {
    return pose.hipWidth;
}

float AIBodyDetector::calculateLegLength(const BodyPose &pose) {
    return pose.legLength;
}

float AIBodyDetector::calculateArmLength(const BodyPose &pose) {
    return pose.armLength;
}

float AIBodyDetector::calculateBodyRatio(const BodyPose &pose) {
    if (pose.legLength <= 0) return 0.0f;
    return pose.totalHeight / pose.legLength;
}

float AIBodyDetector::calculateWaistToHipRatio(const BodyPose &pose) {
    float waist = calculateWaistWidth(pose);
    if (pose.hipWidth <= 0) return 0.0f;
    return waist / pose.hipWidth;
}

void AIBodyDetector::setConfidenceThreshold(float threshold) {
    d->confidenceThreshold = qBound(0.0f, threshold, 1.0f);
}

float AIBodyDetector::confidenceThreshold() const {
    return d->confidenceThreshold;
}

void AIBodyDetector::setInputSize(const QSize &size) {
    d->inputSize = size;
}

QSize AIBodyDetector::inputSize() const {
    return d->inputSize;
}

} // namespace AI
} // namespace Knoux


#include "AIModelManager.h"
#include <QDebug>

AIModelManager* AIModelManager::s_instance = nullptr;

AIModelManager* AIModelManager::instance() {
    if (!s_instance) s_instance = new AIModelManager();
    return s_instance;
}

AIModelManager::AIModelManager(QObject *parent) : QObject(parent) {}

void AIModelManager::initialize() {
    qDebug() << "🏛️ AI Model Manager: Initializing local weights on Drive F:";
    // Logic to verify checksums of ONNX files in F:\KnouxArtStudio\ai_models
}

bool AIModelManager::isGPUAvailable() const {
    // Detect CUDA/OpenCL
    return true; 
}

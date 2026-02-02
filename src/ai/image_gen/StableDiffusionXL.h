#ifndef STABLEDIFFUSIONXL_H
#define STABLEDIFFUSIONXL_H

#include <QObject>
#include <QImage>

struct SDGenerationParams {
    QString prompt;
    QString negativePrompt;
    int width = 1024;
    int height = 1024;
    int steps = 4;
    int seed = -1;
};

class StableDiffusionXL : public QObject
{
    Q_OBJECT
public:
    explicit StableDiffusionXL(QObject *parent = nullptr);

    bool loadModel(const QString &path);
    QImage generate(const SDGenerationParams &params);

signals:
    void progressChanged(int percent);
    void generationFinished(bool success);
};

#endif // STABLEDIFFUSIONXL_H

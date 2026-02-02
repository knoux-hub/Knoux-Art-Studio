#ifndef REALESRGAN_H
#define REALESRGAN_H

#include <QObject>
#include <QImage>

class RealESRGAN : public QObject
{
    Q_OBJECT
public:
    explicit RealESRGAN(QObject *parent = nullptr);

    bool loadModel(const QString &path);
    QImage upscale(const QImage &input, int scale = 4);

signals:
    void progressChanged(int percent);
};

#endif // REALESRGAN_H

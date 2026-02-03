#ifndef RMBG_H
#define RMBG_H

#include <QObject>
#include <QImage>

class RMBG : public QObject
{
    Q_OBJECT
public:
    explicit RMBG(QObject *parent = nullptr);

    bool loadModel(const QString &path);
    QImage removeBackground(const QImage &input);

signals:
    void progressChanged(int percent);
};

#endif // RMBG_H

#ifndef AIMODELMANAGER_H
#define AIMODELMANAGER_H

#include <QObject>

class AIModelManager : public QObject
{
    Q_OBJECT
public:
    static AIModelManager* instance();

    void initialize();
    bool isGPUAvailable() const;

signals:
    void modelLoaded(const QString &modelId, bool success);

private:
    explicit AIModelManager(QObject *parent = nullptr);
    static AIModelManager *s_instance;
};

#endif // AIMODELMANAGER_H

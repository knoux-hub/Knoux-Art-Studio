#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include "../ui/NotificationManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    enum AppMode { Mode_Home, Mode_Photo, Mode_Video, Mode_AI, Mode_Body, Mode_Face, Mode_Makeup, Mode_Settings };
    void switchMode(AppMode mode);
private:
    void setupSovereignUI();
    void applyNeonTheme();
    QStackedWidget *m_centralStack;
    NotificationManager *m_notificationManager;
};
#endif

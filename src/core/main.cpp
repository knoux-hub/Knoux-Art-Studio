#include "MainWindow.h"
#include <QApplication>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Knoux Art Studio");
    MainWindow w;
    w.showMaximized();
    return app.exec();
}

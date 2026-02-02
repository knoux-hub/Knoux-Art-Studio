#include "MainWindow.h"
#include "../ui/CyberpunkSplash.h"

#include <QApplication>
#include <QFontDatabase>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include <QDir>
#include <QDebug>

void loadFonts()
{
    // Load custom fonts
    QStringList fontPaths = {
        ":/fonts/Dubai-Regular.ttf",
        ":/fonts/Dubai-Bold.ttf",
        ":/fonts/Orbitron-Regular.ttf",
        ":/fonts/Orbitron-Bold.ttf"
    };

    for (const QString &path : fontPaths) {
        int id = QFontDatabase::addApplicationFont(path);
        if (id == -1) {
            qDebug() << "Failed to load font:" << path;
        } else {
            QStringList families = QFontDatabase::applicationFontFamilies(id);
            qDebug() << "Loaded font families:" << families;
        }
    }
}

void setupApplication(QApplication &app)
{
    // Application metadata
    app.setApplicationName("Knoux Art Studio");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Knoux");
    app.setOrganizationDomain("knoux.studio");

    // Load settings
    QSettings settings;

    // Set application-wide font
    QFont appFont("Dubai", 11);
    if (!QFontDatabase::hasFamily("Dubai")) {
        appFont = QFont("Segoe UI", 10);
    }
    app.setFont(appFont);

    // Load translator if needed
    QString language = settings.value("Appearance/language", "ar").toString();
    if (language == "ar") {
        QTranslator *translator = new QTranslator(&app);
        if (translator->load(":/translations/knoux_ar.qm")) {
            app.installTranslator(translator);
        }
    }

    // Set RTL layout for Arabic
    bool rtl = settings.value("Appearance/rtlLayout", true).toBool();
    if (rtl) {
        app.setLayoutDirection(Qt::RightToLeft);
    }
}

void createDefaultDirectories()
{
    QStringList dirs = {
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux/Projects",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux/Exports",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux/Models",
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/Knoux",
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/Knoux/Thumbnails",
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/Knoux/Temp"
    };

    for (const QString &dir : dirs) {
        QDir().mkpath(dir);
    }
}

int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);

    // Load fonts
    loadFonts();

    // Setup application
    setupApplication(app);

    // Create default directories
    createDefaultDirectories();

    // Create and show main window
    MainWindow window;

    // Show splash screen
    CyberpunkSplash splash;
    splash.show();

    // Process events to show splash
    app.processEvents();

    // Initialize main window in background
    window.setWindowState(Qt::WindowMaximized);

    // Close splash and show main window after delay
    QTimer::singleShot(2500, &splash, [&]() {
        splash.finish();
        window.show();
    });

    return app.exec();
}

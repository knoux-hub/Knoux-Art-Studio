#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

#include <QWidget>
#include <QSettings>

class GlassButton;
class GlassPanel;
class QLabel;
class QSlider;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QLineEdit;
class QTabWidget;

class SettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPanel(QWidget *parent = nullptr);

    void loadSettings();
    void saveSettings();

public slots:
    void resetToDefaults();
    void applySettings();

signals:
    void settingsChanged(const QString &category);
    void themeChanged(const QString &theme);
    void languageChanged(const QString &language);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onTabChanged(int index);
    void onThemeChanged(int index);
    void onLanguageChanged(int index);
    void onAccentColorChanged(int index);
    void onGlassOpacityChanged(int value);
    void onAnimationSpeedChanged(int value);
    void onAutoSaveChanged(bool enabled);
    void onAutoSaveIntervalChanged(int minutes);
    void onDefaultProjectPathChanged();
    void onBrowseProjectPath();
    void onCacheSizeChanged(int mb);
    void onClearCache();
    void onGpuAccelerationChanged(bool enabled);
    void onMemoryLimitChanged(int gb);
    void onThreadCountChanged(int count);
    void onCheckUpdates();
    void onResetSettings();
    void onExportSettings();
    void onImportSettings();

private:
    void setupUI();
    void createAppearanceTab();
    void createGeneralTab();
    void createPerformanceTab();
    void createShortcutsTab();
    void createAboutTab();
    void setupConnections();

    void loadAppearanceSettings();
    void loadGeneralSettings();
    void loadPerformanceSettings();
    void loadShortcutSettings();

    void saveAppearanceSettings();
    void saveGeneralSettings();
    void savePerformanceSettings();
    void saveShortcutSettings();

    void applyTheme(const QString &theme);
    void applyLanguage(const QString &language);
    void applyAccentColor(const QColor &color);

    // UI Components
    QTabWidget *m_tabWidget;

    // Appearance tab
    QComboBox *m_themeCombo;
    QComboBox *m_languageCombo;
    QComboBox *m_accentColorCombo;
    QSlider *m_glassOpacitySlider;
    QSlider *m_animationSpeedSlider;
    QCheckBox *m_showAnimationsCheck;
    QCheckBox *m_showGlowEffectsCheck;
    QCheckBox *m_rtlLayoutCheck;

    // General tab
    QCheckBox *m_autoSaveCheck;
    QSpinBox *m_autoSaveIntervalSpin;
    QLineEdit *m_defaultPathEdit;
    QCheckBox *m_confirmDeleteCheck;
    QCheckBox *m_showTooltipsCheck;
    QCheckBox *m_showWelcomeCheck;
    QComboBox *m_defaultEditorCombo;

    // Performance tab
    QCheckBox *m_gpuAccelerationCheck;
    QSpinBox *m_memoryLimitSpin;
    QSpinBox *m_threadCountSpin;
    QSpinBox *m_cacheSizeSpin;
    QCheckBox *m_previewOnHoverCheck;
    QComboBox *m_previewQualityCombo;
    GlassButton *m_clearCacheBtn;

    // Shortcuts tab
    QMap<QString, QLineEdit*> m_shortcutEdits;

    // About tab
    QLabel *m_versionLabel;
    QLabel *m_buildLabel;
    QLabel *m_copyrightLabel;
    GlassButton *m_checkUpdatesBtn;

    // Settings storage
    QSettings *m_settings;

    // Default values
    static constexpr int DEFAULT_AUTO_SAVE_INTERVAL = 5;
    static constexpr int DEFAULT_MEMORY_LIMIT = 4;
    static constexpr int DEFAULT_THREAD_COUNT = 4;
    static constexpr int DEFAULT_CACHE_SIZE = 1024;
    static constexpr int DEFAULT_GLASS_OPACITY = 15;
    static constexpr int DEFAULT_ANIMATION_SPEED = 100;
};

// Shortcut Edit Widget
class ShortcutEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ShortcutEdit(QWidget *parent = nullptr);

    void setShortcut(const QKeySequence &shortcut);
    QKeySequence shortcut() const;

signals:
    void shortcutChanged(const QKeySequence &shortcut);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void updateDisplay();

    QKeySequence m_shortcut;
    bool m_recording;
};

#endif // SETTINGSPANEL_H

#include "SettingsPanel.h"
#include "GlassButton.h"
#include "GlassPanel.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_PURPLE QColor(180, 0, 255)

SettingsPanel::SettingsPanel(QWidget *parent)
    : QWidget(parent)
{
    m_settings = new QSettings("Knoux", "ArtStudio", this);
    setupUI();
    setupConnections();
    loadSettings();
}

void SettingsPanel::setupUI()
{
    setObjectName("settingsPanel");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel(tr("⚙ الإعدادات"), this);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { background: rgba(0, 0, 0, 0.2); border: 1px solid rgba(255, 255, 255, 0.1); "
        "border-radius: 8px; }"
        "QTabBar::tab { background: rgba(255, 255, 255, 0.05); color: #AAAAAA; padding: 12px 25px; "
        "border-top-left-radius: 8px; border-top-right-radius: 8px; }"
        "QTabBar::tab:selected { background: rgba(0, 255, 255, 0.2); color: #00FFFF; }"
        "QTabBar::tab:hover { background: rgba(255, 255, 255, 0.1); color: #FFFFFF; }"
    );

    createAppearanceTab();
    createGeneralTab();
    createPerformanceTab();
    createShortcutsTab();
    createAboutTab();

    mainLayout->addWidget(m_tabWidget, 1);

    // Bottom buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    GlassButton *resetBtn = new GlassButton(tr("🔄 إعادة الضبط"), this);
    resetBtn->setFixedHeight(40);
    connect(resetBtn, &GlassButton::clicked, this, &SettingsPanel::resetToDefaults);
    btnLayout->addWidget(resetBtn);

    GlassButton *exportBtn = new GlassButton(tr("📤 تصدير"), this);
    exportBtn->setFixedHeight(40);
    connect(exportBtn, &GlassButton::clicked, this, &SettingsPanel::onExportSettings);
    btnLayout->addWidget(exportBtn);

    GlassButton *importBtn = new GlassButton(tr("📥 استيراد"), this);
    importBtn->setFixedHeight(40);
    connect(importBtn, &GlassButton::clicked, this, &SettingsPanel::onImportSettings);
    btnLayout->addWidget(importBtn);

    GlassButton *applyBtn = new GlassButton(tr("✓ تطبيق"), this);
    applyBtn->setFixedHeight(40);
    applyBtn->setAccentColor(CP_ACCENT_CYAN);
    connect(applyBtn, &GlassButton::clicked, this, &SettingsPanel::applySettings);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);
}

void SettingsPanel::createAppearanceTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // Theme section
    QGroupBox *themeGroup = new QGroupBox(tr("المظهر"), tab);
    themeGroup->setStyleSheet(
        "QGroupBox { color: #FFFFFF; font-size: 14px; font-weight: bold; "
        "border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
    );

    QGridLayout *themeLayout = new QGridLayout(themeGroup);
    themeLayout->setSpacing(15);

    // Theme
    themeLayout->addWidget(new QLabel(tr("السمة:")), 0, 0);
    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({tr("داكن (افتراضي)"), tr("داكن أزرق"), tr("سايبر بانك"), tr("مظلم")});
    themeLayout->addWidget(m_themeCombo, 0, 1);

    // Language
    themeLayout->addWidget(new QLabel(tr("اللغة:")), 1, 0);
    m_languageCombo = new QComboBox();
    m_languageCombo->addItems({tr("العربية"), tr("English"), tr("Français"), tr("Español")});
    themeLayout->addWidget(m_languageCombo, 1, 1);

    // Accent color
    themeLayout->addWidget(new QLabel(tr("لون التمييز:")), 2, 0);
    m_accentColorCombo = new QComboBox();
    m_accentColorCombo->addItems({tr("سماوي"), tr("وردي"), tr("أرجواني"), tr("أخضر"), tr("برتقالي")});
    themeLayout->addWidget(m_accentColorCombo, 2, 1);

    layout->addWidget(themeGroup);

    // Glassmorphism section
    QGroupBox *glassGroup = new QGroupBox(tr("التأثيرات الزجاجية"), tab);
    glassGroup->setStyleSheet(themeGroup->styleSheet());

    QGridLayout *glassLayout = new QGridLayout(glassGroup);
    glassLayout->setSpacing(15);

    // Glass opacity
    glassLayout->addWidget(new QLabel(tr("شفافية الزجاج:")), 0, 0);
    m_glassOpacitySlider = new QSlider(Qt::Horizontal);
    m_glassOpacitySlider->setRange(0, 50);
    m_glassOpacitySlider->setValue(DEFAULT_GLASS_OPACITY);
    glassLayout->addWidget(m_glassOpacitySlider, 0, 1);
    QLabel *opacityValue = new QLabel("15%");
    connect(m_glassOpacitySlider, &QSlider::valueChanged, [opacityValue](int v) {
        opacityValue->setText(QString("%1%").arg(v));
    });
    glassLayout->addWidget(opacityValue, 0, 2);

    // Animation speed
    glassLayout->addWidget(new QLabel(tr("سرعة الحركة:")), 1, 0);
    m_animationSpeedSlider = new QSlider(Qt::Horizontal);
    m_animationSpeedSlider->setRange(0, 200);
    m_animationSpeedSlider->setValue(DEFAULT_ANIMATION_SPEED);
    glassLayout->addWidget(m_animationSpeedSlider, 1, 1);
    QLabel *speedValue = new QLabel("100%");
    connect(m_animationSpeedSlider, &QSlider::valueChanged, [speedValue](int v) {
        speedValue->setText(QString("%1%").arg(v));
    });
    glassLayout->addWidget(speedValue, 1, 2);

    // Show animations
    m_showAnimationsCheck = new QCheckBox(tr("إظهار الحركات"));
    m_showAnimationsCheck->setChecked(true);
    glassLayout->addWidget(m_showAnimationsCheck, 2, 0, 1, 3);

    // Show glow effects
    m_showGlowEffectsCheck = new QCheckBox(tr("إظهار تأثيرات التوهج"));
    m_showGlowEffectsCheck->setChecked(true);
    glassLayout->addWidget(m_showGlowEffectsCheck, 3, 0, 1, 3);

    // RTL layout
    m_rtlLayoutCheck = new QCheckBox(tr("تخطيط من اليمين لليسار"));
    m_rtlLayoutCheck->setChecked(true);
    glassLayout->addWidget(m_rtlLayoutCheck, 4, 0, 1, 3);

    layout->addWidget(glassGroup);
    layout->addStretch();

    m_tabWidget->addTab(tab, tr("المظهر"));
}

void SettingsPanel::createGeneralTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // Auto-save section
    QGroupBox *autoSaveGroup = new QGroupBox(tr("الحفظ التلقائي"), tab);
    autoSaveGroup->setStyleSheet(
        "QGroupBox { color: #FFFFFF; font-size: 14px; font-weight: bold; "
        "border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
    );

    QGridLayout *autoSaveLayout = new QGridLayout(autoSaveGroup);
    autoSaveLayout->setSpacing(15);

    m_autoSaveCheck = new QCheckBox(tr("تفعيل الحفظ التلقائي"));
    m_autoSaveCheck->setChecked(true);
    autoSaveLayout->addWidget(m_autoSaveCheck, 0, 0, 1, 3);

    autoSaveLayout->addWidget(new QLabel(tr("الفاصل الزمني (دقائق):")), 1, 0);
    m_autoSaveIntervalSpin = new QSpinBox();
    m_autoSaveIntervalSpin->setRange(1, 60);
    m_autoSaveIntervalSpin->setValue(DEFAULT_AUTO_SAVE_INTERVAL);
    autoSaveLayout->addWidget(m_autoSaveIntervalSpin, 1, 1);

    layout->addWidget(autoSaveGroup);

    // Files section
    QGroupBox *filesGroup = new QGroupBox(tr("الملفات"), tab);
    filesGroup->setStyleSheet(autoSaveGroup->styleSheet());

    QGridLayout *filesLayout = new QGridLayout(filesGroup);
    filesLayout->setSpacing(15);

    filesLayout->addWidget(new QLabel(tr("مجلد المشاريع الافتراضي:")), 0, 0);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    m_defaultPathEdit = new QLineEdit();
    m_defaultPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux");
    pathLayout->addWidget(m_defaultPathEdit);

    QPushButton *browseBtn = new QPushButton(tr("استعراض..."));
    connect(browseBtn, &QPushButton::clicked, this, &SettingsPanel::onBrowseProjectPath);
    pathLayout->addWidget(browseBtn);

    filesLayout->addLayout(pathLayout, 0, 1);

    m_confirmDeleteCheck = new QCheckBox(tr("التأكيد قبل الحذف"));
    m_confirmDeleteCheck->setChecked(true);
    filesLayout->addWidget(m_confirmDeleteCheck, 1, 0, 1, 2);

    layout->addWidget(filesGroup);

    // Interface section
    QGroupBox *interfaceGroup = new QGroupBox(tr("الواجهة"), tab);
    interfaceGroup->setStyleSheet(autoSaveGroup->styleSheet());

    QVBoxLayout *interfaceLayout = new QVBoxLayout(interfaceGroup);
    interfaceLayout->setSpacing(10);

    m_showTooltipsCheck = new QCheckBox(tr("إظهار تلميحات الأدوات"));
    m_showTooltipsCheck->setChecked(true);
    interfaceLayout->addWidget(m_showTooltipsCheck);

    m_showWelcomeCheck = new QCheckBox(tr("إظهار شاشة الترحيب"));
    m_showWelcomeCheck->setChecked(true);
    interfaceLayout->addWidget(m_showWelcomeCheck);

    QHBoxLayout *editorLayout = new QHBoxLayout();
    editorLayout->addWidget(new QLabel(tr("المحرر الافتراضي:")));
    m_defaultEditorCombo = new QComboBox();
    m_defaultEditorCombo->addItems({tr("محرر الصور"), tr("محرر الفيديو"), tr("استوديو AI")});
    editorLayout->addWidget(m_defaultEditorCombo);
    editorLayout->addStretch();
    interfaceLayout->addLayout(editorLayout);

    layout->addWidget(interfaceGroup);
    layout->addStretch();

    m_tabWidget->addTab(tab, tr("عام"));
}

void SettingsPanel::createPerformanceTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    // Hardware acceleration
    QGroupBox *hwGroup = new QGroupBox(tr("التسريع"), tab);
    hwGroup->setStyleSheet(
        "QGroupBox { color: #FFFFFF; font-size: 14px; font-weight: bold; "
        "border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
    );

    QGridLayout *hwLayout = new QGridLayout(hwGroup);
    hwLayout->setSpacing(15);

    m_gpuAccelerationCheck = new QCheckBox(tr("تسريع GPU (CUDA/OpenCL)"));
    m_gpuAccelerationCheck->setChecked(true);
    hwLayout->addWidget(m_gpuAccelerationCheck, 0, 0, 1, 3);

    hwLayout->addWidget(new QLabel(tr("حد الذاكرة (GB):")), 1, 0);
    m_memoryLimitSpin = new QSpinBox();
    m_memoryLimitSpin->setRange(1, 64);
    m_memoryLimitSpin->setValue(DEFAULT_MEMORY_LIMIT);
    hwLayout->addWidget(m_memoryLimitSpin, 1, 1);

    hwLayout->addWidget(new QLabel(tr("عدد الخيوط:")), 2, 0);
    m_threadCountSpin = new QSpinBox();
    m_threadCountSpin->setRange(1, 32);
    m_threadCountSpin->setValue(DEFAULT_THREAD_COUNT);
    hwLayout->addWidget(m_threadCountSpin, 2, 1);

    layout->addWidget(hwGroup);

    // Cache
    QGroupBox *cacheGroup = new QGroupBox(tr("الذاكرة المؤقتة"), tab);
    cacheGroup->setStyleSheet(hwGroup->styleSheet());

    QGridLayout *cacheLayout = new QGridLayout(cacheGroup);
    cacheLayout->setSpacing(15);

    cacheLayout->addWidget(new QLabel(tr("حجم الذاكرة المؤقتة (MB):")), 0, 0);
    m_cacheSizeSpin = new QSpinBox();
    m_cacheSizeSpin->setRange(100, 10000);
    m_cacheSizeSpin->setValue(DEFAULT_CACHE_SIZE);
    m_cacheSizeSpin->setSingleStep(100);
    cacheLayout->addWidget(m_cacheSizeSpin, 0, 1);

    m_clearCacheBtn = new GlassButton(tr("مسح الذاكرة المؤقتة"));
    m_clearCacheBtn->setFixedHeight(35);
    connect(m_clearCacheBtn, &GlassButton::clicked, this, &SettingsPanel::onClearCache);
    cacheLayout->addWidget(m_clearCacheBtn, 1, 0, 1, 2);

    layout->addWidget(cacheGroup);

    // Preview
    QGroupBox *previewGroup = new QGroupBox(tr("المعاينة"), tab);
    previewGroup->setStyleSheet(hwGroup->styleSheet());

    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    previewLayout->setSpacing(10);

    m_previewOnHoverCheck = new QCheckBox(tr("معاينة عند المرور"));
    m_previewOnHoverCheck->setChecked(true);
    previewLayout->addWidget(m_previewOnHoverCheck);

    QHBoxLayout *qualityLayout = new QHBoxLayout();
    qualityLayout->addWidget(new QLabel(tr("جودة المعاينة:")));
    m_previewQualityCombo = new QComboBox();
    m_previewQualityCombo->addItems({tr("منخفضة"), tr("متوسطة"), tr("عالية")});
    m_previewQualityCombo->setCurrentIndex(1);
    qualityLayout->addWidget(m_previewQualityCombo);
    qualityLayout->addStretch();
    previewLayout->addLayout(qualityLayout);

    layout->addWidget(previewGroup);
    layout->addStretch();

    m_tabWidget->addTab(tab, tr("الأداء"));
}

void SettingsPanel::createShortcutsTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *infoLabel = new QLabel(tr("انقر على اختصار لتغييره"));
    infoLabel->setStyleSheet("color: #888888; font-size: 12px;");
    layout->addWidget(infoLabel);

    // Shortcuts list
    QScrollArea *scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *shortcutsContainer = new QWidget();
    QGridLayout *shortcutsLayout = new QGridLayout(shortcutsContainer);
    shortcutsLayout->setSpacing(10);

    QStringList actions = {
        tr("فتح ملف"), tr("حفظ"), tr("حفظ باسم"), tr("تصدير"),
        tr("تراجع"), tr("إعادة"), tr("قص"), tr("نسخ"), tr("لصق"),
        tr("تحديد الكل"), tr("حذف"), tr("بحث"),
        tr("تشغيل/إيقاف"), tr("إطار تالي"), tr("إطار سابق"),
        tr("تكبير"), tr("تصغير"), tr("ملاءمة الشاشة")
    };

    QStringList defaults = {
        "Ctrl+O", "Ctrl+S", "Ctrl+Shift+S", "Ctrl+E",
        "Ctrl+Z", "Ctrl+Y", "Ctrl+X", "Ctrl+C", "Ctrl+V",
        "Ctrl+A", "Delete", "Ctrl+F",
        "Space", "Right", "Left",
        "Ctrl++", "Ctrl+-", "Ctrl+0"
    };

    for (int i = 0; i < actions.size(); ++i) {
        shortcutsLayout->addWidget(new QLabel(actions[i]), i, 0);

        ShortcutEdit *edit = new ShortcutEdit();
        edit->setShortcut(QKeySequence(defaults[i]));
        shortcutsLayout->addWidget(edit, i, 1);

        m_shortcutEdits[actions[i]] = edit;
    }

    scrollArea->setWidget(shortcutsContainer);
    layout->addWidget(scrollArea);

    m_tabWidget->addTab(tab, tr("الاختصارات"));
}

void SettingsPanel::createAboutTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);

    // Logo
    QLabel *logoLabel = new QLabel("◈");
    logoLabel->setStyleSheet("color: #00FFFF; font-size: 72px;");
    logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(logoLabel);

    // App name
    QLabel *nameLabel = new QLabel("KNOUX ART STUDIO");
    nameLabel->setStyleSheet("color: #FFFFFF; font-size: 28px; font-weight: bold;");
    nameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(nameLabel);

    // Version
    m_versionLabel = new QLabel(tr("الإصدار 1.0.0"));
    m_versionLabel->setStyleSheet("color: #888888; font-size: 14px;");
    m_versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_versionLabel);

    // Build info
    m_buildLabel = new QLabel(tr("البنية: 2024.02.03") + " | Qt " + QT_VERSION_STR);
    m_buildLabel->setStyleSheet("color: #666666; font-size: 12px;");
    m_buildLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_buildLabel);

    layout->addSpacing(20);

    // Description
    QLabel *descLabel = new QLabel(tr("استوديو متكامل للتصميم والتحرير\nمع دعم كامل للذكاء الاصطناعي المحلي"));
    descLabel->setStyleSheet("color: #AAAAAA; font-size: 13px;");
    descLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(descLabel);

    layout->addSpacing(20);

    // Copyright
    m_copyrightLabel = new QLabel("© 2024 Knoux Studio. " + tr("جميع الحقوق محفوظة."));
    m_copyrightLabel->setStyleSheet("color: #666666; font-size: 11px;");
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_copyrightLabel);

    layout->addSpacing(30);

    // Check updates button
    m_checkUpdatesBtn = new GlassButton(tr("🔍 التحقق من التحديثات"));
    m_checkUpdatesBtn->setFixedSize(200, 45);
    connect(m_checkUpdatesBtn, &GlassButton::clicked, this, &SettingsPanel::onCheckUpdates);
    layout->addWidget(m_checkUpdatesBtn, 0, Qt::AlignCenter);

    layout->addStretch();

    m_tabWidget->addTab(tab, tr("حول"));
}

void SettingsPanel::setupConnections()
{
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsPanel::onThemeChanged);
    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsPanel::onLanguageChanged);
    connect(m_accentColorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsPanel::onAccentColorChanged);
}

void SettingsPanel::loadSettings()
{
    loadAppearanceSettings();
    loadGeneralSettings();
    loadPerformanceSettings();
    loadShortcutSettings();
}

void SettingsPanel::loadAppearanceSettings()
{
    m_settings->beginGroup("Appearance");
    m_themeCombo->setCurrentIndex(m_settings->value("theme", 0).toInt());
    m_languageCombo->setCurrentIndex(m_settings->value("language", 0).toInt());
    m_accentColorCombo->setCurrentIndex(m_settings->value("accentColor", 0).toInt());
    m_glassOpacitySlider->setValue(m_settings->value("glassOpacity", DEFAULT_GLASS_OPACITY).toInt());
    m_animationSpeedSlider->setValue(m_settings->value("animationSpeed", DEFAULT_ANIMATION_SPEED).toInt());
    m_showAnimationsCheck->setChecked(m_settings->value("showAnimations", true).toBool());
    m_showGlowEffectsCheck->setChecked(m_settings->value("showGlowEffects", true).toBool());
    m_rtlLayoutCheck->setChecked(m_settings->value("rtlLayout", true).toBool());
    m_settings->endGroup();
}

void SettingsPanel::loadGeneralSettings()
{
    m_settings->beginGroup("General");
    m_autoSaveCheck->setChecked(m_settings->value("autoSave", true).toBool());
    m_autoSaveIntervalSpin->setValue(m_settings->value("autoSaveInterval", DEFAULT_AUTO_SAVE_INTERVAL).toInt());
    m_defaultPathEdit->setText(m_settings->value("defaultPath",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Knoux").toString());
    m_confirmDeleteCheck->setChecked(m_settings->value("confirmDelete", true).toBool());
    m_showTooltipsCheck->setChecked(m_settings->value("showTooltips", true).toBool());
    m_showWelcomeCheck->setChecked(m_settings->value("showWelcome", true).toBool());
    m_defaultEditorCombo->setCurrentIndex(m_settings->value("defaultEditor", 0).toInt());
    m_settings->endGroup();
}

void SettingsPanel::loadPerformanceSettings()
{
    m_settings->beginGroup("Performance");
    m_gpuAccelerationCheck->setChecked(m_settings->value("gpuAcceleration", true).toBool());
    m_memoryLimitSpin->setValue(m_settings->value("memoryLimit", DEFAULT_MEMORY_LIMIT).toInt());
    m_threadCountSpin->setValue(m_settings->value("threadCount", DEFAULT_THREAD_COUNT).toInt());
    m_cacheSizeSpin->setValue(m_settings->value("cacheSize", DEFAULT_CACHE_SIZE).toInt());
    m_previewOnHoverCheck->setChecked(m_settings->value("previewOnHover", true).toBool());
    m_previewQualityCombo->setCurrentIndex(m_settings->value("previewQuality", 1).toInt());
    m_settings->endGroup();
}

void SettingsPanel::loadShortcutSettings()
{
    m_settings->beginGroup("Shortcuts");
    for (auto it = m_shortcutEdits.begin(); it != m_shortcutEdits.end(); ++it) {
        QString key = it.key();
        QString defaultShortcut = it.value()->shortcut().toString();
        QString savedShortcut = m_settings->value(key, defaultShortcut).toString();
        it.value()->setShortcut(QKeySequence(savedShortcut));
    }
    m_settings->endGroup();
}

void SettingsPanel::saveSettings()
{
    saveAppearanceSettings();
    saveGeneralSettings();
    savePerformanceSettings();
    saveShortcutSettings();

    m_settings->sync();
}

void SettingsPanel::saveAppearanceSettings()
{
    m_settings->beginGroup("Appearance");
    m_settings->setValue("theme", m_themeCombo->currentIndex());
    m_settings->setValue("language", m_languageCombo->currentIndex());
    m_settings->setValue("accentColor", m_accentColorCombo->currentIndex());
    m_settings->setValue("glassOpacity", m_glassOpacitySlider->value());
    m_settings->setValue("animationSpeed", m_animationSpeedSlider->value());
    m_settings->setValue("showAnimations", m_showAnimationsCheck->isChecked());
    m_settings->setValue("showGlowEffects", m_showGlowEffectsCheck->isChecked());
    m_settings->setValue("rtlLayout", m_rtlLayoutCheck->isChecked());
    m_settings->endGroup();
}

void SettingsPanel::saveGeneralSettings()
{
    m_settings->beginGroup("General");
    m_settings->setValue("autoSave", m_autoSaveCheck->isChecked());
    m_settings->setValue("autoSaveInterval", m_autoSaveIntervalSpin->value());
    m_settings->setValue("defaultPath", m_defaultPathEdit->text());
    m_settings->setValue("confirmDelete", m_confirmDeleteCheck->isChecked());
    m_settings->setValue("showTooltips", m_showTooltipsCheck->isChecked());
    m_settings->setValue("showWelcome", m_showWelcomeCheck->isChecked());
    m_settings->setValue("defaultEditor", m_defaultEditorCombo->currentIndex());
    m_settings->endGroup();
}

void SettingsPanel::savePerformanceSettings()
{
    m_settings->beginGroup("Performance");
    m_settings->setValue("gpuAcceleration", m_gpuAccelerationCheck->isChecked());
    m_settings->setValue("memoryLimit", m_memoryLimitSpin->value());
    m_settings->setValue("threadCount", m_threadCountSpin->value());
    m_settings->setValue("cacheSize", m_cacheSizeSpin->value());
    m_settings->setValue("previewOnHover", m_previewOnHoverCheck->isChecked());
    m_settings->setValue("previewQuality", m_previewQualityCombo->currentIndex());
    m_settings->endGroup();
}

void SettingsPanel::saveShortcutSettings()
{
    m_settings->beginGroup("Shortcuts");
    for (auto it = m_shortcutEdits.begin(); it != m_shortcutEdits.end(); ++it) {
        m_settings->setValue(it.key(), it.value()->shortcut().toString());
    }
    m_settings->endGroup();
}

void SettingsPanel::applySettings()
{
    saveSettings();

    // Apply theme
    applyTheme(m_themeCombo->currentText());

    // Apply language
    applyLanguage(m_languageCombo->currentText());

    // Apply accent color
    QColor accentColors[] = {CP_ACCENT_CYAN, CP_ACCENT_PINK, CP_ACCENT_PURPLE,
                             QColor(0, 255, 128), QColor(255, 128, 0)};
    applyAccentColor(accentColors[m_accentColorCombo->currentIndex()]);

    emit settingsChanged("all");
}

void SettingsPanel::resetToDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("إعادة الضبط"),
        tr("هل أنت متأكد من إعادة جميع الإعدادات للقيم الافتراضية؟"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_settings->clear();
        loadSettings();
        applySettings();
    }
}

void SettingsPanel::onThemeChanged(int index)
{
    Q_UNUSED(index)
    // Theme will be applied when user clicks Apply
}

void SettingsPanel::onLanguageChanged(int index)
{
    Q_UNUSED(index)
    // Language will be applied when user clicks Apply
}

void SettingsPanel::onAccentColorChanged(int index)
{
    Q_UNUSED(index)
    // Color will be applied when user clicks Apply
}

void SettingsPanel::onBrowseProjectPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("اختيار مجلد المشاريع"),
                                                      m_defaultPathEdit->text());
    if (!path.isEmpty()) {
        m_defaultPathEdit->setText(path);
    }
}

void SettingsPanel::onClearCache()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("مسح الذاكرة المؤقتة"),
        tr("هل أنت متأكد من مسح الذاكرة المؤقتة؟"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Clear cache directory
        QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/Knoux";
        QDir cacheDir(cachePath);
        if (cacheDir.exists()) {
            cacheDir.removeRecursively();
        }

        QMessageBox::information(this, tr("تم"), tr("تم مسح الذاكرة المؤقتة بنجاح"));
    }
}

void SettingsPanel::onCheckUpdates()
{
    m_checkUpdatesBtn->setText(tr("جاري التحقق..."));
    m_checkUpdatesBtn->setEnabled(false);

    // Simulate update check
    QTimer::singleShot(2000, this, [this]() {
        m_checkUpdatesBtn->setText(tr("🔍 التحقق من التحديثات"));
        m_checkUpdatesBtn->setEnabled(true);

        QMessageBox::information(this, tr("التحديثات"),
            tr("أنت تستخدم أحدث إصدار من Knoux Art Studio."));
    });
}

void SettingsPanel::onExportSettings()
{
    QString path = QFileDialog::getSaveFileName(this, tr("تصدير الإعدادات"), QString(),
        tr("ملف الإعدادات (*.ini);;كل الملفات (*)"));

    if (!path.isEmpty()) {
        QSettings exportSettings(path, QSettings::IniFormat);

        // Copy all settings
        for (const QString &group : m_settings->childGroups()) {
            m_settings->beginGroup(group);
            exportSettings.beginGroup(group);
            for (const QString &key : m_settings->childKeys()) {
                exportSettings.setValue(key, m_settings->value(key));
            }
            exportSettings.endGroup();
            m_settings->endGroup();
        }

        QMessageBox::information(this, tr("تم"), tr("تم تصدير الإعدادات بنجاح"));
    }
}

void SettingsPanel::onImportSettings()
{
    QString path = QFileDialog::getOpenFileName(this, tr("استيراد الإعدادات"), QString(),
        tr("ملف الإعدادات (*.ini);;كل الملفات (*)"));

    if (!path.isEmpty()) {
        QSettings importSettings(path, QSettings::IniFormat);

        // Copy all settings
        for (const QString &group : importSettings.childGroups()) {
            importSettings.beginGroup(group);
            m_settings->beginGroup(group);
            for (const QString &key : importSettings.childKeys()) {
                m_settings->setValue(key, importSettings.value(key));
            }
            m_settings->endGroup();
            importSettings.endGroup();
        }

        loadSettings();
        applySettings();

        QMessageBox::information(this, tr("تم"), tr("تم استيراد الإعدادات بنجاح"));
    }
}

void SettingsPanel::applyTheme(const QString &theme)
{
    Q_UNUSED(theme)
    emit themeChanged(theme);
}

void SettingsPanel::applyLanguage(const QString &language)
{
    Q_UNUSED(language)
    emit languageChanged(language);
}

void SettingsPanel::applyAccentColor(const QColor &color)
{
    Q_UNUSED(color)
}

void SettingsPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadSettings();
}

// ==================== ShortcutEdit Implementation ====================

ShortcutEdit::ShortcutEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_recording(false)
{
    setReadOnly(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(
        "QLineEdit { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 6px; color: #FFFFFF; padding: 8px; min-width: 100px; }"
        "QLineEdit:focus { border: 1px solid #00FFFF; }"
    );
}

void ShortcutEdit::setShortcut(const QKeySequence &shortcut)
{
    m_shortcut = shortcut;
    updateDisplay();
}

QKeySequence ShortcutEdit::shortcut() const
{
    return m_shortcut;
}

void ShortcutEdit::keyPressEvent(QKeyEvent *event)
{
    if (m_recording) {
        if (event->key() == Qt::Key_Escape) {
            m_recording = false;
            updateDisplay();
            clearFocus();
            return;
        }

        if (event->key() != Qt::Key_unknown) {
            m_shortcut = QKeySequence(event->modifiers() | event->key());
            m_recording = false;
            updateDisplay();
            clearFocus();
            emit shortcutChanged(m_shortcut);
        }
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void ShortcutEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    m_recording = true;
    setText(tr("اضغط مفتاحاً..."));
    setStyleSheet(
        "QLineEdit { background: rgba(0, 255, 255, 0.1); border: 1px solid #00FFFF; "
        "border-radius: 6px; color: #00FFFF; padding: 8px; min-width: 100px; }"
    );
}

void ShortcutEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    m_recording = false;
    updateDisplay();
}

void ShortcutEdit::updateDisplay()
{
    setText(m_shortcut.toString(QKeySequence::NativeText));
    setStyleSheet(
        "QLineEdit { background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 6px; color: #FFFFFF; padding: 8px; min-width: 100px; }"
        "QLineEdit:focus { border: 1px solid #00FFFF; }"
    );
}

# Knoux Art Studio - Complete Setup and Installation Script
# This script sets up the entire project with all files

param(
    [string]$InstallPath = "F:\KnouxArtStudio",
    [switch]$BuildAfterSetup = $false
)

$ErrorActionPreference = "Stop"

# Color output
function Write-Color($text, $color) {
    $colors = @{
        "Green" = "32"
        "Cyan" = "36"
        "Yellow" = "33"
        "Red" = "31"
    }
    $code = $colors[$color]
    Write-Host "\033[$($code)m$text\033[0m"
}

function Write-Header($text) {
    Write-Color "========================================" "Cyan"
    Write-Color "  $text" "Cyan"
    Write-Color "========================================" "Cyan"
}

function Write-Success($text) {
    Write-Color "✓ $text" "Green"
}

function Write-Error($text) {
    Write-Color "✗ $text" "Red"
}

Write-Header "Knoux Art Studio - Complete Setup"

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Warning "Running without administrator privileges. Some features may be limited."
}

# Create project directory
Write-Host "`nCreating project directory..."
if (-not (Test-Path $InstallPath)) {
    New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
}
Write-Success "Project directory created: $InstallPath"

# Create directory structure
$directories = @(
    "src\core"
    "src\ui\components"
    "src\ui\dialogs"
    "src\ui\panels"
    "src\ui\widgets"
    "src\photo_editor"
    "src\video_editor"
    "src\ai"
    "src\utils"
    "include"
    "resources\icons\dark"
    "resources\icons\light"
    "resources\fonts"
    "resources\themes"
    "resources\images\splash"
    "resources\images\branding"
    "resources\sounds"
    "resources\translations"
    "ai_models\stable_diffusion\checkpoints"
    "ai_models\stable_diffusion\vae"
    "ai_models\stable_diffusion\loras"
    "ai_models\upscalers"
    "ai_models\segmentation"
    "ai_models\enhancement"
    "ai_models\custom"
    "config\presets"
    "config\shortcuts"
    "docs\api"
    "docs\user_manual\ar"
    "docs\user_manual\en"
    "docs\tutorials"
    "assets\brushes"
    "assets\gradients"
    "assets\patterns"
    "assets\presets\photo"
    "assets\presets\video"
    "assets\templates\social"
    "assets\templates\print"
    "build\debug\bin"
    "build\release\bin"
    "build\installer"
    "scripts\build"
    "scripts\deploy"
    "scripts\test"
    "tests\unit\core"
    "tests\unit\ui"
    "tests\integration"
    "tests\ui_automation"
    "third_party\include"
    "third_party\lib"
    "installer\windows"
    "installer\macos"
    "installer\linux"
)

Write-Host "`nCreating directory structure..."
foreach ($dir in $directories) {
    $fullPath = Join-Path $InstallPath $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "  + $dir"
    }
}
Write-Success "Directory structure created"

# Create resource files
Write-Host "`nCreating resource files..."

# icons.qrc
$iconsQrc = @'
<RCC>
    <qresource prefix="/icons">
        <!-- App Icons -->
        <file>icons/app_icon.png</file>
        <file>icons/app_icon_32.png</file>
        <file>icons/app_icon_48.png</file>
        <file>icons/app_icon_256.png</file>
        
        <!-- Navigation -->
        <file>icons/navigation/home.png</file>
        <file>icons/navigation/photo.png</file>
        <file>icons/navigation/video.png</file>
        <file>icons/navigation/ai.png</file>
        <file>icons/navigation/settings.png</file>
        
        <!-- File Operations -->
        <file>icons/file/new.png</file>
        <file>icons/file/open.png</file>
        <file>icons/file/save.png</file>
        <file>icons/file/save_as.png</file>
        <file>icons/file/export.png</file>
        <file>icons/file/import.png</file>
        <file>icons/file/close.png</file>
        
        <!-- Edit Operations -->
        <file>icons/edit/undo.png</file>
        <file>icons/edit/redo.png</file>
        <file>icons/edit/cut.png</file>
        <file>icons/edit/copy.png</file>
        <file>icons/edit/paste.png</file>
        <file>icons/edit/delete.png</file>
        <file>icons/edit/select_all.png</file>
        <file>icons/edit/deselect.png</file>
        
        <!-- View Controls -->
        <file>icons/view/zoom_in.png</file>
        <file>icons/view/zoom_out.png</file>
        <file>icons/view/zoom_fit.png</file>
        <file>icons/view/zoom_100.png</file>
        <file>icons/view/fullscreen.png</file>
        <file>icons/view/grid.png</file>
        <file>icons/view/rulers.png</file>
        <file>icons/view/guides.png</file>
        
        <!-- Photo Tools -->
        <file>icons/tools/move.png</file>
        <file>icons/tools/select.png</file>
        <file>icons/tools/crop.png</file>
        <file>icons/tools/brush.png</file>
        <file>icons/tools/eraser.png</file>
        <file>icons/tools/clone.png</file>
        <file>icons/tools/heal.png</file>
        <file>icons/tools/gradient.png</file>
        <file>icons/tools/text.png</file>
        <file>icons/tools/shapes.png</file>
        <file>icons/tools/eyedropper.png</file>
        <file>icons/tools/hand.png</file>
        
        <!-- Transform -->
        <file>icons/transform/rotate_left.png</file>
        <file>icons/transform/rotate_right.png</file>
        <file>icons/transform/flip_h.png</file>
        <file>icons/transform/flip_v.png</file>
        <file>icons/transform/resize.png</file>
        <file>icons/transform/distort.png</file>
        
        <!-- Layer Operations -->
        <file>icons/layer/new.png</file>
        <file>icons/layer/duplicate.png</file>
        <file>icons/layer/delete.png</file>
        <file>icons/layer/merge.png</file>
        <file>icons/layer/group.png</file>
        <file>icons/layer/mask.png</file>
        <file>icons/layer/adjustment.png</file>
        
        <!-- Filters -->
        <file>icons/filters/blur.png</file>
        <file>icons/filters/sharpen.png</file>
        <file>icons/filters/noise.png</file>
        <file>icons/filters/pixelate.png</file>
        <file>icons/filters/edge.png</file>
        <file>icons/filters/emboss.png</file>
        
        <!-- Video Controls -->
        <file>icons/video/play.png</file>
        <file>icons/video/pause.png</file>
        <file>icons/video/stop.png</file>
        <file>icons/video/record.png</file>
        <file>icons/video/next_frame.png</file>
        <file>icons/video/prev_frame.png</file>
        <file>icons/video/skip_start.png</file>
        <file>icons/video/skip_end.png</file>
        <file>icons/video/loop.png</file>
        
        <!-- Timeline -->
        <file>icons/timeline/razor.png</file>
        <file>icons/timeline/marker.png</file>
        <file>icons/timeline/keyframe.png</file>
        <file>icons/timeline/transition.png</file>
        <file>icons/timeline/effect.png</file>
        <file>icons/timeline/audio.png</file>
        
        <!-- AI Features -->
        <file>icons/ai/generate.png</file>
        <file>icons/ai/enhance.png</file>
        <file>icons/ai/upscale.png</file>
        <file>icons/ai/remove_bg.png</file>
        <file>icons/ai/style_transfer.png</file>
        <file>icons/ai/colorize.png</file>
        <file>ai/portrait.png</file>
        <file>icons/ai/magic.png</file>
        
        <!-- Status -->
        <file>icons/status/info.png</file>
        <file>icons/status/success.png</file>
        <file>icons/status/warning.png</file>
        <file>icons/status/error.png</file>
        <file>icons/status/loading.png</file>
        <file>icons/status/offline.png</file>
    </qresource>
</RCC>
'@

Set-Content -Path (Join-Path $InstallPath "resources\icons.qrc") -Value $iconsQrc

# styles.qrc
$stylesQrc = @'
<RCC>
    <qresource prefix="/styles">
        <file>themes/cyberpunk_dark.qss</file>
        <file>themes/midnight_blue.qss</file>
        <file>themes/pure_dark.qss</file>
        <file>themes/high_contrast.qss</file>
    </qresource>
    <qresource prefix="/fonts">
        <file>fonts/Dubai-Regular.ttf</file>
        <file>fonts/Dubai-Bold.ttf</file>
        <file>fonts/Dubai-Light.ttf</file>
        <file>fonts/Dubai-Medium.ttf</file>
        <file>fonts/Orbitron-Regular.ttf</file>
        <file>fonts/Orbitron-Bold.ttf</file>
        <file>fonts/Orbitron-Black.ttf</file>
    </qresource>
    <qresource prefix="/images">
        <file>images/splash/splash_bg.png</file>
        <file>images/splash/logo_glow.png</file>
        <file>images/branding/logo.png</file>
        <file>images/branding/logo_text.png</file>
        <file>images/branding/watermark.png</file>
    </qresource>
    <qresource prefix="/sounds">
        <file>sounds/notification.wav</file>
        <file>sounds/success.wav</file>
        <file>sounds/error.wav</file>
        <file>sounds/click.wav</file>
        <file>sounds/complete.wav</file>
    </qresource>
    <qresource prefix="/translations">
        <file>translations/knoux_ar.qm</file>
        <file>translations/knoux_en.qm</file>
        <file>translations/knoux_fr.qm</file>
        <file>translations/knoux_es.qm</file>
        <file>translations/knoux_de.qm</file>
    </qresource>
</RCC>
'@

Set-Content -Path (Join-Path $InstallPath "resources\styles.qrc") -Value $stylesQrc

Write-Success "Resource files created"

# Create CMakeLists.txt
Write-Host "`nCreating CMakeLists.txt..."
$cmakeContent = @'
cmake_minimum_required(VERSION 3.16)
project(KnouxArtStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    OpenGL
    OpenGLWidgets
)

# Source files
set(SOURCES
    src/core/main.cpp
    src/core/MainWindow.cpp
    src/ui/GlassButton.cpp
    src/ui/GlassPanel.cpp
    src/ui/CyberpunkSplash.cpp
    src/ui/HomeScreen.cpp
    src/ui/SettingsPanel.cpp
    src/ui/NotificationManager.cpp
    src/photo_editor/PhotoEditor.cpp
    src/video_editor/VideoEditor.cpp
    src/ai/AIStudio.cpp
)

set(HEADERS
    src/core/MainWindow.h
    src/ui/GlassButton.h
    src/ui/GlassPanel.h
    src/ui/CyberpunkSplash.h
    src/ui/HomeScreen.h
    src/ui/SettingsPanel.h
    src/ui/NotificationManager.h
    src/photo_editor/PhotoEditor.h
    src/video_editor/VideoEditor.h
    src/ai/AIStudio.h
)

set(RESOURCES
    resources/icons.qrc
    resources/styles.qrc
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /MP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    KNOUX_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    KNOUX_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    KNOUX_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    KNOUX_VERSION_STRING="${PROJECT_VERSION}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources/
    DESTINATION share/${PROJECT_NAME}/resources
)
'@

Set-Content -Path (Join-Path $InstallPath "CMakeLists.txt") -Value $cmakeContent
Write-Success "CMakeLists.txt created"

# Create build scripts
Write-Host "`nCreating build scripts..."

# build.bat
$buildBat = @'
@echo off
chcp 65001 >nul
title Knoux Art Studio - Build

echo ==========================================
echo   Knoux Art Studio - Build Script
echo ==========================================
echo.

set BUILD_TYPE=%~1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set BUILD_DIR=build\%BUILD_TYPE%

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%

echo [1/3] Configuring with CMake...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64 ..\.. 2>&1

if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building project...
cmake --build . --config %BUILD_TYPE% --parallel 2>&1

if errorlevel 1 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Copying dependencies...
if exist "..\..\third_party\lib" (
    xcopy /Y /E "..\..\third_party\lib\*.dll" "bin\%BUILD_TYPE%\" 2>nul
)

echo.
echo ==========================================
echo   Build completed successfully!
echo   Output: %BUILD_DIR%\bin\%BUILD_TYPE%\KnouxArtStudio.exe
echo ==========================================
cd ..\..

if "%~2"=="--pause" pause
'@

Set-Content -Path (Join-Path $InstallPath "build.bat") -Value $buildBat

# build.sh
$buildSh = @'#!/bin/bash

echo "=========================================="
echo "  Knoux Art Studio - Build Script"
echo "=========================================="
echo ""

BUILD_TYPE="${1:-Release}"
BUILD_DIR="build/$BUILD_TYPE"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "[1/3] Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ../..

if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed!"
    exit 1
fi

echo ""
echo "[2/3] Building project..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed!"
    exit 1
fi

echo ""
echo "[3/3] Copying dependencies..."
if [ -d "../../third_party/lib" ]; then
    cp -v ../../third_party/lib/*.so bin/ 2>/dev/null || true
fi

echo ""
echo "=========================================="
echo "  Build completed successfully!"
echo "  Output: $BUILD_DIR/bin/KnouxArtStudio"
echo "=========================================="
cd ../..
'@

Set-Content -Path (Join-Path $InstallPath "build.sh") -Value $buildSh

Write-Success "Build scripts created"

# Create README
Write-Host "`nCreating README.md..."
$readme = @'
# Knoux Art Studio

<p align="center">
  <img src="resources/images/branding/logo.png" alt="Knoux Logo" width="200"/>
</p>

<p align="center">
  <strong>استوديو متكامل للتصميم والتحرير مع دعم كامل للذكاء الاصطناعي المحلي</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue?style=for-the-badge" alt="Version"/>
  <img src="https://img.shields.io/badge/Qt-6.5+-green?style=for-the-badge" alt="Qt"/>
  <img src="https://img.shields.io/badge/C++-17-orange?style=for-the-badge" alt="C++"/>
  <img src="https://img.shields.io/badge/license-Proprietary-red?style=for-the-badge" alt="License"/>
</p>

---

## 🌟 المميزات الرئيسية

### 📷 محرر الصور الاحترافي
- تحرير طبقات متقدم مع دعم كامل للشفافية
- أدوات رسم احترافية (فرشاة، ممحاة، استنساخ، شفاء)
- تعديلات اللون الكاملة
- فلاتر وتأثيرات متنوعة
- دعم جميع صيغ الصور الشائعة

### 🎬 محرر الفيديو المتقدم
- Timeline متعدد المسارات
- قص ودمج الفيديو بسهولة
- تأثيرات وانتقالات احترافية
- معالجة الصوت المتكاملة
- تصدير بجودة عالية

### 🤖 استوديو الذكاء الاصطناعي
- توليد الصور من النص
- تحويل الصور باستخدام AI
- تكبير الصور بدون فقدان الجودة
- إزالة الخلفية تلقائياً
- تحسين البورتريه
- نقل النمط الفني

### 🔒 الخصوصية والأمان
- تشغيل كامل محلي - لا يحتاج للإنترنت
- حماية كاملة لبياناتك
- لا يتم إرسال أي ملفات للخوادم

---

## 💻 متطلبات النظام

### الحد الأدنى
| المكون | المتطلب |
|--------|---------|
| نظام التشغيل | Windows 10/11 64-bit |
| المعالج | Intel Core i5 أو ما يعادله |
| الذاكرة | 8 GB RAM |
| المساحة | 2 GB مساحة فارغة |
| كرت الشاشة | يدعم DirectX 11 |

### الموصى به
| المكون | المتطلب |
|--------|---------|
| نظام التشغيل | Windows 11 64-bit |
| المعالج | Intel Core i7/i9 أو AMD Ryzen 7/9 |
| الذاكرة | 16-32 GB RAM |
| المساحة | 10 GB SSD |
| كرت الشاشة | NVIDIA RTX 3060 أو أعلى |

---

## 🚀 التثبيت

### Windows
```powershell
# استنساخ المستودع
git clone https://github.com/knoux/art-studio.git
cd art-studio

# تشغيل سكربت الإعداد
.\Setup-CompleteProject.ps1

# البناء
.\build.bat Release
```

### Linux
```bash
# استنساخ المستودع
git clone https://github.com/knoux/art-studio.git
cd art-studio

# البناء
./build.sh Release
```

---

## ⌨️ الاختصارات

| الاختصار | الوظيفة |
|---------|---------|
| `Ctrl + O` | فتح ملف |
| `Ctrl + S` | حفظ |
| `Ctrl + Shift + S` | حفظ باسم |
| `Ctrl + E` | تصدير |
| `Ctrl + Z` | تراجع |
| `Ctrl + Y` | إعادة |
| `Ctrl + X` | قص |
| `Ctrl + C` | نسخ |
| `Ctrl + V` | لصق |
| `Space` | تشغيل/إيقاف |
| `Delete` | حذف |
| `Ctrl + 1-5` | التبديل بين الأقسام |

---

## 📁 هيكل المشروع

```
KnouxArtStudio/
├── src/                    # Source code
│   ├── core/              # Core application
│   ├── ui/                # UI components
│   ├── photo_editor/      # Photo editor
│   ├── video_editor/      # Video editor
│   └── ai/                # AI studio
├── resources/             # Resources
│   ├── icons/            # Icons
│   ├── fonts/            # Fonts
│   ├── themes/           # Themes
│   └── translations/     # Translations
├── ai_models/            # AI models
├── docs/                 # Documentation
├── tests/                # Tests
└── build/                # Build output
```

---

## 🤝 المساهمة

نرحب بمساهماتكم! يرجى اتباع الخطوات التالية:

1. Fork المستودع
2. إنشاء فرع للميزة (`git checkout -b feature/amazing-feature`)
3. Commit التغييرات (`git commit -m 'Add amazing feature'`)
4. Push للفرع (`git push origin feature/amazing-feature`)
5. فتح Pull Request

---

## 📞 الدعم

- 📧 البريد: support@knoux.studio
- 🌐 الموقع: https://knoux.studio
- 💬 الدعم الفني: متوفر على مدار الساعة

---

## 📄 الترخيص

© 2024 Knoux Studio. جميع الحقوق محفوظة.

هذا البرنامج مرخص للاستخدام الشخصي والتجاري وفق شروط اتفاقية الترخيص.

---

<p align="center">
  <strong>الأمان ليس رفاهية، إنه حياة</strong> 🔒
</p>

<p align="center">
  <img src="resources/images/branding/watermark.png" alt="Knoux" width="100"/>
</p>
'@

Set-Content -Path (Join-Path $InstallPath "README.md") -Value $readme -Encoding UTF8
Write-Success "README.md created"

# Summary
Write-Host ""
Write-Header "Setup Complete!"
Write-Host ""
Write-Color "Project Location: $InstallPath" "Green"
Write-Host ""
Write-Host "Next Steps:"
Write-Host "  1. Copy source files to $InstallPath\src\" -ForegroundColor Cyan
Write-Host "  2. cd '$InstallPath'" -ForegroundColor Cyan
Write-Host "  3. .\build.bat Release" -ForegroundColor Cyan
Write-Host ""
Write-Header "Thank you for choosing Knoux Art Studio!"

if ($BuildAfterSetup) {
    Write-Host "`nStarting build..." -ForegroundColor Yellow
    Set-Location $InstallPath
    & .\build.bat Release --pause
}

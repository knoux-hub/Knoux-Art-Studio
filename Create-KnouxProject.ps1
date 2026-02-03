# Knoux Art Studio - Complete Project Setup Script
# This script creates the entire project structure on drive F:
# Run as Administrator for best results

param(
    [string]$Drive = "F:",
    [string]$ProjectName = "KnouxArtStudio"
)

$ErrorActionPreference = "Stop"

# Color codes for console output
$Green = "\033[32m"
$Cyan = "\033[36m"
$Yellow = "\033[33m"
$Red = "\033[31m"
$Reset = "\033[0m"

function Write-Status($message, $color = $Cyan) {
    Write-Host "$color[Knoux Setup]$Reset $message"
}

function Write-Success($message) {
    Write-Status $message $Green
}

function Write-Warning($message) {
    Write-Status $message $Yellow
}

function Write-Error($message) {
    Write-Status $message $Red
}

# Verify drive exists
if (-not (Test-Path $Drive)) {
    Write-Error "Drive $Drive not found!"
    exit 1
}

$ProjectRoot = Join-Path $Drive $ProjectName

Write-Status "Starting Knoux Art Studio project setup..."
Write-Status "Project root: $ProjectRoot"

# Create directory structure
$directories = @(
    # Source code
    "src\core"
    "src\ui"
    "src\ui\components"
    "src\ui\dialogs"
    "src\ui\panels"
    "src\ui\widgets"
    "src\photo_editor"
    "src\video_editor"
    "src\ai"
    "src\utils"
    "src\models"
    
    # Headers
    "include"
    
    # Resources
    "resources\icons"
    "resources\fonts"
    "resources\themes"
    "resources\images"
    "resources\sounds"
    "resources\translations"
    
    # AI Models
    "ai_models\stable_diffusion"
    "ai_models\upscalers"
    "ai_models\segmentation"
    "ai_models\enhancement"
    "ai_models\custom"
    
    # Configuration
    "config"
    
    # Documentation
    "docs\api"
    "docs\user_manual"
    "docs\tutorials"
    
    # Assets
    "assets\brushes"
    "assets\gradients"
    "assets\patterns"
    "assets\presets"
    "assets\templates"
    
    # Build
    "build\debug"
    "build\release"
    
    # Scripts
    "scripts\build"
    "scripts\deploy"
    "scripts\test"
    
    # Tests
    "tests\unit"
    "tests\integration"
    "tests\ui"
    
    # Third party
    "third_party\qt"
    "third_party\opencv"
    "third_party\onnx"
    "third_party\ffmpeg"
    
    # Installer
    "installer\windows"
    "installer\macos"
    "installer\linux"
)

Write-Status "Creating directory structure..."

foreach ($dir in $directories) {
    $fullPath = Join-Path $ProjectRoot $dir
    if (-not (Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "  Created: $dir"
    }
}

Write-Success "Directory structure created!"

# Create resource files
Write-Status "Creating resource files..."

# Icons resource file
$iconsQrc = @"
<RCC>
    <qresource prefix="/icons">
        <file>icons/app_icon.png</file>
        <file>icons/photo_editor.png</file>
        <file>icons/video_editor.png</file>
        <file>icons/ai_studio.png</file>
        <file>icons/settings.png</file>
        <file>icons/new_file.png</file>
        <file>icons/open_file.png</file>
        <file>icons/save.png</file>
        <file>icons/export.png</file>
        <file>icons/undo.png</file>
        <file>icons/redo.png</file>
        <file>icons/cut.png</file>
        <file>icons/copy.png</file>
        <file>icons/paste.png</file>
        <file>icons/zoom_in.png</file>
        <file>icons/zoom_out.png</file>
        <file>icons/fit_screen.png</file>
        <file>icons/brush.png</file>
        <file>icons/eraser.png</file>
        <file>icons/clone.png</file>
        <file>icons/heal.png</file>
        <file>icons/gradient.png</file>
        <file>icons/text.png</file>
        <file>icons/shape.png</file>
        <file>icons/select.png</file>
        <file>icons/move.png</file>
        <file>icons/crop.png</file>
        <file>icons/rotate.png</file>
        <file>icons/flip.png</file>
        <file>icons/layer.png</file>
        <file>icons/mask.png</file>
        <file>icons/filter.png</file>
        <file>icons/adjustment.png</file>
        <file>icons/ai_magic.png</file>
        <file>icons/play.png</file>
        <file>icons/pause.png</file>
        <file>icons/stop.png</file>
        <file>icons/record.png</file>
        <file>icons/timeline.png</file>
        <file>icons/keyframe.png</file>
        <file>icons/transition.png</file>
        <file>icons/effect.png</file>
        <file>icons/audio.png</file>
    </qresource>
</RCC>
"@

Set-Content -Path (Join-Path $ProjectRoot "resources\icons.qrc") -Value $iconsQrc

# Styles resource file
$stylesQrc = @"
<RCC>
    <qresource prefix="/styles">
        <file>themes/cyberpunk_dark.qss</file>
        <file>themes/midnight_blue.qss</file>
        <file>themes/pure_dark.qss</file>
    </qresource>
    <qresource prefix="/fonts">
        <file>fonts/Dubai-Regular.ttf</file>
        <file>fonts/Dubai-Bold.ttf</file>
        <file>fonts/Dubai-Light.ttf</file>
        <file>fonts/Orbitron-Regular.ttf</file>
        <file>fonts/Orbitron-Bold.ttf</file>
    </qresource>
    <qresource prefix="/translations">
        <file>translations/knoux_ar.qm</file>
        <file>translations/knoux_en.qm</file>
        <file>translations/knoux_fr.qm</file>
    </qresource>
</RCC>
"@

Set-Content -Path (Join-Path $ProjectRoot "resources\styles.qrc") -Value $stylesQrc

Write-Success "Resource files created!"

# Create build scripts
Write-Status "Creating build scripts..."

# Windows build script
$buildBat = @"
@echo off
chcp 65001 >nul
echo ==========================================
echo   Knoux Art Studio - Build Script
echo ==========================================
echo.

set BUILD_TYPE=%~1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set BUILD_DIR=build\%BUILD_TYPE:\=\%

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cd %BUILD_DIR%

echo Configuring with CMake...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -A x64 ..\..

if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building...
cmake --build . --config %BUILD_TYPE% --parallel

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo ==========================================
echo   Build completed successfully!
echo   Output: %BUILD_DIR%\bin\%BUILD_TYPE%\KnouxArtStudio.exe
echo ==========================================
cd ..\..
"@

Set-Content -Path (Join-Path $ProjectRoot "build.bat") -Value $buildBat

# Linux/Mac build script
$buildSh = @"#!/bin/bash

echo "=========================================="
echo "  Knoux Art Studio - Build Script"
echo "=========================================="
echo ""

BUILD_TYPE=\"${1:-Release}\"
BUILD_DIR=\"build/$BUILD_TYPE\"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ../..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Building..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "=========================================="
echo "  Build completed successfully!"
echo "  Output: $BUILD_DIR/bin/KnouxArtStudio"
echo "=========================================="
cd ../..
"@

Set-Content -Path (Join-Path $ProjectRoot "build.sh") -Value $buildSh

Write-Success "Build scripts created!"

# Create README
Write-Status "Creating documentation..."

$readme = @"
# Knoux Art Studio

## استوديو متكامل للتصميم والتحرير مع دعم كامل للذكاء الاصطناعي المحلي

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Qt](https://img.shields.io/badge/Qt-6.5+-green)
![License](https://img.shields.io/badge/license-Proprietary-red)

---

## المميزات الرئيسية

### محرر الصور الاحترافي
- ✅ تحرير طبقات متقدم مع دعم كامل للشفافية
- ✅ أدوات رسم احترافية (فرشاة، ممحاة، استنساخ، شفاء)
- ✅ تعديلات اللون الكاملة (سطوع، تباين، تشبع، درجة اللون)
- ✅ فلاتر وتأثيرات متنوعة
- ✅ دعم جميع صيغ الصور الشائعة

### محرر الفيديو المتقدم
- ✅ Timeline متعدد المسارات
- ✅ قص ودمج الفيديو بسهولة
- ✅ تأثيرات وانتقالات احترافية
- ✅ معالجة الصوت المتكاملة
- ✅ تصدير بجودة عالية

### استوديو الذكاء الاصطناعي
- ✅ توليد الصور من النص
- ✅ تحويل الصور باستخدام AI
- ✅ تكبير الصور بدون فقدان الجودة
- ✅ إزالة الخلفية تلقائياً
- ✅ تحسين البورتريه
- ✅ نقل النمط الفني

### الخصوصية والأمان
- 🔒 تشغيل كامل محلي - لا يحتاج للإنترنت
- 🔒 حماية كاملة لبياناتك
- 🔒 لا يتم إرسال أي ملفات للخوادم

---

## متطلبات النظام

### الحد الأدنى
- Windows 10/11 64-bit
- 8 GB RAM
- 2 GB مساحة فارغة
- كرت شاشة يد DirectX 11

### الموصى به
- Windows 11 64-bit
- 16 GB RAM أو أكثر
- 10 GB مساحة فارغة SSD
- كرت شاشة NVIDIA/AMD حديث

---

## التثبيت

### Windows
1. قم بتشغيل `KnouxArtStudio-Setup.exe`
2. اتبع تعليمات معالج التثبيت
3. استمتع!

### البناء من المصدر
```bash
# استنساخ المستودع
git clone https://github.com/knoux/art-studio.git
cd art-studio

# البناء
./build.sh Release    # Linux/Mac
# أو
build.bat Release     # Windows
```

---

## الاختصارات

| الاختصار | الوظيفة |
|---------|---------|
| Ctrl + O | فتح ملف |
| Ctrl + S | حفظ |
| Ctrl + E | تصدير |
| Ctrl + Z | تراجع |
| Ctrl + Y | إعادة |
| Space | تشغيل/إيقاف |
| Delete | حذف |
| Ctrl + 1-5 | التبديل بين الأقسام |

---

## الدعم

- 📧 البريد: support@knoux.studio
- 🌐 الموقع: https://knoux.studio
- 💬 الدعم الفني: متوفر على مدار الساعة

---

## الترخيص

© 2024 Knoux Studio. جميع الحقوق محفوظة.

هذا البرنامج مرخص للاستخدام الشخصي والتجاري وفق شروط اتفاقية الترخيص.

---

## شكر خاص

- فريق Qt لإطار العمل الرائع
- مجتمع مفتوح المصدر للأدوات المساعدة
- مستخدمينا الأوفياء على دعمهم المستمر

---

**الأمان ليس رفاهية، إنه حياة** 🔒
"@

Set-Content -Path (Join-Path $ProjectRoot "README.md") -Value $readme -Encoding UTF8

Write-Success "Documentation created!"

# Summary
Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  Project Setup Complete!" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Project Location: $ProjectRoot"
Write-Host ""
Write-Host "Next Steps:"
Write-Host "  1. Copy source files to src/ directory"
Write-Host "  2. Run: cd '$ProjectRoot'"
Write-Host "  3. Build: .\build.bat Release"
Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan

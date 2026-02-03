#!/usr/bin/env pwsh
# Knoux Art Studio Activation Script

Write-Host "🎨 تفعيل Knoux Art Studio..." -ForegroundColor Cyan

# Check prerequisites
if (!(Get-Command node -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Node.js غير مثبت" -ForegroundColor Red
    exit 1
}

# Install dependencies
Write-Host "📦 تثبيت المتطلبات..." -ForegroundColor Yellow
npm install

# Check for .env.local
if (!(Test-Path ".env.local")) {
    Write-Host "⚙️ إنشاء ملف البيئة..." -ForegroundColor Yellow
    @"
GEMINI_API_KEY=your_api_key_here
NODE_ENV=development
"@ | Out-File -FilePath ".env.local" -Encoding UTF8
}

# Start the application
Write-Host "🚀 تشغيل التطبيق..." -ForegroundColor Green
npm run dev
#!/bin/bash
# Knoux Art Studio Activation Script

echo "🎨 تفعيل Knoux Art Studio..."

# Check Node.js
if ! command -v node &> /dev/null; then
    echo "❌ Node.js غير مثبت"
    exit 1
fi

# Install dependencies
echo "📦 تثبيت المتطلبات..."
npm install

# Create .env.local if not exists
if [ ! -f ".env.local" ]; then
    echo "⚙️ إنشاء ملف البيئة..."
    cat > .env.local << EOF
GEMINI_API_KEY=your_api_key_here
NODE_ENV=development
EOF
fi

# Start application
echo "🚀 تشغيل التطبيق..."
npm run dev
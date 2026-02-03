#!/usr/bin/env bash
set -euo pipefail

echo "🎬 KNOUX FULL MEDIA STUDIO - LAUNCHER"
echo "====================================="

echo ""
echo "1️⃣  Starting Backend Services..."
npm run backend &

echo "2️⃣  Starting Frontend Development Server..."
npm run dev &

echo "3️⃣  Starting Electron App..."
npm run electron &

echo "4️⃣  Opening Dashboard..."
sleep 3
dashboard_url="http://localhost:3000/studio-dashboard"
if command -v xdg-open >/dev/null 2>&1; then
  xdg-open "$dashboard_url" >/dev/null 2>&1 || true
elif command -v open >/dev/null 2>&1; then
  open "$dashboard_url" >/dev/null 2>&1 || true
fi
echo "Dashboard: $dashboard_url"

echo "5️⃣  Monitoring Services..."
echo ""
echo "📊 SERVICES STATUS:"
echo "   • Backend:    http://localhost:3000/api/health"
echo "   • Frontend:   http://localhost:3000"
echo "   • Electron:   Running in background"
echo "   • Dashboard:  http://localhost:3000/studio-dashboard"
echo ""
echo "🎯 QUICK ACTIONS:"
echo "   Ctrl+C to stop all services"
echo ""
echo "📋 Available Routes:"
echo "   /studio-dashboard    - Main Studio"
echo "   /photo-editor       - Photo Editor"
echo "   /video-studio       - Video Editor"
echo "   /body-editor        - Body Editor"
echo "   /makeup-studio      - Makeup Studio"
echo "   /templates          - Template Gallery"
echo ""
echo "🚀 Studio is ready! Happy editing! 🎨"

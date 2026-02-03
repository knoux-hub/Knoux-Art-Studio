# ==============================================================================
# KNOUX ART STUDIO - REACT VIEWS DEPLOYMENT
# ==============================================================================

$Root = "F:\KnouxArtStudio"

function New-SovereignFile($RelativePath, $Content) {
    $FullPath = Join-Path $Root $RelativePath
    $Content | Out-File -FilePath $FullPath -Encoding utf8 -Force
    Write-Host "  [UI-VIEW] $RelativePath" -ForegroundColor Yellow
}

Write-Host "🎨 Building Advanced Creative Interfaces..." -ForegroundColor White

# 1. views/BodyEditor.tsx
New-SovereignFile "src/views/BodyEditor.tsx" @'
import React, { useState, useRef } from 'react';
import { AppScreen, BodyAdjustParams, MediaAsset } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

const BodyEditor: React.FC<{navigate: any, userMode: any}> = ({ navigate, userMode }) => {
  const engine = KnouxEngine.getInstance();
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [params, setParams] = useState<BodyAdjustParams>({
    shoulderWidth: 1.0, chestWidth: 1.0, waistWidth: 1.0, hipWidth: 1.0, legLength: 1.0, muscleDefinition: 0
  });

  const handleImport = async (e: any) => {
    if (e.target.files?.[0]) setAsset(await engine.importLocalImage(e.target.files[0]));
  };

  return (
    <div className="h-full w-full flex bg-[#07070B] text-white">
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-8 gap-8">
        <button onClick={() => navigate(AppScreen.HOME)} className="text-xl">🏠</button>
        <button onClick={() => document.getElementById('file-input')?.click()} className="text-xl">📂</button>
        <input id="file-input" type="file" className="hidden" onChange={handleImport} />
      </aside>
      <main className="flex-1 flex flex-col p-8 items-center justify-center bg-black/40">
        <h2 className="text-xs font-black tracking-[0.3em] text-[#9B59FF] mb-10">NEURAL BODY SCULPTOR G2</h2>
        {asset ? (
          <img src={asset.thumbnail} className="max-h-[70vh] shadow-2xl rounded-sm" style={{ transform: `scaleX(${params.waistWidth})` }} />
        ) : (
          <div className="opacity-10 text-6xl">🧬</div>
        )}
      </main>
      <aside className="w-80 glass border-r border-white/5 p-8 space-y-10">
        <h3 className="text-[10px] font-black uppercase tracking-widest text-[#9B59FF]">Body Adjustments</h3>
        <div className="space-y-6">
          {['waistWidth', 'shoulderWidth', 'legLength'].map(p => (
            <div key={p} className="space-y-2">
              <label className="text-[10px] opacity-40">{p}</label>
              <input type="range" min="0.7" max="1.3" step="0.01" value={(params as any)[p]} 
                onChange={(e) => setParams({...params, [p]: parseFloat(e.target.value)})}
                className="w-full accent-[#9B59FF]" />
            </div>
          ))}
        </div>
        <button className="w-full py-4 bg-[#9B59FF] rounded-2xl text-[10px] font-black uppercase">Apply Neural Sculpt</button>
      </aside>
    </div>
  );
};
export default BodyEditor;
'@

# 2. views/FaceRetouch.tsx
New-SovereignFile "src/views/FaceRetouch.tsx" @'
import React, { useState } from 'react';
import { AppScreen, FaceAdjustParams } from '../types';

const FaceRetouch: React.FC<{navigate: any, userMode: any}> = ({ navigate, userMode }) => {
  const [params, setParams] = useState<FaceAdjustParams>({
    smoothness: 0, removeBlemishes: 0, eyeSize: 1.0, lipThickness: 1.0, faceWidth: 1.0, jawWidth: 1.0
  });

  return (
    <div className="h-full w-full flex bg-[#07070B] text-white">
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-8 gap-8">
        <button onClick={() => navigate(AppScreen.HOME)} className="text-xl">🏠</button>
      </aside>
      <main className="flex-1 flex flex-col p-12 items-center justify-center relative">
        <div className="absolute inset-0 bg-[radial-gradient(circle_at_50%_50%,_rgba(155,89,255,0.05),_transparent)]"></div>
        <div className="text-center opacity-20">
          <div className="text-9xl mb-4">😊</div>
          <p className="text-xl font-black uppercase tracking-[0.5em]">Facial Optics G2</p>
        </div>
      </main>
      <aside className="w-80 glass border-r border-white/5 p-8">
        <h3 className="text-[10px] font-black uppercase tracking-widest text-[#9B59FF] mb-10">Feature Refinement</h3>
        <div className="space-y-8">
          <div className="space-y-2">
            <div className="flex justify-between text-[10px] font-bold"><span>Smoothness</span><span>{params.smoothness}%</span></div>
            <input type="range" value={params.smoothness} onChange={(e) => setParams({...params, smoothness: parseInt(e.target.value)})} className="w-full accent-[#9B59FF]" />
          </div>
          <div className="space-y-2">
            <div className="flex justify-between text-[10px] font-bold"><span>Eye Size</span><span>{params.eyeSize}x</span></div>
            <input type="range" min="0.8" max="1.2" step="0.01" value={params.eyeSize} onChange={(e) => setParams({...params, eyeSize: parseFloat(e.target.value)})} className="w-full accent-[#9B59FF]" />
          </div>
        </div>
      </aside>
    </div>
  );
};
export default FaceRetouch;
'@

Write-Host "`n✅ All React Creative Views Populated on F:." -ForegroundColor Green

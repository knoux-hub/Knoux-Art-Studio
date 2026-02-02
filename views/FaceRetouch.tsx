import React, { useState, useRef } from 'react';
import { AppScreen, UserMode, FaceAdjustParams, MediaAsset } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; userMode: UserMode; }

const FaceRetouch: React.FC<Props> = ({ navigate }) => {
  const engine = KnouxEngine.getInstance();
  const fileRef = useRef<HTMLInputElement>(null);
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  // Fix: Added missing 'jawWidth' property to the initial state to match FaceAdjustParams interface
  const [params, setParams] = useState<FaceAdjustParams>({
    smoothness: 0, removeBlemishes: 0, eyeSize: 1.0, lipThickness: 1.0, faceWidth: 1.0, jawWidth: 1.0
  });

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) setAsset(await engine.importMedia(e.target.files[0]));
  };

  return (
    <div className="h-full w-full flex bg-[#07070B] pr-16 pl-16">
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-8 gap-8">
        <button onClick={() => fileRef.current?.click()} className="text-2xl">📂</button>
        <input type="file" ref={fileRef} onChange={handleImport} className="hidden" />
        <button className="text-2xl text-[#9B59FF]">😊</button>
        <button className="text-2xl opacity-40">✨</button>
        <button className="text-2xl opacity-40">🦷</button>
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-16 flex items-center justify-between px-10 border-b border-white/5">
          <h2 className="text-sm font-black tracking-widest text-[#9B59FF]">FACIAL RETOUCH CORE G2</h2>
          <button onClick={() => navigate(AppScreen.HOME)} className="text-[10px] opacity-40">✕</button>
        </header>
        <div className="flex-1 bg-black/40 flex items-center justify-center p-12">
          {asset ? <img src={asset.thumbnail} className="max-w-[500px] shadow-2xl rounded-sm" /> : <div className="text-xl font-black opacity-10 uppercase tracking-tighter">Waiting for Face Data</div>}
        </div>
      </main>

      <aside className="w-80 glass border-r border-white/5 p-8 space-y-10 overflow-y-auto custom-scrollbar">
        <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-widest">تعديل الملامح</h3>
        <div className="space-y-8">
          <div className="space-y-3">
             <div className="flex justify-between text-[11px] font-bold"><span>تنعيم البشرة AI</span><span>{params.smoothness}%</span></div>
             <input type="range" value={params.smoothness} onChange={e => setParams({...params, smoothness: parseInt(e.target.value)})} className="w-full h-1 appearance-none bg-white/5 accent-[#9B59FF] rounded-full" />
          </div>
          <div className="space-y-3">
             <div className="flex justify-between text-[11px] font-bold"><span>تكبير العينين</span><span>{params.eyeSize}x</span></div>
             <input type="range" min="0.8" max="1.2" step="0.01" value={params.eyeSize} onChange={e => setParams({...params, eyeSize: parseFloat(e.target.value)})} className="w-full h-1 appearance-none bg-white/5 accent-[#9B59FF] rounded-full" />
          </div>
          <div className="space-y-3">
             <div className="flex justify-between text-[11px] font-bold"><span>سمك الشفاه</span><span>{params.lipThickness}x</span></div>
             <input type="range" min="0.8" max="1.4" step="0.01" value={params.lipThickness} onChange={e => setParams({...params, lipThickness: parseFloat(e.target.value)})} className="w-full h-1 appearance-none bg-white/5 accent-[#9B59FF] rounded-full" />
          </div>
        </div>
        <button className="w-full py-5 bg-white text-black rounded-3xl text-xs font-black shadow-2xl hover:scale-105 active:scale-95 transition-all">بدء المعالجة السيادية</button>
      </aside>
    </div>
  );
};

export default FaceRetouch;
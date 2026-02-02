import React, { useState, useRef } from 'react';
import { AppScreen, UserMode, AdjustmentState, Layer, MediaAsset } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; userMode: UserMode; }

const PhotoEditor: React.FC<Props> = ({ navigate, userMode }) => {
  const engine = KnouxEngine.getInstance();
  const fileInputRef = useRef<HTMLInputElement>(null);
  
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [adjustments, setAdjustments] = useState<AdjustmentState>({
    brightness: 0, contrast: 0, exposure: 0, saturation: 0, temperature: 0, tint: 0, sharpness: 0
  });
  const [layers, setLayers] = useState<Layer[]>([
    { id: '1', name: 'الخلفية السيادية', visible: true, locked: true, opacity: 100, type: 'PIXEL', blendMode: 'normal' }
  ]);

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) {
      const newAsset = await engine.importMedia(e.target.files[0]);
      setAsset(newAsset);
    }
  };

  const updateAdj = (key: keyof AdjustmentState, val: number) => {
    setAdjustments(prev => ({ ...prev, [key]: val }));
  };

  return (
    <div className="h-full w-full flex bg-[#07070B] text-[#F8F8F8] animate-in fade-in duration-500 overflow-hidden pr-16 pl-16">
      {/* Sidebar: Tools */}
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-8 gap-8 z-20">
        <button onClick={() => fileInputRef.current?.click()} className="text-2xl hover:text-[#9B59FF] transition-all" title="استيراد">📂</button>
        <input type="file" ref={fileInputRef} onChange={handleImport} className="hidden" accept="image/*" />
        <div className="h-px w-8 bg-white/10"></div>
        <button className="text-xl opacity-40 hover:opacity-100">✥</button>
        <button className="text-xl opacity-40 hover:opacity-100">⬚</button>
        <button className="text-xl text-[#9B59FF]">🖌</button>
        <button className="text-xl opacity-40 hover:opacity-100">Patch</button>
        <button className="text-xl opacity-40 hover:opacity-100 mt-auto">⚙</button>
      </aside>

      {/* Main Workspace */}
      <div className="flex-1 flex flex-col min-w-0">
        <header className="h-16 flex items-center justify-between px-10 border-b border-white/5 z-10 bg-black/20">
          <div className="flex items-center gap-6">
            <button onClick={() => navigate(AppScreen.HOME)} className="text-xs font-bold text-[#8E8E93] hover:text-white transition-colors">✕ خروج</button>
            <span className="text-[10px] font-black tracking-[0.3em] text-[#9B59FF]">KNOUX OPTICS G2 ALPHA</span>
          </div>
          <div className="flex gap-4">
             <button className="px-5 py-2 glass rounded-xl text-[10px] hover:bg-white/10 transition-all">حفظ المشروع</button>
             <button className="px-6 py-2 bg-[#9B59FF] rounded-xl text-[10px] font-black shadow-lg shadow-[#9B59FF]/20 hover:brightness-110">تصدير سيادي</button>
          </div>
        </header>

        <div className="flex-1 bg-black/40 relative flex items-center justify-center p-16 overflow-hidden">
          {asset ? (
            <div className="relative group shadow-2xl rounded-sm">
               <img 
                 src={asset.thumbnail} 
                 className="max-w-full max-h-full object-contain" 
                 style={{ filter: `brightness(${100 + adjustments.brightness}%) contrast(${100 + adjustments.contrast}%) saturate(${100 + adjustments.saturation}%) blur(${adjustments.sharpness < 0 ? Math.abs(adjustments.sharpness)/10 : 0}px)` }} 
               />
               <div className="absolute inset-0 border border-white/10 pointer-events-none"></div>
            </div>
          ) : (
            <div onClick={() => fileInputRef.current?.click()} className="text-center cursor-pointer group">
               <div className="w-24 h-24 bg-white/5 rounded-full flex items-center justify-center text-4xl mx-auto mb-6 group-hover:scale-110 transition-transform">🖼️</div>
               <h2 className="text-xl font-bold opacity-40 uppercase tracking-widest">Waiting for drive f: input</h2>
               <p className="text-xs opacity-20 mt-2">Sovereign Data Protocol Active</p>
            </div>
          )}
          
          {asset && (
            <div className="absolute bottom-8 right-10 glass px-6 py-2 rounded-full text-[10px] border-white/10 flex gap-8 text-[#8E8E93] font-mono">
               <span>ISO 100</span>
               <span>{asset.name}</span>
               <span className="text-green-500">ENCRYPTED</span>
            </div>
          )}
        </div>
      </div>

      {/* Inspector: Right */}
      <aside className="w-80 glass border-r border-white/5 flex flex-col z-20">
        <div className="p-8 space-y-10 overflow-y-auto custom-scrollbar flex-1">
          <section className="space-y-8">
            <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">المعالجة البصرية</h3>
            
            {[
              { label: 'السطوع', key: 'brightness', min: -100, max: 100 },
              { label: 'التباين', key: 'contrast', min: -100, max: 100 },
              { label: 'التعريض', key: 'exposure', min: -100, max: 100 },
              { label: 'التشبع', key: 'saturation', min: -100, max: 100 },
              { label: 'الحدة', key: 'sharpness', min: -100, max: 100 },
            ].map(adj => (
              <div key={adj.key} className="space-y-3">
                <div className="flex justify-between text-[11px] font-bold">
                  <span className="text-[#8E8E93]">{adj.label}</span>
                  <span className="text-white font-mono">{(adjustments as any)[adj.key]}</span>
                </div>
                <input 
                  type="range" 
                  min={adj.min} max={adj.max} 
                  value={(adjustments as any)[adj.key]} 
                  onChange={(e) => updateAdj(adj.key as any, parseInt(e.target.value))}
                  className="w-full h-1 bg-white/5 rounded-full appearance-none accent-[#9B59FF] cursor-pointer"
                />
              </div>
            ))}
          </section>

          <section className="pt-8 border-t border-white/5 space-y-4">
            <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">أدوات الذكاء السيادي</h3>
            <button className="w-full py-4 glass rounded-2xl text-xs font-bold hover:bg-[#9B59FF]/10 transition-all border-white/10">✨ تحسين G2 التلقائي</button>
            <button className="w-full py-4 glass rounded-2xl text-xs font-bold hover:bg-[#9B59FF]/10 transition-all border-white/10">🩹 ترميم عالي الدقة</button>
          </section>

          <section className="pt-8 border-t border-white/5">
            <h3 className="text-[10px] font-black text-[#8E8E93] uppercase tracking-[0.2em] mb-6">الطبقات</h3>
            <div className="space-y-3">
              {layers.map(l => (
                <div key={l.id} className="flex items-center gap-4 p-4 bg-white/5 rounded-2xl border border-white/5 hover:border-white/10 transition-all cursor-pointer">
                   <div className="w-10 h-10 bg-[#16161D] rounded-lg border border-white/5 overflow-hidden">
                     {asset && <img src={asset.thumbnail} className="w-full h-full object-cover opacity-50" />}
                   </div>
                   <div className="flex-1 min-w-0">
                     <p className="text-[10px] font-bold truncate">{l.name}</p>
                     <p className="text-[8px] opacity-40 uppercase">Pixel Layer</p>
                   </div>
                   <span className="text-[10px] opacity-20">🔒</span>
                </div>
              ))}
              <button className="w-full py-3 border border-dashed border-white/10 rounded-2xl text-[10px] text-[#8E8E93] hover:border-[#9B59FF]/30 transition-all">+ إضافة طبقة</button>
            </div>
          </section>
        </div>

        <footer className="p-6 glass border-t border-white/5 flex justify-between items-center text-[10px] font-mono text-[#8E8E93]">
           <div className="flex items-center gap-2">
             <div className="w-2 h-2 rounded-full bg-green-500 animate-pulse"></div>
             <span>LOCAL GPU</span>
           </div>
           <span>v1.0.0-ALPHA</span>
        </footer>
      </aside>
    </div>
  );
};

export default PhotoEditor;

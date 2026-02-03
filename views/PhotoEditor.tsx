import React, { useState, useRef } from 'react';
import { AppScreen, UserMode, AdjustmentState, Layer, MediaAsset } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; userMode: UserMode; }

const PhotoEditor: React.FC<Props> = ({ navigate, userMode }) => {
  const engine = KnouxEngine.getInstance();
  const fileRef = useRef<HTMLInputElement>(null);
  
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [layers, setLayers] = useState<Layer[]>([
    { id: '1', name: 'الخلفية الأصلية', visible: true, locked: true, opacity: 100, type: 'PIXEL', blendMode: 'normal' }
  ]);
  const [adjs, setAdjs] = useState<AdjustmentState>({
    brightness: 0, contrast: 0, exposure: 0, saturation: 0, temperature: 0, tint: 0, sharpness: 0
  });

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) {
      const newAsset = await engine.importLocalImage(e.target.files[0]);
      setAsset(newAsset);
    }
  };

  const updateAdj = (key: keyof AdjustmentState, val: number) => {
    setAdjs(prev => ({ ...prev, [key]: val }));
  };

  return (
    <div className="h-full w-full flex bg-[#07070B] text-white select-none pr-16 pl-16">
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-6 gap-6">
        <button onClick={() => fileRef.current?.click()} className="w-10 h-10 glass rounded-xl hover:text-[#9B59FF] transition-all">📂</button>
        <input type="file" ref={fileRef} onChange={handleImport} className="hidden" accept="image/*" />
        <div className="h-px w-8 bg-white/10"></div>
        <button className="text-xl opacity-40 hover:opacity-100">✥</button>
        <button className="text-xl opacity-40 hover:opacity-100">🖌️</button>
        <button className="text-xl text-[#9B59FF]">✨</button>
        <button className="text-xl opacity-40 hover:opacity-100 mt-auto">⚙️</button>
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-14 glass border-b border-white/5 flex items-center justify-between px-8">
          <div className="flex items-center gap-4">
            <button onClick={() => navigate(AppScreen.HOME)} className="text-xs font-bold text-[#8E8E93] hover:text-white">✕ خروج</button>
            <span className="text-[10px] bg-[#9B59FF]/20 text-[#9B59FF] px-2 py-0.5 rounded-full font-black">G2 OPTICS ACTIVE — {userMode}</span>
          </div>
          <button className="px-6 py-1.5 bg-[#9B59FF] rounded-lg text-xs font-bold shadow-lg shadow-[#9B59FF]/20">تصدير سيادي</button>
        </header>

        <div className="flex-1 bg-black/40 relative flex items-center justify-center p-10">
          {asset ? (
            <div className="relative shadow-2xl">
              <img 
                src={asset.thumbnail} 
                className="max-w-full max-h-full object-contain transition-all duration-300"
                style={{ 
                  filter: `brightness(${100 + adjs.brightness}%) contrast(${100 + adjs.contrast}%) saturate(${100 + adjs.saturation}%)` 
                }}
              />
              <div className="absolute inset-0 border border-white/10 pointer-events-none"></div>
            </div>
          ) : (
            <div onClick={() => fileRef.current?.click()} className="text-center cursor-pointer group">
               <div className="w-32 h-32 bg-white/5 rounded-full flex items-center justify-center text-5xl mx-auto mb-6 group-hover:scale-110 transition-all border border-dashed border-white/20">🖼️</div>
               <h2 className="text-2xl font-black opacity-20">بانتظار الإبداع... استورد من القرص F:</h2>
            </div>
          )}
          
          {asset && (
            <div className="absolute bottom-6 right-8 glass px-4 py-2 rounded-2xl text-[10px] text-[#8E8E93] flex gap-4">
               <span>ISO 100</span>
               <span>{asset.name}</span>
               <span className="text-green-500 font-bold">SECURE DRIVE F:</span>
            </div>
          )}
        </div>
      </main>

      <aside className="w-80 glass border-r border-white/5 flex flex-col">
        <div className="p-6 space-y-8 overflow-y-auto custom-scrollbar flex-1">
          <section className="space-y-6">
            <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">المعالجة البصرية</h3>
            {[
              { label: 'السطوع', key: 'brightness' },
              { label: 'التباين', key: 'contrast' },
              { label: 'التعريض', key: 'exposure' },
              { label: 'التشبع', key: 'saturation' }
            ].map((adj) => (
              <div key={adj.key} className="space-y-3">
                <div className="flex justify-between text-[11px] font-bold">
                  <span className="text-[#8E8E93]">{adj.label}</span>
                  <span className="text-[#9B59FF]">{(adjs as any)[adj.key]}</span>
                </div>
                <input 
                  type="range" min="-100" max="100" 
                  value={(adjs as any)[adj.key]} 
                  onChange={(e) => updateAdj(adj.key as any, parseInt(e.target.value))}
                  className="w-full h-1 bg-white/10 rounded-full appearance-none accent-[#9B59FF] cursor-pointer"
                />
              </div>
            ))}
          </section>

          <section className="pt-8 border-t border-white/5 space-y-4">
            <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">الذكاء الاصطناعي</h3>
            <button className="w-full py-3 glass rounded-xl text-xs font-bold hover:bg-[#9B59FF]/10 transition-all">✨ تحسين تلقائي (Local Inference)</button>
            <button className="w-full py-3 glass rounded-xl text-xs font-bold hover:bg-[#9B59FF]/10 transition-all">🩹 ترميم البشرة والوجوه</button>
          </section>
        </div>
      </aside>
    </div>
  );
};

export default PhotoEditor;
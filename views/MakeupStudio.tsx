import React, { useState, useRef } from 'react';
import { AppScreen, MediaAsset } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; }

const MakeupStudio: React.FC<Props> = ({ navigate }) => {
  const engine = KnouxEngine.getInstance();
  const fileRef = useRef<HTMLInputElement>(null);
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [intensity, setIntensity] = useState(60);
  const [activeColor, setActiveColor] = useState('#C0392B');

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) setAsset(await engine.importMedia(e.target.files[0]));
  };

  const makeupColors = [
    { name: 'كلاسيك', color: '#C0392B' },
    { name: 'وردي', color: '#E91E63' },
    { name: 'نيود', color: '#D2B48C' },
    { name: 'برونزي', color: '#CD7F32' },
    { name: 'بنفسجي', color: '#9B59FF' }
  ];

  return (
    <div className="h-full w-full flex bg-[#07070B] pr-16 pl-16">
      <aside className="w-16 glass border-l border-white/5 flex flex-col items-center py-8 gap-8">
        <button onClick={() => fileRef.current?.click()} className="text-2xl hover:text-[#9B59FF] transition-all">📂</button>
        <input type="file" ref={fileRef} onChange={handleImport} className="hidden" />
        <button className="text-2xl text-[#9B59FF]">💄</button>
        <button className="text-2xl opacity-40">👁️</button>
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-16 flex items-center justify-between px-10 border-b border-white/5">
          <h2 className="text-[10px] font-black tracking-[0.4em] text-[#9B59FF]">VIRTUAL MAKEUP STUDIO G2</h2>
          <button onClick={() => navigate(AppScreen.HOME)} className="text-[10px] font-bold opacity-40 hover:opacity-100 transition-opacity">✕ خروج</button>
        </header>
        <div className="flex-1 bg-black/40 flex items-center justify-center p-12 relative overflow-hidden">
          {asset ? (
            <div className="relative group">
               <img src={asset.thumbnail} className="max-w-full max-h-full rounded-2xl shadow-[0_0_80px_rgba(0,0,0,0.5)] transition-all duration-500" />
               <div className="absolute inset-0 border border-white/5 rounded-2xl pointer-events-none"></div>
            </div>
          ) : (
            <div onClick={() => fileRef.current?.click()} className="text-center opacity-10 select-none cursor-pointer">
               <div className="text-9xl mb-4">🎭</div>
               <p className="text-xl font-black uppercase tracking-widest">Import Face Data from Drive F:</p>
            </div>
          )}
        </div>
      </main>

      <aside className="w-80 glass border-r border-white/5 p-8 flex flex-col">
        <div className="flex-1 space-y-10 overflow-y-auto custom-scrollbar">
           <section className="space-y-6">
              <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-widest">لوحة ألوان الجمال</h3>
              <div className="grid grid-cols-3 gap-3">
                 {makeupColors.map(c => (
                   <button 
                    key={c.color} 
                    onClick={() => setActiveColor(c.color)}
                    className={`h-12 rounded-xl transition-all border-2 ${activeColor === c.color ? 'border-white scale-105' : 'border-transparent opacity-60'}`}
                    style={{ backgroundColor: c.color }}
                   />
                 ))}
              </div>
           </section>

           <section className="space-y-6">
              <div className="flex justify-between text-[11px] font-bold">
                 <span className="text-[#8E8E93]">كثافة التطبيق</span>
                 <span className="text-white font-mono">{intensity}%</span>
              </div>
              <input 
                type="range" min="0" max="100" value={intensity} 
                onChange={(e) => setIntensity(parseInt(e.target.value))}
                className="w-full h-1 bg-white/5 rounded-full appearance-none accent-[#9B59FF] cursor-pointer"
              />
           </section>

           <section className="pt-6 border-t border-white/5 space-y-4">
              <div className="p-5 glass rounded-2xl border-white/5 space-y-2">
                 <p className="text-[10px] font-black text-white">MakeupGAN-3.0 ACTIVE</p>
                 <p className="text-[9px] text-[#8E8E93]">المعالجة تتم محلياً لضمان السيادة الرقمية.</p>
              </div>
           </section>
        </div>
        
        <div className="pt-8 space-y-4">
           <button className="w-full py-5 bg-[#9B59FF] rounded-3xl text-white text-xs font-black shadow-xl shadow-[#9B59FF]/20 hover:scale-[1.02] active:scale-[0.98] transition-all">تطبيق المظهر السيادي</button>
           <button onClick={() => engine.secureSave(asset!)} disabled={!asset} className="w-full py-4 glass rounded-3xl text-[#8E8E93] text-[10px] font-bold hover:text-white transition-all disabled:opacity-20">تشفير وحفظ في الخزنة</button>
        </div>
      </aside>
    </div>
  );
};

export default MakeupStudio;

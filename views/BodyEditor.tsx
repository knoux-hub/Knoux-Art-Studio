import React, { useState, useRef, useEffect } from 'react';
import { AppScreen, UserMode, BodyAdjustParams, MediaAsset, BodyPart } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; userMode: UserMode; }

const BodyEditor: React.FC<Props> = ({ navigate, userMode }) => {
  const engine = KnouxEngine.getInstance();
  const fileRef = useRef<HTMLInputElement>(null);
  const canvasRef = useRef<HTMLDivElement>(null);
  
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [isExpanded, setIsExpanded] = useState(false);
  const [draggingId, setDraggingId] = useState<string | null>(null);
  const [isProcessing, setIsProcessing] = useState(false);

  const [params, setParams] = useState<BodyAdjustParams>({
    shoulderWidth: 1.0, chestWidth: 1.0, waistWidth: 1.0, hipWidth: 1.0, legLength: 1.0, muscleDefinition: 0
  });

  const [parts, setParts] = useState<BodyPart[]>([
    { id: 'shoulders', name: 'الكتفين', x: 40, y: 25, width: 20, height: 8 },
    { id: 'chest', name: 'الصدر', x: 42, y: 35, width: 16, height: 10 },
    { id: 'waist', name: 'الخصر', x: 43, y: 46, width: 14, height: 8 },
    { id: 'hips', name: 'الوركين', x: 40, y: 56, width: 20, height: 12 },
  ]);

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) {
      const newAsset = await engine.importLocalImage(e.target.files[0]);
      setAsset(newAsset);
    }
  };

  // Drag & Drop Logic
  const handleMouseMove = (e: React.MouseEvent) => {
    if (!draggingId || !canvasRef.current) return;
    
    const rect = canvasRef.current.getBoundingClientRect();
    const x = ((e.clientX - rect.left) / rect.width) * 100;
    const y = ((e.clientY - rect.top) / rect.height) * 100;

    setParts(prev => prev.map(p => 
      p.id === draggingId ? { ...p, x: x - p.width / 2, y: y - p.height / 2 } : p
    ));
  };

  const handleApply = async () => {
    setIsProcessing(true);
    // Simulate parallel GPU-accelerated processing
    await engine.parallelProcess(async () => {
       await new Promise(r => setTimeout(r, 200));
    }, 5);
    setIsProcessing(false);
    console.log("Neural Sculpt Applied Successfully");
  };

  const tools = [
    { id: 'detect', icon: '🔍', label: 'كشف تلقائي' },
    { id: 'mesh', icon: '🕸️', label: 'الشبكة العصبية' },
    { id: 'muscle', icon: '💪', label: 'نحت العضلات' },
    { id: 'height', icon: '📏', label: 'تعديل الطول' },
  ];

  return (
    <div className="h-full w-full flex bg-[#07070B] text-[#F8F8F8] pr-16 pl-16 overflow-hidden">
      {/* Expandable Sidebar */}
      <aside 
        className={`glass border-l border-white/5 flex flex-col py-8 transition-all duration-500 ease-in-out z-40 ${isExpanded ? 'w-48' : 'w-16'}`}
      >
        <button 
          onClick={() => setIsExpanded(!isExpanded)}
          className="w-full flex items-center justify-center h-10 hover:bg-white/5 transition-all text-xl"
          title={isExpanded ? "طي القائمة" : "توسيع القائمة"}
        >
          {isExpanded ? '◂' : '☰'}
        </button>
        
        <div className="h-px w-8 bg-white/10 mx-auto my-6"></div>

        <div className="flex-1 flex flex-col gap-4 overflow-x-hidden">
          <button onClick={() => fileRef.current?.click()} className="flex items-center gap-4 px-5 w-full h-12 hover:text-[#9B59FF] transition-all">
            <span className="text-xl shrink-0">📂</span>
            {isExpanded && <span className="text-xs font-black whitespace-nowrap animate-in fade-in slide-in-from-right-2">استيراد</span>}
          </button>
          
          {tools.map(tool => (
            <button key={tool.id} className="flex items-center gap-4 px-5 w-full h-12 hover:bg-white/5 transition-all rounded-lg group">
              <span className="text-xl shrink-0 group-hover:scale-110 transition-transform">{tool.icon}</span>
              {isExpanded && <span className="text-xs font-black whitespace-nowrap animate-in fade-in slide-in-from-right-2">{tool.label}</span>}
            </button>
          ))}
        </div>

        <input type="file" ref={fileRef} onChange={handleImport} className="hidden" />
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-16 flex items-center justify-between px-10 border-b border-white/5 bg-black/20">
          <div className="flex items-center gap-6">
            <h2 className="text-[10px] font-black tracking-[0.4em] text-[#9B59FF] uppercase">Neural Sculptor G2 — {userMode}</h2>
            {isProcessing && <div className="h-1 w-32 bg-white/5 rounded-full overflow-hidden">
              <div className="h-full bg-[#9B59FF] animate-[loading_2s_infinite]"></div>
            </div>}
          </div>
          <button onClick={() => navigate(AppScreen.HOME)} className="glass px-6 py-2 rounded-xl text-[10px] font-bold hover:bg-white/10 transition-all">✕ خروج</button>
        </header>

        <div 
          ref={canvasRef}
          onMouseMove={handleMouseMove}
          onMouseUp={() => setDraggingId(null)}
          className="flex-1 bg-[#09090D] flex items-center justify-center p-12 relative group"
        >
          {asset ? (
            <div className="relative inline-block select-none transition-transform duration-700">
              <img 
                src={asset.thumbnail} 
                draggable={false}
                className="max-w-full max-h-[70vh] rounded-lg shadow-[0_0_60px_rgba(0,0,0,0.8)] pointer-events-none" 
                style={{ 
                  filter: `contrast(${params.muscleDefinition > 0 ? 1.1 : 1.0})`,
                  transform: `scaleX(${params.waistWidth})` 
                }} 
              />
              
              {/* Interactive Neural Nodes */}
              {parts.map(part => (
                <div
                  key={part.id}
                  onMouseDown={(e) => { e.stopPropagation(); setDraggingId(part.id); }}
                  style={{ 
                    left: `${part.x}%`, 
                    top: `${part.y}%`, 
                    width: `${part.width}%`, 
                    height: `${part.height}%` 
                  }}
                  className={`absolute border border-dashed border-[#9B59FF]/30 cursor-grab active:cursor-grabbing hover:bg-[#9B59FF]/10 flex items-center justify-center transition-colors z-30 group/part ${draggingId === part.id ? 'bg-[#9B59FF]/20 border-[#9B59FF] cursor-grabbing' : ''}`}
                >
                  <span className="text-[8px] font-black text-[#9B59FF] uppercase opacity-0 group-hover/part:opacity-60 transition-opacity select-none">{part.name}</span>
                  {/* Anchor handles */}
                  <div className="absolute -top-1 -left-1 w-2 h-2 bg-[#9B59FF] rounded-full scale-0 group-hover/part:scale-100 transition-transform"></div>
                  <div className="absolute -bottom-1 -right-1 w-2 h-2 bg-[#9B59FF] rounded-full scale-0 group-hover/part:scale-100 transition-transform"></div>
                </div>
              ))}
            </div>
          ) : (
            <div onClick={() => fileRef.current?.click()} className="text-center cursor-pointer group opacity-10 hover:opacity-30 transition-opacity">
              <div className="text-8xl mb-4 group-hover:scale-110 transition-transform">🧬</div>
              <p className="text-xl font-black uppercase tracking-tighter">بانتظار البيانات السيادية</p>
            </div>
          )}
        </div>
      </main>

      {/* Inspector Panel */}
      <aside className="w-80 glass border-r border-white/5 p-8 space-y-10 overflow-y-auto custom-scrollbar">
        <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">تعديلات القوام العصبية</h3>
        
        <div className="space-y-8">
          {[
            { label: 'عرض الكتفين', key: 'shoulderWidth', min: 0.8, max: 1.2 },
            { label: 'نحت الخصر', key: 'waistWidth', min: 0.7, max: 1.3 },
            { label: 'عرض الوركين', key: 'hipWidth', min: 0.8, max: 1.2 },
            { label: 'طول الساقين', key: 'legLength', min: 1.0, max: 1.15 },
          ].map(adj => (
            <div key={adj.key} className="space-y-4">
               <div className="flex justify-between text-[11px] font-bold">
                 <span className="text-[#8E8E93]">{adj.label}</span>
                 <span className="text-white font-mono">{(params as any)[adj.key].toFixed(2)}</span>
               </div>
               <input 
                  type="range" min={adj.min} max={adj.max} step="0.01" 
                  value={(params as any)[adj.key]} 
                  onChange={e => setParams({...params, [adj.key]: parseFloat(e.target.value)})} 
                  className="w-full h-1 bg-white/5 rounded-full appearance-none accent-[#9B59FF] cursor-pointer" 
               />
            </div>
          ))}
        </div>

        <div className="pt-10 border-t border-white/5 space-y-4">
           <button 
             onClick={handleApply}
             disabled={isProcessing || !asset}
             className="w-full py-5 bg-gradient-to-r from-[#9B59FF] to-[#667eea] rounded-3xl text-[11px] font-black shadow-xl shadow-[#9B59FF]/20 hover:scale-[1.02] active:scale-[0.98] transition-all disabled:opacity-20 disabled:grayscale"
           >
             تطبيق المعالجة السيادية
           </button>
           <div className="flex items-center gap-3 p-4 glass rounded-2xl border-white/5">
             <div className="w-2 h-2 rounded-full bg-green-500 animate-pulse shadow-[0_0_10px_#22c55e]"></div>
             <p className="text-[9px] text-[#8E8E93] leading-relaxed uppercase tracking-widest font-black">GPU G2 Active</p>
           </div>
        </div>
      </aside>
    </div>
  );
};

export default BodyEditor;

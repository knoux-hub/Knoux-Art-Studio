import React, { useState, useRef, useEffect } from 'react';
import { AppScreen, UserMode, BodyAdjustParams, MediaAsset, BodyPart } from '../types';
import { KnouxEngine } from '../services/knoux_engine';

interface Props { navigate: (s: AppScreen) => void; userMode: UserMode; }

const BodyEditor: React.FC<Props> = ({ navigate }) => {
  const engine = KnouxEngine.getInstance();
  const fileRef = useRef<HTMLInputElement>(null);
  
  // Basic State
  const [asset, setAsset] = useState<MediaAsset | null>(null);
  const [isToolbarExpanded, setIsToolbarExpanded] = useState(false);
  
  // History State
  const [params, setParams] = useState<BodyAdjustParams>({
    shoulderWidth: 1.0, chestWidth: 1.0, waistWidth: 1.0, hipWidth: 1.0, legLength: 1.0, muscleDefinition: 0
  });
  const [history, setHistory] = useState<BodyAdjustParams[]>([]);
  const [historyIndex, setHistoryIndex] = useState(-1);

  // Interaction State
  const [detectedParts, setDetectedParts] = useState<BodyPart[]>([]);
  const [draggingPart, setDraggingPart] = useState<string | null>(null);
  const canvasRef = useRef<HTMLDivElement>(null);

  // Initialize History
  useEffect(() => {
    if (history.length === 0) {
      setHistory([params]);
      setHistoryIndex(0);
    }
  }, []);

  const addToHistory = (newParams: BodyAdjustParams) => {
    const newHistory = history.slice(0, historyIndex + 1);
    newHistory.push(newParams);
    setHistory(newHistory);
    setHistoryIndex(newHistory.length - 1);
  };

  const undo = () => {
    if (historyIndex > 0) {
      const prev = history[historyIndex - 1];
      setParams(prev);
      setHistoryIndex(historyIndex - 1);
    }
  };

  const redo = () => {
    if (historyIndex < history.length - 1) {
      const next = history[historyIndex + 1];
      setParams(next);
      setHistoryIndex(historyIndex + 1);
    }
  };

  const handleImport = async (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files?.[0]) {
      const newAsset = await engine.importMedia(e.target.files[0]);
      setAsset(newAsset);
      // Simulate detection of body parts on import
      setDetectedParts([
        { id: 'shoulders', name: 'الكتفين', x: 40, y: 25, width: 20, height: 10 },
        { id: 'waist', name: 'الخصر', x: 42, y: 45, width: 16, height: 10 },
        { id: 'hips', name: 'الوركين', x: 40, y: 55, width: 20, height: 12 },
      ]);
    }
  };

  const updateParam = (key: keyof BodyAdjustParams, val: number) => {
    const newParams = { ...params, [key]: val };
    setParams(newParams);
  };

  const finalizeParamChange = () => {
    addToHistory(params);
  };

  // Drag logic
  const handleMouseMove = (e: React.MouseEvent) => {
    if (!draggingPart || !canvasRef.current) return;
    
    const rect = canvasRef.current.getBoundingClientRect();
    const x = ((e.clientX - rect.left) / rect.width) * 100;
    const y = ((e.clientY - rect.top) / rect.height) * 100;

    setDetectedParts(prev => prev.map(p => 
      p.id === draggingPart ? { ...p, x: x - p.width / 2, y: y - p.height / 2 } : p
    ));
  };

  const tools = [
    { id: 'measure', icon: '📏', label: 'المقاييس' },
    { id: 'sculpt', icon: '👤', label: 'النحت الحر' },
    { id: 'muscle', icon: '💪', label: 'العضلات' },
    { id: 'pose', icon: '🏃', label: 'الوضعية' },
  ];

  return (
    <div className="h-full w-full flex bg-[#07070B] text-[#F8F8F8] pr-16 pl-16">
      {/* Expandable Sidebar */}
      <aside 
        className={`glass border-l border-white/5 flex flex-col py-8 gap-4 transition-all duration-500 ease-in-out ${isToolbarExpanded ? 'w-48 px-4' : 'w-16 items-center'}`}
      >
        <button 
          onClick={() => setIsToolbarExpanded(!isToolbarExpanded)}
          className="mb-4 text-xs opacity-50 hover:opacity-100 transition-all uppercase tracking-widest"
        >
          {isToolbarExpanded ? 'Collapse' : '☰'}
        </button>

        <button onClick={() => fileRef.current?.click()} className="flex items-center gap-4 w-full h-10 hover:text-[#9B59FF] transition-all group">
          <span className="text-xl">📂</span>
          {isToolbarExpanded && <span className="text-xs font-bold whitespace-nowrap overflow-hidden">استيراد من F:</span>}
        </button>
        
        <input type="file" ref={fileRef} onChange={handleImport} className="hidden" />
        
        <div className="h-px w-full bg-white/10 my-2"></div>

        {tools.map(tool => (
          <button key={tool.id} className="flex items-center gap-4 w-full h-10 transition-all hover:bg-white/5 rounded-xl px-2">
            <span className="text-xl">{tool.icon}</span>
            {isToolbarExpanded && <span className="text-xs font-bold whitespace-nowrap overflow-hidden">{tool.label}</span>}
          </button>
        ))}
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-16 flex items-center justify-between px-10 border-b border-white/5 bg-black/20">
          <div className="flex items-center gap-8">
            <h2 className="text-[10px] font-black tracking-[0.4em] text-[#9B59FF] uppercase">Neural Body Sculptor G2</h2>
            <div className="flex items-center gap-2">
              <button 
                onClick={undo} 
                disabled={historyIndex <= 0}
                className="w-8 h-8 glass rounded-lg flex items-center justify-center text-xs disabled:opacity-20 hover:bg-white/10"
              >
                ↩
              </button>
              <button 
                onClick={redo} 
                disabled={historyIndex >= history.length - 1}
                className="w-8 h-8 glass rounded-lg flex items-center justify-center text-xs disabled:opacity-20 hover:bg-white/10"
              >
                ↪
              </button>
            </div>
          </div>
          <button onClick={() => navigate(AppScreen.HOME)} className="glass px-6 py-2 rounded-xl text-[10px] font-bold hover:bg-white/5 transition-all">✕ خروج</button>
        </header>

        {/* Enhanced Canvas with Glow and Purple Theme */}
        <div 
          ref={canvasRef}
          onMouseMove={handleMouseMove}
          onMouseUp={() => setDraggingPart(null)}
          className="flex-1 bg-[#0F0A1A] flex items-center justify-center p-16 relative transition-all duration-500 hover:shadow-[inset_0_0_100px_rgba(155,89,255,0.15)] group"
        >
          {asset ? (
            <div className="relative inline-block select-none shadow-[0_0_40px_rgba(0,0,0,0.5)] group-hover:shadow-[0_0_50px_rgba(155,89,255,0.2)] transition-shadow duration-700">
              <img 
                src={asset.thumbnail} 
                draggable={false}
                className="max-w-full max-h-[70vh] rounded-sm transition-all duration-700 pointer-events-none" 
                style={{ transform: `scaleX(${params.waistWidth})` }} 
              />
              
              {/* Interactive Neural Nodes */}
              {detectedParts.map(part => (
                <div
                  key={part.id}
                  onMouseDown={() => setDraggingPart(part.id)}
                  style={{ 
                    left: `${part.x}%`, 
                    top: `${part.y}%`, 
                    width: `${part.width}%`, 
                    height: `${part.height}%` 
                  }}
                  className={`absolute border border-dashed border-[#9B59FF]/40 cursor-move hover:bg-[#9B59FF]/10 flex items-center justify-center transition-colors ${draggingPart === part.id ? 'bg-[#9B59FF]/20 border-[#9B59FF]' : ''}`}
                >
                  <span className="text-[8px] font-black text-[#9B59FF] uppercase opacity-40">{part.name}</span>
                </div>
              ))}
            </div>
          ) : (
            <div className="text-center animate-pulse">
              <div className="text-6xl mb-4 opacity-10">🧬</div>
              <div className="text-xl font-black opacity-10 uppercase tracking-tighter">Waiting for body data from F:</div>
            </div>
          )}
        </div>
      </main>

      <aside className="w-80 glass border-r border-white/5 p-8 space-y-10 overflow-y-auto custom-scrollbar">
        <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">تعديل القوام السيادي</h3>
        
        {[
          { label: 'عرض الكتفين', key: 'shoulderWidth', min: 0.8, max: 1.2 },
          { label: 'نحت الخصر', key: 'waistWidth', min: 0.7, max: 1.3 },
          { label: 'عرض الوركين', key: 'hipWidth', min: 0.8, max: 1.2 },
          { label: 'طول الساقين', key: 'legLength', min: 1.0, max: 1.15 },
        ].map(adj => (
          <div key={adj.key} className="space-y-4">
             <div className="flex justify-between text-[11px] font-bold">
               <span className="text-[#8E8E93]">{adj.label}</span>
               <span className="text-white font-mono">{params[adj.key as keyof BodyAdjustParams].toFixed(2)}</span>
             </div>
             <input 
                type="range" min={adj.min} max={adj.max} step="0.01" 
                value={params[adj.key as keyof BodyAdjustParams]} 
                onChange={e => updateParam(adj.key as any, parseFloat(e.target.value))} 
                onMouseUp={finalizeParamChange}
                className="w-full h-1 bg-white/5 rounded-full appearance-none accent-[#9B59FF] cursor-pointer" 
             />
          </div>
        ))}

        <div className="pt-10 border-t border-white/5 space-y-4">
           <button 
             className="w-full py-5 bg-gradient-to-r from-[#9B59FF] to-[#667eea] rounded-3xl text-[11px] font-black shadow-xl shadow-[#9B59FF]/20 hover:scale-[1.02] active:scale-[0.98] transition-all"
           >
             تطبيق المعالجة العصبية
           </button>
           <div className="flex items-center gap-3 p-4 glass rounded-2xl border-white/5">
             <div className="w-2 h-2 rounded-full bg-green-500 animate-pulse"></div>
             <p className="text-[9px] text-[#8E8E93] leading-relaxed">Sovereign Neural Engine Active (Drive F:)</p>
           </div>
        </div>
      </aside>
    </div>
  );
};

export default BodyEditor;

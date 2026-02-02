
import React, { useState, useEffect } from 'react';
import { AppScreen, UserMode, AIModel, PromptHistoryEntry } from '../types';
import { GoogleGenAI } from "@google/genai";

// Define the missing interface for component props
interface ElysianCanvasProps {
  navigate: (screen: AppScreen) => void;
  userMode: UserMode;
}

const MODELS: AIModel[] = [
  // Use gemini-2.5-flash-image for standard image tasks
  { id: 'gemini-2.5-flash-image', name: 'Knoux Vision G2', nameAr: 'نواة كنوس البصرية G2', description: 'توليد سريع وعالي الجودة للفنانين', type: 'IMAGE', local: false, premium: false },
  // Use imagen-4.0-generate-001 for high quality Imagen requests
  { id: 'imagen-4.0-generate-001', name: 'Imagen 4 Pro', nameAr: 'إيماجن 4 برو', description: 'دقة استثنائية وفهم عميق للغة الطبيعية', type: 'IMAGE', local: false, premium: true, cost: '$0.04' },
  { id: 'flux-schnell', name: 'Flux.1 Local', nameAr: 'فليكس 1 محلي', description: 'يعمل بالكامل على جهازك (يتطلب 12GB VRAM)', type: 'IMAGE', local: true, premium: false }
];

const ElysianCanvas: React.FC<ElysianCanvasProps> = ({ navigate, userMode }) => {
  const [prompt, setPrompt] = useState('');
  const [selectedModel, setSelectedModel] = useState<AIModel>(MODELS[0]);
  const [aspectRatio, setAspectRatio] = useState('1:1');
  const [isGenerating, setIsGenerating] = useState(false);
  const [generatedResult, setGeneratedResult] = useState<string | null>(null);
  const [status, setStatus] = useState('');
  const [history, setHistory] = useState<PromptHistoryEntry[]>([]);

  useEffect(() => {
    const saved = localStorage.getItem('elysian_history');
    if (saved) setHistory(JSON.parse(saved));
  }, []);

  const handleGenerate = async () => {
    if (!prompt.trim()) return;
    setIsGenerating(true);
    setStatus('جارٍ استدعاء الخوارزميات السيادية...');
    
    try {
      // Create a new GoogleGenAI instance right before making an API call to ensure it always uses the most up-to-date API key
      const ai = new GoogleGenAI({ apiKey: process.env.API_KEY });
      
      let imgUrl = '';
      
      // Use generateImages for Imagen models and generateContent for Gemini models
      if (selectedModel.id.startsWith('imagen')) {
        const response = await ai.models.generateImages({
          model: selectedModel.id,
          prompt: prompt,
          config: {
            numberOfImages: 1,
            aspectRatio: aspectRatio as any,
          },
        });
        
        if (response.generatedImages?.[0]?.image?.imageBytes) {
          imgUrl = `data:image/png;base64,${response.generatedImages[0].image.imageBytes}`;
        }
      } else {
        const response = await ai.models.generateContent({
          model: selectedModel.id,
          contents: { parts: [{ text: prompt }] },
          config: {
            imageConfig: {
              aspectRatio: aspectRatio as any
            }
          }
        });

        // The output response may contain both image and text parts; iterate to find the image part.
        if (response.candidates?.[0]?.content?.parts) {
          for (const part of response.candidates[0].content.parts) {
            if (part.inlineData) {
              const base64EncodeString: string = part.inlineData.data;
              imgUrl = `data:image/png;base64,${base64EncodeString}`;
              break;
            }
          }
        }
      }

      if (imgUrl) {
        setGeneratedResult(imgUrl);
        const newEntry: PromptHistoryEntry = {
          id: Date.now().toString(),
          timestamp: Date.now(),
          prompt,
          model: selectedModel.name,
          resultUrl: imgUrl
        };
        const updatedHistory = [newEntry, ...history].slice(0, 50);
        setHistory(updatedHistory);
        localStorage.setItem('elysian_history', JSON.stringify(updatedHistory));
        setStatus('اكتمل التجسيد بنجاح.');
      } else {
        setStatus('لم يتمكن المحرك من إنشاء مخرجات بصرية.');
      }
    } catch (error) {
      console.error("Elysian Error:", error);
      setStatus('خطأ في الاتصال بالمحرك العصبي.');
    } finally {
      setIsGenerating(false);
    }
  };

  return (
    <div className="h-full w-full flex flex-col bg-[#07070B] pr-16 pl-16 animate-in zoom-in-95 duration-500">
      {/* Top Header */}
      <header className="h-16 flex justify-between items-center px-10 border-b border-white/5 shrink-0 z-20">
        <div className="flex items-center gap-4">
          <div className="w-12 h-12 bg-gradient-to-br from-[#9B59FF] to-[#667eea] rounded-2xl flex items-center justify-center text-white shadow-[0_0_20px_#9B59FF44] animate-pulse">✨</div>
          <div>
            <h1 className="text-2xl font-bold text-white font-['Amiri'] tracking-wide">إليسيان كانفاس AI</h1>
            <p className="text-[10px] text-[#9B59FF] uppercase font-black tracking-[0.2em]">Neural Sovereignty Layer</p>
          </div>
        </div>
        <button onClick={() => navigate(AppScreen.HOME)} className="w-12 h-12 glass rounded-full flex items-center justify-center hover:bg-white/10 transition-all text-white font-bold">✕</button>
      </header>

      {/* Main Layout */}
      <div className="flex-1 flex min-h-0 overflow-hidden relative">
        {/* Left: Prompt History Sidebar */}
        <aside className="w-80 border-l border-white/5 flex flex-col min-h-0 bg-black/20 z-10">
           <div className="p-5 border-b border-white/5 flex justify-between items-center bg-white/5">
              <h3 className="text-[10px] font-black text-[#8E8E93] uppercase tracking-widest">سجل الإبداع</h3>
              <button onClick={() => setHistory([])} className="text-[10px] text-[#9B59FF] hover:underline">مسح السجل</button>
           </div>
           <div className="flex-1 overflow-y-auto p-5 space-y-6 custom-scrollbar">
              {history.length === 0 ? (
                <div className="h-full flex items-center justify-center text-center opacity-20 px-8">
                  <p className="text-xs">لا توجد أعمال سابقة. ابدأ بكتابة فكرتك الأولى لتجسيدها.</p>
                </div>
              ) : (
                history.map(entry => (
                  <div key={entry.id} onClick={() => setGeneratedResult(entry.resultUrl!)} className="group cursor-pointer space-y-3">
                    <div className="aspect-square glass rounded-2xl overflow-hidden border border-white/10 group-hover:border-[#9B59FF]/50 transition-all shadow-xl">
                       <img src={entry.resultUrl} alt="prev" className="w-full h-full object-cover opacity-60 group-hover:opacity-100 transition-all duration-500 scale-100 group-hover:scale-110" />
                    </div>
                    <p className="text-[10px] text-[#8E8E93] line-clamp-2 leading-relaxed font-medium">{entry.prompt}</p>
                  </div>
                ))
              )}
           </div>
        </aside>

        {/* Center: Generation Workspace */}
        <main className="flex-1 flex flex-col p-10 bg-black/40 min-w-0 z-0 relative">
           <div className="flex-1 flex items-center justify-center relative min-h-0">
             <div className="relative group w-full h-full flex items-center justify-center">
               <div className={`aspect-square max-w-[600px] max-h-[600px] w-full bg-[#0F0F14] border-2 ${isGenerating ? 'border-[#9B59FF] shadow-[0_0_50px_#9B59FF33]' : 'border-white/5'} rounded-[3rem] flex items-center justify-center overflow-hidden transition-all duration-1000 shadow-2xl relative`}>
                  {generatedResult ? (
                    <img src={generatedResult} alt="AI Result" className="w-full h-full object-contain animate-in fade-in zoom-in duration-1000" />
                  ) : (
                    <div className="text-center space-y-6 opacity-5 flex flex-col items-center select-none">
                       <span className="text-[10rem]">🔮</span>
                       <p className="text-2xl font-bold tracking-widest font-['Amiri'] uppercase">Waiting for Inspiration</p>
                    </div>
                  )}
                  
                  {isGenerating && (
                    <div className="absolute inset-0 bg-[#07070B]/90 backdrop-blur-md flex flex-col items-center justify-center gap-8">
                       <div className="relative">
                          <div className="w-24 h-24 border-4 border-[#9B59FF]/20 rounded-full animate-ping"></div>
                          <div className="absolute inset-0 w-24 h-24 border-t-4 border-[#9B59FF] rounded-full animate-spin"></div>
                       </div>
                       <div className="text-center space-y-2">
                          <p className="text-2xl font-bold text-white animate-pulse font-['Amiri']">جارٍ تجسيد الفكرة...</p>
                          <p className="text-[10px] text-[#9B59FF] uppercase tracking-[0.3em] font-black">{status}</p>
                       </div>
                    </div>
                  )}
               </div>
             </div>
           </div>

           {/* Quick Export Tools */}
           {generatedResult && !isGenerating && (
             <div className="mt-10 flex justify-center gap-6 animate-in slide-in-from-bottom-8 duration-700">
                <button className="px-10 py-3 glass rounded-2xl text-xs font-bold hover:bg-white/10 transition-all border-white/20">توليد مماثل</button>
                <button className="px-12 py-3 bg-white text-black rounded-2xl text-xs font-black shadow-2xl hover:scale-105 active:scale-95 transition-all">تحميل العمل الفني</button>
             </div>
           )}
        </main>

        {/* Right: Controls Sidebar */}
        <aside className="w-96 glass border-r border-white/5 flex flex-col min-h-0 z-20">
           <div className="p-8 space-y-10 overflow-y-auto custom-scrollbar">
              {/* Model Select */}
              <div className="space-y-4">
                 <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">المحرك العصبي النشط</h3>
                 <div className="space-y-3">
                    {MODELS.map(m => (
                      <div 
                        key={m.id} 
                        onClick={() => setSelectedModel(m)}
                        className={`p-5 rounded-[1.5rem] border cursor-pointer transition-all duration-300 ${selectedModel.id === m.id ? 'bg-[#9B59FF]/10 border-[#9B59FF] shadow-[0_10px_30px_#9B59FF22]' : 'bg-white/5 border-white/5 hover:border-white/20 hover:bg-white/10'}`}
                      >
                         <div className="flex justify-between items-center mb-2">
                            <span className="text-xs font-black text-white">{m.nameAr}</span>
                            {m.premium && <span className="text-[8px] px-2 py-0.5 bg-[#9B59FF] text-white font-black rounded-full shadow-lg shadow-[#9B59FF]/40 tracking-tighter">PREMIUM</span>}
                            {m.local && <span className="text-[8px] px-2 py-0.5 bg-green-500/20 text-green-400 font-black rounded-full border border-green-500/30">LOCAL</span>}
                         </div>
                         <p className="text-[10px] text-[#8E8E93] leading-relaxed font-medium">{m.description}</p>
                      </div>
                    ))}
                 </div>
              </div>

              {/* Prompt Input Area */}
              <div className="space-y-4">
                 <div className="flex justify-between items-center">
                    <h3 className="text-[10px] font-black text-[#9B59FF] uppercase tracking-[0.2em]">الوصف الإبداعي</h3>
                    <button className="text-[10px] text-[#8E8E93] hover:text-white transition-colors underline underline-offset-4">تحسين الوصف</button>
                 </div>
                 <textarea
                   value={prompt}
                   onChange={(e) => setPrompt(e.target.value)}
                   className="w-full h-48 bg-black/60 border border-white/10 rounded-2xl p-5 text-xs leading-relaxed focus:border-[#9B59FF] outline-none transition-all resize-none shadow-inner custom-scrollbar font-medium"
                   placeholder="صف العمل الفني الذي تحلم به بتفاصيل دقيقة جداً..."
                 />
              </div>

              {/* Advanced Settings */}
              <div className="space-y-6 pt-4">
                 <div className="flex justify-between items-center">
                    <span className="text-[10px] font-bold text-[#8E8E93]">نسبة الأبعاد</span>
                    <select 
                      value={aspectRatio} 
                      onChange={(e) => setAspectRatio(e.target.value)}
                      className="bg-[#16161D] border border-white/10 rounded-xl px-3 py-1.5 outline-none text-[11px] font-bold text-white shadow-lg"
                    >
                       <option value="1:1">1:1 مربع</option>
                       <option value="16:9">16:9 سينمائي</option>
                       <option value="9:16">9:16 طولي</option>
                       <option value="4:3">4:3 كلاسيكي</option>
                    </select>
                 </div>
                 
                 <div className="space-y-3">
                    <div className="flex justify-between items-center text-[10px] font-bold text-[#8E8E93]">
                       <span>الالتزام بالوصف (CFG)</span>
                       <span className="text-[#9B59FF] font-mono">7.5</span>
                    </div>
                    <div className="h-1.5 bg-white/5 rounded-full relative cursor-pointer">
                       <div className="absolute top-1/2 left-[60%] -translate-y-1/2 w-4 h-4 bg-white rounded-full shadow-[0_0_10px_rgba(255,255,255,0.5)]"></div>
                    </div>
                 </div>
              </div>

              <button 
                onClick={handleGenerate}
                disabled={isGenerating || !prompt.trim()}
                className="w-full py-5 bg-[#9B59FF] rounded-3xl text-white font-black text-sm shadow-2xl shadow-[#9B59FF]/40 hover:brightness-110 active:scale-[0.98] transition-all disabled:opacity-20 disabled:cursor-not-allowed group relative overflow-hidden"
              >
                <div className="absolute inset-0 bg-gradient-to-r from-white/20 to-transparent translate-x-[-100%] group-hover:translate-x-[100%] transition-transform duration-1000"></div>
                {isGenerating ? 'جارٍ العمل المحكم...' : 'تجسيد الرؤية 🎨'}
              </button>

              <p className="text-[9px] text-[#8E8E93] text-center leading-relaxed font-medium">كل المحتوى المولد ينطبق عليه بروتوكول السيادة الرقمية من كنوس. لا يتم تخزين أو تحليل بياناتك خارج هذا الجهاز.</p>
           </div>
        </aside>
      </div>
    </div>
  );
};

export default ElysianCanvas;

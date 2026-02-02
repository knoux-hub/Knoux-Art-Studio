
import React, { useState } from 'react';
import { AppScreen, UserMode, VideoTrack } from '../types';

interface VideoEditorProps {
  navigate: (screen: AppScreen) => void;
  userMode: UserMode;
}

const VideoEditor: React.FC<VideoEditorProps> = ({ navigate, userMode }) => {
  const [isPlaying, setIsPlaying] = useState(false);
  const [currentTime, setCurrentTime] = useState('00:00:24:05');
  const [tracks, setTracks] = useState<VideoTrack[]>([
    { id: 'v2', name: 'Overlay Text', type: 'TITLE', clips: [{ id: 't1', name: 'عنوان البداية', startTime: 10, duration: 40, color: '#F59E0B' }] },
    { id: 'v1', name: 'Main Video', type: 'VIDEO', clips: [{ id: 'c1', name: 'Clip_Shot_01', startTime: 0, duration: 150, color: '#14B8A6' }, { id: 'c2', name: 'Clip_Shot_02', startTime: 150, duration: 80, color: '#14B8A6' }] },
    { id: 'a1', name: 'Background Score', type: 'AUDIO', clips: [{ id: 's1', name: 'Ambient Track', startTime: 0, duration: 300, color: '#3B82F6' }] },
  ]);

  return (
    <div className="h-full w-full flex flex-col bg-[#07070B] pr-16 pl-16 animate-in slide-in-from-right duration-700">
      {/* Top Header */}
      <header className="h-16 glass border-b border-white/5 flex items-center justify-between px-8 shrink-0 z-30">
        <div className="flex gap-6 items-center">
          <button onClick={() => navigate(AppScreen.HOME)} className="w-10 h-10 flex items-center justify-center glass rounded-full hover:text-[#9B59FF] transition-all">✕</button>
          <div className="h-6 w-px bg-white/10"></div>
          <h1 className="text-sm font-bold text-white tracking-wide">Knoux Montage Studio — <span className="text-[#8E8E93]">v1.0 Sovereign</span></h1>
        </div>
        <div className="flex gap-4">
           <button className="px-5 py-2 glass rounded-xl text-[11px] font-bold hover:bg-white/5 transition-all">إعدادات المشروع</button>
           <button className="px-6 py-2 bg-[#9B59FF] rounded-xl text-[11px] font-bold shadow-xl shadow-[#9B59FF]/30 hover:brightness-110 active:scale-95 transition-all">رندر وتصدير MP4</button>
        </div>
      </header>

      {/* Upper Area: Media & Preview */}
      <div className="flex-1 flex min-h-0 overflow-hidden relative">
        {/* Media Bin */}
        <aside className="w-72 glass border-l border-white/5 flex flex-col min-h-0 z-20">
           <div className="p-5 border-b border-white/5 flex justify-between items-center bg-white/5">
              <span className="text-[10px] font-bold text-[#9B59FF] uppercase tracking-widest">مكتبة الوسائط</span>
              <button className="w-8 h-8 flex items-center justify-center glass rounded-lg hover:text-white">+</button>
           </div>
           <div className="flex-1 overflow-y-auto p-4 space-y-4 custom-scrollbar">
              {[1, 2, 3, 4, 5, 6].map(i => (
                <div key={i} className="group relative aspect-video bg-[#16161D] rounded-2xl border border-white/5 hover:border-[#9B59FF]/50 transition-all cursor-grab active:cursor-grabbing overflow-hidden shadow-lg">
                   <div className="absolute inset-0 flex items-center justify-center opacity-10 text-4xl">🎞️</div>
                   <div className="absolute bottom-0 left-0 right-0 p-3 bg-gradient-to-t from-black/90 to-transparent translate-y-2 group-hover:translate-y-0 transition-transform">
                      <p className="text-[10px] text-white truncate font-bold">SOVEREIGN_CLIP_00{i}.mp4</p>
                      <p className="text-[8px] text-[#8E8E93] mt-0.5">4K UHD • 60 FPS</p>
                   </div>
                </div>
              ))}
           </div>
        </aside>

        {/* Live Preview Monitor */}
        <main className="flex-1 flex flex-col bg-black/60 p-8 z-10 relative">
           <div className="flex-1 flex items-center justify-center relative">
              <div className="aspect-video w-full max-w-5xl bg-[#07070B] border border-white/10 shadow-[0_30px_60px_rgba(0,0,0,0.5)] flex flex-col overflow-hidden rounded-lg">
                 <div className="flex-1 flex items-center justify-center relative">
                    <div className="absolute inset-4 border border-dashed border-white/5 pointer-events-none opacity-40"></div>
                    <span className="text-white/5 text-8xl font-black italic tracking-tighter select-none">MONITOR G1</span>
                 </div>
                 
                 {/* On-screen Controls */}
                 <div className="h-16 glass border-t border-white/5 flex flex-col">
                    <div className="h-1.5 w-full bg-white/5 relative cursor-pointer">
                       <div className="absolute top-0 bottom-0 left-[24%] w-1.5 bg-[#9B59FF] shadow-[0_0_15px_#9B59FF]"></div>
                    </div>
                    <div className="flex-1 flex items-center justify-between px-10">
                       <div className="text-sm font-mono text-[#9B59FF] font-bold">{currentTime}</div>
                       <div className="flex items-center gap-10">
                          <button className="text-xl text-[#8E8E93] hover:text-white transition-colors">⏮</button>
                          <button 
                            onClick={() => setIsPlaying(!isPlaying)}
                            className="w-12 h-12 flex items-center justify-center bg-[#9B59FF]/10 text-[#9B59FF] rounded-full border border-[#9B59FF]/30 hover:scale-110 transition-all text-2xl shadow-inner"
                          >
                            {isPlaying ? '⏸' : '▶'}
                          </button>
                          <button className="text-xl text-[#8E8E93] hover:text-white transition-colors">⏭</button>
                       </div>
                       <div className="flex items-center gap-5 text-[10px] font-bold">
                          <span className="px-2 py-0.5 bg-white/5 rounded text-[#9B59FF]">1/2 RESOLUTION</span>
                          <button className="text-lg opacity-60 hover:opacity-100">🔊</button>
                       </div>
                    </div>
                 </div>
              </div>
           </div>
        </main>

        {/* Effects & Grading Panel */}
        <aside className="w-80 glass border-r border-white/5 flex flex-col min-h-0 z-20">
           <div className="p-5 border-b border-white/5 bg-white/5">
              <span className="text-[10px] font-bold text-[#9B59FF] uppercase tracking-widest">تأثيرات الفيديو و Color Grading</span>
           </div>
           <div className="flex-1 overflow-y-auto custom-scrollbar">
              {[
                { name: 'تصحيح الألوان المتقدم', icon: '🎨' },
                { name: 'تثبيت الصورة AI', icon: '🤖' },
                { name: 'إزالة الخلفية الذكية', icon: '✂' },
                { name: 'مؤثرات بصرية (VFX)', icon: '🌋' },
                { name: 'الانتقالات السينمائية', icon: '🎞️' },
              ].map(cat => (
                <div key={cat.name} className="p-5 border-b border-white/5 hover:bg-white/5 cursor-pointer group transition-all">
                   <div className="flex justify-between items-center">
                      <div className="flex items-center gap-3">
                         <span className="text-xl">{cat.icon}</span>
                         <span className="text-xs font-bold group-hover:text-[#9B59FF] transition-colors">{cat.name}</span>
                      </div>
                      <span className="text-[#8E8E93] group-hover:translate-x-[-4px] transition-transform">›</span>
                   </div>
                </div>
              ))}
           </div>
        </aside>
      </div>

      {/* Multi-Track Timeline */}
      <footer className="h-80 glass border-t border-white/5 flex flex-col shrink-0 z-30 shadow-[0_-20px_40px_rgba(0,0,0,0.3)]">
        <div className="h-10 bg-[#0F0F14] border-b border-white/5 flex items-center px-6 gap-8 select-none">
           <div className="flex gap-4">
             <button title="قص" className="text-[#8E8E93] hover:text-white transition-colors">✂</button>
             <button title="تحديد" className="text-[#9B59FF] transition-colors">✥</button>
             <button title="مغناطيس" className="text-[#8E8E93] hover:text-white transition-colors">🧲</button>
           </div>
           <div className="h-4 w-px bg-white/10"></div>
           <div className="flex-1 relative h-full flex items-center">
              <div className="flex gap-32 text-[9px] text-[#8E8E93] font-mono opacity-40">
                 {['00:00', '00:10', '00:20', '00:30', '00:40', '00:50', '01:00', '01:10'].map(s => <span key={s}>{s}</span>)}
              </div>
              {/* Playhead Marker */}
              <div className="absolute top-0 bottom-0 left-[24%] w-px bg-[#9B59FF] z-10 shadow-[0_0_10px_#9B59FF]">
                 <div className="absolute -top-1 -left-1 w-2.5 h-2.5 bg-[#9B59FF] rotate-45"></div>
              </div>
           </div>
        </div>

        <div className="flex-1 overflow-y-auto flex flex-col p-1.5 gap-1.5 custom-scrollbar bg-black/40">
           {tracks.map(track => (
             <div key={track.id} className="flex h-16 items-stretch gap-1.5 group">
                <div className="w-56 glass rounded-2xl flex flex-col justify-center px-5 border-l-4 border-l-[#9B59FF]/20 group-hover:border-l-[#9B59FF] transition-all bg-[#16161D]/50">
                   <span className="text-[11px] font-black text-white truncate uppercase tracking-tighter">{track.name}</span>
                   <div className="flex items-center gap-3 mt-1.5">
                      <button className="text-[10px] text-[#8E8E93] hover:text-white font-bold">M</button>
                      <button className="text-[10px] text-[#8E8E93] hover:text-white font-bold">S</button>
                      <div className="flex-1 h-1 bg-white/5 rounded-full overflow-hidden">
                         <div className="h-full bg-gradient-to-r from-green-500/50 to-green-400 w-[75%]"></div>
                      </div>
                   </div>
                </div>
                <div className="flex-1 glass rounded-2xl relative flex items-center px-1 overflow-hidden bg-white/5 shadow-inner">
                   {track.clips.map(clip => (
                     <div 
                      key={clip.id} 
                      className="h-[85%] rounded-xl border-t border-white/20 shadow-2xl flex items-center px-4 cursor-pointer hover:brightness-125 transition-all relative overflow-hidden group/clip"
                      style={{ 
                        backgroundColor: `${clip.color}22`, 
                        borderColor: `${clip.color}55`,
                        width: `${clip.duration * 3}px`,
                        left: `${clip.startTime * 3}px`
                      }}
                     >
                        <div className="absolute inset-0 bg-gradient-to-r from-white/5 to-transparent pointer-events-none"></div>
                        <span className="text-[10px] font-bold truncate text-white drop-shadow-md relative z-10">{clip.name}</span>
                        <div className="absolute right-0 top-0 bottom-0 w-1 bg-white/10 cursor-col-resize opacity-0 group-hover/clip:opacity-100 transition-opacity"></div>
                     </div>
                   ))}
                </div>
             </div>
           ))}
        </div>
      </footer>
    </div>
  );
};

export default VideoEditor;

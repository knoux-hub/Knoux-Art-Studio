import React from 'react';
import { AppScreen, UserMode } from '../types';

interface HomeProps {
  navigate: (screen: AppScreen) => void;
  userMode: UserMode;
}

const Home: React.FC<HomeProps> = ({ navigate, userMode }) => {
  const stats = [
    { label: 'المشاريع', value: '14', icon: '📁' },
    { label: 'عمليات AI', value: '1.2k', icon: '🧠' },
    { label: 'القرص F:', value: '820 GB', icon: '📀' },
  ];

  return (
    <div className="h-full w-full overflow-y-auto px-12 lg:px-24 py-12 space-y-16 custom-scrollbar animate-in fade-in duration-1000">
      {/* Sovereign Header */}
      <header className="flex flex-col md:flex-row justify-between items-start md:items-end gap-6">
        <div className="space-y-4">
          <div className="flex items-center gap-3">
             <span className="px-3 py-1 bg-[#9B59FF]/10 text-[#9B59FF] text-[10px] font-black rounded-full border border-[#9B59FF]/20 uppercase tracking-widest">Sovereign Core Active</span>
             <div className="h-2 w-2 rounded-full bg-green-500 animate-pulse shadow-[0_0_10px_#22c55e]"></div>
          </div>
          <h1 className="text-6xl font-bold text-white font-['Amiri'] tracking-tight">مختبر كنوس الفني</h1>
          <p className="text-[#8E8E93] text-xl max-w-2xl leading-relaxed">
            بيئة الإبداع السيادية. جميع المحركات (C++ و AI) تعمل محلياً على جهازك. 
            <span className="text-white"> لا بيانات تترك هذا النظام.</span>
          </p>
        </div>

        <div className="flex gap-4">
           {stats.map(s => (
             <div key={s.label} className="glass px-6 py-4 rounded-2xl flex flex-col items-center min-w-[120px] border-white/5">
                <span className="text-xl mb-1">{s.icon}</span>
                <span className="text-white font-bold text-lg">{s.value}</span>
                <span className="text-[10px] text-[#8E8E93] uppercase tracking-tighter">{s.label}</span>
             </div>
           ))}
        </div>
      </header>

      {/* Main Studio Modules */}
      <section className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
        <button onClick={() => navigate(AppScreen.PHOTO_EDITOR)} className="group h-72 glass rounded-[2.5rem] p-10 text-right transition-all hover:translate-y-[-8px] neon-border overflow-hidden flex flex-col justify-between">
          <div className="w-16 h-16 bg-[#9B59FF]/10 rounded-2xl flex items-center justify-center text-4xl group-hover:scale-110 transition-all duration-500">📷</div>
          <div>
            <h2 className="text-3xl font-bold text-white">تحرير الصور G2</h2>
            <p className="text-[#8E8E93] mt-3 text-sm">معالجة RAW وبكسل احترافية.</p>
          </div>
        </button>

        <button onClick={() => navigate(AppScreen.BODY_EDITOR)} className="group h-72 glass rounded-[2.5rem] p-10 text-right transition-all hover:translate-y-[-8px] border-blue-500/30 overflow-hidden flex flex-col justify-between">
          <div className="w-16 h-16 bg-blue-500/10 rounded-2xl flex items-center justify-center text-4xl group-hover:rotate-12 transition-all">👤</div>
          <div>
            <h2 className="text-3xl font-bold text-white">نحت القوام AI</h2>
            <p className="text-[#8E8E93] mt-3 text-sm">تعديل نسب الجسم بالذكاء العصبي.</p>
          </div>
        </button>

        <button onClick={() => navigate(AppScreen.FACE_RETOUCH)} className="group h-72 glass rounded-[2.5rem] p-10 text-right transition-all hover:translate-y-[-8px] border-pink-500/30 overflow-hidden flex flex-col justify-between">
          <div className="w-16 h-16 bg-pink-500/10 rounded-2xl flex items-center justify-center text-4xl group-hover:scale-90 transition-all">😊</div>
          <div>
            <h2 className="text-3xl font-bold text-white">ريتوش الوجه</h2>
            <p className="text-[#8E8E93] mt-3 text-sm">ترميم الوجوه وتجميلها ذكياً.</p>
          </div>
        </button>
      </section>

      {/* Vault Status */}
      <section className="pt-10">
         <div className="glass rounded-[3rem] p-12 border-white/5 bg-gradient-to-br from-[#121216] to-[#0B0B10] relative overflow-hidden">
            <div className="absolute top-0 left-0 w-full h-1 bg-gradient-to-r from-transparent via-[#9B59FF] to-transparent opacity-30"></div>
            <div className="flex flex-col lg:flex-row justify-between items-center gap-12">
               <div className="space-y-6 flex-1">
                  <h3 className="text-3xl font-bold text-white">بروتوكول الخزنة السيادية</h3>
                  <p className="text-[#8E8E93] text-lg leading-relaxed">
                     يتم الآن تشفير كافة مشاريعك باستخدام معيار AES-256 قبل تخزينها في القرص F:. 
                     النظام يعمل بوضع "العزلة التامة" لضمان عدم وصول أي جهة خارجية لعملك الفني.
                  </p>
                  <div className="flex gap-6">
                     <div className="flex items-center gap-2 text-green-400 text-sm font-bold">
                        <span>●</span> تشفير نشط
                     </div>
                     <div className="flex items-center gap-2 text-[#9B59FF] text-sm font-bold">
                        <span>●</span> معالجة GPU محلية
                     </div>
                  </div>
               </div>
               <div className="w-full lg:w-96 glass rounded-3xl p-8 border-[#9B59FF]/20">
                  <div className="flex justify-between items-center mb-6">
                     <span className="text-[10px] font-black uppercase tracking-widest text-[#8E8E93]">سجل الأمان</span>
                     <span className="text-[#9B59FF] text-xs font-bold font-mono">0 ERRORS</span>
                  </div>
                  <div className="space-y-4">
                     <div className="h-1 w-full bg-white/5 rounded-full overflow-hidden">
                        <div className="h-full bg-[#9B59FF] w-[100%] shadow-[0_0_10px_#9B59FF]"></div>
                     </div>
                     <p className="text-[10px] text-[#8E8E93] leading-relaxed">
                        تم فحص وتأمين 1.4 TB من البيانات الإبداعية على القرص F: بنجاح.
                     </p>
                  </div>
               </div>
            </div>
         </div>
      </section>
    </div>
  );
};

export default Home;


import React from 'react';

const SplashScreen: React.FC = () => {
  return (
    <div className="h-full w-full flex flex-col items-center justify-center bg-[#0B0B10]">
      <div className="relative group">
        <div className="absolute -inset-1 bg-[#9B59FF] rounded-full blur opacity-25 group-hover:opacity-100 transition duration-1000 group-hover:duration-200"></div>
        <div className="relative h-48 w-48 flex items-center justify-center bg-[#121216] rounded-full border border-white/5 shadow-2xl">
          <span className="text-7xl font-bold text-[#9B59FF]">K</span>
        </div>
      </div>
      <h1 className="mt-8 text-4xl font-bold tracking-tight text-white font-['Amiri']">Knoux Art Studio</h1>
      <p className="mt-4 text-[#A7A7A7] text-lg">الأمان ليس رفاهية، إنه حياة</p>
      
      <div className="mt-12 w-64 h-1 bg-white/5 rounded-full overflow-hidden">
        <div className="h-full bg-[#9B59FF] animate-[loading_2s_ease-in-out_infinite]"></div>
      </div>
      
      <style>{`
        @keyframes loading {
          0% { width: 0%; transform: translateX(-100%); }
          50% { width: 70%; transform: translateX(50%); }
          100% { width: 0%; transform: translateX(200%); }
        }
      `}</style>
    </div>
  );
};

export default SplashScreen;

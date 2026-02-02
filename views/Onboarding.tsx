
import React, { useState } from 'react';
import { UserMode } from '../types';

interface OnboardingProps {
  onComplete: (mode: UserMode) => void;
}

const Onboarding: React.FC<OnboardingProps> = ({ onComplete }) => {
  const [step, setStep] = useState(1);
  const [selectedMode, setSelectedMode] = useState<UserMode>(UserMode.BEGINNER);

  const modes = [
    { id: UserMode.BEGINNER, name: 'مبتدئ', desc: 'واجهة بسيطة، أدوات أساسية، مساعد ذكي دائم.', icon: '⭐' },
    { id: UserMode.CREATOR, name: 'صانع محتوى', desc: 'أدوات متوازنة، قوالب جاهزة للسوشيال ميديا.', icon: '✨' },
    { id: UserMode.PROFESSIONAL, name: 'محترف', desc: 'تحكم كامل، طبقات غير محدودة، أدوات دقيقة.', icon: '💎' },
  ];

  return (
    <div className="h-full w-full flex items-center justify-center p-6 bg-[#0B0B10]">
      <div className="max-w-4xl w-full glass rounded-2xl p-12 space-y-8 animate-in fade-in duration-700">
        {step === 1 && (
          <div className="text-center space-y-6">
            <h1 className="text-4xl font-bold text-white">مرحباً بك في Knoux Art Studio</h1>
            <p className="text-[#A7A7A7] text-xl max-w-2xl mx-auto leading-relaxed">
              Knoux هو بيئتك الآمنة للإبداع الرقمي. هنا، مشاريعك ملك لك وحدك، لا سحابة، لا تتبع، لا تنازل عن الخصوصية.
            </p>
            <button 
              onClick={() => setStep(2)}
              className="px-12 py-4 bg-[#9B59FF] text-white rounded-full text-xl font-bold hover:scale-105 transition-transform"
            >
              ابدأ الرحلة
            </button>
          </div>
        )}

        {step === 2 && (
          <div className="space-y-8">
            <div className="text-center">
              <h2 className="text-3xl font-bold text-white">اختر مستوى خبرتك</h2>
              <p className="text-[#A7A7A7] mt-2">سنقوم بتخصيص الواجهة لتناسب احتياجاتك</p>
            </div>
            
            <div className="grid md:grid-cols-3 gap-6">
              {modes.map(mode => (
                <div 
                  key={mode.id}
                  onClick={() => setSelectedMode(mode.id)}
                  className={`cursor-pointer p-6 rounded-xl border transition-all duration-300 ${selectedMode === mode.id ? 'border-[#9B59FF] bg-[#9B59FF]/10 ring-1 ring-[#9B59FF]' : 'border-white/5 bg-white/5 hover:border-white/20'}`}
                >
                  <div className="text-4xl mb-4">{mode.icon}</div>
                  <h3 className="text-xl font-bold text-white mb-2">{mode.name}</h3>
                  <p className="text-sm text-[#A7A7A7]">{mode.desc}</p>
                </div>
              ))}
            </div>

            <div className="flex justify-between items-center pt-8">
              <button onClick={() => setStep(1)} className="text-[#A7A7A7] hover:text-white">رجوع</button>
              <button 
                onClick={() => onComplete(selectedMode)}
                className="px-12 py-3 bg-[#9B59FF] text-white rounded-lg font-bold hover:opacity-90"
              >
                تأكيد وبدء العمل
              </button>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default Onboarding;

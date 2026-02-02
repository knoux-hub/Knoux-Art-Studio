
import React from 'react';
import { AppScreen } from '../types';
import { PROJECTS_LIST, OFFICIAL_LINKS } from '../constants';

interface SettingsProps {
  navigate: (screen: AppScreen) => void;
}

const Settings: React.FC<SettingsProps> = ({ navigate }) => {
  return (
    <div className="h-full w-full overflow-y-auto p-8 space-y-12 pr-20 pl-20 max-w-6xl mx-auto">
      <header className="flex justify-between items-center border-b border-white/5 pb-8">
        <h1 className="text-4xl font-bold text-white">إعدادات Knoux</h1>
        <button onClick={() => navigate(AppScreen.HOME)} className="px-6 py-2 glass hover:bg-white/5 rounded-lg text-sm">حفظ والعودة</button>
      </header>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-12">
        <div className="md:col-span-2 space-y-10">
          <section className="space-y-6">
            <h2 className="text-2xl font-bold text-[#9B59FF]">تخصيص الواجهة</h2>
            <div className="space-y-4 glass p-6 rounded-2xl">
              <div className="flex justify-between items-center">
                <span>لغة التطبيق</span>
                <select className="bg-[#0B0B10] border border-white/10 p-2 rounded text-sm">
                  <option>العربية (Arabic)</option>
                  <option>English</option>
                </select>
              </div>
              <div className="flex justify-between items-center">
                <span>السمة البصرية</span>
                <div className="flex gap-2">
                  <div className="w-6 h-6 rounded-full bg-black border-2 border-[#9B59FF]"></div>
                  <div className="w-6 h-6 rounded-full bg-white opacity-20"></div>
                </div>
              </div>
            </div>
          </section>

          <section className="space-y-6">
            <h2 className="text-2xl font-bold text-[#9B59FF]">الأمان والخصوصية</h2>
            <div className="glass p-6 rounded-2xl space-y-4">
              <div className="flex justify-between items-center">
                <div>
                  <h4 className="font-bold">تشفير المشاريع</h4>
                  <p className="text-xs text-[#A7A7A7]">تأمين ملفات .knxproj بكلمة مرور</p>
                </div>
                <input type="checkbox" className="w-10 h-5 bg-white/10 rounded-full appearance-none checked:bg-[#9B59FF] transition-all relative after:content-[''] after:absolute after:top-1 after:left-1 after:w-3 after:h-3 after:bg-white after:rounded-full checked:after:left-6 after:transition-all" />
              </div>
              <div className="flex justify-between items-center pt-4 border-t border-white/5">
                <div>
                  <h4 className="font-bold">المسح الآمن</h4>
                  <p className="text-xs text-[#A7A7A7]">حذف الملفات المؤقتة عند إغلاق التطبيق</p>
                </div>
                <input type="checkbox" defaultChecked className="w-10 h-5 bg-white/10 rounded-full appearance-none checked:bg-[#9B59FF] transition-all relative after:content-[''] after:absolute after:top-1 after:left-1 after:w-3 after:h-3 after:bg-white after:rounded-full checked:after:left-6 after:transition-all" />
              </div>
            </div>
          </section>

          <section className="space-y-6">
            <h2 className="text-2xl font-bold text-[#9B59FF]">عن Knoux</h2>
            <div className="glass p-6 rounded-2xl space-y-4 text-sm leading-relaxed">
               <p>Knoux Art Studio هو مشروع متفرد يهدف إلى إعادة السيادة الرقمية للفنانين. نحن نؤمن بأن أدوات الإبداع لا ينبغي أن تكون أدوات للمراقبة.</p>
               <div className="grid grid-cols-2 gap-4 pt-4">
                  {PROJECTS_LIST.map(p => (
                    <a key={p.name} href={p.url} target="_blank" className="p-3 glass rounded-lg hover:border-[#9B59FF]/40 transition-all">
                       <h5 className="font-bold text-[#9B59FF]">{p.name}</h5>
                       <p className="text-[10px] text-[#A7A7A7]">{p.desc}</p>
                    </a>
                  ))}
               </div>
            </div>
          </section>
        </div>

        <div className="md:col-span-1 space-y-8">
           <div className="glass rounded-2xl p-6 text-center space-y-4">
              <div className="w-24 h-24 bg-[#121216] rounded-full border border-[#9B59FF]/20 flex items-center justify-center text-4xl font-bold text-[#9B59FF] mx-auto shadow-2xl">K</div>
              <h3 className="text-xl font-bold">Knoux Art Studio</h3>
              <p className="text-xs text-[#A7A7A7]">الإصدار 1.0.0 (نواة سيادية)</p>
              <div className="flex justify-center gap-4 text-xl">
                 <a href={OFFICIAL_LINKS.TWITTER} target="_blank">𝕏</a>
                 <a href={OFFICIAL_LINKS.GITHUB} target="_blank">🐙</a>
                 <a href={OFFICIAL_LINKS.WHATSAPP} target="_blank">💬</a>
              </div>
           </div>

           <div className="glass rounded-2xl p-6 space-y-4">
              <h4 className="font-bold text-[#9B59FF]">تواصل معنا</h4>
              <ul className="text-xs space-y-3">
                 <li className="flex justify-between">
                    <span className="text-[#A7A7A7]">الموقع الرسمي:</span>
                    <a href={OFFICIAL_LINKS.WEBSITE} className="hover:text-[#9B59FF]">knoux.dev</a>
                 </li>
                 <li className="flex justify-between">
                    <span className="text-[#A7A7A7]">البريد:</span>
                    <span>{OFFICIAL_LINKS.EMAIL}</span>
                 </li>
              </ul>
           </div>
        </div>
      </div>
    </div>
  );
};

export default Settings;

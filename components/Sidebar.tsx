
import React from 'react';
import { AppScreen } from '../types';

interface SidebarProps {
  activeScreen: AppScreen;
  onNavigate: (screen: AppScreen) => void;
  isOpen: boolean;
  toggle: () => void;
  language: 'ar' | 'en';
  setLanguage: (lang: 'ar' | 'en') => void;
}

const Sidebar: React.FC<SidebarProps> = ({ activeScreen, onNavigate, isOpen, toggle, language, setLanguage }) => {
  const items = [
    { id: AppScreen.HOME, labelAr: 'الرئيسية', labelEn: 'Home', icon: '🏠' },
    { id: AppScreen.PHOTO_EDITOR, labelAr: 'الصور', labelEn: 'Photo', icon: '📷' },
    { id: AppScreen.VIDEO_EDITOR, labelAr: 'الفيديو', labelEn: 'Video', icon: '🎬' },
    { id: AppScreen.BODY_EDITOR, labelAr: 'الجسم', labelEn: 'Body', icon: '👤' },
    { id: AppScreen.FACE_RETOUCH, labelAr: 'الوجه', labelEn: 'Face', icon: '😊' },
    { id: AppScreen.MAKEUP_STUDIO, labelAr: 'مكياج', labelEn: 'Makeup', icon: '💄' },
    { id: AppScreen.ELYSIAN_CANVAS, labelAr: 'AI Canvas', labelEn: 'AI Canvas', icon: '✨' },
    { id: AppScreen.SETTINGS, labelAr: 'الإعدادات', labelEn: 'Settings', icon: '⚙' },
  ];

  return (
    <div className={`fixed top-0 bottom-0 z-50 transition-all duration-300 glass ${isOpen ? 'w-64' : 'w-16'} ${language === 'ar' ? 'right-0 border-l' : 'left-0 border-r'} border-white/5`}>
      <div className="flex flex-col h-full py-4">
        <button onClick={toggle} className="p-4 mb-4 text-xl hover:bg-white/5 transition-colors">
          {isOpen ? '✕' : '☰'}
        </button>

        <div className="flex-1 space-y-1 px-2 overflow-y-auto custom-scrollbar">
          {items.map(item => (
            <button
              key={item.id}
              onClick={() => onNavigate(item.id)}
              className={`w-full flex items-center p-3 rounded-lg transition-all duration-200 ${activeScreen === item.id ? 'bg-[#9B59FF] shadow-lg shadow-[#9B59FF]/20' : 'hover:bg-white/5'}`}
            >
              <span className="text-xl">{item.icon}</span>
              {isOpen && (
                <span className={`mx-4 font-medium whitespace-nowrap overflow-hidden text-xs`}>
                  {language === 'ar' ? item.labelAr : item.labelEn}
                </span>
              )}
            </button>
          ))}
        </div>

        <div className="px-2 pt-4 border-t border-white/5 space-y-2">
           <button 
             onClick={() => setLanguage(language === 'ar' ? 'en' : 'ar')}
             className="w-full flex items-center p-3 rounded-lg hover:bg-white/5 transition-all"
           >
             <span className="text-lg">🌐</span>
             {isOpen && <span className="mx-4 text-xs">{language === 'ar' ? 'English' : 'العربية'}</span>}
           </button>
        </div>
      </div>
    </div>
  );
};

export default Sidebar;

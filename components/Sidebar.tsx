
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
    <div className={`fixed top-0 bottom-0 z-50 transition-all duration-300 ease-out glass backdrop-blur-md ${isOpen ? 'w-64' : 'w-16'} ${language === 'ar' ? 'right-0 border-l' : 'left-0 border-r'} border-white/10 bg-black/20`}>
      <div className="flex flex-col h-full py-4">
        <button 
          onClick={toggle} 
          className="p-4 mb-4 text-xl hover:bg-white/10 transition-all duration-200 rounded-lg mx-2 group"
          title={isOpen ? 'Collapse' : 'Expand'}
        >
          <span className={`transform transition-transform duration-300 ${isOpen ? 'rotate-180' : 'rotate-0'}`}>
            {isOpen ? '✕' : '☰'}
          </span>
        </button>

        <div className="flex-1 space-y-1 px-2 overflow-y-auto custom-scrollbar">
          {items.map(item => (
            <button
              key={item.id}
              onClick={() => onNavigate(item.id)}
              className={`w-full flex items-center p-3 rounded-lg transition-all duration-200 group relative overflow-hidden ${
                activeScreen === item.id 
                  ? 'bg-gradient-to-r from-[#9B59FF] to-[#7C3AED] shadow-lg shadow-[#9B59FF]/30 text-white' 
                  : 'hover:bg-white/10 hover:shadow-md'
              }`}
              title={!isOpen ? (language === 'ar' ? item.labelAr : item.labelEn) : ''}
            >
              <span className="text-xl z-10 relative group-hover:scale-110 transition-transform duration-200">
                {item.icon}
              </span>
              <span className={`mx-4 font-medium whitespace-nowrap text-sm z-10 relative transition-all duration-300 ${
                isOpen ? 'opacity-100 translate-x-0' : 'opacity-0 -translate-x-4'
              }`}>
                {language === 'ar' ? item.labelAr : item.labelEn}
              </span>
              {activeScreen === item.id && (
                <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/5 to-transparent animate-pulse" />
              )}
            </button>
          ))}
        </div>

        <div className="px-2 pt-4 border-t border-white/10 space-y-2">
           <button 
             onClick={() => setLanguage(language === 'ar' ? 'en' : 'ar')}
             className="w-full flex items-center p-3 rounded-lg hover:bg-white/10 transition-all duration-200 group"
             title={!isOpen ? (language === 'ar' ? 'English' : 'العربية') : ''}
           >
             <span className="text-lg group-hover:scale-110 transition-transform duration-200">🌐</span>
             <span className={`mx-4 text-sm transition-all duration-300 ${
               isOpen ? 'opacity-100 translate-x-0' : 'opacity-0 -translate-x-4'
             }`}>
               {language === 'ar' ? 'English' : 'العربية'}
             </span>
           </button>
        </div>
      </div>
    </div>
  );
};

export default Sidebar;

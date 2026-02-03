import React, { useState, useEffect } from 'react';
import { AppScreen, UserMode } from './types';
import SplashScreen from './views/SplashScreen';
import Onboarding from './views/Onboarding';
import Home from './views/Home';
import PhotoEditor from './views/PhotoEditor';
import VideoEditor from './views/VideoEditor';
import ElysianCanvas from './views/ElysianCanvas';
import BodyEditor from './views/BodyEditor';
import FaceRetouch from './views/FaceRetouch';
import MakeupStudio from './views/MakeupStudio';
import Settings from './views/Settings';
import StudioDashboard from './views/StudioDashboard';
import TemplateGallery from './views/TemplateGallery';
import Sidebar from './components/Sidebar';
import { KnouxEngine } from './services/knoux_engine';

const App: React.FC = () => {
  const [currentScreen, setCurrentScreen] = useState<AppScreen>(AppScreen.SPLASH);
  const [userMode, setUserMode] = useState<UserMode>(UserMode.BEGINNER);
  const [isSidebarOpen, setSidebarOpen] = useState(false);
  const [language, setLanguage] = useState<'ar' | 'en'>('ar');

  useEffect(() => {
    if (currentScreen === AppScreen.SPLASH) {
      const timer = setTimeout(() => {
        const firstLaunch = localStorage.getItem('knoux_initialized') !== 'true';
        setCurrentScreen(firstLaunch ? AppScreen.ONBOARDING : AppScreen.HOME);
      }, 2500);
      return () => clearTimeout(timer);
    }
  }, [currentScreen]);

  const handleModeChange = (mode: UserMode) => {
    setUserMode(mode);
    KnouxEngine.getInstance().setMode(mode);
  };

  const renderScreen = () => {
    switch (currentScreen) {
      case AppScreen.SPLASH: return <SplashScreen />;
      case AppScreen.ONBOARDING: return <Onboarding onComplete={(mode) => {
          handleModeChange(mode);
          localStorage.setItem('knoux_initialized', 'true');
          setCurrentScreen(AppScreen.HOME);
        }} />;
      case AppScreen.HOME: return <Home navigate={setCurrentScreen} userMode={userMode} />;
      // Fix: Changed 'mode' to 'userMode' to match the component's interface definition
      case AppScreen.PHOTO_EDITOR: return <PhotoEditor navigate={setCurrentScreen} userMode={userMode} />;
      case AppScreen.VIDEO_EDITOR: return <VideoEditor navigate={setCurrentScreen} userMode={userMode} />;
      case AppScreen.ELYSIAN_CANVAS: return <ElysianCanvas navigate={setCurrentScreen} userMode={userMode} />;
      case AppScreen.BODY_EDITOR: return <BodyEditor navigate={setCurrentScreen} userMode={userMode} />;
      // Fix: Added missing 'userMode' prop required by FaceRetouch component
      case AppScreen.FACE_RETOUCH: return <FaceRetouch navigate={setCurrentScreen} userMode={userMode} />;
      case AppScreen.MAKEUP_STUDIO: return <MakeupStudio navigate={setCurrentScreen} />;
      case AppScreen.STUDIO_DASHBOARD: return <StudioDashboard />;
      case AppScreen.TEMPLATES: return <TemplateGallery />;
      case AppScreen.SETTINGS: return <Settings navigate={setCurrentScreen} />;
      default: return <Home navigate={setCurrentScreen} userMode={userMode} />;
    }
  };

  return (
    <div className={`h-screen w-screen flex flex-col bg-[#0B0B10] text-[#F8F8F8] select-none overflow-hidden ${language === 'ar' ? 'rtl' : 'ltr'}`}>
      {currentScreen !== AppScreen.SPLASH && currentScreen !== AppScreen.ONBOARDING && (
        <Sidebar 
          activeScreen={currentScreen} 
          onNavigate={setCurrentScreen} 
          isOpen={isSidebarOpen} 
          toggle={() => setSidebarOpen(!isSidebarOpen)}
          language={language}
          setLanguage={setLanguage}
        />
      )}
      <main className="flex-1 relative overflow-hidden transition-all duration-500">
        {renderScreen()}
      </main>
    </div>
  );
};

export default App;

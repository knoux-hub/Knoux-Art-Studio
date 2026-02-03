import React, { useEffect, useMemo, useState } from 'react';
import PhotoEditor from './PhotoEditor';
import ElysianCanvas from './ElysianCanvas';
import VideoStudio from './VideoStudio';
import MakeupStudio from './MakeupStudio';
import BodyEditor from './BodyEditor';
import TemplateGallery from './TemplateGallery';
import MetadataPanel from '../components/MetadataPanel';
import LivePreview from '../components/LivePreview';
import { KnouxSonar } from '../utils/knouxSonar';
import { UserMode } from '../types';
import '../styles/StudioDashboard.css';

type TabId = 'photo' | 'video' | 'body' | 'makeup' | 'templates';
type SplitView = 'horizontal' | 'vertical';

const StudioDashboard: React.FC = () => {
  const [activeTab, setActiveTab] = useState<TabId>('photo');
  const [projectStatus, setProjectStatus] = useState<{ completionPercentage: number } | null>(null);
  const [showLivePreview, setShowLivePreview] = useState(true);
  const [splitView, setSplitView] = useState<SplitView>('horizontal');

  useEffect(() => {
    KnouxSonar.scanProject().then(setProjectStatus);
  }, []);

  const tabs = useMemo(
    () => [
      { id: 'photo', label: '🎨 Photo Editor', icon: '🖼️' },
      { id: 'video', label: '🎬 Video Studio', icon: '📹' },
      { id: 'body', label: '💪 Body Editor', icon: '👤' },
      { id: 'makeup', label: '💄 Makeup Studio', icon: '✨' },
      { id: 'templates', label: '📚 Templates', icon: '🗂️' },
    ],
    []
  );

  const renderActiveTab = () => {
    const navigate = () => {};
    const userMode = UserMode.BEGINNER;
    switch (activeTab) {
      case 'photo':
        return <PhotoEditor navigate={navigate} userMode={userMode} />;
      case 'video':
        return <VideoStudio />;
      case 'body':
        return <BodyEditor navigate={navigate} userMode={userMode} />;
      case 'makeup':
        return <MakeupStudio navigate={navigate} />;
      case 'templates':
        return <TemplateGallery />;
      default:
        return <PhotoEditor navigate={navigate} userMode={userMode} />;
    }
  };

  const memoryUsage = useMemo(() => {
    const memory = (performance as unknown as { memory?: { usedJSHeapSize?: number } }).memory;
    if (!memory?.usedJSHeapSize) return 'N/A';
    return `${(memory.usedJSHeapSize / 1024 / 1024).toFixed(1)} MB`;
  }, []);

  return (
    <div className="studio-dashboard">
      <header className="dashboard-header">
        <h1>🎬 KNOUX Full Media Studio</h1>
        <div className="header-controls">
          <button onClick={() => setShowLivePreview((prev) => !prev)}>
            {showLivePreview ? 'Hide Preview' : 'Show Preview'}
          </button>
          <button
            onClick={() =>
              setSplitView((prev) => (prev === 'horizontal' ? 'vertical' : 'horizontal'))
            }
          >
            {splitView === 'horizontal' ? 'Vertical Split' : 'Horizontal Split'}
          </button>
          {projectStatus && (
            <div className="sonar-badge">🟢 {projectStatus.completionPercentage}% Complete</div>
          )}
        </div>
      </header>

      <nav className="dashboard-tabs">
        {tabs.map((tab) => (
          <button
            key={tab.id}
            className={`tab-button ${activeTab === tab.id ? 'active' : ''}`}
            onClick={() => setActiveTab(tab.id)}
          >
            <span className="tab-icon">{tab.icon}</span>
            {tab.label}
          </button>
        ))}
      </nav>

      <div className={`dashboard-content split-${splitView}`}>
        <div className="editor-panel">{renderActiveTab()}</div>

        {showLivePreview && (
          <div className="preview-panel">
            <LivePreview />
            <MetadataPanel />
          </div>
        )}
      </div>

      <footer className="dashboard-footer">
        <div className="status-items">
          <div className="status-item">
            <span className="status-label">Mode:</span>
            <span className="status-value">🟢 Online (Mocked)</span>
          </div>
          <div className="status-item">
            <span className="status-label">Memory:</span>
            <span className="status-value">{memoryUsage}</span>
          </div>
          <div className="status-item">
            <span className="status-label">Render:</span>
            <span className="status-value">60 FPS</span>
          </div>
          <div className="status-item">
            <span className="status-label">Undo Stack:</span>
            <span className="status-value">24 actions</span>
          </div>
        </div>
        <div className="quick-actions">
          <button title="Undo (Ctrl+Z)">↩️ Undo</button>
          <button title="Redo (Ctrl+Y)">↪️ Redo</button>
          <button title="Save (Ctrl+S)">💾 Save</button>
          <button title="Export (Ctrl+E)">📤 Export</button>
        </div>
      </footer>
    </div>
  );
};

export default StudioDashboard;

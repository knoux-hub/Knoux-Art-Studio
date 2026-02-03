import React, { useState } from 'react';

type MakeupSettings = {
  lips: { color: string; opacity: number; size: number };
  eyeshadow: { color: string; intensity: number; blend: number };
  blush: { color: string; opacity: number; position: 'apple' | 'cheek' };
  contour: { intensity: number; areas: string[] };
  eyeliner: { thickness: number; wing: boolean; color: string };
  highlighter: { intensity: number; areas: string[] };
};

const MakeupStudio: React.FC = () => {
  const [makeupSettings, setMakeupSettings] = useState<MakeupSettings>({
    lips: { color: '#D44F6A', opacity: 50, size: 0 },
    eyeshadow: { color: '#4A4A4A', intensity: 30, blend: 50 },
    blush: { color: '#FFB6C1', opacity: 40, position: 'apple' },
    contour: { intensity: 25, areas: ['cheekbones', 'jawline', 'nose'] },
    eyeliner: { thickness: 2, wing: false, color: '#000000' },
    highlighter: { intensity: 35, areas: ['cheekbones', 'nose', 'browbone'] },
  });

  const makeupPresets: Array<{ name: string; settings: Partial<MakeupSettings> }> = [
    {
      name: 'Natural',
      settings: {
        lips: { color: '#D44F6A', opacity: 30, size: 0 },
        eyeshadow: { color: '#4A4A4A', intensity: 20, blend: 50 },
        blush: { color: '#FFB6C1', opacity: 25, position: 'apple' },
        contour: { intensity: 15, areas: ['cheekbones'] },
      },
    },
    {
      name: 'Glam',
      settings: {
        lips: { color: '#C2185B', opacity: 80, size: 5 },
        eyeshadow: { color: '#9C27B0', intensity: 70, blend: 30 },
        blush: { color: '#FF4081', opacity: 60, position: 'cheek' },
        contour: { intensity: 40, areas: ['cheekbones', 'jawline', 'nose', 'forehead'] },
      },
    },
    {
      name: 'Editorial',
      settings: {
        lips: { color: '#000000', opacity: 90, size: 0 },
        eyeshadow: { color: '#2C387E', intensity: 85, blend: 20 },
        blush: { color: '#F50057', opacity: 70, position: 'apple' },
        contour: { intensity: 50, areas: ['cheekbones', 'jawline'] },
      },
    },
  ];

  const applyPreset = (preset: (typeof makeupPresets)[number]) => {
    setMakeupSettings((prev) => ({
      ...prev,
      ...preset.settings,
    }));
  };

  return (
    <div className="makeup-studio">
      <h2>💄 Makeup Studio</h2>

      <div className="makeup-container">
        <div className="face-canvas">
          <div className="face-outline">
            <div className="face-feature eyes">
              <div className="eye left" />
              <div className="eye right" />
            </div>
            <div className="face-feature nose" />
            <div className="face-feature lips" />
            <div className="face-feature cheeks">
              <div className="cheek left" />
              <div className="cheek right" />
            </div>
          </div>

          <div
            className="makeup-overlay lips-overlay"
            style={{
              backgroundColor: makeupSettings.lips.color,
              opacity: makeupSettings.lips.opacity / 100,
              transform: `scale(${1 + makeupSettings.lips.size / 100})`,
            }}
          />

          <div
            className="makeup-overlay eyeshadow-overlay"
            style={{
              backgroundColor: makeupSettings.eyeshadow.color,
              opacity: makeupSettings.eyeshadow.intensity / 100,
            }}
          />

          <div
            className="makeup-overlay blush-overlay"
            style={{
              backgroundColor: makeupSettings.blush.color,
              opacity: makeupSettings.blush.opacity / 100,
            }}
          />
        </div>

        <div className="makeup-controls">
          <div className="presets-section">
            <h4>📚 Presets</h4>
            <div className="preset-buttons">
              {makeupPresets.map((preset) => (
                <button
                  key={preset.name}
                  className="preset-button"
                  onClick={() => applyPreset(preset)}
                >
                  {preset.name}
                </button>
              ))}
            </div>
          </div>

          {Object.entries(makeupSettings).map(([feature, settings]) => (
            <div key={feature} className="feature-controls">
              <h5>{feature.charAt(0).toUpperCase() + feature.slice(1)}</h5>

              {typeof settings === 'object' && 'color' in settings && (
                <div className="control-group">
                  <label>Color</label>
                  <input
                    type="color"
                    value={settings.color}
                    onChange={(event) =>
                      setMakeupSettings({
                        ...makeupSettings,
                        [feature]: { ...settings, color: event.target.value },
                      })
                    }
                  />
                </div>
              )}

              {typeof settings === 'object' && 'opacity' in settings && (
                <div className="control-group">
                  <label>Opacity</label>
                  <input
                    type="range"
                    min="0"
                    max="100"
                    value={settings.opacity}
                    onChange={(event) =>
                      setMakeupSettings({
                        ...makeupSettings,
                        [feature]: { ...settings, opacity: parseInt(event.target.value, 10) },
                      })
                    }
                  />
                  <span>{settings.opacity}%</span>
                </div>
              )}

              {typeof settings === 'object' && 'intensity' in settings && (
                <div className="control-group">
                  <label>Intensity</label>
                  <input
                    type="range"
                    min="0"
                    max="100"
                    value={settings.intensity}
                    onChange={(event) =>
                      setMakeupSettings({
                        ...makeupSettings,
                        [feature]: { ...settings, intensity: parseInt(event.target.value, 10) },
                      })
                    }
                  />
                  <span>{settings.intensity}%</span>
                </div>
              )}
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default MakeupStudio;

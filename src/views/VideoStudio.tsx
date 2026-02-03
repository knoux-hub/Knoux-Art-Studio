import React, { useRef, useState } from 'react';
import { VideoProcessor } from '../services/videoProcessor';

const VideoStudio: React.FC = () => {
  const [videoFile, setVideoFile] = useState<File | null>(null);
  const [videoUrl, setVideoUrl] = useState<string | null>(null);
  const [isProcessing, setIsProcessing] = useState(false);
  const [progress, setProgress] = useState(0);
  const [effects, setEffects] = useState({
    brightness: 0,
    contrast: 0,
    saturation: 0,
    speed: 1.0,
    stabilization: 0,
    filters: [] as string[],
  });

  const videoRef = useRef<HTMLVideoElement>(null);
  const processor = new VideoProcessor();

  const handleFileUpload = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (file) {
      setVideoFile(file);
      const url = URL.createObjectURL(file);
      setVideoUrl(url);
    }
  };

  const applyEffects = async () => {
    if (!videoUrl) return;

    setIsProcessing(true);
    setProgress(0);

    const interval = setInterval(() => {
      setProgress((prev) => {
        if (prev >= 100) {
          clearInterval(interval);
          setIsProcessing(false);
          return 100;
        }
        return prev + 10;
      });
    }, 200);

    try {
      const processed = await processor.processVideo(videoUrl, effects);
      setVideoUrl(processed.url);
    } catch (error) {
      console.error('Video processing error:', error);
    } finally {
      setIsProcessing(false);
    }
  };

  const videoFilters = [
    { id: 'cinematic', name: 'Cinematic', values: { brightness: -5, contrast: 15, saturation: -10 } },
    { id: 'vintage', name: 'Vintage', values: { brightness: 8, saturation: -20, sepia: 40 } },
    { id: 'dramatic', name: 'Dramatic', values: { contrast: 25, saturation: -5, vignette: 30 } },
    { id: 'vibrant', name: 'Vibrant', values: { saturation: 30, brightness: 5, contrast: 10 } },
  ];

  return (
    <div className="video-studio">
      <h2>🎬 Video Studio</h2>

      <div className="upload-section">
        <input type="file" accept="video/*" onChange={handleFileUpload} className="file-input" />
        <div className="video-info">
          {videoFile && (
            <p>
              📹 {videoFile.name} • {(videoFile.size / 1024 / 1024).toFixed(2)} MB
            </p>
          )}
        </div>
      </div>

      <div className="video-content">
        <div className="video-player-container">
          {videoUrl ? (
            <>
              <video ref={videoRef} src={videoUrl} controls className="video-player" />
              {isProcessing && (
                <div className="processing-overlay">
                  <div className="progress-bar">
                    <div className="progress-fill" style={{ width: `${progress}%` }} />
                  </div>
                  <p>Processing... {progress}%</p>
                </div>
              )}
            </>
          ) : (
            <div className="video-placeholder">
              <p>📹 Upload a video to get started</p>
            </div>
          )}
        </div>

        <div className="video-controls">
          <div className="control-group">
            <h4>🎨 Color Adjustments</h4>
            {(['brightness', 'contrast', 'saturation'] as const).map((control) => (
              <div key={control} className="slider-control">
                <label>{control.charAt(0).toUpperCase() + control.slice(1)}</label>
                <input
                  type="range"
                  min={-100}
                  max={100}
                  value={effects[control]}
                  onChange={(event) =>
                    setEffects({
                      ...effects,
                      [control]: parseInt(event.target.value, 10),
                    })
                  }
                />
                <span>{effects[control]}</span>
              </div>
            ))}
          </div>

          <div className="control-group">
            <h4>⚡ Speed & Stabilization</h4>
            <div className="slider-control">
              <label>Playback Speed</label>
              <input
                type="range"
                min={0.25}
                max={4}
                step={0.25}
                value={effects.speed}
                onChange={(event) => setEffects({ ...effects, speed: parseFloat(event.target.value) })}
              />
              <span>{effects.speed.toFixed(2)}x</span>
            </div>

            <div className="slider-control">
              <label>Stabilization</label>
              <input
                type="range"
                min={0}
                max={100}
                value={effects.stabilization}
                onChange={(event) =>
                  setEffects({ ...effects, stabilization: parseInt(event.target.value, 10) })
                }
              />
              <span>{effects.stabilization}%</span>
            </div>
          </div>

          <div className="control-group">
            <h4>🎞️ Quick Filters</h4>
            <div className="filter-buttons">
              {videoFilters.map((filter) => (
                <button
                  key={filter.id}
                  className="filter-button"
                  onClick={() => setEffects({ ...effects, ...filter.values })}
                >
                  {filter.name}
                </button>
              ))}
            </div>
          </div>

          <div className="action-buttons">
            <button onClick={applyEffects} disabled={!videoUrl || isProcessing} className="apply-button">
              {isProcessing ? 'Processing...' : 'Apply Effects'}
            </button>

            <button className="export-button">📤 Export Video</button>
          </div>
        </div>
      </div>

      <div className="video-timeline">
        <div className="timeline-track">
          <div className="timeline-indicator" />
          {Array.from({ length: 10 }).map((_, i) => (
            <div key={i} className="timeline-marker">
              {i + 1}s
            </div>
          ))}
        </div>
        <div className="timeline-controls">
          <button>⏪</button>
          <button>⏸️</button>
          <button>⏩</button>
          <button>✂️ Split</button>
          <button>🔊 Audio</button>
        </div>
      </div>
    </div>
  );
};

export default VideoStudio;

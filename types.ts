export enum UserMode {
  BEGINNER = 'BEGINNER',
  CREATOR = 'CREATOR',
  PROFESSIONAL = 'PROFESSIONAL',
  POWER_USER = 'POWER_USER',
  SECURE = 'SECURE'
}

export enum AppScreen {
  SPLASH = 'SPLASH',
  ONBOARDING = 'ONBOARDING',
  HOME = 'HOME',
  PHOTO_EDITOR = 'PHOTO_EDITOR',
  VIDEO_EDITOR = 'VIDEO_EDITOR',
  ELYSIAN_CANVAS = 'ELYSIAN_CANVAS',
  BODY_EDITOR = 'BODY_EDITOR',
  FACE_RETOUCH = 'FACE_RETOUCH',
  MAKEUP_STUDIO = 'MAKEUP_STUDIO',
  SETTINGS = 'SETTINGS'
}

export interface AdjustmentState {
  brightness: number;
  contrast: number;
  exposure: number;
  saturation: number;
  temperature: number;
  tint: number;
  sharpness: number;
}

export interface BodyAdjustParams {
  shoulderWidth: number;
  chestWidth: number;
  waistWidth: number;
  hipWidth: number;
  legLength: number;
  muscleDefinition: number;
}

export interface BodyPart {
  id: string;
  name: string;
  x: number;
  y: number;
  width: number;
  height: number;
}

export interface FaceAdjustParams {
  smoothness: number;
  removeBlemishes: number;
  eyeSize: number;
  lipThickness: number;
  faceWidth: number;
  jawWidth: number;
}

export interface Layer {
  id: string;
  name: string;
  visible: boolean;
  locked: boolean;
  opacity: number;
  type: 'PIXEL' | 'ADJUSTMENT' | 'TEXT';
  blendMode: string;
}

export interface MediaAsset {
  id: string;
  path: string;
  name: string;
  type: 'IMAGE' | 'VIDEO';
  size: number;
  resolution: string;
  thumbnail: string;
  // Added metadata property to match implementation in KnouxEngine and prevent type errors
  metadata?: {
    size: string;
    dimensions: string;
    type: string;
  };
}

export interface AIModel {
  id: string;
  name: string;
  nameAr: string;
  description: string;
  type: 'IMAGE' | 'VIDEO';
  local: boolean;
  premium: boolean;
  cost?: string;
}

export interface PromptHistoryEntry {
  id: string;
  timestamp: number;
  prompt: string;
  model: string;
  resultUrl: string;
}

export interface VideoClip {
  id: string;
  name: string;
  startTime: number;
  duration: number;
  color: string;
}

export interface VideoTrack {
  id: string;
  name: string;
  type: 'VIDEO' | 'AUDIO' | 'TITLE';
  clips: VideoClip[];
}
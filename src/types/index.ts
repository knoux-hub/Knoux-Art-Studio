export enum AppScreen { SPLASH, ONBOARDING, HOME, PHOTO_EDITOR, VIDEO_EDITOR, ELYSIAN_CANVAS, SETTINGS }
export enum UserMode { BEGINNER, CREATOR, PROFESSIONAL, POWER_USER, SECURE }

export interface AdjustmentState {
  brightness: number;
  contrast: number;
  exposure: number;
  saturation: number;
  temperature: number;
  tint: number;
  sharpness: number;
}

export interface Layer {
  id: string;
  name: string;
  visible: boolean;
  locked: boolean;
  opacity: number;
  image?: string;
}

export interface MediaAsset {
  id: string;
  path: string;
  name: string;
  thumbnail: string;
  metadata: {
    size: string;
    dimensions: string;
    type: string;
  }
<<<<<<< HEAD
}
=======
}
>>>>>>> a71b9f8 (Initial Deployment: Knoux Art Studio Sovereign A-to-Z)

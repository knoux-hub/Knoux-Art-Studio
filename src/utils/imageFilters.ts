import { AdjustmentState } from '../types';

export const DEFAULT_ADJUSTMENTS: AdjustmentState = {
  brightness: 0,
  contrast: 0,
  exposure: 0,
  saturation: 0,
  temperature: 0,
  tint: 0,
  sharpness: 0,
};

export const buildFilter = (adjustments: AdjustmentState): string => {
  const brightness = 100 + adjustments.brightness + adjustments.exposure;
  const contrast = 100 + adjustments.contrast;
  const saturation = 100 + adjustments.saturation;
  return `brightness(${brightness}%) contrast(${contrast}%) saturate(${saturation}%)`;
};

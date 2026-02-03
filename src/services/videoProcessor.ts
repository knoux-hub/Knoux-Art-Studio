export class VideoProcessor {
  async processVideo(
    url: string,
    _effects: {
      brightness: number;
      contrast: number;
      saturation: number;
      speed: number;
      stabilization: number;
      filters: string[];
    }
  ): Promise<{ url: string }> {
    return Promise.resolve({ url });
  }
}

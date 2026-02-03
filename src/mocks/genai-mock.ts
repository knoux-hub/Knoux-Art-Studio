// TODO: Replace this mock with the real SDK once registry access is restored.
// https://www.npmjs.com/package/@google/genai
type GenerateImagesResponse = {
  generatedImages?: Array<{ image?: { imageBytes?: string } }>;
};

type GenerateContentResponse = {
  candidates?: Array<{ content?: { parts?: Array<{ inlineData?: { data: string } }> } }>;
};

export class GoogleGenAI {
  readonly models = {
    generateImages: async (): Promise<GenerateImagesResponse> => {
      console.warn("GoogleGenAI mock: generateImages called without real SDK.");
      return { generatedImages: [] };
    },
    generateContent: async (): Promise<GenerateContentResponse> => {
      console.warn("GoogleGenAI mock: generateContent called without real SDK.");
      return { candidates: [] };
    },
  };

  constructor(_options: { apiKey?: string }) {}
}

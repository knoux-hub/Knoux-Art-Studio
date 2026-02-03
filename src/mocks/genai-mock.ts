// TODO: Replace this mock with the real SDK once registry access is restored.
// https://www.npmjs.com/package/@google/genai
type GenerateImagesResponse = {
  generatedImages?: Array<{ image?: { imageBytes?: string } }>;
};

type GenerateContentResponse = {
  candidates?: Array<{ content?: { parts?: Array<{ inlineData?: { data: string } }> } }>;
};

export class GoogleGenAI {
  constructor(_options: { apiKey?: string }) {
    console.warn("GoogleGenAI mock in use. Restore @google/genai when registry access is available.");
  }

  readonly models = {
    generateImages: async (): Promise<GenerateImagesResponse> => {
      console.warn("GoogleGenAI mock: generateImages called without real SDK.");
      return { generatedImages: [] };
    },
    generateContent: async (): Promise<GenerateContentResponse> => {
      console.warn("GoogleGenAI mock: generateContent called without real SDK.");
      return { candidates: [] };
    },
    listModels: async (): Promise<unknown[]> => {
      console.warn("GoogleGenAI mock: listModels called without real SDK.");
      return [];
    },
    callModel: async (_model: string, _input: unknown): Promise<Record<string, never>> => {
      console.warn("GoogleGenAI mock: callModel called without real SDK.");
      return {};
    },
  };

  async generateText(_prompt: string): Promise<string> {
    console.warn("GoogleGenAI mock: generateText called without real SDK.");
    return "";
  }

  async generateImage(_prompt: string): Promise<string> {
    console.warn("GoogleGenAI mock: generateImage called without real SDK.");
    return "";
  }

  async enhanceBodyPart(_part: string, _settings: Record<string, unknown>): Promise<Record<string, never>> {
    console.warn("GoogleGenAI mock: enhanceBodyPart called without real SDK.");
    return {};
  }

  async applyMakeup(_type: string, _intensity: number): Promise<Record<string, never>> {
    console.warn("GoogleGenAI mock: applyMakeup called without real SDK.");
    return {};
  }
}

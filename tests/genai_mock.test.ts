import { describe, expect, it } from "vitest";
import { GoogleGenAI } from "../src/mocks/genai-mock";

describe("GoogleGenAI mock", () => {
  it("returns safe placeholders for extended helpers", async () => {
    const mock = new GoogleGenAI({ apiKey: "offline" });
    await expect(mock.generateText("test")).resolves.toBe("");
    await expect(mock.generateImage("test")).resolves.toBe("");
    await expect(mock.enhanceBodyPart("arm", { intensity: 1 })).resolves.toEqual({});
    await expect(mock.applyMakeup("lipstick", 50)).resolves.toEqual({});
    await expect(mock.models.listModels()).resolves.toEqual([]);
    await expect(mock.models.callModel("model", { input: true })).resolves.toEqual({});
  });
});

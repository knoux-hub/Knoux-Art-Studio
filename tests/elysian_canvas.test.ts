import { describe, expect, it } from "vitest";
import { getElysianCanvasFilter } from "../views/ElysianCanvas";

describe("ElysianCanvas filter helper", () => {
  it("returns a neutral filter when using default adjustments", () => {
    expect(getElysianCanvasFilter()).toBe("brightness(100%) contrast(100%) saturate(100%)");
  });
});

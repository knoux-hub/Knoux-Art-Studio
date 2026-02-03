import { describe, expect, it } from "vitest";
import { buildFilter } from "../src/utils/imageFilters";

describe("buildFilter", () => {
  it("builds a CSS filter string based on adjustment values", () => {
    const filter = buildFilter({
      brightness: 10,
      contrast: -5,
      exposure: 5,
      saturation: 20,
      temperature: 0,
      tint: 0,
      sharpness: 0,
    });

    expect(filter).toBe("brightness(115%) contrast(95%) saturate(120%)");
  });
});

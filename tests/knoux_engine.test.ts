import { describe, expect, it, beforeEach } from "vitest";
import { buildMediaPath, KnouxEngine } from "../services/knoux_engine";

describe("buildMediaPath", () => {
  it("builds a normalized media path with forward slashes in non-windows environments", () => {
    const result = buildMediaPath("/mnt/knoux/KnouxArtStudio/", "image.png");
    expect(result).toBe("/mnt/knoux/KnouxArtStudio/projects/media/image.png");
  });
});

describe("importLocalImage", () => {
  beforeEach(() => {
    Object.defineProperty(globalThis, "navigator", {
      value: { hardwareConcurrency: 8, userAgent: "Linux" },
      configurable: true,
    });

    class MockFileReader {
      onload: ((event: ProgressEvent<FileReader>) => void) | null = null;
      readAsDataURL() {
        if (this.onload) {
          this.onload({ target: { result: "data:image/png;base64,AAA" } } as ProgressEvent<FileReader>);
        }
      }
    }

    Object.defineProperty(globalThis, "FileReader", {
      value: MockFileReader,
      configurable: true,
    });
  });

  it("returns metadata shape with size and dimensions", async () => {
    const file = { name: "image.png", size: 1024, type: "image/png" } as File;
    const engine = KnouxEngine.getInstance();
    const asset = await engine.importLocalImage(file);

    expect(asset.metadata.size).toContain("MB");
    expect(asset.metadata.dimensions).toBeTruthy();
    expect(asset.metadata.type).toBe("image/png");
  });
});

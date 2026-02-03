<div align="center">
<img width="1200" height="475" alt="GHBanner" src="https://github.com/user-attachments/assets/0aa67016-6eaf-458a-adb2-6e31a0763ed6" />
</div>

# Run and deploy your AI Studio app

This contains everything you need to run your app locally.

View your app in AI Studio: https://ai.studio/apps/drive/1JdRJaxsyjG8noEGiHg2_UQgYScMNOR_1

## Run Locally

**Prerequisites:**  Node.js


1. Install dependencies:
   `npm install`
2. Set the `GEMINI_API_KEY` in [.env.local](.env.local) to your Gemini API key
3. Run the app:
   `npm run dev`

## Smoke Build (CI-friendly)

If registry access is restricted, use the smoke build script to validate the project safely:

```bash
./scripts/smoke-build.sh
```

The script runs `npm ci`, `tsc --noEmit`, and `vite build` and will skip when registry access blocks required packages.

If type definitions are blocked, you can run a temporary typecheck with:

```bash
tsc --noEmit -p tsconfig.temp.json
```

## GenAI Mock Usage

When `@google/genai` cannot be installed, a local mock is used to unblock development:

- Mock file: `src/mocks/genai-mock.ts`
- Restore helper: `scripts/restore-genai.sh`

Once registry access is available, restore the dependency and replace the mock import with the real SDK:

```bash
./scripts/restore-genai.sh
```

The mock returns safe placeholders for calls like `generateText`, `generateImage`, `enhanceBodyPart`, and `applyMakeup` to keep offline development deterministic.

## Cross-platform Deployment Scripts

PowerShell deployment scripts now share helpers for root detection and logging:

- Helper: `knoux_helpers.ps1`
- Override root: set `KNOUX_ROOT` in the environment

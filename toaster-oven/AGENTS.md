# Toaster Oven

A Vue web app for building the configuration and face data files for **Protosoft Toaster OS**, the Arduino firmware for protogen fursuit heads that use MAX7219 8x8 LED panels. Users edit settings in the browser, then download C++ header files and copy them into the firmware folder.

- This app lives in `toaster-oven/`. The firmware is a sibling folder, `../ToasterOS/`, in the same git repo. `../README.md` explains the hardware and config, with panel diagrams in `../Docs/Images/`.
- `../face-demo.html` is the original standalone expression editor. The Expressions tab replaces it.
- The app outputs two files:
  - `Config.h`: pins, panel layout and optional features.
  - `Protogen_Faces.h`: the expressions.

## Commands

```sh
npm run dev         # dev server
npm run build       # type-check (vue-tsc) + production build. Run before saying a change is done
npm run format      # prettier --write src/
npm run preview     # serve dist/
```

There's no test framework or linter. Prettier handles formatting only (`.prettierrc.json`: no semicolons, single quotes, 100 columns). Run `npx prettier --write src/` after editing.

## Stack

- Vue 3 (`<script setup lang="ts">`), vue-router, Vite, TypeScript.
- Tailwind CSS v4 through `@tailwindcss/vite`. There's no `tailwind.config.js`; the theme lives in `src/assets/main.css` under `@theme`.
- Fonts come from npm packages so the app works offline: `@fontsource-variable/doto` (dot-matrix display face) and `@fontsource-variable/atkinson-hyperlegible-next` (body text). They're imported in `src/main.ts`.
- No state library. State is a single `reactive()` store.

## Layout

```
src/
  main.ts, App.vue          App shell (nav + <main>). App.vue warns before leaving with undownloaded changes
  router/index.ts           /  and  /configure/{panels,expressions,features}
  assets/
    main.css                Theme tokens + small component classes (.btn, .form-input, .kbd, .display-title)
    Protogen_Faces.h        COPY of ../ToasterOS/Protogen_Faces.h, parsed at build time for the preset expressions
  state/
    app-state.ts            appStore: faceConfig, featureConfig, expressions. findExpression(name)
    config-types.ts         TS mirror of ../ToasterOS/ConfigTypes.h (panel type ids, structs)
    config-defaults.ts      Feature defaults matching ../ToasterOS/Config.h, plus the panel layout presets
    generate-config.ts      appStore -> Config.h text. Also downloadBlob()
    generate-faces.ts       expressions -> Protogen_Faces.h text
    config-download.ts      Per-file "changed since last download" tracking + downloadFile(kind)
    expressions.ts          Expression type, defaults, naming/validation helpers
    expression-editor.ts    Selected expression + undo/redo stacks (module state, survives tab switches)
    pin-usage.ts            Detects digital pins assigned twice
    import/
      read-config.ts        Config.h -> state. A list of readers per setting, newest format first
      read-faces.ts         Protogen_Faces.h -> expressions. Adds back missing required expressions
      import-files.ts       Detects each file's type, applies it in place, builds the import summary
  faces/
    faces.ts                Parses the presets from assets/Protogen_Faces.h. mirror(), expressionToPanels()
    parse-faces.ts          Regex parser for `struct FaceExpression Face_X = {...}` blocks
    required-expressions.ts Expressions the firmware references by name (see below)
  components/
    VisorDisplay.vue        SVG visor that draws panels as LED dots. Used on Home (hero), the panel map and the preview
    expressions/            ExpressionCanvas (paint LEDs), ExpressionThumbnail, expression-layout.ts
    form/                   SettingsCard, FormField, ToggleSwitch
    DownloadConfigButton.vue  One button per file (kind="config" | "expressions")
  views/
    HomeView.vue            Hero visor (blinks; click to boop: Surprised -> Angry -> X_X, like the firmware)
    ConfigureView.vue       Heading, both download buttons, tabs, sticky "changed" reminder
    configure/              PanelLayoutView, ExpressionsView, FeaturesView
```

## How the firmware is modelled

- **Panels:** there are 15 panel types (`PanelType` in `config-types.ts`, matching the `#define PANEL_*` values). Each **connection** is a chain of panels on 3 pins (DIN, CS, CLK), listed in wiring order. `-1` means an empty slot. Up to 16 panels per chain.
- **Left and right:** these are as seen from the front of the head, not from the wearer's point of view.
- **Expressions:** every expression is drawn for the **right side** only. The left side is mirrored horizontally, as in `ExpressionManager::Render()`.
  - Each expression has 1 nose panel, 2 eye panels (front, back), an optional 2-panel blink frame, and 4 mouth panels (front → back).
  - A bitmap is 8 numbers, one per row. The most significant bit is the leftmost LED.
- **Visor geometry:** units are LEDs. Panel positions are in `VisorDisplay.vue` and `expression-layout.ts`, laid out like `../Docs/Images/panels_diagram_twocon.png`.
- **Required expressions:** `required-expressions.ts` lists the 16 expressions that `ExpressionManager.h` and `RemoteControl.h` refer to by name.
  - The editor lets users change their LEDs, but blocks renaming and removing them, because either would break the firmware build.
  - Only expressions not in that list can be renamed or removed, e.g. Neutral, Happy Closed eyes, Cute, Silly, OWO, Binary, UWU and user-created ones.
  - The owner plans to make this configurable.
- **Default expression:** the face shown when idle, `DEFAULT_FACE` at the end of `Protogen_Faces.h`. It's stored as `appStore.defaultExpressionId`, so renaming it is fine; it can't be removed while it's the default. `getDefaultExpression()` returns it, and the home page visor and panel map show it. Importing a file without a usable `DEFAULT_FACE` (downloaded before format 3) falls back to Neutral, adding Neutral back if needed.
- **Extra default expressions:** `ExtraDefaultExpressions` in `Protogen_Faces.h`, shown now and then in place of the default expression (every 10–14 seconds for 5 seconds, and after a boop). An empty list turns this off. Stored as `appStore.extraDefaultExpressionIds` (update in place) and toggled per expression in the Expressions tab. `getExtraDefaultExpressions()` returns them in list order, leaving out the default expression, which the firmware would skip anyway. Removing an expression drops it from the list. Importing a file without the list (downloaded before format 4) uses the usual four: Happy Closed eyes, Cute, Blep and Silly.
- **Generated files:**
  - `Protogen_Faces.h` copies `Face_Nose_Single` from the parsed header as-is, and ends with `DEFAULT_FACE` and `ExtraDefaultExpressions` (an empty list is `{}`; the firmware's GCC allows it). The digits used by the game and debug mode 3 live in the firmware's `Utils.h` (`ASCIIGlyphs`), not in this file.
  - Expression names are C++ identifiers, saved as `Face_<name>`. The UI shows them with spaces instead of underscores.

**Keep in sync with the firmware.** If `../ToasterOS/ConfigTypes.h`, `Config.h`, `Protogen_Faces.h` or the files that reference expressions change, update:

- `config-types.ts`
- `config-defaults.ts`
- the generators
- `required-expressions.ts`
- the copy at `src/assets/Protogen_Faces.h`

**Keep old downloads importable.** Users import files they downloaded earlier, possibly from an older version of the site. When the generated format changes, add a reader for the new format at the front of that setting's list in `import/read-config.ts` and keep the old ones. Bump `FORMAT_VERSION` in `config-download.ts`. Settings no reader finds fall back to their defaults and are listed in the import summary.

**Default panel layout differs on purpose.** The app starts with the first preset in `FACE_LAYOUT_PRESETS` (14 panels, one connection). `../ToasterOS/Config.h` keeps the two-connection layout, which is the `14-two` preset. Don't change either to match the other.

To check a generator, render the defaults and diff the output against the firmware file. Expect the panel layout to differ unless you pick the `14-two` preset first. `vite`'s `createServer` + `ssrLoadModule` can load the TS modules from a Node script.

## State and downloads

- All state is in memory. **Nothing persists across a refresh yet.** Saving is planned as "projects" (multiple saved configurations to switch between). Keep state serialisable: plain objects, number bitmaps, and numeric ids on expressions.
- `config-download.ts` regenerates both files reactively and compares each with its text at the last download, starting from the defaults. The download buttons, nav dot and sticky reminder all use `changedDownloads` / `hasChanged(kind)`. A clean import counts as a download (`markDownloaded`).
- The Configure views hold references to `appStore.faceConfig`, `featureConfig` and `expressions`. Update them in place (assign properties, `splice`), never replace the objects.
- Undo/redo covers LED edits and adding or removing a blink, not renames or removals.

## Design conventions

The theme comes from the owner's actual head: smoked plum visor, blue LEDs, white shell and gold horns. The tokens are in `main.css`:

- `page`, `surface`, `border` and `visor-glass` are the dark plum surfaces.
- `led` / `accent` is the blue highlight. `white` is the shell white used for text.
- Tailwind's **`slate` scale is overridden** with plum-tinted greys, so `text-slate-400` etc. match the theme.
- **`gold-*` means "needs attention"**: unsaved changes, pin conflicts, duplicate panels, over 16 panels and invalid names. Use red only for destructive actions.

Rules:

- The Doto dot-matrix font is for page titles and the logo only (`.display-title`). Everything else uses Atkinson Hyperlegible Next.
- No all-caps labels and no decorative motion. The only automatic animation is the visor powering on row by row in `VisorDisplay`; respect `prefers-reduced-motion`.
- Buttons are `class="btn btn-primary"` / `btn btn-secondary` / `btn btn-icon`. Always include the base `btn` class.
- The layout must work at 390px wide. To check screens, use headless Chrome: `chrome --headless=new --screenshot=... --window-size=...` against `npm run preview`. Chrome on Windows has a minimum window width, so for phone widths load the page inside a 390px `<iframe>`.
- Copy is short, plain and in sentence case. The owner edits copy themselves, so keep their wording when changing nearby code.

## Working with the owner

- Run `npm run build` before saying a change is done. Use screenshots to check UI changes.
- Don't commit unless asked. The owner reviews and commits themselves.

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
  router/index.ts           /, /configure/{panels,expressions,features}, /guides and /guides/<section>/<guide>
  assets/
    main.css                Theme tokens + small component classes (.btn, .form-input, .kbd, .display-title)
    Protogen_Faces.h        COPY of ../ToasterOS/Protogen_Faces.h, plus OWO, Binary and UWU, parsed at build time for the preset expressions
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
    boop-expressions.ts     Boop expression helpers, matching ExpressionManager::GetBoopExpression()
    remote-actions.ts       Remote control combos (AA-DD), actions and their labels
    movement-speed.ts       Normal / slow / very slow movement per expression
    import/
      read-config.ts        Config.h -> state. A list of readers per setting, newest format first
      read-faces.ts         Protogen_Faces.h -> expressions. Adds back missing required expressions
      import-files.ts       Detects each file's type, applies it in place, builds the import summary
  guides/
    <section>/*.md          The guides, a folder per section (toaster-os/, later building/ ...).
                            A --- header with title, description, order and optional group, then Markdown
    images/                 Pictures for the guides, linked as images/name.png
    sections.ts             GUIDE_SECTIONS: each section's title, description, coming-soon topics and unnumbered groups
    guides.ts               Loads the guides (import.meta.glob) and their pictures
    markdown.ts             Small Markdown renderer (no package needed). See its header for what it supports
    embeds.ts               Parts of the site a guide can show with a `::name args` line, e.g. ::panel-layout 14-one
  faces/
    faces.ts                Parses the presets from assets/Protogen_Faces.h. mirror(), expressionToPanels()
    parse-faces.ts          Regex parser for `struct FaceExpression Face_X = {...}` blocks and their animations
    animation.ts            Animation frames: frameView(), detachFrame(), pruneFrames(), frameIndexAt()
    required-expressions.ts Expressions the firmware references by name (see below)
  components/
    VisorDisplay.vue        SVG visor that draws panels as LED dots. Used on Home (hero), the panel map and the preview
    PanelLayoutMap.vue      The panel layout map for some connections: C1 #3 labels and wiring. Panels page and guides
    expressions/            ExpressionCanvas (paint LEDs), ExpressionThumbnail, expression-layout.ts
    form/                   SettingsCard, FormField, ToggleSwitch
    DownloadConfigButton.vue  One button per file (kind="config" | "expressions")
  views/
    GuidesView.vue          The guides by section and group, with each section's "coming soon" topics
    GuideView.vue           One guide, with "On this page" and previous/next links
    HomeView.vue            Hero visor (blinks; click to boop, using the boop expressions like the firmware)
    ConfigureView.vue       Heading, both download buttons, tabs, sticky "changed" reminder
    configure/              PanelLayoutView, ExpressionsView, FeaturesView
```

## How the firmware is modelled

- **Panels:** there are 15 panel types (`PanelType` in `config-types.ts`, matching the `#define PANEL_*` values). Each **connection** is a chain of panels on 3 pins (DIN, CS, CLK), listed in wiring order. `-1` means an empty slot. Up to 16 panels per chain.
- **Left and right:** these are as seen from the front of the head, not from the wearer's point of view.
- **Expressions:** every expression is drawn for the **right side** only. The left side is mirrored horizontally, as in `ExpressionManager::Render()`.
  - Each expression has 1 nose panel, 2 eye panels (front, back), an optional 2-panel blink frame, 4 mouth panels (front → back) and an optional animation.
  - A bitmap is 8 numbers, one per row. The most significant bit is the leftmost LED.
- **Visor geometry:** units are LEDs. Panel positions are in `VisorDisplay.vue` and `expression-layout.ts`, laid out like `../Docs/Images/panels_diagram_twocon.png`.
- **Required expressions:** `required-expressions.ts` lists the expressions `ExpressionManager.h` refers to by name: only Heart, for the nose rub.
  - The editor lets users change their LEDs, but blocks renaming and removing them, because either would break the firmware build.
  - Only expressions not in that list can be renamed or removed, i.e. everything except Heart.
  - The owner plans to make this configurable.
- **Default expression:** the face shown when idle, `DEFAULT_FACE` at the end of `Protogen_Faces.h`. It's stored as `appStore.defaultExpressionId`, so renaming it is fine; it can't be removed while it's the default. `getDefaultExpression()` returns it, and the home page visor and panel map show it.
- **Extra default expressions:** `ExtraDefaultExpressions` in `Protogen_Faces.h`, shown now and then in place of the default expression (every 10–14 seconds for 5 seconds, and after a boop). An empty list turns this off. Stored as `appStore.extraDefaultExpressionIds` (update in place) and toggled per expression in the Expressions tab. `getExtraDefaultExpressions()` returns them in list order, leaving out the default expression, which the firmware would skip anyway. Removing an expression drops it from the list.
- **Boop expressions:** `BoopExpressions` in `Protogen_Faces.h`: `{ minBoops, &Face_X }` entries. The firmware shows the entry with the highest `minBoops` reached; with none reached, or an empty list, the current expression stays. Rubbing the nose always shows the hearts, which aren't configurable yet. Stored as `appStore.boopExpressions` (`{ minBoops, expressionId }`, update in place) and edited under Features → Boop sensor, though it's saved in `Protogen_Faces.h`, not `Config.h`. The generator sorts entries by `minBoops` and leaves out invalid ones, which the editor flags. Removing an expression drops its entries.
- **Remote control combos:** `RemoteControlActions` in `Protogen_Faces.h`: `{ "AB", REMOTE_EXPRESSION, &Face_X }` or `{ "DA", REMOTE_CYCLE_BRIGHTNESS }` entries (actions are `RemoteActionType` in `../ToasterOS/ConfigTypes.h`). Combos not listed do nothing; the firmware uses the first entry for a combo. Stored as `appStore.remoteActions`, always one entry per combo AA to DD with `action: 'none'` for unused ones (update in place), and edited under Features → Remote control, though it's saved in `Protogen_Faces.h`. Removing an expression sets its combos to nothing.
- **Movement speed:** `SlowMovementExpressions` (every 300 ms) and `VerySlowMovementExpressions` (every 600 ms) in `Protogen_Faces.h`; other expressions move every 200 ms. The firmware checks the slow list first. Stored as `appStore.slowMovementExpressionIds` and `verySlowMovementExpressionIds` (update in place, never both for one expression) and shown as a Normal / Slow / Very slow dropdown on each expression. `movement-speed.ts` reads and sets it. Lists are generated in expression order.
- **Animations:** an expression's `animation` is a list of frames (`AnimationFrame`, `FaceAnimation` and `ANIMATION()` in the firmware), or null. Frame 1 is the expression itself and never replaces parts; later frames store only the eyes, mouth or nose they replace (`null`: follow frame 1). The editor draws on each frame as a whole face: `frameView()` shows a frame, `detachFrame()` gives it copies before a stroke and `pruneFrames()` drops copies identical to frame 1 after it, so undrawn parts keep following frame 1. The parser moves parts a hand-written first frame replaces into the expression (`normaliseFirstFrame()`). Animated expressions don't blink, so the blink controls are hidden for them; the preview plays the animation unless the viewer prefers reduced motion. Generated names are `<Name>_Frames`, `<Name>_Animation` and `<Name>_Frame<N>_Eye/Mouth/Nose`, written just before the expression. Durations are 1-65535 ms (an `unsigned int` on the Arduino).
- **Generated files:**
  - `Protogen_Faces.h` starts with the face types (keep them identical to the firmware's), copies `Face_Nose_Single` from the parsed header as-is, and ends with `DEFAULT_FACE`, `ExtraDefaultExpressions`, `BoopExpressions`, `RemoteControlActions` and the two movement lists (an empty list is `{}`; the firmware's GCC allows it). The digits used by the game and debug mode 3 live in the firmware's `Utils.h` (`ASCIIGlyphs`), not in this file.
  - Expression names are C++ identifiers, saved as `Face_<name>`. The UI shows them with spaces instead of underscores.

**Keep in sync with the firmware.** If `../ToasterOS/ConfigTypes.h`, `Config.h`, `Protogen_Faces.h` or the files that reference expressions change, update:

- `config-types.ts`
- `config-defaults.ts`
- the generators
- `required-expressions.ts`
- the copy at `src/assets/Protogen_Faces.h`. Keep the OWO, Binary and UWU expressions in it: the firmware no longer has them, but they stay in the editor for others to use

**Keep old downloads importable.** Users import files they downloaded earlier, possibly from an older version of the site. When the generated format changes, add a reader for the new format at the front of that setting's list in `import/read-config.ts` and keep the old ones. Bump `FORMAT_VERSION` in `config-download.ts`. Settings no reader finds fall back to their defaults and are listed in the import summary. Format 1 is the baseline: nobody had used the site before it, so nothing older needs to import. `Protogen_Faces.h` has no per-setting readers: a file missing any of `DEFAULT_FACE`, `ExtraDefaultExpressions`, `BoopExpressions`, `RemoteControlActions` or the movement lists fails to import. Within a file, missing required expressions are added back and settings that name expressions it doesn't have are skipped (a default that isn't there falls back to the firmware's), with a note.

**Default panel layout differs on purpose.** The app starts with the first preset in `FACE_LAYOUT_PRESETS` (14 panels, one connection). `../ToasterOS/Config.h` keeps the two-connection layout, which is the `14-two` preset. Don't change either to match the other.

To check a generator, render the defaults and diff the output against the firmware file. Expect the panel layout to differ unless you pick the `14-two` preset first. `vite`'s `createServer` + `ssrLoadModule` can load the TS modules from a Node script.

## Guides

The Guides section (`/guides`) is Markdown in `src/guides/<section>/`. Sections are listed in `GUIDE_SECTIONS` in `sections.ts`, with a title, a description and the topics still to come (shown as "coming soon"). `toaster-os/` covers this software; `building/` (head, body and armour) has no guides yet. The owner's plan for both is `../guides-overview.md`.

To add a guide, add a `.md` file to its section's folder. Its path is its address: `toaster-os/side-lights.md` is `/guides/toaster-os/side-lights`. Each file starts with a header:

```
---
title: Side lights
description: One sentence, shown on the guides list.
order: 3
group: Enabling features
---
```

`order` sets its place in the section. `group` is optional: guides with the same group are listed under that heading, where the first of them comes in the order. Each list of guides (a group, or a run of guides without one) is numbered from 1, unless the group is in its section's `unnumberedGroups`. Previous/Next links stay within the section.

`markdown.ts` renders a small subset of Markdown, listed at the top of that file, including callouts (`> [!NOTE]`, `> [!TIP]`, `> [!WARNING]`). HTML in a guide is shown as text. Links to other pages start with `/` (e.g. `/guides/toaster-os/debugging#check-the-panels`); heading ids are the heading text in lowercase with dashes. Pictures are shared by all sections, in `src/guides/images/`, linked as `images/name.png`; keep them small, as they're bundled with the site. `power-splitting.svg` (the Power section of Setting up) is drawn by `scripts/power-splitting.py`: change the script and run it with `python scripts/power-splitting.py`, rather than editing the SVG. Add new guides to `public/sitemap.xml`. A line like `::panel-layout 14-one` shows a live part of the site (here the panel layout map for a preset) in place of a picture; `embeds.ts` lists what's available, and `GuideView.vue` mounts them. Styles are `.guide-content` in `main.css`. `> [!NOTE]` callouts starting with **To do:** mark parts the owner still needs to write. Toaster OS runs on an Arduino Nano Every or an ESP32, so guides say "microcontroller" (lowercase, except at the start of a sentence or heading), not "Arduino". Name a board only for something specific to it, such as the Nano Every's 6 KB of memory, or for the Arduino IDE. Pin numbers are examples, as people can use any free pins on their microcontroller: say "for example, pin 9" or label a table column "Example pin".

## State and downloads

- All state is in memory. **Nothing persists across a refresh yet.** Saving is planned as "projects" (multiple saved configurations to switch between). Keep state serialisable: plain objects, number bitmaps, and numeric ids on expressions.
- `config-download.ts` regenerates both files reactively and compares each with its text at the last download, starting from the defaults. The download buttons, nav dot and sticky reminder all use `changedDownloads` / `hasChanged(kind)`. A clean import counts as a download (`markDownloaded`).
- The Configure views hold references to `appStore.faceConfig`, `featureConfig` and `expressions`. Update them in place (assign properties, `splice`), never replace the objects.
- Undo/redo covers LED edits, adding or removing a blink, and changes to animation frames (except durations), not renames or removals.

## Design conventions

The theme comes from the owner's actual head: smoked plum visor, blue LEDs, white shell and gold horns. The tokens are in `main.css`:

- `page`, `surface`, `border` and `visor-glass` are the dark plum surfaces.
- `led` / `accent` is the blue highlight. `white` is the shell white used for text.
- Tailwind's **`slate` scale is overridden** with plum-tinted greys, so `text-slate-400` etc. match the theme.
- **`gold-*` means "needs attention"**: unsaved changes, pin conflicts, duplicate panels, over 16 panels, invalid names and boop expression problems. Use red only for destructive actions.

Rules:

- The Doto dot-matrix font is for page titles and the logo only (`.display-title`). Everything else uses Atkinson Hyperlegible Next.
- No all-caps labels and no decorative motion. The only automatic animation is the visor powering on row by row in `VisorDisplay`; respect `prefers-reduced-motion`.
- Buttons are `class="btn btn-primary"` / `btn btn-secondary` / `btn btn-icon`. Always include the base `btn` class.
- The layout must work at 390px wide. To check screens, use headless Chrome: `chrome --headless=new --screenshot=... --window-size=...` against `npm run preview`. Chrome on Windows has a minimum window width, so for phone widths load the page inside a 390px `<iframe>`.
- Copy is short, plain and in sentence case. The owner edits copy themselves, so keep their wording when changing nearby code.

## Working with the owner

- Run `npm run build` before saying a change is done. Use screenshots to check UI changes.
- Don't commit unless asked. The owner reviews and commits themselves.

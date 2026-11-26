---
title: Adding custom features
description: Make your own copy of Toaster OS, learn how it fits together, and extend it.
order: 100
group: Enabling features
---

Want to change more than Toaster Oven lets you? You can make your own copy of the code and change anything. This guide covers setting that up, how the code fits together, and the usual ways to extend it.

> [!TIP]
> Make one small change at a time, and upload it to check it works. It's much easier to find a problem that way.

## Make your own copy

1. Make a [GitHub](https://github.com) account if you don't have one.
2. Open [Toaster OS on GitHub](https://github.com/BadgerCode/ProtosoftToasterOS) and click **Fork**. This makes your own copy, which you can change freely.
3. Clone your fork to your computer, and open `ToasterOS/ToasterOS.ino` in the Arduino IDE.
4. Set it up as in [Setting up](/guides/toaster-os/setting-up), and commit your `Config.h` and `Protogen_Faces.h`.

To bring in new changes from the original project:

```sh
git remote add upstream https://github.com/BadgerCode/ProtosoftToasterOS.git
git pull upstream main
```

You only need the first line once. If an update changes `Config.h` or `Protogen_Faces.h`, keep the new files, then import your old ones into [Toaster Oven](/configure), check everything, and download them again.

## How the code fits together

Everything is in the `ToasterOS` folder:

| File                  | What it does                                                                         |
| --------------------- | ------------------------------------------------------------------------------------ |
| `ToasterOS.ino`       | Starts everything up, and runs the main loop                                         |
| `Config.h`            | Your settings: pins, panel layout and features                                       |
| `ConfigTypes.h`       | The types the settings use, and the panel names                                      |
| `Protogen_Faces.h`    | Your expressions and animations, and which are used for boops, the remote and so on  |
| `ExpressionManager.h` | Chooses which expression to show, and handles blinking, face movement and animations |
| `FaceRender.h`        | Keeps track of what each panel shows, and sends only the changes to the panels       |
| `MAX7219Control.h`    | Talks to the MAX7219 panels                                                          |
| `BoopState.h`         | Reads the boop sensor, and counts boops and nose rubs                                |
| `RemoteControl.h`     | Reads the remote, and runs the button combos                                         |
| `LEDStripRender.h`    | Drives the side lights                                                               |
| `Game.h`              | A secret game, started by lots of boops in a row                                     |
| `Utils.h`             | Small helpers, and the letters and numbers used by the game and debug mode           |

Each time round, `loop()` in `ToasterOS.ino`:

1. Reads the boop sensor and the remote
2. Starts the game if there have been enough boops in a row
3. Updates the current expression: blinking, movement, and the occasional extra expression
4. Draws the face (or runs the game), and updates the side lights
5. Sends any changes to the LEDs

Expressions are drawn for the **right** side of the face, as seen from the front, and mirrored for the left. A panel is 8 rows of 8 LEDs, written as 8 binary numbers, with the leftmost digit as the leftmost LED:

```cpp
{ B00110000, B01110000, B11100000, B11000000, B00000000, B00000000, B00000000, B00000000 }
```

## Adding a setting

Settings live in `ProtoConfig`, in `Config.h`.

1. Add the setting to `ProtoConfigStruct` in `ConfigTypes.h`:

```cpp
bool EnableMySetting;  // What it does
```

2. Add it to `ProtoConfig` in `Config.h`, **in the same position** as in the struct. C++ needs the settings in the same order in both places, or it won't compile:

```cpp
.EnableMySetting = true, // What it does
```

3. Use it anywhere in the code as `ProtoConfig.EnableMySetting`.

To set it from Toaster Oven too, add it to the site in `toaster-oven/src`: `FeatureConfig` in `state/config-types.ts`, its default in `state/config-defaults.ts`, its line in `state/generate-config.ts`, a reader in `state/import/read-config.ts`, and a control in `views/configure/FeaturesView.vue`.

## Adding a remote control action

1. Add your action to `RemoteActionType` in `ConfigTypes.h`:

```cpp
REMOTE_MY_ACTION,  // What it does
```

2. Add a `case` for it in `RunAction()` in `RemoteControl.h`:

```cpp
case REMOTE_MY_ACTION:
  // Do something
  break;
```

3. Give it a button combo in `RemoteControlActions` in `Protogen_Faces.h`:

```cpp
{ "DC", REMOTE_MY_ACTION },
```

To choose it in Toaster Oven, add it to `REMOTE_BUILT_IN_ACTIONS` in `toaster-oven/src/state/remote-actions.ts`.

## Writing an animation by hand

Toaster Oven can [make animations for you](/guides/toaster-os/managing-expressions#animations). To write one by hand in `Protogen_Faces.h`, list the frames, then point the expression at them:

```cpp
// The eyes for frame 2. Frames can also replace the mouth (byte X[4][8]) or nose (byte X[1][8])
const EyeFrame Wink_Frame2_Eye = {
  { B00000000, B00000000, B00000000, B11111111, B00000000, B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, B11111100, B00000000, B00000000, B00000000, B00000000 }
};

const AnimationFrame Wink_Frames[] = {
  { 1000 },                   // 1000ms of the expression itself
  { 300, &Wink_Frame2_Eye }   // Then 300ms with different eyes
};
const FaceAnimation Wink_Animation = ANIMATION(Wink_Frames);

struct FaceExpression Face_Wink = {
  // ... .Nose, .Eye, .HasBlink, .Eye_Blink and .Mouth as usual ...
  .Animation = &Wink_Animation
};
```

Each frame is a duration in milliseconds, then the eyes, mouth and nose it replaces. Leave a part out (or use `NULL`) to keep the expression's own.

## Drawing on the panels yourself

To show your own picture on a panel:

```cpp
byte picture[8] = { B00011000, B00111100, B01111110, B11111111, B11111111, B01111110, B00111100, B00011000 };
ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, picture, 0, false);
```

The last two values move the picture up or down, and mirror it. The panel names, like `PANEL_RIGHT_EYE_FRONT`, are in `ConfigTypes.h`.

The current expression is drawn every loop too, so draw yours in `loop()` straight after `ExpressionState->Render()`, or the expression will draw over it. Only panels that have changed are sent, so it's fine to do this every loop.

## Memory

The Nano Every has just 6 KB of memory, and every expression, frame and list uses some. When you compile, check the line that says **Global variables use … bytes of dynamic memory**. If it goes much above 75%, the Arduino can start behaving strangely.

## Debugging

Set **Debug mode** on Toaster Oven's Features page (or in `Config.h`) to get help while you work:

| Mode | What it does                                                        |
| ---- | ------------------------------------------------------------------- |
| 1    | Prints how many times the main loop runs each second                |
| 2    | Prints the longest time one loop took recently, in milliseconds     |
| 3    | Shows each panel's number, to check the panel order and orientation |

Open the Serial Monitor (**Tools → Serial Monitor**) at **9600 baud** to see the output from modes 1 and 2.

## Changing Toaster Oven

Toaster Oven (this website) is in the `toaster-oven` folder of the same repository. To run your own copy, install [Node.js](https://nodejs.org), then:

```sh
cd toaster-oven
npm install
npm run dev
```

`toaster-oven/AGENTS.md` describes how the site is put together.

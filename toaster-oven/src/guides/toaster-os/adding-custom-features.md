---
title: Adding custom features
description: Write your own feature, such as reacting to a microphone, and hook it into Toaster OS.
order: 100
group: Enabling features
---

Want your head to do something Toaster OS doesn't do yet, like moving the mouth to a microphone? This guide shows how to write your own feature and hook it into Toaster OS, one small example at a time.

You'll need Toaster OS already set up and working on your head. If it isn't yet, start with [Setting up](/guides/toaster-os/setting-up).

> [!TIP]
> Make one small change at a time, and upload it to check it works. It's much easier to find a problem that way.

## How it works

Your feature lives in **its own file** in the `ToasterOS` folder, with its own settings at the top.

You also need to change a few lines of Toaster OS itself, so it runs your feature. You only need to do this once.

> [!WARNING]
> Do not add new configuration options to `Config.h`!\
> It will get lost the next time you download configuration from the Toaster Oven website.

### The main loop

In `ToasterOS.ino`, the main loop `loop()` runs constantly. It does the following:

1. Reads the boop sensor and the remote
2. Updates the current expression: blinking, movement, and the occasional extra expression
3. Draws the expression on the face panels
4. Sets the side lights
5. Sends any changes to the LEDs

Your feature gets a turn after each of steps 2, 3 and 4. Whatever it draws comes after the expression and the side lights, so it's drawn over them.

### The files

Everything is in the `ToasterOS` folder:

| File                  | What it does                                                                           |
| --------------------- | -------------------------------------------------------------------------------------- |
| `ToasterOS.ino`       | Starts everything up, and runs the main loop                                           |
| `Config.h`            | Your settings from Toaster Oven: pins, panel layout and features                       |
| `ConfigTypes.h`       | The types the settings use, and the panel names                                        |
| `Protogen_Faces.h`    | Your expressions from Toaster Oven, and which are used for boops, the remote and so on |
| `ExpressionManager.h` | Chooses which expression to show, and handles blinking, face movement and animations   |
| `FaceRender.h`        | Keeps track of what each panel shows, and sends only the changes to the panels         |
| `MAX7219Control.h`    | Talks to the MAX7219 panels                                                            |
| `BoopState.h`         | Reads the boop sensor, and counts boops and nose rubs                                  |
| `RemoteControl.h`     | Reads the remote, and runs the button combos                                           |
| `LEDStripRender.h`    | Drives the side lights                                                                 |
| `Game.h`              | A secret game, started by lots of boops in a row                                       |
| `Utils.h`             | Small helpers, and the letters and numbers used by the game and debug mode             |

## 1. An empty feature

Every feature your write should start like this, to make sure you've hooked into the rest of the protogen code.

### Make your file

In the Arduino IDE, click the **⋯** button at the top right of the code, choose **New Tab**.\
Give it a suitable name which describes what the feature does (e.g. `MicMouth.h`).

Anywhere you see "MyFeature" use your feature name instead (e.g. `MicMouth.h`).

Paste the following code into it:

```cpp
// MyFeature: what it does (e.g. animates the mouth to your speech, using a microphone)

// Settings for this feature
// Keep them here, not in Config.h, so they aren't lost when you download a new Config.h
// Start each name with the feature's name, so it doesn't clash with anything in Toaster OS
// #define MYFEATURE_EXAMPLE_SETTING 1
// const char MYFEATURE_EXAMPLE_SETTING2[] = "Example";


class MyFeature {
private:
  // The parts of Toaster OS this feature can use
  FaceRender* FaceRenderer;
  LEDStripRender* LEDStripRenderer;
  BoopStateHandler* BoopState;
  RemoteControl* RemoteControlState;

public:
  MyFeature(FaceRender* faceRenderer, LEDStripRender* ledStripRenderer, BoopStateHandler* boopState, RemoteControl* remoteControlState) {
    FaceRenderer = faceRenderer;
    LEDStripRenderer = ledStripRenderer;
    BoopState = boopState;
    RemoteControlState = remoteControlState;
  }

  // Runs once, when the head turns on. Set up your pins here
  void Initialise() {
  }

  // Runs every loop, after the boop sensor and remote are read. Read your sensors and update your feature here
  void Update() {
  }

  // Runs every loop, just after the expression is drawn. Draw on the face panels here
  void DrawFace() {
  }

  // Runs every loop, just after the side lights are set. Set the side lights here
  void DrawSideLights() {
  }
};
```

### Hook it into Toaster OS

Open `ToasterOS.ino` and make these changes. The lines to add are marked `// Add this`.

1. Include your file, after the others at the top:

```cpp
#include "Game.h"
#include "MyFeature.h"  // Add this (replace MyFeature.h with the name you used)
```

2. Create your feature, after the others are created:

```cpp
// Secret game
CubeGame* CubeGameRunner = new CubeGame();

// My feature
MyFeature* MyFeatureRunner = new MyFeature(ProtoFaceRenderer, LEDStripRenderer, BoopState, RemoteControlState);  // Add this
```

3. Set it up, at the end of `setup()`:

```cpp
  // Remote control
  if (ProtoConfig.EnableRemoteControl) {
    RemoteControlState->Initialise();
  }

  // My feature
  MyFeatureRunner->Initialise();  // Add this
}
```

4. Run it in `loop()`, in these three places:

```cpp
  // UPDATE STATE- EXPRESSIONS
  ExpressionState->Update();

  // UPDATE STATE- MY FEATURE
  MyFeatureRunner->Update();  // Add this


  // ...Further down...


  // Main logic
  if (EnableGame) {
    EnableGame = CubeGameRunner->GameLoop(ProtoFaceRenderer, LEDStripRenderer, BoopState->BoopSensorTouched);
  } else {

    // Render the face
    ExpressionState->Render();
    MyFeatureRunner->DrawFace();  // Add this
    ProtoFaceRenderer->ProcessRenderQueue();

    // LED strips
    if (ProtoConfig.EnableSideLEDs && NextLEDStripUpdate <= curTime) {
      // ... leave this part as it is ...
    }

    MyFeatureRunner->DrawSideLights();  // Add this
  }
```

`DrawFace()` must go between `ExpressionState->Render()` and `ProtoFaceRenderer->ProcessRenderQueue()`, so it draws after the expression but before the changes are sent to the panels.

Upload it. The head should work exactly as before. If it doesn't compile, check each change against the steps above.

## 2. Drawing on the face

Now draw something of your own over the expression. This replaces both eyes with a round eye, looking forward.

Add the picture to the `private:` part of your class, and fill in `DrawFace()`:

```cpp
class MyFeature {
private:
  // ... the parts of Toaster OS, as before ...

  // A round eye, with the pupil on the right. Drawn for the right side of the face, as seen from the front
  byte EyePicture[8] = { B00111100, B01000010, B10000001, B10000111, B10000111, B10000001, B01000010, B00111100 };

public:
  // ... the rest, as before ...

  void DrawFace() {
    // The right side
    FaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, EyePicture, 0, false);
    FaceRenderer->ClearPanel(PANEL_RIGHT_EYE_BACK);

    // The left side: the same picture, mirrored
    FaceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, EyePicture, 0, true);
    FaceRenderer->ClearPanel(PANEL_LEFT_EYE_BACK);
  }
};
```

A panel is 8 rows of 8 LEDs, written as 8 binary numbers from top to bottom. A `1` is an LED that's on, and the leftmost digit is the leftmost LED.

`UpdatePanel()` takes:

- **The panel**: its name, like `PANEL_RIGHT_EYE_FRONT`. They're all listed at the top of `ConfigTypes.h`. Left and right are as seen from the front of the head.
- **The picture**: 8 rows.
- **An offset**: moves the picture up (negative) or down (positive) by that many rows. Use `0` to leave it where it is.
- **Mirror**: `true` flips it left to right. Like expressions, draw for the right side and mirror it for the left, so the face matches on both sides.

The expression is drawn every loop, and your picture is drawn over it straight after, so you won't see the expression's eyes flicker through. Any panel you don't draw on, like the mouth and nose here, keeps showing the expression.

> [!TIP]
> It's fine to draw every loop. Toaster OS only sends the rows that have changed to the panels.

## 3. Setting the side lights

The side lights work the same way: Toaster OS sets them, then your feature can change them. This sends a white dot along the side lights, over your first side light colour.

```cpp
class MyFeature {
private:
  // ... as before ...

  // The dot's position, and when it moves next
  int DotPosition = 0;
  unsigned long NextDotMove = 0;

public:
  // ... as before ...

  void DrawSideLights() {
    if (!ProtoConfig.EnableSideLEDs) return;

    // Move the dot along one LED every 50ms, going back to the start at the end
    if (millis() >= NextDotMove) {
      DotPosition = (DotPosition + 1) % LEDSTRIP_NUM_LEDS;
      NextDotMove = millis() + 50;
    }

    // Draw every loop, so Toaster OS's own colours don't show through
    LEDStripRenderer->SetAllLEDs(CRGB(ProtoConfig.SideLEDColourA));
    LEDStripRenderer->SetLED(DotPosition, CRGB::White);
  }
};
```

- `SetAllLEDs()` sets every LED, and `SetLED()` sets one, counting from 0. Both strips always show the same thing.
- Colours are [FastLED](https://fastled.io) colours: a name like `CRGB::White`, red, green and blue like `CRGB(255, 0, 128)`, or hue, saturation and brightness like `CHSV(160, 255, 255)`.
- The side lights match the face's brightness automatically.
- You can read any of the settings from Toaster Oven with `ProtoConfig`, like `ProtoConfig.SideLEDColourA` here. Read them, but leave changing them to Toaster Oven.

### Timing

`millis()` is how long the head has been on, in milliseconds. To do something every so often, save when it should next happen, and check for it each loop, as `NextDotMove` does here.

> [!WARNING]
> Never use `delay()`. It stops the whole head while it waits: the face, the boop sensor and the remote.

## 4. Reading the boop sensor

Your feature can react to boops, using what the boop sensor has already worked out. This counts every boop, and shows the count on the eyes for 2 seconds after each one. Its `DrawFace()` replaces the round eyes from step 2:

```cpp
class MyFeature {
private:
  // ... as before ...

  int BoopCount = 0;
  bool WasBooped = false;
  unsigned long ShowCountUntil = 0;

public:
  // ... as before ...

  void Update() {
    // A boop has just started
    if (BoopState->BoopActive && !WasBooped) {
      BoopCount++;
      ShowCountUntil = millis() + 2000;
    }

    WasBooped = BoopState->BoopActive;
  }

  void DrawFace() {
    // Only draw while showing the count. The rest of the time, the expression shows as normal
    if (millis() >= ShowCountUntil) return;

    // The tens on the left eye, and the units on the right. ASCIIGlyphs (in Utils.h) has the digits 0 to 9, then A to Z
    int count = BoopCount % 100;
    FaceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, ASCIIGlyphs[count / 10], 0, false);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, ASCIIGlyphs[count % 10], 0, false);
  }
};
```

Neither side is mirrored here, so the numbers read the right way round.

`BoopState` has:

| Name                               | What it is                                                                                                        |
| ---------------------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `BoopState->BoopActive`            | `true` while the nose is being booped. It ignores very short touches, and lasts half a second after the boop ends |
| `BoopState->BoopSensorTouched`     | `true` while the sensor sees something, this very moment                                                          |
| `BoopState->BoopJustEnded`         | `true` for one loop, when a boop ends                                                                             |
| `BoopState->ConsecutiveShortBoops` | How many boops in a row. Goes back to 0 after a few seconds without boops                                         |
| `BoopState->IsFaceRub()`           | `true` while the nose is being rubbed: one long boop                                                              |
| `BoopState->TimeSinceBoopStart()`  | How long the current boop has lasted, in milliseconds, or -1 if there isn't one                                   |

These only change while the boop sensor is turned on, in Toaster Oven or with the remote.

## 5. Turning it on and off with the remote

A remote control combo can turn your feature on and off.

You can find which button combo has been pressed by checking:\
`RemoteControlState->JustPressedCombo`

Add two settings to the top of your file, and an `Enabled` switch to your class. Check for the combo in `Update()`, and do nothing while it's turned off:

```cpp
// Settings for this feature
const bool MyFeature_StartEnabled = true;   // Whether it's on when the head turns on
const char MyFeature_ToggleCombo[] = "DC";  // The remote combo that turns it on or off


class MyFeature {
private:
  // ... as before ...

  bool Enabled = MyFeature_StartEnabled;

public:
  // ... as before ...

  void Update() {
    // Turn on or off with the remote
    if (strcmp(RemoteControlState->JustPressedCombo, MyFeature_ToggleCombo) == 0) {
      Enabled = !Enabled;
    }

    if (!Enabled) return;

    // ... the rest of Update(), as before ...
  }

  void DrawFace() {
    if (!Enabled) return;

    // ... the rest of DrawFace(), as before ...
  }

  void DrawSideLights() {
    if (!Enabled) return;

    // ... the rest of DrawSideLights(), as before ...
  }
};
```

Pick a combo that does **Nothing** on Toaster Oven's [Features page](/configure/features), under **Remote control**, or both will happen. **D then C** does nothing by default.

To start something with the remote instead, like showing the boop count on demand, do it when the combo is pressed:

```cpp
    if (strcmp(RemoteControlState->JustPressedCombo, "DC") == 0) {
      ShowCountUntil = millis() + 2000;
    }
```

## Ideas

- **A microphone**: connect an analog microphone module to a free analog pin (for example, A1). In `Update()`, read it with `analogRead()`, and in `DrawFace()`, draw the mouth open wider the louder it is.
- **A different sensor**: some sensors, like the APDS9960 mentioned in [Boop sensor](/guides/toaster-os/boop-sensor#other-sensors), come with their own library. Install it with **Tools → Manage Libraries**, `#include` it at the top of your file, and set it up in `Initialise()`.
- **More than one feature**: give each its own file and class, and hook each into `ToasterOS.ino` the same way.

## Tips

### Keep the loop quick

Toaster OS runs everything in one loop, so anything slow in your feature slows the whole head. Use `millis()` for timing, never `delay()`, and only read slow sensors every so often.

Set **Debug mode** to **2** on Toaster Oven's Features page (or in `Config.h`) to check. Open the Serial Monitor (**Tools → Serial Monitor**) at **9600 baud** to see the longest time one loop took recently, in milliseconds. Compare it with and without your feature.

### Printing messages

To see what your feature is doing, print to the Serial Monitor:

```cpp
Serial.println(BoopCount);
PrintF("Boops: %d\n", BoopCount);
```

The Serial Monitor only works while **Debug mode** is on. Mode 1 prints the loops per second too, which is handy for checking your feature hasn't slowed things down.

### Memory

The Nano Every has just 6 KB of memory, and every expression, frame, list and variable uses some. When you compile, check the line that says **Global variables use … bytes of dynamic memory**. If it goes much above 75%, the Nano Every can start behaving strangely.

### Updating Toaster OS

Before updating, keep a copy of your feature's file, and note the changes you made to `ToasterOS.ino`. Then make them again in the new version.

If you use [Git](https://git-scm.com), commit your changes first, and Git will usually bring them across for you.

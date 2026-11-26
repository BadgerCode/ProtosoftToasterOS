---
title: Setting up
description: Wire up the LED panels, then configure Toaster OS and put it on your microcontroller.
order: 1
---

This guide gets a face working: the microcontroller, the LED panels, and the software. Extras like the side lights, boop sensor and remote each have their own guide under **Enabling features**.

## The electronics

### Microcontroller

Toaster OS runs on an **Arduino Nano Every**. It controls everything in the head, and you program it from your computer over USB.

### MAX7219 LED panels

The face is made of **MAX7219 8x8 LED panels**: modules with 64 LEDs and a MAX7219 chip that drives them. A typical face uses 14:

| Part of the face | Panels per side |
| ---------------- | --------------- |
| Mouth            | 4               |
| Eye              | 2               |
| Nose             | 1               |

You can also use one centre nose panel instead of two (13 panels in total).

> [!NOTE]
> Some panels come with their pin headers loose in the bag, so you may have to solder them on before you can wire them up.

The panels are wired in **chains**: the first panel connects to the Arduino, and each panel's output pins connect to the next panel's input pins. Each chain uses 3 of the Arduino's pins, plus 5V and ground:

- **DIN** (data in)
- **CS** (chip select)
- **CLK** (clock)

A chain can have up to 16 panels. You can use one chain for the whole face, or one per side.

#### Two chains (the default)

Each side of the face is its own chain, starting at the back of the mouth and finishing at the back of the eye:

![The panel layout with two chains: the right side on pins 3, 4 and 5, and the left side on pins 6, 7 and 8](images/panels_diagram_twocon.png)

| Chain      | DIN | CS  | CLK | Order                                                                               |
| ---------- | --- | --- | --- | ----------------------------------------------------------------------------------- |
| Right side | 3   | 4   | 5   | Mouth back, mouth mid-back, mouth mid-front, mouth front, nose, eye front, eye back |
| Left side  | 6   | 7   | 8   | The same, on the left                                                               |

"Left" and "right" are as you look at the head from the front.

#### One chain

Toaster Oven also has presets for a single chain on pins 3, 4 and 5, with 14 panels or 13. Choose one on the **Panels** page to see its order.

### Power

Everything runs on 5V from a USB battery pack. Split the USB cable's 5V and ground so they feed the Arduino and the LED panels (and any extras later). Every part must share the same ground.

> [!WARNING]
> Power the head from its battery, not your computer's USB port. A computer can't supply enough power for the panels, and parts like the boop sensor stop working properly without it.

## Software

### Arduino IDE

1. Install [Arduino IDE 2](https://docs.arduino.cc/software/ide-v2).
2. Open **Tools → Board → Boards Manager**, search for **Arduino megaAVR Boards** and install it. This adds support for the Nano Every.
3. Open **Tools → Manage Libraries**, search for **FastLED** and install it. Toaster OS uses it for the side lights and the remote's lights.

New to Arduino? The [Arduino getting started guide](https://docs.arduino.cc/learn/starting-guide/getting-started-arduino#a-typical-workflow) explains the basics.

### VS Code

If you'd rather use VS Code, SparkFun's guide to [Arduino programming with the Arduino CLI and VS Code](https://learn.sparkfun.com/tutorials/efficient-arduino-programming-with-arduino-cli-and-visual-studio-code/all) shows how to set it up instead of the Arduino IDE.

### Download Toaster OS

Download the code from [GitHub](https://github.com/BadgerCode/ProtosoftToasterOS): click **Code → Download ZIP** and unzip it, or clone it with Git.

The firmware is in the `ToasterOS` folder. Open `ToasterOS/ToasterOS.ino` in the Arduino IDE.

### Configure

Toaster OS reads its settings from two files in the `ToasterOS` folder:

| File               | What it holds                                                                    |
| ------------------ | -------------------------------------------------------------------------------- |
| `Config.h`         | Pins, the order of your LED panels, and features like the side lights and remote |
| `Protogen_Faces.h` | The expressions, and which ones are used for boops, the remote and so on         |

Make them with [Toaster Oven](/configure). For now, the important part is the panels:

1. On **Panels**, pick the preset closest to your wiring.
2. Match the pins to the ones each chain is wired to.
3. Put the panels in the order they are along each chain, and tick **Upside down** for any panel mounted upside down.
4. Download both files with the buttons at the top of the page, and replace the ones in the `ToasterOS` folder.

To change them later, use **Import** to load the files you downloaded, make your changes, and download them again.

> [!TIP]
> Always download both files together, from the same version of Toaster Oven. They're designed to work together.

### Upload

1. Connect the Arduino to your computer with the USB cable.
2. Choose **Tools → Board → Arduino megaAVR Boards → Arduino Nano Every**.
3. Choose the Arduino's port under **Tools → Port**.
4. Click **Upload**.

When it's done, the face should light up.

### Check the panels

If the face looks scrambled, some panels are probably in the wrong order or upside down. Toaster OS has a debug mode to help:

1. In Toaster Oven, go to **Features → General** and set **Debug mode** to **Render numbers on all panels**. Download `Config.h` and upload again.
2. Each panel now shows its position in its chain (0, 1, 2 and so on), with a dot in its top outer corner.
3. Check every number reads the right way up, with the dot at the top. If a panel's dot is at the bottom, change **Upside down** for it on the Panels page. If the numbers are in the wrong places, fix the panel order.
4. Set **Debug mode** back to **Off** when you're done.

## Troubleshooting

- **The code won't compile after downloading new files.** Check `Config.h` and `Protogen_Faces.h` came from the same version of Toaster Oven, and that your copy of Toaster OS is up to date.
- **"Low memory available" when compiling.** The Nano Every has 6 KB of memory. If you've added lots of expressions or animation frames, remove some you don't use.
- **A panel stops updating or shows junk.** This is usually a loose wire. **Rapid panel auto-recovery** (Features → General) fixes stuck panels automatically, at a small cost to speed.

Next: [Managing expressions](/guides/toaster-os/managing-expressions).

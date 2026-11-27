---
title: Setting up
description: Wire up the LED panels, then configure Toaster OS and put it on your microcontroller.
order: 1
group: Setup
---

This guide gets a face working: the microcontroller, the LED panels, and the software. Extras like the side lights, boop sensor and remote each have their own guide under **Enabling features**.

## The electronics

### Microcontroller

Toaster OS can run on an **Arduino Nano Every** or an **ESP32** microcontroller. It controls everything in the head, and you program it from your computer over USB.

### MAX7219 LED panels

The face is made of **MAX7219 8x8 LED panels**: modules with 64 LEDs and a MAX7219 chip that drives them.

> [!NOTE]
> Some panels come with their pin headers loose in the bag, so you may have to solder them on before you can wire them up.

#### How many panels to use

A typical face uses 14 panels

| Part of the face | Panels per side |
| ---------------- | --------------- |
| Mouth            | 4               |
| Eye              | 2               |
| Nose             | 1               |

You can also use one centre nose panel instead of two (13 panels in total).\
Or use less panels (e.g. only 3 panels for the mouth).


#### Panel layout
When using 16 panels or less, you can drive the whole face off one connection.\
Power and data are passed from one panel, to the next.

::panel-layout 14-one

You will need to provide power to the chain of panels (5V power and ground).\
And you will need to plug 3 wires into your microcontroller- 
- **DIN** (data in)
- **CS** (chip select)
- **CLK** (clock)

#### Other panel layouts

The software supports a range of layouts
- Using a single nose panel
- Upside down panels
- Separate connections for the left & right sides of the face
- Wiring the panels in a different order
- Using fewer panels for the mouth

See [Toaster Oven](/configure) configure page.


### Power

Everything runs on 5V.

- The easiest option is to use standard USB battery pack
    - Use an exposed USB cable, so that you can access the voltage (red) and ground (black) wires
    - You can either buy one or cut the end off a USB cable
    - Power delivery (PD) is not required but will work
    - A 5000 mAh powerbank lasts me a few hours at max brightness (daytime) and all night at low brightness
- Use a splitter on the voltage and ground to allow multiple components to connect to it
    - E.g. a WAGO splitter or a JST Y splitter

![A USB cable from a power bank with its end cut off. The red 5V wire and black ground wire each go into a splitter, which sends a 5V and ground pair to the Arduino, LED panels, side LED strips and fan.](images/power-splitting.svg)

> [!WARNING]
> Every part must be connected to the same ground or you will get weird behaviour

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


### Next steps

If the head isn't working properly, see [Debugging](/guides/toaster-os/debugging).

Otherwise, it's time to [manage the facial expresions](/guides/toaster-os/managing-expressions) your protogen has :3.

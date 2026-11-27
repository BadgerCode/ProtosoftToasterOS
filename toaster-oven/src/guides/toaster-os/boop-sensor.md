---
title: Boop sensor
description: Detect nose boops with an IR sensor, and react with different expressions.
order: 5
group: Enabling features
---

A sensor behind the nose detects boops. The face reacts with different expressions the more times in a row it's booped, shows hearts while the nose is rubbed, and the side lights show a rainbow.

## The electronics

### The distance sensor

The boop sensor is an **IR obstacle sensor** (3 pins, 5V): a small module that detects when something is close in front of it.

> [!NOTE]
> **To do:** add a link to the sensor you used, and where it sits behind the visor.

| Sensor pin | Connect to                              |
| ---------- | --------------------------------------- |
| VCC        | 5V                                      |
| GND        | Ground, shared with the microcontroller |
| OUT        | Any free analog pin, for example A0     |

These sensors usually have their own light, which turns on when something is close. It's a quick way to check the sensor is working before blaming the code.

> [!WARNING]
> The sensor only works properly with the head running from its battery. When the microcontroller is only powered by your computer's USB port, the sensor often doesn't detect anything.

## The config

On Toaster Oven's [Features page](/configure/features), turn on **Boop sensor** and set:

- **Analog pin**: the pin the sensor's output connects to, as a number. For example, `0` for A0.
- **Boops to activate game**: how many boops in a row start the secret game.
- **Boop expressions**: what the face shows after a number of boops in a row. The highest number reached is used. By default, 1 boop shows Surprised, 3 show Angry, and 6 show X_X. With none, the face doesn't change when booped.

Download `Config.h` and `Protogen_Faces.h`, put them in the `ToasterOS` folder and upload.

## How boops are counted

- A boop counts once the sensor has seen something for a moment, and lasts until it's been gone for half a second.
- Boops in a row add up. The count resets after 4 seconds without a boop following the first one, or 7 seconds after later ones.
- Holding something in front of the sensor for more than 1.7 seconds counts as rubbing the nose, and shows the heart.

If you've set up the [remote control](/guides/toaster-os/remote-control), the **D then B** combo turns the boop sensor on and off.

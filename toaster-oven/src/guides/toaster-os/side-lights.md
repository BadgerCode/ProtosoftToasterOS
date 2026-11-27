---
title: Side lights
description: Add RGB LED strips to the sides of the head, fading between two colours.
order: 4
group: Enabling features
---

The side lights are RGB LED strips in each side panel or "headphone". They fade between two colours of your choice, and show a scrolling rainbow while the nose is being booped.

## The electronics

You need two **addressable RGB LED strips** (5V, 3 wires), one for each side. Toaster OS expects 15 LEDs per strip, but you can change that.

Each strip has three wires:

| Wire   | Connect to                                 |
| ------ | ------------------------------------------ |
| 5V     | 5V from the power supply                   |
| Ground | Ground, shared with the Arduino            |
| Data   | Pin 9 (left strip) or pin 10 (right strip) |

Both strips show the same thing, so they need the same number of LEDs.

> [!NOTE]
> **To do:** add a link to the strips you used.

## The config

On Toaster Oven's [Features page](/configure/features), turn on **Side LED strips** and set:

- **Left strip data pin** and **Right strip data pin**: the pins the strips' data wires connect to.
- **LEDs per strip**: how many LEDs each strip has.
- **Colours**: the two colours the strips fade between. The fade takes about 0.6 seconds each way.

The strips match the face's brightness, set on **Features → General**.

Download `Config.h`, put it in the `ToasterOS` folder and upload.

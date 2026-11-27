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

| Wire   | Connect to                                                                          |
| ------ | ----------------------------------------------------------------------------------- |
| 5V     | 5V from the power supply                                                            |
| Ground | Ground, shared with the microcontroller                                             |
| Data   | Any free pin, one for each strip (for example, 9 for the left and 10 for the right) |

Both strips show the same thing, so they need the same number of LEDs.

I used [this WS2812 RGB LED strip](https://www.amazon.co.uk/dp/B01CDTED80), which has 60 LEDs, and cut it into pieces of 15 LEDs, one for each side. You can cut these strips between any two LEDs, across the copper pads.

The strip comes with wires on one end, but you'll need to solder three wires onto the other pieces: ground, data and 5V.

![Black, blue and red wires soldered onto the GND, DIN and +5V pads at the end of a cut piece of LED strip](images/led_strip_wires.jpg)

> [!TIP]
> Solder the wires to the end with the **DIN** (data in) pad. The arrows printed on the strip point away from it, the way the data flows.

## The config

On Toaster Oven's [Features page](/configure/features), turn on **Side LED strips** and set:

- **Left strip data pin** and **Right strip data pin**: the pins the strips' data wires connect to.
- **LEDs per strip**: how many LEDs each strip has.
- **Colours**: the two colours the strips fade between. The fade takes about 0.6 seconds each way.

The strips match the face's brightness, set on **Features → General**.

Download `Config.h`, put it in the `ToasterOS` folder and upload.

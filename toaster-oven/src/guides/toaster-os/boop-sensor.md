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

I used [these IR obstacle avoidance sensors](https://www.amazon.co.uk/dp/B07PY3CVSV). Their long shape helps them fit in the nose of the protogen.

- Make sure to get one that works with 5V.
- Buy a few. One of mine died while I was fitting it.

| Sensor pin | Connect to                              |
| ---------- | --------------------------------------- |
| VCC        | 5V                                      |
| GND        | Ground, shared with the microcontroller |
| OUT        | Any free analog pin, for example A0     |

These sensors usually have their own light, which turns on when something is close. It's a quick way to check the sensor is working before blaming the code.\

You may want to cover the indicator light with some blu tack or tape to darken it; it's quite bright inside the head.

> [!WARNING]
> The sensor only works properly with the head running from its battery. When the microcontroller is only powered by your computer's USB port, the sensor often doesn't detect anything.

### Fitting it in the nose

I drilled two holes in the nose, so the sensor's emitter and receiver bulbs fit through.

![An IR obstacle sensor held next to the nose, where another sensor's two bulbs poke through holes drilled between the LED panels](images/boop_sensor.jpg)

The sensor can work through the visor, but it needs a small gap between the sensor and the visor. The small dial on the sensor sets how far away it detects things, if it needs adjusting.

### Sunlight

> [!WARNING]
> The sensor doesn't work well in sunlight. It helps to be able to turn it off during the day with the [remote control](/guides/toaster-os/remote-control), using the **D then B** combo by default.

If you don't have a remote, you can re-use the remote control system with a simple push button, hidden somewhere on the head:

1. Wire the button so it connects a free pin to 5V while it's pressed. Add a resistor (around 10kΩ) from that pin to ground, so it reads as off when the button isn't pressed.
2. On Toaster Oven's [Features page](/configure/features), turn on **Remote control**, set **Button A pin** to the button's pin, and set the **A then A** combo to **Turn the boop sensor on or off**.
3. Connect the pins for buttons B, C and D to ground, so they don't pick up stray presses.
4. Press the button twice to turn the boop sensor on or off.

### Other sensors

Alternatively, you could use the [Adafruit APDS9960](https://learn.adafruit.com/adafruit-apds9960-breakout) sensor, which may be less affected by sunlight than IR obstacle sensors. It also has a bunch of extra features, like gesture and colour sensing.

Toaster OS reads the boop sensor from an analog pin, and the APDS9960 connects over I2C instead, so it needs some changes to the code. See [Adding custom features](/guides/toaster-os/adding-custom-features).

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

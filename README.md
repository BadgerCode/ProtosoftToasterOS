# Protosoft Toaster OS

Software to control Arduino-based protogen heads, using 8x8 MAX7219 LED matrices.

![Protogen preview](/Docs/Images/protogen_head.jpg)

<br><br>

# Features

- Controls a protogen head using 8x8 LED matrices to render eyes, nose & mouth on each side of a face
- Supports light strips in each ear/headphones
- Nose "boop" detection & animation
- Blinking animations
- Different expressions, with simple animations
- Remote control support

<br><br>

# Setup
_A better guide is coming soon!_

## Dependencies

- [FastLED](https://fastled.io/) (not included; installed via Arduino IDE)

<br>

## Configuring

Consider using the [Toaster Oven](https://toaster.badgercode.co.uk/) to easily configure your protogen!

_If not, for examples and more info, see the [Configuration wiki page](https://github.com/BadgerCode/ProtosoftToasterOS/wiki/Configuration)._


<br><br><br>

# Electronics
Here are the electronics used. Better guides are coming soon.

- For both sides of the face
   - Mouth: 4 x MAX7219 8x8 LED martices
   - Eyes: 2 x MAX7219 8x8 LED martices
   - Nose: 1 x MAX7219 8x8 LED martices
- For both sides of the head
   - A strip of 15 RGB LEDs (5V, 3-pin)
- Boop sensor
   - A 3-pin 5V IR obstacle detection sensor
- A 5V Noctua fan (40x40x10mm) for ventilation (not connected to the arduino)
- A USB cable and voltage + ground splitters to power everything via a USB battery pack

<br><br>

# Arduino Basics

- [Docs](https://docs.arduino.cc/learn/starting-guide/getting-started-arduino#a-typical-workflow)
- [Arduino IDE 2](https://docs.arduino.cc/software/ide-v2)
- [Example projects](https://github.com/BadgerCode/Arduino-Test-Projects)

## Nano Every Pin out

![nano every pin layout](https://content.arduino.cc/assets/Pinout-NANOevery_latest.png)


<br><br>

# Additional
For information on changing the config management website, see the [Toaster Oven README](./toaster-oven/README.md).


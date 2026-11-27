---
title: Remote control
description: Change expressions and settings with a 4-button remote.
order: 6
group: Enabling features
---

A 4-button remote lets you change the expression, brightness and more while wearing the head. You press two buttons, one after the other, and each combo does something different.

## The electronics

### The remote control

Toaster OS works with a remote receiver that has four outputs, one for each button, which turn on while the button is pressed. I used [this remote control](https://www.amazon.co.uk/dp/B0DCZD1TJF).

> [!NOTE]
> **To do:** describe the receiver's pins, and how it's powered.

Connect the receiver's four outputs to the Arduino:

| Button | Pin |
| ------ | --- |
| A      | 18  |
| B      | 17  |
| C      | 16  |
| D      | 15  |

### Output lights (optional)

Two addressable RGB LEDs on **pin 11** show what's happening, which is handy when you can't see the face yourself:

| Colour | Button |
| ------ | ------ |
| Green  | A      |
| Red    | B      |
| Purple | C      |
| Yellow | D      |

While you press a combo, they show the buttons pressed. Otherwise they show the combo for the current expression, or blue while the nose is being booped.

## The config

On Toaster Oven's [Features page](/configure/features), turn on **Remote control** and set:

- **Button A pin** to **Button D pin**: the pins the receiver's outputs connect to.
- **Output LEDs** and **Output LED pin**, if you've fitted the output lights.
- **Button combos**: what each combo does. Each can show an expression, go back to the default expression, cycle the brightness (0%, 25%, 50%, 100%), or turn the boop sensor on or off.

Press the two buttons within 3 seconds of each other. These are the default combos:

| First button | Then A             | Then B             | Then C    | Then D  |
| ------------ | ------------------ | ------------------ | --------- | ------- |
| A            | Default expression | Blep               | Sleepy    | Excited |
| B            | Angry              | Sad                | Surprised | X_X     |
| C            | Heart              | BoyKisser          | Smirk     | Spiral  |
| D            | Cycle brightness   | Boop sensor on/off | Nothing   | AmongUs |

Download `Config.h` and `Protogen_Faces.h`, put them in the `ToasterOS` folder and upload.

---
title: Remote control
description: Change expressions and settings with a 4-button remote.
order: 6
group: Enabling features
---

A remote control allows for a lot more expression while wearing the head!\
You can change your facial expression to react to the situation around you.

It also can adjust the brightness of your lights, toggle your boop sensor and more, all without taking your paws off!

## The electronics (easy)

I have gone with the easy option: **a pre-made remote and receiver system**.

I used [this remote control](https://www.amazon.co.uk/dp/B0DCZD1TJF): a 4-button **EV1527** remote, with a plastic case and its own battery, and an **RX480E** receiver.

![A four button remote with a receiver chip](images/remote_control.jpg)

It's very cheap and can be set up in less than 10 minutes if you have everything ready.

The remote control feature uses a 4 button remote, but supports up to 16 options by requiring one button press followed by another to pick an option.\
E.g. Button A, then Button B

Alternatively, **you could build your own remote control system**.

### The receiver

Toaster OS works with a remote receiver that has four outputs, one for each button, which turn on while the button is pressed.

The receiver runs on 3.3V to 5V, and has a pin for power, a pin for ground, and one output pin for each button:

| Receiver pin | Connect to                    |
| ------------ | ----------------------------- |
| +V           | 5V                            |
| GND          | Ground                        |
| D0 to D3     | The microcontroller, as below |
| VT           | Nothing                       |

Connect the receiver's four outputs to the microcontroller:

| Button | Receiver pin | Example pin |
| ------ | ------------ | ----------- |
| A      | D3           | 18          |
| B      | D2           | 17          |
| C      | D1           | 16          |
| D      | D0           | 15          |

> [!WARNING]
> You must first "pair" your remote to the receiver, so that they work together.\
> The kit should come with instructions on how to do this.\
> Typically this will involve pressing the learning button on the receiver once, then press a button on the remote to pair it.\
> You must also make sure the receiver is in **momentary** mode, so each output is only on while its button is held.

### Output lights (optional)

Two addressable RGB LEDs, on any free pin (for example, **pin 11**), show what's happening, which is handy when you can't see the face yourself:

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

Download `Config.h` and `Protogen_Faces.h`, put them in the `ToasterOS` folder and upload.

### Default remote options

| First button | Then A             | Then B             | Then C    | Then D  |
| ------------ | ------------------ | ------------------ | --------- | ------- |
| A            | Default expression | Blep               | Sleepy    | Excited |
| B            | Angry              | Sad                | Surprised | X_X     |
| C            | Heart              | BoyKisser          | Smirk     | Spiral  |
| D            | Cycle brightness   | Boop sensor on/off | Nothing   | AmongUs |

A is used for H**a**ppy feelings, B for **B**ad feelings, C for Sau**c**y expressions and D for **D**ebug stuff.

Press the two buttons within 3 seconds of each other.

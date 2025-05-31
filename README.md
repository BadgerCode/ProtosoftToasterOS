# Protosoft Toaster OS
Software to control Arduino-based protogen heads, using 8x8 MAX7219 LED matrices.

![Protogen preview](https://i.imgur.com/5MpisuR.png)


<br><br>

# Features
* Two halves of a protogen face, synchronised (eyes, nose, mouth)
* Lights in each ear/headphones
* Nose "boop" detection & animation
* Blinking animations
* Different expressions
* A moving face


<br><br>

# Setup


## Dependencies
* [FastLED](https://fastled.io/) (not included; installed via Arduino IDE)
* [LedControl](https://github.com/wayoda/LedControl) (included)

<br>

## Electronics
* Each half of the face
    * 4 8x8 MAX7219 LED matrices 
        * On a single board
    * Daisy chained to a single 8x8 MAX7219 LED matrix
    * Daisy chained to 2 8x8 MAX7219 LED matrices
        * On a single board
    * A strip of 15 RGB LEDs (5V, 3-pin)
* Boop sensor
    * A 3-pin 5V IR proximity distance sensor
* A 5V Noctua fan (40x40x10mm) for ventilation (not connected to the arduino)
* A USB cable and voltage + ground splitters to power everything via a USB battery pack

<br>

## Configuring
The configuration can be found in `ToasterOS/Config.h`

### Arudino Pin Layout

TODO: Diagram (left vs right, the different panel names, IR sensor, pins used on the arduino, VIN/GND)


### Face LEDs Configuration
These are configured in the `FaceConfig` class.

The default set up is for faces with separate connections for the left and right half.<br>
But there is an example below for people with a single connection for all LED panels.

<br>

#### Example: Single connection for all LED panels

TODO: DIAGRAM

```cpp
NumConnections = 1;

Connections = new FaceLEDConnection[NumConnections]{
    {
        // Pin configuration
        .PIN_DataIn = 11,
        .PIN_CS = 9,
        .PIN_CLK = 13,

        // Define the order of the panels
        .NumPanels = 14,
        .Panels = {
            // Start: Right mouth back
            { .PanelType = PANEL_RIGHT_MOUTH_BACK, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_RIGHT_MOUTH_MID_BACK, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_RIGHT_MOUTH_MID_FRONT, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_RIGHT_MOUTH_FRONT, .FlipX = false, .FlipY = false },

            // -> Left mouth front
            { .PanelType = PANEL_LEFT_MOUTH_FRONT, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_LEFT_MOUTH_BACK, .FlipX = false, .FlipY = false },

            // -> Left eye back
            { .PanelType = PANEL_LEFT_EYE_BACK, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_LEFT_EYE_FRONT, .FlipX = false, .FlipY = false },

            // -> Left nose
            { .PanelType = PANEL_LEFT_NOSE, .FlipX = false, .FlipY = false },

            // -> Right nose
            { .PanelType = PANEL_RIGHT_NOSE, .FlipX = false, .FlipY = false },

            // -> Right eye front
            { .PanelType = PANEL_RIGHT_EYE_FRONT, .FlipX = false, .FlipY = false },
            { .PanelType = PANEL_RIGHT_EYE_BACK, .FlipX = false, .FlipY = false },
        },
    }
};
```

<br>

### Debugging issues with the face config
If the face isn't rendering properly, there are some steps you can take

1. In `ToasterOS/Config.h`, set `DEBUG_MODE` to 3
    * This will render 7s to every LED panel
2. If one of the panels is displaying the 7 the wrong way or upside down, update the config
    * In `ToasterOS/Config.h` under `FaceConfig`
    * Find the right panel (see the diagrams above)
    * If the 7 is facing the wrong way, change the `FlipX` value
    * If the 7 is upside down, change the `FlipY` value
3. In `ToasterOS/Config.h`, set `DEBUG_MODE` back to 0
4. If the face still looks wrong, check that you have added the panels to the config in the right order
    * They should be added in the same order they are wired


<br><br><br>

# Creating new faces
1. Open the file `face-demo.html` in your browser
2. Draw your new face
3. Add it to the code
    * `ToasterOS/Protogen_Faces.h`
    * Add in your `FaceExpression` struct
    * Give it a good variable name
    * Increase the variable `NumSpecialFaces` by 1
    * Add `&YourVariableName` to the `SpecialExpressions` array (see others for examples)

<br><br>


# Arduino Basics
* [Docs](https://docs.arduino.cc/learn/starting-guide/getting-started-arduino#a-typical-workflow)
* [Arduino IDE 2](https://docs.arduino.cc/software/ide-v2)
* [Example projects](https://github.com/BadgerCode/Arduino-Test-Projects)


## Nano Every Pin out
![nano every pin layout](https://content.arduino.cc/assets/Pinout-NANOevery_latest.png)

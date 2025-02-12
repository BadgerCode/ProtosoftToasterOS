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


### Arudino Pin Layout
LEFT = The protogen's left (your right)<br>
RIGHT = The protogen's right (your left)

* VIN/GND: USB cable power connection
    * ⚠⚠⚠ Do not plug the power connection into the wrong pins/the wrong way around
    * This can kill the board if left for too long
* Analog 0: IR sensor data connection
* Digital 3: LEFT LED matrices data in (DIN)
* Digital 4: LEFT LED matrices CS
* Digital 5: LEFT LED matrices CLK
* Digital 6: RIGHT LED matrices data in (DIN)
* Digital 7: RIGHT LED matrices CS
* Digital 8: RIGHT LED matrices CLK
* Digital 9: LEFT LED matrices data in (DIN)
* Digital 10: RIGHT LED matrices data in (DIN)

<br>

### Nano Every
![nano every pin layout](https://content.arduino.cc/assets/Pinout-NANOevery_latest.png)



<br><br>


# Arduino Basics
* [Docs](https://docs.arduino.cc/learn/starting-guide/getting-started-arduino#a-typical-workflow)
* [Arduino IDE 2](https://docs.arduino.cc/software/ide-v2)
* [Example projects](https://github.com/BadgerCode/Arduino-Test-Projects)




#include <FastLED.h>
#include "LedControl.h"
#include "Protogen_Faces.h"


// LED strips
#define LEFT_LEDSTRIP_DATA_PIN 9
#define RIGHT_LEDSTRIP_DATA_PIN 10
#define LEDSTRIP_NUM_LEDS 15

CRGB LEDSTRIP_LEDS[LEDSTRIP_NUM_LEDS];



// General IO
int ButtonPin = 0;
int TemperatureAnalogPin = 1;
int DistanceAnalogPin = 0;

int Brightness = 6;  // 0 - 15




// Left face
int PIN_LEFT_DIN = 3;
int PIN_LEFT_CS = 4;
int PIN_LEFT_CLK = 5;
int LEFT_NUM_PANELS = 7;

LedControl LEFT_LEDs = LedControl(PIN_LEFT_DIN, PIN_LEFT_CLK, PIN_LEFT_CS, LEFT_NUM_PANELS);



// Right face
int PIN_RIGHT_DIN = 6;
int PIN_RIGHT_CS = 7;
int PIN_RIGHT_CLK = 8;
int RIGHT_NUM_PANELS = 7;

LedControl RIGHT_LEDs = LedControl(PIN_RIGHT_DIN, PIN_RIGHT_CLK, PIN_RIGHT_CS, RIGHT_NUM_PANELS);



// Panel indexes (left and right have the same indexes)
int PANEL_MOUTH1 = 3;
int PANEL_MOUTH2 = 2;
int PANEL_MOUTH3 = 1;
int PANEL_MOUTH4 = 0;
int PANEL_NOSE = 4;
int PANEL_EYE1 = 5;
int PANEL_EYE2 = 6;






void setup() {
  // Debug output on serial port; Used for Serial.println("blah blah");
  // Serial.begin(9600);

  // //pinMode(ButtonPin, INPUT);
  pinMode(PIN_LEFT_CS, OUTPUT);
  pinMode(PIN_RIGHT_CS, OUTPUT);

  for (int address = 0; address < LEFT_NUM_PANELS; address++) {
    LEFT_LEDs.shutdown(address, false);           // Disable power saving
    LEFT_LEDs.setIntensity(address, Brightness);  // Set brightness 0-15
    LEFT_LEDs.clearDisplay(address);              // Turn all LEDs off
  }

  for (int address = 0; address < RIGHT_NUM_PANELS; address++) {
    RIGHT_LEDs.shutdown(address, false);           // Disable power saving
    RIGHT_LEDs.setIntensity(address, Brightness);  // Set brightness 0-15
    RIGHT_LEDs.clearDisplay(address);              // Turn all LEDs off
  }


  // LED strips
  FastLED.addLeds<NEOPIXEL, LEFT_LEDSTRIP_DATA_PIN>(LEDSTRIP_LEDS, LEDSTRIP_NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, RIGHT_LEDSTRIP_DATA_PIN>(LEDSTRIP_LEDS, LEDSTRIP_NUM_LEDS);
}

// Face movement
int Face_OffsetX = 0;  // OffsetX doesn't work, because the LEDs from one panel don't wrap to the next panel
int Face_OffsetY = 0;
int Face_OffsetY_Dir = 1;
int OffsetDelay = 170;
unsigned long NextOffsetShift = millis() + OffsetDelay;

// Blinking
int MinBlinkWait = 2000;
unsigned long NextBlink = millis() + random(100) + MinBlinkWait;
int BlinkDurationMs = 100;

// Expression faces
int Special_Face_Index = -1;
int MinSpecialFaceWait = 10000;
unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
int SpecialFaceDurationMs = 5000;

// Glitch mode
int MinGlitchModeWait = 300000;
unsigned long NextGlitchMode = millis() + random(15000) + MinGlitchModeWait;
int GlitchModeDurationMs = 8000;

// Debug
unsigned long NextPrint = millis() + 100;


// LED Strips
uint8_t ledStripHue = 0;
unsigned long NextLEDStripUpdate = millis();


bool heartFaceRendered = false;


void loop() {
  unsigned long curTime = millis();

  // Check if it's time to stop blinking
  if (curTime >= NextBlink + BlinkDurationMs) {
    NextBlink = millis() + random(100) + MinBlinkWait;
  }

  // Time for a special face?
  if (curTime >= NextSpecialFace && Special_Face_Index == -1) {
    Special_Face_Index = random(0, NumSpecialFaces);
  }

  // Time to return to the neutral face?
  if (curTime >= NextSpecialFace + SpecialFaceDurationMs) {
    NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
    Special_Face_Index = -1;
  }

  // Disable glitch mode
  if (curTime >= NextGlitchMode + GlitchModeDurationMs) {
    NextGlitchMode = millis() + random(15000) + MinGlitchModeWait;
  }

  // Offset the face to do a basic animation
  if (curTime >= NextOffsetShift) {
    NextOffsetShift = curTime + OffsetDelay;

    Face_OffsetY += Face_OffsetY_Dir;

    if (Face_OffsetY >= 1) {
      Face_OffsetY_Dir = -1;
    } else if (Face_OffsetY <= -1) {
      Face_OffsetY_Dir = 1;
    }
  }




  // Determine expression
  struct FaceExpression facialExpression = Face_Neutral;

  if (Special_Face_Index != -1) {
    facialExpression = *(SpecialExpressions[Special_Face_Index]);
  }

  // Touch sensor
  int distance = getDistance();
  bool touchNearby = (distance < 500);
  if (touchNearby) {
    facialExpression = Face_Heart;
    Face_OffsetY = 0; // TODO: temporary optimisation for expensive face re-rendering
  }


  // Glitch mode
  bool glitchModeActive = (curTime >= NextGlitchMode);
  int rowMappings[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

  if (glitchModeActive) {
    for (int i = 1; i < 8; i++) {
      if (random(0, 3) == 0) {
        int newValue = rowMappings[i - 1];
        rowMappings[i - 1] = rowMappings[i];
        rowMappings[i] = newValue;
      }
    }
  }


  if(!heartFaceRendered) {
    // TODO: Instead of re-rendering the entire face each loop, only re-render rows that have changed
    // Store one half of the face as a series of rows (numbers)
    // Track the previous and current face
    // Render any rows that are different
    // Expression + Offset -> LED Rows -> Render different rows

    // Mouth
    renderLeftAndRightPanel(PANEL_MOUTH1, (facialExpression).Mouth[0], false, Face_OffsetX, Face_OffsetY, rowMappings);
    renderLeftAndRightPanel(PANEL_MOUTH2, (facialExpression).Mouth[1], false, Face_OffsetX, Face_OffsetY, rowMappings);
    renderLeftAndRightPanel(PANEL_MOUTH3, (facialExpression).Mouth[2], false, Face_OffsetX, Face_OffsetY, rowMappings);
    renderLeftAndRightPanel(PANEL_MOUTH4, (facialExpression).Mouth[3], false, Face_OffsetX, Face_OffsetY, rowMappings);

    // Nose
    renderLeftAndRightPanel(PANEL_NOSE, (facialExpression).Nose[0], true, 0, 0, rowMappings);


    // Eyes
    bool shouldBlink = (curTime >= NextBlink);
    EyeFrame* eyes = shouldBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);
    renderLeftAndRightPanel(PANEL_EYE1, (*eyes)[0], true, Face_OffsetX, Face_OffsetY, rowMappings);
    renderLeftAndRightPanel(PANEL_EYE2, (*eyes)[1], true, Face_OffsetX, Face_OffsetY, rowMappings);
  }

  heartFaceRendered = touchNearby; // TODO: temporary optimisation for expensive face re-rendering

  // LED strips
  if(touchNearby) {
    if(NextLEDStripUpdate <= curTime) {
      for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
        LEDSTRIP_LEDS[i] = CHSV(ledStripHue, 255, 255);
        ledStripHue += 16;
      }

      NextLEDStripUpdate = curTime + 40;
    }
  }
  else {
    fill_solid(LEDSTRIP_LEDS, LEDSTRIP_NUM_LEDS, CRGB(0, 0, 255));
  }
  FastLED.show();


  // Debug print code
  // Make sure to enable Serial.begin(9600); in setup
  // if (curTime >= NextPrint) {
  //   NextPrint = millis() + 20;
  //   Serial.println(distance);
  // }
}



void renderLeftAndRightPanel(int panelIndex, byte data[], bool isReversed, int offsetX, int offsetY, int rowMappings[]) {
  renderPanel(LEFT_LEDs, panelIndex, data, isReversed, isReversed, offsetX, offsetY, rowMappings);
  renderPanel(RIGHT_LEDs, panelIndex, data, isReversed, !isReversed, offsetX, offsetY * -1, rowMappings);
}


void renderPanel(LedControl faceLEDs, int panelIndex, byte data[], bool isReversed, bool isUpsideDown, int offsetX, int offsetY, int rowMappings[]) {
  for (int row = 0; row < 8; row++) {
    int rowIndex = rowMappings[row] + offsetY;
    if (rowIndex < 0 || rowIndex >= 8) {
      faceLEDs.setRow(panelIndex, row, 0);
      continue;
    }

    byte rowData = 0;
    int rowDataIndex = isUpsideDown ? (7 - rowIndex) : rowIndex;
    if (isReversed) {
      rowData = reverse(data[rowDataIndex] << offsetX);
    } else {
      rowData = data[rowDataIndex] << offsetX;
    }

    faceLEDs.setRow(panelIndex, row, rowData);
  }
}

byte reverse(byte b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

int getDistance() {
  return 1023 - analogRead(DistanceAnalogPin);  // Higher value = further away
}


// /// Returns a two decimal number for the temperature in celcius
// // float getTemperature() {
// //   float voltage = analogRead(TemperatureAnalogPin) * 0.004882814;
// //   return (voltage - 0.5) * 100;
// // }

// // bool isButtonDown() {
// //   return digitalRead(ButtonPin) == 0;  // Invert it, so that it's 0 when off and 1 when on
// // }
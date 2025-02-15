#include <FastLED.h>
#include "LedControl.h"
#include "Protogen_Faces.h"

#define DEBUG_MODE 1


// LED strips
#define LEFT_LEDSTRIP_DATA_PIN 9
#define RIGHT_LEDSTRIP_DATA_PIN 10
#define LEDSTRIP_NUM_LEDS 15

CRGB LEDSTRIP_LEDS[LEDSTRIP_NUM_LEDS];



// General IO
int ButtonPin = 0;
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
  if (DEBUG_MODE) {
    Serial.begin(9600);
  }

  // pinMode(ButtonPin, INPUT);
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
int Face_OffsetY = 0;
int Face_OffsetY_Dir = 1;
int OffsetDelay = 200;
unsigned long NextOffsetShift = millis() + OffsetDelay;

// Blinking
int MinBlinkWait = 2000;
unsigned long NextBlink = millis() + random(100) + MinBlinkWait;
int BlinkDurationMs = 200;

// Expression faces
int Special_Face_Index = -1;
int MinSpecialFaceWait = 10000;
unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
int SpecialFaceDurationMs = 5000;

// LED Strips
bool HueShiftForward = true;
uint8_t LEDStripAnimationOffset = 0;
unsigned long NextLEDStripUpdate = millis();

// Booping
bool BeingBooped = false;
unsigned long BoopHoldStarted = 0;
int MinBoopHoldForTriggerMs = 300;
unsigned long BoopLastDetected = 0;
int MaxBoopRetainAfterStopMs = 2000;


// Debug
unsigned long FrameDuration_NextPrint = millis() + 20;
unsigned int FrameDuration_MaxDuration = 0;
unsigned long FPSCOUNT_CountingStarted = 0;
unsigned int FPSCOUNT_Iterations = 0;


void loop() {
  unsigned long curTime = millis();

  // Time to stop blinking
  if (timeSince(NextBlink) >= BlinkDurationMs) {
    NextBlink = millis() + random(100) + MinBlinkWait;
  }

  // Blink
  bool shouldBlink = (curTime >= NextBlink);


  // Time for a special face?
  if (curTime >= NextSpecialFace && Special_Face_Index == -1) {
    Special_Face_Index = random(0, NumSpecialFaces);
  }

  // Time to return to the neutral face?
  if (curTime >= NextSpecialFace + SpecialFaceDurationMs) {
    NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
    Special_Face_Index = -1;
  }


  // Touch sensor
  if (getDistance() < 500) {
    if (!BeingBooped && BoopHoldStarted == 0) {
      BoopHoldStarted = millis();
    }

    BoopLastDetected = millis();
  } else {
    BoopHoldStarted = 0;
  }

  // Determine if we're being booped
  BeingBooped = BoopLastDetected > 0                                           // Don't trigger on reboot
                && (timeSince(BoopHoldStarted) >= MinBoopHoldForTriggerMs)     // Avoid accidental short detection
                && (timeSince(BoopLastDetected) <= MaxBoopRetainAfterStopMs);  // Retain the boop for some time after detection stops


  // Offset the face up & down to do a basic animation
  if (curTime >= NextOffsetShift) {
    NextOffsetShift = curTime + (BeingBooped ? OffsetDelay * 1.5 : OffsetDelay);

    Face_OffsetY += Face_OffsetY_Dir;

    // Check if it's time to reverse direction
    if (abs(Face_OffsetY) >= 1) Face_OffsetY_Dir *= -1;
  }


  // Determine expression
  struct FaceExpression facialExpression = Face_Neutral;
  EyeFrame* eyes = shouldBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);

  if (BeingBooped) {
    facialExpression = Face_Heart;
  } else if (Special_Face_Index != -1) {
    facialExpression = *(SpecialExpressions[Special_Face_Index]);
  }


  // Render the face
  // Mouth
  updateLeftAndRightPanel(PANEL_MOUTH1, (facialExpression).Mouth[0], false, Face_OffsetY);
  updateLeftAndRightPanel(PANEL_MOUTH2, (facialExpression).Mouth[1], false, Face_OffsetY);
  updateLeftAndRightPanel(PANEL_MOUTH3, (facialExpression).Mouth[2], false, Face_OffsetY);
  updateLeftAndRightPanel(PANEL_MOUTH4, (facialExpression).Mouth[3], false, Face_OffsetY);

  // Nose
  updateLeftAndRightPanel(PANEL_NOSE, (facialExpression).Nose[0], true, 0);

  // Eyes
  updateLeftAndRightPanel(PANEL_EYE1, (*eyes)[0], true, Face_OffsetY);
  updateLeftAndRightPanel(PANEL_EYE2, (*eyes)[1], true, Face_OffsetY);

  processRenderQueue();



  // LED strips
  if (NextLEDStripUpdate <= curTime) {
    if (BeingBooped) {
      // RGB scrolling
      for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
        LEDSTRIP_LEDS[i] = CHSV(LEDStripAnimationOffset + (i * 5), 255, 255);
      }

      LEDStripAnimationOffset = (LEDStripAnimationOffset + 2) % 255;
    } else {
      // Pulsating blue
      int minShift = 170;
      int maxShift = 250;
      LEDStripAnimationOffset = LEDStripAnimationOffset + (HueShiftForward ? 2 : -2);

      if (LEDStripAnimationOffset < minShift) {
        LEDStripAnimationOffset = minShift;
        HueShiftForward = true;
      } else if (LEDStripAnimationOffset > maxShift) {
        LEDStripAnimationOffset = maxShift;
        HueShiftForward = false;
      }

      for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
        LEDSTRIP_LEDS[i] = CHSV(160, 255, LEDStripAnimationOffset);
      }
    }

    NextLEDStripUpdate = curTime + 15;
    FastLED.show();
  }


  // Debug print code
  if (DEBUG_MODE) {

    // Count FPS
    FPSCOUNT_Iterations++;
    if (timeSince(FPSCOUNT_CountingStarted) >= 1000) {
      Serial.println(FPSCOUNT_Iterations);
      FPSCOUNT_Iterations = 0;
      FPSCOUNT_CountingStarted = millis();
    }

    // Display max frame duration
    // unsigned int duration = millis() - curTime;
    // if (duration > FrameDuration_MaxDuration) FrameDuration_MaxDuration = duration;

    // if (curTime >= FrameDuration_NextPrint) {
    //   FrameDuration_NextPrint = millis() + 10;

    //   Serial.println(FrameDuration_MaxDuration);
    //   FrameDuration_MaxDuration = 0;
    // }
  }
}




// Render functions
void updateLeftAndRightPanel(int panelIndex, byte data[], bool isReversed, int offsetY) {
  updatePanel(true, panelIndex, data, isReversed, isReversed, offsetY);
  updatePanel(false, panelIndex, data, isReversed, !isReversed, offsetY * -1);
}



void updatePanel(bool isLeft, int panelIndex, byte data[], bool isReversed, bool isUpsideDown, int offsetY) {
  for (int row = 0; row < 8; row++) {
    int rowIndex = row + offsetY;
    if (rowIndex < 0 || rowIndex >= 8) {
      updateRowIfDifferent(isLeft, panelIndex, row, 0);
      continue;
    }

    byte rowData = 0;
    int rowDataIndex = isUpsideDown ? (7 - rowIndex) : rowIndex;
    if (isReversed) {
      rowData = reverse(data[rowDataIndex]);
    } else {
      rowData = data[rowDataIndex];
    }

    updateRowIfDifferent(isLeft, panelIndex, row, rowData);
  }
}

// 2 sides, 7 panels, 8 rows per panel
byte PreviousOutputs[2][7][8];
bool RequiresRendering[2][7][8];
void updateRowIfDifferent(bool isLeft, int panelIndex, int row, byte output) {
  // If the output hasn't changed, do nothing
  byte previousOutput = PreviousOutputs[isLeft ? 0 : 1][panelIndex][row];
  if (previousOutput == output) return;

  // Update previous output to the new output
  PreviousOutputs[isLeft ? 0 : 1][panelIndex][row] = output;
  RequiresRendering[isLeft ? 0 : 1][panelIndex][row] = true;

  // Rendering is handled by the render queue- processRenderQueue()
}


unsigned int nextRenderSection = 0;  // 0 = eyes, 1 = nose, 2 = mouth
void processRenderQueue() {
  int sectionsRendered = 0;
  int sectionsChecked = 0;


  // Render 1 section a time (eyes, nose, mouth) and avoid infinite loops
  while (sectionsRendered < 1 && sectionsChecked < 3) {
    // Determine the panels, based on the section
    int firstPanel = -1;
    int lastPanel = -1;

    if (nextRenderSection == 0) {  // eyes
      firstPanel = PANEL_EYE1;
      lastPanel = PANEL_EYE2;
    } else if (nextRenderSection == 1) {  // nose
      firstPanel = PANEL_NOSE;
      lastPanel = PANEL_NOSE;
    } else if (nextRenderSection == 2) {  // mouth
      firstPanel = PANEL_MOUTH4;
      lastPanel = PANEL_MOUTH1;
    } else {  // something's gone wrong
      nextRenderSection = 0;
      break;
    }


    // Check if there's anything to render
    bool anyPanelsRendered = false;
    for (int panel = firstPanel; panel <= lastPanel; panel++) {
      for (int row = 0; row < 8; row++) {
        // Render left
        bool shouldRenderLeft = RequiresRendering[0][panel][row];

        if (shouldRenderLeft) {
          byte output = PreviousOutputs[0][panel][row];
          LEFT_LEDs.setRow(panel, row, output);

          RequiresRendering[0][panel][row] = false;
          anyPanelsRendered = true;
        }

        // Render right
        bool shouldRenderRight = RequiresRendering[1][panel][row];

        if (shouldRenderRight) {
          byte output = PreviousOutputs[1][panel][row];
          RIGHT_LEDs.setRow(panel, row, output);

          RequiresRendering[1][panel][row] = false;
          anyPanelsRendered = true;
        }
      }
    }

    sectionsChecked++;
    if (anyPanelsRendered) sectionsRendered++;

    nextRenderSection++;
    if (nextRenderSection > 2) nextRenderSection = 0;
  }
}


// utility functions

byte reverse(byte b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

int getDistance() {
  return 1023 - analogRead(DistanceAnalogPin);  // Higher value = further away
}

unsigned long timeSince(unsigned long previousTime) {
  return millis() - previousTime;
}


// // bool isButtonDown() {
// //   return digitalRead(ButtonPin) == 0;  // Invert it, so that it's 0 when off and 1 when on
// // }
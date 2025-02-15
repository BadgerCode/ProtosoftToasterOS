#include <FastLED.h>
#include "LedControl.h"
#include "Protogen_Faces.h"
#include "FaceRender.h"

#define DEBUG_MODE 0


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

LedControl LEFT_LEDs = LedControl(PIN_LEFT_DIN, PIN_LEFT_CLK, PIN_LEFT_CS, FACE_PANEL_COUNT);



// Right face
int PIN_RIGHT_DIN = 6;
int PIN_RIGHT_CS = 7;
int PIN_RIGHT_CLK = 8;

LedControl RIGHT_LEDs = LedControl(PIN_RIGHT_DIN, PIN_RIGHT_CLK, PIN_RIGHT_CS, FACE_PANEL_COUNT);










void setup() {
  // Debug output on serial port; Used for Serial.println("blah blah");
  if (DEBUG_MODE) {
    Serial.begin(9600);
  }

  // pinMode(ButtonPin, INPUT);
  pinMode(PIN_LEFT_CS, OUTPUT);
  pinMode(PIN_RIGHT_CS, OUTPUT);

  for (int address = 0; address < FACE_PANEL_COUNT; address++) {
    LEFT_LEDs.shutdown(address, false);           // Disable power saving
    LEFT_LEDs.setIntensity(address, Brightness);  // Set brightness 0-15
    LEFT_LEDs.clearDisplay(address);              // Turn all LEDs off
  }

  for (int address = 0; address < FACE_PANEL_COUNT; address++) {
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
  bool shouldBlink = (curTime >= NextBlink);

  if (BeingBooped) {
    facialExpression = Face_Heart;
  } else if (Special_Face_Index != -1) {
    facialExpression = *(SpecialExpressions[Special_Face_Index]);
  }


  // Render the face
  FaceRender::LoadFaceExpression(facialExpression, shouldBlink, Face_OffsetY);
  FaceRender::ProcessRenderQueue(LEFT_LEDs, RIGHT_LEDs);



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









// utility functions



int getDistance() {
  return 1023 - analogRead(DistanceAnalogPin);  // Higher value = further away
}

unsigned long timeSince(unsigned long previousTime) {
  return millis() - previousTime;
}


// bool isButtonDown() {
//   return digitalRead(ButtonPin) == 0;  // Invert it, so that it's 0 when off and 1 when on
// }
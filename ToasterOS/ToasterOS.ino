// Imports
#include <FastLED.h>
#include "LedControl.h"
#include "Utils.h"
#include "ConfigTypes.h"


// CONFIGURATION
#include "Config.h"

// PINS: Input
#define PIN_BUTTON 0
#define PIN_ANALOG_DISTANCE 0

// PINS: LED STRIPS
#define PIN_LEFT_LEDSTRIP_DATA 9
#define PIN_RIGHT_LEDSTRIP_DATA 10


// Additional defines
#define DEBUG_MODE 0  // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render 7s on all panels
#define BOOPS_FOR_GAME 7



// Imports
#include "Protogen_Faces.h"
#include "FaceRender.h"
#include "LEDStripRender.h"
#include "Game.h"



// Face LEDs
FaceConfig* ProtoFaceConfig = new FaceConfig();
// If you new up this variable in the setup method, the nose will turn off when you're debugging via USB
FaceRender* ProtoFaceRenderer = new FaceRender(ProtoFaceConfig);

// LED Strips
LEDStripRender* LEDStripRenderer = new LEDStripRender();

CubeGame* CubeGameRunner = new CubeGame();


void setup() {
  if (DEBUG_MODE) {
    // Debug output on serial port; Used for Serial.println("blah blah");
    Serial.begin(9600);
  }

  randomSeed(analogRead(3));

  // pinMode(PIN_BUTTON, INPUT);

  // LED Face
  ProtoFaceRenderer->Initialise();
  ProtoFaceRenderer->Clear();

  // LED strips
  FastLED.addLeds<NEOPIXEL, PIN_LEFT_LEDSTRIP_DATA>(LEDStripRenderer->LED_Data, LEDSTRIP_NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PIN_RIGHT_LEDSTRIP_DATA>(LEDStripRenderer->LED_Data, LEDSTRIP_NUM_LEDS);
}



// PROTOGEN STATE
// Face movement
int Face_OffsetY = 0;
int Face_OffsetY_Dir = 1;
int OffsetDelay = 200;
unsigned long NextOffsetShift = millis() + OffsetDelay;

// Face rotation
int Eye_Rotation = 0;
unsigned long LastEyeRotation = 0;

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
unsigned long LastBoop = 0;
int ConsecutiveBoops = 0;


// Game
bool EnableGame = false;


// Debug
unsigned long DEBUG_LastOutputTime = 0;
unsigned int FrameDuration_MaxDuration = 0;
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
  bool boopSensorTouched = getDistance() < 500;
  if (boopSensorTouched) {
    // If there's not an active boop
    // And we're not still within the grace window for a boop
    if (BoopHoldStarted == 0 && !BeingBooped) BoopHoldStarted = millis();

    BoopLastDetected = millis();
  } else {
    BoopHoldStarted = 0;
  }

  bool wasBeingBooped = BeingBooped;
  BeingBooped = BoopLastDetected > 0                                           // Don't trigger on reboot
                && (timeSince(BoopHoldStarted) >= MinBoopHoldForTriggerMs)     // Avoid accidental short detection
                && (timeSince(BoopLastDetected) <= MaxBoopRetainAfterStopMs);  // Retain the boop for some time after detection stops


  // Consecutive boop detection
  if (!wasBeingBooped && BeingBooped) LastBoop = millis();

  if (timeSince(LastBoop) > 10000) ConsecutiveBoops = 0;

  // If we're coming out of a boop
  if (wasBeingBooped && !BeingBooped) {
    ConsecutiveBoops++;

    // Force change to a special face
    Special_Face_Index = random(0, NumSpecialFaces);
    NextSpecialFace = millis();
  }

  if (ConsecutiveBoops >= BOOPS_FOR_GAME) EnableGame = true;


  if (EnableGame) {
    EnableGame = CubeGameRunner->GameLoop(ProtoFaceRenderer, LEDStripRenderer, boopSensorTouched);
  } else {
    // Make the face bounce up and down
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

      // Special spiral rotation logic
      if (SpecialExpressions[Special_Face_Index] == &Face_Spiral) {
        if (timeSince(LastEyeRotation) > 150) {
          Eye_Rotation += 90;
          LastEyeRotation = millis();
        }

        if (Eye_Rotation >= 360) Eye_Rotation = 0;

        if (Eye_Rotation == 90) {
          facialExpression = Face_Spiral2;
        } else if (Eye_Rotation == 180) {
          facialExpression = Face_Spiral3;
        } else if (Eye_Rotation == 270) {
          facialExpression = Face_Spiral4;
        }
      }
    }


    // Render the face
    ProtoFaceRenderer->LoadFaceExpression(facialExpression, shouldBlink, Face_OffsetY);
    ProtoFaceRenderer->ProcessRenderQueue();


    // LED strips
    if (NextLEDStripUpdate <= curTime) {
      if (BeingBooped && !EnableGame) {
        // RGB scrolling
        for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
          LEDStripRenderer->SetLED(i, CHSV(LEDStripAnimationOffset + (i * 5), 255, 255));
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

        LEDStripRenderer->SetAllLEDs(CHSV(160, 255, LEDStripAnimationOffset));
      }

      NextLEDStripUpdate = curTime + 15;
      LEDStripRenderer->Render();
    }
  }


  // Debug print code
  if (DEBUG_MODE == 1) {
    // Count FPS
    FPSCOUNT_Iterations++;

    // Output
    if (timeSince(DEBUG_LastOutputTime) >= 1000) {
      DEBUG_LastOutputTime = millis();

      Serial.println(FPSCOUNT_Iterations);
      FPSCOUNT_Iterations = 0;
    }
  } else if (DEBUG_MODE == 2) {
    // Display max frame duration
    unsigned int duration = millis() - curTime;
    if (duration > FrameDuration_MaxDuration) FrameDuration_MaxDuration = duration;

    // Output
    if (timeSince(DEBUG_LastOutputTime) >= 10) {
      DEBUG_LastOutputTime = millis();

      Serial.println(FrameDuration_MaxDuration);
      FrameDuration_MaxDuration = 0;
    }
  }
}

// utility functions
int getDistance() {
  return 1023 - analogRead(PIN_ANALOG_DISTANCE);  // Higher value = further away
}


// bool isButtonDown() {
//   return digitalRead(PIN_BUTTON) == 0;  // Invert it, so that it's 0 when off and 1 when on
// }
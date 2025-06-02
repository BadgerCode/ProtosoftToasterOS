// Imports
#include <FastLED.h>
#include "LedControl.h"
#include "Utils.h"
#include "ConfigTypes.h"

// CONFIGURATION
#include "Config.h"

// Imports
#include "Protogen_Faces.h"
#include "FaceRender.h"
#include "LEDStripRender.h"
#include "BoopState.h"
#include "Game.h"


// Global variables
// Face LEDs
FaceConfig* ProtoFaceConfig = new FaceConfig();
// If you new up this variable in the setup method, the nose will turn off when you're debugging via USB
FaceRender* ProtoFaceRenderer = new FaceRender(ProtoFaceConfig);

// LED Strips
LEDStripRender* LEDStripRenderer = new LEDStripRender();

BoopStateHandler* BoopState = new BoopStateHandler(PIN_ANALOG_BOOP_SENSOR);

// Secret game
CubeGame* CubeGameRunner = new CubeGame();


void setup() {
  if (DEBUG_MODE) {
    // Debug output on serial port; Used for Serial.println("blah blah");
    Serial.begin(9600);
  }

  randomSeed(analogRead(3));

  // LED Face
  ProtoFaceRenderer->Initialise();
  ProtoFaceRenderer->Clear();

  // LED strips
  if (ENABLE_SIDE_LEDS) {
    FastLED.addLeds<NEOPIXEL, PIN_LEFT_LEDSTRIP_DATA>(LEDStripRenderer->LED_Data, LEDSTRIP_NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, PIN_RIGHT_LEDSTRIP_DATA>(LEDStripRenderer->LED_Data, LEDSTRIP_NUM_LEDS);
  }
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
int MinBlinkWait = 10000;
int MaxBlinkRandomDelay = 5000;
unsigned long NextBlink = millis() + random(MaxBlinkRandomDelay) + MinBlinkWait;
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
    NextBlink = millis() + random(MaxBlinkRandomDelay) + MinBlinkWait;
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


  // Boop behaviour
  BoopState->Update();

  // Force change to a special face if a boop just ended
  if (BoopState->BoopJustEnded) {
    Special_Face_Index = random(0, NumSpecialFaces);
    NextSpecialFace = millis();
  }

  if (BoopState->ConsecutiveBoops >= BOOPS_FOR_GAME) EnableGame = true;


  // Main logic
  if (EnableGame) {
    EnableGame = CubeGameRunner->GameLoop(ProtoFaceRenderer, LEDStripRenderer, BoopState->BoopSensorTouched);
  } else {
    // Make the face bounce up and down
    if (curTime >= NextOffsetShift) {
      NextOffsetShift = curTime + (BoopState->BoopActive ? OffsetDelay * 1.5 : OffsetDelay);

      Face_OffsetY += Face_OffsetY_Dir;

      // Check if it's time to reverse direction
      if (abs(Face_OffsetY) >= 1) Face_OffsetY_Dir *= -1;
    }


    // Determine expression
    struct FaceExpression facialExpression = Face_Neutral;
    bool shouldBlink = (curTime >= NextBlink);

    if (BoopState->BoopActive) {
      facialExpression = BoopState->DetermineExpression();
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
    RenderFaceExpression(facialExpression, shouldBlink, Face_OffsetY);
    ProtoFaceRenderer->ProcessRenderQueue();


    // LED strips
    if (NextLEDStripUpdate <= curTime) {
      if (BoopState->BoopActive && !EnableGame) {
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


void RenderFaceExpression(FaceExpression facialExpression, bool shouldBlink, int offsetY) {
  bool mirrorLeft = true;
  bool mirrorRight = false;

  // Mouth
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorLeft);

  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorRight);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorRight);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorRight);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorRight);

  // Nose
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_NOSE, (facialExpression).Nose[0], 0, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_NOSE, (facialExpression).Nose[0], 0, mirrorRight);

  // Eyes
  EyeFrame* eyes = shouldBlink && facialExpression.HasBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, (*eyes)[0], offsetY * -1, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_LEFT_EYE_BACK, (*eyes)[1], offsetY * -1, mirrorLeft);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, (*eyes)[0], offsetY * -1, mirrorRight);
  ProtoFaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_BACK, (*eyes)[1], offsetY * -1, mirrorRight);
}

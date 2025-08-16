// Imports
#include <FastLED.h>
#include "LedControl.h"
#include "Utils.h"
#include "ConfigTypes.h"

// CONFIGURATION
#include "Config.h"

// Imports
#include "Protogen_Faces.h"
#include "Protogen_Face_Collections.h"
#include "FaceRender.h"
#include "LEDStripRender.h"
#include "BoopState.h"
#include "RemoteMenu.h"
#include "ExpressionManager.h"
#include "Game.h"


// Global variables
// Face LEDs
FaceConfig* ProtoFaceConfig = new FaceConfig();
// If you new up this variable in the setup method, the nose will turn off when you're debugging via USB
FaceRender* ProtoFaceRenderer = new FaceRender(ProtoFaceConfig);

// LED Strips
LEDStripRender* LEDStripRenderer = new LEDStripRender();

// State managers
BoopStateHandler* BoopState = new BoopStateHandler(PIN_ANALOG_BOOP_SENSOR);
RemoteMenuClass* RemoteMenu = new RemoteMenuClass();
ExpressionManager* Expression = new ExpressionManager(RemoteMenu);


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

  // Remote control
  if (ENABLE_REMOTE_CONTROL) {
    pinMode(PIN_REMOTE_BUTTON_A, INPUT);
    pinMode(PIN_REMOTE_BUTTON_B, INPUT);
    pinMode(PIN_REMOTE_BUTTON_C, INPUT);
    pinMode(PIN_REMOTE_BUTTON_D, INPUT);
  }
}



// PROTOGEN STATE
// Face movement
int Face_OffsetY = 0;
int Face_OffsetY_Dir = 1;
int OffsetDelay = 200;
unsigned long NextOffsetShift = millis() + OffsetDelay;

// Blinking
int MinBlinkWait = 10000;
int MaxBlinkRandomDelay = 5000;
unsigned long NextBlink = millis() + random(MaxBlinkRandomDelay) + MinBlinkWait;
int BlinkDurationMs = 200;

// Expression faces
// int Special_Face_Index = -1;
// int MinSpecialFaceWait = 10000;
// unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
// int SpecialFaceDurationMs = 5000;

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

  // Update states
  BoopState->Update();
  RemoteMenu->Update();

  if (BoopState->ConsecutiveShortBoops >= BOOPS_FOR_GAME) EnableGame = true;



  // Main logic
  if (EnableGame) {
    EnableGame = CubeGameRunner->GameLoop(ProtoFaceRenderer, LEDStripRenderer, BoopState->BoopSensorTouched);
  } else {

    // Make the face bounce up and down
    if (curTime >= NextOffsetShift) {
      int adjustedDelay = OffsetDelay;

      // Make the face move slower when being booped/face rubbed
      if (ShouldShowBoopExpression()) {
        adjustedDelay = BoopState->ConsecutiveShortBoops < 6 ? OffsetDelay * 1.5 : OffsetDelay * 2.25;
      }

      // Update state
      NextOffsetShift = curTime + adjustedDelay;
      Face_OffsetY += Face_OffsetY_Dir;

      // Check if it's time to reverse direction
      if (abs(Face_OffsetY) >= 1) Face_OffsetY_Dir *= -1;
    }


    bool forceRandomExpression = BoopState->BoopJustEnded; // Force change to a special face if a boop just ended

    // Determine expression (boop overrides expression)
    struct FaceExpression facialExpression = ShouldShowBoopExpression()
                                               ? DetermineBoopExpression()
                                               : Expression->GetExpression(forceRandomExpression);
    bool shouldBlink = (curTime >= NextBlink);

    // Render the face
    RenderFaceExpression(facialExpression, shouldBlink, Face_OffsetY);
    ProtoFaceRenderer->ProcessRenderQueue();

    // LED strips
    if (NextLEDStripUpdate <= curTime) {
      if (BoopState->BoopActive) {
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



bool ShouldShowBoopExpression() {
  return BoopState->BoopActive || BoopState->ConsecutiveShortBoops > 0;
}

FaceExpression DetermineBoopExpression() {
  if (BoopState->IsFaceRub()) {
    // Simulate beating heart, by changing between the small & big heart
    // Big (800 ms), Small (400 ms), Big (800 ms)
    bool showSmallHeart = ((BoopState->TimeSinceBoopStart() / 400) % 3 == 1);
    return showSmallHeart ? Face_Heart_Small : Face_Heart;
  }

  if (BoopState->ConsecutiveShortBoops < 3) return Face_Surprised;
  if (BoopState->ConsecutiveShortBoops < 6) return Face_Angry;
  return Face_X_X;
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

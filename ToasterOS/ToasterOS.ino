// Imports
#include <FastLED.h>
#include "MAX7219Control.h"
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
#include "ExpressionManager.h"
#include "RemoteControl.h"
#include "Game.h"


// Global variables
// Face LEDs
FaceRender* ProtoFaceRenderer = new FaceRender(new FaceConfig());

// LED Strips
LEDStripRender* LEDStripRenderer = new LEDStripRender();

// State managers
BoopStateHandler* BoopState = new BoopStateHandler(ProtoConfig.PinAnalogBoopSensor);
ExpressionManager* ExpressionState = new ExpressionManager(BoopState, ProtoFaceRenderer);

// Remote control
RemoteControl* RemoteControlState = new RemoteControl(ExpressionState, ProtoFaceRenderer, BoopState);

// Secret game
CubeGame* CubeGameRunner = new CubeGame();


void setup() {
  if (ProtoConfig.DebugMode) {
    // Debug output on serial port; Used for Serial.println("blah blah");
    Serial.begin(9600);
  }

  randomSeed(analogRead(3));

  // LED Face
  ProtoFaceRenderer->Initialise();
  ProtoFaceRenderer->SetBrightness(ProtoConfig.Brightness);

  // LED strips
  if (ProtoConfig.EnableSideLEDs) {
    LEDStripRenderer->Initialise();
  }

  // Remote control
  if (ProtoConfig.EnableRemoteControl) {
    RemoteControlState->Initialise();
  }
}



// PROTOGEN STATE
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

  // UPDATE STATE- BOOP SENSOR
  if (ProtoConfig.EnableBoopSensor) BoopState->Update();

  // UPDATE STATE- REMOTE CONTROL
  if (ProtoConfig.EnableRemoteControl) RemoteControlState->Update();

  // UPDATE STATE- GAME TRIGGER
  if (BoopState->ConsecutiveShortBoops >= ProtoConfig.NumBoopsToActivateGame) EnableGame = true;

  // UPDATE STATE- EXPRESSIONS
  ExpressionState->Update();



  // Main logic
  if (EnableGame) {
    EnableGame = CubeGameRunner->GameLoop(ProtoFaceRenderer, LEDStripRenderer, BoopState->BoopSensorTouched);
  } else {

    // Render the face
    ExpressionState->Render();
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
      if (ProtoConfig.EnableSideLEDs) LEDStripRenderer->Render();
    }
  }


  // Debug print code
  if (ProtoConfig.DebugMode == 1) {
    // Count FPS
    FPSCOUNT_Iterations++;

    // Output
    if (timeSince(DEBUG_LastOutputTime) >= 1000) {
      DEBUG_LastOutputTime = millis();

      Serial.println(FPSCOUNT_Iterations);
      FPSCOUNT_Iterations = 0;
    }
  } else if (ProtoConfig.DebugMode == 2) {
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

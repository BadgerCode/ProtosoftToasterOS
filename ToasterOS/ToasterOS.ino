// Imports
#include <FastLED.h>
#include "Version.h"
#include "MAX7219Control.h"
#include "Utils.h"
#include "ConfigTypes.h"

// CONFIGURATION
#include "Config.h"

// Imports
#include "Protogen_Faces.h"
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

    Serial.print("Toaster OS ");
    Serial.println(TOASTER_OS_VERSION);
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
uint8_t LEDStripAnimationOffset = 0;  // Rainbow position while booped
const int SideLEDFadeSteps = 40;      // Steps from one colour to the other, one every 15ms
int SideLEDFadeStep = 0;
bool SideLEDFadeForward = true;
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
    if (ProtoConfig.EnableSideLEDs && NextLEDStripUpdate <= curTime) {
      if (BoopState->BoopActive) {
        // RGB scrolling
        for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
          LEDStripRenderer->SetLED(i, CHSV(LEDStripAnimationOffset + (i * 5), 255, 255));
        }

        LEDStripAnimationOffset = (LEDStripAnimationOffset + 2) % 255;
      } else {
        // Fade back and forth between the two side LED colours (Config.h)
        SideLEDFadeStep += SideLEDFadeForward ? 1 : -1;

        if (SideLEDFadeStep <= 0) {
          SideLEDFadeStep = 0;
          SideLEDFadeForward = true;
        } else if (SideLEDFadeStep >= SideLEDFadeSteps) {
          SideLEDFadeStep = SideLEDFadeSteps;
          SideLEDFadeForward = false;
        }

        uint8_t amountOfColourB = SideLEDFadeStep * 255 / SideLEDFadeSteps;
        LEDStripRenderer->SetAllLEDs(blend(CRGB(ProtoConfig.SideLEDColourA), CRGB(ProtoConfig.SideLEDColourB), amountOfColourB));
      }

      NextLEDStripUpdate = curTime + 15;
    }
  }

  // Send out any LED strip updates
  LEDStripRender::Render();


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

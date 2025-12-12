#define BUTTON_A 1
#define BUTTON_B 2
#define BUTTON_C 4
#define BUTTON_D 8


class RemoteControl {
private:
  // Buttons currently down (bitwise)
  int ButtonPressState = 0;

  // Buttons that have been pressed this frame (bitwise)
  int NewPressedButtonsState = 0;

  // Dependencies
  ExpressionManager* ExpressionState;
  FaceRender* FaceRenderer;
  BoopStateHandler* BoopState;

public:
  RemoteControl(ExpressionManager* expressionState, FaceRender* faceRenderer, BoopStateHandler* boopState) {
    ExpressionState = expressionState;
    FaceRenderer = faceRenderer;
    BoopState = boopState;
  }

  Initialise() {
    pinMode(ProtoConfig.PinRemoteButtonA, INPUT);
    pinMode(ProtoConfig.PinRemoteButtonB, INPUT);
    pinMode(ProtoConfig.PinRemoteButtonC, INPUT);
    pinMode(ProtoConfig.PinRemoteButtonD, INPUT);
  }

  bool IsButtonDown(int button) {
    return ButtonPressState & button;
  }

  // Returns a number representing all of the buttons that have been pressed this frame
  // Returns 0 if no buttons have been pressed this frame
  // If multiple buttons have been pressed, then multiple bits will be set
  // E.g. 3 = BUTTON_A (1) and BUTTON_B (2)
  int GetOnPressButtons() {
    return NewPressedButtonsState;
  }

  // Update state
  void Update() {
    int buttonsPressed = 0;
    if (digitalRead(ProtoConfig.PinRemoteButtonA)) buttonsPressed += 1;
    if (digitalRead(ProtoConfig.PinRemoteButtonB)) buttonsPressed += 2;
    if (digitalRead(ProtoConfig.PinRemoteButtonC)) buttonsPressed += 4;
    if (digitalRead(ProtoConfig.PinRemoteButtonD)) buttonsPressed += 8;

    // If no buttons were down but buttons are now down, they're new
    if (ButtonPressState == 0 && buttonsPressed != 0) {
      NewPressedButtonsState = buttonsPressed;
    } else {
      // No new buttons have been pressed
      NewPressedButtonsState = 0;
    }

    // Update state
    ButtonPressState = buttonsPressed;

    // Update menu state
    UpdateMenuState();
  }

  // Menu stuff
private:
  // Button press sequence
  static const int MaxButtonSequenceLength = 3;
  int NumPressedButtons = 0;
  int PressedButtons[MaxButtonSequenceLength] = { 0 };  // Track consecutive presses

  // Button sequence timeout
  const int MaxButtonPressWaitMs = 3000;
  unsigned long LastButtonPressTime = 0;

  void ResetMenuSelection() {
    NumPressedButtons = 0;
    memset(PressedButtons, 0, sizeof(PressedButtons));
    LastButtonPressTime = 0;
  }

  void UpdateMenuState() {
    // Reset button presses if its been too long
    if (NumPressedButtons > 0 && timeSince(LastButtonPressTime) >= MaxButtonPressWaitMs)
      ResetMenuSelection();

    // Get pressed button
    int pressedButton = GetOnPressButtons();

    // Do nothing if a button hasn't been pressed this frame
    if (pressedButton == 0) return false;

    // If they've pressed too many buttons, reset
    if (NumPressedButtons >= MaxButtonSequenceLength)
      ResetMenuSelection();

    // Track the button press
    PressedButtons[NumPressedButtons++] = pressedButton;
    LastButtonPressTime = millis();

    // MENU: A - HAPPY
    if (PressedButtons[0] == BUTTON_A) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

      // MENU: AA
      if (PressedButtons[1] == BUTTON_A) {
        // Wait for 3 button presses
        if (NumPressedButtons < 3) return;

        // MENU: AAA - Reset to neutral
        if (PressedButtons[2] == BUTTON_A) {
          ExpressionState->ResetToNeutral();
        }
      }

      // MENU: AB - BLEP
      if (PressedButtons[1] == BUTTON_B) {
        SetExpression(&Face_Blep);
      }

      // MENU: AC - CLOSED EYES
      if (PressedButtons[1] == BUTTON_C) {
        SetExpression(&Face_Sleepy);
      }

      // MENU: AD - EXCITED
      if (PressedButtons[1] == BUTTON_D) {
        SetExpression(&Face_Excited);
      }
    }

    // MENU: B - BAD EMOTIONS
    else if (PressedButtons[0] == BUTTON_B) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

      // MENU: BA - ANGRY
      if (PressedButtons[1] == BUTTON_A) {
        SetExpression(&Face_Angry);
      }

      // MENU: BB - BORED
      if (PressedButtons[1] == BUTTON_B) {
        SetExpression(&Face_Bored);
      }

      // MENU: BC - SHOCKED
      if (PressedButtons[1] == BUTTON_C) {
        SetExpression(&Face_Surprised);
      }

      // MENU: BD - DEAD
      if (PressedButtons[1] == BUTTON_D) {
        SetExpression(&Face_X_X);
      }
    }

    // MENU: C - SAUCY
    else if (PressedButtons[0] == BUTTON_C) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

      // MENU: CA - HEARTS
      if (PressedButtons[1] == BUTTON_A) {
        SetExpression(&Face_Heart);
      }

      // MENU: CC - SMIRK
      if (PressedButtons[1] == BUTTON_C) {
        SetExpression(&Face_Smirk);
      }

      // MENU: CD - DAZED/SPIRALS
      if (PressedButtons[1] == BUTTON_D) {
        SetExpression(&Face_Spiral);
      }
    }

    // MENU: D
    else if (PressedButtons[0] == BUTTON_D) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

      // MENU: DA - CYCLE BRIGHTNESS
      if (PressedButtons[1] == BUTTON_A) {
        // Cycle between values 3 7 11 15
        ProtoConfig.Brightness += 4;
        if (ProtoConfig.Brightness > 15) ProtoConfig.Brightness = 3;

        // Update brightness
        FaceRenderer->SetBrightness(ProtoConfig.Brightness);
      }

      // MENU: DB - TOGGLE BOOP SENSOR
      if (PressedButtons[1] == BUTTON_B) {
        ProtoConfig.EnableBoopSensor = !ProtoConfig.EnableBoopSensor;
      }

      // MENU: DD - AMONG US
      if (PressedButtons[1] == BUTTON_D) {
        SetExpression(&Face_AmongUs);
      }
    }

    // If we didn't return early to wait for more button presses, reset
    ResetMenuSelection();
  }

  void SetExpression(FaceExpression* expression) {
    ExpressionState->SetExpression(expression);
    BoopState->StopBoop();
  }
};
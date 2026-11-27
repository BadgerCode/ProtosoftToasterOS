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

  // Output
  static const int OutputNumLEDs = 2;
  CRGB OutputLEDData[OutputNumLEDs];

public:
  // The combo pressed this loop, like "DC", for custom features. Empty if no combo was finished this loop
  char JustPressedCombo[3] = "";

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

    if (ProtoConfig.EnableRemoteControlOutputLEDs) {
      FastLED.addLeds<NEOPIXEL, PIN_REMOTECONTROL_OUTPUT>(OutputLEDData, OutputNumLEDs);
    }
  }

  bool IsButtonDown(int button) {
    return ButtonPressState & button;
  }

  // Returns a number representing all of the buttons that have been pressed this frame
  // Returns 0 if no buttons have been pressed this frame
  // If multiple buttons have been pressed, then multiple bits will be set
  // E.g. 3 = BUTTON_A (1) and BUTTON_B (2)
  int GetNewPressedButtons() {
    return NewPressedButtonsState;
  }

  // Update state
  void Update() {
    JustPressedCombo[0] = 0;

    int buttonsPressed = 0;
    if (digitalRead(ProtoConfig.PinRemoteButtonA)) buttonsPressed += BUTTON_A;
    if (digitalRead(ProtoConfig.PinRemoteButtonB)) buttonsPressed += BUTTON_B;
    if (digitalRead(ProtoConfig.PinRemoteButtonC)) buttonsPressed += BUTTON_C;
    if (digitalRead(ProtoConfig.PinRemoteButtonD)) buttonsPressed += BUTTON_D;

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

    // Update menu output LEDs
    if (ProtoConfig.EnableRemoteControlOutputLEDs) {
      // Show the pressed button
      if (PressedButtons[0] != 0) {
        OutputLEDData[0] = ButtonToColour(PressedButtons[1]);
        OutputLEDData[1] = ButtonToColour(PressedButtons[0]);
      }
      // Show boop status
      else if (BoopState->ShouldShowBoopExpression()) {
        OutputLEDData[0] = CRGB::Blue;  //CHSV(180, 255, 255);
        OutputLEDData[1] = CRGB::Blue;  //CHSV(180, 255, 255);
      }
      // Show expression
      else {
        // Reverse order, first LED should be the sub-expression
        OutputLEDData[0] = ButtonToColour(CurrentExpressionPressedButtons[1]);
        OutputLEDData[1] = ButtonToColour(CurrentExpressionPressedButtons[0]);
      }

      fadeToBlackBy(&OutputLEDData[0], 2, 250);
    }
  }

private:
  // Menu stuff
  // Button press sequence
  static const int MaxButtonSequenceLength = 2;
  int NumPressedButtons = 0;
  int PressedButtons[MaxButtonSequenceLength] = { 0 };  // Track consecutive presses

  // Current expression
  int CurrentExpressionPressedButtons[2] = { BUTTON_A, BUTTON_A };

  // Button sequence timeout
  const int MaxButtonPressWaitMs = 3000;
  unsigned long LastButtonPressTime = 0;

  void SetExpression(FaceExpression* expression) {
    ExpressionState->SetExpression(expression);
    BoopState->StopBoop();
    RecordSelectedExpression();
  }

  void RecordSelectedExpression() {
    CurrentExpressionPressedButtons[0] = PressedButtons[0];
    CurrentExpressionPressedButtons[1] = PressedButtons[1];
  }

  CHSV ButtonToColour(int button) {
    switch (button) {
      case BUTTON_A: return CHSV(91, 255, 255);   // Green
      case BUTTON_B: return CHSV(0, 255, 255);    // Red
      case BUTTON_C: return CHSV(211, 255, 255);  // Purple
      case BUTTON_D: return CHSV(37, 255, 255);   // Yellow
      default: return CHSV(0, 0, 0);
    }
  }

  void ResetMenuSelection() {
    NumPressedButtons = 0;
    memset(PressedButtons, 0, sizeof(PressedButtons));
    LastButtonPressTime = 0;
  }

  // 'A' for BUTTON_A etc. 0 if it isn't a single button
  char ButtonToLetter(int button) {
    switch (button) {
      case BUTTON_A: return 'A';
      case BUTTON_B: return 'B';
      case BUTTON_C: return 'C';
      case BUTTON_D: return 'D';
      default: return 0;
    }
  }

  // The first action in RemoteControlActions for these two buttons, or NULL if there isn't one
  const RemoteControlActionConfig* FindAction(int firstButton, int secondButton) {
    char first = ButtonToLetter(firstButton);
    char second = ButtonToLetter(secondButton);
    if (first == 0 || second == 0) return NULL;

    for (int i = 0; i < NumRemoteControlActions; i++) {
      const RemoteControlActionConfig* action = &RemoteControlActions[i];
      if (action->Buttons[0] == first && action->Buttons[1] == second) return action;
    }
    return NULL;
  }

  void RunAction(const RemoteControlActionConfig* action) {
    switch (action->Action) {
      case REMOTE_EXPRESSION:
        if (action->Expression != NULL) SetExpression(action->Expression);
        break;

      case REMOTE_RESET_TO_DEFAULT:
        ExpressionState->ResetToNeutral();
        RecordSelectedExpression();
        break;

      case REMOTE_CYCLE_BRIGHTNESS:
        // Cycle between values 0%, 25%, 50%, 100%
        if (ProtoConfig.Brightness <= 0.0)
          ProtoConfig.Brightness = 0.25;
        else if (ProtoConfig.Brightness <= 0.25)
          ProtoConfig.Brightness = 0.5;
        else if (ProtoConfig.Brightness <= 0.5)
          ProtoConfig.Brightness = 1.0;
        else
          ProtoConfig.Brightness = 0.0;

        FaceRenderer->SetBrightness(ProtoConfig.Brightness);
        break;

      case REMOTE_TOGGLE_BOOP_SENSOR:
        ProtoConfig.EnableBoopSensor = !ProtoConfig.EnableBoopSensor;
        if (!ProtoConfig.EnableBoopSensor) BoopState->StopBoop();
        break;
    }
  }

  void UpdateMenuState() {
    // Reset button presses if its been too long
    if (NumPressedButtons > 0 && timeSince(LastButtonPressTime) >= MaxButtonPressWaitMs)
      ResetMenuSelection();

    // Get pressed button
    int pressedButton = GetNewPressedButtons();

    // Do nothing if a button hasn't been pressed this frame
    if (pressedButton == 0) return;

    // If they've pressed too many buttons, reset
    if (NumPressedButtons >= MaxButtonSequenceLength)
      ResetMenuSelection();

    // Track the button press
    PressedButtons[NumPressedButtons++] = pressedButton;
    LastButtonPressTime = millis();

    // Combos start with a single button. Ignore several buttons pressed at once
    if (ButtonToLetter(PressedButtons[0]) == 0) {
      ResetMenuSelection();
      return;
    }

    // Wait for 2 button presses
    if (NumPressedButtons < 2) return;

    // Record the combo, for custom features
    JustPressedCombo[0] = ButtonToLetter(PressedButtons[0]);
    JustPressedCombo[1] = ButtonToLetter(PressedButtons[1]);

    // Run the action for this combo, if there is one (RemoteControlActions in Protogen_Faces.h)
    const RemoteControlActionConfig* action = FindAction(PressedButtons[0], PressedButtons[1]);
    if (action != NULL) RunAction(action);

    // If we didn't return early to wait for more button presses, reset
    ResetMenuSelection();
  }
};
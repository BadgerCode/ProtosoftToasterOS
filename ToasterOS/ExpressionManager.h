
class ExpressionManager {
private:
  // Random face state
  int Special_Face_Index = -1;
  int MinSpecialFaceWait = 10000;
  unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  int SpecialFaceDurationMs = 5000;

  // Remote control state
  bool InNeutralState = true;
  FaceExpression* CurrentExpression = &Face_Neutral;

  // Dependencies
  RemoteMenuClass* RemoteMenu;

public:
  ExpressionManager(RemoteMenuClass* remoteMenu) {
    RemoteMenu = remoteMenu;
  }

  void SetExpression(FaceExpression* expression) {
    InNeutralState = false;
    CurrentExpression = expression;
  }

  void ResetToNeutral() {
    InNeutralState = true;
    CurrentExpression = &Face_Neutral;
    NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  }

  struct FaceExpression GetExpression(bool forceRandomExpression) {
    unsigned long curTime = millis();

    if (!ENABLE_REMOTE_CONTROL) {
      // Random expressions every so often

      // Time for a special face?
      if (forceRandomExpression || (Special_Face_Index == -1 && curTime >= NextSpecialFace)) {
        Special_Face_Index = random(0, NumRandomExpressions);
        NextSpecialFace = millis();
      }
      // Time to return to the neutral face?
      else if (timeSince(NextSpecialFace) >= SpecialFaceDurationMs) {
        NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
        Special_Face_Index = -1;
      }

      struct FaceExpression facialExpression = Face_Neutral;  // For some reason, this is more memory efficient than using pointers
      if (Special_Face_Index != -1) {
        facialExpression = *(RandomExpressions[Special_Face_Index]);

        // Special spiral rotation logic
        if (RandomExpressions[Special_Face_Index] == &Face_Spiral) {
          return GetSpiralFrame();
        }
      }

      return facialExpression;
    }
    // End of no remote control logic



    // Remote control
    CheckForMenuSelection();
    // if (RemoteMenu->ButtonWasHeld(BUTTON_A)) {
    //   ResetToNeutral();
    // } else {
    //   bool expressionChanged = CheckForMenuSelection();
    //   if (expressionChanged) InNeutralState = false;
    // }

    if (InNeutralState) {
      // Time for a different expression?
      if (forceRandomExpression || (CurrentExpression == &Face_Neutral && curTime >= NextSpecialFace)) {
        CurrentExpression = HappyExpressions[random(0, NumHappyExpressions)];
        NextSpecialFace = millis();
      }
      // Time to return to the neutral expression?
      else if (timeSince(NextSpecialFace) >= SpecialFaceDurationMs) {
        CurrentExpression = &Face_Neutral;
        NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
      }
    }
    // Animate spirals
    else if (CurrentExpression == &Face_Spiral) {
      return GetSpiralFrame();
    }

    return *CurrentExpression;
  }

private:
  FaceExpression GetSpiralFrame() {
    // Rotate the eye 90 degrees every 200ms
    int eyeRotation = (millis() / 200) % 4;

    if (eyeRotation == 0) {
      return Face_Spiral;
    } else if (eyeRotation == 1) {
      return Face_Spiral2;
    } else if (eyeRotation == 2) {
      return Face_Spiral3;
    } else if (eyeRotation == 3) {
      return Face_Spiral4;
    }
  }




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

  void CheckForMenuSelection() {
    // Reset button presses if its been too long
    if (NumPressedButtons > 0 && timeSince(LastButtonPressTime) >= MaxButtonPressWaitMs)
      ResetMenuSelection();

    // Get pressed button
    int pressedButton = RemoteMenu->GetOnPressButtons();

    // Do nothing if a button hasn't been pressed this frame
    if (pressedButton == 0) return false;

    // If they've pressed too many buttons, reset
    if (NumPressedButtons >= MaxButtonSequenceLength)
      ResetMenuSelection();

    // Track the button press
    PressedButtons[NumPressedButtons++] = pressedButton;
    LastButtonPressTime = millis();

    // MENU: A
    if (PressedButtons[0] == BUTTON_A) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

      // MENU: AA
      if (PressedButtons[1] == BUTTON_A) {
        // Wait for 3 button presses
        if (NumPressedButtons < 3) return;

        // MENU: AAA - Reset to neutral
        if (PressedButtons[2] == BUTTON_A) {
          ResetToNeutral();
        }
      }

      // MENU: AB - BLEP
      if (PressedButtons[1] == BUTTON_B) {
        SetExpression(&Face_Blep);
      }
    }

    // MENU: B
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

      // MENU: BD - DEAD
      if (PressedButtons[1] == BUTTON_D) {
        SetExpression(&Face_X_X);
      }
    }

    // MENU: C - SMIRK
    else if (PressedButtons[0] == BUTTON_C) {
      // Wait for 2 button presses
      if (NumPressedButtons < 2) return;

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
    }

    // If we didn't return early to wait for more button presses, reset
    ResetMenuSelection();
  }
};
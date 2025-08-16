
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
    if (RemoteMenu->ButtonWasHeld(BUTTON_A)) {
      InNeutralState = true;
      CurrentExpression = &Face_Neutral;
      NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
    } else {
      bool expressionChanged = CheckForMenuSelection();
      if (expressionChanged) InNeutralState = false;
    }

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

  const int MaxButtonPressWaitMs = 3000;
  unsigned long LastButtonPressTime = 0;
  int LastButtonPressed = -1;

  bool CheckForMenuSelection() {
    int pressedButton = GetPressedButton();
    if (pressedButton == -1) return false;

    // Reset last button press if its been too long
    if (timeSince(LastButtonPressTime) >= MaxButtonPressWaitMs) {
      LastButtonPressTime = 0;
      LastButtonPressed = -1;
    }

    bool expressionChanged = false;

    if (LastButtonPressed == -1) {
      if (pressedButton == BUTTON_A) {
        CurrentExpression = &Face_Blep;
        expressionChanged = true;
      } else if (pressedButton == BUTTON_B) {
        // B = Feeling Bad emotions
      } else if (pressedButton == BUTTON_C) {
        CurrentExpression = &Face_Smirk;
        expressionChanged = true;
      } else if (pressedButton == BUTTON_D) {
        CurrentExpression = &Face_Spiral;
        expressionChanged = true;
      }
    } else {
      if (LastButtonPressed == BUTTON_B) {
        if (pressedButton == BUTTON_A) {
          // A for angry
          CurrentExpression = &Face_Angry;
          expressionChanged = true;
        } else if (pressedButton == BUTTON_B) {
          // B for bored
          CurrentExpression = &Face_Bored;
          expressionChanged = true;
        } else if (pressedButton == BUTTON_D) {
          // D for dead
          CurrentExpression = &Face_X_X;
          expressionChanged = true;
        }
      }
    }

    // If they've made a selection, reset the menu
    if (expressionChanged) {
      LastButtonPressTime = 0;
      LastButtonPressed = -1;
    } else {
      LastButtonPressed = pressedButton;
      LastButtonPressTime = millis();
    }

    return expressionChanged;
  }

  int GetPressedButton() {
    if (RemoteMenu->ButtonWasTapped(BUTTON_A)) {
      return BUTTON_A;
    } else if (RemoteMenu->ButtonWasTapped(BUTTON_B)) {
      return BUTTON_B;
    } else if (RemoteMenu->ButtonWasTapped(BUTTON_C)) {
      return BUTTON_C;
    } else if (RemoteMenu->ButtonWasTapped(BUTTON_D)) {
      return BUTTON_D;
    }

    return -1;
  }


  // TODO: Fix
  // bool CheckForMenuSelection() {
  //   int pageNumber = RemoteMenu->SelectedMenuPage();
  //   int menuItemButton = RemoteMenu->SelectedMenuItemButton();

  //   switch (RemoteMenu->SelectedMenuButton()) {
  //     case BUTTON_A:
  //       if (pageNumber == 1) {
  //         if (RemoteMenu->ButtonIsDown(BUTTON_B)) {
  //           CurrentExpression = &Face_Blep;
  //           return true;
  //         } else if (RemoteMenu->ButtonIsDown(BUTTON_C)) {
  //           // TODO: Wink
  //           // CurrentExpression = &Face_Wink;
  //           // return true;
  //         } else if (RemoteMenu->ButtonIsDown(BUTTON_D)) {
  //           CurrentExpression = &Face_Surprised;
  //           return true;
  //         }
  //       }
  //       break;
  //     case BUTTON_B:
  //       if (pageNumber == 1) {
  //         switch (menuItemButton) {
  //           case BUTTON_A:
  //             CurrentExpression = &Face_Spiral;
  //             return true;
  //           case BUTTON_C:
  //             CurrentExpression = &Face_Silly;
  //             return true;
  //           // case BUTTON_D:
  //           // TODO: Sad
  //           // CurrentExpression = &Face_Sad;
  //           // return true;
  //           default:
  //             break;
  //         }
  //       } else if (pageNumber == 2) {
  //         switch (menuItemButton) {
  //           case BUTTON_A:
  //             CurrentExpression = &Face_Bored;
  //             return true;
  //           case BUTTON_C:
  //             CurrentExpression = &Face_Angry;
  //             return true;
  //           case BUTTON_D:
  //             CurrentExpression = &Face_X_X;
  //             return true;
  //           default:
  //             break;
  //         }
  //       } else if (pageNumber == 3) {
  //         switch (menuItemButton) {
  //           case BUTTON_A:
  //             CurrentExpression = &Face_Smirk;
  //             return true;
  //           case BUTTON_C:
  //             CurrentExpression = &Face_Heart;
  //             return true;
  //           default:
  //             break;
  //         }
  //       }
  //       break;
  //     case BUTTON_C:
  //       if (pageNumber == 1) {
  //         switch (menuItemButton) {
  //           case BUTTON_A:
  //             CurrentExpression = &Face_OWO;
  //             return true;
  //           case BUTTON_B:
  //             CurrentExpression = &Face_UWU;
  //             return true;
  //           case BUTTON_D:
  //             CurrentExpression = &Face_AmongUs;
  //             return true;
  //           default:
  //             break;
  //         }
  //       }
  //       break;
  //     case BUTTON_D:
  //       break;
  //     default:
  //       break;
  //   }
  //   return false;
  // }
};
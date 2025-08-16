
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
        Special_Face_Index = random(0, NumSpecialFaces);
        NextSpecialFace = millis();
      }
      // Time to return to the neutral face?
      else if (timeSince(NextSpecialFace) >= SpecialFaceDurationMs) {
        NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
        Special_Face_Index = -1;
      }

      struct FaceExpression facialExpression = Face_Neutral;  // For some reason, this is more memory efficient than using pointers
      if (Special_Face_Index != -1) {
        facialExpression = *(HappyExpressions[Special_Face_Index]);

        // Special spiral rotation logic
        if (HappyExpressions[Special_Face_Index] == &Face_Spiral) {
          return GetSpiralFrame();
        }
      }

      return facialExpression;

    } else {
      // Remote control
      if (RemoteMenu->ButtonWasHeld(BUTTON_A)) {
        InNeutralState = true;
        CurrentExpression = &Face_Neutral;
      } else if (RemoteMenu->ButtonWasTapped(BUTTON_A)) {
        InNeutralState = false;
        CurrentExpression = &Face_Blep;
      } else if (RemoteMenu->ButtonWasTapped(BUTTON_B)) {
        InNeutralState = false;
        CurrentExpression = &Face_Smirk;
      } else if (RemoteMenu->ButtonWasTapped(BUTTON_C)) {
        InNeutralState = false;
        CurrentExpression = &Face_Spiral;
      } else if (RemoteMenu->ButtonWasTapped(BUTTON_D)) {
        InNeutralState = false;
        CurrentExpression = &Face_Silly;
      }

      // Animate spirals
      if (CurrentExpression == &Face_Spiral) {
        return GetSpiralFrame();
      }

      return *CurrentExpression;
    }
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
};
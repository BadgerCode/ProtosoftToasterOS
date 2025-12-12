
class ExpressionManager {
private:
  // Facial expression state
  FaceExpression* CurrentExpression = &Face_Neutral;
  bool InNeutralState = true;

  // Random face state
  int MinSpecialFaceWait = 10000;
  unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  int SpecialFaceDurationMs = 5000;

  // Face movement
  int FaceMovementDelay = 200;
  int Face_OffsetY = 0;
  int Face_OffsetY_Dir = 1;
  unsigned long NextOffsetShift = millis();

  // Blinking
  int MinBlinkWait = 10000;
  int MaxBlinkRandomDelay = 5000;
  unsigned long NextBlink = millis() + random(MaxBlinkRandomDelay) + MinBlinkWait;
  int BlinkDurationMs = 200;

  // Dependencies
  BoopStateHandler* BoopState;
  FaceRender* FaceRenderer;

public:
  ExpressionManager(BoopStateHandler* boopState, FaceRender* faceRenderer) {
    BoopState = boopState;
    FaceRenderer = faceRenderer;
  }

  int GetMovementDelay() {
    // Make the face move slower for these expressions
    // Booping
    if (ShouldShowBoopExpression()) return BoopState->ConsecutiveShortBoops < 6 ? FaceMovementDelay * 1.5 : FaceMovementDelay * 2.25;
    // Slow
    if (CurrentExpression == &Face_Heart || CurrentExpression == &Face_AmongUs) return FaceMovementDelay * 1.5;
    // Very slow
    if (CurrentExpression == &Face_Sleepy || CurrentExpression == &Face_X_X) return FaceMovementDelay * 3;


    // Default delay
    return FaceMovementDelay;
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

  void Update() {
    unsigned long curTime = millis();

    // Update blink state- time to stop blinking?
    if (timeSince(NextBlink) >= BlinkDurationMs) {
      NextBlink = curTime + random(MaxBlinkRandomDelay) + MinBlinkWait;
    }

    // Update face movement state- time to move?
    if (curTime >= NextOffsetShift) {
      // Update state
      NextOffsetShift = curTime + GetMovementDelay();
      Face_OffsetY += Face_OffsetY_Dir;

      // Check if it's time to reverse direction
      if (abs(Face_OffsetY) >= 1) Face_OffsetY_Dir *= -1;
    }

    // Show different neutral/happy faces
    if (InNeutralState) {
      // Time for a different expression?
      // Force change to a special face if a boop just ended
      if (BoopState->BoopJustEnded || (CurrentExpression == &Face_Neutral && curTime >= NextSpecialFace)) {
        CurrentExpression = HappyExpressions[random(0, NumHappyExpressions)];
        NextSpecialFace = curTime;
      }
      // Time to return to the neutral expression?
      else if (timeSince(NextSpecialFace) >= SpecialFaceDurationMs) {
        CurrentExpression = &Face_Neutral;
        NextSpecialFace = curTime + random(4000) + MinSpecialFaceWait;
      }
    }
  }

  void Render() {
    struct FaceExpression facialExpression = DetermineExpression();

    bool shouldBlink = (millis() >= NextBlink);
    int offsetY = Face_OffsetY;
    bool mirrorLeft = true;
    bool mirrorRight = false;

    // Mouth
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorLeft);

    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorRight);

    // Nose
    FaceRenderer->UpdatePanel(PANEL_LEFT_NOSE, (facialExpression).Nose[0], 0, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_NOSE, (facialExpression).Nose[0], 0, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_SINGLE_NOSE, Face_Nose_Single, 0, false);

    // Eyes
    EyeFrame* eyes = shouldBlink && facialExpression.HasBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);
    FaceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, (*eyes)[0], offsetY * -1, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_EYE_BACK, (*eyes)[1], offsetY * -1, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, (*eyes)[0], offsetY * -1, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_EYE_BACK, (*eyes)[1], offsetY * -1, mirrorRight);
  }




  // Animated expressions helpers
  // Animation- Spirals
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

  // Animation- Hearts
  FaceExpression GetHeartFrame() {
    // Simulate beating heart, by changing between the small & big heart
    // Big (800 ms), Small (400 ms), Big (800 ms)
    bool showSmallHeart = (millis() / 400) % 3 == 1;

    return showSmallHeart ? Face_Heart_Small : Face_Heart;
  }

private:
  bool ShouldShowBoopExpression() {
    return ProtoConfig.EnableBoopSensor && (BoopState->BoopActive || BoopState->ConsecutiveShortBoops > 0);
  }

  struct FaceExpression DetermineExpression() {
    // Show boop faces
    if (ShouldShowBoopExpression()) {
      if (BoopState->IsFaceRub()) {
        return GetHeartFrame();
      }

      if (BoopState->ConsecutiveShortBoops < 3) return Face_Surprised;
      if (BoopState->ConsecutiveShortBoops < 6) return Face_Angry;
      return Face_X_X;
    }

    // Animate spirals
    if (CurrentExpression == &Face_Spiral) {
      return GetSpiralFrame();
    }
    // Animate hearts
    else if (CurrentExpression == &Face_Heart) {
      return GetHeartFrame();
    }

    return *CurrentExpression;
  }
};
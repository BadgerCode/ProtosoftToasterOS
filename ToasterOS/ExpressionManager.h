
class ExpressionManager {
private:
  // Facial expression state
  FaceExpression* CurrentExpression = DEFAULT_FACE;
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

  void SetExpression(FaceExpression* expression) {
    InNeutralState = false;
    CurrentExpression = expression;
    NextOffsetShift = millis() + GetMovementDelay();
  }

  void ResetToNeutral() {
    InNeutralState = true;
    CurrentExpression = DEFAULT_FACE;
    NextOffsetShift = millis() + GetMovementDelay();
    NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  }

  void Update() {
    unsigned long curTime = millis();

    // Update blink state- time to stop blinking?
    if (timeSince(NextBlink) >= BlinkDurationMs) {
      NextBlink = curTime + random(MaxBlinkRandomDelay) + MinBlinkWait;
    }

    // Update face movement state- time to move?
    if (!ProtoConfig.EnableFaceBobbing) {
      // Face bobbing turned off: keep the face still
      Face_OffsetY = 0;
    } else if (curTime >= NextOffsetShift) {
      // Update state
      NextOffsetShift = curTime + GetMovementDelay();
      Face_OffsetY += Face_OffsetY_Dir;

      // Check if it's time to reverse direction
      if (abs(Face_OffsetY) >= 1) Face_OffsetY_Dir *= -1;
    }

    // Show different neutral/happy faces, unless there are none to show
    if (InNeutralState && NumExtraDefaultExpressions > 0) {
      // Time for a different expression?
      // Force change to a special face if a boop just ended
      if (BoopState->BoopJustEnded || (CurrentExpression == DEFAULT_FACE && curTime >= NextSpecialFace)) {
        CurrentExpression = ExtraDefaultExpressions[random(0, NumExtraDefaultExpressions)];
        NextSpecialFace = curTime;
      }
      // Time to return to the neutral expression?
      else if (timeSince(NextSpecialFace) >= SpecialFaceDurationMs) {
        CurrentExpression = DEFAULT_FACE;
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

private:
  int GetMovementDelay() {
    // Booping. Slower once the last boop expression is reached (e.g. Face_X_X)
    if (BoopState->ShouldShowBoopExpression()) {
      bool onLastBoopExpression = NumBoopExpressions > 0 && BoopState->ConsecutiveShortBoops >= GetLastBoopExpressionMinBoops();
      return onLastBoopExpression ? FaceMovementDelay * 2.25 : FaceMovementDelay * 1.5;
    }

    // Slow (SlowMovementExpressions in Protogen_Faces.h)
    if (IsInList(CurrentExpression, SlowMovementExpressions, NumSlowMovementExpressions)) {
      return FaceMovementDelay * 1.5;
    }

    // Very slow (VerySlowMovementExpressions in Protogen_Faces.h)
    if (IsInList(CurrentExpression, VerySlowMovementExpressions, NumVerySlowMovementExpressions)) {
      return FaceMovementDelay * 3;
    }

    // Default delay
    return FaceMovementDelay;
  }

  struct FaceExpression DetermineExpression() {
    // Show boop faces

    if (BoopState->ShouldShowBoopExpression()) {
      if (BoopState->IsFaceRub()) {
        return GetHeartFrame();
      }

      FaceExpression* boopExpression = GetBoopExpression(BoopState->ConsecutiveShortBoops);
      if (boopExpression != NULL) return *boopExpression;
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


  bool IsInList(FaceExpression* expression, FaceExpression* const list[], int listLength) {
    for (int i = 0; i < listLength; i++) {
      if (list[i] == expression) return true;
    }
    return false;
  }

  // Boop expressions
  // The expression for the highest MinBoops that has been reached, or NULL if none has
  FaceExpression* GetBoopExpression(int boops) {
    FaceExpression* expression = NULL;
    int bestMinBoops = 0;

    for (int i = 0; i < NumBoopExpressions; i++) {
      auto boopExpression = BoopExpressions[i];
      if (boops >= boopExpression.MinBoops && (expression == NULL || boopExpression.MinBoops >= bestMinBoops)) {
        expression = boopExpression.Expression;
        bestMinBoops = boopExpression.MinBoops;
      }
    }

    return expression;
  }

  // The highest MinBoops in BoopExpressions, e.g. 6 for Face_X_X
  int GetLastBoopExpressionMinBoops() {
    int maxMinBoops = 0;
    for (int i = 0; i < NumBoopExpressions; i++) {
      if (BoopExpressions[i].MinBoops > maxMinBoops) maxMinBoops = BoopExpressions[i].MinBoops;
    }
    return maxMinBoops;
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
};
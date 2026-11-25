
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
    FaceParts face = GetFaceParts(DetermineExpression());

    bool shouldBlink = (millis() >= NextBlink);
    int offsetY = Face_OffsetY;
    bool mirrorLeft = true;
    bool mirrorRight = false;

    // Mouth
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_FRONT, face.Mouth[0], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_FRONT, face.Mouth[1], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_BACK, face.Mouth[2], offsetY, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_BACK, face.Mouth[3], offsetY, mirrorLeft);

    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_FRONT, face.Mouth[0], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_FRONT, face.Mouth[1], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_BACK, face.Mouth[2], offsetY, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_BACK, face.Mouth[3], offsetY, mirrorRight);

    // Nose
    FaceRenderer->UpdatePanel(PANEL_LEFT_NOSE, face.Nose[0], 0, mirrorLeft);
    FaceRenderer->UpdatePanel(PANEL_RIGHT_NOSE, face.Nose[0], 0, mirrorRight);
    FaceRenderer->UpdatePanel(PANEL_SINGLE_NOSE, Face_Nose_Single, 0, false);

    // Eyes
    const EyeFrame* eyes = shouldBlink && face.EyeBlink != NULL ? face.EyeBlink : face.Eye;
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

  // The expression to show now: a boop or nose rub expression, or the current one
  const FaceExpression* DetermineExpression() {
    if (BoopState->ShouldShowBoopExpression()) {
      // Nose rub: the heart, which animates by its own frames
      if (BoopState->IsFaceRub()) return &Face_Heart;

      FaceExpression* boopExpression = GetBoopExpression(BoopState->ConsecutiveShortBoops);
      if (boopExpression != NULL) return boopExpression;
    }

    return CurrentExpression;
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


  // Animated expressions
  // The parts of the face to draw this frame. Points into the expression and its animation,
  // so nothing is copied
  struct FaceParts {
    const byte (*Nose)[8];     // 1 panel
    const EyeFrame* Eye;
    const EyeFrame* EyeBlink;  // NULL if it doesn't blink
    const byte (*Mouth)[8];    // 4 panels
  };

  FaceParts GetFaceParts(const FaceExpression* expression) {
    FaceParts parts = {
      expression->Nose,
      &expression->Eye,
      // Animated expressions don't blink
      expression->HasBlink && expression->Animation == NULL ? &expression->Eye_Blink : NULL,
      expression->Mouth,
    };

    // Swap in any parts the current animation frame replaces
    const AnimationFrame* frame = GetAnimationFrame(expression->Animation);
    if (frame != NULL) {
      if (frame->Eye != NULL) parts.Eye = frame->Eye;
      if (frame->Mouth != NULL) parts.Mouth = frame->Mouth;
      if (frame->Nose != NULL) parts.Nose = frame->Nose;
    }

    return parts;
  }

  // The frame to show now, or NULL if the expression isn't animated.
  // Based on millis(), so every animation shares one clock
  const AnimationFrame* GetAnimationFrame(const FaceAnimation* animation) {
    if (animation == NULL || animation->NumFrames == 0) return NULL;

    unsigned long totalMs = 0;
    for (int i = 0; i < animation->NumFrames; i++) totalMs += animation->Frames[i].DurationMs;
    if (totalMs == 0) return &animation->Frames[0];

    unsigned long timeInLoop = millis() % totalMs;
    for (int i = 0; i < animation->NumFrames; i++) {
      if (timeInLoop < animation->Frames[i].DurationMs) return &animation->Frames[i];
      timeInLoop -= animation->Frames[i].DurationMs;
    }
    return &animation->Frames[animation->NumFrames - 1];
  }
};
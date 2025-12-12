
class ExpressionManager {
private:
  // Facial expression state
  FaceExpression* CurrentExpression = &Face_Neutral;
  bool InNeutralState = true;

  // Random face state
  int MinSpecialFaceWait = 10000;
  unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  int SpecialFaceDurationMs = 5000;

public:
  ExpressionManager() {}

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
    // Animate hearts
    else if (CurrentExpression == &Face_Heart) {
      return GetHeartFrame();
    }

    return *CurrentExpression;
  }

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

  FaceExpression GetHeartFrame() {
    // Simulate beating heart, by changing between the small & big heart
    // Big (800 ms), Small (400 ms), Big (800 ms)
    bool showSmallHeart = (millis() / 400) % 3 == 1;

    return showSmallHeart ? Face_Heart_Small : Face_Heart;
  }
};
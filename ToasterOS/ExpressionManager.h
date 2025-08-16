
class ExpressionManager {
private:
  // Random face state
  int Special_Face_Index = -1;
  int MinSpecialFaceWait = 10000;
  unsigned long NextSpecialFace = millis() + random(4000) + MinSpecialFaceWait;
  int SpecialFaceDurationMs = 5000;

public:

  struct FaceExpression GetExpression(bool forceRandomExpression) {
    unsigned long curTime = millis();

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


    // Remote control (override expression)
    // Buttons can be combined
    // int remoteState = (digitalRead(PIN_REMOTE_BUTTON_A))
    //                   + (digitalRead(PIN_REMOTE_BUTTON_B) << 1)
    //                   + (digitalRead(PIN_REMOTE_BUTTON_C) << 2)
    //                   + (digitalRead(PIN_REMOTE_BUTTON_D) << 3);

    if (digitalRead(PIN_REMOTE_BUTTON_A)) {
      Special_Face_Index = 4;
      NextSpecialFace = millis() + 60000;
    } else if (digitalRead(PIN_REMOTE_BUTTON_B)) {
      Special_Face_Index = 2;
      NextSpecialFace = millis() + 60000;
    } else if (digitalRead(PIN_REMOTE_BUTTON_C)) {
      Special_Face_Index = 5;
      NextSpecialFace = millis() + 60000;
    } else if (digitalRead(PIN_REMOTE_BUTTON_D)) {
      Special_Face_Index = 6;
      NextSpecialFace = millis() + 60000;
    }

    struct FaceExpression facialExpression = Face_Neutral;  // For some reason, this is more memory efficient than using pointers
    if (Special_Face_Index != -1) {
      facialExpression = *(HappyExpressions[Special_Face_Index]);

      // Special spiral rotation logic
      if (HappyExpressions[Special_Face_Index] == &Face_Spiral) {
        // Rotate the eye 90 degrees every 200ms
        int eyeRotation = (millis() / 200) % 4;

        if (eyeRotation == 1) {
          facialExpression = Face_Spiral2;
        } else if (eyeRotation == 2) {
          facialExpression = Face_Spiral3;
        } else if (eyeRotation == 3) {
          facialExpression = Face_Spiral4;
        }
      }
    }

    return facialExpression;
  }
};
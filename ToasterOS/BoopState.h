
class BoopStateHandler {
public:
  // Public state
  bool BoopSensorTouched = false;
  bool BoopActive = false;
  bool BoopJustEnded = false;
  int ConsecutiveBoops = 0;

private:
  // Config
  const int MinBoopHoldForTriggerMs = 300;
  const int MaxBoopRetainAfterStopMs = 2000;

  int BoopSensorPin;

  // Private state
  unsigned long BoopHoldStarted = 0;
  unsigned long BoopLastDetected = 0;
  unsigned long LastBoopStart = 0;


public:
  BoopStateHandler(int boopSensorPin) {
    BoopSensorPin = boopSensorPin;
  }

  void Update() {
    BoopSensorTouched = GetSensorDistance() < 500;
    if (BoopSensorTouched) {
      // If a boop hasn't already started, track the start time
      if (BoopHoldStarted == 0 && !BoopActive) BoopHoldStarted = millis();

      BoopLastDetected = millis();
    } else {
      BoopHoldStarted = 0;
    }

    bool boopWasActive = BoopActive;

    // Update boop active state
    BoopActive = BoopLastDetected > 0                                           // Don't trigger on reboot
                 && (timeSince(BoopHoldStarted) >= MinBoopHoldForTriggerMs)     // Avoid accidental short detection
                 && (timeSince(BoopLastDetected) <= MaxBoopRetainAfterStopMs);  // Retain the boop for some time after detection stops

    // Record the start time of a new boop
    if (!boopWasActive && BoopActive) LastBoopStart = millis();

    // Determine if a boop just ended
    BoopJustEnded = boopWasActive && !BoopActive;

    // Consecutive boop detection
    if (timeSince(LastBoopStart) > 10000) ConsecutiveBoops = 0;
    if (BoopJustEnded) ConsecutiveBoops++;
  }

  FaceExpression DetermineExpression() {
    // Simulate beating heart, by changing between the small & big heart
    // Big (800 ms), Small (400 ms), Big (800 ms)
    bool showSmallHeart = ((timeSince(LastBoopStart) / 400) % 3 == 1);

    return showSmallHeart ? Face_Heart_Small : Face_Heart;
  }

private:
  int GetSensorDistance() {
    return 1023 - analogRead(BoopSensorPin);  // Higher value = further away
  }
};


class BoopStateHandler {
public:
  // Public state
  bool BoopSensorTouched = false;
  bool BoopActive = false;
  bool BoopJustEnded = false;
  int ConsecutiveShortBoops = 0;


private:
  // Config
  const int MinBoopHoldForTriggerMs = 50;
  const int MaxBoopRetainAfterStopMs = 500;
  const int ShortBoopMaxDuration = 1700;
  const int FirstShortBoopResetDuration = 4000;
  const int ShortBoopResetDuration = 7000;

  int BoopSensorPin;

  // Private state
  unsigned long BoopHoldStarted = 0;
  unsigned long BoopLastDetected = 0;
  unsigned long LastBoopStart = 0;
  unsigned long LastBoopEnded = 0;


public:
  BoopStateHandler(int boopSensorPin) {
    BoopSensorPin = boopSensorPin;
  }

  void StopBoop() {
    BoopSensorTouched = false;
    BoopActive = false;
    BoopJustEnded = false;
    ConsecutiveShortBoops = 0;

    BoopHoldStarted = 0;
    BoopLastDetected = 0;
    LastBoopStart = 0;
    LastBoopEnded = 0;
  }

  void Update() {
    BoopSensorTouched = IsBoopSensorActive();
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
    bool boopJustStarted = !boopWasActive && BoopActive;
    if (boopJustStarted) {
      LastBoopStart = millis();
      ConsecutiveShortBoops++;
    }

    // Determine if a boop just ended
    BoopJustEnded = boopWasActive && !BoopActive;

    // Record the duration of a boop
    if (BoopJustEnded) {
      LastBoopEnded = millis();
    }

    // Reset consecutive boops upon face rub or after a period of no boops
    int adjustedBoopResetDuration = ConsecutiveShortBoops == 1 ? FirstShortBoopResetDuration : ShortBoopResetDuration;  // Shorter reset period for the first boop
    if (IsFaceRub()) ConsecutiveShortBoops = 0;
    else if (BoopActive == false && timeSince(LastBoopEnded) >= adjustedBoopResetDuration) ConsecutiveShortBoops = 0;
  }

  bool IsFaceRub() {
    return BoopActive && timeSince(LastBoopStart) > ShortBoopMaxDuration;
  }

  int TimeSinceBoopStart() {
    return BoopActive ? timeSince(LastBoopStart) : -1;
  }

  bool ShouldShowBoopExpression() {
    return ProtoConfig.EnableBoopSensor && (BoopActive || ConsecutiveShortBoops > 0);
  }

private:

  int Threshold = 100;
  int NextReadingIndex = 0;
  int PreviousReadings[5] = { 1023, 1023, 1023, 1023, 1023 };  // LOW when ACTIVE

  int IsBoopSensorActive() {
    // Update readings
    PreviousReadings[NextReadingIndex++] = analogRead(BoopSensorPin);
    if (NextReadingIndex >= 5) NextReadingIndex = 0;

    int sum = 0;
    for (int i = 0; i < 5; i++)
      sum += PreviousReadings[i];

    return (sum / 5) <= Threshold;
  }
};

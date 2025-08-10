class LEDStripRender {
public:
  // LED Strips
  CRGB LED_Data[LEDSTRIP_NUM_LEDS];
  const int NUM_LEDs = LEDSTRIP_NUM_LEDS;

  void SetLED(int index, CRGB value) {
    LED_Data[index] = value;
  }

  void SetAllLEDs(CRGB value) {
    for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
      LED_Data[i] = value;
    }
  }

  void Render() {
    if (ENABLE_SIDE_LEDS) {
      FastLED.show();
    }
  }
};
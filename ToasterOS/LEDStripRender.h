class LEDStripRender {
private:
  // LED strip data
  CRGB LED_Data[LEDSTRIP_NUM_LEDS];

public:
  void Initialise() {
    FastLED.addLeds<NEOPIXEL, PIN_LEFT_LEDSTRIP_DATA>(LED_Data, LEDSTRIP_NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, PIN_RIGHT_LEDSTRIP_DATA>(LED_Data, LEDSTRIP_NUM_LEDS);
  }

  void SetLED(int index, CRGB value) {
    LED_Data[index] = value;
  }

  void SetAllLEDs(CRGB value) {
    for (int i = 0; i < LEDSTRIP_NUM_LEDS; i++) {
      LED_Data[i] = value;
    }
  }

  void Render() {
    FastLED.show();
  }
};
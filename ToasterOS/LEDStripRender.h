class LEDStripRender {
private:
  // LED strip data
  CRGB LED_Data[LEDSTRIP_NUM_LEDS];

public:
  void Initialise() {
    // TODO: Allow passing in pin and number of LEDs
    // TODO: Use a static method with a switch statement for all of the possible pins
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

  // Render all LED strips that are using FastLED
  static void Render() {
    FastLED.show();
  }
};
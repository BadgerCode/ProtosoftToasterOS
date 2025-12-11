// PINS: Input
#define ENABLE_BOOP_SENSOR 1
#define PIN_ANALOG_BOOP_SENSOR 0

// PINS: LED STRIPS
#define ENABLE_SIDE_LEDS 1
#define PIN_LEFT_LEDSTRIP_DATA 9
#define PIN_RIGHT_LEDSTRIP_DATA 10
#define LEDSTRIP_NUM_LEDS 15


// Remote control
#define ENABLE_REMOTE_CONTROL 1
#define PIN_REMOTE_BUTTON_A 18
#define PIN_REMOTE_BUTTON_B 17
#define PIN_REMOTE_BUTTON_C 16
#define PIN_REMOTE_BUTTON_D 15


// Additional defines
#define DEBUG_MODE 0  // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render 7s on all panels
#define BOOPS_FOR_GAME 15


// Configure the pins and layout of the face LED panels
// See README for more info
#define FACE_BRIGHTNESS 7 // 0-15
FaceConfig::FaceConfig() {
  // Separate left and right connection
  NumConnections = 2;

  Connections = new FaceLEDConnection[NumConnections]{
    {
      // Pin configuration
      .PIN_DataIn = 3,
      .PIN_CS = 4,
      .PIN_CLK = 5,

      // Define the order of the panels
      .NumPanels = 7,
      .Panels = new PanelConfig[7]{
        // Start: mouth back
        { .PanelType = PANEL_RIGHT_MOUTH_BACK, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_MOUTH_MID_BACK, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_MOUTH_MID_FRONT, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_MOUTH_FRONT, .UpsideDown = true },
        // -> nose
        { .PanelType = PANEL_RIGHT_NOSE, .UpsideDown = false },
        // -> eye front
        { .PanelType = PANEL_RIGHT_EYE_FRONT, .UpsideDown = false },
        { .PanelType = PANEL_RIGHT_EYE_BACK, .UpsideDown = false },
      },
    },
    {
      // Pin configuration
      .PIN_DataIn = 6,
      .PIN_CS = 7,
      .PIN_CLK = 8,
      // Define the order of the panels
      .NumPanels = 7,
      .Panels = new PanelConfig[7]{
        // Start: mouth back
        { .PanelType = PANEL_LEFT_MOUTH_BACK, .UpsideDown = false },
        { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .UpsideDown = false },
        { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .UpsideDown = false },
        { .PanelType = PANEL_LEFT_MOUTH_FRONT, .UpsideDown = false },
        // -> nose
        { .PanelType = PANEL_LEFT_NOSE, .UpsideDown = true },
        // -> eye front
        { .PanelType = PANEL_LEFT_EYE_FRONT, .UpsideDown = true },
        { .PanelType = PANEL_LEFT_EYE_BACK, .UpsideDown = true },
      },
    }
  };
}

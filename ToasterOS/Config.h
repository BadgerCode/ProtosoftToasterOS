// PINS: Input
#define ENABLE_BOOP_SENSOR 0
#define PIN_ANALOG_BOOP_SENSOR 0

// PINS: LED STRIPS
#define ENABLE_SIDE_LEDS 0
#define PIN_LEFT_LEDSTRIP_DATA 9
#define PIN_RIGHT_LEDSTRIP_DATA 10
#define LEDSTRIP_NUM_LEDS 15


// Remote control
#define ENABLE_REMOTE_CONTROL 0
#define PIN_REMOTE_BUTTON_A 18
#define PIN_REMOTE_BUTTON_B 17
#define PIN_REMOTE_BUTTON_C 16
#define PIN_REMOTE_BUTTON_D 15


// Additional defines
#define DEBUG_MODE 0  // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render 7s on all panels
#define BOOPS_FOR_GAME 15


// Configure the pins and layout of the face LED panels
// See README for more info
FaceConfig::FaceConfig() {
  // Separate left and right connection
  NumConnections = 1;

  Connections = new FaceLEDConnection[NumConnections]{
    {
      // Pin configuration
      .PIN_DataIn = 12,
      .PIN_CS = 11,
      .PIN_CLK = 10,

      // Define the order of the panels
      .NumPanels = 11,
      .Panels = new PanelConfig[11]{
        // Start Right eye back
        { .PanelType = PANEL_RIGHT_EYE_BACK, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_EYE_FRONT, .UpsideDown = true },

        // -> Right mouth back
        { .PanelType = PANEL_RIGHT_MOUTH_BACK, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_MOUTH_MID_BACK, .UpsideDown = true },
        { .PanelType = PANEL_RIGHT_MOUTH_MID_FRONT, .UpsideDown = true },

        // -> Nose
        { .PanelType = PANEL_SINGLE_NOSE, .UpsideDown = true },

        // -> Left mouth front
        { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .UpsideDown = true },
        { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .UpsideDown = true },
        { .PanelType = PANEL_LEFT_MOUTH_BACK, .UpsideDown = true },

        // // -> Left eye back
        { .PanelType = PANEL_LEFT_EYE_FRONT, .UpsideDown = true },
        { .PanelType = PANEL_LEFT_EYE_BACK, .UpsideDown = true },

        // Example empty panel
        // { .PanelType = -1, .UpsideDown = false },
      },
    }
  };
}

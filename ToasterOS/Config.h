// CONSTANT CONFIG
// PINS: LED STRIPS (have to be constant)
#define PIN_LEFT_LEDSTRIP_DATA 9
#define PIN_RIGHT_LEDSTRIP_DATA 10
#define LEDSTRIP_NUM_LEDS 15


// Config
static ProtoConfigStruct ProtoConfig {
  // General config
  .Brightness = 7, // 0 - 15
  .DebugMode = 0, // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render 7s on all panels

  // Boop sensor config
  .EnableBoopSensor = true,
  .PinAnalogBoopSensor = 0,
  .NumBoopsToActivateGame = 15,

  // LED Strip config
  .EnableSideLEDs = true,

  // Remote control config
  .EnableRemoteControl = true,
  .PinRemoteButtonA = 18,
  .PinRemoteButtonB = 17,
  .PinRemoteButtonC = 16,
  .PinRemoteButtonD = 15,
};


// Configure the pins and layout of the face LED panels
// See README for more info
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

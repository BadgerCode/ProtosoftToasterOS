// PINS: Input
#define PIN_BUTTON 0
#define PIN_ANALOG_DISTANCE 0

// PINS: LED STRIPS
#define PIN_LEFT_LEDSTRIP_DATA 9
#define PIN_RIGHT_LEDSTRIP_DATA 10


// Additional defines
#define DEBUG_MODE 0  // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render 7s on all panels
#define BOOPS_FOR_GAME 7


/*
PROTOGEN FACE LAYOUT
(Looking into the protogen from the outside)

LEFT                                                                 RIGHT

      [EYE 1][EYE 2]                                  [EYE 2][EYE 1]
                              [NOSE]  [NOSE]
[MOUTH 1][MOUTH 2][MOUTH 3][MOUTH 4]  [MOUTH 4][MOUTH 3][MOUTH 2][MOUTH 1]
*/


class FaceConfig {
public:
  int NumConnections;
  FaceLEDConnection* Connections;

  FaceConfig() {
    // CONFIG EXAMPLE 1:
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
          { .PanelType = PANEL_RIGHT_MOUTH_BACK, .FlipX = false, .FlipY = false },
          { .PanelType = PANEL_RIGHT_MOUTH_MID_BACK, .FlipX = false, .FlipY = false },
          { .PanelType = PANEL_RIGHT_MOUTH_MID_FRONT, .FlipX = false, .FlipY = false },
          { .PanelType = PANEL_RIGHT_MOUTH_FRONT, .FlipX = false, .FlipY = false },
          // -> nose
          { .PanelType = PANEL_RIGHT_NOSE, .FlipX = true, .FlipY = true },
          // -> eye front
          { .PanelType = PANEL_RIGHT_EYE_FRONT, .FlipX = true, .FlipY = true },
          { .PanelType = PANEL_RIGHT_EYE_BACK, .FlipX = true, .FlipY = true },
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
          { .PanelType = PANEL_LEFT_MOUTH_BACK, .FlipX = true, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .FlipX = true, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .FlipX = true, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_FRONT, .FlipX = true, .FlipY = true },
          // -> nose
          { .PanelType = PANEL_LEFT_NOSE, .FlipX = false, .FlipY = false },
          // -> eye front
          { .PanelType = PANEL_LEFT_EYE_FRONT, .FlipX = false, .FlipY = false },
          { .PanelType = PANEL_LEFT_EYE_BACK, .FlipX = false, .FlipY = false },
        },
      }
    };
  }
};

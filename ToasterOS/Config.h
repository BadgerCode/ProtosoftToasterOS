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
    // Single connection
    // NumConnections = 1;
    // Connections = new FaceLEDConnection[NumConnections]{
    //   {
    //     .PIN_DataIn = 11,
    //     .PIN_CS = 9,
    //     .PIN_CLK = 13,
    //     .NumPanels = 14,
    //     .Panels = {
    //       // Start: Right mouth back
    //       { .PanelType = PANEL_RIGHT_MOUTH_BACK, .UpsideDown = false },
    //       { .PanelType = PANEL_RIGHT_MOUTH_MID_BACK, .UpsideDown = false },
    //       { .PanelType = PANEL_RIGHT_MOUTH_MID_FRONT, .UpsideDown = false },
    //       { .PanelType = PANEL_RIGHT_MOUTH_FRONT, .UpsideDown = false },
    //       // -> Left mouth front
    //       { .PanelType = PANEL_LEFT_MOUTH_FRONT, .UpsideDown = false },
    //       { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .UpsideDown = false },
    //       { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .UpsideDown = false },
    //       { .PanelType = PANEL_LEFT_MOUTH_BACK, .UpsideDown = false },
    //       // -> Left eye back
    //       { .PanelType = PANEL_LEFT_EYE_BACK, .UpsideDown = false },
    //       { .PanelType = PANEL_LEFT_EYE_FRONT, .UpsideDown = false },
    //       // -> Left nose
    //       { .PanelType = PANEL_LEFT_NOSE, .UpsideDown = false },
    //       // -> Right nose
    //       { .PanelType = PANEL_RIGHT_NOSE, .UpsideDown = false },
    //       // -> Right eye front
    //       { .PanelType = PANEL_RIGHT_EYE_FRONT, .UpsideDown = false },
    //       { .PanelType = PANEL_RIGHT_EYE_BACK, .UpsideDown = false },
    //     },
    //   }
    // };

    // Separate left and right connection
    NumConnections = 2;
    Connections = new FaceLEDConnection[NumConnections]{
      {
        .PIN_DataIn = 3,
        .PIN_CS = 4,
        .PIN_CLK = 5,
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
        .PIN_DataIn = 6,
        .PIN_CS = 7,
        .PIN_CLK = 8,
        .NumPanels = 7,
        .Panels = new PanelConfig[7]{
          // Start: mouth back
          { .PanelType = PANEL_LEFT_MOUTH_BACK, .FlipX = false, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_MID_BACK, .FlipX = false, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_MID_FRONT, .FlipX = false, .FlipY = true },
          { .PanelType = PANEL_LEFT_MOUTH_FRONT, .FlipX = false, .FlipY = true },
          // -> nose
          { .PanelType = PANEL_LEFT_NOSE, .FlipX = true, .FlipY = false },
          // -> eye front
          { .PanelType = PANEL_LEFT_EYE_FRONT, .FlipX = true, .FlipY = false },
          { .PanelType = PANEL_LEFT_EYE_BACK, .FlipX = true, .FlipY = false },
        },
      }
    };
  }
};

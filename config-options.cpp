// Things you can't do
// Overriding object properties in the global space
// Calling object/static methods in the global space
// Overriding array items in the global space
// You can set class static properties in the global space, but you have to specify the type for some reason



// Option 0
// Problems: Doesn't handle upside down panels

// Two connections
struct FaceLEDConnection FACE_CONNECTION_LEFT = {
  .PIN_DATAIN = 3,
  .PIN_CS = 4,
  .PIN_CLK = 5,
};

struct FaceLEDConnection FACE_CONNECTION_RIGHT = {
  .PIN_DATAIN = 6,
  .PIN_CS = 7,
  .PIN_CLK = 8,
};


struct FaceLEDConnection* FaceLEDConnections[2] = {
  &FACE_CONNECTION_LEFT,  // Connection 0
  &FACE_CONNECTION_RIGHT  // Connection 1
};
struct FaceConfiguration Face_Config = {
  .Left = {
    .Face_Connection_Number = 0,
    .EyePanelNumbers = { 5, 6 },
    .NosePanelNumbers = { 4 },
    .MouthPanelNumbers = { 0, 1, 2, 3 },
  },
  .Right = {
    .Face_Connection_Number = 1,
    .EyePanelNumbers = { 5, 6 },
    .NosePanelNumbers = { 4 },
    .MouthPanelNumbers = { 0, 1, 2, 3 },
  }
};






// Option 1
// Problems: Doesn't handle upside down panels

// One connection
struct FaceConfiguration FACE_CONFIG = {
  .NumConnections = 1,  // The number of separate connections between your controller and face LED panels
  .Connections = {
    // Config for each connection
    // Left & right face
    {
      .PIN_DataIn = 3,
      .PIN_CS = 4,
      .PIN_CLK = 5,
      .NumPanels = 14,
      // Define the order that the panels are connected
      .Panels = {
        // Start: Right mouth back
        PANEL_RIGHT_MOUTH1,
        PANEL_RIGHT_MOUTH2,
        PANEL_RIGHT_MOUTH3,
        PANEL_RIGHT_MOUTH4,
        // Next: Left mouth front
        PANEL_LEFT_MOUTH4,
        PANEL_LEFT_MOUTH3,
        PANEL_LEFT_MOUTH2,
        PANEL_LEFT_MOUTH1,
        // Next: Left eye back
        PANEL_LEFT_EYE1,
        PANEL_LEFT_EYE2,
        // Next: Left nose
        PANEL_LEFT_NOSE,
        // Next: Right nose
        PANEL_RIGHT_NOSE,
        // Next: Right eye front
        PANEL_RIGHT_EYE2,
        PANEL_RIGHT_EYE1,
      },
    },
  }
};


// Two connections
struct FaceConfiguration FACE_CONFIG = {
  .NumConnections = 2,  // The number of separate connections between your controller and face LED panels
  .Connections = {
    // Config for each connection
    // Left face
    {
      .PIN_DataIn = 6,
      .PIN_CS = 7,
      .PIN_CLK = 8,
      .NumPanels = 7,
      // Define the order that the panels are connected
      .Panels = {
        // Start: Left mouth back
        PANEL_LEFT_MOUTH1,
        PANEL_LEFT_MOUTH2,
        PANEL_LEFT_MOUTH3,
        PANEL_LEFT_MOUTH4,
        // Next: Left nose
        PANEL_LEFT_NOSE,
        // Next: Left eye front
        PANEL_LEFT_EYE2,
        PANEL_LEFT_EYE1,
      },
    },
    // Right face
    {
      .PIN_DataIn = 3,
      .PIN_CS = 4,
      .PIN_CLK = 5,
      .NumPanels = 7,
      // Define the order that the panels are connected
      .Panels = {
        // Start: Right mouth back
        PANEL_RIGHT_MOUTH1,
        PANEL_RIGHT_MOUTH2,
        PANEL_RIGHT_MOUTH3,
        PANEL_RIGHT_MOUTH4,
        // Next: Right nose
        PANEL_RIGHT_NOSE,
        // Next: Right eye front
        PANEL_RIGHT_EYE2,
        PANEL_RIGHT_EYE1,
      },
    },
  }
};







// Option 2
// Problems: Can't define the panels in the order they are connected

// One connection
struct FaceConfiguration FACE_CONFIG = {
  .NumConnections = 1,
  .Connections = (FaceLEDConnection[]){
    // Whole face (ConnectionNumber 0)
    { .PIN_DataIn = 11, .PIN_CS = 9, .PIN_CLK = 13 },
  },
  // Connection 0: Whole face
  // 3: Left eye back
  .LeftEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 8, .FrontPanelNumber = 9 },
  // 4: Left nose
  .LeftNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 10 },
  // 2: Left mouth front
  .LeftMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 7, .MiddleBackPanelNumber = 6, .MiddleFrontPanelNumber = 5, .FrontPanelNumber = 4 },
  // 6: Right eye front
  .RightEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 13, .FrontPanelNumber = 12 },
  // 5: Right nose
  .RightNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 11 },
  // 1: Right mouth back
  .RightMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },
};

// Two Connections
struct FaceConfiguration FACE_CONFIG = {
  .NumConnections = 2,
  .Connections = (FaceLEDConnection[]){
    // ConnectionNumber 0
    { .PIN_DataIn = 6, .PIN_CS = 7, .PIN_CLK = 8 },
    // ConnectionNumber 1
    { .PIN_DataIn = 3, .PIN_CS = 4, .PIN_CLK = 5 },
  },

  // Connection 0 (left side)
  // 3: Left eye front
  .LeftEyes = { .ConnectionNumber = 0, .UpsideDown = true, .BackPanelNumber = 6, .FrontPanelNumber = 5 },
  // 2: Left nose
  .LeftNose = { .ConnectionNumber = 0, .UpsideDown = true, .PanelNumber = 4 },
  // 1: Left mouth back
  .LeftMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },

  // Connection 1 (right side)
  // 3: Right eye front
  .RightEyes = { .ConnectionNumber = 1, .UpsideDown = false, .BackPanelNumber = 6, .FrontPanelNumber = 5 },
  // 2: Right nose
  .RightNose = { .ConnectionNumber = 1, .UpsideDown = false, .PanelNumber = 4 },
  // 1: Right mouth back
  .RightMouth = { .ConnectionNumber = 1, .UpsideDown = true, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },
};




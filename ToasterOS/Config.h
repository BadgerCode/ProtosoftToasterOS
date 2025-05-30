#define PANEL_LEFT_MOUTH_BACK 0
#define PANEL_LEFT_MOUTH_MID_BACK 1
#define PANEL_LEFT_MOUTH_MID_FRONT 2
#define PANEL_LEFT_MOUTH_FRONT 3
#define PANEL_LEFT_NOSE 4
#define PANEL_LEFT_EYE_BACK 5
#define PANEL_LEFT_EYE_FRONT 6
#define PANEL_RIGHT_MOUTH_BACK 7
#define PANEL_RIGHT_MOUTH_MID_BACK 8
#define PANEL_RIGHT_MOUTH_MID_FRONT 9
#define PANEL_RIGHT_MOUTH_FRONT 10
#define PANEL_RIGHT_NOSE 11
#define PANEL_RIGHT_EYE_BACK 12
#define PANEL_RIGHT_EYE_FRONT 13

/*
PROTOGEN FACE LAYOUT
(Looking into the protogen from the outside)

LEFT                                                                 RIGHT

      [EYE 1][EYE 2]                                  [EYE 2][EYE 1]
                              [NOSE]  [NOSE]
[MOUTH 1][MOUTH 2][MOUTH 3][MOUTH 4]  [MOUTH 4][MOUTH 3][MOUTH 2][MOUTH 1]
*/

// TODO: My rendering code does it the opposite way around (left = left when looking out of the protogen)

// Option 3: Overriding the bits you care about
struct PanelConfig {
  int PanelType;
  bool FlipX;
  bool FlipY;
};


struct FaceLEDConnection {
  int PIN_DataIn;
  int PIN_CS;
  int PIN_CLK;
  int NumPanels;
  PanelConfig* Panels;
};


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





// struct FaceLEDConnection {
//   int PIN_DataIn;
//   int PIN_CS;
//   int PIN_CLK;
// };

// struct FaceConfigurationEyes {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int BackPanelNumber;
//   int FrontPanelNumber;
// };

// struct FaceConfigurationNose {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int PanelNumber;
// };

// struct FaceConfigurationMouth {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int BackPanelNumber;
//   int MiddleBackPanelNumber;
//   int MiddleFrontPanelNumber;
//   int FrontPanelNumber;
// };

// struct PanelConfig {
//   int PanelNumber;
//   int ConnectionNumber;
//   bool UpsideDown;
// };


// // Connections
// int NumConnections;
// FaceLEDConnection* Connections;

// PanelConfig PanelConfigs[14];

// // Left side
// FaceConfigurationEyes LeftEyes;
// FaceConfigurationNose LeftNose;
// FaceConfigurationMouth LeftMouth;
// // Right side
// FaceConfigurationEyes RightEyes;
// FaceConfigurationNose RightNose;
// FaceConfigurationMouth RightMouth;
// Configure the connections
// NumConnections = 1;
// Connections = new FaceLEDConnection[NumConnections];

// // Set up the connections
// Connections[0] = { .PIN_DataIn = 11, .PIN_CS = 9, .PIN_CLK = 13 };

// // Start: Right mouth back
// PanelConfigs[PANEL_RIGHT_MOUTH_BACK] = { .PanelNumber = 0, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_RIGHT_MOUTH_MID_BACK] = { .PanelNumber = 1, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_RIGHT_MOUTH_MID_FRONT] = { .PanelNumber = 2, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_RIGHT_MOUTH_FRONT] = { .PanelNumber = 3, .ConnectionNumber = 0, .UpsideDown = false };

// // ->: Left mouth back
// PanelConfigs[PANEL_LEFT_MOUTH_FRONT] = { .PanelNumber = 4, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_LEFT_MOUTH_MID_FRONT] = { .PanelNumber = 5, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_LEFT_MOUTH_MID_BACK] = { .PanelNumber = 6, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_LEFT_MOUTH_BACK] = { .PanelNumber = 7, .ConnectionNumber = 0, .UpsideDown = false };

// // ->: Left mouth back
// PanelConfigs[PANEL_LEFT_MOUTH_FRONT] = { .PanelNumber = 4, .ConnectionNumber = 0, .UpsideDown = false };
// PanelConfigs[PANEL_LEFT_MOUTH_MID_FRONT] = { .PanelNumber = 5, .ConnectionNumber = 0, .UpsideDown = false };


// // Configure the panels
// // 1: Right mouth back
// RightMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 };

// // 2: Left mouth front
// LeftMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 7, .MiddleBackPanelNumber = 6, .MiddleFrontPanelNumber = 5, .FrontPanelNumber = 4 };

// // 3: Left eye back
// LeftEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 8, .FrontPanelNumber = 9 };

// // 4: Left nose
// LeftNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 10 };

// // 5: Right nose
// RightNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 11 };

// // 6: Right eye front
// RightEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 13, .FrontPanelNumber = 12 };




/*
// Option 2:
// Face based approach

var connections = [
  new LEDConnection{ PIN_DATAIN = 3, PIN_CS = 4, PIN_CLK = 5 },
  new LEDConnection{ PIN_DATAIN = 6, PIN_CS = 7, PIN_CLK = 8 },
];

var ledPanels = new LEDPanels {
  // Left
  LEFT_MOUTH1 = new{ Connection = 0, Panel = 0, IsUpsideDown = false },
  LEFT_MOUTH2 = new{ Connection = 0, Panel = 1, IsUpsideDown = false },
  LEFT_MOUTH3 = new{ Connection = 0, Panel = 2, IsUpsideDown = false },
  LEFT_MOUTH4 = new{ Connection = 0, Panel = 3, IsUpsideDown = false },
  LEFT_NOSE = new{ Connection = 0, Panel = 4, IsUpsideDown = true },
  LEFT_EYE2 = new{ Connection = 0, Panel = 5, IsUpsideDown = true },
  LEFT_EYE1 = new{ Connection = 0, Panel = 6, IsUpsideDown = true },

  // Right
  RIGHT_MOUTH1 = new{ Connection = 1, Panel = 0, IsUpsideDown = true },
  RIGHT_MOUTH2 = new{ Connection = 1, Panel = 1, IsUpsideDown = true },
  RIGHT_MOUTH3 = new{ Connection = 1, Panel = 2, IsUpsideDown = true },
  RIGHT_MOUTH4 = new{ Connection = 1, Panel = 3, IsUpsideDown = true },
  RIGHT_NOSE = new{ Connection = 1, Panel = 4, IsUpsideDown = false },
  RIGHT_EYE2 = new{ Connection = 1, Panel = 5, IsUpsideDown = false },
  RIGHT_EYE1 = new{ Connection = 1, Panel = 6, IsUpsideDown = false },
};
*/

// struct FaceLEDConnection {
//   int PIN_DataIn;
//   int PIN_CS;
//   int PIN_CLK;
// };

// struct FaceConfigurationEyes {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int BackPanelNumber;
//   int FrontPanelNumber;
// };

// struct FaceConfigurationNose {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int PanelNumber;
// };

// struct FaceConfigurationMouth {
//   int ConnectionNumber;
//   bool UpsideDown;
//   int BackPanelNumber;
//   int MiddleBackPanelNumber;
//   int MiddleFrontPanelNumber;
//   int FrontPanelNumber;
// };

// struct FaceConfiguration {
//   int NumConnections;
//   FaceLEDConnection* Connections;
//   // Left side
//   FaceConfigurationEyes LeftEyes;
//   FaceConfigurationNose LeftNose;
//   FaceConfigurationMouth LeftMouth;
//   // Right side
//   FaceConfigurationEyes RightEyes;
//   FaceConfigurationNose RightNose;
//   FaceConfigurationMouth RightMouth;
// };


// // One connection
// struct FaceConfiguration FACE_CONFIG = {
//   .NumConnections = 1,
//   .Connections = (FaceLEDConnection[]){
//     // Whole face (ConnectionNumber 0)
//     { .PIN_DataIn = 11, .PIN_CS = 9, .PIN_CLK = 13 },
//   },
//   // Connection 0: Whole face
//   // 3: Left eye back
//   .LeftEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 8, .FrontPanelNumber = 9 },
//   // 4: Left nose
//   .LeftNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 10 },
//   // 2: Left mouth front
//   .LeftMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 7, .MiddleBackPanelNumber = 6, .MiddleFrontPanelNumber = 5, .FrontPanelNumber = 4 },
//   // 6: Right eye front
//   .RightEyes = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 13, .FrontPanelNumber = 12 },
//   // 5: Right nose
//   .RightNose = { .ConnectionNumber = 0, .UpsideDown = false, .PanelNumber = 11 },
//   // 1: Right mouth back
//   .RightMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },
// };

// // Two Connections
// struct FaceConfiguration FACE_CONFIG = {
//   .NumConnections = 2,
//   .Connections = (FaceLEDConnection[]){
//     // ConnectionNumber 0
//     { .PIN_DataIn = 6, .PIN_CS = 7, .PIN_CLK = 8 },
//     // ConnectionNumber 1
//     { .PIN_DataIn = 3, .PIN_CS = 4, .PIN_CLK = 5 },
//   },

//   // Connection 0 (left side)
//   // 3: Left eye front
//   .LeftEyes = { .ConnectionNumber = 0, .UpsideDown = true, .BackPanelNumber = 6, .FrontPanelNumber = 5 },
//   // 2: Left nose
//   .LeftNose = { .ConnectionNumber = 0, .UpsideDown = true, .PanelNumber = 4 },
//   // 1: Left mouth back
//   .LeftMouth = { .ConnectionNumber = 0, .UpsideDown = false, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },

//   // Connection 1 (right side)
//   // 3: Right eye front
//   .RightEyes = { .ConnectionNumber = 1, .UpsideDown = false, .BackPanelNumber = 6, .FrontPanelNumber = 5 },
//   // 2: Right nose
//   .RightNose = { .ConnectionNumber = 1, .UpsideDown = false, .PanelNumber = 4 },
//   // 1: Right mouth back
//   .RightMouth = { .ConnectionNumber = 1, .UpsideDown = true, .BackPanelNumber = 0, .MiddleBackPanelNumber = 1, .MiddleFrontPanelNumber = 2, .FrontPanelNumber = 3 },
// };







// Option 2b: classes (suck)
// class FaceConfig {
// public:
//   static int NumConnections;
//   static FaceLEDConnection* Connections;

//   // static void SetupConnections(int numConnections) {
//   //   NumConnections = numConnections;
//   // }

//   // static void SetupConnections(int numConnections, FaceLEDConnection* connections) {
//   //   NumConnections = numConnections;
//   //   Connections = connections;
//   // }

//   // // Left side
//   static FaceConfigurationEyes LeftEyes;
//   // FaceConfigurationNose LeftNose;
//   // FaceConfigurationMouth LeftMouth;
//   // // Right side
//   // FaceConfigurationEyes RightEyes;
//   // FaceConfigurationNose RightNose;
//   // FaceConfigurationMouth RightMouth;
// };

// FaceConfigurationTest* FaceConfig = new FaceConfigurationTest();
// FaceConfig->SetupConnections(2);
// FaceConfig->NumConnections = 2;
// FaceConfig::SetupConnections(2);
// FaceConfig::SetupConnections(2, { { .PIN_DataIn = 6, .PIN_CS = 7, .PIN_CLK = 8 }, { .PIN_DataIn = 3, .PIN_CS = 4, .PIN_CLK = 5 } });
// int FaceConfig::NumConnections = 2;
// FaceLEDConnection* FaceConfig::Connections = (FaceLEDConnection[]){
//   { .PIN_DataIn = 6, .PIN_CS = 7, .PIN_CLK = 8 },
//   { .PIN_DataIn = 3, .PIN_CS = 4, .PIN_CLK = 5 },
// };
// FaceConfigurationEyes FaceConfig::LeftEyes = { .ConnectionNumber = 0, .UpsideDown = true, .BackPanelNumber = 6, .FrontPanelNumber = 5 };

/*
FaceConfiguration.Connections = new [
  new Connection { PIN_DataIn = 6, PIN_CS = 7, PIN_CLK = 8 },
  new Connection { PIN_DataIn = 3, PIN_CS = 4, PIN_CLK = 5 }
];

*/




// Two connections: Left side, Right side
// The wiring starts at the back of the mouth, goes to the nose, then into the front of the eyes
// struct FaceConfiguration FACE_CONFIG;
// FACE_CONFIG.NumConnections = 2;







/// OPTION 1: Define each connection, one at a time, with panels in order

/*
// Connection-orientated approach

var connection1 = new FaceConnection {
  PIN_DATAIN = 3,
  PIN_CS = 4,
  PIN_CLK = 5,
  NUM_PANELS = 7,
  PANELS = [LEFT_MOUTH1, LEFT_MOUTH2, LEFT_MOUTH3, LEFT_MOUTH4, LEFT_NOSE, LEFT_EYE2, LEFT_EYE1]
};

var connection2 = new FaceConnection {
  PIN_DATAIN = 6,
  PIN_CS = 7,
  PIN_CLK = 8,
  NUM_PANELS = 7,
  PANELS = [RIGHT_MOUTH1, RIGHT_MOUTH2, RIGHT_MOUTH3, RIGHT_MOUTH4, RIGHT_NOSE, RIGHT_EYE2, RIGHT_EYE1]
};

// In constructor
// Will need to take in number of connections as a parameter
var connection = new LEDConnection(connection.PIN_DATAIN, connection.PIN_CS, connection.PIN_CLK);
for(var i = 0; i < connection.PANELS.length; i++) {
  PanelDefinitions[connection.PANELS[i]] = new { connection = connection, index = i };
}

// At render time
// RenderFace
RenderPanel(PanelDefintions[LEFT_MOUTH1], ...)
RenderPanel(PanelDefintions[LEFT_MOUTH2], ...)
*/

// struct FaceLEDConnection {
//   int PIN_DataIn;
//   int PIN_CS;
//   int PIN_CLK;
//   int NumPanels;
//   int Panels[];
// };

// struct FaceConfiguration {
//   int NumConnections;
//   FaceLEDConnection Connections[];
// };

// Face configuration: SNAPPY
// struct FaceConfiguration FACE_CONFIG = {
//   .NumConnections = 1,  // The number of separate connections between your controller and face LED panels
//   .Connections = {
//     // Config for each connection
//     // Left & right face
//     {
//       .PIN_DataIn = 3,
//       .PIN_CS = 4,
//       .PIN_CLK = 5,
//       .NumPanels = 14,
//       // Define the order that the panels are connected
//       .Panels = {
//         // Start: Right mouth back
//         PANEL_RIGHT_MOUTH1,
//         PANEL_RIGHT_MOUTH2,
//         PANEL_RIGHT_MOUTH3,
//         PANEL_RIGHT_MOUTH4,
//         // Next: Left mouth front
//         PANEL_LEFT_MOUTH4,
//         PANEL_LEFT_MOUTH3,
//         PANEL_LEFT_MOUTH2,
//         PANEL_LEFT_MOUTH1,
//         // Next: Left eye back
//         PANEL_LEFT_EYE1,
//         PANEL_LEFT_EYE2,
//         // Next: Left nose
//         PANEL_LEFT_NOSE,
//         // Next: Right nose
//         PANEL_RIGHT_NOSE,
//         // Next: Right eye front
//         PANEL_RIGHT_EYE2,
//         PANEL_RIGHT_EYE1,
//       },
//     },
//   }
// };


// Face configuration: Separate left/right side
// TODO: My rendering code does it the opposite way around (left = left when looking out of the protogen)
// TODO: Need to handle whether the panels are upside down or not
// struct FaceConfiguration FACE_CONFIG = {
//   .NumConnections = 2,  // The number of separate connections between your controller and face LED panels
//   .Connections = {
//     // Config for each connection
//     // Left face
//     {
//       .PIN_DataIn = 6,
//       .PIN_CS = 7,
//       .PIN_CLK = 8,
//       .NumPanels = 7,
//       // Define the order that the panels are connected
//       .Panels = {
//         // Start: Left mouth back
//         PANEL_LEFT_MOUTH1,
//         PANEL_LEFT_MOUTH2,
//         PANEL_LEFT_MOUTH3,
//         PANEL_LEFT_MOUTH4,
//         // Next: Left nose
//         PANEL_LEFT_NOSE,
//         // Next: Left eye front
//         PANEL_LEFT_EYE2,
//         PANEL_LEFT_EYE1,
//       },
//     },
//     // Right face
//     {
//       .PIN_DataIn = 3,
//       .PIN_CS = 4,
//       .PIN_CLK = 5,
//       .NumPanels = 7,
//       // Define the order that the panels are connected
//       .Panels = {
//         // Start: Right mouth back
//         PANEL_RIGHT_MOUTH1,
//         PANEL_RIGHT_MOUTH2,
//         PANEL_RIGHT_MOUTH3,
//         PANEL_RIGHT_MOUTH4,
//         // Next: Right nose
//         PANEL_RIGHT_NOSE,
//         // Next: Right eye front
//         PANEL_RIGHT_EYE2,
//         PANEL_RIGHT_EYE1,
//       },
//     },
//   }
// };











// OPTION 0: Define the face in terms of panel numbers

// struct FaceLEDConnection {
//   int PIN_DATAIN;
//   int PIN_CS;
//   int PIN_CLK;
// };

// struct FaceLEDConnection FACE_CONNECTION_LEFT = {
//   .PIN_DATAIN = 3,
//   .PIN_CS = 4,
//   .PIN_CLK = 5,
// };

// struct FaceLEDConnection FACE_CONNECTION_RIGHT = {
//   .PIN_DATAIN = 6,
//   .PIN_CS = 7,
//   .PIN_CLK = 8,
// };


// struct FaceLEDConnection* FaceLEDConnections[2] = {
//   &FACE_CONNECTION_LEFT,  // Connection 0
//   &FACE_CONNECTION_RIGHT  // Connection 1
// };

// struct FaceHalf {
//   int Face_Connection_Number;
//   int EyePanelNumbers[2];
//   int NosePanelNumbers[1];
//   int MouthPanelNumbers[4];
// };


// struct FaceConfiguration {
//   FaceHalf Left;
//   FaceHalf Right;
// };




// struct FaceConfiguration Face_Config = {
//   .Left = {
//     .Face_Connection_Number = 0,
//     .EyePanelNumbers = { 5, 6 },
//     .NosePanelNumbers = { 4 },
//     .MouthPanelNumbers = { 0, 1, 2, 3 },
//   },
//   .Right = {
//     .Face_Connection_Number = 1,
//     .EyePanelNumbers = { 5, 6 },
//     .NosePanelNumbers = { 4 },
//     .MouthPanelNumbers = { 0, 1, 2, 3 },
//   }
// };

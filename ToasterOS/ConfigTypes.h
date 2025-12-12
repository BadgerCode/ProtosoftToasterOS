#define TOTAL_LED_PANELS 15
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
#define PANEL_SINGLE_NOSE 14

struct PanelConfig {
  int PanelType;
  bool UpsideDown;
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
  FaceConfig();
};

struct ProtoConfigStruct {
  // General config
  int Brightness;
  int DebugMode;

  // Boop sensor config
  bool EnableBoopSensor;
  int PinAnalogBoopSensor;
  int NumBoopsToActivateGame;

  // LED Strip config
  bool EnableSideLEDs;

  // Remote control config
  bool EnableRemoteControl;
  int PinRemoteButtonA;
  int PinRemoteButtonB;
  int PinRemoteButtonC;
  int PinRemoteButtonD;
};

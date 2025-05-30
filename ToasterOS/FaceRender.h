// Panel indexes (left and right have the same indexes)
#define FACE_PANEL_MOUTH1 3
#define FACE_PANEL_MOUTH2 2
#define FACE_PANEL_MOUTH3 1
#define FACE_PANEL_MOUTH4 0
#define FACE_PANEL_NOSE 4
#define FACE_PANEL_EYE1 5
#define FACE_PANEL_EYE2 6


struct FacePanelConfig {
  LedControl* Controller;
  int Address;
  bool UpsideDown;
};


class FaceRender {
private:
  const int Brightness = 8;  // 0 - 15
  byte EmptyPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // Face LED state
  // 2 sides with 7 panels, 8 rows per panel
  byte FaceLEDRowValues[14][8];
  bool FaceLEDRowRequiresRendering[14][8];

  // Render queue
  // 0 = eyes, 1 = nose, 2 = mouth
  unsigned int NextRenderSection = 0;

  // LED interface
  int NumLEDControls;
  LedControl** LEDControls;

  FacePanelConfig PanelMappings[14];

  int EyePanelTypes[4] = { PANEL_LEFT_EYE_BACK, PANEL_LEFT_EYE_FRONT, PANEL_RIGHT_EYE_BACK, PANEL_RIGHT_EYE_FRONT };
  int NosePanelTypes[2] = { PANEL_LEFT_NOSE, PANEL_RIGHT_NOSE };
  int MouthPanelTypes[8] = { PANEL_LEFT_MOUTH_BACK, PANEL_LEFT_MOUTH_MID_BACK, PANEL_LEFT_MOUTH_MID_FRONT, PANEL_LEFT_MOUTH_FRONT,
                             PANEL_RIGHT_MOUTH_BACK, PANEL_RIGHT_MOUTH_MID_BACK, PANEL_RIGHT_MOUTH_MID_FRONT, PANEL_RIGHT_MOUTH_FRONT };
public:
  FaceRender(FaceConfig* faceConfig) {
    NumLEDControls = faceConfig->NumConnections;
    LEDControls = new LedControl*[NumLEDControls];

    // Set up all of the LED controllers
    for (int i = 0; i < NumLEDControls; i++) {
      auto connection = faceConfig->Connections[i];
      LEDControls[i] = new LedControl(connection.PIN_DataIn, connection.PIN_CLK, connection.PIN_CS, connection.NumPanels);

      // Register all of the panel mappings
      // E.g. PANEL_LEFT_MOUTH_BACK -> Controller 0, Address 0, Upside down: false
      for (int p = 0; p < connection.NumPanels; p++) {
        auto panel = connection.Panels[p];
        PanelMappings[panel.PanelType] = { .Controller = LEDControls[i], .Address = p, .UpsideDown = panel.UpsideDown };
      }
    }
  }

  void Initialise() {
    for (int c = 0; c < NumLEDControls; c++) {
      auto controller = LEDControls[c];
      int numPanels = controller->getDeviceCount();

      for (int address = 0; address < numPanels; address++) {
        controller->shutdown(address, false);           // Disable power saving
        controller->setIntensity(address, Brightness);  // Set brightness 0-15
        controller->clearDisplay(address);              // Turn all LEDs off
      }
    }
  }


  void Clear() {
    // Mouth
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH1, EmptyPanel, false, 0);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH2, EmptyPanel, false, 0);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH3, EmptyPanel, false, 0);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH4, EmptyPanel, false, 0);

    // Nose
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_NOSE, EmptyPanel, false, 0);

    // Eyes
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_EYE1, EmptyPanel, false, 0);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_EYE2, EmptyPanel, false, 0);
  }

  void ClearPanel(int panelIndex, bool isLeft) {
    FaceRender::SetPanel(isLeft, panelIndex, EmptyPanel, false, false, 0);
  }


  void LoadFaceExpression(FaceExpression facialExpression, bool shouldBlink, int offsetY) {
    // Mouth
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH1, (facialExpression).Mouth[0], false, offsetY);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH2, (facialExpression).Mouth[1], false, offsetY);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH3, (facialExpression).Mouth[2], false, offsetY);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_MOUTH4, (facialExpression).Mouth[3], false, offsetY);

    // Nose
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_NOSE, (facialExpression).Nose[0], true, 0);

    // Eyes
    EyeFrame* eyes = shouldBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_EYE1, (*eyes)[0], true, offsetY);
    FaceRender::SetLeftAndRightPanel(FACE_PANEL_EYE2, (*eyes)[1], true, offsetY);
  }


  // Update face LED state
  void SetLeftAndRightPanel(int panelIndex, byte data[], bool isReversed, int offsetY) {
    SetPanel(true, panelIndex, data, isReversed, isReversed, offsetY);
    SetPanel(false, panelIndex, data, isReversed, !isReversed, offsetY * -1);
  }



  void SetPanel(bool isLeft, int panelIndex, byte data[], bool isReversed, bool isUpsideDown, int offsetY) {
    for (int row = 0; row < 8; row++) {
      int rowIndex = row + offsetY;
      if (rowIndex < 0 || rowIndex >= 8) {
        SetRowIfDifferent(isLeft, panelIndex, row, 0);
        continue;
      }

      byte rowData = 0;
      int rowDataIndex = isUpsideDown ? (7 - rowIndex) : rowIndex;
      if (isReversed) {
        rowData = Reverse(data[rowDataIndex]);
      } else {
        rowData = data[rowDataIndex];
      }

      SetRowIfDifferent(isLeft, panelIndex, row, rowData);
    }
  }


  void SetRowIfDifferent(bool isLeft, int panelIndex, int row, byte output) {
    // If the output hasn't changed, do nothing
    byte previousOutput = FaceLEDRowValues[isLeft ? panelIndex : panelIndex + 7][row];
    if (previousOutput == output) return;

    // Update LED output to the new output
    FaceLEDRowValues[isLeft ? panelIndex : panelIndex + 7][row] = output;
    FaceLEDRowRequiresRendering[isLeft ? panelIndex : panelIndex + 7][row] = true;

    // Rendering is handled by the render queue- processRenderQueue()
  }


  // Render face
  void ProcessRenderQueue() {
    int sectionsRendered = 0;
    int sectionsChecked = 0;


    // Render 1 section a time (eyes, nose, mouth) and avoid infinite loops
    // Render both the left and right section at the same time
    while (sectionsRendered < 1 && sectionsChecked < 3) {
      // Determine the panels, based on the section
      int firstPanel = -1;
      int lastPanel = -1;

      if (NextRenderSection == 0) {  // eyes
        firstPanel = FACE_PANEL_EYE1;
        lastPanel = FACE_PANEL_EYE2;
      } else if (NextRenderSection == 1) {  // nose
        firstPanel = FACE_PANEL_NOSE;
        lastPanel = FACE_PANEL_NOSE;
      } else if (NextRenderSection == 2) {  // mouth
        firstPanel = FACE_PANEL_MOUTH4;
        lastPanel = FACE_PANEL_MOUTH1;
      } else {  // something's gone wrong
        NextRenderSection = 0;
        break;
      }


      // Check if there's anything to render
      bool anyPanelsRendered = false;
      for (int panel = firstPanel; panel <= lastPanel; panel++) {
        for (int row = 0; row < 8; row++) {
          // Render left
          bool shouldRenderLeft = FaceLEDRowRequiresRendering[panel][row];

          // TODO: Left and right are the wrong way around
          if (shouldRenderLeft) {
            byte output = FaceLEDRowValues[panel][row];
            LEDControls[0]->setRow(panel, row, output);

            FaceLEDRowRequiresRendering[panel][row] = false;
            anyPanelsRendered = true;
          }

          // Render right
          bool shouldRenderRight = FaceLEDRowRequiresRendering[panel + 7][row];

          if (shouldRenderRight) {
            byte output = FaceLEDRowValues[panel + 7][row];
            LEDControls[1]->setRow(panel, row, output);

            FaceLEDRowRequiresRendering[panel + 7][row] = false;
            anyPanelsRendered = true;
          }
        }
      }

      sectionsChecked++;
      if (anyPanelsRendered) sectionsRendered++;

      NextRenderSection++;
      if (NextRenderSection > 2) NextRenderSection = 0;
    }
  }
};

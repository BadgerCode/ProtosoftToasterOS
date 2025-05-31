struct FacePanelConfig {
  LedControl* Controller;
  int Address;
  bool FlipX;
  bool FlipY;
};


class FaceRender {
private:
  const int Brightness = 8;  // 0 - 15
  byte EmptyPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // Face LED state
  // 2 sides with 7 panels, 8 rows per panel
  byte FaceLEDRowValues[14][8];
  bool FaceLEDRowRequiresRendering[14][8];

  // LED interface
  int NumLEDControls;
  LedControl** LEDControls;

  FacePanelConfig PanelMappings[14];

  // Rendering
  unsigned int NextRenderSection = 0;  // 0 = eyes, 1 = nose, 2 = mouth

  // Split the face into sections, so we can render all panels for a section at the same time
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
      // E.g. PANEL_LEFT_MOUTH_BACK -> Controller 0, Address 0, FlipX: false, FlipY: True
      for (int p = 0; p < connection.NumPanels; p++) {
        auto panel = connection.Panels[p];
        PanelMappings[panel.PanelType] = { .Controller = LEDControls[i], .Address = p, .FlipX = panel.FlipX, .FlipY = panel.FlipY };
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
    ClearPanel(PANEL_LEFT_MOUTH_BACK);
    ClearPanel(PANEL_LEFT_MOUTH_MID_BACK);
    ClearPanel(PANEL_LEFT_MOUTH_MID_FRONT);
    ClearPanel(PANEL_LEFT_MOUTH_FRONT);

    ClearPanel(PANEL_RIGHT_MOUTH_BACK);
    ClearPanel(PANEL_RIGHT_MOUTH_MID_BACK);
    ClearPanel(PANEL_RIGHT_MOUTH_MID_FRONT);
    ClearPanel(PANEL_RIGHT_MOUTH_FRONT);


    // Nose
    ClearPanel(PANEL_LEFT_NOSE);
    ClearPanel(PANEL_RIGHT_NOSE);

    // Eyes
    ClearPanel(PANEL_LEFT_EYE_FRONT);
    ClearPanel(PANEL_LEFT_EYE_BACK);
    ClearPanel(PANEL_RIGHT_EYE_FRONT);
    ClearPanel(PANEL_RIGHT_EYE_BACK);
  }

  void LoadFaceExpression(FaceExpression facialExpression, bool shouldBlink, int offsetY) {
    bool mirrorLeft = true;
    bool mirrorRight = false;

    // Mouth
    UpdatePanel(PANEL_LEFT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorLeft);
    UpdatePanel(PANEL_LEFT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorLeft);
    UpdatePanel(PANEL_LEFT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorLeft);
    UpdatePanel(PANEL_LEFT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorLeft);

    UpdatePanel(PANEL_RIGHT_MOUTH_FRONT, (facialExpression).Mouth[0], offsetY, mirrorRight);
    UpdatePanel(PANEL_RIGHT_MOUTH_MID_FRONT, (facialExpression).Mouth[1], offsetY, mirrorRight);
    UpdatePanel(PANEL_RIGHT_MOUTH_MID_BACK, (facialExpression).Mouth[2], offsetY, mirrorRight);
    UpdatePanel(PANEL_RIGHT_MOUTH_BACK, (facialExpression).Mouth[3], offsetY, mirrorRight);

    // Nose
    UpdatePanel(PANEL_LEFT_NOSE, (facialExpression).Nose[0], 0, mirrorLeft);
    UpdatePanel(PANEL_RIGHT_NOSE, (facialExpression).Nose[0], 0, mirrorRight);

    // Eyes
    EyeFrame* eyes = shouldBlink ? &((facialExpression).Eye_Blink) : &((facialExpression).Eye);
    UpdatePanel(PANEL_LEFT_EYE_FRONT, (*eyes)[0], offsetY, mirrorLeft);
    UpdatePanel(PANEL_LEFT_EYE_BACK, (*eyes)[1], offsetY, mirrorLeft);
    UpdatePanel(PANEL_RIGHT_EYE_FRONT, (*eyes)[0], offsetY, mirrorRight);
    UpdatePanel(PANEL_RIGHT_EYE_BACK, (*eyes)[1], offsetY, mirrorRight);
  }

  void ClearPanel(int panelType) {
    UpdatePanel(panelType, EmptyPanel, 0, false);
  }

  void UpdatePanel(int panelType, byte data[], int offsetY, bool mirror) {
    if (DEBUG_MODE == 3) {
      mirror = false;
      data = Numbers_ASCII[7];
    }
    // auto outputOverride = DEBUG_MODE == 3 ? Numbers_ASCII[7] : data;

    auto panelMapping = PanelMappings[panelType];
    bool flipX = mirror ? !panelMapping.FlipX : panelMapping.FlipX;

    for (int row = 0; row < 8; row++) {
      byte rowData = 0;
      int rowDataIndex = (panelMapping.FlipY ? (7 - row) : row) + offsetY;

      // If the offset has made this row empty, clear it
      if (rowDataIndex < 0 || rowDataIndex >= 8) {
        UpdatePanelRow(panelType, row, 0);
        continue;
      }

      if (flipX) {
        rowData = Reverse(data[rowDataIndex]);
      } else {
        rowData = data[rowDataIndex];
      }

      UpdatePanelRow(panelType, row, rowData);
    }
  }


  void UpdatePanelRow(int panelType, int row, byte output) {
    // If the output hasn't changed, do nothing
    byte previousOutput = FaceLEDRowValues[panelType][row];
    if (previousOutput == output) return;

    // Update LED output to the new output
    FaceLEDRowValues[panelType][row] = output;
    FaceLEDRowRequiresRendering[panelType][row] = true;

    // Rendering is handled by the render queue- processRenderQueue()
  }


  // Render face
  void ProcessRenderQueue() {
    int sectionsRendered = 0;
    int sectionsChecked = 0;

    // Render 1 section a time (eyes, nose, mouth) and avoid infinite loops
    // Render both the left and right section at the same time
    while (sectionsRendered < 1 && sectionsChecked < 3) {
      int* panelTypes;
      int numPanelTypes;

      // Determine the panels, based on the section
      if (NextRenderSection == 0) {  // eyes
        panelTypes = EyePanelTypes;
        numPanelTypes = 4;
      } else if (NextRenderSection == 1) {  // nose
        panelTypes = NosePanelTypes;
        numPanelTypes = 2;
      } else if (NextRenderSection == 2) {  // mouth
        panelTypes = MouthPanelTypes;
        numPanelTypes = 8;
      } else {  // something's gone wrong
        NextRenderSection = 0;
        break;
      }


      // Render all panels with updates for the section
      bool anyPanelsRendered = false;
      for (int p = 0; p < numPanelTypes; p++) {
        int panelType = panelTypes[p];
        auto panelMapping = PanelMappings[panelType];

        for (int row = 0; row < 8; row++) {
          bool shouldRender = FaceLEDRowRequiresRendering[panelType][row];

          if (shouldRender) {
            byte output = FaceLEDRowValues[panelType][row];
            panelMapping.Controller->setRow(panelMapping.Address, row, output);

            FaceLEDRowRequiresRendering[panelType][row] = false;
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

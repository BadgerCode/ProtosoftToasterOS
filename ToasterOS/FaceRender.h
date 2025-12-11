struct FacePanelConfig {
  MAX7219Control* Controller;
  int Address;
  bool UpsideDown;
  bool Enabled;
};


class FaceRender {
private:
  const int Brightness = FACE_BRIGHTNESS;  // 0 - 15
  byte EmptyPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // Face LED state
  // 2 sides with 7 panels, 8 rows per panel
  byte FaceLEDRowValues[TOTAL_LED_PANELS][8];
  bool FaceLEDRowRequiresRendering[TOTAL_LED_PANELS][8];

  // LED interface
  int NumLEDControls;
  MAX7219Control** LEDControls;

  FacePanelConfig PanelConfigs[TOTAL_LED_PANELS];

  // Rendering
  unsigned int NextRenderSection = 0;  // 0 = eyes, 1 = nose, 2 = mouth

  // Split the face into sections, so we can render all panels for a section at the same time
  int EyePanelTypes[4] = { PANEL_LEFT_EYE_BACK, PANEL_LEFT_EYE_FRONT, PANEL_RIGHT_EYE_BACK, PANEL_RIGHT_EYE_FRONT };
  int NosePanelTypes[3] = { PANEL_LEFT_NOSE, PANEL_RIGHT_NOSE, PANEL_SINGLE_NOSE };
  int MouthPanelTypes[8] = { PANEL_LEFT_MOUTH_BACK, PANEL_LEFT_MOUTH_MID_BACK, PANEL_LEFT_MOUTH_MID_FRONT, PANEL_LEFT_MOUTH_FRONT,
                             PANEL_RIGHT_MOUTH_BACK, PANEL_RIGHT_MOUTH_MID_BACK, PANEL_RIGHT_MOUTH_MID_FRONT, PANEL_RIGHT_MOUTH_FRONT };
public:
  FaceRender(FaceConfig* faceConfig) {
    NumLEDControls = faceConfig->NumConnections;
    LEDControls = new MAX7219Control*[NumLEDControls];

    for (int i = 0; i < TOTAL_LED_PANELS; i++) {
      PanelConfigs[i] = { .Controller = NULL, .Address = 0, .UpsideDown = false, .Enabled = false };
    }

    // Set up all of the LED controllers
    for (int i = 0; i < NumLEDControls; i++) {
      auto connection = faceConfig->Connections[i];
      LEDControls[i] = new MAX7219Control(connection.PIN_DataIn, connection.PIN_CS, connection.PIN_CLK, connection.NumPanels);

      // Register all of the panel mappings
      // E.g. PANEL_LEFT_MOUTH_BACK -> Controller 0, Address 0, UpsideDown: false
      for (int p = 0; p < connection.NumPanels; p++) {
        auto panel = connection.Panels[p];
        if (panel.PanelType < 0) continue;

        PanelConfigs[panel.PanelType] = { .Controller = LEDControls[i], .Address = p, .UpsideDown = panel.UpsideDown, .Enabled = true };
      }
    }
  }

  void Initialise() {
    for (int c = 0; c < NumLEDControls; c++) {
      auto controller = LEDControls[c];
      controller->Initialise();
      controller->SetBrightness(Brightness);
    }
  }

  void Clear() {
    for (int i = 0; i < TOTAL_LED_PANELS; i++) {
      ClearPanel(i);
    }
  }

  void ClearPanel(int panelType) {
    UpdatePanel(panelType, EmptyPanel, 0, false);
  }

  void UpdatePanel(int panelType, byte data[], int offsetY, bool mirror) {
    if (DEBUG_MODE == 3) {
      mirror = false;
      data = Numbers_ASCII[7];
    }

    auto panelConfig = PanelConfigs[panelType];
    bool flipX = mirror ? !panelConfig.UpsideDown : panelConfig.UpsideDown;

    if (!panelConfig.Enabled) return;  // Skip disabled panels

    for (int row = 0; row < 8; row++) {
      int rowDataIndex = (panelConfig.UpsideDown ? (7 - row) : row) + offsetY;

      // If the offset has pushed us beyond the available data, render it as empty
      byte rowData = 0;
      if (0 <= rowDataIndex && rowDataIndex < 8) {
        rowData = flipX ? Reverse(data[rowDataIndex]) : data[rowDataIndex];
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
    bool hasRenderedASection = false;

    // Check if any sections need rendering (eyes, nose, mouth)
    for (int i = 0; i < 3; i++) {
      // Only render 1 section at a time
      if (hasRenderedASection) break;

      int* panelTypes;
      int numPanelTypes;

      // Determine the panels, based on the section
      if (NextRenderSection == 0) {  // eyes
        panelTypes = EyePanelTypes;
        numPanelTypes = 4;
      } else if (NextRenderSection == 1) {  // nose
        panelTypes = NosePanelTypes;
        numPanelTypes = 3;
      } else if (NextRenderSection == 2) {  // mouth
        panelTypes = MouthPanelTypes;
        numPanelTypes = 8;
      } else {  // something's gone wrong
        NextRenderSection = 0;
        break;
      }


      // For some reason, sometimes just after booting the nose will render once but then get wiped.
      // And never render again. This fixes that.
      bool hasJustBooted = millis() < 2000;

      // Render all panels with updates for the section
      for (int p = 0; p < numPanelTypes; p++) {
        int panelType = panelTypes[p];
        auto panelConfig = PanelConfigs[panelType];

        for (int row = 0; row < 8; row++) {
          if (!FaceLEDRowRequiresRendering[panelType][row] && !hasJustBooted) continue;

          panelConfig.Controller->SetRow(panelConfig.Address, row, FaceLEDRowValues[panelType][row]);

          FaceLEDRowRequiresRendering[panelType][row] = false;
          hasRenderedASection = true;
        }
      }

      // Move onto the next section
      NextRenderSection++;
      if (NextRenderSection > 2) NextRenderSection = 0;
    }

    for (int c = 0; c < NumLEDControls; c++) {
      auto controller = LEDControls[c];
      controller->RenderDisplays();
    }
  }
};

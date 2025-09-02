struct FacePanelConfig {
  LedControl* Controller;
  int Address;
  bool FlipX;
  bool FlipY;
  bool Enabled;
};


class FaceRender {
private:
  const int Brightness = 15;  // 0 - 15
  byte EmptyPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // Face LED state
  // 2 sides with 7 panels, 8 rows per panel
  byte FaceLEDRowValues[TOTAL_LED_PANELS][8];
  bool FaceLEDRowRequiresRendering[TOTAL_LED_PANELS][8];

  // LED interface
  int NumLEDControls;
  LedControl** LEDControls;

  FacePanelConfig PanelConfigs[TOTAL_LED_PANELS];

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

    for (int i = 0; i < TOTAL_LED_PANELS; i++) {
      PanelConfigs[i] = { .Controller = NULL, .Address = 0, .FlipX = false, .FlipY = false, .Enabled = false };
    }

    // Set up all of the LED controllers
    for (int i = 0; i < NumLEDControls; i++) {
      auto connection = faceConfig->Connections[i];
      LEDControls[i] = new LedControl(connection.PIN_DataIn, connection.PIN_CLK, connection.PIN_CS, connection.NumPanels);

      // Register all of the panel mappings
      // E.g. PANEL_LEFT_MOUTH_BACK -> Controller 0, Address 0, FlipX: false, FlipY: True
      for (int p = 0; p < connection.NumPanels; p++) {
        auto panel = connection.Panels[p];
        if (panel.PanelType < 0) continue;

        PanelConfigs[panel.PanelType] = { .Controller = LEDControls[i], .Address = p, .FlipX = panel.FlipX, .FlipY = panel.FlipY, .Enabled = true };
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
    for (int i = 0; i < TOTAL_LED_PANELS; i++) {
      ClearPanel(i);
    }
  }

  void ClearPanel(int panelType) {
    // Instead of directly calling controller->clearDisplay, render an empty screen.
    // The render system avoids re-rendering rows that have not changed.
    UpdatePanel(panelType, EmptyPanel, 0, false);
  }

  void UpdatePanel(int panelType, byte data[], int offsetY, bool mirror) {
    if (DEBUG_MODE == 3) {
      mirror = false;
      data = Numbers_ASCII[7];
    }

    auto panelConfig = PanelConfigs[panelType];
    bool flipX = mirror ? !panelConfig.FlipX : panelConfig.FlipX;

    if (!panelConfig.Enabled) return; // Skip disabled panels

    for (int row = 0; row < 8; row++) {
      int rowDataIndex = (panelConfig.FlipY ? (7 - row) : row) + offsetY;

      // If the offset has pushed us beyond the available data, render it as empty
      if (rowDataIndex < 0 || rowDataIndex >= 8) {
        UpdatePanelRow(panelType, row, 0);
        continue;
      }

      byte rowData = flipX ? Reverse(data[rowDataIndex]) : data[rowDataIndex];
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
        numPanelTypes = 2;
      } else if (NextRenderSection == 2) {  // mouth
        panelTypes = MouthPanelTypes;
        numPanelTypes = 8;
      } else {  // something's gone wrong
        NextRenderSection = 0;
        break;
      }


      // Render all panels with updates for the section
      for (int p = 0; p < numPanelTypes; p++) {
        int panelType = panelTypes[p];
        auto panelConfig = PanelConfigs[panelType];

        if (!panelConfig.Enabled) return; // Skip disabled panels

        for (int row = 0; row < 8; row++) {
          if (!FaceLEDRowRequiresRendering[panelType][row]) continue;

          panelConfig.Controller->setRow(panelConfig.Address, row, FaceLEDRowValues[panelType][row]);

          FaceLEDRowRequiresRendering[panelType][row] = false;
          hasRenderedASection = true;
        }
      }

      // Move onto the next section
      NextRenderSection++;
      if (NextRenderSection > 2) NextRenderSection = 0;
    }
  }
};

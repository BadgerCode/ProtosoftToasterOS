struct FacePanelConfig {
  LedControl2* Controller;
  int Address;
  bool FlipX;
  bool FlipY;
  bool Enabled;
};


class FaceRender {
private:
  const int Brightness = 15;  // 0 - 15
  byte EmptyPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // LED interface
  int NumLEDControls;
  LedControl2** LEDControls;

  FacePanelConfig PanelConfigs[TOTAL_LED_PANELS];
public:
  FaceRender(FaceConfig* faceConfig) {
    NumLEDControls = faceConfig->NumConnections;
    LEDControls = new LedControl2*[NumLEDControls];

    for (int i = 0; i < TOTAL_LED_PANELS; i++) {
      PanelConfigs[i] = { .Controller = NULL, .Address = 0, .FlipX = false, .FlipY = false, .Enabled = false };
    }

    // Set up all of the LED controllers
    for (int i = 0; i < NumLEDControls; i++) {
      auto connection = faceConfig->Connections[i];
      LEDControls[i] = new LedControl2(connection.PIN_DataIn, connection.PIN_CS, connection.PIN_CLK, connection.NumPanels);

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
    bool flipX = mirror ? !panelConfig.FlipX : panelConfig.FlipX;

    if (!panelConfig.Enabled) return;  // Skip disabled panels

    for (int row = 0; row < 8; row++) {
      int rowDataIndex = (panelConfig.FlipY ? (7 - row) : row) + offsetY;

      // If the offset has pushed us beyond the available data, render it as empty
      byte rowData = 0;
      if (rowDataIndex > 0 && rowDataIndex < 8) {
        rowData = flipX ? Reverse(data[rowDataIndex]) : data[rowDataIndex];
      }

      panelConfig.Controller->SetRow(panelConfig.Address, row, rowData);
    }
  }

  // Render face
  void ProcessRenderQueue() {
    for (int c = 0; c < NumLEDControls; c++) {
      auto controller = LEDControls[c];
      controller->RenderDisplays();
    }
  }
};

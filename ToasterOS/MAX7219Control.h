/*
For guides and info, see
https://github.com/BadgerCode/MAX7219Control

// Setup the controller
LEDPanels = new MAX7219Control(DATAPIN, CSPIN, CLKPIN, NUMPANELS);
LEDPanels->Initialise();
LEDPanels->SetBrightness(7); // 0-15

// Use this if your panels sometimes get stuck because of a loose connection
LEDPanels->SetRapidAutoRecovery(true);

// Clear panels
LEDPanels->ClearAllPanels();
LEDPanels->ClearPanel(panelNumber);

// Update panels
LEDPanels->SetRow(panelNumber, row, B11001100);
LEDPanels->SetPanel(panel, new byte[8]{ B00000000, B00111100, B01100110, B01101110, B01110110, B01100110, B01100110, B00111100 });

// Update upside down panels
LEDPanels->SetFlippedRow(panelNumber, row, B11001100);
LEDPanels->SetFlippedPanel(panel, new byte[8]{ B00000000, B00111100, B01100110, B01101110, B01110110, B01100110, B01100110, B00111100 });

// Render any changes (at the end of every loop iteration)
LEDPanels->RenderDisplays();
*/


class MAX7219Control {
private:
  int DataPin;
  int CSPin;
  int CLKPin;

  int NumPanels;
  int NumRows = 8;

  byte Brightness = 7;  // 0-15

  bool* RowsRequiringUpdate;
  byte** NewPanelRowData;
  byte** CurrentPanelRowData;

  bool EnableRapidAutoRecovery = false;
  unsigned long LastAutoRecovery = 0;
  const int NormalAutoRecoveryDelay = 30000; // ms
  const int RapidAutoRecoveryDelay = 3000; // ms


public:
  MAX7219Control(int dataPin, int csPin, int clkPin, int numPanels) {
    DataPin = dataPin;
    CSPin = csPin;
    CLKPin = clkPin;
    NumPanels = numPanels;

    // Initialise panel data
    RowsRequiringUpdate = new bool[NumRows];
    for (int row = 0; row < NumRows; row++) {
      RowsRequiringUpdate[row] = true;
    }

    NewPanelRowData = new byte*[NumPanels];
    CurrentPanelRowData = new byte*[NumPanels];
    for (int panel = 0; panel < NumPanels; panel++) {
      NewPanelRowData[panel] = new byte[NumRows];
      CurrentPanelRowData[panel] = new byte[NumRows];

      for (int row = 0; row < NumRows; row++) {
        NewPanelRowData[panel][row] = 0;
        CurrentPanelRowData[panel][row] = 0;
      }
    }
  }

  Initialise() {
    // Set up data, cs, clk pins
    pinMode(CLKPin, OUTPUT);
    pinMode(CSPin, OUTPUT);
    pinMode(DataPin, OUTPUT);

    // Initialise all panels
    InitialisePanelState();

    // Force all rows to clear, bypassing caching
    for (int row = 1; row <= NumRows; row++) {
      SetValueForAllPanels(row, 0);
    }
  }

  /**
    Enables/Disables rapid auto-recovery (disabled by default).

    Every minute, the panels will re-initialise and re-render.
    This allows panels that get stuck/broken from a loose connection to recover and start working again.

    Enabling rapid auto-recovery will make this process happen every 5 seconds.
    This slightly reduce performance.
  */
  SetRapidAutoRecovery(bool enabled) {
    EnableRapidAutoRecovery = enabled;
  }

  // Value between 0-15
  SetBrightness(byte value) {
    Brightness = value;
    SetValueForAllPanels(10, Brightness);
  }

  ClearAllPanels() {
    for (int panel = 0; panel < NumPanels; panel++)
      ClearPanel(panel);
  }

  ClearPanel(int panelNumber) {
    for (int row = 0; row < NumRows; row++)
      SetRow(panelNumber, row, 0);
  }

  /**
  * Renders rows of data to a panel, top to bottom
  * panelNumber - Which panel the row is on. Starting from 0.
  * rowsData - one byte of data per row
  */
  SetPanel(int panelNumber, byte* rowsData) {
    for (int i = 0; i < NumRows; i++) {
      SetRow(panelNumber, i, rowsData[i]);
    }
  }

  /**
  * Renders rows of data to a panel, rotated 180 degrees
  * panelNumber - Which panel the row is on. Starting from 0.
  * rowsData - one byte of data per row
  */
  SetFlippedPanel(int panelNumber, byte* rowsData) {
    for (int i = 0; i < NumRows; i++) {
      SetRow(panelNumber, i, Reverse(rowsData[NumRows - 1 - i]));
    }
  }

  /**
  * panelNumber - Which panel the row is on. Starting from 0.
  * rowNumber - 0-7, from top to bottom
  * value - 8 bits to turn LEDs on/off, from left to right (0, 1, 2, 4, ...)
  */
  SetRow(int panelNumber, int rowNumber, byte value) {
    if (panelNumber < 0 || panelNumber > NumPanels) return;
    if (rowNumber < 0 || rowNumber >= NumRows) return;

    NewPanelRowData[panelNumber][rowNumber] = value;

    if (CurrentPanelRowData[panelNumber][rowNumber] != NewPanelRowData[panelNumber][rowNumber])
      RowsRequiringUpdate[rowNumber] = true;
  }

  /**
  * panelNumber - Which panel the row is on. Starting from 0.
  * rowNumber - 0-7, from bottom to top
  * value - 8 bits to turn LEDs on/off, from left to right (0, 1, 2, 4, ...)
  */
  SetFlippedRow(int panelNumber, int rowNumber, byte value) {
    SetRow(panelNumber, NumRows - 1 - rowNumber, Reverse(value));
  }


  byte Reverse(byte b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
  }


  RenderDisplays() {
    bool ignoreCache = false;
    // Auto-recovery for loose connections
    int autoRecoveryDelay = EnableRapidAutoRecovery ? RapidAutoRecoveryDelay : NormalAutoRecoveryDelay;
    if (millis() - LastAutoRecovery > autoRecoveryDelay) {
      // Re-initialise panels, incase of loose connections
      InitialisePanelState();
      ignoreCache = true;
    }

    // Render one row at a time for all of the panels (e.g. the first row for all panels)
    for (int row = 0; row < NumRows; row++) {
      // Only render a row if there are changes
      if (!RowsRequiringUpdate[row] && !ignoreCache) continue;
      RowsRequiringUpdate[row] = false;

      // Flip row order - the hardware orders row bottom to top; we want top to bottom
      int hardwareRow = NumRows - row;  // 1 - 8

      // Render panels, starting with the one furthest away
      // When you render a panel, the data is sent to the first one
      // If a panel already has data, it gets pushed to the next one in the chain
      digitalWrite(CSPin, LOW);
      for (int panel = (NumPanels - 1); panel >= 0; panel--) {
        // Output the row number we'll be setting
        shiftOut(DataPin, CLKPin, MSBFIRST, hardwareRow);

        // Reverse the order of the bits, by doing least significant bit first
        // This means that B10000000 will light up the left most LED, instead of the right most one
        shiftOut(DataPin, CLKPin, LSBFIRST, NewPanelRowData[panel][row]);

        CurrentPanelRowData[panel][row] = NewPanelRowData[panel][row];
      }
      digitalWrite(CSPin, HIGH);
    }
  }

private:
  InitialisePanelState() {
    SetValueForAllPanels(9, 0);  // Decode mode (always 0)
    SetValueForAllPanels(10, Brightness);
    SetValueForAllPanels(11, NumRows - 1);  // 8 rows
    SetValueForAllPanels(12, 1);            // Shutdown mode (set to normal)
    SetValueForAllPanels(15, 0);            // Disable test mode

    LastAutoRecovery = millis();
  }

  /**
  0 = do nothing
  1-8 = set output for rows 1-8
  9 = decode mode (always set to 0)
  10/A = brightness (0-15)
  11/B = scan limit (number of rows; 0 = 1 row, 7 = 8 rows)
  12/C = shutdown mode (0 = wipe the displays, 1 = normal operation)
  15/F = display test (0 = normal operation, 1 = turn on all LEDs)
  */
  SetValueForAllPanels(int setting, int value) {
    digitalWrite(CSPin, LOW);
    for (int panel = 0; panel < NumPanels; panel++) {
      shiftOut(DataPin, CLKPin, MSBFIRST, setting);
      shiftOut(DataPin, CLKPin, MSBFIRST, value);
    }
    digitalWrite(CSPin, HIGH);
  }
};

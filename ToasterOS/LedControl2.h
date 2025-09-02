/*
Comparison
For n panels, 8 rows per panel
Ops = number of data frames transmitted

- Render row 1 to panel 1
  - LedControl: n ops
  - LedControl2: n ops
- Render row 1 to all panels
  - LedControl: 8n ops
  - LedControl2: n ops
- Render all rows to panel 1
  - LedControl: 8n ops
  - LedControl2: 8n ops
- Render all rows to all panels
  - LedControl: 8n^2 ops
  - LedControl2: 8n ops

For 14 panels, re-rendering everything
  - LedControl: 1568 ops
  - LedControl2: 112 ops
*/




class LedControl2 {
private:
  int DataPin;
  int CSPin;
  int CLKPin;

  int NumPanels;
  int NumRows = 8;

  byte** NewPanelRowData;
  byte** CurrentPanelRowData;


public:
  LedControl2(int dataPin, int csPin, int clkPin, int numPanels) {
    DataPin = dataPin;
    CSPin = csPin;
    CLKPin = clkPin;
    NumPanels = numPanels;

    // Initialise panel data
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
    SetValueForAllPanels(9, 0);             // Decode mode (always 0)
    SetValueForAllPanels(11, NumRows - 1);  // 8 rows
    SetValueForAllPanels(12, 1);            // Shutdown mode (set to normal)
    SetValueForAllPanels(15, 0);            // Disable test mode

    // Force all rows to clear, bypassing caching
    // Have to do it a few times for some reason
    for (int i = 0; i < 1; i++) {
      for (int row = 1; row <= NumRows; row++) {
        SetValueForAllPanels(row, 0);
      }
    }
  }

  // Value between 0-15
  SetBrightness(byte value) {
    SetValueForAllPanels(10, value);
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
  * panelNumber - Which panel the row is on. Starting from 0.
  * rowNumber - 0-7, from top to bottom
  * value - 8 bits to turn LEDs on/off, from left to right (0, 1, 2, 4, ...)
  */
  SetRow(int panelNumber, int rowNumber, byte value) {
    if (panelNumber < 0 || panelNumber > NumPanels) return;
    if (rowNumber < 0 || rowNumber >= NumRows) return;

    NewPanelRowData[panelNumber][rowNumber] = value;
  }


  RenderDisplays() {
    // Render one row at a time for all of the panels (e.g. the first row for all panels)
    for (int row = 0; row < NumRows; row++) {
      // Flip row order - the hardware orders row bottom to top; we want top to bottom
      int hardwareRow = NumRows - row;  // 1 - 8

      // Only render a row if there are changes
      bool anyChanges = false;
      for (int panel = 0; panel < NumPanels; panel--) {
        if (NewPanelRowData[panel][row] != CurrentPanelRowData[panel][row]) {
          anyChanges = true;
          break;
        }
      }
      if (!anyChanges) continue;

      // Render panels, starting with the one furthest away
      // When you render a panel, the data is sent to the first one
      // If a panel already has data, it gets pushed to the next one in the chain
      digitalWrite(CSPin, LOW);
      for (int panel = (NumPanels - 1); panel >= 0; panel--) {
        shiftOut(DataPin, CLKPin, MSBFIRST, hardwareRow);
        shiftOut(DataPin, CLKPin, MSBFIRST, NewPanelRowData[panel][row]);

        CurrentPanelRowData[panel][row] = NewPanelRowData[panel][row];
      }
      digitalWrite(CSPin, HIGH);
    }
  }

private:
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

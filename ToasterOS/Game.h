
// TODO: Put this somewhere else
unsigned long timeSince(unsigned long previousTime) {
  return millis() - previousTime;
}


bool GAME_Initalised = false;
unsigned long GAME_TouchStarted = 0;

int GAME_ObstacleX = -2;  // bottom left corner
unsigned long GAME_LastScroll = 0;


void renderOutput(FaceRender* faceRenderer, bool output[][8]);
void drawRect(bool output[][8], int rectX, int rectY, int width, int height);


void GameInit(FaceRender* faceRenderer) {
  faceRenderer->Clear();
}

bool GameLoop(FaceRender* faceRenderer, bool boopSensorTouched) {
  if (!GAME_Initalised) GameInit(faceRenderer);

  // Quit by holding the boop for 10 seconds
  if (!boopSensorTouched) GAME_TouchStarted = 0;
  else if (GAME_TouchStarted == 0) GAME_TouchStarted = millis();
  else if (millis() - GAME_TouchStarted > 10000) return false;


  // Scroll game to the side
  if (timeSince(GAME_LastScroll) > 100) {
    GAME_LastScroll = millis();

    GAME_ObstacleX++;
    if (GAME_ObstacleX > 33) GAME_ObstacleX = -2;
  }

  bool output[32][8] = {};

  // Floor
  drawRect(output, 0, 0, 32, 1);

  // Character
  int charX = 27;
  int charY = boopSensorTouched ? 5 : 1;
  drawRect(output, charX, charY, 3, 3);

  // Obstacle 1
  drawRect(output, GAME_ObstacleX, 1, 2, 2);


  renderOutput(faceRenderer, output);
  faceRenderer->ProcessRenderQueue();  // For some reason, this tanks FPS compared to the normal face???

  return true;
}


// x,y = bottom-left corner
void drawRect(bool output[][8], int rectX, int rectY, int width, int height) {
  for (int x = rectX; x < (rectX + width); x++) {
    if (x < 0 || x > 31) continue;

    for (int y = rectY; y < (rectY + height); y++) {
      if (y < 0 || y > 7) continue;
      output[x][y] = true;
    }
  }
}


void renderOutput(FaceRender* faceRenderer, bool output[][8]) {
  byte panel1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  byte panel2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  byte panel3[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  byte panel4[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  for (int c = 0; c < 32; c++) {
    int panel = c / 8;
    int panelColumnIndex = c % 8;
    int rowBinaryValue = 1 << (7 - panelColumnIndex);

    for (int r = 0; r < 8; r++) {
      if (output[c][r] == false) continue;

      int panelRowIndex = 7 - r;  // panel rows start at the top. game starts at the bottom

      switch (panel) {
        case 0:
          panel1[panelRowIndex] |= rowBinaryValue;
          break;
        case 1:
          panel2[panelRowIndex] |= rowBinaryValue;
          break;
        case 2:
          panel3[panelRowIndex] |= rowBinaryValue;
          break;
        case 3:
          panel4[panelRowIndex] |= rowBinaryValue;
          break;
      }
    }
  }

  // From front to back
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_MOUTH1, panel1, false, 0);
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_MOUTH2, panel2, false, 0);
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_MOUTH3, panel3, false, 0);
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_MOUTH4, panel4, false, 0);
}

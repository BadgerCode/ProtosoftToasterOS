struct GameObstacle {
  int x;
  int height;
  int width;
};


// TODO: Put this somewhere else
unsigned long timeSince(unsigned long previousTime) {
  return millis() - previousTime;
}


bool GAME_Initalised = false;
unsigned long GAME_TouchStarted = 0;

const int GAME_NumObstacles = 5;
struct GameObstacle GAME_Obstacles[GAME_NumObstacles];

int GAME_ObstacleX = -2;  // bottom left corner
unsigned long GAME_LastScroll = 0;
int GAME_ScrollDelay = 100;


unsigned long GAME_JumpStarted = 0;
int GAME_JumpDuration = 800;
int GAME_JumpDelay = 100;


int GAME_HP = 3;  // 4 max
int GAME_SCORE = 0;


void renderOutput(FaceRender* faceRenderer, bool output[][8]);
void renderScore(FaceRender* faceRenderer, int hp, int score);
void drawRect(bool output[][8], int rectX, int rectY, int width, int height);


void GameInit(FaceRender* faceRenderer) {
  faceRenderer->Clear();


  for (int i = 0; i < GAME_NumObstacles; i++) {
    GAME_Obstacles[i].x = -2 - (i * 12);
    GAME_Obstacles[i].height = (i % 2) == 0 ? 2 : 3;
    GAME_Obstacles[i].width = 2;
  }
}

bool GameLoop(FaceRender* faceRenderer, bool boopSensorTouched) {
  if (!GAME_Initalised) GameInit(faceRenderer);

  // Quit by holding the boop for 10 seconds
  if (!boopSensorTouched) GAME_TouchStarted = 0;
  else if (GAME_TouchStarted == 0) GAME_TouchStarted = millis();
  else if (millis() - GAME_TouchStarted > 10000) return false;


  // Scroll game to the side
  if (timeSince(GAME_LastScroll) > GAME_ScrollDelay) {
    GAME_LastScroll = millis();

    GAME_ObstacleX++;
    if (GAME_ObstacleX > (GAME_NumObstacles * 12 + 50)) GAME_ObstacleX = -2;
  }


  // Jump detection
  bool isJumping = GAME_JumpStarted != 0 && timeSince(GAME_JumpStarted) < GAME_JumpDuration;
  if (!isJumping && timeSince(GAME_JumpStarted) > (GAME_JumpDuration + GAME_JumpDelay) && boopSensorTouched) {
    GAME_JumpStarted = millis();
    isJumping = true;
  }


  // Generate output
  bool output[32][8] = {};

  // Floor
  drawRect(output, 0, 0, 32, 1);

  // Character
  int charX = 27;
  int charY = isJumping ? 5 : 1;
  drawRect(output, charX, charY, 3, 3);

  // Obstacle
  for (int i = 0; i < GAME_NumObstacles; i++) {
    struct GameObstacle obstacle = GAME_Obstacles[i];
    drawRect(output, obstacle.x + GAME_ObstacleX, 1, obstacle.width, obstacle.height);
  }


  renderOutput(faceRenderer, output);
  renderScore(faceRenderer, GAME_HP, GAME_SCORE);
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

void renderScore(FaceRender* faceRenderer, int hp, int score) {
  byte panel1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  byte panel2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  if (hp > 0) {
    panel1[0] |= B10100000;
    panel1[1] |= B11100000;
    panel1[2] |= B01000000;
  }
  if (hp > 1) {
    panel1[0] |= B00001010;
    panel1[1] |= B00001110;
    panel1[2] |= B00000100;
  }
  if (hp > 2) {
    panel2[0] |= B10100000;
    panel2[1] |= B11100000;
    panel2[2] |= B01000000;
  }
  if (hp > 3) {
    panel2[0] |= B00001010;
    panel2[1] |= B00001110;
    panel2[2] |= B00000100;
  }

  // From front to back
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_EYE1, panel1, true, 0);
  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_EYE2, panel2, true, 0);
}

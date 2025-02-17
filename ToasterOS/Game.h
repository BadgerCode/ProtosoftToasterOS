struct Rect {
  int x;
  int y;
  int height;
  int width;
};


bool GAME_Initalised = false;
unsigned long GAME_TouchStarted = 0;

const int GAME_NumObstacles = 5;
struct Rect GAME_Obstacles[GAME_NumObstacles];

int GAME_OffsetX = 0;               // Game state
unsigned long GAME_LastScroll = 0;  // Game state
int GAME_ScrollDelay = 100;


unsigned long GAME_JumpStarted = 0;  // Game state
int GAME_JumpDuration = 1000;
int GAME_JumpDelay = 100;


int GAME_HP = 4;                           // Game state; 4 max
unsigned long GAME_PlayerLastTookHit = 0;  // Game state
int GAME_SCORE = 0;                        // Game state


void renderOutput(FaceRender* faceRenderer, bool output[][8]);
void renderScore(FaceRender* faceRenderer, int hp, int score);
void drawRect(bool output[][8], int rectX, int rectY, int width, int height);
bool areRectsColliding(Rect rect1, Rect rect2);


void GameInit(FaceRender* faceRenderer) {
  faceRenderer->Clear();

  GAME_OffsetX = 0;
  GAME_HP = 4;
  GAME_PlayerLastTookHit = 0;
  GAME_SCORE = 0;
  GAME_JumpStarted = 0;
  GAME_LastScroll = 0;

  for (int i = 0; i < GAME_NumObstacles; i++) {
    GAME_Obstacles[i].x = -2 - (i * 12);
    GAME_Obstacles[i].y = 1;
    GAME_Obstacles[i].height = (i % 2) == 0 ? 2 : 3;
    GAME_Obstacles[i].width = 2;
  }

  GAME_Initalised = true;
}

bool GameLoop(FaceRender* faceRenderer, bool boopSensorTouched) {
  if (!GAME_Initalised) GameInit(faceRenderer);

  // Quit by holding the boop for 10 seconds
  if (!boopSensorTouched) GAME_TouchStarted = 0;
  else if (GAME_TouchStarted == 0) GAME_TouchStarted = millis();
  else if (millis() - GAME_TouchStarted > 10000) {
    GAME_Initalised = false;
    return false;
  }


  // Scroll game to the side
  if (timeSince(GAME_LastScroll) > GAME_ScrollDelay) {
    GAME_LastScroll = millis();

    for (int i = 0; i < GAME_NumObstacles; i++) {
      GAME_Obstacles[i].x++;
    }

    GAME_OffsetX++;
    if (GAME_OffsetX >= (GAME_NumObstacles * 12 + 50)) {
      GAME_OffsetX = 0;

      for (int i = 0; i < GAME_NumObstacles; i++) {
        GAME_Obstacles[i].x = -2 - (i * 12);
      }
    }
  }

  // Jump detection
  bool isJumping = GAME_JumpStarted != 0 && timeSince(GAME_JumpStarted) < GAME_JumpDuration;
  if (!isJumping && timeSince(GAME_JumpStarted) > (GAME_JumpDuration + GAME_JumpDelay) && boopSensorTouched) {
    GAME_JumpStarted = millis();
    isJumping = true;
  }


  // Player
  struct Rect player = { 27, (isJumping ? 5 : 1), 3, 3 };


  // hit detection
  bool playerHasBeenHit = false;
  for (int i = 0; i < GAME_NumObstacles; i++) {
    struct Rect obstacle = GAME_Obstacles[i];
    if (areRectsColliding(player, obstacle)) playerHasBeenHit = true;
  }

  if (playerHasBeenHit && timeSince(GAME_PlayerLastTookHit) > 1000) {
    GAME_HP--;
    GAME_PlayerLastTookHit = millis();

    if (GAME_HP <= 0) {
      GAME_Initalised = false;
      return true;
    }
  }


  // Generate output
  bool output[32][8] = {};

  // Floor
  drawRect(output, 0, 0, 32, 1);

  // Character
  drawRect(output, player.x, player.y, player.width, player.height);

  // Obstacle
  for (int i = 0; i < GAME_NumObstacles; i++) {
    struct Rect obstacle = GAME_Obstacles[i];
    drawRect(output, obstacle.x, obstacle.y, obstacle.width, obstacle.height);
  }


  renderOutput(faceRenderer, output);
  renderScore(faceRenderer, GAME_HP, GAME_SCORE);
  faceRenderer->ProcessRenderQueue();

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

bool areRectsColliding(Rect rect1, Rect rect2) {
  // if (rect2.x > 29 || (rect2.x + rect2.width) < 27)
  // if (rect2.x < 32)
  // return false;

  // rect 1: 0,0 size: 2x2
  // rect 2: 1,0 size: 2x2

  int minX = rect2.x;                 // 1
  int maxX = rect2.x + rect2.width;   // 3
  int minY = rect2.y;                 // 0
  int maxY = rect2.y + rect2.height;  // 2

  // If rect 1 is to the left or to the right of rect 2
  if (rect1.x + rect1.width < minX || rect1.x > maxX)  // 0 + 2 < 1 || 0 > 3
    return false;

  // If rect 1 is below or above rect 2
  if (rect1.y + rect1.height < minY || rect1.y > maxY)  // 0 + 2 < 0 || 0 > 2
    return false;

  return true;
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

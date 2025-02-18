struct Rect {
  // x,y = bottom-left corner
  int x;
  int y;
  int height;
  int width;
};


bool GAME_Initalised = false;
bool GAME_GameOver = false;
unsigned long GAME_Started = 0;  // Game state
int GAME_RoundNumber = 0;        // Game state
unsigned long GAME_Ended = 0;    // Game state
unsigned long GAME_TouchStarted = 0;

const int GAME_NumObstacles = 12;
struct Rect GAME_Obstacles[GAME_NumObstacles];

unsigned long GAME_LastScroll = 0;  // Game state
int GAME_ScrollDelay = 100;


unsigned long GAME_JumpStarted = 0;  // Game state
int GAME_JumpDuration = 1000;
int GAME_JumpDelay = 100;


int GAME_HP = 4;                           // Game state; 4 max
unsigned long GAME_PlayerLastTookHit = 0;  // Game state
int GAME_InvulnerabilityDuration = 3000;   //

// TODO: Use a class instead (I hate the need for forward declaration)
void ResetObstacles();
void Gameover(FaceRender* faceRenderer, LEDStripRender* ledStripRenderer, bool boopSensorTouched);
void renderOutput(FaceRender* faceRenderer, bool output[][8]);
void renderHUD(FaceRender* faceRenderer, int hp);
void drawRect(bool output[][8], int rectX, int rectY, int width, int height);
bool areRectsColliding(Rect rect1, Rect rect2);


void GameInit(FaceRender* faceRenderer) {
  faceRenderer->Clear();

  GAME_Started = millis();
  GAME_RoundNumber = 0;
  GAME_Ended = 0;
  GAME_GameOver = false;
  GAME_HP = 4;
  GAME_PlayerLastTookHit = 0;
  GAME_JumpStarted = 0;
  GAME_LastScroll = 0;

  ResetObstacles();

  GAME_Initalised = true;
}

bool GameLoop(FaceRender* faceRenderer, LEDStripRender* ledStripRenderer, bool boopSensorTouched) {
  if (!GAME_Initalised) GameInit(faceRenderer);

  // Quit by holding the boop for 10 seconds
  if (!boopSensorTouched) GAME_TouchStarted = 0;
  else if (GAME_TouchStarted == 0) GAME_TouchStarted = millis();
  else if (timeSince(GAME_TouchStarted) > 10000) {
    GAME_Initalised = false;
    return false;
  }

  // Show gameover screen
  if (GAME_GameOver) {
    Gameover(faceRenderer, ledStripRenderer, boopSensorTouched);
  } else {
    // Player
    struct Rect player = { 27, 1, 3, 3 };

    // Scroll game to the side
    if (timeSince(GAME_LastScroll) > GAME_ScrollDelay) {
      GAME_LastScroll = millis();

      for (int i = 0; i < GAME_NumObstacles; i++) {
        GAME_Obstacles[i].x++;
      }

      // Reset obstacles once the last one has left the screen
      if (GAME_Obstacles[GAME_NumObstacles - 1].x > 40) {
        ResetObstacles();
      }
    }

    // Jump detection
    unsigned long timeSinceJump = timeSince(GAME_JumpStarted);
    bool isJumping = GAME_JumpStarted != 0 && timeSinceJump < GAME_JumpDuration;
    if (!isJumping && timeSinceJump > (GAME_JumpDuration + GAME_JumpDelay) && boopSensorTouched) {
      GAME_JumpStarted = millis();
      isJumping = true;
    }

    if (isJumping) player.y = (timeSinceJump < 100 || timeSinceJump > (GAME_JumpDuration - 100)) ? 3 : 5;


    // hit detection
    bool playerIsInvulnerable = GAME_PlayerLastTookHit != 0 && timeSince(GAME_PlayerLastTookHit) < GAME_InvulnerabilityDuration;
    if (!playerIsInvulnerable) {
      bool obstacleCollision = false;
      for (int i = 0; i < GAME_NumObstacles; i++) {
        struct Rect obstacle = GAME_Obstacles[i];
        if (areRectsColliding(player, obstacle)) obstacleCollision = true;
      }


      // Game over detection
      if (obstacleCollision) {
        GAME_HP--;
        GAME_PlayerLastTookHit = millis();

        if (GAME_HP <= 0) {
          GAME_GameOver = true;
          GAME_Ended = millis();
        }
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
    renderHUD(faceRenderer, GAME_HP);


    // Side lights
    if (playerIsInvulnerable) {
      ledStripRenderer->SetAllLEDs(CHSV(7, 242, 255));
    } else if (isJumping) {
      ledStripRenderer->SetAllLEDs(CHSV(135, 242, 255));
    } else {
      ledStripRenderer->SetAllLEDs(CHSV(163, 242, 255));
    }
  }

  faceRenderer->ProcessRenderQueue();
  ledStripRenderer->Render();
  return true;
}

unsigned long getScore() {
  return (GAME_Ended - GAME_Started) / 1000;
}

void Gameover(FaceRender* faceRenderer, LEDStripRender* ledStripRenderer, bool boopSensorTouched) {
  // Restart the game by holding the boop sensor for 5 seconds
  unsigned long timeSinceTouchStarted = timeSince(GAME_TouchStarted);
  if (GAME_TouchStarted != 0 && timeSinceTouchStarted > 5000) {
    GAME_Initalised = false;
    return;
  }


  unsigned long score = getScore();
  if (score > 999) score = 999;

  int digit1 = (score / 100) % 10;
  int digit2 = (score / 10) % 10;
  int digit3 = score % 10;

  bool isReversed = true;
  // Left
  faceRenderer->SetPanel(true, FACE_PANEL_NOSE, Numbers_ASCII[digit1], isReversed, isReversed, 0);
  faceRenderer->SetPanel(true, FACE_PANEL_EYE1, Numbers_ASCII[digit2], isReversed, isReversed, 0);
  faceRenderer->SetPanel(true, FACE_PANEL_EYE2, Numbers_ASCII[digit3], isReversed, isReversed, 0);

  // Right
  faceRenderer->SetPanel(false, FACE_PANEL_NOSE, Numbers_ASCII[digit3], !isReversed, !isReversed, 0);
  faceRenderer->SetPanel(false, FACE_PANEL_EYE1, Numbers_ASCII[digit2], !isReversed, !isReversed, 0);
  faceRenderer->SetPanel(false, FACE_PANEL_EYE2, Numbers_ASCII[digit1], !isReversed, !isReversed, 0);

  // Side lights
  if (boopSensorTouched) {
    int hue = 0 + ((float)timeSinceTouchStarted / 5000.0f * 135.0f);
    ledStripRenderer->SetAllLEDs(CHSV(hue, 242, 255));
  } else {
    ledStripRenderer->SetAllLEDs(CHSV(0, 242, 255));
  }
}


bool areRectsColliding(Rect rect1, Rect rect2) {
  return rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x && rect1.y < rect2.y + rect2.height && rect1.y + rect1.height > rect2.y;
}

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

void renderHUD(FaceRender* faceRenderer, int hp) {
  byte panel1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  byte panel2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  // HP
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


void ResetObstacles() {
  GAME_RoundNumber++;

  int minHeight = 2;
  int maxHeight = 2;
  int minWidth = 2;
  int maxWidth = 3;
  int minDistance = 12;

  if (GAME_RoundNumber >= 2) {
    maxHeight = 3;
  }
  if (GAME_RoundNumber >= 3) {
    minDistance = 11;
  }
  if (GAME_RoundNumber >= 4) {
    minWidth = 3;
    maxWidth = 4;
  }
  if (GAME_RoundNumber >= 5) {
    minDistance = 10;
    maxWidth = 5;
  }
  if (GAME_RoundNumber >= 6) {
    minHeight = 3;
    maxHeight = 4;
  }


  int currentX = -5;

  for (int i = 0; i < GAME_NumObstacles; i++) {
    GAME_Obstacles[i].x = currentX;
    GAME_Obstacles[i].y = 1;
    GAME_Obstacles[i].height = random(minHeight, maxHeight + 1);
    GAME_Obstacles[i].width = random(minWidth, maxWidth + 1);

    currentX -= random(minDistance, 15);
  }
}
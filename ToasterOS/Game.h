#define GAME_NUM_OBSTACLES 12

struct Rect {
  // x,y = bottom-left corner
  int x;
  int y;
  int height;
  int width;
};


class CubeGame {
private:
  bool GAME_Initalised = false;
  bool GAME_GameOver = false;      // Game state
  unsigned long GAME_Started = 0;  // Game state
  int GAME_RoundNumber = 0;        // Game state
  unsigned long GAME_Ended = 0;    // Game state
  unsigned long GAME_TouchStarted = 0;

  struct Rect GAME_Obstacles[GAME_NUM_OBSTACLES];

  unsigned long GAME_LastScroll = 0;  // Game state
  int GAME_ScrollDelay = 100;


  unsigned long GAME_JumpStarted = 0;  // Game state
  int GAME_JumpDuration = 1000;
  int GAME_JumpDelay = 100;


  int GAME_HP = 4;                           // Game state; 4 max
  unsigned long GAME_PlayerLastTookHit = 0;  // Game state
  int GAME_InvulnerabilityDuration = 3000;   //

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

public:
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

        for (int i = 0; i < GAME_NUM_OBSTACLES; i++) {
          GAME_Obstacles[i].x++;
        }

        // Reset obstacles once the last one has left the screen
        if (GAME_Obstacles[GAME_NUM_OBSTACLES - 1].x > 40) {
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
        for (int i = 0; i < GAME_NUM_OBSTACLES; i++) {
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
      for (int i = 0; i < GAME_NUM_OBSTACLES; i++) {
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
    if (ProtoConfig.EnableSideLEDs) ledStripRenderer->Render();
    return true;
  }


private:
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

    // Don't mirror the numbers
    bool mirrorDigits = false;

    // Right
    if (score < 100) {
      // Don't use the nose for scores less than 100
      faceRenderer->ClearPanel(PANEL_RIGHT_NOSE);
      faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, Numbers_ASCII[digit2], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_BACK, Numbers_ASCII[digit3], 0, mirrorDigits);
    } else {
      // Use the nose for the hundreds
      faceRenderer->UpdatePanel(PANEL_RIGHT_NOSE, Numbers_ASCII[digit1], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, Numbers_ASCII[digit2], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_BACK, Numbers_ASCII[digit3], 0, mirrorDigits);
    }

    // Left
    if (score < 100) {
      // Don't use the nose for scores less than 100
      faceRenderer->UpdatePanel(PANEL_LEFT_EYE_BACK, Numbers_ASCII[digit2], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, Numbers_ASCII[digit3], 0, mirrorDigits);
      faceRenderer->ClearPanel(PANEL_LEFT_NOSE);
    } else {
      // Use the nose for the ones when the score is above 100
      faceRenderer->UpdatePanel(PANEL_LEFT_EYE_BACK, Numbers_ASCII[digit1], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, Numbers_ASCII[digit2], 0, mirrorDigits);
      faceRenderer->UpdatePanel(PANEL_LEFT_NOSE, Numbers_ASCII[digit3], 0, mirrorDigits);
    }

    // Side lights
    if (boopSensorTouched) {
      int hue = 0 + ((float)timeSinceTouchStarted / 5000.0f * 135.0f);
      ledStripRenderer->SetAllLEDs(CHSV(hue, 242, 255));
    } else {
      ledStripRenderer->SetAllLEDs(CHSV(0, 242, 255));
    }
  }


  unsigned long getScore() {
    return (GAME_Ended - GAME_Started) / 1000;
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

    for (int i = 0; i < GAME_NUM_OBSTACLES; i++) {
      GAME_Obstacles[i].x = currentX;
      GAME_Obstacles[i].y = 1;
      GAME_Obstacles[i].height = random(minHeight, maxHeight + 1);
      GAME_Obstacles[i].width = random(minWidth, maxWidth + 1);

      currentX -= random(minDistance, 15);
    }
  }



  // Rendering
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
    // From front to back
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

    bool mirrorLeft = true;
    faceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_FRONT, panel1, 0, mirrorLeft);
    faceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_FRONT, panel2, 0, mirrorLeft);
    faceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_MID_BACK, panel3, 0, mirrorLeft);
    faceRenderer->UpdatePanel(PANEL_LEFT_MOUTH_BACK, panel4, 0, mirrorLeft);

    bool mirrorRight = false;
    faceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_FRONT, panel1, 0, mirrorRight);
    faceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_FRONT, panel2, 0, mirrorRight);
    faceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_MID_BACK, panel3, 0, mirrorRight);
    faceRenderer->UpdatePanel(PANEL_RIGHT_MOUTH_BACK, panel4, 0, mirrorRight);
  }

  void renderHUD(FaceRender* faceRenderer, int hp) {
    // From front to back
    byte lowerHealthPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    byte upperHealthPanel[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    // HP
    if (hp > 0) {
      lowerHealthPanel[0] |= B00001010;
      lowerHealthPanel[1] |= B00001110;
      lowerHealthPanel[2] |= B00000100;
    }
    if (hp > 1) {
      lowerHealthPanel[0] |= B10100000;
      lowerHealthPanel[1] |= B11100000;
      lowerHealthPanel[2] |= B01000000;
    }
    if (hp > 2) {
      upperHealthPanel[0] |= B00001010;
      upperHealthPanel[1] |= B00001110;
      upperHealthPanel[2] |= B00000100;
    }
    if (hp > 3) {
      upperHealthPanel[0] |= B10100000;
      upperHealthPanel[1] |= B11100000;
      upperHealthPanel[2] |= B01000000;
    }

    bool mirrorLeft = true;
    faceRenderer->UpdatePanel(PANEL_LEFT_EYE_FRONT, upperHealthPanel, 0, mirrorLeft);
    faceRenderer->UpdatePanel(PANEL_LEFT_EYE_BACK, lowerHealthPanel, 0, mirrorLeft);

    bool mirrorRight = false;
    faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_FRONT, upperHealthPanel, 0, mirrorRight);
    faceRenderer->UpdatePanel(PANEL_RIGHT_EYE_BACK, lowerHealthPanel, 0, mirrorRight);
  }



  // Util
  bool areRectsColliding(Rect rect1, Rect rect2) {
    return rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x && rect1.y < rect2.y + rect2.height && rect1.y + rect1.height > rect2.y;
  }
};

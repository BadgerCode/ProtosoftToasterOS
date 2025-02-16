
bool GAME_Initalised = false;
unsigned long GAME_TouchStarted = 0;


void GameInit(FaceRender* faceRenderer) {
  faceRenderer->Clear();
}

bool GameLoop(FaceRender* faceRenderer, bool boopSensorTouched) {
  if (!GAME_Initalised) GameInit(faceRenderer);

  // Quit by holding the boop for 10 seconds
  if (!boopSensorTouched) GAME_TouchStarted = 0;
  else if (GAME_TouchStarted == 0) GAME_TouchStarted = millis();
  else if (millis() - GAME_TouchStarted > 10000) return false;



  byte panelData[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  if (boopSensorTouched) {
    panelData[1] = B00011000;
    panelData[2] = B00011000;
  } else {
    panelData[3] = B00011000;
    panelData[4] = B00011000;
  }

  faceRenderer->SetLeftAndRightPanel(FACE_PANEL_MOUTH4, panelData, false, 0);

  faceRenderer->ProcessRenderQueue();

  return true;
}

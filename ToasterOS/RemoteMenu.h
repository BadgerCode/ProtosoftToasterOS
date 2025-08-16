#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_C 2
#define BUTTON_D 3


class RemoteMenuClass {
private:
  // Button press tracking
  bool PressedButtons[4] = { 0, 0, 0, 0 };
  bool ShortPressButtons[4] = { 0, 0, 0, 0 };
  int ShortPressSingleButton = -1;
  bool LongPressButtons[4] = { 0, 0, 0, 0 };
  unsigned long ButtonPressStart = 0;
  const int HoldDurationMs = 3000;

  // Menu tracking
  unsigned long MenuSelectionLastPress = 0;
  int MenuButton = -1;
  int MenuPage = 1;
  int MenuItem = -1;
  const int MenuSelectionWaitMs = 5000;

public:
  int SelectedMenuButton() {
    return MenuButton;
  }
  int SelectedMenuPage() {
    return MenuPage;
  }
  int SelectedMenuItemButton() {
    return MenuItem;
  }

  bool ButtonIsDown(int button) {
    return PressedButtons[button];
  }

  bool ButtonWasTapped(int button) {
    return ShortPressButtons[button];
  }

  bool ButtonWasHeld(int button) {
    return LongPressButtons[button];
  }

  void Update() {
    // Reset menu state (selection made last frame or long press made last frame or timeout)
    bool anyButtonsLongPressed = LongPressButtons[0] || LongPressButtons[1] || LongPressButtons[2] || LongPressButtons[3];
    if (MenuItem != -1 || anyButtonsLongPressed || MenuButton != -1 && timeSince(MenuSelectionLastPress) >= MenuSelectionWaitMs) {
      MenuButton = -1;
      MenuPage = 1;
      MenuItem = 0;
    }

    UpdateButtonPressState();

    // Check for menu selection
    if (ShortPressSingleButton != -1) {
      // Menu selection
      if (MenuButton == -1) {
        MenuButton = ShortPressSingleButton;
        MenuSelectionLastPress = millis();
      }
      // Page selection
      else if (ShortPressSingleButton == MenuButton) {
        MenuPage++;
        MenuSelectionLastPress = millis();
      }
      // Item Selection
      else {
        MenuItem = ShortPressSingleButton;
        MenuSelectionLastPress = millis();
      }
    }
  }

private:
  void UpdateButtonPressState() {
    // Reset press state
    ShortPressSingleButton = -1;
    for (int i = 0; i < 4; i++) {
      ShortPressButtons[i] = 0;
      LongPressButtons[i] = 0;
    }

    bool NewPressedButtons[4] = {
      digitalRead(PIN_REMOTE_BUTTON_A),
      digitalRead(PIN_REMOTE_BUTTON_B),
      digitalRead(PIN_REMOTE_BUTTON_C),
      digitalRead(PIN_REMOTE_BUTTON_D)
    };

    bool anyButtonsAlreadyPressed = PressedButtons[0] || PressedButtons[1] || PressedButtons[2] || PressedButtons[3];
    bool anyButtonsNowPressed = NewPressedButtons[0] || NewPressedButtons[1] || NewPressedButtons[2] || NewPressedButtons[3];

    // Nothing happening
    if (!anyButtonsAlreadyPressed && !anyButtonsNowPressed) return;

    // Buttons just started being pressed
    if (!anyButtonsAlreadyPressed && anyButtonsNowPressed) ButtonPressStart = millis();

    // Buttons just released
    if (anyButtonsAlreadyPressed && !anyButtonsNowPressed) {
      bool isLongPress = timeSince(ButtonPressStart) >= HoldDurationMs;
      ButtonPressStart = 0;

      if (isLongPress) {
        LongPressButtons[0] = PressedButtons[0];
        LongPressButtons[1] = PressedButtons[1];
        LongPressButtons[2] = PressedButtons[2];
        LongPressButtons[3] = PressedButtons[3];
      } else {
        ShortPressButtons[0] = PressedButtons[0];
        ShortPressButtons[1] = PressedButtons[1];
        ShortPressButtons[2] = PressedButtons[2];
        ShortPressButtons[3] = PressedButtons[3];

        // Find if a single button was short pressed
        for (int i = 0; i < 4; i++) {
          if (!ShortPressButtons[i]) continue;

          // Multiple buttons were short pressed
          if (ShortPressSingleButton != -1) {
            ShortPressSingleButton = -1;
            break;
          }

          ShortPressSingleButton = i;
        }
      }
    }

    // Update pressed state
    PressedButtons[0] = NewPressedButtons[0];
    PressedButtons[1] = NewPressedButtons[1];
    PressedButtons[2] = NewPressedButtons[2];
    PressedButtons[3] = NewPressedButtons[3];
  }
};
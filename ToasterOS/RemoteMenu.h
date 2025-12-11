#define BUTTON_A 1
#define BUTTON_B 2
#define BUTTON_C 4
#define BUTTON_D 8


class RemoteMenuClass {
private:
  // Buttons currently down (bitwise)
  int ButtonPressState = 0;

  // Buttons that have been pressed this frame (bitwise)
  int NewPressedButtonsState = 0;

public:
  bool IsButtonDown(int button) {
    return ButtonPressState & button;
  }

  // Returns a number representing all of the buttons that have been pressed this frame
  // Returns 0 if no buttons have been pressed this frame
  // If multiple buttons have been pressed, then multiple bits will be set
  // E.g. 3 = BUTTON_A (1) and BUTTON_B (2)
  int GetOnPressButtons() {
    return NewPressedButtonsState;
  }

  void Update() {
    int buttonsPressed = 0;
    if (digitalRead(PIN_REMOTE_BUTTON_A)) buttonsPressed += 1;
    if (digitalRead(PIN_REMOTE_BUTTON_B)) buttonsPressed += 2;
    if (digitalRead(PIN_REMOTE_BUTTON_C)) buttonsPressed += 4;
    if (digitalRead(PIN_REMOTE_BUTTON_D)) buttonsPressed += 8;

    // If no buttons were down but buttons are now down, they're new
    if (ButtonPressState == 0 && buttonsPressed != 0) {
      NewPressedButtonsState = buttonsPressed;
    } else {
      // No new buttons have been pressed
      NewPressedButtonsState = 0;
    }

    // Update state
    ButtonPressState = buttonsPressed;
  }
};
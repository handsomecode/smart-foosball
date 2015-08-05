#include <Button.h>
#include "Arduino.h"

#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
                           //switch is closed, this is negative logic, i.e. a high state
                           //means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.

class ControlPanel {
    
  public:
    ControlPanel(byte increaseButtonPin, byte decreaseButtonPin);
    ~ControlPanel();
    
    bool isIncreaseButtonPressed();
    bool isDecreaseButtonPressed();
    bool isButtonPressedTogether();
    bool isIncreaseButtonLongPressed();
    bool isDecreaseButtonLongPressed();
    bool isButtonsLongPressedTogether();
    void update();

  private:
    byte increaseButtonPin;
    byte decreaseButtonPin;

    Button* increaseButton;
    Button* decreaseButton;
};

#endif

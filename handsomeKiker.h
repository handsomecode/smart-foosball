

#include <Bounce2.h>
#include "Arduino.h"

//#define DEBUG_MODE
#define DEBUG_LDR
//#define DEBUG_BUTTON

//==================================== Display =====================================
class Display {
  public:

    Display(int _dataPin, int _latchPin, int _clockPin);
    void setup();
    void displaySigns(int signOne, int signTwo);

    void displayNumber(int number);

  protected:
    byte dataPin; //Пин подключеный к SER (data) входу SN74HC595N
    byte latchPin;//Пин подключеный к SRCLR (latch) входу SN74HC595N
    byte clockPin;//Пин подключеный к SRCLK (clock) входу SN74HC595N
};
//==================================== Gate =====================================

//Порог фоторезистора считающийся как "Нет сигнала"
#define HEALTH_TRESHOLD 50
//Число милисеунд без сигнала свидетельствующих об ошибке
#define HEALTH_TIMER 5000
#define BOUNCE_INTERVAL 500

class Gate {
  public:
    Gate(int _ldrPin);
    void setup();
    bool checkHealth();
    bool checkHealthTimer();
    bool update();
  protected:
    byte ldrPin;
    int lastLdrValue;
    unsigned long health_tag;
    unsigned long bounce_tag;
};
//==================================== Team =====================================
#define BUTTON_BOUNCE_TIME 5
class Team {
  public:
    static volatile int finalScore;
    Team(Display* _teamDisplay, Gate* _teamGate, byte _incrButtonPin, byte _decrButtonPin, byte _tonePin);
    void increaseScore();
    void decreaseScore();
    void resetScore();
    void setup();
    void updateGate();
    void updateButtons();
    void celebrateVictory();
    void reset();
    byte getScore();
    bool endGame;

  protected:

    byte score;
    byte incrButtonPin;
    byte decrButtonPin;
    byte tonePin;
    Display* teamDisplay;
    Gate* teamGate;
    Bounce* debouncerIncrButton;
    Bounce* debouncerDecrButton;
};



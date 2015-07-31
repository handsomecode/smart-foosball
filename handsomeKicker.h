

#include <Bounce2.h>
#include "Arduino.h"

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
#define BOUNCE_INTERVAL 500

class Gate {
  public:
    Gate(int _ldrPin);
    void setup();
    bool update();
  protected:
    byte ldrPin;
    int lastLdrValue;
    unsigned long bounce_tag;
};
//==================================== Team =====================================
#define BUTTON_BOUNCE_TIME 5
class Team {
  public:
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

//==================================== ButtonGroup =====================================
class ButtonGroup {
    /*
     * Класс обертка вокруг двух кнопок
     * - фиксирует нажатие одной кнопки
     * - нажатие двух кнопок (c учетом дребезга)
     * - долгое нажатие одной или двух кнопок
     */
  public:
    bool isIncreasePress();
    bool isDecreasePress();
    bool isDoublePress();
    bool isIcreaseLongPress();
    bool isDecreaseLongPress();
    bool isDoubleLongPress();

};

//==================================== Game =====================================
class Game {
    /**
     * Ограничение счета до 15 или 20 или 5 по 3
     */
  public:
    void _start(); //инициализируем
    void _stop(); // прекращаем считать
    void update(); //обновляем компоненты, проверяем победные условия
    void toneVictory(); //пищим победу
  protected:
    Team* teamA_;
    Team* teamB_;
};

//==================================== Referee =====================================
/*
 *  Создает и удаляет объекты
 *  Сбрасывает стол при простое
 *  Меняет настройки игры
 *  Управляет отображением на экране
 */
class Referee {

  public:

    Referee(Display* displayA, Display* displayB, Gate* GateA, Gate* GateB, byte incrButtonPinA, byte decrButtonPinA, byte tonePinA, byte incrButtonPinB, byte decrButtonPinB, byte tonePinB);
    void setup();
    void update();

  protected:
    Display*      displayA_;
    Display*      displayB_;
    Gate*         gateA_;
    Gate*         gateB_;
    Team*         teamA_;
    Team*         teamB_;
    int finalScore;


};




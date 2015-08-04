

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
//==================================== Detector =====================================
/*
 * Инкапсулирует работу с фоторезистором
 *  - bounce
 *  - goal detect
 */
//Порог фоторезистора считающийся как "Нет сигнала"
#define HEALTH_TRESHOLD 50
#define BOUNCE_INTERVAL 500

class Detector {
  public:
    Detector(int _ldrPin);
    void setup();
    bool update();
  protected:
    byte ldrPin;
    int lastLdrValue;
    unsigned long bounce_tag;
};
//==================================== ButtonsGroup =====================================
class ButtonsGroup {
    /*
     * Класс обертка вокруг двух кнопок
     * - фиксирует нажатие одной кнопки
     * - нажатие двух кнопок (c учетом дребезга)
     * - долгое нажатие одной или двух кнопок
     */
  public:
    ButtonsGroup(byte incrButtonPin, byte decrButtonPin);
    bool isIncreasePress();
    bool isDecreasePress();
    bool isDoublePress();
    bool isIcreaseLongPress();
    bool isDecreaseLongPress();
    bool isDoubleLongPress();
    void setup();
    void update();
  protected:
    byte incrButtonPin_;
    byte decrButtonPin_;
    Bounce* debouncerIncrButton_;
    Bounce* debouncerDecrButton_;
    int incrButtonValue_;
    int decrButtonValue_;
    //const BUTTON_BOUNCE_TIME =5;

};



//==================================== Team =====================================
#define BUTTON_BOUNCE_TIME 5
/*
 * Следит за своим счетом  и отображает изменение на своем дисплее
 * Следит за своими кнопками
 */
class Team {
  public:
    Team(Display* _teamDisplay, Detector* _teamDetector, ButtonsGroup* buttonsGroup, byte _tonePin);
    void increaseScore();
    void decreaseScore();
    void resetScore();
    bool update();
    void reset();
    byte getScore();
  protected:
    byte score;
    byte tonePin;
    Display* display_;
    Detector* detector_;
    ButtonsGroup* buttonsGroup_;
};
//==================================== Game =====================================
class Game {
    /**
     * Ограничение счета до 15 или 20 или 5 по 3
     */
  public:
    Game();
    Game(byte finalScore);
    void _start(); //инициализируем
    void _stop(); // прекращаем считать
    byte isGameOver(byte scoreA, byte scoreB);
    bool isRun();

  protected:
    byte finalScore_;
    bool gameRun_;

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

    Referee(Display* displayA, Display* displayB, Detector* detectorA, Detector* detectorB, ButtonsGroup* buttonsGroupA, ButtonsGroup* buttonsGroupB, byte tonePinA, byte tonePinB);
    void setup();
    void update();

  protected:

    Display*      displayA_;
    Display*      displayB_;
    Detector*         detectorA_;
    Detector*         detectorB_;
    ButtonsGroup* buttonsGroupA_;
    ButtonsGroup* buttonsGroupB_;
    byte tonePinA_;
    byte tonePinB_;
    Team*         teamA_;
    Team*         teamB_;
    Game* currentGame_;
    bool settingsMode_;

    void celebrateVictory(byte winner);


};




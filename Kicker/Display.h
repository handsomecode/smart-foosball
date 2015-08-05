#include "Arduino.h"

#ifndef DISPLAY_H
#define DISPLAY_H

#define C 0x39
#define F 0x71
#define G 0x3D
#define O 0x3F
#define DASH 0x40
#define NONE 0

class Display {

    public:
        Display(byte dataPin, byte latchPin, byte clockPin);
        void displayNumber(int number);
        void displayDash();
        void displayGO();
        void displayCF();
        void switchOff();
    private:
        void initPins();
        void setSymbols(byte highDigit, byte lowDigit);

        const byte numbers[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6F};

        byte dataPin; //Пин подключеный к SER (data) входу SN74HC595N
        byte latchPin;//Пин подключеный к SRCLR (latch) входу SN74HC595N
        byte clockPin;//Пин подключеный к SRCLK (clock) входу SN74HC595N
};

#endif

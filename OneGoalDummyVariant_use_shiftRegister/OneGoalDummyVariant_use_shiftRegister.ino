#include "ShiftRegister595.h"
#include <Multiplex7Seg.h>
//#include "SevSeg.h"


byte seven_seg_digits[10][7] = { { 1, 1, 1, 1, 1, 1, 0 }, // = 0
  { 0, 1, 1, 0, 0, 0, 0 }, // = 1
  { 1, 1, 0, 1, 1, 0, 1 }, // = 2
  { 1, 1, 1, 1, 0, 0, 1 }, // = 3
  { 0, 1, 1, 0, 0, 1, 1 }, // = 4
  { 1, 0, 1, 1, 0, 1, 1 }, // = 5
  { 1, 0, 1, 1, 1, 1, 1 }, // = 6
  { 1, 1, 1, 0, 0, 0, 0 }, // = 7
  { 1, 1, 1, 1, 1, 1, 1 }, // = 8
  { 1, 1, 1, 0, 0, 1, 1 } // = 9
};

//byte numbers[10] = { 0b1000000, 0b1111001, 0b0100100, 0b0110000, 0b0011001, 0b0010010,
//0b0000010, 0b1111000, 0b0000000, 0b0010000 };
//byte numbers[10] = { 24, 222, 52, 148, 210, 145,
//17, 220, 16, 144};

int ldrPin = 0; // light dependent resistor
int lastLdrValue = 0;
int SCORE_TRESHOLD = 50;
volatile int currentScore = 0;
volatile boolean lastDecrButton = LOW;
volatile boolean currentDecrButton = LOW;
int decrButtonPin = 21;
int incrButtonPin = 3;

int i = 0;

byte latchPin = 8;  //Pin connected to ST_CP of 74HC595
byte clockPin = 12; //Pin connected to SH_CP of 74HC595
byte dataPin = 11;  //Pin connected to DS    of 74HC595

byte latch2Pin = 4;  //Pin connected to ST_CP of 74HC595
byte clock2Pin = 7; //Pin connected to SH_CP of 74HC595
byte data2Pin = 6;  //Pin connected to DS    of 74HC595

byte number[10] = {B01110111, B00010100, B10110011, B10110110, B11010100, B11100110, B11100111, B00110100, B11110111, B11110110};

//ShiftRegister595 shiftRegister595 = ShiftRegister595( latchPin , clockPin , dataPin );

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initializeArduinoPins();
  writeDigit(0);
  attachInterrupt(0, resetScore, RISING);
  attachInterrupt(1, increaseScore, RISING);
  attachInterrupt(2, decreaseScore, RISING);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(latch2Pin, OUTPUT);
  pinMode(clock2Pin, OUTPUT);
  pinMode(data2Pin, OUTPUT);

  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
//  pinMode(42, OUTPUT);

//  writeNumber(11);
}

void loop() {

  int currentLdrValue = analogRead(ldrPin);
//  Serial.println(currentLdrValue);
  if (currentLdrValue < lastLdrValue - SCORE_TRESHOLD) {
    ++currentScore;
    if (currentScore > 9) {
      currentScore = 0;
    }
    writeDigit(currentScore);
//    setNumber(currentScore + 10);
    delay(1000);
  }
  lastLdrValue = currentLdrValue;
//
//  digitalWrite(40, HIGH);
//  digitalWrite(41, HIGH);
//  digitalWrite(42, HIGH);

digitalWrite(40, HIGH);
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, number[0]);
    digitalWrite(latchPin, HIGH);
    delay(5);
    
    digitalWrite(40, LOW);

    digitalWrite(41, HIGH);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, number[0]);
    digitalWrite(latchPin, HIGH);
    delay(5);
    digitalWrite(40, LOW);

//  for(int k = 0; k < 10; k++){
//    digitalWrite(latch2Pin, LOW);
//    shiftOut(data2Pin, clock2Pin, LSBFIRST, number[k]);
//    digitalWrite(latch2Pin, HIGH);
//
//    digitalWrite(latchPin, LOW);
//    shiftOut(dataPin, clockPin, LSBFIRST, number[k]);
//    digitalWrite(latchPin, HIGH);
//
////    writeLDig(k);
////    if(k == 16) {
////      writeHDig(B10000000);
////      writeLDig(B10000000);
////    } else {
////      writeNumber(k);
////    }
//    delay(1500);
//  }
  

  
}

void writeHDig(byte digit) {
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, number[digit]);
    digitalWrite(latchPin, HIGH);
}

void writeLDig(byte digit) {
  digitalWrite(latch2Pin, LOW);
    shiftOut(data2Pin, clock2Pin, LSBFIRST, number[digit]);
    digitalWrite(latch2Pin, HIGH);
}

void writeDigit(byte digit) {
  int currentPin = 30;
  for (byte segment = 0; segment < 7; ++segment) {
    digitalWrite(currentPin, seven_seg_digits[digit][segment]);
    ++currentPin;
  }
}

void writeDot(byte dot) {
  digitalWrite(37, dot);
}

void initializeArduinoPins() {
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  writeDot(0);  // start with the "dot" off

  pinMode(2, INPUT);
  pinMode(19, INPUT);
}

void resetScore() {
  currentScore = 0;
  writeDigit(currentScore);
}

void increaseScore() {
  if(currentScore < 9) {
    currentScore++;
    writeDigit(currentScore);  
  } 
}

void decreaseScore() {
  if (currentScore > 0) {
    currentDecrButton = debounce(decrButtonPin);
    Serial.println(currentDecrButton);
  if(currentDecrButton == HIGH){
      currentScore--;
      writeDigit(currentScore);
    }
  }
}

boolean debounce(int pin) {
  delay(5);
  boolean current = digitalRead(pin);
  return current;
}

void writeDigit(int startPin, byte digit) {
  for (byte segment = 0; segment < 7; ++segment) {
    digitalWrite(startPin, seven_seg_digits[digit][segment]);
    ++startPin;
  }
}

void writeNumber(int number) {
  byte h = number / 10;
  writeHDig(h);
  byte l = number % 10;
  writeLDig(l);
}


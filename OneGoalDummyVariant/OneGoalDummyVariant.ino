#include "ShiftRegister595.h"

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

byte numbers[10] = { 0b1000000, 0b1111001, 0b0100100, 0b0110000, 0b0011001, 0b0010010,
0b0000010, 0b1111000, 0b0000000, 0b0010000 };

int ldrPin = 0; // light dependent resistor
int lastLdrValue = 0;
int SCORE_TRESHOLD = 50;
volatile int currentScore = 0;
volatile boolean lastDecrButton = LOW;
volatile boolean currentDecrButton = LOW;
int decrButtonPin = 21;
int incrButtonPin = 3;

int latchPin = 9;
int clockPin = 10;
int dataPin = 11;

int latch2Pin = 5;
int clock2Pin = 6;
int data2Pin = 7;

int latch3Pin = 50;
int clock3Pin = 51;
int data3Pin = 52;

int i = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initializeArduinoPins();
  writeDigit(0);
  attachInterrupt(0, resetScore, RISING);
  attachInterrupt(1, increaseScore, RISING);
  attachInterrupt(2, decreaseScore, RISING);

//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);

//  pinMode(latch2Pin, OUTPUT);
//  pinMode(clock2Pin, OUTPUT);
//  pinMode(data2Pin, OUTPUT);

//  writeNumber(11);

  pinMode(latch3Pin, OUTPUT);
  pinMode(clock3Pin, OUTPUT);
  pinMode(data3Pin, OUTPUT);
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
  if(i < 10) {
    digitalWrite(latch3Pin, HIGH);
    shiftOut(data3Pin, clock3Pin, MSBFIRST, numbers[i]);
    Serial.println(numbers[i]);
    i++;
    digitalWrite(latch3Pin, LOW);
  } else {
    i = 0;
  }
  
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
//  writeDigit(40, h);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, numbers[h]);
  digitalWrite(latchPin, HIGH);

  byte l = number % 10;
  digitalWrite(latch2Pin, LOW);
  shiftOut(data2Pin, clock2Pin, MSBFIRST, numbers[l]);
  digitalWrite(latch2Pin, HIGH);
  
  delay(500);
  
//  byte l = number % 10;
//  writeDigit(50, l);
}


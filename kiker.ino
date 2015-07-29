#include <Bounce2.h>


//TODO
/*
 * Больше меньше
 * Финал игры
 * Победная музыка
 * ДОлгое нажатие
 * Мигание
 * Ошибка
 * Изменени натсроек
 * Рефракторинг
 * ОБщий сброс
 * 
 */


//Пин подключен к SER входу SN74HC595N
int dataPinA = 2;
//Пин подключеный к SRCLR входу SN74HC595N
int latchPinA = 3;
//Пин подключеный к SRCLK входу SN74HC595N
int clockPinA = 4;
//Пин подключен к SER входу SN74HC595N
int dataPinB = 7;
//Пин подключеный к SRCLR входу SN74HC595N
int latchPinB = 8;
//Пин подключеный к SRCLK входу SN74HC595N
int clockPinB = 9;

int ldrPinA = 0;
int ldrPinB = 1;


volatile int currentScoreA = 0;
volatile int currentScoreB = 0;
volatile int finalScore = 15;
int decrButtonPinA = 5;
int incrButtonPinA = 6;

int decrButtonPinB = 10;
int incrButtonPinB = 11;

int tonePinA = 12;
int tonePinB = 13;




// Instantiate a Bounce object
Bounce debouncerIncrButtonA = Bounce();
Bounce debouncerDecrButtonA = Bounce();
Bounce debouncerIncrButtonB = Bounce();
Bounce debouncerDecrButtonB = Bounce();

volatile int lastLdrAValue = 0;
volatile int lastLdrBValue = 0;
int SCORE_TRESHOLD = 150;




void setup() {


  Serial.begin(9600);
  initializeArduinoPins();

  debouncerIncrButtonA.attach(incrButtonPinA);
  debouncerIncrButtonA.interval(5);

  debouncerDecrButtonA.attach(decrButtonPinA);
  debouncerDecrButtonA.interval(5);


  debouncerIncrButtonB.attach(incrButtonPinB);
  debouncerIncrButtonB.interval(5);

  debouncerDecrButtonB.attach(decrButtonPinB);
  debouncerDecrButtonB.interval(5);
  
  
  displayWrite(currentScoreA);
  displayWriteB(currentScoreB);

  
}
void initializeArduinoPins() {
  pinMode(latchPinA, OUTPUT);
  pinMode(dataPinA, OUTPUT);
  pinMode(clockPinA, OUTPUT);
  pinMode(latchPinB, OUTPUT);
  pinMode(dataPinB, OUTPUT);
  pinMode(clockPinB, OUTPUT);

  pinMode(tonePinA, OUTPUT);
  pinMode(tonePinB, OUTPUT);
  
  pinMode(incrButtonPinA, INPUT_PULLUP);
  pinMode(decrButtonPinA, INPUT_PULLUP);
   pinMode(incrButtonPinB, INPUT_PULLUP);
  pinMode(decrButtonPinB, INPUT_PULLUP);

}

void resetScore(int command) {
  currentScoreA = 0;
  displayWrite(currentScoreA);
}
void resetScoreB(int command) {
  currentScoreB = 0;
  displayWriteB(currentScoreB);
}

void increaseScore(int command) {
  if ( (currentScoreA ) < finalScore) {
    currentScoreA++;
    displayWrite(currentScoreA);
    if (currentScoreA == finalScore){
      tone (tonePinA, 500); //включаем на 500 Гц
      delay(100);
      noTone(tonePinA);
    }
  }
}

void increaseScoreB(int command) {
  if ( (currentScoreB ) < finalScore ) {
    currentScoreB++;
    displayWriteB(currentScoreB);
    if (currentScoreB == finalScore){
      tone (tonePinB, 500); //включаем на 500 Гц
      delay(100);
      noTone(tonePinB);
    }
  }
}


void decreaseScore(int command) {
  if (currentScoreA > 0) {
    currentScoreA = currentScoreA - 1;
    displayWrite(currentScoreA);
  }

}

void decreaseScoreB(int command) {
  if (currentScoreB > 0) {
    currentScoreB = currentScoreB - 1;
    displayWriteB(currentScoreB);
  }

}


void displayWrite(int number) {

  int numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
  digitalWrite(latchPinA, LOW);
  shiftOut(dataPinA, clockPinA, MSBFIRST, numbers[number % 10]);
  shiftOut(dataPinA, clockPinA, MSBFIRST, numbers[(number / 10) % 10]);
  digitalWrite(latchPinA, HIGH);

}
void displayWriteB(int number) {

  int numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
  digitalWrite(latchPinB, LOW);
  shiftOut(dataPinB, clockPinB, MSBFIRST, numbers[number % 10]);
  shiftOut(dataPinB, clockPinB, MSBFIRST, numbers[(number / 10) % 10]);
  digitalWrite(latchPinB, HIGH);

}


void loop()
{

  debouncerIncrButtonA.update();
  debouncerDecrButtonA.update();
  debouncerIncrButtonB.update();
  debouncerDecrButtonB.update();

  int incrButtonValueA = debouncerIncrButtonA.read();
  int decrButtonValueA = debouncerDecrButtonA.read();
  int incrButtonValueB = debouncerIncrButtonB.read();
  int decrButtonValueB = debouncerDecrButtonB.read();

  if ( incrButtonValueA == HIGH && decrButtonValueA == HIGH ) {
    resetScore(0);
    Serial.println("reset");
  }
   if ( incrButtonValueB == HIGH && decrButtonValueB == HIGH ) {
    resetScoreB(0);
    Serial.println("resetB");
  }

  if (debouncerIncrButtonA.fell()) {
    increaseScore(0);
    
    Serial.println("increase");
  }

  if (debouncerDecrButtonA.fell()) {
    decreaseScore(0);
    Serial.println("decrease");
  }

   if (debouncerIncrButtonB.fell()) {
    increaseScoreB(0);
    Serial.println("increaseB");
  }

  if (debouncerDecrButtonB.fell()) {
    decreaseScoreB(0);
    Serial.println("decreaseB");
  }


  int currentLdrAValue = analogRead(ldrPinA);
  Serial.print("current ");
  Serial.print(currentLdrAValue);
  Serial.print(" last ");
  Serial.print(lastLdrAValue);
  Serial.print(" score ");
  Serial.println(currentScoreA);
  if (currentLdrAValue < lastLdrAValue - SCORE_TRESHOLD) {
    Serial.println("yes");
    increaseScore(0);
    delay(500);
  }
  lastLdrAValue = currentLdrAValue;


 int currentLdrBValue = analogRead(ldrPinB);

  if (currentLdrBValue < lastLdrBValue - SCORE_TRESHOLD) {
    
    increaseScoreB(0);
    delay(500);
  }
  lastLdrBValue = currentLdrBValue;




}

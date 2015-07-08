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

int ldrPin = 0; // light dependent resistor
int lastLdrValue = 0;
int SCORE_TRESHOLD = 50;
volatile int currentScore = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initializeArduinoPins();
  writeDigit(0);
  attachInterrupt(0, resetScore, RISING);
  attachInterrupt(1, increaseScore, RISING);
  attachInterrupt(4, decreaseScore, CHANGE);
}

void loop() {

  if (currentScore > 9) {
    currentScore = 0;
    writeDigit(currentScore);
  }

  int currentLdrValue = analogRead(ldrPin);
  Serial.println(currentLdrValue);
  if (currentLdrValue < lastLdrValue - SCORE_TRESHOLD) {
    writeDigit(++currentScore);
    delay(1000);
  }
  lastLdrValue = currentLdrValue;

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
    currentScore--;
    writeDigit(currentScore);
  }
}


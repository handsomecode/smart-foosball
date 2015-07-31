#include "WinnerMelody.h"
 
byte speakerPin;
 
// MELODIES and TIMING //
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
 
// Melody 2: Star Wars Theme
int melody2[] = {  f4,  f4, f4,  a4s,   f5,  d5s,  d5,  c5, a5s, f5, d5s,  d5,  c5, a5s, f5, d5s, d5, d5s,   c5};
int beats2[]  = {  21,  21, 21,  128,  128,   21,  21,  21, 128, 64,  21,  21,  21, 128, 64,  21, 21,  21, 128 }; 
 
int MAX_COUNT = sizeof(melody2) / 2; // Melody length, for looping.
 
// Set overall tempo
long tempo = 10000;
 
// Set length of pause between notes
int pause = 1000; 
 
// Loop variable to increase Rest length (BLETCHEROUS HACK; See NOTES)
int rest_count = 50; 
 
// Initialize core variables
int toneM = 0;
int beat = 0;
long duration  = 0;
 
// PLAY TONE  //
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (toneM > 0) { // if this isn't a Rest beat, while the tone has 
    
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {
      digitalWrite(speakerPin,HIGH);
      delayMicroseconds(toneM / 2);
 
      // DOWN
      digitalWrite(speakerPin, LOW);
      delayMicroseconds(toneM / 2);
 
      // Keep track of how long we pulsed
      elapsed_time += (toneM);
    }
     
  } else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

void playMelody(byte tonePin) {
  speakerPin = tonePin;
  pinMode(speakerPin, OUTPUT);
  
  for (int i = 0; i < MAX_COUNT; i++) {
    toneM = melody2[i];
    beat = beats2[i];
 
    // Set up timing
    duration = beat * tempo; 
 
    playTone(); 

    // A pause between notes
    delayMicroseconds(pause);
  }
}

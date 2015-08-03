//based on this article http://courses.ischool.berkeley.edu/i262/s13/content/mariespliid/lab-5-star-wars-song-selector

#include "WinnerMelody.h"
 
byte speakerPin;
 
// MELODIES and TIMING //
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note)

// Melody 1: Star Wars Imperial March
int imperialMarchMelody[] = {  a4, R,  a4, R,  a4, R,  f4, R, c5, R,  a4, R,  f4, R, c5, R, a4, R,  e5, R,  e5, R,  e5, R,  f5, R, c5, R,  g5, R,  f5, R,  c5, R, a4, R};
int imperialMarchBeats[]  = {  50, 20, 50, 20, 50, 20, 40, 5, 20, 5,  60, 10, 40, 5, 20, 5, 60, 80, 50, 20, 50, 20, 50, 20, 40, 5, 20, 5,  60, 10, 40, 5,  20, 5, 60, 40};
 
// Melody 2: Star Wars Theme
int starWarsThemeMelody[] = {  f4,  f4, f4,  a4s,   f5,  d5s,  d5,  c5, a5s, f5, d5s,  d5,  c5, a5s, f5, d5s, d5, d5s,   c5};
int starWarsThemeBeats[]  = {  21,  21, 21,  128,  128,   21,  21,  21, 128, 64,  21,  21,  21, 128, 64,  21, 21,  21, 128 }; 
 
int MAX_COUNT_THEME = sizeof(starWarsThemeMelody) / 2; // Melody length, for looping.
int MAX_COUNT_IMPERIAL_MARCH = sizeof(imperialMarchMelody) / 2;
 
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

void playMelody(byte tonePin, int* melody, int* beats, int melodyLength) {
  speakerPin = tonePin;
  pinMode(speakerPin, OUTPUT);

  for (int i = 0; i < melodyLength; i++) {
    toneM = melody[i];
    beat = beats[i];
 
    // Set up timing
    duration = beat * tempo; 
 
    //play toneM
    playTone(); 

    // A pause between notes
    delayMicroseconds(pause);
  }
}

void playStarWarsTheme(byte tonePin) {
  playMelody(tonePin, starWarsThemeMelody, starWarsThemeBeats, MAX_COUNT_THEME);
}

void playImperialMarch(byte tonePin) {
  playMelody(tonePin, imperialMarchMelody, imperialMarchBeats, MAX_COUNT_IMPERIAL_MARCH);
}


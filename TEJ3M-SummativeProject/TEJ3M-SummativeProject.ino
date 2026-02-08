/* Author : Sinan Mohammed
   Date : Jan 19th, 2026. 
   Description: KARAOKE BOOGIE NIGHTS!
   This code runs with an LCD, 5 LED lights and a buzzer. The LCD displays scrolling lyrics from right to left.
   The LEDs turn into a disco light show, they are turned on based on the note playing. The buzzer will be used
   to play the music. You get to pick between 2 songs which will display karaoke.

   ||Version Control||
   Version 1 : Setting up LCD, adding variables and adding error trapping.
   Version 2 : Added Lyric scrolling.
   Version 3 : Second song added.
   Version 4 : Added Tone and Disco Lights.
   Version 5 : Error Delay and Song finished bools added.
   Version 6 : Fixing Lyric and tone sync.
   Version 7 : Fixing scroll end and syncing.
   Version 8 : Implemented Flash Memory and added Happy Birthday Song.
   Version 9 : Added play again feature.
   Version 10 : Fixed Tempo, lyric scrolling and added delay for led to turn off when same note plays.
   Version 11 : Added a dimming Led at each note.
   Version 12 : Commenting/Indentation.
 */
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "pitches.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
#define BUZZER_PIN   7
// LED PINS
#define LED_PIN1   8
#define LED_PIN2 9
#define LED_PIN3 10
#define LED_PIN4 11
#define LED_PIN5 12
#define DIM_LED_PIN 3

#define TEMPO 1800
// Total notes
#define SONG1_NOTES 51
#define SONG2_NOTES 26

// Game end
bool finish = false;
unsigned long previousMillis_finish = 0;
const int finishDelay = 1000;
bool gameOver = false;

// Player choices
char choice;
char playAgainChoice;

//Lyrics 
const char song1[] PROGMEM = "Jingle Bells, Jingle Bells, Jingle all the Way, Oh what fun it is to ride on a one horse open sleigh, Hey! Jingle Bells, Jingle Bells, Jingle all the Way, Oh what fun it is to ride on a one horse open sleigh!                 ";
const char song2[] PROGMEM = "Happy Birthday To You, Happy Birthday To You, Happy Birthday Dear XXX, Happy Birthday To You!            ";
int lyricTrack = 0;
char lyricBuffer[17]; // 16 chars + null

// Notes
const int melody[2][51] PROGMEM = {
  {NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4},
  {NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4, NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4}
};
int noteTrack = 0;
unsigned long previousMillis_note = 0;
bool notePlaying = false;

// Duration of each note
const int noteDurations[2][51] PROGMEM = {
  {4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 4, 2, 2, 4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 4, 1},
  {4, 4, 4, 4, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1}
};
int noteDuration = 0;

int currentSong = -1; // -1 = no song playing, 0 = song1, 1 = song2

// LED millis for visible blinking
unsigned long previousMillis_led = 0;
const unsigned long ledOffTime = 40; // 40 ms = very visible
bool ledWaiting = false;
int pendingLEDNote = -1;

// Dimming LED variables.
int ledBrightness = 0;
unsigned long previousMillis_dim = 0;
const int dimInterval = 10; // speed of fade

// error
unsigned long previousMillis_error = 0;
const int errorDelay = 1500;
bool error = false;

void setup() {
  // put your setup code here, to run once:'
  Serial.begin(9600);
  // setting pin modes.
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DIM_LED_PIN, OUTPUT);
  for (int i = 8; i < 13; i++) {
    pinMode(i, OUTPUT);
  }
  // setting up LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
}
// Methods

//playSong method plays every note and scrolls the lyrics by 4 chars as each note is played.
void playSong(int songNum,  const char* songText) {

  unsigned long currentMillis = millis();

  int totalNotes = (songNum == 0) ? SONG1_NOTES : SONG2_NOTES;
  // when the song ends.
  if (noteTrack >= totalNotes) {
    noTone(BUZZER_PIN);
    lcd.clear();
    offLEDs();
    lyricTrack = 0;
    noteTrack = 0;
    finish = true;
    currentSong = -1;
    previousMillis_finish = millis();
    return;
  }
  int currentNote = pgm_read_word(&melody[songNum][noteTrack]); // "&"" takes the address of the element from flash
  int durationVal = pgm_read_word(&noteDurations[songNum][noteTrack]);

  // Note starts
  if (!notePlaying && !ledWaiting) {
    noTone(BUZZER_PIN);
    offLEDs();
    // LED stays off for 40ms 
    pendingLEDNote = currentNote;
    previousMillis_led = currentMillis;
    ledWaiting = true; // LED turns on

    ledBrightness = 255;
    analogWrite(DIM_LED_PIN, ledBrightness); // start at full brightness

    tone(BUZZER_PIN, currentNote);
    noteDuration = TEMPO / durationVal;
    previousMillis_note = currentMillis;
    notePlaying = true;

    // scrolling lyrics by 4 indexes at each note
    lcd.setCursor(0, 0);
    for (int i = 0; i < 16; i++) {
      // reading lyrics from PROGMEM. Prints the current char and goes up from 0 till 15, and then scrolls up by 4 
      lyricBuffer[i] = pgm_read_byte(songText + lyricTrack + i); //
    }
    lyricBuffer[16] = '\0';
    lcd.print(lyricBuffer);
    lyricTrack += 4;
    notePlaying = true;
  }
  if (notePlaying) {
    //Dimming LED every 10ms after the note starts playing
    if (currentMillis - previousMillis_dim >= dimInterval){
      previousMillis_dim = currentMillis;
      ledBrightness -= 5;   // fade speed
      if (ledBrightness < 0) ledBrightness = 0;
      analogWrite(DIM_LED_PIN, ledBrightness);
    }
    // When note ends, turn off LEDs and tone for a pause
    if (currentMillis - previousMillis_note >= noteDuration * 0.9) {
      previousMillis_note = currentMillis;
      noTone(BUZZER_PIN);
      noteTrack++;
      offLEDs();
      notePlaying = false;
    }
  }
}

// Turn on LED based on the note.
void updateLED(int note) {
  offLEDs();
  switch (note) {
    case NOTE_C4 :
      digitalWrite(LED_PIN1, HIGH);
      break;
    case NOTE_D4 :
      digitalWrite(LED_PIN2, HIGH);
      break;
    case NOTE_E4 :
      digitalWrite(LED_PIN3, HIGH);
      break;
    case NOTE_F4 :
      digitalWrite(LED_PIN4, HIGH);
      break;
    case NOTE_G4 :
    case NOTE_A4 :
    case NOTE_AS4 :
    case NOTE_C5 :
      digitalWrite(LED_PIN5, HIGH);
      break;
  }
}

// turns off every led.
void offLEDs() {
  for (int i = 8; i <= 12; i++) digitalWrite(i, LOW);
}

void loop() {
  // put your main code here, to run repeatedly
  unsigned long currentMillis = millis();
  if (gameOver) {
    // do nothing forever
    return;
  }
  // 1s delay for the errors
  if (error && currentMillis - previousMillis_error >= errorDelay) {
    lcd.clear();
    error = false;
  }
  if (ledWaiting && currentMillis - previousMillis_led >= ledOffTime) {
    updateLED(pendingLEDNote);   // LEDs turn ON after 40ms delay
    ledWaiting = false;
  }
  // play again feature after the song ends
  if (finish && !error && currentMillis - previousMillis_finish >= finishDelay) {
    previousMillis_finish = currentMillis;
    lcd.setCursor(0, 0);
    lcd.print("1-Play Again");
    lcd.setCursor(0, 1);
    lcd.print("2-End Show");

    if (Serial.available() > 0) {
      playAgainChoice = Serial.read();
      while (Serial.available() > 0) Serial.read();
      if (playAgainChoice == '1') {
        // reset everything to play again
        lcd.clear();
        noteTrack = 0;
        lyricTrack = 0;
        finish = false;
        error = true; // basically a 1s delay
      }
      else if (playAgainChoice == '2') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("THE MUSIC SHOW");
        lcd.setCursor(0, 1);
        lcd.print("   IS OVER");
        gameOver = true;
        noTone(BUZZER_PIN);
        for (int i = 8; i <= 12; i++) digitalWrite(i, HIGH);
        analogWrite(DIM_LED_PIN, 255);
      }
      else { // error handling
        lcd.clear();
        lcd.print("Invalid input");
        error = true;
        previousMillis_error = currentMillis;
      }
    }
  }
  if (currentSong != -1) {
    playSong(currentSong, currentSong == 0 ? song1 : song2);
  }
  if (currentSong == -1 && !finish && !gameOver && !error) {
    lcd.setCursor(0, 0);
    lcd.print(" 1-Jingle Bells");
    lcd.setCursor(0, 1);
    lcd.print("2-Happy Birthday");
  }
  if (Serial.available() > 0 && !finish) {
    choice = Serial.read();
    while (Serial.available() > 0) Serial.read(); // clears every extra value which you input
    if (choice == '1') {
      lcd.clear();
      noteTrack = 0;
      lyricTrack = 0;
      currentSong = 0;
    }
    else if (choice == '2') {
      lcd.clear();
      noteTrack = 0;
      lyricTrack = 0;
      currentSong = 1;
    }
    else { // error handling
      lcd.clear();
      lcd.print("Invalid input");
      error = true;
      previousMillis_error = currentMillis;
    }
  }
}
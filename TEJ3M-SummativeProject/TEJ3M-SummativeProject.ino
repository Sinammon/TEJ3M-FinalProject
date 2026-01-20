/* Author : Sinan Mohammed
  Date : Jan 19th, 2026.
  
  version 1 : added liquid crystal I2C added pitches file 


  
  
  */

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "pitches.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LOW_NOTE_MAX 220        
#define MEDIUM_LOW_NOTE_MAX 280 
#define MEDIUM_NOTE_MAX 340     
#define MEDIUM_HIGH_NOTE_MAX 400
#define HIGH_NOTE_MAX 480

// Pins
const int BUZZER_PIN = 8;

const int LED_PIN1 = 9;
const int LED_PIN2 = 10;
const int LED_PIN3 = 11;
const int LED_PIN4 = 12;
const int LED_PIN5 = 7;

// Millis function variables.
unsigned long previousMillis_lyric = 0;
unsigned long previousMillis_lyric2 = 0;
unsigned long previousMillis_choice = 0;
unsigned long previousMillis_error = 0;
unsigned long previousMillis_note = 0;
unsigned long previousMillis_songFinished = 0;

const int errorDelay = 1000;
const int songFinishedDelay = 1500;
const int choiceDelay = 1000;
const int lyricDelay = 150;

String choice;

//Lyrics
String song1 = "Jingle Bells, Jingle Bells, Jingle all the Way, Oh what fun it is to ride on a one horse open sleigh, Hey! Jingle Bells, Jingle Bells, Jingle all the Way, Oh what fun it is to ride on a one horse open sleigh! ";
String song2 = "Mary had a little lamb, little lamb, little lamb. Mary had a little lamb, its fleece was white as snow.";

String [] jingleBells = {"Ma","ry", "had" ,"a", "little", "lamb,", "little", "lamb," ,"little","lamb.", "Ma", "ry", "had", 
"a", "little", "lamb,", "its", "fleece", "was", "white", "as", "snow."}

String[] durations =
int lyricTrack = 0;

// Notes
int melody[2 ][ 51]={
  {NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,
NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4,
NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,NOTE_G4, NOTE_C4, NOTE_D4,
NOTE_E4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
NOTE_G4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4},
{NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4,
NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_D4,
NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4,
NOTE_C4}};
int noteTrack = 0;

// Duration of each note

int noteDurations[ 2][ 51]={
  {4,4,2,4,4,2,4,4,4,4,1,4,4,4,4,4,4,4,8,8,4,4,4,4,2,2,4,4,2,4,4,2,4,4,4,4,1,4,4,4,4,4,4,4,8,8,4,4,4,4,1},
  {4,4,4,4,4,4,2,4,4,2,4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,1}
};

boolean playingSong1 = false;
boolean playingSong2 = false;


boolean error = false;
boolean songFinished = false;

void setup() {
  // put your setup code here, to run once:'
  Serial.begin(9600);
  
  pinMode(BUZZER_PIN,OUTPUT);

  pinMode(LED_PIN1,OUTPUT);
  pinMode(LED_PIN2,OUTPUT);
  pinMode(LED_PIN3,OUTPUT);
  pinMode(LED_PIN4,OUTPUT);
  pinMode(LED_PIN5,OUTPUT);



  lcd.init();
  lcd.backlight();
  lcd.clear();

}
// Methods

   void playSong(int songNum){
  
  unsigned long currentMillis = millis();

  if (noteTrack >= 51){
    noTone(BUZZER_PIN);
    /*playingSong1 = false;
    playingSong2 = false;
    songFinished = true;
    previousMillis_songFinished = currentMillis;
    */
    return;
  }
  int noteDuration = 1000/ noteDurations[songNum][noteTrack];

  if (currentMillis - previousMillis_note >= noteDuration){
    previousMillis_note = currentMillis;

    tone(BUZZER_PIN, melody[songNum][noteTrack], noteDuration* 0.9);
    updateLED(melody[songNum][noteTrack]);
    noteTrack++;
  }
 }
 void updateLED(int note){
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  digitalWrite(LED_PIN3, LOW);
  digitalWrite(LED_PIN4, LOW);
  digitalWrite(LED_PIN5, LOW);

  if (note == NOTE_E4){
    digitalWrite(LED_PIN1,HIGH);
  }
  else if (note == NOTE_G4){
    digitalWrite(LED_PIN2,HIGH);
  }
  else if (note == NOTE_C4){
    digitalWrite(LED_PIN3,HIGH);
  }
  else if (note == NOTE_D4){
    digitalWrite(LED_PIN4,HIGH);
  }
  else{
    digitalWrite(LED_PIN5,HIGH);
  }

 }
 void offLEDs(){
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  digitalWrite(LED_PIN3, LOW);
  digitalWrite(LED_PIN4, LOW);
  digitalWrite(LED_PIN5, LOW);
 }

void loop() {
  // put your main code here, to run repeatedly
   unsigned long currentMillis = millis();

   /* if (songFinished){
    if (currentMillis - previousMillis_songFinished >= songFinishedDelay){
      previousMillis_songFinished = currentMillis;
      lcd.clear();
      return;
    }else {
      lcd.setCursor(0,0);
      lcd.print(" Song Finished  ");
      return;
    }
   }
   */
   /*
  if (error) {
    if (currentMillis - previousMillis_error >= errorDelay) {
      previousMillis_error = currentMillis;
      lcd.clear();
      error = false;
    }else{
      lcd.setCursor(0,0);
      lcd.print("Invalid number");
      return;
    }
  } 
  */
if(playingSong1){
    playSong(0);
  if (currentMillis - previousMillis_lyric >= lyricDelay){
    previousMillis_lyric = currentMillis;

    lcd.clear();
    lcd.print(song1.substring(lyricTrack, lyricTrack + 16));
    
    if (lyricTrack >= song1.length()-16){
    lcd.clear();
    //lcd.print("Song Finished");
   // previousMillis_error = currentMillis;
    //error = true;  
    playingSong1 = false;
    lyricTrack = 0;
    previousMillis_lyric = 0;
    }
    lyricTrack++;
  }
}
 if(playingSong2){
    playSong(1);
    if (currentMillis - previousMillis_lyric2 >= lyricDelay){

    previousMillis_lyric2 = currentMillis;

    lcd.clear();
    lcd.print(song2.substring(lyricTrack, lyricTrack + 16));
    
    if (lyricTrack >= song2.length()-16){
    lcd.clear();
   // previousMillis_songFinished = currentMillis;
    //songFinished = true;  
    offLEDs();
    playingSong2 = false;
    lyricTrack = 0;
    previousMillis_lyric2 = 0;
  }
    lyricTrack++;
 }
 }
if (!playingSong1 && !playingSong2 && !error) {
 lcd.setCursor(0,0);
 lcd.print(" Input 1 or 2");
 lcd.setCursor(0,1);
 lcd.print("into the monitor");
}
if (Serial.available()>0 && !playingSong1 && !playingSong2 && !error){

  lcd.clear();
  choice = Serial.readString();
  choice.trim();
  choice.toUpperCase();

  if (choice != "1" && choice != "2"){
    lcd.print("Invalid number ");
    delay(1000);
    lcd.clear();
    return;
  }
  if (choice == "1"){
    noteTrack = 0;
    lyricTrack = 0;
    previousMillis_lyric = currentMillis;
    playingSong1 = true;
    playingSong2 = false;
  }
  if (choice == "2"){
    noteTrack = 0;
    lyricTrack = 0;
    previousMillis_lyric2 = currentMillis;
    playingSong1 = false;
    playingSong2 = true;
  }
 }

}

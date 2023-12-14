#include "user_console.h"
#include "Arduino.h"
#include "config.h"

#include <EnableInterrupt.h>

#define BOUNCING_TIME 100
#define __DEBUG__

uint8_t inputPins[] = {BUT01_PIN, BUT02_PIN, BUT03_PIN, BUT04_PIN};

bool buttonPressed[] = { false, false, false, false };

int sequenceLen;
int sequence[10]; 

long lastButtonPressedTimeStamps[NUM_INPUT_POS];

void button_handler(int i);
void button_handler_0(){ button_handler(0); }
void button_handler_1(){ button_handler(1); }
void button_handler_2(){ button_handler(2); }
void button_handler_3(){ button_handler(3); }

void (*button_handlers[4])() = { button_handler_0, button_handler_1, button_handler_2, button_handler_3 };

void button_handler(int i){
  long ts = millis();
  if (ts - lastButtonPressedTimeStamps[i] > BOUNCING_TIME){
    lastButtonPressedTimeStamps[i] = ts;
    int status = digitalRead(inputPins[i]);
    if (status == HIGH && !buttonPressed[i]) { 
        buttonPressed[i] = true;
        sequence[sequenceLen] = i;
        sequenceLen++;
    }
  }
}

int read_difficulty_level(){
  double value = analogRead(POT_PIN);
  double valueMapped = ((((double)value) + 1.0)*((double)DIFFICULTY_LEVELS))/1024.0; 
  int trunc = (int) valueMapped;
  if (valueMapped > ((double)trunc)){
    trunc++;
  }
  return trunc;
}

void init_player_console(){
  Serial.begin(9600);
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    pinMode(inputPins[i], INPUT);  
    enableInterrupt(inputPins[i], button_handlers[i], CHANGE);       
  }
}

void print_on_console(const String& msg){
  Serial.println(msg);
}

void reset_player_input(){
  long ts = millis();
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    buttonPressed[i] = false;      
    lastButtonPressedTimeStamps[i] = ts;    
  }
  sequenceLen = 0;
  delay(BOUNCING_TIME);
  log("input reset");
}

int getCurrentInputSequenceLength(){
  return sequenceLen;
}

int getCurrentInputSequenceElem(int index){
  if (index < sequenceLen){
    return sequence[index];
  } else {
    return -1;
  }
}

String getCurrentInputSequenceAsString(){
  String st = "";
  for (int i = 0; i < sequenceLen; i++){
    st = st + sequence[i] + " ";   
  }
  return st;
}

bool player_input_started(){
  if (buttonPressed[0]){
    return true;
  } else {
    return false;
  }
}

void log(const String& msg){
  #ifdef __DEBUG__
  Serial.println(msg);
  #endif
}


void test_player_input(){
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    if (buttonPressed[i]) {
      Serial.println(String("button ") + i + " pressed"); 
      buttonPressed[i] = false;   
    }
  }
  // int value = analogRead(POT_PIN);
  // Serial.println(value);
}

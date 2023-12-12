#include "UserConsole.h"
#include <Arduino.h>
#include "devices/ButtonImpl.h"
#include "kernel/MsgService.h"
#include <avr/sleep.h>
#include "config.h"
#include "kernel/Logger.h"

UserConsole::UserConsole(){
  pLcd = new  LiquidCrystal_I2C(0x27,20,4);
  pButton = new ButtonImpl(START_BT_PIN);
}

void UserConsole::init(){
  pLcd->init();
  pLcd->backlight();
  pLcd->noDisplay();
}

void UserConsole::sync(){
  pButton->sync();
}

void UserConsole::turnOnDisplay(){
  pLcd->display();
  pLcd->clear();
}

void UserConsole::turnOffDisplay(){
  pLcd->noDisplay();
}


bool UserConsole::started(){
  return pButton->isPressed();
}

void UserConsole::displayWelcome(){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Welcome");
}

void UserConsole::displayChekingIn(){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Proceed to the Washing Area");
}

void UserConsole::displayWaitToStart(){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Ready To Wash");
}

void UserConsole::displayWashing(int perc){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Washing");
  pLcd->setCursor(0, 1);
  pLcd->setCursor(0, 1);
  pLcd->print(String(perc));
  for (int i = 0; i < perc/10; i++){
    pLcd->setCursor(5 + i, 1);
    pLcd->print((char) 0xFF);
  }
}

void UserConsole::displayWashingSuspended(){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Washing Suspended");
}

void UserConsole::displayChekingOut(){
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Washing complete, you can leave the area");
}

void UserConsole::prepareToSleep(){
  pLcd->noDisplay();
}

void UserConsole::resumeFromSleeping(){
  pLcd->display();  
}


void UserConsole::test(){
  Logger.log("Testing the User Console");
  delay(1000);

  Logger.log("Display test...");
  pLcd->display();
  pLcd->clear();
  pLcd->setCursor(0, 0); 
  pLcd->print("Testing");

  Logger.log("Button test...");
  long t0 = millis();
  while (millis() - t0 < 5000){
    sync();
    if (pButton->isPressed()){
        Logger.log("pressed");
    }
  }  
}




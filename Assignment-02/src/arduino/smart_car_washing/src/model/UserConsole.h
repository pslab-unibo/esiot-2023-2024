#ifndef __USER_CONSOLE__
#define __USER_CONSOLE__

#include "config.h"
#include "devices/Button.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// #include "Bridge.h"

class UserConsole {

public:
  UserConsole();

  void init();
  void sync();

  void turnOffDisplay();
  void turnOnDisplay();

  bool started();

  void displayWelcome();
  void displayChekingIn();
  void displayWaitToStart();
  void displayWashing(int perc);
  void displayWashingSuspended();
  void displayChekingOut();
  
  void prepareToSleep();
  void resumeFromSleeping();

  void test();
private:
  Button* pButton; 
  LiquidCrystal_I2C* pLcd; 
};

#endif

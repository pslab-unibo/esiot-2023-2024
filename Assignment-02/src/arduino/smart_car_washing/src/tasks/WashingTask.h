#ifndef __WASHING_TASK__
#define __WASHING_TASK__

#include "kernel/Task.h"
#include "model/CarWashingPlant.h"
#include "model/UserConsole.h"
#include "tasks/BlinkTask.h"

class WashingTask: public Task {

public:
  WashingTask(CarWashingPlant* pPlant, UserConsole* pUserConsole, BlinkTask* pBlinkTask); 
  void tick();

private:  

  void setState(int state);
  long elapsedTimeInState();
  void logOnce(const String& msg);

  enum { IDLE, WAITING_TO_START, WASHING, PROBLEM_DETECED, WASHING_SUSPENDED} state;
  long stateTimestamp;
  bool justEntered;

  int washingPerc;
  long lastTimeWashing;

  CarWashingPlant* pPlant;
  UserConsole* pUserConsole;
  BlinkTask* pBlinkTask;
};

#endif
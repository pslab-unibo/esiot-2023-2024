#ifndef __CHECKIN_TASK__
#define __CHECKIN_TASK__

#include "kernel/Task.h"
#include "model/CarWashingPlant.h"
#include "tasks/BlinkTask.h"

class CheckInTask: public Task {

public:
  CheckInTask(CarWashingPlant* pPlant, BlinkTask* pBlinkTask); 
  void tick();

private:  

  void setState(int state);
  long elapsedTimeInState();
  void logOnce(const String& msg);

  enum { IDLE, GATE_OPENING, CAR_ENTERING, CAR_IN_PLACE, GATE_CLOSING} state;
  long stateTimestamp;
  bool justEntered;

  CarWashingPlant* pPlant;
  BlinkTask* pBlinkTask;
};

#endif
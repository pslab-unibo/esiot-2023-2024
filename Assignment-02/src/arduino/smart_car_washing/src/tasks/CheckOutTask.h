#ifndef __CHECKOUT_TASK__
#define __CHECKOUT_TASK__

#include "kernel/Task.h"
#include "model/CarWashingPlant.h"

class CheckOutTask: public Task {

public:
  CheckOutTask(CarWashingPlant* pPlant); 
  void tick();

private:  

  void setState(int state);
  long elapsedTimeInState();
  void logOnce(const String& msg);

  enum { IDLE, GATE_OPENING, CAR_LEAVING, CAR_OUT, GATE_CLOSING} state;
  long stateTimestamp;
  bool justEntered;


  CarWashingPlant* pPlant;
};

#endif
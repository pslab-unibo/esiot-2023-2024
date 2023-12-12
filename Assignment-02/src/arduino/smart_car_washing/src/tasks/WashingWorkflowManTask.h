#ifndef __WASHINGWORKFLOWMAN_TASK__
#define __WASHINGWORKFLOWMAN_TASK__

#include "kernel/Task.h"
#include "model/CarWashingPlant.h"
#include "model/UserConsole.h"

class WashingWorkflowManTask: public Task {

public:
  WashingWorkflowManTask(CarWashingPlant* pPlant, UserConsole* pUserConsole); 
  void tick();

private:  

  bool checkMaintenanceDoneMsg();
  
  void setState(int state);
  long elapsedTimeInState();
  void logOnce(const String& msg);


  enum {  SLEEPING, 
          WAITING_FOR_CARS, CAR_DETECTED, 
          CHECKING_IN, WASHING, CHECKING_OUT,
          IN_MAINTENANCE} state;
  long stateTimestamp;
  bool justEntered;

  CarWashingPlant* pPlant;
  UserConsole* pUserConsole;
};

#endif
#ifndef __TELEMETRY_TASK__
#define __TELEMETRY_TASK__

#include "kernel/Task.h"
#include "model/CarWashingPlant.h"
#include "model/UserConsole.h"

class TelemetryTask: public Task {

public:
  TelemetryTask(CarWashingPlant* pPlant, UserConsole* pUserConsole); 
  void tick();

private:  

  void setState(int state);
  long elapsedTimeInState();
  void logOnce(const String& msg);

  enum { IDLE, SENDING_DATA } state;
  long stateTimestamp;
  bool justEntered;

  CarWashingPlant* pPlant;
  UserConsole* pUserConsole;
};

#endif
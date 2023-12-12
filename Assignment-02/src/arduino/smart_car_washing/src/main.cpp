/*
 * SMART CAR WASHING - Assignment #02 - ESIOT a.y. 2023-2024
 * 
 * Solution sketch by AR
 * 
 */
#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#include "model/UserConsole.h"
#include "model/CarWashingPlant.h"

#include "tasks/WashingWorkflowManTask.h"
#include "tasks/CheckInTask.h"
#include "tasks/CheckOutTask.h"
#include "tasks/BlinkTask.h"
#include "tasks/WashingTask.h"
#include "tasks/TelemetryTask.h"


Scheduler sched;

CarWashingPlant* pCarWashingPlant;
UserConsole* pUserConsole;

void setup() {
  MsgService.init();
  sched.init(100);

  Logger.log(".:: Smart Car Washing ::.");
  
  pCarWashingPlant = new CarWashingPlant();
  pCarWashingPlant->init();

  pUserConsole = new UserConsole();
  pUserConsole->init();

  Task* pCheckInInBlinkTask = new BlinkTask(LED2_PIN);
  pCheckInInBlinkTask->init(100);
  pCheckInInBlinkTask->setActive(false);

  Task* pWashingWorkflowManTask = new WashingWorkflowManTask(pCarWashingPlant, pUserConsole);
  pWashingWorkflowManTask->init(100);

  Task* pCheckInTask = new CheckInTask(pCarWashingPlant, pCheckInInBlinkTask);
  pCheckInTask->init(200);

  Task* pCheckOutTask = new CheckOutTask(pCarWashingPlant);
  pCheckOutTask->init(200);

  Task* pWashingBlinkTask = new BlinkTask(LED2_PIN);
  pWashingBlinkTask->init(100);
  pWashingBlinkTask->setActive(false);

  Task* pWashingTask = new WashingTask(pCarWashingPlant, pUserConsole, pWashingBlinkTask);
  pWashingTask->init(100);

  Task* pTelemetryTask = new TelemetryTask(pCarWashingPlant, pUserConsole);
  pTelemetryTask->init(100);

  sched.addTask(pWashingWorkflowManTask);
  sched.addTask(pCheckInTask);  
  sched.addTask(pCheckInInBlinkTask);  
  sched.addTask(pCheckOutTask);  
  sched.addTask(pWashingTask); 
  sched.addTask(pWashingBlinkTask);  
  sched.addTask(pTelemetryTask);
  
  // 

  // pCarWashingPlant->test();
  // pUserConsole->test();
 
}

void loop() {
    sched.schedule();
}
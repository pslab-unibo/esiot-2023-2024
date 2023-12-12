#include "tasks/WashingWorkflowManTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"
#include <avr/sleep.h>

#define SLEEP_TIMEOUT 5000
#define CAR_DETECTED_TIMEOUT 2000

void wakeUp(){}

WashingWorkflowManTask::WashingWorkflowManTask(CarWashingPlant* pPlant, UserConsole* pUserConsole): 
    pPlant(pPlant), pUserConsole(pUserConsole) {
    attachInterrupt(digitalPinToInterrupt(PIR_PIN), wakeUp, RISING); 
    setState(WAITING_FOR_CARS);
}
  
void WashingWorkflowManTask::tick(){
    switch (state){    
    case WAITING_FOR_CARS: {
        this->logOnce(F("[W] waiting"));
        pPlant->sampleCarPresence();
        if (pPlant->detectedCarPresence()){
            pUserConsole->turnOnDisplay();
            pUserConsole->displayWelcome();
            pPlant->readyToAccept();
            setState(CAR_DETECTED);
        } else if (elapsedTimeInState() > SLEEP_TIMEOUT){
            setState(SLEEPING);
        } 
        break;
    }
    case SLEEPING: {
        this->logOnce(F("[W] going to sleep"));
        pPlant->prepareToSleep();
        pUserConsole->prepareToSleep();
        delay(100);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
        sleep_enable();
        sleep_mode();  
        pPlant->resumeFromSleeping();
        pUserConsole->resumeFromSleeping();
        setState(WAITING_FOR_CARS);
        break;
    }
    case CAR_DETECTED: {  
        this->logOnce(F("[M] detected")); 
        if (elapsedTimeInState() > CAR_DETECTED_TIMEOUT){
            pPlant->readyForCheckIn();
            pUserConsole->displayChekingIn();
            setState(CHECKING_IN);
        }
        break;       
    }
    case CHECKING_IN: {
        this->logOnce(F("[M] checking in")); 
        if (pPlant->isCheckInCompleted()){
            pPlant->readyForWashing();
            setState(WASHING);
        }
        break;
    }    
    case WASHING: {
        this->logOnce(F("[M] washing")); 
        if (pPlant->isWashingCompleted()){
            pPlant->readyForCheckOut();
            pUserConsole->displayChekingOut();
            setState(CHECKING_OUT);
        } else if (pPlant->isInMaintenance()){
            setState(IN_MAINTENANCE);
            // call mantanance
        }
        break;
    }

    case IN_MAINTENANCE: {
        logOnce(F("[M] maintenance"));
        if (checkMaintenanceDoneMsg()){
            pPlant->resumeWashing();
            setState(WASHING);
        }
        break;
    }

    case CHECKING_OUT: {
        this->logOnce(F("[M] checking out")); 
        if (pPlant->isCheckOutCompleted()){
            pPlant->idle();
            pUserConsole->displayWelcome();
            setState(WAITING_FOR_CARS);            
        }
        break;
    }
    }
}

void WashingWorkflowManTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long WashingWorkflowManTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

void WashingWorkflowManTask::logOnce(const String& msg){
    if (justEntered){
      Logger.log(msg);
      justEntered = false;
    }
}

bool WashingWorkflowManTask::checkMaintenanceDoneMsg(){
  bool done = false;
  if (MsgService.isMsgAvailable()){
    Msg* msg = MsgService.receiveMsg();
    if (msg != NULL){
      Logger.log(msg->getContent());
      if (msg->getContent() == "ok"){
        done = true;
      }
      delete msg;
    }
  }
  return done;
}

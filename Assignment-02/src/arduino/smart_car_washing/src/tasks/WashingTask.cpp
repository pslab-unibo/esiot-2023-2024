#include "tasks/WashingTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
#include "tasks/BlinkTask.h"

#define WASHING_TIMEOUT ((double) 10000)
#define MAX_TEMPERATURE 30
#define MAX_TEMPERATURE_TIMEOUT 5000

WashingTask::WashingTask(CarWashingPlant* pPlant, UserConsole* pUserConsole, BlinkTask* pBlinkTask): 
    pPlant(pPlant), pUserConsole(pUserConsole), pBlinkTask(pBlinkTask) {
    setState(IDLE);
}
  
void WashingTask::tick(){
    switch (state){    
    case IDLE: {
        logOnce(F("[WT] idle"));
        if (pPlant->isReadyForWashing()){
            setState(WAITING_TO_START);
            pUserConsole->displayWaitToStart();
        }
        break;
    }
    case WAITING_TO_START: {        
        logOnce(F("[WT] wait to start"));
        pUserConsole->sync();
        if (pUserConsole->started()){
            pPlant->washingStarted();
            pBlinkTask->setActive(true);
            washingPerc = 0;
            lastTimeWashing = millis();
            setState(WASHING);
        }
        break;       
    }

    case WASHING: {
        logOnce(F("[WT] washing"));
        /*
        // for testing the maintenance
        pPlant->setInMaintenance();
        setState(WASHING_SUSPENDED);    
        */
        long t = millis();
        double dt = t - lastTimeWashing;
        int perc = (dt*100/WASHING_TIMEOUT);
        pUserConsole->displayWashing(perc);
        if (perc >= 100){
            pPlant->washingCompleted();
            pBlinkTask->setActive(false);
            setState(IDLE);
        } else if (pPlant->getCurrentTemperature() > MAX_TEMPERATURE){
            setState(PROBLEM_DETECED);    
        }
        break;
    }    
    case PROBLEM_DETECED: {
        logOnce(F("[WT] problem detected"));
        if (pPlant->getCurrentTemperature() < MAX_TEMPERATURE){
            setState(WASHING);    
        } else if (elapsedTimeInState() > MAX_TEMPERATURE_TIMEOUT){
            pPlant->setInMaintenance();
            setState(WASHING_SUSPENDED);
        }
        break;
    }
    case WASHING_SUSPENDED: {
        logOnce(F("[WT] suspended"));
        if (pPlant->isReadyForWashing()){
            pPlant->washingStarted();
            setState(WASHING);
        }
        break;
    }
    }
}

void WashingTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long WashingTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}


void WashingTask::logOnce(const String& msg){
    if (justEntered){
      Logger.log(msg);
      justEntered = false;
    }
}



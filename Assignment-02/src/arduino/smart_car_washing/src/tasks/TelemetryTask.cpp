#include "tasks/TelemetryTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#define SENDING_DATA_TIMEOUT 500

TelemetryTask::TelemetryTask(CarWashingPlant* pPlant, UserConsole* pUserConsole): 
    pPlant(pPlant), pUserConsole(pUserConsole) {
    setState(IDLE);
}
  
void TelemetryTask::tick(){
    switch (state){    
    case IDLE: {
        // logOnce("[TT] idle");
        if (elapsedTimeInState() > SENDING_DATA_TIMEOUT){
            setState(SENDING_DATA);
        }
        break;
    }
    case SENDING_DATA: {        
        // logOnce("[TT] sending data");
        int stateCode = 0;
        
        if (pPlant->isIdle()){
            stateCode = 0;
        } else if (pPlant->isCheckingIn()){
            stateCode = 1;
        } else if (pPlant->isCheckingOut()){
            stateCode = 2;
        } else if (pPlant->isReadyForWashing()){
            stateCode = 3;
        } else if (pPlant->isWashing()){
            stateCode = 4;
        } else if (pPlant->isInMaintenance()){
            stateCode = 5;
        }

        int numWashes = pPlant->getNumWashes();
        pPlant->sampleTemperature();
        double currentTemp = pPlant->getCurrentTemperature();

        MsgService.sendMsg(String("cw:st:") + String(stateCode) + ":" + String(numWashes).substring(0,5) + ":" +  String(currentTemp).substring(0,5));  
        setState(IDLE);
        break;       
    }
    }
}

void TelemetryTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long TelemetryTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}


void TelemetryTask::logOnce(const String& msg){
    if (justEntered){
      Logger.log(msg);
      justEntered = false;
    }
}



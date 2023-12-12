#include "tasks/CheckInTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
// #include "UserConsole.h"

#define MIN_CAR_DISTANCE 0.20
#define CAR_IN_PLACE_TIMEOUT 2000
#define GATE_OPENING_TIME 500
#define GATE_CLOSING_TIME 500

CheckInTask::CheckInTask(CarWashingPlant* pPlant, BlinkTask* pBlinkTask): 
    pPlant(pPlant), pBlinkTask(pBlinkTask) {
    setState(IDLE);
}
  
void CheckInTask::tick(){
    switch (state){    
    case IDLE: {
        this->logOnce(F("[CI] idle"));
        if (pPlant->isReadyForCheckIn()){
            pPlant->checkInStarted();
            pBlinkTask->setActive(true);
            pPlant->openGate();
            setState(GATE_OPENING);
        }
        break;
    }
    case GATE_OPENING: {        
        this->logOnce(F("[CI] opening"));
        if (elapsedTimeInState() > GATE_OPENING_TIME){
            setState(CAR_ENTERING);
        }
        break;       
    }

    case CAR_ENTERING: {
        this->logOnce(F("[CI] entering"));
        pPlant->sampleCarDistance();
        if (pPlant->getCurrentCarDistance() <= MIN_CAR_DISTANCE){
            setState(CAR_IN_PLACE);
        }
        break;
    }    
    case CAR_IN_PLACE: {
        this->logOnce(F("[CI] in place"));
        pPlant->sampleCarDistance();
        if (pPlant->getCurrentCarDistance() > MIN_CAR_DISTANCE){
            setState(CAR_ENTERING);
        } else if (elapsedTimeInState() > CAR_IN_PLACE_TIMEOUT){
            pPlant->closeGate();
            setState(GATE_CLOSING);
        } else 
        break;
    }
    case GATE_CLOSING: {
        this->logOnce(F("[CI] closing"));
        if (elapsedTimeInState() > GATE_CLOSING_TIME){
            pPlant->checkInCompleted();
            pBlinkTask->setActive(false);
            setState(IDLE);
        }
        break;
    }
    }
}

void CheckInTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long CheckInTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

void CheckInTask::logOnce(const String& msg){
    if (justEntered){
      Logger.log(msg);
      justEntered = false;
    }
}
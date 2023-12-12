#include "tasks/CheckOutTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

#define MAX_CAR_DISTANCE 0.60
#define CAR_OUT_TIMEOUT 2000
#define GATE_OPENING_TIME 500
#define GATE_CLOSING_TIME 500

CheckOutTask::CheckOutTask(CarWashingPlant* pPlant): 
    pPlant(pPlant) {
    setState(IDLE);
}
  
void CheckOutTask::tick(){
    switch (state){    
    case IDLE: {
        logOnce(F("[CO] idle"));
        if (pPlant->isReadyForCheckOut()){
            pPlant->checkOutStarted();
            pPlant->openGate();
            setState(GATE_OPENING);
        }
        break;
    }
    case GATE_OPENING: {        
        logOnce(F("[CO] Gate opening"));
        if (elapsedTimeInState() > GATE_OPENING_TIME){
            setState(CAR_LEAVING);
        }
        break;       
    }

    case CAR_LEAVING: {
        logOnce(F("[CO] Car leaving"));
        pPlant->sampleCarDistance();
        if (pPlant->getCurrentCarDistance() >= MAX_CAR_DISTANCE){
            setState(CAR_OUT);
        }
        break;
    }    
    case CAR_OUT: {
        logOnce(F("[CO] Car out"));
        pPlant->sampleCarDistance();
        if (pPlant->getCurrentCarDistance() < MAX_CAR_DISTANCE){
            setState(CAR_LEAVING);
        } else if (elapsedTimeInState() > CAR_OUT_TIMEOUT){
            pPlant->closeGate();
            setState(GATE_CLOSING);
        }
        break;
    }
    case GATE_CLOSING: {
        logOnce(F("[CO] Gate closing"));
        if (elapsedTimeInState() > GATE_CLOSING_TIME){
            pPlant->checkOutCompleted();
            setState(IDLE);
        }
        break;
    }
    }
}

void CheckOutTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long CheckOutTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

void CheckOutTask::logOnce(const String& msg){
    if (justEntered){
      Logger.log(msg);
      justEntered = false;
    }
}
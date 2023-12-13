#include "model/CarWashingPlant.h"
#include "config.h"
#include "devices/Led.h"
#include "kernel/Logger.h"
#include "devices/LightSensorImpl.h"
#include "devices/TempSensorLM35.h"
#include "devices/servo_motor_impl.h"

CarWashingPlant::CarWashingPlant(){
}

void CarWashingPlant::init(){
    pSonar = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN, 10000);
    pPir = new Pir(PIR_PIN);
    pTempSensor = new TempSensorLM35(TEMP_SENSOR_PIN);
    pLed1 = new Led(LED1_PIN);
    pLed2 = new Led(LED2_PIN);
    pLed3 = new Led(LED3_PIN);
    pGateMotor = new ServoMotorImpl(GATE_PIN);
    
    Logger.log("Calibrating sensors in plant...");
    pPir->calibrate();

    nWashes = 0;
    state = IDLE;
}

bool CarWashingPlant::isIdle() {
    return state == IDLE;
}

bool CarWashingPlant::isCheckingIn(){
    return state == CHECKING_IN;
}

bool CarWashingPlant::isWashing(){
    return state == WASHING;
}

bool CarWashingPlant::isCheckingOut(){
    return state == CHECKING_OUT;
}

void CarWashingPlant::readyForCheckIn(){
    state = READY_FOR_CHECKIN;
    
}

bool CarWashingPlant::isReadyForCheckIn(){
    return state == READY_FOR_CHECKIN;
}

void CarWashingPlant::checkInStarted(){
    state = CHECKING_IN;
}

void CarWashingPlant::checkInCompleted(){
    state = CHECK_IN_COMPLETED;
    pGateMotor->off();    
}

bool CarWashingPlant::isCheckInCompleted(){
    return state == CHECK_IN_COMPLETED;
}

void CarWashingPlant::readyForWashing(){
    state = READY_FOR_WASHING;
}

bool CarWashingPlant::isReadyForWashing(){
    return (state == READY_FOR_WASHING) || (state == READY_TO_RESUME);
}

void CarWashingPlant::washingStarted(){
    state = WASHING;
}

void CarWashingPlant::resumeWashing(){
    state = READY_FOR_WASHING;
}

void CarWashingPlant::washingCompleted(){
    state = WASHING_COMPLETED;
    pLed2->switchOff();
}

bool CarWashingPlant::isWashingCompleted(){
    return state == WASHING_COMPLETED;
}

void CarWashingPlant::readyForCheckOut(){
    state = READY_FOR_CHECKOUT;
    pLed3->switchOn();
}

bool CarWashingPlant::isReadyForCheckOut(){
    return state == READY_FOR_CHECKOUT;
}

void CarWashingPlant::checkOutStarted(){
    state = CHECKING_OUT;
}

void CarWashingPlant::checkOutCompleted(){
    state = CHECK_OUT_COMPLETED;
    pGateMotor->off();
    pLed3->switchOff();
    nWashes++;
}

bool CarWashingPlant::isCheckOutCompleted(){
    return state == CHECK_OUT_COMPLETED;
}

void CarWashingPlant::idle(){
    state = IDLE;
}


void CarWashingPlant::setInMaintenance(){
    state = MAINTENANCE;
}

bool CarWashingPlant::isInMaintenance(){
    return state == MAINTENANCE;
}

void CarWashingPlant::maintenanceDone(){
    state = READY_TO_RESUME;
}

bool CarWashingPlant::isReadyToResume(){
    return state == READY_TO_RESUME;
}


void CarWashingPlant::sampleCarPresence(){
    this->pPir->sync();
    this->detectedCarPres = pPir->isDetected();
}

bool CarWashingPlant::detectedCarPresence(){
    return this->detectedCarPres; 
}

void CarWashingPlant::sampleTemperature(){
    this->currentTemperature = pTempSensor->getTemperature();
}

double CarWashingPlant::getCurrentTemperature(){
    return currentTemperature;
}

void CarWashingPlant::sampleCarDistance(){   
    double d = pSonar->getDistance();
    if (d == NO_OBJ_DETECTED){
        this->currentCarDistance = 1000000;
    } else {
        this->currentCarDistance = d;
    }
}

double CarWashingPlant::getCurrentCarDistance() {
    return this->currentCarDistance;
}

int CarWashingPlant::getNumWashes(){
    return nWashes;
}

void CarWashingPlant::openGate(){
    pGateMotor->on();
    pGateMotor->setPosition(90);
}

void CarWashingPlant::closeGate(){
    pGateMotor->setPosition(0);
}

void CarWashingPlant::readyToAccept(){
    this->pLed1->switchOn();
}

void CarWashingPlant::prepareToSleep(){
    pLed1->switchOff();
}

void CarWashingPlant::resumeFromSleeping(){
    pLed1->switchOn();
}

void CarWashingPlant::test(){

  Logger.log("Testing the Car Washing Plant...");

  delay(2000);

  Logger.log("Led test...");

  pLed2->switchOn();
  delay(1000);
  pLed2->switchOff();

  pLed3->switchOn();
  delay(1000);
  pLed3->switchOff();

  pWashingCompletedLed->switchOn();
  delay(1000);
  pWashingCompletedLed->switchOff();

  Logger.log("Motor test...");
  pGateMotor->on();
  pGateMotor->setPosition(0);
  delay(100);
  pGateMotor->setPosition(180);
  delay(1000);
  pGateMotor->setPosition(0);
  delay(1000);
  pGateMotor->off();
  
  for (int i = 0; i < 5; i++){
    double temp = pTempSensor->getTemperature();
    Logger.log("Temperature: " + String(temp));
  }

  Logger.log("Sonar test...");
  long t0 = millis();
  while (millis() - t0 < 5000){
    double dist = pSonar->getDistance();
    Logger.log("Distance: " + String(dist));
    delay(500);
  }  

  Logger.log("PIR test...");
  pPir->calibrate();
  t0 = millis();
  while (millis() - t0 < 5000){
    pPir->sync();
    bool detected = pPir->isDetected();
    Logger.log("Detected: " + String(detected));
    delay(500);
  }  

}

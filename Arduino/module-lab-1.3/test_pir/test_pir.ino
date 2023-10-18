
void setup(){
  Serial.begin(9600);
  
  //give the sensor some time to calibrate
  Serial.print("Calibrating sensor... ");
  for(int i = 0; i < CALIBRATION_TIME_SEC; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("PIR SENSOR READY.");
  delay(50);
}

void loop(){
  int detected = digitalRead(PIR_PIN);
  if (detected == HIGH){
    Serial.println("detected!");  
  }
};

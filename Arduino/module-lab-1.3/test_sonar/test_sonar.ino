const int trigPin = 7;
const int echoPin = 8;

/* Supposing to run this test 
   in an environment with 20 °C 
  ì https://en.wikipedia.org/wiki/Speed_of_sound
   */

const float temperature = 20;
const float vs = 331.45 + 0.62*temperature;

void setup()
{
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
}

float getDistance()
{
    /* Triggering stage: sending the impulse */

    digitalWrite(trigPin,LOW);
    delayMicroseconds(3);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin,LOW);
    
    /* Receiving the echo */

    float tUS = pulseIn(echoPin, HIGH);
    float t = tUS / 1000.0 / 1000.0 / 2;
    float d = t*vs;
    return d;
}

void loop()
{
  float d = getDistance();
  Serial.println(d);
  delay(50); 
}

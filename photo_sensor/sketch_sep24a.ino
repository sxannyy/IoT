#define PHOTO_SENSOR_PIN A4
#define LED_PIN1 2
#define LED_PIN2 3
#define SWITCHING_TIME 500

long int prevMillis1 = 0;
long int prevMillis2 = 0;
bool led1State = LOW;
bool led2State = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
}

void loop() {
  int photo_val = analogRead(PHOTO_SENSOR_PIN);
  Serial.print("Photo val = ");
  Serial.println(photo_val);

  if (photo_val > 300) {
    turnOnLights();
  } 
  else if (photo_val > 40 && photo_val < 130) {
    switchLight1();
    turnOffLight2();
  } 
  else if (photo_val > 130) {
    switchLight2();
    turnOffLight1();
  } 
  else {
    turnOffLights();
  }
}

void turnOnLights() {
  digitalWrite(LED_PIN1, HIGH);
  digitalWrite(LED_PIN2, HIGH);
}

void turnOffLights() {
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
}

void turnOffLight1() {
  digitalWrite(LED_PIN1, LOW);
}

void turnOffLight2() {
  digitalWrite(LED_PIN2, LOW);
}

void switchLight1() {
  long int currentMillis = millis();
  if (currentMillis - prevMillis1 >= SWITCHING_TIME) {
    prevMillis1 = currentMillis;
    led1State = !led1State;
    digitalWrite(LED_PIN1, led1State);
  }
}

void switchLight2() {
  long int currentMillis = millis();
  if (currentMillis - prevMillis2 >= SWITCHING_TIME) {
    prevMillis2 = currentMillis;
    led2State = !led2State;
    digitalWrite(LED_PIN2, led2State);
  }
}
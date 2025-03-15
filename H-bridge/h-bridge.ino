#define RIGHT_WAY 6
#define LEFT_WAY 5
#define SPEED_PIN 3


void setup()
{
  Serial.begin(9600);
  Serial.println(1);
  pinMode(RIGHT_WAY, OUTPUT);
  pinMode(LEFT_WAY, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);
}

void loop()
{
  for(int i = 0; i < 256; i++){
    setDirection(true, i);
    delay(50);
  }
  
  for(int i = 0; i < 256; i++){
    setDirection(false, i);
    delay(50);
  }
}

void setDirection(bool isRight, int speed) {
  if (isRight) {
    digitalWrite(RIGHT_WAY, HIGH);
    digitalWrite(LEFT_WAY, LOW);
  } else {
    digitalWrite(LEFT_WAY, HIGH);
    digitalWrite(RIGHT_WAY, LOW);
  }
  analogWrite(SPEED_PIN, speed);
}

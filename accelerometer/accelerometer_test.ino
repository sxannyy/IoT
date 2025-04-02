#include <Wire.h>

#define LED_PIN 3
int ADXL345 = 0x53;
float X_out, Y_out, Z_out;

float X1, Y1, Z1;
unsigned long last_time = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
}

void loop() {
  if(millis() - last_time > 3000)
    digitalWrite(LED_PIN, LOW);
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  X_out = (Wire.read() | Wire.read() << 8);
  X1 = X_out / 256;
  Y_out = (Wire.read() | Wire.read() << 8);
  Y1 = Y_out / 256;
  Z_out = (Wire.read() | Wire.read() << 8);
  Z1 = Z_out / 256;
  if(abs(X1) < 0.08 and abs(Y1) < 0.08 and abs(Z1) < 0.08){
    digitalWrite(LED_PIN, HIGH);
    last_time = millis();
  }
  Serial.print(X1);
  Serial.print("    ");
  Serial.print(Y1);
  Serial.print("    ");
  Serial.print(Z1);
  Serial.println("    ");
  delay(100); 
  
}

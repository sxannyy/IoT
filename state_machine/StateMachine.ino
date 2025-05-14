#include <PID_v1.h>

const uint8_t PIN_TRIG_LEFT = 8;
const uint8_t PIN_ECHO_LEFT = 2;
const uint8_t PIN_TRIG_FRONT = 9;
const uint8_t PIN_ECHO_FRONT = 3;

const uint8_t PIN_DIR_LEFT = 7;
const uint8_t PIN_PWM_LEFT = 6;
const uint8_t PIN_PWM_RIGHT = 5;
const uint8_t PIN_DIR_RIGHT = 4;

constexpr uint16_t TARGET_DIST = 15;
constexpr uint16_t FRONT_LIMIT = 15;
constexpr uint8_t  BASE_PWM = 150;

double distLeft = 0;
double pidOut = 0;
double setpoint = TARGET_DIST;
double Kp = 2, Ki = 5, Kd = 1;
PID wallPID(&distLeft, &pidOut, &setpoint, Kp, Ki, Kd, DIRECT);

volatile unsigned long tStartL = 0, tEchoL = 0;
volatile unsigned long tStartF = 0, tEchoF = 0;
volatile bool newL = false, newF = false;

unsigned long lastPing = 0;
constexpr uint16_t PING_PERIOD = 50;

const bool DIR_FWD_L  = HIGH;
const bool DIR_REV_L  = LOW;
const bool DIR_FWD_R  = LOW;
const bool DIR_REV_R  = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_DIR_LEFT , OUTPUT);
  pinMode(PIN_DIR_RIGHT, OUTPUT);
  pinMode(PIN_PWM_LEFT , OUTPUT);
  pinMode(PIN_PWM_RIGHT, OUTPUT);

  pinMode(PIN_TRIG_LEFT , OUTPUT);
  pinMode(PIN_TRIG_FRONT, OUTPUT);
  pinMode(PIN_ECHO_LEFT , INPUT);
  pinMode(PIN_ECHO_FRONT, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_ECHO_LEFT),  []{
      if (!tStartL) tStartL = micros();
      else { tEchoL = micros() - tStartL; tStartL = 0; newL = true; }
    }, CHANGE);

  attachInterrupt(digitalPinToInterrupt(PIN_ECHO_FRONT), []{
      if (!tStartF) tStartF = micros();
      else { tEchoF = micros() - tStartF; tStartF = 0; newF = true; }
    }, CHANGE);

  wallPID.SetMode(AUTOMATIC);
}

void loop() {
  if (millis() - lastPing >= PING_PERIOD) {
    lastPing = millis();
    static bool toggle = false;

    digitalWrite(PIN_TRIG_LEFT , LOW);
    digitalWrite(PIN_TRIG_FRONT, LOW);
    delayMicroseconds(2);

    digitalWrite(toggle ? PIN_TRIG_LEFT : PIN_TRIG_FRONT, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG_LEFT , LOW);
    digitalWrite(PIN_TRIG_FRONT, LOW);

    toggle = !toggle;
  }

  if (newL && newF) {
    newL = newF = false;

    distLeft = (tEchoL * 0.0343) / 2.0;
    double distFront = (tEchoF * 0.0343) / 2.0;

    wallPID.Compute();

    if (distFront < FRONT_LIMIT) {
      drive(-BASE_PWM, BASE_PWM);
      delay(250);
    } else {
      int pwmL = constrain(BASE_PWM - pidOut, 0, 255);
      int pwmR = constrain(BASE_PWM + pidOut, 0, 255);
      drive(pwmL, pwmR);
    }
  }
}

void drive(int pwmL, int pwmR) {
  bool dirL = pwmL >= 0 ? DIR_FWD_L : DIR_REV_L;
  bool dirR = pwmR >= 0 ? DIR_FWD_R : DIR_REV_R;

  digitalWrite(PIN_DIR_LEFT , dirL);
  digitalWrite(PIN_DIR_RIGHT, dirR);
  analogWrite(PIN_PWM_LEFT ,  abs(pwmL));
  analogWrite(PIN_PWM_RIGHT,  abs(pwmR));
}

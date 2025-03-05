#define TRIG_PIN 3
#define ECHO_PIN 4
#define UF_PIN A0
#define US_REQUEST 'U'
#define RESPONSE_LEN 5
#define IR_REQUEST 'I'

char cmd;
float duration = 0.0, distance = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(UF_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
    if (cmd == US_REQUEST) {
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(20);
      digitalWrite(TRIG_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
      if (duration != 0) {
        String duration_string = extend_int_zeroes(duration, RESPONSE_LEN);
        Serial.write(duration_string.c_str(), RESPONSE_LEN);
        // Serial.println();
        digitalWrite(TRIG_PIN, LOW);
        duration = 0;
      }
    }
    if (cmd == IR_REQUEST) {
      int val = analogRead(UF_PIN);
      String distance_string = extend_int_zeroes(val, RESPONSE_LEN);
      Serial.write(distance_string.c_str(), RESPONSE_LEN);
      // Serial.println();
    }
  }
}

String extend_int_zeroes(int val, int len) {
  String st = String(val);

  while (st.length() < len - 1)
    st = "0" + st;
  st = st + '\n';
  return st;
}

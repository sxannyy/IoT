#define PHOTO_SENSOR_PIN A4

size_t len_nums = 3;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read(); 

    if (command == 'p') {
      int sensorValue = analogRead(PHOTO_SENSOR_PIN); 
      String st = String(sensorValue / 4); 
      while (st.length() != len_nums) {
        st = '0' + st;
      }
      Serial.write(st.c_str(), len_nums);
    }
  }
}
#define PIN_D3 3
#define PIN_D4 4
#define PIN_D5 5
#define PIN_D6 6

#define interval 3

int pins[] = {PIN_D3, PIN_D4, PIN_D5, PIN_D6};
long int temp_time = 0;
long int blinking = 0;
bool is_phase = true;

int** phases1 = new int*[15];
int** phases2 = new int*[15];

int i = 0;

void setup() {
  pinMode(PIN_D3, OUTPUT);
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D5, OUTPUT);
  pinMode(PIN_D6, OUTPUT);
  for (int i = 0; i < 15; i++) {
    phases1[i] = new int[4];
    phases2[i] = new int[4];
  }

  // Заполняем массив phases1
  int temp_phases1[15][4] = {
    {1, 1, 0, 0}, //on
    {1, 0, 0, 1}, //1
    {1, 0, 1, 0}, //2
    {0, 1, 0, 1}, //3
    {0, 1, 1, 0}, //4
    {1, 0, 0, 0}, //1, 2
    {1, 1, 1, 0}, //2, 4
    {0, 1, 0, 0}, //3, 4
    {1, 1, 0, 1}, //1, 3
    {1, 0, 0, 1}, //1, 4
    {1, 0, 1, 0}, //2, 3
    {1, 0, 0, 0}, //1, 2, 3
    {1, 0, 0, 0}, //1, 2, 4
    {1, 1, 0, 1}, //1, 3, 4
    {1, 0, 1, 0} //2, 3, 4
  };

  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 4; j++) {
      phases1[i][j] = temp_phases1[i][j];
    }
  }

  // Заполняем массив phases2
  int temp_phases2[15][4] = {
    {0, 0, 0, 0}, //on
    {0, 0, 0, 0}, //1
    {0, 0, 0, 0}, //2
    {0, 0, 0, 0}, //3
    {0, 0, 0, 0}, //4
    {0, 0, 0, 0}, //1, 2
    {0, 0, 0, 0}, //2, 4
    {0, 0, 0, 0}, //3, 4
    {0, 0, 0, 0}, //1, 3
    {0, 1, 1, 0}, //1, 4
    {0, 1, 0, 1}, //2, 3
    {0, 1, 0, 1}, //1, 2, 3
    {0, 1, 1, 0}, //1, 2, 4
    {0, 1, 1, 0}, //1, 3, 4
    {0, 1, 0, 0} // 2, 3, 4
  };

  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 4; j++) {
      phases2[i][j] = temp_phases2[i][j];
    }
  }
  
}

void loop() {
  if (switch_phases(phases1[i % 15], phases2[i % 15], temp_time, is_phase)){
    is_phase = !is_phase;
    temp_time = millis();
  }
  if(blinking == 0)
    blinking = millis();
  if(millis() > blinking + 2000){
    i++;
    blinking = millis();
  }
  
}


bool switch_phases(int* phase1, int* phase2, long int start_loop, bool is_first_phase){
  if(millis() > start_loop + interval and is_first_phase){
    for(int i = 0; i < 4; i++)
      digitalWrite(pins[i], phase1[i]);
    start_loop = millis();
    is_first_phase = !is_first_phase;
    return true;
  }
  else if (millis() > start_loop + interval and !is_first_phase){
    for(int i = 0; i < 4; i++)
      digitalWrite(pins[i], phase2[i]);
    start_loop = millis();
    is_first_phase = !is_first_phase;
    return true;
  }
  return false;
}


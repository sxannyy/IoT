#define COLUMN_PIN_1 13
#define COLUMN_PIN_2 A0
#define COLUMN_PIN_3 A1
#define COLUMN_PIN_4 A2
#define COLUMN_PIN_5 A3
#define COLUMN_PIN_6 A4
#define COLUMN_PIN_7 A5
#define COLUMN_PIN_8 2

#define ROW_PIN_1 10
#define ROW_PIN_2 9
#define ROW_PIN_3 8
#define ROW_PIN_4 7
#define ROW_PIN_5 6
#define ROW_PIN_6 5
#define ROW_PIN_7 4
#define ROW_PIN_8 3

#define T 2

int heart[8][8] = {
    {0, 1, 1, 0, 0, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}
};

int columns[] = {
    COLUMN_PIN_1,
    COLUMN_PIN_2,
    COLUMN_PIN_3,
    COLUMN_PIN_4,
    COLUMN_PIN_5,
    COLUMN_PIN_6,
    COLUMN_PIN_7,
    COLUMN_PIN_8
};

int rows[] = {
    ROW_PIN_1,
    ROW_PIN_2,
    ROW_PIN_3,
    ROW_PIN_4,
    ROW_PIN_5,
    ROW_PIN_6,
    ROW_PIN_7,
    ROW_PIN_8
};

unsigned long start_loop = 0;
int current_row = 0;

void setup() {
    for (int i = 0; i < 8; i++) {
        pinMode(columns[i], OUTPUT);
        pinMode(rows[i], OUTPUT);
        digitalWrite(columns[i], LOW);
        digitalWrite(rows[i], LOW);
    }
    digitalWrite(rows[7], HIGH);
    digitalWrite(columns[7], HIGH);
}

void loop() {
    if (millis() - start_loop >= T) {
        digitalWrite(rows[current_row], LOW);
        current_row = (current_row + 1) % 8;
        digitalWrite(rows[current_row], HIGH);

        for (int j = 0; j < 8; j++) {
            digitalWrite(columns[j], !heart[current_row][j]);
        }

        start_loop = millis();
    }
}

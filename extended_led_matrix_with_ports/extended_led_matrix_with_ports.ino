int heart[8][8] = {
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1}
};

int current_row = 0;
int set_output_pins = (1 << 9) - 1;

void setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 30; // Value to compare with
  TCCR1B |= (1 << WGM12);
  // for 256 prescaler
  TCCR1B = TCCR1B | (1 << CS12); //bitSet(TCCR1B, CS12);  
  // for 1024 prescaler
  //TCCR1B = TCCR1B | ((1 << CS12) | (1 << CS10)); 
  // timer overflow interrupt
  TIMSK1 |= (1 << OCIE1A);

  DDRD |= set_output_pins;
  DDRC |= (set_output_pins ^ ((1 << 6) | (1 << 7)));
  DDRB |= ((1 << DDB0) |(1 << DDB1));
  sei();
}

void loop() {
  
}

ISR(TIMER1_COMPA_vect) {
  current_row++;
  current_row %= 8;
  PORTD = 0 | (1 << current_row);
  PORTC = 0;
  PORTB = 0;
  int k = 0;
  for(int i = 0; i < 8; i++){
    if(i < 6)
      PORTC ^= ((heart[current_row][i]) << i);
    else{
      PORTB ^= ((heart[current_row][i]) << k);
      k++;
    }
  }
  
}

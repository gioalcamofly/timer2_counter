#include <avr/interrupt.h>

uint16_t int_count= 0;
uint16_t int_count2 = 0;
uint8_t value;
uint8_t i = 0;
uint8_t int_time = 5;
int16_t count = 0;
boolean increment = LOW;
boolean pause = LOW;
int8_t sum = 1;
uint8_t period = 10;


uint8_t table [10] = {
  0x3F,
  0x06,
  0x5B,
  0x4F,
  0x66,
  0x6D,
  0x7D,
  0x07,
  0x7F,
  0x6F
};

void setup() {
  Serial.begin(9600);
  DDRD = B11111100;
  DDRB = B100011;
  DDRC = B001111;
  set_timer();
}

void set_timer() {
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~((1<<CS21) | (1<<CS20) | (1<<WGM22));
  TCCR2B |= (1<<CS22);
  ASSR &= ~(1<<AS2);
  value = 256 - (int)((float)F_CPU*0.001/64.0);
  TCNT2 = value;
  TIMSK2 |= (1<<TOIE2);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "reset") {
      count = 0;
    } else if (command == "pause on") {
      pause = HIGH;
    } else if (command == "pause off") {
      pause = LOW;
    } else if (command == "up") {
      sum = 1;
    } else if (command == "down") {
      sum = -1;
    } else if (command.startsWith("period") && isDigit(command.charAt(7))) {
      String sub = command.substring(7);
      uint8_t new_period = sub.toInt();
      if (new_period != 0) { period = new_period; }
    }
  }

}

void int_handler() {
  
  uint16_t x = count/(1*pow(10, i));
  showNumber(x%10);
  if (i == 0) {
    PINB=(1<<5);
  }
  
  i++;
  
  if (i == 4) {
    i = 0;
  }

}

void counter() {
  increment = LOW;
  count += sum;
  if (count == 6000) {
    count = 0;
  } else if (count == -1) {
    count = 5999;
  }
}


void showNumber(uint16_t x) {

  
    PORTC = (PORTC = 0x0F) & ~(1<<i);
    //Serial.println(table[numbers[i]], HEX);
    PORTD = (PORTD &= 0x03) | (table[x]<<2);
    PORTB = (PORTB &= 0x7C) | (table[x]>>6);
    
    
}

void is_overflow() {
  int_count += 1;
  int_count2 +=1;
  if (int_count >= int_time) {
    int_count -= int_time;

    
    int_handler();
  }

  if (int_count2 >= period && pause == LOW) {
    int_count2 -= period;
    counter();
  }
}

ISR(TIMER2_OVF_vect) {
  TCNT2 = value;
  is_overflow();
}


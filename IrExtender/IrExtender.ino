/*

  IrExtender

  This sketch allows you to create an IR remote extender.

  Connect an IR receiver to pin 1 and an IR LED to pin 5. A received IR
  signal will be remodulated with a 38khz carrier frequency and outputted
  to the IR LED. Depending on your arduino and IR LED you may need to power
  the LED directly from a power source through a transistor driven by
  the arduino.

  Used components:

  - IR Receiver: https://www.sparkfun.com/products/10266
  - IR LED:      https://www.sparkfun.com/products/9349

*/

#include <PinChangeInt.h>

const int IR_RECEIVER_PIN = 1;
const int IR_LED_PIN = 5;

volatile bool ledLevel = LOW;
volatile bool carrierLevel = ledLevel;

void setup() {
  pinMode(IR_LED_PIN, OUTPUT);

  cli();      // disable global interrupts
  TCCR1A = 0; // disable interrupt configuration registers
  TCCR1B = 0; // disable interrupt configuration registers

  // We want a 38khz carrier for the led, Light Blue Bean is 8Mhz.
  // Use no prescaler and 50% duty cycle:
  // (0.5/38000)/(1/8000000) = 105 ticks per toggle
  OCR1A = 105;
  // Turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for no prescaling:
  TCCR1B |= (1 << CS10);
  // Enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // Enable global interrupts
  sei();

  // Since the initial ledLevel is hard coded to LOW above, this will only
  // work if IR led is LOW while setup is running.
  PCintPort::attachInterrupt(IR_RECEIVER_PIN, toggle, CHANGE);
}

void loop() { }

void toggle() {
  ledLevel = !ledLevel;
}

// For each 38khz interrupt we toggle the carrier level and update LED pin
ISR(TIMER1_COMPA_vect) {
  carrierLevel = !carrierLevel;
  digitalWrite(IR_LED_PIN, ledLevel && carrierLevel);
}

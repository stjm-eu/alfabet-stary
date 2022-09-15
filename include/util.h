#include <Arduino.h>

// Zwraca ilość bitów równych 1 w podanej zmiennej.
int popCount(uint32_t x) {
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x = (x + (x >> 4)) & 0x0F0F0F0F;
  x = x + (x >> 8);
  x = x + (x >> 16);
  return x & 0x0000003F;
}

// Zamienia bitfield na string złożony z 0 i 1.
String bitString(uint32_t bits) {
  char string[] = "00000000000000000000000000000000";
  for (int i = 0; i < 32; i++) {
    if ((bits >> (31 - i)) & 1) {
      string[i] = '1';
    }
  }
  return string;
}

String debugBitfield(uint32_t bits, uint32_t data, uint32_t mask) {
  char string[] = "00000000000000000000000000000000";
  for (int i = 0; i < 32; i++) {
    if (bitRead(mask, 31 - i)) {
      if (bitRead(bits ^ data, 31 - i)) {
        string[i] = '1';
      }
      else {
        string[i] = '0';
      }
    }
    else {
      if (bitRead(bits ^ data, 31 - i)) {
        string[i] = '-';
      }
      else {
        string[i] = '_';
      }
    }
  }
  return string;
}

void led_high() {
  digitalWrite(LED_BUILTIN, HIGH);
}
void led_high(unsigned long ms) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(ms);
}

void led_low() {
  digitalWrite(LED_BUILTIN, LOW);
}
void led_low(unsigned long ms) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(ms);
}

void serialCountdown(int i) {
  for (; i > 0; i--) {
    Serial.print(i);
    Serial.print('.');
    delay(1000);
  }
}
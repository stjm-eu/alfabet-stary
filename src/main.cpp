#include <Arduino.h>

// Lista pinów cyfrowych.
const int digitalPin[9] = {
  24, // kciuk opuszek
  23, // kciuk bok
  25, // wskazujący opuszek
  15, // wskazujący bok
  22, // środkowy opuszek
  17, // środkowy bok
  12, //serdeczny opuszek
  11, // serdeczny bok
  9 // mały opuszek
};

// Numer pinu analogowego i jego wartość graniczna.
struct AnalogPin {
  const int pin;
  const int treshold;
};

// Lista pinów analogowych i ich wartości granicznych.
const AnalogPin analog[10] = {
  {A0, 520}, // mały 1 (dół)
  {A1, 720}, // mały 2 (góra)
  {A2, 500}, // serdeczny 1
  {A3, 570}, // serdeczny 2
  {A4, 470}, // środkowy 1
  {A5, 730}, // środkowy 2
  {A6, 600}, // wskazujący 1
  {A7, 620}, // wskazujący 2
  {A8, 500}, // kciuk 1
  {A9, 500} // kciuk 2
};

// Litera z jej bitfieldem i maską.
struct LetterStruct {
  const char* letter;
  uint32_t bits;
  uint32_t mask;
};

// Lista liter z ich bitfieldami i maskami.
const LetterStruct letters[] = {
  { "a",  0b11111111010000000000000111111110ul,
          0b11111111110000000000000111111111ul },
  { "b",  0b00000000010000000000000010101010ul,
          0b11111111110000000000000111111111ul },
  { "c",  0b11111101100000000000000111110000ul,
          0b11111111110000000000000111111111ul },
  { "ch", 0b11110101100000000000000111001000ul,
          0b11111111110000000000000111111111ul },
  { "d",  0b11111100100000000000000111110011ul,
          0b11111111110000000000000111111111ul },
  { "e",  0b10101010000000000000000010101001ul,
          0b11111111110000000000000111111111ul },
  { "f",  0b00000011000000000000000000000010ul,
          0b11111111110000000000000111111111ul },
  { "g",  0b00001100100000000000000000000000ul,
          0b11111111110000000000000111111111ul },
  { "h",  0b11110101110000000000000111000000ul,
          0b11111111110000000000000111111111ul },
  { "i",  0b00111111010000000000000001111110ul,
          0b11111111110000000000000111111111ul },
  { "k",  0b11111010000000000000000111000000ul,
          0b11111111110000000000000111111111ul },
  { "l",  0b11111100000000000000000111110000ul,
          0b11111111110000000000000111111111ul },
  { "m",  0b10101010000000000000000010101000ul,
          0b11111111110000000000000111111111ul },
  { "n",  0b11111010000000000000000111001000ul,
          0b11111111110000000000000111111111ul },
  { "o",  0b00000011100000000000000000000000ul,
          0b11111111110000000000000111111111ul },
  { "p",  0b11111110000000000000000111110001ul,
          0b11111111110000000000000111111111ul },
  { "w",  0b11000000010000000000000100000000ul,
          0b11111111110000000000000111111111ul },
  { "y",  0b00111100010000000000000001110000ul,
          0b11111111110000000000000111111111ul },
};

// Ilość liter.
const int LETTERS_LENGTH = sizeof(letters) / sizeof(letters[0]);

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

// Odczytuje, łączy i zwraca danych z czujników.
uint32_t readSensors() {
  uint32_t sensorBitfield = 0;
// odczytywanie danych z czujników cyfrowych - bitfield od prawej
  for (int i = 0; i < 9; i++) {
    bitWrite(sensorBitfield, i, !digitalRead(digitalPin[i]));
  }
// odczytywanie danych z czujników analogowych - bitfield od lewej
  for (int i = 0; i < 10; i++) {
    bitWrite(sensorBitfield, 31 - i, analogRead(analog[i].pin) > analog[i].treshold);
  }
  return sensorBitfield;
}

// Zwraca indeks najlepiej dopasowanej litery wraz ze stopniem dopasowania.
auto matchLetter(uint32_t sensorsBitfield) {
  int matchedIndex = 0;
  int bestMatchXOR = 1;
  int bestMatchMask = 1;

  for (int i = 0; i < LETTERS_LENGTH; i++) {
    uint32_t bitfieldXOR = (sensorsBitfield ^ letters[i].bits) & letters[i].mask;

    if (bitfieldXOR == 0) {
      matchedIndex = i;
      bestMatchXOR = 0;
      bestMatchMask = 1;
      break;
    }

    const int matchXOR = popCount(bitfieldXOR);
    const int matchMask = popCount(letters[i].mask);
    if (matchXOR * bestMatchMask < bestMatchXOR * matchMask) {
      matchedIndex = i;
      bestMatchXOR = matchXOR;
      bestMatchMask = matchMask;
    }
  }

  struct result { int index; float match; };
  return result { matchedIndex, (float) bestMatchXOR / bestMatchMask };
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 9; i++) {
    pinMode(digitalPin[i], INPUT_PULLUP);
  }
}

void loop() {
  static int previous;
  static int iterations;

  delay(100);

// 32-bitowy bitfield przechowujący dane z czujników.
  uint32_t bitfield = readSensors();

  // Serial.println(bitString(bitfield));

  auto [index, match] = matchLetter(bitfield);
  if (match < 0.5) {
    if (index == previous) {
      iterations++;
      if (iterations == 8) {
        Serial.print(' ');
        Serial.print(letters[index].letter);
      }
    }
    else {
      previous = index;
      iterations = 0;
    }
  }
  else {
    previous = -1;
    iterations = 0;
  }
  // Serial.println(match);
}
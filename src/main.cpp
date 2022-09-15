#include <Arduino.h>
#include <Wire.h>
#include "data.h"
#include "util.h"
#include <I2Cdev.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void readMPU() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
}

bool dev_log = false;

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

void calibrate() {
  Serial.println("\nKalibrowanie czujników zgięcia");

  int straight[10];
  int bent[10];

  Serial.println("Wyprostuj palce");
  serialCountdown(5);

  for (int i = 0; i < 10; i++) {
    straight[i] = analogRead(analog[i].pin);
  }

  Serial.println("\nZegnij palce");
  serialCountdown(5);

  for (int i = 0; i < 10; i++) {
    bent[i] = analogRead(analog[i].pin);
  }

  for (int i = 0; i < 10; i++) {
    analog[i].treshold = (straight[i] + bent[i]) / 2;
  }

  Serial.println("Skalibrowano czujniki zgięcia!");

  Serial.println(F("Ułóż rękę w pozycji do kalibracji MPU6050"));
  delay(1000);
  serialCountdown(5);

  Serial.print("\nKalibrowanie akcelerometru ");
  mpu.CalibrateAccel();

  Serial.print("\nKalibrowanie żyroskopu ");
  mpu.CalibrateGyro();

  Serial.println("\nSkalibrowano MPU6050!");
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  led_low();

  for (int i = 0; i < 9; i++) {
    pinMode(digitalPin[i], INPUT_PULLUP);
  }

  // delay(1000);

  Wire.begin();
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  calibrate();

  // while(true) {
  //   readMPU();
  //   Serial.print("a/g:\t");
  //   Serial.print(ax); Serial.print("\t");
  //   Serial.print(ay); Serial.print("\t");
  //   Serial.print(az); Serial.print("\t");
  //   Serial.print(gx); Serial.print("\t");
  //   Serial.print(gy); Serial.print("\t");
  //   Serial.println(gz);
  //   delay(100);
  // };
}

void loop() {
  static int previous;
  static int iterations;

  delay(40);

  readMPU();

// 32-bitowy bitfield przechowujący dane z czujników.
  uint32_t bitfield = readSensors();

  if (dev_log) Serial.println(bitString(bitfield));

  auto [index, match] = matchLetter(bitfield);
  if (match < 0.3) {
    if (index == previous) {
      iterations++;
      if (iterations == 6) {
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
  if (dev_log) Serial.println(match);
}

void executeCommand(String inString) {
  if (inString == "kalibracja") calibrate();
  else if (inString == "log") dev_log = true;
  else if (inString == "!log") {
    dev_log = false;
    for (int i = 0; i < 10; i++, Serial.println());
  }
  else {
    Serial.write("Błąd: nieznana komenda");
  }
}

void serialEvent() {
  static String inString;

  while (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == '\n') {
      executeCommand(inString);
      inString = "";
    }
    else {
      inString += inChar;
    }
  }
}
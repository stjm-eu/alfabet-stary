#include <Arduino.h>

// Lista pinów cyfrowych.
const int digitalPin[9] = {
	24, // kciuk opuszek
	23, // kciuk bok
	25, // wskazujący opuszek
	15, // wskazujący bok
	22, // środkowy opuszek
	17, // środkowy bok
	12, // serdeczny opuszek
	11, // serdeczny bok
	9	// mały opuszek
};

// Numer pinu analogowego i jego wartość graniczna.
struct AnalogPin
{
	const int pin;
	int treshold;
};

// Lista pinów analogowych i ich wartości granicznych.
AnalogPin analog[10] = {
	{A0, 520}, // mały 1 (dół)
	{A1, 720}, // mały 2 (góra)
	{A2, 500}, // serdeczny 1
	{A3, 570}, // serdeczny 2
	{A4, 470}, // środkowy 1
	{A5, 730}, // środkowy 2
	{A6, 600}, // wskazujący 1
	{A7, 620}, // wskazujący 2
	{A8, 500}, // kciuk 1
	{A9, 500}  // kciuk 2
};

// Litera z jej bitfieldem i maską.
struct LetterStruct
{
	const char *letter;
	uint32_t bits;
	uint32_t mask;
};

// Lista liter z ich bitfieldami i maskami.
const LetterStruct letters[] = {
	{"a", 0b11111111010000000000000111111110ul,
	      0b11111111110000000000000111111111ul},
	{"b", 0b00000000010000000000000010101010ul,
	      0b11111111110000000000000111111111ul},
	{"c", 0b11111101100000000000000111110000ul,
	      0b11111111110000000000000111111111ul},
	{"ch", 0b11110101100000000000000111001000ul,
	      0b11111111110000000000000111111111ul},
	{"d", 0b11111100100000000000000111110011ul,
	      0b11111111110000000000000111111111ul},
	{"e", 0b10101010000000000000000010101001ul,
	      0b11111111110000000000000111111111ul},
	{"f", 0b00000011000000000000000000000010ul,
	      0b11111111110000000000000111111111ul},
	{"g", 0b00001100100000000000000000000000ul,
	      0b11111111110000000000000111111111ul},
	{"h", 0b11110101110000000000000111000000ul,
	      0b11111111110000000000000111111111ul},
	{"i", 0b00111111010000000000000001111110ul,
	      0b11111111110000000000000111111111ul},
	{"k", 0b11111010000000000000000111000000ul,
	      0b11111111110000000000000111111111ul},
	{"l", 0b11111100000000000000000111110000ul,
	      0b11111111110000000000000111111111ul},
	{"m", 0b10101010000000000000000010101000ul,
	      0b11111111110000000000000111111111ul},
	{"n", 0b11111010000000000000000111001000ul,
	      0b11111111110000000000000111111111ul},
	{"o", 0b00000011100000000000000000000000ul,
	      0b11111111110000000000000111111111ul},
	{"p", 0b11111110000000000000000111110001ul,
	      0b11111111110000000000000111111111ul},
	{"w", 0b11000000010000000000000100000000ul,
	      0b11111111110000000000000111111111ul},
	{"y", 0b00111100010000000000000001110000ul,
	      0b11111111110000000000000111111111ul},
};

// Ilość liter.
const int LETTERS_LENGTH = sizeof(letters) / sizeof(letters[0]);

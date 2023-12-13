#include "LiquidCrystal.h"

//  Custom chars
byte customChars[3][8] = {
	{ 0b00100, 0b01110, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 },
    { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b01110, 0b00100 },
    { 0b01110, 0b10001, 0b01110, 0b00100, 0b00100, 0b00100, 0b00110, 0b00100 }
};

//  Defines for custom chars
#define UP_ARROW 0
#define DOWN_ARROW 1
#define KEY 2

struct Display {
    //  Pins
    const byte rs;
    const byte en;
    const byte d4;
    const byte d5;
    const byte d6;
    const byte d7;
    const byte A;

    const byte numRows = 2;
    const byte numCols = 16;

    LiquidCrystal lcd;

    //  Constructor
    Display(byte rs, byte en, byte d4, byte d5, byte d6, byte d7, byte A)
    : rs(rs), en(en), d4(d4), d5(d5), d6(d6), d7(d7), A(A), lcd(LiquidCrystal(rs, en, d4, d5, d6, d7)) {}

    void setBrightness(const byte brightness) {
        analogWrite(A, 256 / 8 * brightness - 1);
    }

    void setup() {
        lcd.createChar(UP_ARROW, customChars[UP_ARROW]);
        lcd.createChar(DOWN_ARROW, customChars[DOWN_ARROW]);
        lcd.createChar(KEY, customChars[KEY]);

        lcd.begin(16, 2);

        pinMode(A, OUTPUT);
    }
};
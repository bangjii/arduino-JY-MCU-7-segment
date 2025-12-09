#ifndef JYMCU6Digit_h
#define JYMCU6Digit_h

#include <Arduino.h>

class JYMCU6Digit {
public:
    JYMCU6Digit(uint8_t sdi, uint8_t clk, uint8_t le, uint8_t oe);

    void begin(uint8_t digits = 6);

    void clear();
    void render();     // <-- penting

    void printDigit(uint8_t pos, uint8_t val);
    void setChar(uint8_t pos, char ch, bool dot = false);
    void setRaw(uint8_t pos, uint8_t raw);
    void printRaw(uint8_t digit, uint8_t pattern);

    void printNumber(long num, bool leadingZeros=false);
    void displayOn();
    void displayOff();

private:
    uint8_t pinSDI, pinCLK, pinLE, pinOE;
    uint8_t numDigits;

    uint8_t buffer[6];  // <-- FIX UTAMA

    void shiftOutByte(uint8_t data);
    void latch();

};

#endif

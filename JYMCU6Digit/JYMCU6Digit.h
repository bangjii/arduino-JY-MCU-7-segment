#ifndef JYMCU7SEGMENT_H
#define JYMCU7SEGMENT_H

#include <Arduino.h>

class JYMCU7SEGMENT {
public:
    JYMCU7SEGMENT(uint8_t sdi, uint8_t clk, uint8_t le, uint8_t oe);

    void begin(uint16_t digits);

    void clear();
    void render();

    // Full display printing (right aligned)
    void printInt(long num);
    void printFloat(float num, uint8_t decimals = 1);
    void printString(const char* txt);

    // Per-digit printing (left-to-right indexing)
    void printIntAt(uint16_t pos, long value);
    void printStringAt(uint16_t pos, const char* str);
    void printCharAt(uint16_t pos, const char* str);
    void printCharAt(uint16_t pos, char c, bool dot = false);
    void printRawAt(uint16_t pos, uint8_t pattern);

    // Manual render control
    void enableAutoRender(bool enable);
    void update();

    // Display control
    void clearRange(uint16_t start, uint16_t length);
    void setBrightness(uint8_t level);
    void displayOn();
    void displayOff();

private:
    uint8_t pinSDI, pinCLK, pinLE, pinOE;
    uint16_t numDigits;

    bool autoRender = true;
    uint8_t* buffer;

    void shiftOutByte(uint8_t data);
};

#endif

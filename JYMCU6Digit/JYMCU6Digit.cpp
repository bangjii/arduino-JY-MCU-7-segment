#include "JYMCU6Digit.h"

// Digit map
static const uint8_t digitMap[10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

// Symbols
static const uint8_t CH_BLANK  = 0b00000000;
static const uint8_t CH_MINUS  = 0b01000000;
static const uint8_t CH_C      = 0b01100001;

JYMCU6Digit::JYMCU6Digit(uint8_t sdi, uint8_t clk, uint8_t le, uint8_t oe) {
    pinSDI = sdi;
    pinCLK = clk;
    pinLE  = le;
    pinOE  = oe;
}

void JYMCU6Digit::begin(uint8_t digits) {
    numDigits = digits;

    pinMode(pinSDI, OUTPUT);
    pinMode(pinCLK, OUTPUT);
    pinMode(pinLE, OUTPUT);
    pinMode(pinOE, OUTPUT);

    digitalWrite(pinOE, LOW);
    clear();
}

void JYMCU6Digit::shiftOutByte(uint8_t data) {
    for (int i = 7; i >= 0; i--) {
        digitalWrite(pinCLK, LOW);
        digitalWrite(pinSDI, (data >> i) & 1);
        digitalWrite(pinCLK, HIGH);
    }
}

void JYMCU6Digit::latch() {
    digitalWrite(pinLE, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLE, LOW);
}

void JYMCU6Digit::clear() {
    for (int i = 0; i < numDigits; i++)
        buffer[i] = 0;

    render();
}

void JYMCU6Digit::render() {
    for (int i = numDigits - 1; i >= 0; i--)
        shiftOutByte(buffer[i]);
    latch();
}

void JYMCU6Digit::displayOn()  { digitalWrite(pinOE, LOW); }
void JYMCU6Digit::displayOff() { digitalWrite(pinOE, HIGH); }

void JYMCU6Digit::printDigit(uint8_t pos, uint8_t val) {
    if (pos >= numDigits || val > 9) return;
    buffer[pos] = digitMap[val];
    render();
}

void JYMCU6Digit::setRaw(uint8_t pos, uint8_t raw) {
    if (pos >= numDigits) return;
    buffer[pos] = raw;
    render();
}

void JYMCU6Digit::setChar(uint8_t pos, char ch, bool dot) {
    uint8_t p = CH_BLANK;

    if (ch >= '0' && ch <= '9') p = digitMap[ch - '0'];
    else if (ch == '-') p = CH_MINUS;
    else if (ch == 'C' || ch == 'c') p = CH_C;

    if (dot) p |= 0x80;

    buffer[pos] = p;
    render();
}

void JYMCU6Digit::printNumber(long num, bool leadingZeros) {
    for (int i = 0; i < numDigits; i++) {
        uint8_t d = num % 10;
        num /= 10;

        if (!leadingZeros && num == 0 && d == 0 && i != 0)
            buffer[i] = CH_BLANK;
        else
            buffer[i] = digitMap[d];
    }
    render();
}
void JYMCU6Digit::printRaw(uint8_t digit, uint8_t pattern)
{
    if (digit >= numDigits) return;

    buffer[digit] = pattern;

    render();   // langsung tampilkan
}



#include "JYMCU7SEGMENT.h"

// Common cathode segment font: DP G F E D C B A
static const uint8_t DIGIT_MAP[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

static const uint8_t CH_BLANK = 0b00000000;
static const uint8_t CH_MINUS = 0b01000000;
static const uint8_t CH_C     = 0b01100001;
static const uint8_t CH_DEG   = 0b01100011;

// ============================================================================

JYMCU7SEGMENT::JYMCU7SEGMENT(uint8_t sdi, uint8_t clk, uint8_t le, uint8_t oe) {
    pinSDI = sdi;
    pinCLK = clk;
    pinLE  = le;
    pinOE  = oe;
}

void JYMCU7SEGMENT::begin(uint16_t digits) {
    numDigits = digits;

    buffer = (uint8_t*)malloc(numDigits);

    for (uint16_t i = 0; i < numDigits; i++)
        buffer[i] = CH_BLANK;

    pinMode(pinSDI, OUTPUT);
    pinMode(pinCLK, OUTPUT);
    pinMode(pinLE, OUTPUT);
    pinMode(pinOE, OUTPUT);

    digitalWrite(pinOE, LOW);  // display on
    clear();
}

// ============================================================================
// Low-level shift out
// ============================================================================
void JYMCU7SEGMENT::shiftOutByte(uint8_t data) {
    for (int8_t i = 7; i >= 0; i--) {
        digitalWrite(pinCLK, LOW);
        digitalWrite(pinSDI, (data >> i) & 1);
        digitalWrite(pinCLK, HIGH);
    }
}

// ============================================================================
// Render whole buffer → display
// ============================================================================
void JYMCU7SEGMENT::render() {
    for (int16_t i = 0; i < numDigits; i++)
        shiftOutByte(buffer[i]);

    digitalWrite(pinLE, HIGH);
    delayMicroseconds(1);
    digitalWrite(pinLE, LOW);
}

void JYMCU7SEGMENT::clear() {
    for (uint16_t i = 0; i < numDigits; i++)
        buffer[i] = CH_BLANK;
    render();
}

// ============================================================================
// NEW: clearRange - clear specific range of digits
// ============================================================================
void JYMCU7SEGMENT::clearRange(uint16_t start, uint16_t length) {
    for (uint16_t i = 0; i < length && (start + i) < numDigits; i++) {
        buffer[start + i] = CH_BLANK;
    }
    
    if (autoRender) render();
}

// ============================================================================
// PER-DIGIT PRINT (LEFT→RIGHT)
// ============================================================================
// ============================================================================
// FIXED: printIntAt - handle multi-digit numbers
// ============================================================================
void JYMCU7SEGMENT::printIntAt(uint16_t pos, long value) {
    // Handle negative numbers
    bool isNegative = false;
    if (value < 0) {
        isNegative = true;
        value = -value;
    }
    
    // Convert number to string to get digits
    char temp[20];
    ltoa(value, temp, 10);
    
    uint16_t startPos = pos;
    
    // Print negative sign if needed
    if (isNegative) {
        if (startPos < numDigits) {
            buffer[startPos] = CH_MINUS;
            startPos++;
        }
    }
    
    // Print each digit from left to right
    for (uint16_t i = 0; temp[i] != '\0' && startPos < numDigits; i++) {
        if (temp[i] >= '0' && temp[i] <= '9') {
            buffer[startPos] = DIGIT_MAP[temp[i] - '0'];
            startPos++;
        }
    }
    
    if (autoRender) render();
}

// ============================================================================
// FIXED: printCharAt - handle string of characters
// ============================================================================
void JYMCU7SEGMENT::printCharAt(uint16_t pos, const char* str) {
    uint16_t currentPos = pos;
    
    for (uint16_t i = 0; str[i] != '\0' && currentPos < numDigits; i++) {
        // Handle dot - apply to previous digit
        if (str[i] == '.') {
            if (currentPos > pos) {
                buffer[currentPos - 1] |= 0x80;
            }
            continue; // don't consume a new position
        }
        
        uint8_t p = CH_BLANK;
        if (str[i] >= '0' && str[i] <= '9') 
            p = DIGIT_MAP[str[i] - '0'];
        else if (str[i] == '-')        
            p = CH_MINUS;
        else if (str[i] == 'C' || str[i] == 'c') 
            p = CH_C;
        else if (str[i] == '°')        
            p = CH_DEG;
        
        buffer[currentPos] = p;
        currentPos++;
    }
    
    if (autoRender) render();
}

// Overload: single character version (backward compatibility)
void JYMCU7SEGMENT::printCharAt(uint16_t pos, char c, bool dot) {
    if (pos >= numDigits) return;

    uint8_t p = CH_BLANK;
    if (c >= '0' && c <= '9') p = DIGIT_MAP[c - '0'];
    else if (c == '-')        p = CH_MINUS;
    else if (c == 'C' || c == 'c') p = CH_C;
    else if (c == '°')        p = CH_DEG;

    if (dot) p |= 0x80;

    buffer[pos] = p;
    if (autoRender) render();
}

// ============================================================================
// NEW: printStringAt - print string starting from specific position
// ============================================================================
void JYMCU7SEGMENT::printStringAt(uint16_t pos, const char* str) {
    uint16_t currentPos = pos;
    
    for (uint16_t i = 0; str[i] != '\0' && currentPos < numDigits; i++) {
        // Handle dot - apply to previous digit
        if (str[i] == '.') {
            if (currentPos > pos) {
                buffer[currentPos - 1] |= 0x80;
            }
            continue; // don't consume a new position
        }
        
        uint8_t p = CH_BLANK;
        if (str[i] >= '0' && str[i] <= '9') 
            p = DIGIT_MAP[str[i] - '0'];
        else if (str[i] == '-')        
            p = CH_MINUS;
        else if (str[i] == 'C' || str[i] == 'c') 
            p = CH_C;
        else if (str[i] == '°')        
            p = CH_DEG;
        
        buffer[currentPos] = p;
        currentPos++;
    }
    
    if (autoRender) render();
}
void JYMCU7SEGMENT::printRawAt(uint16_t pos, uint8_t pattern) {
    if (pos >= numDigits) return;
    buffer[pos] = pattern;
    if (autoRender) render();
}

// ============================================================================
// FULL PRINT (RIGHT-ALIGN)
// ============================================================================
// LEFT-ALIGN printing: mulai dari pos = 0 (kiri) dan maju ke kanan
void JYMCU7SEGMENT::printString(const char* txt) {
    int16_t pos = 0; // mulai dari kiri (pos 0)

    for (uint16_t i = 0; txt[i] != 0 && pos < numDigits; i++) {
        if (txt[i] == '.') {
            // dot untuk digit sebelumnya (kiri->kanan)
            if (pos > 0) {
                // previous filled digit = pos-1
                buffer[pos - 1] |= 0x80;
            }
            // jangan konsumsi posisi baru
            continue;
        }

        uint8_t p = CH_BLANK;
        if (txt[i] >= '0' && txt[i] <= '9') p = DIGIT_MAP[txt[i] - '0'];
        else if (txt[i] == '-')             p = CH_MINUS;
        else if (txt[i] == 'C' || txt[i] == 'c') p = CH_C;
        else if (txt[i] == '°')             p = CH_DEG;
        // else remain blank for unsupported chars

        buffer[pos] = p;
        pos++;
    }

    // clear the rest to blanks
    while (pos < numDigits) {
        buffer[pos++] = CH_BLANK;
    }

    render();
}

void JYMCU7SEGMENT::printInt(long num) {
    char s[20];
    ltoa(num, s, 10);
    printString(s);
}

void JYMCU7SEGMENT::printFloat(float num, uint8_t decimals) {
    char s[32];
    dtostrf(num, 0, decimals, s);
    printString(s);
}


// ============================================================================
// CONTROL
// ============================================================================
void JYMCU7SEGMENT::enableAutoRender(bool enable) {
    autoRender = enable;
}

void JYMCU7SEGMENT::update() {
    render();
}

void JYMCU7SEGMENT::setBrightness(uint8_t level) {
    analogWrite(pinOE, 255 - level); // active low brightness
}

void JYMCU7SEGMENT::displayOn()  { digitalWrite(pinOE, LOW); }
void JYMCU7SEGMENT::displayOff() { digitalWrite(pinOE, HIGH); }

#include "JYMCU6Digit.h"

#define PIN_SDI 4
#define PIN_CLK 3
#define PIN_LE  2
#define PIN_OE 11

JYMCU6Digit disp(PIN_SDI, PIN_CLK, PIN_LE, PIN_OE);

void setup() {
  disp.begin(3);                // jumlah digit yang digunakan
  disp.printNumber(123);        // tampilkan angka 123
  disp.printDigit(2, 2);        // hanya tampilkan angka 2 pada digit ke-2
  disp.printRaw(1, 0b01100001); // huruf 'c' kecil
}

void loop() {
  //for (int a = 0; a < 999; a++){    
    //disp.displayNumber(a);
    //delay(1000);
  //}
}

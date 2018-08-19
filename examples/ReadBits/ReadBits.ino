#include <bitreader.h>

/**
* Encoding "text" as 5 bits characters using the following:
*   A -> 0
*   Z -> 25
*     -> 26
*   . -> 27
*   0 -> 28
*   1 -> 29
*   2 -> 30
*   3 -> 31
*/

#ifndef USE_BITADDRESS_READ_WRITE
BitReader bitreader;
#else
BitAddress bitreader;
#endif

const unsigned char textBuffer[] = {0x87, 0xAC, 0xE5, 0x34, 0xC3, 0xBA, 0x01, 0x46, 0x34, 0x92, 0x1A, 0xB4, 0xE9, 0x50, 0x23, 0x5B, 0x23, 0x6D, 0xA3, 0x9B, 0x44, 0xCF, 0x43, 0x74, 0x40, 0x33, 0x12, 0x30, 0x48, 0xD4, 0x0B, 0x85, 0x08, 0x22, 0xDE};
int textLength = 56;

void setup() {
  Serial.begin(9600);

  bitreader.setBuffer(textBuffer);
  
  Serial.println("Decoding bits...");
  unsigned char c = 0;
  for(int i=0; i<textLength; i++) {
    c = 0;
    bitreader.read(5, &c);
    //Serial.println(c);
    if (c >= 0 && c <= 25)
      c += 'A';
    else if (c == 26)
      c = ' ';
    else if (c == 27)
      c = '.';
    else if (c >= 28 && c <= 31)
      c += ('0' - 28);
    Serial.print((char)c);
  }
  Serial.println();
  
}

void loop() {
}


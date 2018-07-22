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
BitWriter bitwriter;
#else
BitAddress bitwriter;
#endif

const char * text = "HELLO MY NAME IS ANTOINE. I WROTE THE BITREADER LIBRARY.";

#define BUFFER_SIZE 50
unsigned char textBuffer[BUFFER_SIZE];

void setup() {
  Serial.begin(9600);
  
  //set all bits of textBuffer to 0 to be "printable"
  bitwriter.setBuffer(textBuffer);

  //write all characters of text to the BitWriter
  const char * character = text;
  while(*character != '\0') {
    Serial.print("Encoding character: '");
    Serial.print(*character);
    unsigned char valueInBits = *character;
    if (valueInBits >= 'A' && valueInBits <= 'Z')
      valueInBits -= 'A'; //back to 0 based
    else if (valueInBits == ' ')
      valueInBits = 26;
    else if (valueInBits == '.')
      valueInBits = 27;
    else if (valueInBits >= '0' && valueInBits <= '3')
      valueInBits -= ('0' - 28); //back to 0 based

    Serial.print("' into valueInBits -> ");
    Serial.println(valueInBits);
    
    //write bits into textBuffer
    bitwriter.write(&valueInBits, 5);

    character++; //next character
  }

  //show the content of buffer
  #ifndef USE_BITADDRESS_READ_WRITE
  int textBufferSize = bitwriter.getAddress().byte;
  #else
  int textBufferSize = bitwriter.byte;
  #endif
  Serial.print("Encoded ");
  Serial.print(strlen(text));
  Serial.print(" bytes into ");
  Serial.print("textBuffer which is now ");
  Serial.print(textBufferSize);
  Serial.println(" bytes.");
  Serial.print("const unsigned char textBuffer[] = {");
  for(int i=0; i<textBufferSize; i++)
  {
    char tmp[5];
    sprintf(tmp, "0x%02X", textBuffer[i]);
    if (i != 0) Serial.print(", ");
    Serial.print(tmp);
  }
  Serial.println("};");
}

void loop() {
}


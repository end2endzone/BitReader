#include <bitreader.h>

/**
* Encoding "text" as 5 bits characters using the following:
*   A -> 0
*   Z -> 26
*     -> 27
*   . -> 28
*   0 -> 29
*   1 -> 30
*   2 -> 31
*/

#define BUFFER_SIZE 600
unsigned char sourceBuffer[BUFFER_SIZE];
unsigned char targetBuffer[BUFFER_SIZE];

void setup() {
  Serial.begin(9600);

  #ifdef USE_BITADDRESS_SETTER_GETTER
  Serial.println("#define USE_BITADDRESS_SETTER_GETTER");
  #endif
  #ifdef USE_SINGLEBIT_MACROS
  Serial.println("#define USE_SINGLEBIT_MACROS");
  #endif
  #ifdef USE_BITADDRESS_READ_WRITE
  Serial.println("#define USE_BITADDRESS_READ_WRITE");
  #endif

  //copy all characters from sourceBuffer to targetBuffer using 1 to 16 bits block.
  for(int numBits = 1; numBits<=16; numBits++) {
    #ifndef USE_BITADDRESS_READ_WRITE
    BitReader bitreader;
    BitWriter bitwriter;
    #else
    BitAddress bitreader;
    BitAddress bitwriter;
    #endif
    
    bitreader.setBuffer(sourceBuffer);
    bitwriter.setBuffer(targetBuffer);

    //initialize sourceBuffer
    for(int i=0; i<BUFFER_SIZE; i++) {
      sourceBuffer[i] = i+numBits;
    }

    //start counter
    unsigned long timeStart = millis();

    //proceed with buffer copy
    int totalBits = BUFFER_SIZE*BITS_PER_BYTE;
    int numBlocks = totalBits / numBits;
    int temp = 0;
    for(int i=0; i<numBlocks; i++) {
      bitreader.read(numBits, &temp);
      bitwriter.write(&temp, numBits);
    }

    //stop timer
    unsigned long timeEnd = millis();
    unsigned long timeElapsed = timeEnd - timeStart;

    //make sure both buffer are equals (that bit copy operation worked as expected)
    bool equal = true;
    for(int i=0; i<BUFFER_SIZE; i++) {

      // Do not validate the last 2 bytes of the buffer
      // With certain numBits size, the remaining bits
      // of the source buffer are not copied to the 
      // target buffer. The same as would 3, 6 and 7 bits
      // would not copy the totality of a byte (8 bits)
      if (i >= BUFFER_SIZE-2) {
        continue;
      }
      
      if (sourceBuffer[i] != targetBuffer[i]) {
        equal = false;
      }
    }
    
    //outputting benchnmark test values
    if (!equal) {
      Serial.println("*** Warning! Both buffers are not equals!");
    }
    char msg[65];
    sprintf(msg, "Copied %d bytes using blocks of %d bits in %d milliseconds.", BUFFER_SIZE, numBits, timeElapsed);
    Serial.println(msg);
  }
}

void loop() {
}


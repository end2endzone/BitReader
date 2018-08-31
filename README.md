![BitReader logo](https://github.com/end2endzone/BitReader/raw/master/docs/BitReader-splashscreen.png)


[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/BitReader.svg)](https://github.com/end2endzone/BitReader/releases)



# BitReader #

The BitReader library is an arduino library that allows one to read or write data which is not aligned on 8, 16 or 32 bits variables. In other words, the library allows one to quickly store (or read) multiple variables from binary buffers.

Library features:
* Reduces size of data (strings, structures, arrays) in memory to the minimum amount of bits required.
* Help reading/updating bit-field structures.
* Handles bit [serialization](http://en.wikipedia.org/wiki/Serialization) data to/from buffers.



## Status ##

Build:

| Service | Build | Tests |
|----|-------|-------|
| AppVeyor | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/BitReader/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/BitReader) | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/BitReader/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/BitReader/branch/master/tests) |
| Travis CI | [![Build Status](https://img.shields.io/travis/end2endzone/BitReader/master.svg?logo=travis&style=flat)](https://travis-ci.org/end2endzone/BitReader) |  |

Statistics:

| AppVeyor | Travic CI |
|----------|-----------|
| [![Statistics](https://buildstats.info/appveyor/chart/end2endzone/BitReader)](https://ci.appveyor.com/project/end2endzone/BitReader/branch/master) | [![Statistics](https://buildstats.info/travisci/chart/end2endzone/BitReader)](https://travis-ci.org/end2endzone/BitReader) |



# Purpose #

Desktop application developers are not used to have memory (or even CPU) requirements. Most will not look at memory consumption as an arduino developer would. Memory & CPU is cheap. If an application is struggling to run on today's computer, it will run smoothly in 1-2 years. Arduino developers know that the amount of memory available on embedded devices is pretty limited. Having a way to save bits of precious memory is more than interesting.

The conventional way of storing bit data into sequential buffers is to use a combination of [bit mask](http://en.wikipedia.org/wiki/Mask_(computing)) and [bit shift](http://en.wikipedia.org/wiki/Logical_shift):

```cpp
//define storage for multiple values
uint8_t value1; //5 bits
uint8_t value2; //8 bits
uint8_t value3; //1 bit
uint8_t value4; //6 bits

uint8_t buffer[] = {0xAA, 0xAA, 0x0F};

// buffer[2] buffer[1] buffer[0]
//  00001111  10101010  10101010
//                         |---| ==> value1
//               |----  --| ==> value2
//              | ==> value3
//      |---  -| ==> value4

value1 =   buffer[0] & 0x1F;        //first 5 bits of buffer[0]
value2 = ((buffer[0] & 0xE0) >> 5)+ //next  3 bits of buffer[0]
         ((buffer[1] & 0x1F) << 3); //first 5 bits of buffer[1]
value3 = ((buffer[1] & 0x20) >> 5); //next    bit  of buffer[1]
value4 = ((buffer[1] & 0xC0) >> 6)+ //next  2 bits of buffer[1]
         ((buffer[2] & 0x0F) << 2); //next  4 bits of buffer[2]

//expect value1 == 0x0A
//expect value2 == 0x55
//expect value3 == 0x01
//expect value4 == 0x3E
```

This approach works great if you do not make a change on existing code but it gets pretty complicated and error prone as soon as you need to make a change:

* Insert `value0` (2 bits) at the beginning.
* Insert a new value between `value3` and `value4`.
* Change the size of `value3`
* etc...

In these kind of situations, the BitReader library becomes handy and takes care of all the bit handling complexity.



# Usage #

The following instructions show how to use the library.




## General ##

Create an instance of `BitReader`, `BitWriter` or `BitAddress` depending on the following optimization switches:
* USE_BITADDRESS_SETTER_GETTER
* USE_SINGLEBIT_MACROS
* USE_BITADDRESS_READ_WRITE

Then assign a reading or writing buffer using the `setBuffer()` method.

Call the `write()` or `read()` methods for writing or reading bits to the assigned buffer. Each method allows one to specify the amount of bits to read or write.




## Making text strings shorter ##

The library is useful for storing or decoding data in binary format using the **minimum amount of bits**.

Storing words composed of only lower case letters, spaces and dots requires only 5 bits per characters instead of 8 bits (which saves 3 bits per character). The phase `hello my name is antoine. i wrote the bitreader library.` takes 56 bytes as an array of char but uses 35 bytes as 5 bits chunks.

Allow 6 bits per characters and you can also include capital letters and numbers: `Hello my name is Antoine. I wrote the BitReader library when I was 34 years old.` takes 80 bytes as an array of char but only requires 60 bytes as 6 bits chunks.




## Shorter structures definition ##

The library is particularly useful for dealing with [bit-field structures](http://en.cppreference.com/w/cpp/language/bit_field).

Consider the following:

```cpp
struct Person {
  char name[10];
  char birthDay;
  char birthMonth;
  short birthYear;
  bool maried;
  char children;
};
```

The `Person` structure requires 16 bytes in memory (10+1+1+2+1+1) or 128 bits. However, using bit-field structure, the `Person` structure can be defined as the following:

```cpp
struct Person2 {
 uint8_t name[10]; //keep 8 bits per letters for compatibility.
 union
 {
   struct //aligned on 8 bits
   {
     uint8_t birthDay : 5; //from 1 to 31
     uint8_t children : 3; //from 0 to 7. 
     uint8_t birthMonth : 4; //from 1 to 12
     uint8_t maried : 1; //true or false
     //uint8_t : 3; //birthYear (low)
     //uint8_t : 8; //birthYear (high)
   };
   struct //aligned on 32 bits
   {
     uint32_t padding1 :13; //padding for birthDay, children, birthMonth, maried
     uint32_t birthYear :12; //allows years from 0 to 4095.
     //uint16_t : 4; //unused
   };
 };
};
```

The `Person2` structure contains the same information as the `Person` structure but instead of using 128 bits in memory it only requires 105 bits (80+5+3+4+1+12).

A person could also be defined with masks:

```cpp
struct Person3 {
  uint8_t name[10];
  uint32_t buffer;
};
static const uint32_t MASK_BIRTHDAY = (1<<4 | 1<<3 | 1<<2 | 1<<1 | 1<<0);
static const uint32_t MASK_CHILDREN = (1<<7 | 1<<6 | 1<<5);
static const uint32_t MASK_BIRTHMONTH = (1<<11 | 1<<10 | 1<<9 | 1<<8);
static const uint32_t MASK_MARIED = (1<<12);
static const uint32_t MASK_BIRTHYEAR =  (1<<24 | 1<<23 | 1<<22 | 1<<21 | 1<<20 | 1<<19 | 1<<18 | 1<<17 | 1<<16 | 1<<15 | 1<<14 | 1<<13);
static const uint32_t MASK_PADDING = (1<<31 | 1<<30 | 1<<29 | 1<<28 | 1<<27 | 1<<26 | 1<<25);
```

In the last 2 scenarios (`Person2` and `Person3`), the BitReader library allows one to decode a `Person` structure from a binary buffer.




## Make data arrays much shorter ##

Consider an algorithm that plays a [Morse code](https://en.wikipedia.org/wiki/Morse_code). Morse code defines 3 symbols that can be played: dots, dashes and pauses.

For example, the following string `Hello my name is Antoine. I wrote the BitReader library when I was 34 years old.` (56 bytes) translate into Morse code as:

```
.... . .-.. .-.. --- / -- -.-- / -. .- -- . / .. ... / .- -. 
- --- .. -. . .-.-.- / .. / .-- .-. --- - . / - .... . / -... 
.. - .-. . .- -.. . .-. / .-.. .. -... .-. .- .-. -.-- / .-- 
.... . -. / .. / .-- .- ... / ...-- ....- / -.-- . .- .-. 
... / --- .-.. -.. .-.-.-
```

According to [this translator](http://morsecode.scphillips.com/translator.html). The whole code takes 267 bytes in memory. However, using 2 bits per code, the whole string can be encoded in a char buffer with only 534 bits (~67 bytes).

The same concept applies to all numeric array.




## Sample code ##

The following demo show how to use the library:

```cpp
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
```




## Other samples ##

See the library examples for more details:

* [Benchmark.ino](/src/BitReader/examples/Benchmark/Benchmark.ino)
* [ReadBits.ino](src/BitReader/examples/ReadBits/ReadBits.ino)
* [WriteBits.ino](src/BitReader/examples/WriteBits/WriteBits.ino)




# Building #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platforms #

win32Arduino has been tested with the following platform:

  * Linux x86/x64
  * Windows x86/x64




# Versioning #

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/BitReader/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/BitReader/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at http://github.com/end2endzone/BitReader
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef BITREADER_H
#define BITREADER_H

#include "Arduino.h"

#ifndef BITREADER_API
#define BITREADER_API
#endif

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif

//Bit optimization switches
#define USE_BITADDRESS_SETTER_GETTER
//#define USE_SINGLEBIT_MACROS
#define USE_BITADDRESS_READ_WRITE

struct BITREADER_API BitAddress
{
  unsigned int byte; //byte index
  unsigned char bit; //bit index

  BitAddress();
  BitAddress(const BitAddress & iBitAddress);
  void reset();
#ifdef USE_BITADDRESS_SETTER_GETTER
  void set(void * iBuffer, bool iBitValue);
  bool get(const void * iBuffer);
#endif
  void add(unsigned int iBits);
  void next();
  unsigned int toAbsBitOffset();

#ifdef WIN32
  std::string toString();
#endif

#ifdef USE_BITADDRESS_READ_WRITE
  void setBuffer(const void * iBuffer);
  void read(unsigned int iNumBits, void * iTargetBits);
  void write(const void * iSourceBits, unsigned int iNumBits);
  const unsigned char * mBuffer;
#endif

};

#ifndef USE_BITADDRESS_READ_WRITE

class BITREADER_API BitReader
{
public:
  BitReader();
  void setBuffer(const void * iBuffer);
  void read(unsigned int iNumBits, void * iTargetBits);
  unsigned char read8(unsigned int iNumBits);
  unsigned short read16(unsigned int iNumBits);
  const BitAddress & getAddress();
private:
  const unsigned char * mBuffer;
  BitAddress mAddr;
};

class BITREADER_API BitWriter
{
public:
  BitWriter();
  void setBuffer(void * iBuffer);
  void write(const void * iSourceBits, unsigned int iNumBits);
  void write8(unsigned char iSourceBits, unsigned int iNumBits);
  void write16(unsigned short iSourceBits, unsigned int iNumBits);
  void skip(unsigned int iNumBits);
  const BitAddress & getAddress();
private:
  unsigned char * mBuffer;
  BitAddress mAddr;
};

#endif //USE_BITADDRESS_READ_WRITE
#endif //BITREADER_H

#include <stdio.h>
#include <string.h>
#include <string>

#include "TestBitReader.h"
#include "Arduino.h"
#include "bitreader.h"
#include "rapidassist/gtesthelp.h"

using namespace testarduino;

namespace arduino { namespace test
{
  static const size_t PERSON_NAME_SIZE = 10;

  struct Person {
    char name[PERSON_NAME_SIZE];  //10*7 bits, 70 bits
    uint8_t birthDay;             //from 1 to 31, 5 bits
    uint8_t birthMonth;           //from 1 to 12, 4 bits
    uint16_t birthYear;           //from 1900 to 2149, 250 years, 8 bits
    bool maried;                  //1 bit
    uint8_t children;             //from 0 to 7, 3 bits
  };

  //--------------------------------------------------------------------------------------------------
  void TestBitReader::SetUp()
  {
    //disable function logging
    testarduino::setLogFile("");
  }
  //--------------------------------------------------------------------------------------------------
  void TestBitReader::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestBitReader, testDeserialize)
  {
    #ifndef USE_BITADDRESS_READ_WRITE
    BitReader bitreader;
    #else
    BitAddress bitreader;
    #endif

    static const uint8_t buffer[] = {0x41, 0x37, 0xfd, 0x9d, 0x76, 0x97, 0x01, 0x7e, 0x3f, 0x46, 0xa9, 0xca};
    static const uint8_t BUFFER_SIZE = sizeof(buffer)/sizeof(buffer[0]);

    //define an output Person
    Person p = {0};

    //setup BitWriter
    bitreader.setBuffer(buffer);

    //deserialize buffer into Person p
    for(int i=0; i<PERSON_NAME_SIZE; i++)
    {
      char c = 0;
      bitreader.read(7, &c);
      p.name[i] = c;
    }
    bitreader.read(5, &p.birthDay);
    bitreader.read(4, &p.birthMonth);
    bitreader.read(8, &p.birthYear);    p.birthYear += 1900;
    bitreader.read(1, &p.maried);
    bitreader.read(3, &p.children);

    //assert content properly extracted
    ASSERT_EQ(std::string("Antoine"), p.name);
    ASSERT_EQ(24, p.birthDay);
    ASSERT_EQ(8, p.birthMonth);
    ASSERT_EQ(1982, p.birthYear);
    ASSERT_EQ(true, p.maried);
    ASSERT_EQ(2, p.children);


  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestBitReader, testSerialize)
  {
    #ifndef USE_BITADDRESS_READ_WRITE
    BitWriter bitwriter;
    #else
    BitAddress bitwriter;
    #endif

    //fill Person information
    Person p;
    strcpy(p.name, "Antoine");
    p.birthDay = 24;
    p.birthMonth = 8;
    p.birthYear = 1982;
    p.maried = true;
    p.children = 2;

    //define an output buffer
    static const size_t MAX_BUFFER_SIZE = 100;
    uint8_t buffer[MAX_BUFFER_SIZE];

    //setup BitWriter
    bitwriter.setBuffer(buffer);

    //serialize Person p into buffer
    for(int i=0; i<PERSON_NAME_SIZE; i++)
    {
      char c = p.name[i];
      c = c & 0x7F;
      bitwriter.write(&c, 7);
    }
    bitwriter.write(&p.birthDay, 5);
    bitwriter.write(&p.birthMonth, 4);
    uint16_t tmpBirthYear = p.birthYear - 1900;
    bitwriter.write(&tmpBirthYear, 8);
    bitwriter.write(&p.maried, 1);
    bitwriter.write(&p.children, 3);

    //compute total size of serialized data
    uint32_t numBits = bitwriter.toAbsBitOffset();
    uint32_t numBytes = numBits/8;
    if (numBits%8)
      numBytes++;

    static const uint8_t EXPECTED_BUFFER[] = {0x41, 0x37, 0xfd, 0x9d, 0x76, 0x97, 0x01, 0x7e, 0x3f, 0x46, 0xa9, 0xca}; //expected serialized buffer content
    static const uint8_t EXPECTED_BUFFER_SIZE = sizeof(EXPECTED_BUFFER)/sizeof(EXPECTED_BUFFER[0]);

    //assert size of serialized buffer
    ASSERT_EQ(91, numBits);
    ASSERT_EQ(EXPECTED_BUFFER_SIZE, numBytes);

    //assert serialized buffer content
    for(int i=0; i<EXPECTED_BUFFER_SIZE; i++)
    {
      const uint8_t EXPECTED_CHARACTER = EXPECTED_BUFFER[i];
      const uint8_t actualCharacter = buffer[i];
      ASSERT_EQ(EXPECTED_CHARACTER, actualCharacter);
    }
  }
  //--------------------------------------------------------------------------------------------------
} // End namespace test
} // End namespace arduino

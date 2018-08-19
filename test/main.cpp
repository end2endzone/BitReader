#include <stdio.h>
#include <iostream>

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
  //define default values for xml output report
#ifdef NDEBUG
  ::testing::GTEST_FLAG(output) = "xml:bitreader_unittest.release.xml";
#else
  ::testing::GTEST_FLAG(output) = "xml:bitreader_unittest.debug.xml";
#endif

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  int wResult = RUN_ALL_TESTS(); //Find and run all tests
  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}

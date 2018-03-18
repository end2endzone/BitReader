include_directories(${CMAKE_SOURCE_DIR}/BitReader)
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories($ENV{WIN32ARDUINO_HOME}/src/win32Arduino)
link_directories($ENV{WIN32ARDUINO_HOME}/cmake/build/bin)

include_directories($ENV{GTEST_INCLUDE})
link_directories($ENV{GTEST_LIBRARY_DIR})

add_executable(bitreader_unittest
  gtesthelper.cpp
  gtesthelper.h
  main.cpp
  TestBitReader.cpp
  TestBitReader.h
)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(bitreader_unittest debug     bitreader.lib win32Arduino.lib gtest.lib)
target_link_libraries(bitreader_unittest optimized bitreader.lib win32Arduino.lib gtest.lib)

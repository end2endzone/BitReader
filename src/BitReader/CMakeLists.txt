include_directories($ENV{WIN32ARDUINO_HOME}/src/win32Arduino)
link_directories($ENV{WIN32ARDUINO_HOME}/cmake/build/bin)

add_library(bitreader STATIC 
  bitreader.cpp
  bitreader.h
)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

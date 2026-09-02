CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(desktop-window.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

DALI_ADD_SAMPLE(desktop-window.example
  desktop-window-example.cpp
  desktop-window.cpp
)

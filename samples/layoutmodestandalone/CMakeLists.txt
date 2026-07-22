CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(layoutmodestandalone.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(view-standalone.example view-standalone-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-standalone.example absolutelayout-standalone-example.cpp)
DALI_ADD_SAMPLE(flexlayout-standalone.example flexlayout-standalone-example.cpp)
DALI_ADD_SAMPLE(gridlayout-standalone.example gridlayout-standalone-example.cpp)
DALI_ADD_SAMPLE(stacklayout-standalone.example stacklayout-standalone-example.cpp)

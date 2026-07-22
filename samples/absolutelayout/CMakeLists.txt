CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(absolutelayout.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(absolutelayout.example absolutelayout-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-proportional.example absolutelayout-proportional-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-overlap.example absolutelayout-overlap-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-margin-padding.example absolutelayout-margin-padding-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-wrapcontent-proportional.example absolutelayout-wrapcontent-proportional-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-wrapcontent-position-proportional-label.example absolutelayout-wrapcontent-position-proportional-label-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-per-axis-proportional.example absolutelayout-per-axis-proportional-example.cpp)
DALI_ADD_SAMPLE(absolutelayout-layout-direction.example absolutelayout-layout-direction-example.cpp)

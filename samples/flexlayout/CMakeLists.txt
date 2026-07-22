CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(flexlayout.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(flexlayout.example flexlayout-example.cpp)
DALI_ADD_SAMPLE(flexlayout-justify-align.example flexlayout-justify-align-example.cpp)
DALI_ADD_SAMPLE(flexlayout-wrap-grow.example flexlayout-wrap-grow-example.cpp)
DALI_ADD_SAMPLE(flexlayout-margin-padding.example flexlayout-margin-padding-example.cpp)
DALI_ADD_SAMPLE(flexlayout-direction-reverse.example flexlayout-direction-reverse-example.cpp)
DALI_ADD_SAMPLE(flexlayout-justify-content.example flexlayout-justify-content-example.cpp)
DALI_ADD_SAMPLE(flexlayout-shrink.example flexlayout-shrink-example.cpp)
DALI_ADD_SAMPLE(flexlayout-wrapcontent-grow.example flexlayout-wrapcontent-grow-example.cpp)
DALI_ADD_SAMPLE(flexlayout-layout-direction.example flexlayout-layout-direction-example.cpp)

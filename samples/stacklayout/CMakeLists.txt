CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(stacklayout.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(stacklayout.example stacklayout-example.cpp)
DALI_ADD_SAMPLE(stacklayout-alignment.example stacklayout-alignment-example.cpp)
DALI_ADD_SAMPLE(stacklayout-margin-padding-nested.example stacklayout-margin-padding-nested-example.cpp)
DALI_ADD_SAMPLE(stacklayout-horizontal.example stacklayout-horizontal-example.cpp)
DALI_ADD_SAMPLE(stacklayout-wrapcontent-weight.example stacklayout-wrapcontent-weight-example.cpp)
DALI_ADD_SAMPLE(stacklayout-layout-direction.example stacklayout-layout-direction-example.cpp)

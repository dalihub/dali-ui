CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(gridlayout.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(gridlayout.example gridlayout-example.cpp)
DALI_ADD_SAMPLE(gridlayout-star-auto.example gridlayout-star-auto-example.cpp)
DALI_ADD_SAMPLE(gridlayout-span.example gridlayout-span-example.cpp)
DALI_ADD_SAMPLE(gridlayout-margin-padding.example gridlayout-margin-padding-example.cpp)
DALI_ADD_SAMPLE(gridlayout-auto.example gridlayout-auto-example.cpp)
DALI_ADD_SAMPLE(gridlayout-wrapcontent-star.example gridlayout-wrapcontent-star-example.cpp)
DALI_ADD_SAMPLE(gridlayout-layout-direction.example gridlayout-layout-direction-example.cpp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(viewlayout.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(view-matchparent.example view-matchparent-example.cpp)
DALI_ADD_SAMPLE(view-wrapcontent.example view-wrapcontent-example.cpp)
DALI_ADD_SAMPLE(view-margin.example view-margin-example.cpp)
DALI_ADD_SAMPLE(view-padding.example view-padding-example.cpp)
DALI_ADD_SAMPLE(view-wrapcontent-matchparent.example view-wrapcontent-matchparent-example.cpp)
DALI_ADD_SAMPLE(view-layout-direction.example view-layout-direction-example.cpp)

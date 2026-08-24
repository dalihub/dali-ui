CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(layout-invalidation-park.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(layout-invalidation-park.example layout-invalidation-park-example.cpp)

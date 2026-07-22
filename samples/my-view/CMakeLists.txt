CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(my-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(my-view.example
  my-view-example.cpp
  my-view.cpp
  my-view-impl.cpp
)

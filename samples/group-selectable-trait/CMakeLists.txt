CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(group-selectable-trait.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(group-selectable-trait.example group-selectable-trait-example.cpp)
DALI_ADD_SAMPLE(group-selectable-trait-group-name.example group-selectable-trait-group-name-example.cpp)

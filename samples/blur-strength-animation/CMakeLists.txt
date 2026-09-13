CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(blur-strength-animation.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(blur-strength-animation.example blur-strength-animation-example.cpp)
DALI_SET_SAMPLE_RESOURCES(
  blur-strength-animation.example
  image-view
  "${CMAKE_CURRENT_SOURCE_DIR}/../image-view/res"
)

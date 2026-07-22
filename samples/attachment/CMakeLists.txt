CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(attachment.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(attachment.example attachment-example.cpp)
DALI_ADD_SAMPLE(my-button.example my-button-example.cpp)
DALI_ADD_SAMPLE(label-button.example label-button.cpp label-button-example.cpp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(markdown.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(markdown-view.example
  markdown-view-example.cpp
  markdown-samples.cpp
)

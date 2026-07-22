CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(tv-extension.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(tv-extension.example
  flux-image-view-data.cpp
  flux-label-data.cpp
  flux-view-data.cpp
  tv-config.cpp
  tv-ext.cpp
  tv-extension-example.cpp
)

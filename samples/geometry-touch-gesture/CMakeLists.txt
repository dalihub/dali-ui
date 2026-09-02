CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(geometry-touch-gesture.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(
  geometry-touch-gesture.example
  geometry-touch-gesture-example.cpp
  sample-common.cpp
  scenario-factory.cpp
  manual-feed-scroller.cpp
  touch-routing-scenario.cpp
  gesture-feed-scenarios.cpp
  nested-pan-scenarios.cpp
)

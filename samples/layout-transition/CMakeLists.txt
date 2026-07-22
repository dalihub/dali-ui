CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(layout-transition.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(layout-transition-spec.example layout-transition-spec-example.cpp)
DALI_ADD_SAMPLE(layout-transition-animator.example layout-transition-animator-example.cpp)
DALI_ADD_SAMPLE(layout-transition-reorder.example layout-transition-reorder-example.cpp)
DALI_ADD_SAMPLE(layout-transition-subtree.example layout-transition-subtree-example.cpp)
DALI_ADD_SAMPLE(layout-transition-grid-reorder.example layout-transition-grid-reorder-example.cpp)

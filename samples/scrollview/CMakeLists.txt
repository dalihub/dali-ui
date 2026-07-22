CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(scrollview.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(scrollview.example scrollview-example.cpp)
DALI_ADD_SAMPLE(scrollview-interactive-children scrollview-interactive-children.cpp)
DALI_ADD_SAMPLE(scrollview-scrollbar-example scrollbar-example.cpp)
DALI_ADD_SAMPLE(scrollview-nofocus-keyscroll scrollview-nofocus-keyscroll.cpp)
DALI_ADD_SAMPLE(scrollview-farfocus-keyscroll scrollview-farfocus-keyscroll.cpp)

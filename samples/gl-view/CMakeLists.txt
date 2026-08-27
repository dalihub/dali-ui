CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(gl-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

# GlView hands raw GL to the application, so unlike every other sample this one
# calls GLES directly and has to link it itself. Skip the sample where GLES is
# not available rather than breaking the whole sample build.
IF(WIN32)
  MESSAGE(STATUS "Skipping gl-view sample: GLES is not wired up for the Windows samples")
  RETURN()
ENDIF()

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GLES_SAMPLE IMPORTED_TARGET glesv2)
IF(NOT GLES_SAMPLE_FOUND)
  MESSAGE(STATUS "Skipping gl-view sample: pkg-config could not find glesv2")
  RETURN()
ENDIF()

DALI_ADD_SAMPLE(gl-view.example gl-view-example.cpp)
TARGET_LINK_LIBRARIES(gl-view.example PRIVATE PkgConfig::GLES_SAMPLE)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(dali-ui-samples)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# DALi callbacks store member-function pointers in a fixed general form.
# Keep sample consumers ABI-compatible with the Windows DALi libraries.
IF(MSVC)
  ADD_COMPILE_OPTIONS(/vmg)
  ADD_COMPILE_DEFINITIONS(__restrict__=__restrict)
  SET(DALI_UI_SAMPLE_MSVC_OPTIONS_INHERITED TRUE)
ENDIF()

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/common.cmake)

IF(WIN32)
  SET(DALI_UI_SAMPLE_LIST "hello-world" CACHE STRING
      "Semicolon-separated sample directories to build on Windows")
ELSE()
  SET(DALI_UI_SAMPLE_LIST "" CACHE STRING
      "Semicolon-separated sample directories to build; empty builds all samples")
ENDIF()

# Auto-discover sample subdirectories containing CMakeLists.txt
FILE(GLOB CHILDREN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
FOREACH(CHILD ${CHILDREN})
  IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CHILD}
     AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CHILD}/CMakeLists.txt)
    IF(DALI_UI_SAMPLE_LIST AND NOT CHILD IN_LIST DALI_UI_SAMPLE_LIST)
      CONTINUE()
    ENDIF()
    MESSAGE(STATUS "Adding sample: ${CHILD}")
    ADD_SUBDIRECTORY(${CHILD})
  ENDIF()
ENDFOREACH()

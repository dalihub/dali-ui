CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(web-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

IF(WIN32)
  # Windows exercises the LWE web-engine backend
  # (dali2-web-engine-lwe-plugin-win.dll) through a dedicated sample.
  DALI_ADD_SAMPLE(web-view-win.example web-view-win-example.cpp)
  # Button-driven test harness: google.com vs the local GC-stress page.
  DALI_ADD_SAMPLE(web-view-win-test.example web-view-win-test-example.cpp)
ELSE()
  DALI_ADD_SAMPLE(web-view.example web-view-example.cpp)
  DALI_ADD_SAMPLE(web-view-lwe.example web-view-lwe-example.cpp)
  DALI_ADD_SAMPLE(web-browser.example web-browser-example.cpp)
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(widget-viewer.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# The widget engine plugin is currently available on Tizen platforms only.
IF(TIZEN)
  IF(NOT COMMAND DALI_ADD_SAMPLE)
    INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
  ENDIF()

  FIND_PACKAGE(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(BUNDLE       REQUIRED bundle)
  PKG_CHECK_MODULES(DLOG         REQUIRED dlog)
  PKG_CHECK_MODULES(MESSAGE_PORT REQUIRED message-port)

  DALI_ADD_SAMPLE(widget-viewer.example widget-viewer-example.cpp)
  # Tizen launchpad loads the executable with dlopen() and resolves main.
  TARGET_LINK_OPTIONS(widget-viewer.example PRIVATE -pie -Wl,--export-dynamic)
  TARGET_LINK_LIBRARIES(widget-viewer.example PRIVATE
    ${BUNDLE_LIBRARIES}
    ${DLOG_LIBRARIES}
    ${MESSAGE_PORT_LIBRARIES}
  )
  TARGET_INCLUDE_DIRECTORIES(widget-viewer.example PRIVATE
    ${BUNDLE_INCLUDE_DIRS}
    ${DLOG_INCLUDE_DIRS}
    ${MESSAGE_PORT_INCLUDE_DIRS}
  )
  TARGET_COMPILE_OPTIONS(widget-viewer.example PRIVATE
    ${BUNDLE_CFLAGS_OTHER}
    ${DLOG_CFLAGS_OTHER}
    ${MESSAGE_PORT_CFLAGS_OTHER}
  )
ENDIF()

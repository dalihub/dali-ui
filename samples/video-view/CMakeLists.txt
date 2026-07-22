CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(video-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

# This sample directly uses the Tizen MMPlayer and ESPlayer APIs.
IF(WIN32)
  MESSAGE(STATUS "video-view sample: Tizen media APIs are unavailable on Windows, skipping")
  RETURN()
ENDIF()

PKG_CHECK_MODULES(CAPI_MEDIA_PLAYER IMPORTED_TARGET capi-media-player)
IF(NOT CAPI_MEDIA_PLAYER_FOUND)
  MESSAGE(STATUS "video-view sample: capi-media-player not found, skipping")
  RETURN()
ENDIF()
PKG_CHECK_MODULES(ESPLUSPLAYER REQUIRED IMPORTED_TARGET esplusplayer)

DALI_ADD_SAMPLE(video-view.example video-view-example.cpp)
TARGET_LINK_LIBRARIES(video-view.example PRIVATE
  PkgConfig::CAPI_MEDIA_PLAYER
  PkgConfig::ESPLUSPLAYER
)

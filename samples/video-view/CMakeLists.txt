CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(video-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# Find required packages
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
PKG_CHECK_MODULES(DALI_UI_FOUNDATION REQUIRED dali2-ui-foundation)
PKG_CHECK_MODULES(DALI_UI_COMPONENTS REQUIRED dali2-ui-components)

# The sample drives a native CAPI player_h (capi-media-player). This is only
# available on a Tizen target, so the sample is skipped on platforms (e.g. the
# desktop build) where the package is not present.
PKG_CHECK_MODULES(CAPI_MEDIA_PLAYER capi-media-player)
IF(NOT CAPI_MEDIA_PLAYER_FOUND)
  MESSAGE(STATUS "video-view sample: capi-media-player not found, skipping this sample")
  RETURN()
ENDIF()

# The sample drives an ESPlayer session directly in its ESPlayer modes, so it
# needs the ESPlayer CAPI headers regardless of which mode is run.
PKG_CHECK_MODULES(ESPLUSPLAYER REQUIRED esplusplayer)

# Add executable
SET(EXE_NAME video-view.example)
ADD_EXECUTABLE(${EXE_NAME} video-view-example.cpp)

# Set output directory
SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION)

# Build Options
SET(DALI_UI_TEST_APP_CFLAGS -fvisibility=hidden -DHIDE_DALI_INTERNALS)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  MESSAGE(STATUS "CMAKE_BUILD_TYPE: " Debug)
  SET(DALI_UI_TEST_APP_CFLAGS ${DALI_UI_TEST_APP_CFLAGS} -g -O0 -DDEBUG_ENABLED)
ELSE()
  MESSAGE(STATUS "CMAKE_BUILD_TYPE: " Release)
  SET(DALI_UI_TEST_APP_CFLAGS ${DALI_UI_TEST_APP_CFLAGS} -Os)
ENDIF()

# Add -rdynamic to export main symbol, so dali-loader could run the main.
SET(DALI_UI_TEST_APP_CFLAGS ${DALI_UI_TEST_APP_CFLAGS} -Werror -Wall -fPIE -rdynamic)

# Remove some warnings that are not useful for our test apps.
IF( WIN32 )
  # TODO: Add Windows-specific compiler flags if needed.
ELSE()
  INCLUDE(CheckCXXCompilerFlag)

  CHECK_CXX_COMPILER_FLAG( -Wno-unused-const-variable HAVE_NO_UNUSED_CONST_VARIABLE )
  IF( HAVE_NO_UNUSED_CONST_VARIABLE )
    SET(DALI_UI_TEST_APP_CFLAGS ${DALI_UI_TEST_APP_CFLAGS} -Wno-unused-const-variable)
  ENDIF()

  CHECK_CXX_COMPILER_FLAG( -Wno-unused-command-line-argument HAVE_NO_UNUSED_COMMAND_LINE_ARGUMENT )
  IF( HAVE_NO_UNUSED_COMMAND_LINE_ARGUMENT )
    SET(DALI_UI_TEST_APP_CFLAGS ${DALI_UI_TEST_APP_CFLAGS} -Wno-unused-command-line-argument)
  ENDIF()
ENDIF()

# Link libraries
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${DALICORE_LIBRARIES}
  ${DALIADAPTOR_LIBRARIES}
  ${DALI_UI_FOUNDATION_LIBRARIES}
  ${DALI_UI_COMPONENTS_LIBRARIES}
  ${CAPI_MEDIA_PLAYER_LIBRARIES}
  ${ESPLUSPLAYER_LIBRARIES}
)

# Add link directories
TARGET_LINK_DIRECTORIES(${EXE_NAME} PRIVATE
  $ENV{DESKTOP_PREFIX}/lib
)

# Include directories
TARGET_INCLUDE_DIRECTORIES(${EXE_NAME} PRIVATE
  ${DALICORE_INCLUDE_DIRS}
  ${DALIADAPTOR_INCLUDE_DIRS}
  ${DALI_UI_FOUNDATION_INCLUDE_DIRS}
  ${DALI_UI_COMPONENTS_INCLUDE_DIRS}
  ${CAPI_MEDIA_PLAYER_INCLUDE_DIRS}
  ${ESPLUSPLAYER_INCLUDE_DIRS}
)

# Compiler flags
TARGET_COMPILE_OPTIONS(${EXE_NAME} PRIVATE
  ${DALICORE_CFLAGS_OTHER}
  ${DALIADAPTOR_CFLAGS_OTHER}
  ${DALI_UI_FOUNDATION_CFLAGS_OTHER}
  ${DALI_UI_COMPONENTS_CFLAGS_OTHER}
  ${CAPI_MEDIA_PLAYER_CFLAGS_OTHER}
  ${DALI_UI_TEST_APP_CFLAGS}
)

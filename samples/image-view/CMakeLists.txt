CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(image-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# Find required packages
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
PKG_CHECK_MODULES(DALI_UI_FOUNDATION REQUIRED dali2-ui-foundation)

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

# Helper macro to add an image-view sample executable
macro(add_image_view_sample EXE_NAME SOURCE_FILE)
  ADD_EXECUTABLE(${EXE_NAME} ${SOURCE_FILE})

  TARGET_COMPILE_DEFINITIONS(${EXE_NAME} PRIVATE
    RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/"
  )

  SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

  INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION)

  TARGET_LINK_LIBRARIES(${EXE_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    ${DALI_UI_FOUNDATION_LIBRARIES}
  )

  TARGET_LINK_DIRECTORIES(${EXE_NAME} PRIVATE
    $ENV{DESKTOP_PREFIX}/lib
  )

  TARGET_INCLUDE_DIRECTORIES(${EXE_NAME} PRIVATE
    ${DALICORE_INCLUDE_DIRS}
    ${DALIADAPTOR_INCLUDE_DIRS}
    ${DALI_UI_FOUNDATION_INCLUDE_DIRS}
  )

  TARGET_COMPILE_OPTIONS(${EXE_NAME} PRIVATE
    ${DALICORE_CFLAGS_OTHER}
    ${DALIADAPTOR_CFLAGS_OTHER}
    ${DALI_UI_FOUNDATION_CFLAGS_OTHER}
    ${DALI_UI_TEST_APP_CFLAGS}
  )
endmacro()

# Copy resources when building
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_image_view_sample(image-view.example image-view-example.cpp)
add_image_view_sample(image-view-with-layout.example image-view-with-layout-example.cpp)
add_image_view_sample(image-fitting-mode.example image-fitting-mode-example.cpp)
add_image_view_sample(image-placeholder.example image-placeholder-example.cpp)
add_image_view_sample(image-pixel-area.example image-pixel-area-example.cpp)
add_image_view_sample(image-alphamask.example image-alpha-mask-example.cpp)
add_image_view_sample(image-view-npatch.example image-view-npatch-example.cpp)
add_image_view_sample(image-color.example image-color-example.cpp)
add_image_view_sample(image-corner-radius.example image-corner-radius-example.cpp)
add_image_view_sample(image-sampling-desired-size.example image-sampling-desired-size-example.cpp)
add_image_view_sample(image-view-sizing.example image-view-sizing-example.cpp)
add_image_view_sample(image-loading-policy.example image-loading-policy-example.cpp)
add_image_view_sample(image-view-svg.example image-view-svg-example.cpp)

add_image_view_sample(animated-image-view.example animated-image-view-example.cpp)
add_image_view_sample(lottie-animation-view.example lottie-animation-view-example.cpp)

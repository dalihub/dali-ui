CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(image-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

# Find required packages
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
PKG_CHECK_MODULES(DALI_UI_FOUNDATION REQUIRED dali2-ui-foundation)

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
  )
endmacro()

# Copy resources when building
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_image_view_sample(image-view.example image-view-example.cpp)
add_image_view_sample(image-view-with-layout.example image-view-with-layout-example.cpp)
add_image_view_sample(image-view-fitting-mode.example image-view-fitting-mode-example.cpp)
add_image_view_sample(image-view-placeholder.example image-view-placeholder-example.cpp)
add_image_view_sample(image-view-pixel-area.example image-view-pixel-area-example.cpp)
add_image_view_sample(image-view-alpha-mask.example image-view-alpha-mask-example.cpp)
add_image_view_sample(image-view-npatch.example image-view-npatch-example.cpp)
add_image_view_sample(image-view-image-color.example image-view-image-color-example.cpp)
add_image_view_sample(image-view-sampling-desired-size.example image-view-sampling-desired-size-example.cpp)
add_image_view_sample(image-view-sizing.example image-view-sizing-example.cpp)
add_image_view_sample(image-view-loading-policy.example image-view-loading-policy-example.cpp)

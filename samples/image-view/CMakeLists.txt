CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(image-view.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

SET(IMAGE_VIEW_SAMPLES
  image-view.example
  image-view-with-layout.example
  image-view-with-image-url.example
  image-fitting-mode.example
  image-placeholder.example
  image-pixel-area.example
  image-alphamask.example
  image-view-npatch.example
  image-color.example
  image-corner-radius.example
  image-sampling-desired-size.example
  image-view-sizing.example
  image-loading-policy.example
  image-cache-measurement.example
  image-view-svg.example
  animated-image-view.example
  lottie-animation-view.example
)

SET(IMAGE_VIEW_SOURCES
  image-view-example.cpp
  image-view-with-layout-example.cpp
  image-view-with-image-url-example.cpp
  image-fitting-mode-example.cpp
  image-placeholder-example.cpp
  image-pixel-area-example.cpp
  image-alpha-mask-example.cpp
  image-view-npatch-example.cpp
  image-color-example.cpp
  image-corner-radius-example.cpp
  image-sampling-desired-size-example.cpp
  image-view-sizing-example.cpp
  image-loading-policy-example.cpp
  image-cache-measurement-example.cpp
  image-view-svg-example.cpp
  animated-image-view-example.cpp
  lottie-animation-view-example.cpp
)

LIST(LENGTH IMAGE_VIEW_SAMPLES IMAGE_VIEW_SAMPLE_COUNT)
MATH(EXPR IMAGE_VIEW_SAMPLE_LAST "${IMAGE_VIEW_SAMPLE_COUNT} - 1")
FOREACH(INDEX RANGE ${IMAGE_VIEW_SAMPLE_LAST})
  LIST(GET IMAGE_VIEW_SAMPLES ${INDEX} TARGET_NAME)
  LIST(GET IMAGE_VIEW_SOURCES ${INDEX} SOURCE_FILE)

  IF(IMAGE_VIEW_SAMPLE_LIST AND NOT TARGET_NAME IN_LIST IMAGE_VIEW_SAMPLE_LIST)
    CONTINUE()
  ENDIF()

  DALI_ADD_SAMPLE(${TARGET_NAME} ${SOURCE_FILE})
ENDFOREACH()

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(lottie-measure-reload-probe.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(lottie-measure-reload-probe.example
  lottie-measure-reload-probe-example.cpp
  measure-reload-probe-view.cpp
  measure-reload-probe-view-impl.cpp
)

# Reuse the image-view resource directory rather than duplicating jolly_walker.json.
# The install rule is registered once per RESOURCE_NAME, so passing "image-view" is safe
# whichever sample is configured first. Same shape as samples/background-blur-dither.
DALI_SET_SAMPLE_RESOURCES(
  lottie-measure-reload-probe.example
  image-view
  "${CMAKE_CURRENT_SOURCE_DIR}/../image-view/res"
)

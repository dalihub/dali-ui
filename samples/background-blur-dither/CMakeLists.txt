CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(background-blur-dither.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

DALI_ADD_SAMPLE(background-blur-dither.example background-blur-dither-example.cpp)

TARGET_COMPILE_DEFINITIONS(background-blur-dither.example PRIVATE
  RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../image-view/res/"
)

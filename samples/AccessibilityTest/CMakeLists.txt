CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(accessibility-test.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

DALI_ADD_SAMPLE(accessibility-test.example accessibility-test-example.cpp)

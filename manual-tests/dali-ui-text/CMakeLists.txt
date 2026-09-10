CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
PROJECT(manual-test-dali-ui-text)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(TEXT_DALI REQUIRED IMPORTED_TARGET
  dali2-core dali2-adaptor dali2-ui-foundation)
OPTION(TEXT_PIXEL_CHECKS "Enable PNG-based automatic pixel validation (requires libpng)" ON)
IF(TEXT_PIXEL_CHECKS)
  FIND_PACKAGE(PNG 1.6 REQUIRED)
  INCLUDE(CheckCXXSourceCompiles)
  INCLUDE(CMakePushCheckState)
  CMAKE_PUSH_CHECK_STATE(RESET)
  SET(CMAKE_REQUIRED_LIBRARIES PNG::PNG)
  CHECK_CXX_SOURCE_COMPILES("#include <png.h>
    #if !defined(PNG_SIMPLIFIED_READ_SUPPORTED) || !defined(PNG_SIMPLIFIED_WRITE_STDIO_SUPPORTED)
    #error libpng requires simplified PNG file read/write support
    #endif
    int main() { png_image p{}; p.version=PNG_IMAGE_VERSION;
      png_image_begin_read_from_file(&p, \"\");
      png_image_finish_read(&p, nullptr, nullptr, 0, nullptr);
      png_image_write_to_file(&p, \"\", 0, nullptr, 0, nullptr); png_image_free(&p); }
    " TEXT_PNG_FILE_API_AVAILABLE)
  CMAKE_POP_CHECK_STATE()
  IF(NOT TEXT_PNG_FILE_API_AVAILABLE)
    MESSAGE(FATAL_ERROR "libpng lacks the required PNG file API. Use a full libpng 1.6 build, or -DTEXT_PIXEL_CHECKS=OFF for manual interaction without pixel validation.")
  ENDIF()
ENDIF()

ADD_LIBRARY(text-image-check STATIC common/image-check.cpp)
SET_TARGET_PROPERTIES(text-image-check PROPERTIES POSITION_INDEPENDENT_CODE ON)
TARGET_INCLUDE_DIRECTORIES(text-image-check PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
TARGET_COMPILE_DEFINITIONS(text-image-check PUBLIC TEXT_PIXEL_CHECKS=$<BOOL:${TEXT_PIXEL_CHECKS}>)
IF(TEXT_PIXEL_CHECKS)
  TARGET_LINK_LIBRARIES(text-image-check PUBLIC PNG::PNG)
ENDIF()
TARGET_COMPILE_OPTIONS(text-image-check PRIVATE -Wall -Wextra -Werror)

FILE(GLOB TC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tc/*.cpp")
# Check every build, including incremental edits after CMake configuration.
ADD_CUSTOM_TARGET(text-public-api-check
  COMMAND ${CMAKE_COMMAND} -DTEXT_APP_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/check-public-api.cmake
  VERBATIM)
ADD_LIBRARY(text-run-report STATIC common/run-report.cpp)
TARGET_INCLUDE_DIRECTORIES(text-run-report PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
TARGET_COMPILE_OPTIONS(text-run-report PRIVATE -Wall -Wextra -Werror)
ADD_EXECUTABLE(manual-test-dali-ui-text main.cpp common/image-capture.cpp ${TC_SOURCES})
ADD_DEPENDENCIES(manual-test-dali-ui-text text-public-api-check)
TARGET_INCLUDE_DIRECTORIES(manual-test-dali-ui-text PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
TARGET_LINK_LIBRARIES(manual-test-dali-ui-text PRIVATE PkgConfig::TEXT_DALI text-image-check text-run-report)
TARGET_COMPILE_OPTIONS(manual-test-dali-ui-text PRIVATE
  -Wall -Wextra -Werror -fPIE -fvisibility=hidden)
TARGET_COMPILE_DEFINITIONS(manual-test-dali-ui-text PRIVATE HIDE_DALI_INTERNALS)
SET_PROPERTY(TARGET manual-test-dali-ui-text APPEND_STRING PROPERTY LINK_FLAGS " -rdynamic")
IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET_TARGET_PROPERTIES(manual-test-dali-ui-text PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
ENDIF()

IF(TIZEN)
  SET(TEXT_TEST_RESOURCE_DIR "${CMAKE_INSTALL_PREFIX}/res/")
ELSE()
  SET(TEXT_TEST_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/")
ENDIF()
TARGET_COMPILE_DEFINITIONS(manual-test-dali-ui-text PRIVATE
  RESOURCES_DIR="${TEXT_TEST_RESOURCE_DIR}")
INSTALL(TARGETS manual-test-dali-ui-text RUNTIME DESTINATION bin)
INSTALL(DIRECTORY res/ DESTINATION res)

OPTION(TEXT_IMAGE_CHECK_SELF_TESTS "Build renderer-independent image checker and report tests" OFF)
IF(TEXT_IMAGE_CHECK_SELF_TESTS)
  ENABLE_TESTING()
  ADD_EXECUTABLE(text-image-check-tests tests/image-check-test.cpp)
  TARGET_LINK_LIBRARIES(text-image-check-tests PRIVATE text-image-check)
  TARGET_COMPILE_OPTIONS(text-image-check-tests PRIVATE -Wall -Wextra -Werror)
  ADD_TEST(NAME text-image-check COMMAND text-image-check-tests)
  ADD_EXECUTABLE(text-run-report-tests tests/run-report-test.cpp)
  TARGET_LINK_LIBRARIES(text-run-report-tests PRIVATE text-run-report)
  TARGET_COMPILE_OPTIONS(text-run-report-tests PRIVATE -Wall -Wextra -Werror)
  ADD_TEST(NAME text-run-report COMMAND text-run-report-tests)
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(dali-ui-common-tests LANGUAGES CXX)

INCLUDE(GNUInstallDirs)

IF(NOT WIN32)
  MESSAGE(FATAL_ERROR "The DALi UI common Windows tests must be built for Windows.")
ENDIF()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

FIND_PACKAGE(dali-windows-dependencies CONFIG REQUIRED)
FIND_PACKAGE(dali2-core CONFIG REQUIRED)
FIND_PACKAGE(dali2-adaptor CONFIG REQUIRED)
FIND_PACKAGE(Intl REQUIRED)

GET_TARGET_PROPERTY(DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIRS
  dali-windows-dependencies::dali-windows-dependencies
  INTERFACE_INCLUDE_DIRECTORIES)
LIST(GET DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIRS 0 DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR)

SET(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
SET(FOUNDATION_DIR "${ROOT_DIR}/dali-ui-foundation")
SET(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
SET(INTERNAL_TEST_DIR "${TEST_DIR}/dali-ui-foundation-internal")
SET(TEST_UTIL_DIR "${TEST_DIR}/dali-test-suite-utils")

FILE(GLOB_RECURSE FOUNDATION_SOURCES CONFIGURE_DEPENDS "${FOUNDATION_DIR}/*.cpp")
LIST(FILTER FOUNDATION_SOURCES EXCLUDE REGEX "[/\\]shader-generator[/\\]shader-generator\\.cpp$")
LIST(FILTER FOUNDATION_SOURCES EXCLUDE REGEX "[/\\]drag-drop-detector[/\\]drag-and-drop-detector-impl\\.cpp$")
LIST(FILTER FOUNDATION_SOURCES EXCLUDE REGEX "[/\\]drag-and-drop[/\\]drag-and-drop-detector\\.cpp$")

ADD_LIBRARY(dali-ui-foundation-test-objects OBJECT ${FOUNDATION_SOURCES})
TARGET_INCLUDE_DIRECTORIES(dali-ui-foundation-test-objects PRIVATE
  "${ROOT_DIR}"
  "${FOUNDATION_DIR}/third-party"
  "${FOUNDATION_DIR}/internal"
  "${FOUNDATION_DIR}/integration-api"
)
TARGET_COMPILE_DEFINITIONS(dali-ui-foundation-test-objects PRIVATE
  BUILDING_DALI_ADAPTOR
  BUILDING_DALI_UI_FOUNDATION
  $<$<CONFIG:Debug>:DEBUG_ENABLED>
  DALI_UI_HAS_GETTEXT
  EGLAPI=
  GL_API=
  GL_APICALL=
  GL_GLEXT_PROTOTYPES
  NO_THORVG
  NOMINMAX
  WIN32_LEAN_AND_MEAN
  __restrict__=__restrict
  DALI_IMAGE_DIR="${CMAKE_INSTALL_PREFIX}/share/dali/ui/images/"
  DALI_DATA_READ_ONLY_DIR="${CMAKE_INSTALL_PREFIX}/share/dali"
  DALI_SOUND_DIR="${CMAKE_INSTALL_PREFIX}/share/dali/toolkit/sounds/"
  DALI_STYLE_DIR="${CMAKE_INSTALL_PREFIX}/share/dali/toolkit/styles/"
  DALI_STYLE_IMAGE_DIR="${CMAKE_INSTALL_PREFIX}/share/dali/toolkit/styles/images/"
)
TARGET_COMPILE_OPTIONS(dali-ui-foundation-test-objects PRIVATE
  /FI "${DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR}/dali-windows-dependencies.h"
  /vmg /utf-8 /W3 /wd4251 /wd4275
)
TARGET_LINK_LIBRARIES(dali-ui-foundation-test-objects PRIVATE
  dali-windows-dependencies::dali-windows-dependencies
  dali2-adaptor::dali2-adaptor
  dali2-core::dali2-core
)

SET(DALI_TEST_SOURCE_LIST_ONLY ON)
INCLUDE("${INTERNAL_TEST_DIR}/CMakeLists.txt")
UNSET(DALI_TEST_SOURCE_LIST_ONLY)

SET(INTERNAL_TC_NAMES ${TC_SOURCE_LIST})
LIST(TRANSFORM INTERNAL_TC_NAMES PREPEND "${INTERNAL_TEST_DIR}/")
SET(INTERNAL_RUN_SOURCES ${TC_SOURCES})
LIST(TRANSFORM INTERNAL_RUN_SOURCES PREPEND "${INTERNAL_TEST_DIR}/")
LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "${INTERNAL_TEST_DIR}/utc-Dali-WindowsWarningCoverage-internal.cpp")
LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "${INTERNAL_TEST_DIR}/utc-Dali-InteractiveView-internal.cpp")
LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "${INTERNAL_TEST_DIR}/../dali-test-suite-utils/test-harness.cpp")



FUNCTION(GENERATE_TEST_HEADER OUTPUT_HEADER ARRAY_NAME)
  SET(TEST_DECLARATIONS "")
  SET(TEST_ENTRIES "")
  STRING(ASCII 34 QUOTE)
  FOREACH(TEST_SOURCE IN LISTS ARGN)
    FILE(READ "${TEST_SOURCE}" TEST_CONTENT)
    STRING(REGEX MATCH "void[ \t]+[A-Za-z0-9_]+_startup\\(void\\)" STARTUP_MATCH "${TEST_CONTENT}")
    STRING(REGEX MATCH "void[ \t]+[A-Za-z0-9_]+_cleanup\\(void\\)" CLEANUP_MATCH "${TEST_CONTENT}")
    STRING(REGEX REPLACE "^void[ \t]+|\\(void\\)$" "" STARTUP_NAME "${STARTUP_MATCH}")
    STRING(REGEX REPLACE "^void[ \t]+|\\(void\\)$" "" CLEANUP_NAME "${CLEANUP_MATCH}")
    IF(STARTUP_NAME STREQUAL "")
      SET(STARTUP_NAME "nullptr")
    ELSE()
      STRING(APPEND TEST_DECLARATIONS "extern void ${STARTUP_NAME}(void);\n")
    ENDIF()
    IF(CLEANUP_NAME STREQUAL "")
      SET(CLEANUP_NAME "nullptr")
    ELSE()
      STRING(APPEND TEST_DECLARATIONS "extern void ${CLEANUP_NAME}(void);\n")
    ENDIF()
    STRING(REGEX MATCHALL "int[ \t]+Utc[A-Za-z0-9_]+\\(void\\)" TEST_MATCHES "${TEST_CONTENT}")
    FOREACH(TEST_MATCH IN LISTS TEST_MATCHES)
      STRING(REGEX REPLACE "^int[ \t]+|\\(void\\)$" "" TEST_NAME "${TEST_MATCH}")
      STRING(APPEND TEST_DECLARATIONS "extern int ${TEST_NAME}(void);\n")
      STRING(APPEND TEST_ENTRIES "  {${QUOTE}${TEST_NAME}${QUOTE}, ${TEST_NAME}, ${STARTUP_NAME}, ${CLEANUP_NAME}},\n")
    ENDFOREACH()
  ENDFOREACH()
  FILE(WRITE "${OUTPUT_HEADER}"
    "#pragma once\n#include <testcase.h>\n\n${TEST_DECLARATIONS}\n"
    "inline testcase ${ARRAY_NAME}[] = {\n${TEST_ENTRIES}  {nullptr, nullptr, nullptr, nullptr}\n};\n")
ENDFUNCTION()

SET(INTERNAL_HEADER "${CMAKE_CURRENT_BINARY_DIR}/dali-ui-foundation-internal-testcases.h")
GENERATE_TEST_HEADER("${INTERNAL_HEADER}" gDaliUiFoundationInternalTests ${INTERNAL_TC_NAMES})

ADD_EXECUTABLE(tct-dali-ui-foundation-internal-core
  src/common-test-main.cpp
  src/adaptor-static-constants.cpp
  src/windows-unsupported-tests.cpp
  ${INTERNAL_RUN_SOURCES}
  $<TARGET_OBJECTS:dali-ui-foundation-test-objects>
)
TARGET_INCLUDE_DIRECTORIES(tct-dali-ui-foundation-internal-core PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${TEST_DIR}/common"
  "${TEST_DIR}/dali-ui-foundation"
  "${TEST_DIR}/dali-ui-components"
  "${TEST_UTIL_DIR}"
  "${TEST_UTIL_DIR}/dali-ui"
  "${ROOT_DIR}"
  "${ROOT_DIR}/../dali-adaptor/dali"
)
TARGET_COMPILE_DEFINITIONS(tct-dali-ui-foundation-internal-core PRIVATE
  ADDON_LIBS_PATH="${CMAKE_CURRENT_BINARY_DIR}"
  BUILDING_DALI_ADAPTOR
  DALI_TEST_ARRAY=gDaliUiFoundationInternalTests
  DALI_TEST_HEADER="dali-ui-foundation-internal-testcases.h"
  DALI_TEST_SUITE="foundation-internal"
  $<$<CONFIG:Debug>:DEBUG_ENABLED>
  EGLAPI=
  GL_API=
  GL_APICALL=
  GL_GLEXT_PROTOTYPES
  NOMINMAX
  WIN32_LEAN_AND_MEAN
  _USE_MATH_DEFINES
  __restrict__=__restrict
)
TARGET_COMPILE_OPTIONS(tct-dali-ui-foundation-internal-core PRIVATE
  /FI "${DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR}/dali-windows-dependencies.h"
  /vmg /utf-8 /W3 /wd4251 /wd4275
)
TARGET_LINK_LIBRARIES(tct-dali-ui-foundation-internal-core PRIVATE
  dali-windows-dependencies::dali-windows-dependencies
  dali2-adaptor::dali2-adaptor
  dali2-core::dali2-core
  Intl::Intl
  dbghelp
)

INSTALL(TARGETS tct-dali-ui-foundation-internal-core RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/addons.txt" "")

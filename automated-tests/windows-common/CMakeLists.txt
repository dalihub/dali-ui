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

# MSVC includes member access levels in decorated names. Tests that rewrite
# private/protected as public therefore cannot link to separately compiled
# foundation implementation objects and are reported as unsupported on Windows.
SET(WINDOWS_UNSUPPORTED_INTERNAL_TESTS
  utc-Dali-AnimatedImageReleasePolicy-internal.cpp
  utc-Dali-ArcWireframeVisual-internal.cpp
  utc-Dali-FeedbackStyle-internal.cpp
  utc-Dali-ImageCache-internal.cpp
  utc-Dali-InteractiveView-internal.cpp
  utc-Dali-MeshVisual-internal.cpp
  utc-Dali-ObjLoader-internal.cpp
  utc-Dali-PageScrollView-internal.cpp
  utc-Dali-PrimitiveVisual-internal.cpp
  utc-Dali-WindowsWarningCoverage-internal.cpp
)

SET(WINDOWS_UNSUPPORTED_TEST_NAMES "")
SET(WINDOWS_UNSUPPORTED_TEST_STUBS "")
FOREACH(TEST_SOURCE IN LISTS WINDOWS_UNSUPPORTED_INTERNAL_TESTS)
  SET(TEST_PATH "${INTERNAL_TEST_DIR}/${TEST_SOURCE}")
  LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "${TEST_PATH}")
  FILE(READ "${TEST_PATH}" TEST_CONTENT)

  STRING(REGEX MATCH "void[ \t]+[A-Za-z0-9_]+_startup\\(void\\)" STARTUP_MATCH "${TEST_CONTENT}")
  IF(STARTUP_MATCH)
    STRING(REGEX REPLACE "^void[ \t]+|\\(void\\)$" "" STARTUP_NAME "${STARTUP_MATCH}")
    STRING(APPEND WINDOWS_UNSUPPORTED_TEST_STUBS "void ${STARTUP_NAME}(void) {}\n")
  ENDIF()

  STRING(REGEX MATCH "void[ \t]+[A-Za-z0-9_]+_cleanup\\(void\\)" CLEANUP_MATCH "${TEST_CONTENT}")
  IF(CLEANUP_MATCH)
    STRING(REGEX REPLACE "^void[ \t]+|\\(void\\)$" "" CLEANUP_NAME "${CLEANUP_MATCH}")
    STRING(APPEND WINDOWS_UNSUPPORTED_TEST_STUBS "void ${CLEANUP_NAME}(void) {}\n")
  ENDIF()

  STRING(REGEX MATCHALL "int[ \t]+Utc[A-Za-z0-9_]+\\(void\\)" TEST_MATCHES "${TEST_CONTENT}")
  FOREACH(TEST_MATCH IN LISTS TEST_MATCHES)
    STRING(REGEX REPLACE "^int[ \t]+|\\(void\\)$" "" TEST_NAME "${TEST_MATCH}")
    STRING(APPEND WINDOWS_UNSUPPORTED_TEST_NAMES "  \"${TEST_NAME}\",\n")
    STRING(APPEND WINDOWS_UNSUPPORTED_TEST_STUBS "int ${TEST_NAME}(void) { return 77; }\n")
  ENDFOREACH()
  STRING(APPEND WINDOWS_UNSUPPORTED_TEST_STUBS "\n")
ENDFOREACH()

FOREACH(TEST_SOURCE IN LISTS INTERNAL_TC_NAMES)
  FILE(READ "${TEST_SOURCE}" TEST_CONTENT)
  STRING(REGEX MATCH "#[ \t]*define[ \t]+(private|protected)[ \t]+public" ACCESS_OVERRIDE "${TEST_CONTENT}")
  IF(ACCESS_OVERRIDE)
    GET_FILENAME_COMPONENT(TEST_FILENAME "${TEST_SOURCE}" NAME)
    LIST(FIND WINDOWS_UNSUPPORTED_INTERNAL_TESTS "${TEST_FILENAME}" UNSUPPORTED_INDEX)
    IF(UNSUPPORTED_INDEX EQUAL -1)
      MESSAGE(FATAL_ERROR "${TEST_FILENAME} rewrites C++ access levels and must be listed in WINDOWS_UNSUPPORTED_INTERNAL_TESTS for MSVC.")
    ENDIF()
  ENDIF()
ENDFOREACH()

SET(WINDOWS_UNSUPPORTED_TEST_NAMES_HEADER "${CMAKE_CURRENT_BINARY_DIR}/windows-private-access-unsupported-tests.h")
FILE(WRITE "${WINDOWS_UNSUPPORTED_TEST_NAMES_HEADER}"
  "#pragma once\n\ninline constexpr const char* WINDOWS_PRIVATE_ACCESS_UNSUPPORTED_TESTS[] = {\n${WINDOWS_UNSUPPORTED_TEST_NAMES}  nullptr,\n};\n")

SET(WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/windows-private-access-unsupported-tests.cpp")
FILE(WRITE "${WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE}" "${WINDOWS_UNSUPPORTED_TEST_STUBS}")

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
  ${WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE}
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
# view-debug.h guards its Dump*() declarations (and the view-impl.h include
# that provides GetImpl) behind DEBUG_ENABLED, and view-debug.cpp compiles the
# matching definitions out without it. The prebuilt Release dali2-core does not
# export Debug::Filter::New (dali-core defines it only under DEBUG_ENABLED), so
# DEBUG_ENABLED cannot be switched on target-wide: scope it to exactly the two
# translation units that need it, in every config.
SET_SOURCE_FILES_PROPERTIES("${FOUNDATION_DIR}/internal/views/view/view-debug.cpp"
  PROPERTIES COMPILE_DEFINITIONS DEBUG_ENABLED)
SET_SOURCE_FILES_PROPERTIES("${INTERNAL_TEST_DIR}/utc-Dali-ViewDebug-internal.cpp"
  PROPERTIES COMPILE_DEFINITIONS DEBUG_ENABLED)
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

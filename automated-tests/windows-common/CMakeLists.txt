CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(dali-ui-common-tests LANGUAGES C CXX)

INCLUDE(GNUInstallDirs)

IF(NOT WIN32)
  MESSAGE(FATAL_ERROR "The DALi UI common Windows tests must be built for Windows.")
ENDIF()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(DALI_UI_WINDOWS_TEST_MODULES "dali-ui-foundation-internal" CACHE STRING
  "DALi UI test modules to build on Windows")
SET(SUPPORTED_DALI_UI_WINDOWS_TEST_MODULES
  dali-ui-foundation
  dali-ui-foundation-internal
  dali-ui-components
)
FOREACH(MODULE IN LISTS DALI_UI_WINDOWS_TEST_MODULES)
  IF(NOT MODULE IN_LIST SUPPORTED_DALI_UI_WINDOWS_TEST_MODULES)
    MESSAGE(FATAL_ERROR "Unsupported DALi UI Windows test module: ${MODULE}")
  ENDIF()
ENDFOREACH()

FIND_PACKAGE(dali-windows-dependencies CONFIG REQUIRED)
FIND_PACKAGE(dali2-core CONFIG REQUIRED)
FIND_PACKAGE(dali2-adaptor CONFIG REQUIRED)
FIND_PACKAGE(dali2-ui-foundation CONFIG REQUIRED)
FIND_PACKAGE(dali2-ui-components CONFIG REQUIRED)
FIND_PACKAGE(Intl REQUIRED)

GET_TARGET_PROPERTY(DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIRS
  dali-windows-dependencies::dali-windows-dependencies
  INTERFACE_INCLUDE_DIRECTORIES)
LIST(GET DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIRS 0 DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR)

SET(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
SET(FOUNDATION_DIR "${ROOT_DIR}/dali-ui-foundation")
SET(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
SET(FOUNDATION_TEST_DIR "${TEST_DIR}/dali-ui-foundation")
SET(INTERNAL_TEST_DIR "${TEST_DIR}/dali-ui-foundation-internal")
SET(COMPONENTS_TEST_DIR "${TEST_DIR}/dali-ui-components")
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
INCLUDE("${FOUNDATION_TEST_DIR}/CMakeLists.txt")
SET(FOUNDATION_TC_NAMES ${TC_SOURCE_LIST})
SET(FOUNDATION_RUN_SOURCES ${TC_SOURCES})

INCLUDE("${INTERNAL_TEST_DIR}/CMakeLists.txt")
SET(INTERNAL_TC_NAMES ${TC_SOURCE_LIST})
SET(INTERNAL_RUN_SOURCES ${TC_SOURCES})

INCLUDE("${COMPONENTS_TEST_DIR}/CMakeLists.txt")
SET(COMPONENTS_TC_NAMES ${TC_SOURCE_LIST})
SET(COMPONENTS_RUN_SOURCES ${TC_SOURCES})
SET(COMPONENTS_DIRECT_SOURCES ${COMPONENT_INTERNAL_TEST_SOURCES})
UNSET(DALI_TEST_SOURCE_LIST_ONLY)

LIST(REMOVE_ITEM FOUNDATION_RUN_SOURCES "../dali-test-suite-utils/test-harness.cpp")
LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "../dali-test-suite-utils/test-harness.cpp")
LIST(REMOVE_ITEM COMPONENTS_RUN_SOURCES "../dali-test-suite-utils/test-harness.cpp")

LIST(TRANSFORM FOUNDATION_TC_NAMES PREPEND "${FOUNDATION_TEST_DIR}/")
LIST(TRANSFORM FOUNDATION_RUN_SOURCES PREPEND "${FOUNDATION_TEST_DIR}/")
LIST(TRANSFORM INTERNAL_TC_NAMES PREPEND "${INTERNAL_TEST_DIR}/")
LIST(TRANSFORM INTERNAL_RUN_SOURCES PREPEND "${INTERNAL_TEST_DIR}/")
LIST(TRANSFORM COMPONENTS_TC_NAMES PREPEND "${COMPONENTS_TEST_DIR}/")
LIST(TRANSFORM COMPONENTS_RUN_SOURCES PREPEND "${COMPONENTS_TEST_DIR}/")
LIST(TRANSFORM COMPONENTS_DIRECT_SOURCES PREPEND "${COMPONENTS_TEST_DIR}/")

# MSVC includes member access levels in decorated names. Tests that rewrite
# private/protected as public therefore cannot link to separately compiled
# foundation implementation objects and are reported as unsupported on Windows.
SET(WINDOWS_UNSUPPORTED_TEST_SOURCES
  "${FOUNDATION_TEST_DIR}/utc-Dali-ScrollView.cpp"
  "${FOUNDATION_TEST_DIR}/utc-Dali-RecyclerView.cpp"
  "${FOUNDATION_TEST_DIR}/utc-Dali-WebView.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-AnimatedImageReleasePolicy-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-ArcWireframeVisual-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-FeedbackStyle-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-ImageCache-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-InteractiveView-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-MeshVisual-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-ObjLoader-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-PageScrollView-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-PrimitiveVisual-internal.cpp"
  "${INTERNAL_TEST_DIR}/utc-Dali-WindowsWarningCoverage-internal.cpp"
  "${COMPONENTS_TEST_DIR}/utc-Dali-ChartView.cpp"
)

SET(WINDOWS_UNSUPPORTED_TEST_NAMES "")
SET(WINDOWS_UNSUPPORTED_TEST_STUBS "")
FOREACH(TEST_PATH IN LISTS WINDOWS_UNSUPPORTED_TEST_SOURCES)
  LIST(REMOVE_ITEM FOUNDATION_RUN_SOURCES "${TEST_PATH}")
  LIST(REMOVE_ITEM INTERNAL_RUN_SOURCES "${TEST_PATH}")
  LIST(REMOVE_ITEM COMPONENTS_RUN_SOURCES "${TEST_PATH}")
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

SET(ALL_TC_NAMES ${FOUNDATION_TC_NAMES} ${INTERNAL_TC_NAMES} ${COMPONENTS_TC_NAMES})
FOREACH(TEST_PATH IN LISTS ALL_TC_NAMES)
  FILE(READ "${TEST_PATH}" TEST_CONTENT)
  STRING(REGEX MATCH "#[ \t]*define[ \t]+(private|protected)[ \t]+public" ACCESS_OVERRIDE "${TEST_CONTENT}")
  IF(ACCESS_OVERRIDE)
    LIST(FIND WINDOWS_UNSUPPORTED_TEST_SOURCES "${TEST_PATH}" UNSUPPORTED_INDEX)
    IF(UNSUPPORTED_INDEX EQUAL -1)
      GET_FILENAME_COMPONENT(TEST_FILENAME "${TEST_PATH}" NAME)
      MESSAGE(FATAL_ERROR "${TEST_FILENAME} rewrites C++ access levels and must be listed in WINDOWS_UNSUPPORTED_TEST_SOURCES for MSVC.")
    ENDIF()
  ENDIF()
ENDFOREACH()

SET(WINDOWS_UNSUPPORTED_TEST_NAMES_HEADER "${CMAKE_CURRENT_BINARY_DIR}/windows-private-access-unsupported-tests.h")
FILE(WRITE "${WINDOWS_UNSUPPORTED_TEST_NAMES_HEADER}"
  "#pragma once\n\ninline constexpr const char* WINDOWS_PRIVATE_ACCESS_UNSUPPORTED_TESTS[] = {\n${WINDOWS_UNSUPPORTED_TEST_NAMES}  nullptr,\n};\n")

SET(WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/windows-private-access-unsupported-tests.cpp")
FILE(WRITE "${WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE}" "${WINDOWS_UNSUPPORTED_TEST_STUBS}")

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

SET(FOUNDATION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/dali-ui-foundation-testcases.h")
SET(INTERNAL_HEADER "${CMAKE_CURRENT_BINARY_DIR}/dali-ui-foundation-internal-testcases.h")
SET(COMPONENTS_HEADER "${CMAKE_CURRENT_BINARY_DIR}/dali-ui-components-testcases.h")
GENERATE_TEST_HEADER("${FOUNDATION_HEADER}" gDaliUiFoundationTests ${FOUNDATION_TC_NAMES})
GENERATE_TEST_HEADER("${INTERNAL_HEADER}" gDaliUiFoundationInternalTests ${INTERNAL_TC_NAMES})
GENERATE_TEST_HEADER("${COMPONENTS_HEADER}" gDaliUiComponentsTests ${COMPONENTS_TC_NAMES})

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


FUNCTION(CONFIGURE_UI_TEST_RUNNER TARGET ARRAY_NAME HEADER_NAME SUITE_LABEL)
  TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${TEST_DIR}/common"
    "${FOUNDATION_TEST_DIR}"
    "${COMPONENTS_TEST_DIR}"
    "${TEST_UTIL_DIR}"
    "${TEST_UTIL_DIR}/dali-ui"
    "${ROOT_DIR}"
    "${ROOT_DIR}/third-party/md4c"
    "${ROOT_DIR}/../dali-adaptor/dali"
  )
  TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
    BUILDING_DALI_ADAPTOR
    DALI_TEST_ARRAY=${ARRAY_NAME}
    DALI_TEST_HEADER="${HEADER_NAME}"
    DALI_TEST_SUITE="${SUITE_LABEL}"
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
  TARGET_COMPILE_OPTIONS(${TARGET} PRIVATE
    /FI "${DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR}/dali-windows-dependencies.h"
    /vmg /utf-8 /W3 /wd4251 /wd4275
  )
  TARGET_LINK_LIBRARIES(${TARGET} PRIVATE
    dali-windows-dependencies::dali-windows-dependencies
    dali2-ui-components::dali2-ui-components
    dali2-ui-foundation::dali2-ui-foundation
    dali2-adaptor::dali2-adaptor
    dali2-core::dali2-core
    Intl::Intl
    dbghelp
  )
ENDFUNCTION()

ADD_EXECUTABLE(tct-dali-ui-foundation-core
  ${WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE}
  src/common-test-main.cpp
  src/adaptor-static-constants.cpp
  ${FOUNDATION_RUN_SOURCES}
)
CONFIGURE_UI_TEST_RUNNER(tct-dali-ui-foundation-core
  gDaliUiFoundationTests dali-ui-foundation-testcases.h foundation)

ADD_LIBRARY(dali-ui-components-test-md4c STATIC
  "${ROOT_DIR}/third-party/md4c/entity.c"
  "${ROOT_DIR}/third-party/md4c/md4c.c"
)
TARGET_INCLUDE_DIRECTORIES(dali-ui-components-test-md4c PRIVATE
  "${ROOT_DIR}/third-party/md4c"
)

ADD_EXECUTABLE(tct-dali-ui-components-core
  ${WINDOWS_UNSUPPORTED_TEST_STUB_SOURCE}
  src/common-test-main.cpp
  src/adaptor-static-constants.cpp
  ${COMPONENTS_RUN_SOURCES}
)
CONFIGURE_UI_TEST_RUNNER(tct-dali-ui-components-core
  gDaliUiComponentsTests dali-ui-components-testcases.h components)
TARGET_LINK_LIBRARIES(tct-dali-ui-components-core PRIVATE dali-ui-components-test-md4c)
SET_SOURCE_FILES_PROPERTIES(${COMPONENTS_DIRECT_SOURCES}
  PROPERTIES COMPILE_DEFINITIONS BUILDING_DALI_UI_COMPONENTS)

FUNCTION(ADD_UI_TEST_ADDON ADDON_TARGET TEST_TARGET TEST_SOURCE ADDON_DIRECTORY)
  TARGET_COMPILE_DEFINITIONS(${TEST_TARGET} PRIVATE ADDON_LIBS_PATH="${ADDON_DIRECTORY}")

  ADD_LIBRARY(${ADDON_TARGET} MODULE "${TEST_SOURCE}")
  TARGET_INCLUDE_DIRECTORIES(${ADDON_TARGET} PRIVATE
    "${ROOT_DIR}"
    "${TEST_UTIL_DIR}"
  )
  TARGET_COMPILE_DEFINITIONS(${ADDON_TARGET} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
  TARGET_COMPILE_OPTIONS(${ADDON_TARGET} PRIVATE
    /FI "${DALI_WINDOWS_DEPENDENCIES_INCLUDE_DIR}/dali-windows-dependencies.h"
    /vmg /utf-8 /W3 /wd4251 /wd4275
  )
  TARGET_LINK_LIBRARIES(${ADDON_TARGET} PRIVATE
    dali-windows-dependencies::dali-windows-dependencies
    dali2-core::dali2-core
  )
  SET_TARGET_PROPERTIES(${ADDON_TARGET} PROPERTIES
    OUTPUT_NAME SampleAddOn
    LIBRARY_OUTPUT_DIRECTORY "${ADDON_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${ADDON_DIRECTORY}"
  )
  FILE(GENERATE OUTPUT "${ADDON_DIRECTORY}/addons.txt"
    CONTENT "$<TARGET_FILE_NAME:${ADDON_TARGET}>\n")
ENDFUNCTION()

ADD_UI_TEST_ADDON(FoundationSampleAddOn
  tct-dali-ui-foundation-core
  "${FOUNDATION_TEST_DIR}/test-sample-addon.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/foundation-addon")
ADD_UI_TEST_ADDON(ComponentsSampleAddOn
  tct-dali-ui-components-core
  "${COMPONENTS_TEST_DIR}/test-sample-addon.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/components-addon")

FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/addons.txt" "")

IF("dali-ui-foundation-internal" IN_LIST DALI_UI_WINDOWS_TEST_MODULES)
  INSTALL(TARGETS tct-dali-ui-foundation-internal-core
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
ELSE()
  SET_TARGET_PROPERTIES(
    dali-ui-foundation-test-objects
    tct-dali-ui-foundation-internal-core
    PROPERTIES EXCLUDE_FROM_ALL TRUE)
ENDIF()

IF("dali-ui-foundation" IN_LIST DALI_UI_WINDOWS_TEST_MODULES)
  INSTALL(TARGETS tct-dali-ui-foundation-core
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
ELSE()
  SET_TARGET_PROPERTIES(
    tct-dali-ui-foundation-core
    FoundationSampleAddOn
    PROPERTIES EXCLUDE_FROM_ALL TRUE)
ENDIF()

IF("dali-ui-components" IN_LIST DALI_UI_WINDOWS_TEST_MODULES)
  INSTALL(TARGETS tct-dali-ui-components-core
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
ELSE()
  SET_TARGET_PROPERTIES(
    dali-ui-components-test-md4c
    tct-dali-ui-components-core
    ComponentsSampleAddOn
    PROPERTIES EXCLUDE_FROM_ALL TRUE)
ENDIF()

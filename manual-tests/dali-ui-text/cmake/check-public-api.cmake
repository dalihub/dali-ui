# Inspect authored app code, not DALi's transitive public-header dependencies.
# This is a guard against accidental private API use, not a C++ parser.
IF(NOT TEXT_APP_SOURCE_DIR)
  MESSAGE(FATAL_ERROR "TEXT_APP_SOURCE_DIR is required")
ENDIF()
FILE(GLOB_RECURSE TEXT_APP_SOURCES
  "${TEXT_APP_SOURCE_DIR}/tc/*.cpp" "${TEXT_APP_SOURCE_DIR}/tc/*.h"
  "${TEXT_APP_SOURCE_DIR}/common/*.cpp" "${TEXT_APP_SOURCE_DIR}/common/*.h")
LIST(APPEND TEXT_APP_SOURCES "${TEXT_APP_SOURCE_DIR}/main.cpp" "${TEXT_APP_SOURCE_DIR}/text-test-case.h")
IF(EXISTS "${TEXT_APP_SOURCE_DIR}/../dali-ui-foundation/manual-test-case.h")
  LIST(APPEND TEXT_APP_SOURCES "${TEXT_APP_SOURCE_DIR}/../dali-ui-foundation/manual-test-case.h")
ENDIF()
FOREACH(SRC ${TEXT_APP_SOURCES})
  FILE(READ "${SRC}" CONTENT)
  IF(CONTENT MATCHES "#[ \t]*include[^\n]*(devel-api|integration-api|/internal/|extension-api)" OR
     CONTENT MATCHES "(^|[^A-Za-z0-9_])(Internal|Integration|Devel[A-Za-z0-9_]*)::|using[ \t]+namespace[^;\n]*(Internal|Integration|Devel[A-Za-z0-9_]*)")
    MESSAGE(FATAL_ERROR "Text manual tests must use Public API only: ${SRC}")
  ENDIF()
ENDFOREACH()

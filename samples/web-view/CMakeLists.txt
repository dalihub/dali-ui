CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
PROJECT(web-view.example)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(DALICORE      REQUIRED dali2-core)
PKG_CHECK_MODULES(DALIADAPTOR   REQUIRED dali2-adaptor)
PKG_CHECK_MODULES(DALI_UI_FOUNDATION REQUIRED dali2-ui-foundation)

# ---------------------------------------------------------------------------
# Macro: add one executable target for a web-view sample
# ---------------------------------------------------------------------------
MACRO(ADD_WEBVIEW_SAMPLE EXE_NAME SOURCE_FILE)
  ADD_EXECUTABLE(${EXE_NAME} ${SOURCE_FILE})

  SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

  INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)

  TARGET_LINK_LIBRARIES(${EXE_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    ${DALI_UI_FOUNDATION_LIBRARIES})

  TARGET_LINK_DIRECTORIES(${EXE_NAME}
    PRIVATE $ENV{DESKTOP_PREFIX}/lib)

  TARGET_INCLUDE_DIRECTORIES(${EXE_NAME}
    PRIVATE
      ${DALICORE_INCLUDE_DIRS}
      ${DALIADAPTOR_INCLUDE_DIRS}
      ${DALI_UI_FOUNDATION_INCLUDE_DIRS})

  TARGET_COMPILE_OPTIONS(${EXE_NAME}
    PRIVATE
      ${DALICORE_CFLAGS_OTHER}
      ${DALIADAPTOR_CFLAGS_OTHER}
      ${DALI_UI_FOUNDATION_CFLAGS_OTHER})
ENDMACRO()

# ---------------------------------------------------------------------------
# Sample targets
# ---------------------------------------------------------------------------

# Basic: loads a URL, shows load events, supports navigation keys
ADD_WEBVIEW_SAMPLE(web-view.example web-view-example.cpp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables
IF(POLICY CMP0177)
  CMAKE_POLICY(SET CMP0177 NEW) # install() DESTINATION paths are normalized.
ENDIF()

SET(name "dali2-ui")
PROJECT(${name})

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 17)
SET(GCC_COMPILER_VERSION_REQUIRED "6")

# API VERSION (Not DALi release version)
SET(DALI_UI_VERSION_MAJOR 2)
SET(DALI_UI_VERSION_MINOR 0)
SET(DALI_UI_VERSION_PATCH 0)
SET(DALI_UI_VERSION ${DALI_UI_VERSION_MAJOR}.${DALI_UI_VERSION_MINOR}.${DALI_UI_VERSION_PATCH})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_COMPILER_VERSION_REQUIRED)
    message(FATAL_ERROR "The GCC required compiler version is " ${GCC_COMPILER_VERSION_REQUIRED})
  endif()
endif()

# From root/build/tizen, get back to root
SET(ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# Make sure the path is absolute
GET_FILENAME_COMPONENT(ROOT_SRC_DIR ${ROOT_SRC_DIR} ABSOLUTE)

# Register clang hook
if(NOT GBS_BUILD AND UNIX)
    set(HOOK_SOURCE "${ROOT_SRC_DIR}/scripts/pre-commit")
    set(HOOK_DEST "${ROOT_SRC_DIR}/.git/hooks/pre-commit")

    if(EXISTS "${ROOT_SRC_DIR}/.git" AND EXISTS "${HOOK_SOURCE}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${HOOK_SOURCE} ${HOOK_DEST})
        execute_process(COMMAND chmod +x ${HOOK_DEST})
        message(STATUS "Git pre-commit hook registered automatically.")
    endif()
endif()

# Define options to CMake
IF(WIN32)
  SET(ENABLE_PKG_CONFIGURE_DEFAULT OFF)
  SET(INSTALL_CMAKE_MODULES_DEFAULT ON)
  SET(ENABLE_VECTOR_BASED_TEXT_RENDERING_DEFAULT OFF)
ELSE()
  SET(ENABLE_PKG_CONFIGURE_DEFAULT ON)
  SET(INSTALL_CMAKE_MODULES_DEFAULT OFF)
  SET(ENABLE_VECTOR_BASED_TEXT_RENDERING_DEFAULT ON)
ENDIF()

OPTION(USE_DEFAULT_RESOURCE_DIR  "Whether to use the default resource folders." ON)
OPTION(ENABLE_EXPORTALL          "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG              "Enable Debug" OFF)
OPTION(ENABLE_TRACE              "Enable Trace" OFF)
OPTION(ENABLE_COVERAGE           "Coverage" OFF)
OPTION(ENABLE_PKG_CONFIGURE      "Use pkgconfig" ${ENABLE_PKG_CONFIGURE_DEFAULT})
OPTION(ENABLE_GPU_MEMORY_PROFILE "Enable GPU memory profiling" OFF)
OPTION(ENABLE_VECTOR_BASED_TEXT_RENDERING "Enable vector based text rendering" ${ENABLE_VECTOR_BASED_TEXT_RENDERING_DEFAULT})
OPTION(INSTALL_CMAKE_MODULES     "Install CMake package configuration files" ${INSTALL_CMAKE_MODULES_DEFAULT})
OPTION(GBS_BUILD                 "Whether it is for GBS build" OFF)

IF(WIN32)
  IF(ENABLE_PKG_CONFIGURE)
    MESSAGE(FATAL_ERROR "pkg-config is not supported by the Windows build; use the installed CMake packages")
  ENDIF()
  IF(ENABLE_VECTOR_BASED_TEXT_RENDERING)
    MESSAGE(FATAL_ERROR "ENABLE_VECTOR_BASED_TEXT_RENDERING is not supported by the Windows backend")
  ENDIF()

  FIND_PACKAGE(dali2-core CONFIG REQUIRED)
  FIND_PACKAGE(dali2-adaptor CONFIG REQUIRED)
  FIND_PACKAGE(unofficial-angle CONFIG REQUIRED)

  SET(DALI_UI_PLATFORM_LIBRARIES
    dali2-core::dali2-core
    dali2-adaptor::dali2-adaptor
    unofficial::angle::libEGL
    unofficial::angle::libGLESv2
  )
ELSEIF(ENABLE_PKG_CONFIGURE)
  FIND_PACKAGE( PkgConfig REQUIRED )
  PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
  PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
  PKG_CHECK_MODULES(OPENGLES glesv2 egl)
  SET(DALI_UI_PLATFORM_LIBRARIES
    ${DALICORE_LDFLAGS}
    ${DALIADAPTOR_LDFLAGS}
    ${OPENGLES_LDFLAGS}
  )
ELSE()
  FIND_PACKAGE(dali2-core CONFIG REQUIRED)
  FIND_PACKAGE(dali2-adaptor CONFIG REQUIRED)
  SET(DALI_UI_PLATFORM_LIBRARIES
    dali2-core::dali2-core
    dali2-adaptor::dali2-adaptor
  )
ENDIF()

# gettext is optional on desktop Windows. Existing backends retain their
# platform-provided gettext implementation.
IF(WIN32)
  FIND_PACKAGE(Intl QUIET)
  IF(Intl_FOUND)
    ADD_COMPILE_DEFINITIONS(DALI_UI_HAS_GETTEXT)
    SET(DALI_UI_GETTEXT_INCLUDE_DIRS ${Intl_INCLUDE_DIRS})
    SET(DALI_UI_GETTEXT_LIBRARIES ${Intl_LIBRARIES})
  ENDIF()
ENDIF()

SET( VCPKG_INCLUDE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

# Deployment folder should come from spec file or command line:
SET( PREFIX ${CMAKE_INSTALL_PREFIX})
SET( EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})

IF( DEFINED ENV{DALI_DATA_RW_DIR} )
  SET( dataReadWriteDir $ENV{DALI_DATA_RW_DIR} )
ELSE()
  SET( dataReadWriteDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

IF( DEFINED ENV{DALI_DATA_RO_DIR} )
  SET( dataReadOnlyDir $ENV{DALI_DATA_RO_DIR} )
ELSE()
  SET( dataReadOnlyDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

IF( DEFINED ENV{DALI_DATA_RO_INSTALL_DIR} )
  SET( dataReadOnlyInstallDir $ENV{DALI_DATA_RO_INSTALL_DIR} )
ELSE()
  SET( dataReadOnlyInstallDir ${dataReadOnlyDir} )
ENDIF()

IF( USE_DEFAULT_RESOURCE_DIR )
  ADD_COMPILE_DEFINITIONS( DALI_IMAGE_DIR="${dataReadOnlyDir}/ui/images/" )
  ADD_COMPILE_DEFINITIONS( DALI_DATA_READ_ONLY_DIR="${dataReadOnlyDir}" )
  # Share resources with toolkit
  ADD_COMPILE_DEFINITIONS( DALI_SOUND_DIR="${dataReadOnlyDir}/toolkit/sounds/" )
  ADD_COMPILE_DEFINITIONS( DALI_STYLE_DIR="${dataReadOnlyDir}/toolkit/styles/" )
  ADD_COMPILE_DEFINITIONS( DALI_STYLE_IMAGE_DIR="${dataReadOnlyDir}/toolkit/styles/images/" )
ELSE()
  ADD_COMPILE_DEFINITIONS( DALI_IMAGE_DIR=0 DALI_SOUND_DIR=0 DALI_STYLE_DIR=0 DALI_STYLE_IMAGE_DIR=0 DALI_DATA_READ_ONLY_DIR=0 )
ENDIF()

IF(ENABLE_VECTOR_BASED_TEXT_RENDERING)
  ADD_COMPILE_DEFINITIONS(ENABLE_VECTOR_BASED_TEXT_RENDERING)
ENDIF()

# Set up compiler definitions
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  SET( ENABLE_DEBUG ON )
ENDIF()

IF( ENABLE_DEBUG )
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Debug )
  ADD_COMPILE_DEFINITIONS(DEBUG_ENABLED)
  SET( ENABLE_EXPORTALL ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

IF(WIN32)
  ADD_COMPILE_DEFINITIONS(WIN32_LEAN_AND_MEAN NOMINMAX)
ELSE()
  ADD_COMPILE_DEFINITIONS(PIC STDC_HEADERS)
ENDIF()

IF("${ARCH}" STREQUAL "arm")
  ADD_COMPILE_DEFINITIONS(TARGET)
ENDIF("${ARCH}" STREQUAL "arm")

IF(NOT WIN32 AND NOT ENABLE_EXPORTALL)
  ADD_COMPILE_DEFINITIONS(HIDE_DALI_INTERNALS)
ENDIF()

IF( ENABLE_TRACE )
  ADD_COMPILE_DEFINITIONS(TRACE_ENABLED)
ENDIF()

# Remove below when thorvg is ready
ADD_COMPILE_DEFINITIONS(NO_THORVG)

IF( ENABLE_GPU_MEMORY_PROFILE )
  ADD_COMPILE_DEFINITIONS(GPU_MEMORY_PROFILE_ENABLED)
ENDIF()

# Set up the include dir
SET( INCLUDE_DIR $ENV{includedir} )
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} )
ENDIF()
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${PREFIX}/include )
ENDIF()

# Set up the lib dir
SET( LIB_DIR $ENV{libdir} )
IF( NOT LIB_DIR )
  SET( LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
ENDIF()
IF( NOT LIB_DIR )
  SET( LIB_DIR ${PREFIX}/lib )
ENDIF()

# Set up the bin dir
SET( BIN_DIR $ENV{bindir} )
IF( NOT BIN_DIR )
  SET( BIN_DIR ${CMAKE_INSTALL_BINDIR} )
ENDIF()
IF( NOT BIN_DIR )
  SET( BIN_DIR ${PREFIX}/bin )
ENDIF()

INCLUDE(CheckCXXCompilerFlag)

# Set up compiler flags and warnings
IF(MSVC)
  ADD_COMPILE_OPTIONS(/W3 /vmg /wd4251 /wd4275)
  ADD_COMPILE_DEFINITIONS(__restrict__=__restrict)
ELSE()
  ADD_COMPILE_OPTIONS(-Wno-ignored-qualifiers)
  ADD_COMPILE_OPTIONS(-Wall -Wno-unused-parameter -Wno-float-equal)

  CHECK_CXX_COMPILER_FLAG(-Wno-dangling-reference HAVE_NO_DANGLING_REFERENCE)
  IF(HAVE_NO_DANGLING_REFERENCE)
    ADD_COMPILE_OPTIONS($<$<COMPILE_LANGUAGE:CXX>:-Wno-dangling-reference>)
  ENDIF()

  CHECK_CXX_COMPILER_FLAG(-Wno-class-memaccess HAVE_NO_CLASS_MEMACCESS)
  IF(HAVE_NO_CLASS_MEMACCESS)
    ADD_COMPILE_OPTIONS(-Wno-class-memaccess)
  ENDIF()

  CHECK_CXX_COMPILER_FLAG(-Wno-enum-compare-switch HAVE_NO_ENUM_COMPARE_SWITCH)
  IF(HAVE_NO_ENUM_COMPARE_SWITCH)
    ADD_COMPILE_OPTIONS(-Wno-enum-compare-switch)
  ENDIF()
ENDIF()

IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage OR "${CMAKE_CXX_FLAGS}" MATCHES --coverage )
  ADD_COMPILE_OPTIONS( --coverage )
  SET(ENABLE_COVERAGE ON)
  SET(COVERAGE --coverage)
ENDIF()
IF(NOT WIN32 AND NOT ENABLE_EXPORTALL)
  ADD_COMPILE_OPTIONS( "-fvisibility=hidden" )
ENDIF()

SET(DALI_UI_INSTALL_BIN_DIR ${BIN_DIR})
SET(DALI_UI_INSTALL_LIB_DIR ${LIB_DIR})
IF(WIN32 AND ENABLE_DEBUG)
  SET(DALI_UI_INSTALL_BIN_DIR "${BIN_DIR}/debug")
  SET(DALI_UI_INSTALL_LIB_DIR "${LIB_DIR}/debug")
ENDIF()

# Include animation autogen script
IF (NOT GBS_BUILD)
  INCLUDE(${ROOT_SRC_DIR}/scripts/autogen/autogen_anim.cmake)
ENDIF()

# Add libraries
ADD_SUBDIRECTORY(dali-ui-foundation)
ADD_SUBDIRECTORY(dali-ui-components)

SET(CUSTOM_COMPILE_OPTIONS "")


# From the top-level CMake script using ADD_SUBDIRECTORY() to avoid
# target names duplication with other DALi modules.
IF( ENABLE_COVERAGE )
  SET(FASTCOV_BIN "${PREFIX}/bin/fastcov.py")
  FIND_PROGRAM(GCOV_BIN "gcov")
  SET(GCOV_EXPECTED_VERSION_MAJOR "")

  IF( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    STRING(REGEX MATCH "^[0-9]+" GCOV_EXPECTED_VERSION_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
    UNSET(GCOV_VERSIONED_BIN CACHE)
    FIND_PROGRAM(GCOV_VERSIONED_BIN "gcov-${GCOV_EXPECTED_VERSION_MAJOR}")

    IF( GCOV_VERSIONED_BIN )
      SET(GCOV_BIN "${GCOV_VERSIONED_BIN}")
    ENDIF()
  ENDIF()

  IF( EXISTS ${FASTCOV_BIN} )
    IF( NOT GCOV_BIN )
      MESSAGE( FATAL_ERROR "gcov not found; coverage targets cannot be generated" )
    ENDIF()
    IF( GCOV_EXPECTED_VERSION_MAJOR )
      EXECUTE_PROCESS(
        COMMAND ${GCOV_BIN} --version
        OUTPUT_VARIABLE GCOV_VERSION_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE )
      STRING(REGEX MATCH "[0-9]+\\." GCOV_VERSION_MAJOR_MATCH "${GCOV_VERSION_OUTPUT}")
      STRING(REGEX REPLACE "\\.$" "" GCOV_VERSION_MAJOR "${GCOV_VERSION_MAJOR_MATCH}")

      IF( GCOV_VERSION_MAJOR AND NOT GCOV_VERSION_MAJOR STREQUAL GCOV_EXPECTED_VERSION_MAJOR )
        MESSAGE( FATAL_ERROR "${GCOV_BIN} does not match ${CMAKE_CXX_COMPILER}. Compiler major version: ${GCOV_EXPECTED_VERSION_MAJOR}, gcov major version: ${GCOV_VERSION_MAJOR}" )
      ENDIF()
    ENDIF()

    # Define custom rules for coverage
    SET(COVERAGE_DIR .cov)
    SET(COVERAGE_OUTPUT_DIR doc/coverage)

    # fastcov.py doesn't need version detection or complex rc options
    SET(FASTCOV_OPTS "--branch-coverage" "--lcov" "--gcov" "${GCOV_BIN}")

    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}rename_cov_data ./rename-cov-data )

    # fastcov.py combines capture and exclude into a single operation
    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}cov_data
      COMMAND ${FASTCOV_BIN} ${FASTCOV_OPTS}
        -d .
        --exclude dali-env /usr/include /usr/local/include third-party
        -o dali.info )

    # genhtml works with fastcov.py output (lcov-compatible format)
    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}coverage
      COMMAND genhtml --branch-coverage -o ${COVERAGE_OUTPUT_DIR} dali.info
      DEPENDS cov_data )

    # Reset coverage using fastcov.py's zerocounters option
    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}reset_coverage
      COMMAND ${FASTCOV_BIN} -z -d . )

    # Define custom rule for distclean
    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}distclean @echo cleaning for source distribution )
    ADD_CUSTOM_COMMAND(
          DEPENDS ${DALI_UI_PREFIX}clean
          COMMENT "distribution clean"
          COMMAND find
          ARGS    .
          -not -name config.cmake -and \(
          -name tester.c -or
          -name Testing -or
          -name CMakeFiles -or
          -name doc -or
          -name cmake.depends -or
          -name cmake.check_depends -or
          -name CMakeCache.txt -or
          -name cmake.check_cache -or
          -name *.cmake -or
          -name Makefile -or
          -name core -or
          -name core.* -or
          -name gmon.out -or
          -name install_manifest.txt -or
          -name *.pc -or
          -name *.gcov -or
          -name *.gcno -or
          -name *.gcda -or
          -name *~ -or
          -name libdali*.so* \)
          | grep -v TC | xargs rm -rf
          TARGET  ${DALI_UI_PREFIX}distclean
          VERBATIM
          )

  ELSE()
    MESSAGE( FATAL_ERROR "fastcov.py not found at ${FASTCOV_BIN}; coverage targets cannot be generated" )
  ENDIF()
ENDIF()

# Configuration Messages
MESSAGE( STATUS "Configuration:\n" )
MESSAGE( STATUS "Prefix:                        " ${PREFIX} )
MESSAGE( STATUS "Lib Dir:                       " ${LIB_DIR} )
MESSAGE( STATUS "Include Dir:                   " ${INCLUDE_DIR} )
MESSAGE( STATUS "Debug build:                   " ${ENABLE_DEBUG} )
MESSAGE( STATUS "Export all symbols:            " ${ENABLE_EXPORTALL} )
MESSAGE( STATUS "Coverage:                      " ${ENABLE_COVERAGE} )
MESSAGE( STATUS "Trace:                         " ${ENABLE_TRACE} )
MESSAGE( STATUS "Data Dir (Read/Write):         " ${dataReadWriteDir} )
MESSAGE( STATUS "Data Dir (Read Only):          " ${dataReadOnlyDir} )
MESSAGE( STATUS "Use pkg configure:             " ${ENABLE_PKG_CONFIGURE} )
MESSAGE( STATUS "GPU memory profiling:          " ${ENABLE_GPU_MEMORY_PROFILE} )
MESSAGE( STATUS "CXXFLAGS:                      " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:                       " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )

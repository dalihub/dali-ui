CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables

# Project names for the two libraries
SET(foundation_name "dali2-ui-foundation")
SET(components_name "dali2-ui-components")
SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 17)

PROJECT(${foundation_name})

# Package names for pkg-config
SET(FOUNDATION_PKG_NAME ${foundation_name})
SET(COMPONENTS_PKG_NAME ${components_name})

SET(GCC_COMPILER_VERSION_REQUIRED "6")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_COMPILER_VERSION_REQUIRED)
    message(FATAL_ERROR "The GCC required compiler version is " ${GCC_COMPILER_VERSION_REQUIRED})
  endif()
endif()

# API VERSION (Not DALi release version)
SET(${foundation_name}_VERSION_MAJOR 2)
SET(${foundation_name}_VERSION_MINOR 0)
SET(${foundation_name}_VERSION_PATCH 0)
SET(${foundation_name}_VERSION ${${foundation_name}_VERSION_MAJOR}.${${foundation_name}_VERSION_MINOR}.${${foundation_name}_VERSION_PATCH} )
SET(DALI_UI_FOUNDATION_VERSION ${${foundation_name}_VERSION} )

SET(${components_name}_VERSION_MAJOR 2)
SET(${components_name}_VERSION_MINOR 0)
SET(${components_name}_VERSION_PATCH 0)
SET(${components_name}_VERSION ${${components_name}_VERSION_MAJOR}.${${components_name}_VERSION_MINOR}.${${components_name}_VERSION_PATCH} )
SET(DALI_UI_COMPONENTS_VERSION ${${components_name}_VERSION} )

# Define options to CMake
OPTION(ENABLE_EXPORTALL          "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG              "Enable Debug" OFF)
OPTION(ENABLE_TRACE              "Enable Trace" OFF)
OPTION(ENABLE_I18N               "Turns on internationalisation" OFF)
OPTION(ENABLE_COVERAGE           "Coverage" OFF)
OPTION(ENABLE_PKG_CONFIGURE      "Use pkgconfig" ON)
OPTION(ENABLE_LINK_TEST          "Enable the link test" ON)
OPTION(INSTALL_DOXYGEN_DOC       "Install doxygen doc" ON)
OPTION(CONFIGURE_AUTOMATED_TESTS "Configure automated tests" ON)
OPTION(USE_DEFAULT_RESOURCE_DIR  "Whether to use the default resource folders. Otherwise set environment variables for DALI_IMAGE_DIR, DALI_SOUND_DIR, DALI_STYLE_DIR, DALI_STYLE_IMAGE_DIR and DALI_DATA_READ_ONLY_DIR" ON)
OPTION(ENABLE_GPU_MEMORY_PROFILE "Enable GPU memory profiling" OFF)

IF( ENABLE_PKG_CONFIGURE )
  FIND_PACKAGE( PkgConfig REQUIRED )
  PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
  PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
  PKG_CHECK_MODULES(DALITOOLKIT REQUIRED dali2-toolkit)
ENDIF()

IF( INSTALL_DOXYGEN_DOC )
  FIND_PACKAGE( Doxygen QUIET )
ENDIF()

SET( VCPKG_INCLUDE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

# from root/build/tizen, get back to root
SET(ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# Deployment folder should come from spec file or command line:
SET( PREFIX ${CMAKE_INSTALL_PREFIX})
SET( EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})

# Make sure the path is absolute
GET_FILENAME_COMPONENT(ROOT_SRC_DIR ${ROOT_SRC_DIR} ABSOLUTE)

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

# Set up compiler definitions
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  SET( ENABLE_DEBUG ON )
ENDIF()

IF( ENABLE_DEBUG )
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Debug )
  ADD_DEFINITIONS( "-DDEBUG_ENABLED" )
  SET( ENABLE_EXPORTALL ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

IF( ENABLE_I18N )
  ADD_DEFINITIONS( "-DDGETTEXT_ENABLED" )
ENDIF( ENABLE_I18N )

ADD_DEFINITIONS(-DPIC -DSTDC_HEADERS)
IF( USE_DEFAULT_RESOURCE_DIR )
  ADD_DEFINITIONS( -DDALI_IMAGE_DIR=\"${dataReadOnlyDir}/ui/images/\"
                    -DDALI_SOUND_DIR=\"${dataReadOnlyDir}/ui/sounds/\"
                    -DDALI_STYLE_DIR=\"${dataReadOnlyDir}/ui/styles/\"
                    -DDALI_STYLE_IMAGE_DIR=\"${dataReadOnlyDir}/ui/styles/images/\"
                    -DDALI_DATA_READ_ONLY_DIR=\"${dataReadOnlyDir}\" )
ENDIF()

IF("${ARCH}" STREQUAL "arm")
  ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

IF( NOT ${ENABLE_EXPORTALL} )
  ADD_DEFINITIONS( "-DHIDE_DALI_INTERNALS" )
ENDIF( NOT ${ENABLE_EXPORTALL} )

IF( NOT USE_DEFAULT_RESOURCE_DIR )
  ADD_DEFINITIONS( -DDALI_IMAGE_DIR=0
                   -DDALI_SOUND_DIR=0
                   -DDALI_STYLE_DIR=0
                   -DDALI_STYLE_IMAGE_DIR=0
                   -DDALI_DATA_READ_ONLY_DIR=0 )
ENDIF()

IF( ENABLE_TRACE )
  ADD_DEFINITIONS("-DTRACE_ENABLED")
ENDIF()

# Remove below when thorvg is ready
ADD_DEFINITIONS( "-DNO_THORVG" )

IF( ENABLE_GPU_MEMORY_PROFILE )
  ADD_DEFINITIONS("-DENABLE_GPU_MEMORY_PROFILE")
ENDIF()

# Set paths
SET( package_doxy_dir    ${ROOT_SRC_DIR}/doc )

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

IF( ENABLE_PKG_CONFIGURE )
  # Configure the pkg-config files
  SET( DEV_INCLUDE_PATH ${INCLUDE_DIR} )
  SET( FOUNDATION_PKG_CFG_FILE dali2-ui-foundation.pc )
  SET( COMPONENTS_PKG_CFG_FILE dali2-ui-components.pc )
  CONFIGURE_FILE( ${FOUNDATION_PKG_CFG_FILE}.in ${FOUNDATION_PKG_CFG_FILE} @ONLY )
  CONFIGURE_FILE( ${COMPONENTS_PKG_CFG_FILE}.in ${COMPONENTS_PKG_CFG_FILE} @ONLY )
ENDIF()

INCLUDE(CheckCXXCompilerFlag)
# Set up compiler flags and warnings
ADD_COMPILE_OPTIONS( -Wno-ignored-qualifiers )
ADD_COMPILE_OPTIONS( -Wall -Wno-unused-parameter -Wno-float-equal )
CHECK_CXX_COMPILER_FLAG( -Wno-dangling-reference HAVE_NO_DANGLING_REFERENCE )
IF (HAVE_NO_DANGLING_REFERENCE)
  ADD_COMPILE_OPTIONS( $<$<COMPILE_LANGUAGE:CXX>:-Wno-dangling-reference> )
ENDIF()
IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage )
  ADD_COMPILE_OPTIONS( --coverage )
  SET(ENABLE_COVERAGE ON)
  SET(COVERAGE --coverage)
ENDIF()
IF( NOT ${ENABLE_EXPORTALL} )
  ADD_COMPILE_OPTIONS( "-fvisibility=hidden" )
ENDIF( NOT ${ENABLE_EXPORTALL} )

INCLUDE_DIRECTORIES(  ${ROOT_SRC_DIR}
  ${DALICORE_INCLUDE_DIRS}
  ${DALIADAPTOR_INCLUDE_DIRS}
  ${DALITOOLKIT_INCLUDE_DIRS}
  ${VCPKG_INCLUDE_DIR}
  ${INCLUDE_DIR})

#SET(SOURCE_DIR "${ROOT_SRC_DIR}/dali")
SET(PACKAGE_DOXY_SRC_DIR "${ROOT_SRC_DIR}/doc")

# ============================================================================
# DALI-UI-FOUNDATION LIBRARY
# ============================================================================

SET(FOUNDATION_SOURCES "")

# Add foundation source files if they exist
IF(EXISTS ${ROOT_SRC_DIR}/dali-ui-foundation)
  FILE(GLOB_RECURSE UI_FOUNDATION_SOURCES
    "${ROOT_SRC_DIR}/dali-ui-foundation/*.cpp")
  SET(FOUNDATION_SOURCES ${FOUNDATION_SOURCES} ${UI_FOUNDATION_SOURCES})
ENDIF()

SET(LIBTYPE SHARED)
IF(DEFINED STATIC)
  SET(LIBTYPE STATIC)
ENDIF()

# Create dali2-ui-foundation library
ADD_LIBRARY( ${foundation_name} ${LIBTYPE} ${FOUNDATION_SOURCES} )

SET(CUSTOM_COMPILE_OPTIONS "")

# TODO: Clang is a lot more strict with warnings, we should address
# those issues at some point.
# Matches "Clang" or "AppleClang"
IF( NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" )
  SET(CUSTOM_COMPILE_OPTIONS ${MY_COMPILE_OPTIONS} -Werror)
ENDIF()

CHECK_CXX_COMPILER_FLAG(-Wno-class-memaccess HAVE_NO_CLASS_MEMACCESS)
IF (HAVE_NO_CLASS_MEMACCESS)
  SET(CUSTOM_COMPILE_OPTIONS ${MY_COMPILE_OPTIONS} -Wno-class-memaccess)
ENDIF()

target_compile_options(${foundation_name} PRIVATE ${CUSTOM_COMPILE_OPTIONS})

TARGET_LINK_LIBRARIES( ${foundation_name}
  ${DALICORE_LDFLAGS}
  ${DALIADAPTOR_LDFLAGS}
  ${DALITOOLKIT_LDFLAGS}
  ${COVERAGE})

SET_TARGET_PROPERTIES( ${foundation_name}
  PROPERTIES
  VERSION ${DALI_UI_FOUNDATION_VERSION}
  SOVERSION ${${foundation_name}_VERSION_MAJOR}
  CLEAN_DIRECT_OUPUT 1)

# ============================================================================
# DALI-UI-COMPONENTS LIBRARY
# ============================================================================

SET(COMPONENTS_SOURCES "")

# Add components source files if they exist
IF(EXISTS ${ROOT_SRC_DIR}/dali-ui-components)
  FILE(GLOB_RECURSE UI_COMPONENTS_SOURCES
    "${ROOT_SRC_DIR}/dali-ui-components/*.cpp")
  SET(COMPONENTS_SOURCES ${COMPONENTS_SOURCES} ${UI_COMPONENTS_SOURCES})
ENDIF()

# Create dali2-ui-components library only if there are source files
IF(COMPONENTS_SOURCES)
  ADD_LIBRARY( ${components_name} ${LIBTYPE} ${COMPONENTS_SOURCES} )

  target_compile_options(${components_name} PRIVATE ${CUSTOM_COMPILE_OPTIONS})

  TARGET_LINK_LIBRARIES( ${components_name}
    ${DALICORE_LDFLAGS}
    ${DALIADAPTOR_LDFLAGS}
    ${DALITOOLKIT_LDFLAGS}
    ${COVERAGE})

  # Components library depends on foundation
  TARGET_LINK_LIBRARIES( ${components_name} ${foundation_name} )

  IF( ANDROID )
    TARGET_LINK_LIBRARIES( ${components_name} log GLESv3 EGL)
  ENDIF()

  SET_TARGET_PROPERTIES( ${components_name}
    PROPERTIES
    VERSION ${DALI_UI_COMPONENTS_VERSION}
    SOVERSION ${${components_name}_VERSION_MAJOR}
    CLEAN_DIRECT_OUPUT 1)

  SET(HAS_COMPONENTS_LIB TRUE)
ELSE()
  MESSAGE(STATUS "No source files found for dali2-ui-components, skipping library creation")
  SET(HAS_COMPONENTS_LIB FALSE)
ENDIF()

# ============================================================================
# INSTALLATION
# ============================================================================

IF( INSTALL_CMAKE_MODULES )
  IF( ENABLE_DEBUG )
    SET( BIN_DIR "${BIN_DIR}/debug" )
    SET( LIB_DIR "${LIB_DIR}/debug" )
  ENDIF()

  # Install the foundation library files
  INSTALL( TARGETS ${foundation_name}
    EXPORT ${foundation_name}-targets
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR}
    RUNTIME DESTINATION ${BIN_DIR}
  )

  # Install the components library files
  IF(HAS_COMPONENTS_LIB)
    INSTALL( TARGETS ${components_name}
      EXPORT ${components_name}-targets
      LIBRARY DESTINATION ${LIB_DIR}
      ARCHIVE DESTINATION ${LIB_DIR}
      RUNTIME DESTINATION ${BIN_DIR}
    )
  ENDIF()

  # Install the cmake modules for foundation
  INSTALL(
    EXPORT ${foundation_name}-targets
    NAMESPACE ${foundation_name}::
    FILE ${foundation_name}-targets.cmake
    DESTINATION share/${foundation_name}
  )

  # Install the cmake modules for components
  IF(HAS_COMPONENTS_LIB)
    INSTALL(
      EXPORT ${components_name}-targets
      NAMESPACE ${components_name}::
      FILE ${components_name}-targets.cmake
      DESTINATION share/${components_name}
    )
  ENDIF()

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${foundation_name}-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${foundation_name}-targets.cmake)
  ")

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${components_name}-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${components_name}-targets.cmake)
    find_dependency(${foundation_name} REQUIRED)
  ")

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${foundation_name}-config.cmake DESTINATION share/${foundation_name})
  IF(HAS_COMPONENTS_LIB)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${components_name}-config.cmake DESTINATION share/${components_name})
  ENDIF()

  # Install the pdb files.
  IF( ENABLE_DEBUG AND WIN32 )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${foundation_name}.pdb DESTINATION ${BIN_DIR} )
    IF(HAS_COMPONENTS_LIB)
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${components_name}.pdb DESTINATION ${BIN_DIR} )
    ENDIF()
  ENDIF()
ELSE()
  # Install the library so files and symlinks
  INSTALL( TARGETS ${foundation_name} DESTINATION ${LIB_DIR} )
  IF(HAS_COMPONENTS_LIB)
    INSTALL( TARGETS ${components_name} DESTINATION ${LIB_DIR} )
  ENDIF()
ENDIF()

# Install the pkg-config files
IF( ENABLE_PKG_CONFIGURE )
  INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/${FOUNDATION_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )
  IF(HAS_COMPONENTS_LIB)
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/${COMPONENTS_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )
  ENDIF()
ENDIF()

# Install headers if they exist
IF(EXISTS ${ROOT_SRC_DIR}/dali-ui-components/public-api)
  INSTALL(DIRECTORY ${ROOT_SRC_DIR}/dali-ui-components/public-api/
          DESTINATION ${INCLUDE_DIR}/dali-ui-components
          FILES_MATCHING PATTERN "*.h")
ENDIF()

IF(EXISTS ${ROOT_SRC_DIR}/dali-ui-foundation)
  INSTALL(DIRECTORY ${ROOT_SRC_DIR}/dali-ui-foundation/public-api/
          DESTINATION ${INCLUDE_DIR}/dali-ui-foundation/public-api
          FILES_MATCHING PATTERN "*.h")
  INSTALL(DIRECTORY ${ROOT_SRC_DIR}/dali-ui-foundation/integration-api/
          DESTINATION ${INCLUDE_DIR}/dali-ui-foundation/integration-api
          FILES_MATCHING PATTERN "*.h")
ENDIF()

# Install main dali-ui-foundation.h if it exists
INSTALL( FILES ${ROOT_SRC_DIR}/dali-ui-foundation/dali-ui-foundation.h DESTINATION ${INCLUDE_DIR}/dali-ui-foundation )

# The DALI_UI_PREFIX must be set if this CMakeLists.txt is executed
# from the top-level CMake script using ADD_SUBDIRECTORY() to avoid
# target names duplication with other DALi modules.
IF( ENABLE_COVERAGE )
  FIND_PROGRAM( LCOV_BIN "lcov" )
  IF( LCOV_BIN )

    # Define custom rules for coverage
    SET(COVERAGE_DIR .cov)
    SET(COVERAGE_OUTPUT_DIR doc/coverage)

    # lcov prior to 1.10 doesn't have -rc option; this and subsequent version don't output
    # branch coverage. Determine the lcov version, and enable branch coverage accordingly.
    # Additionally, lcov version 2.0 and later have different options for branch coverage, and need to ignore some errors
    EXECUTE_PROCESS( COMMAND bash -c "${LCOV_BIN} --version | cut -d' ' -f4" OUTPUT_VARIABLE LCOV_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE )
    IF( NOT LCOV_VERSION VERSION_LESS "2.0" )
      SET(LCOV_OPTS --rc branch_coverage=1 --rc geninfo_unexecuted_blocks=1 --rc no_exception_branch=1 --ignore-errors unused --ignore-errors empty --ignore-errors negative --ignore-errors mismatch)
    ELSEIF( NOT LCOV_VERSION VERSION_LESS "1.10" )
      SET(LCOV_OPTS --rc lcov_branch_coverage=1)
    ENDIF()

    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}rename_cov_data ./rename-cov-data )

    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}cov_data ${LCOV_BIN} ${LCOV_OPTS} --base-directory . --directory . -c -o dali.info
      COMMAND ${LCOV_BIN} ${LCOV_OPTS} --remove dali.info \"*/dali-env/*\" \"/usr/include/*\" \"*/third-party/*\" -o dali.info )

    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}coverage genhtml -p ${ROOT_SRC_DIR} ${LCOV_OPTS} -o ${COVERAGE_OUTPUT_DIR} dali.info
      DEPENDS cov_data )

    ADD_CUSTOM_TARGET( ${DALI_UI_PREFIX}reset_coverage @${LCOV_BIN} -z --directory `pwd` )

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

  ENDIF( LCOV_BIN )
ENDIF()

IF( CONFIGURE_AUTOMATED_TESTS )
  # Configure automated tests
  CONFIGURE_FILE( ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt.in
                  ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt @ONLY )
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
MESSAGE( STATUS "Doxygen:                       " ${doxygenEnabled} )
MESSAGE( STATUS "Data Dir (Read/Write):         " ${dataReadWriteDir} )
MESSAGE( STATUS "Data Dir (Read Only):          " ${dataReadOnlyDir} )
MESSAGE( STATUS "i18n:                          " ${ENABLE_I18N} )
MESSAGE( STATUS "Use pkg configure:             " ${ENABLE_PKG_CONFIGURE} )
MESSAGE( STATUS "Enable link test:              " ${ENABLE_LINK_TEST} )
MESSAGE( STATUS "Configure automated tests:     " ${CONFIGURE_AUTOMATED_TESTS} )
MESSAGE( STATUS "GPU memory profiling:          " ${ENABLE_GPU_MEMORY_PROFILE} )
MESSAGE( STATUS "CXXFLAGS:                      " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:                       " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )
MESSAGE( " Folder       INSTALL_SHARE_DIR : [" ${INSTALL_SHARE_DIR} "]" )
MESSAGE( " Folder          DALI_IMAGE_DIR : [" ${DALI_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_STYLE_DIR : [" ${DALI_STYLE_DIR} "]" )
MESSAGE( " Folder    DALI_STYLE_IMAGE_DIR : [" ${DALI_STYLE_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_SOUND_DIR : [" ${DALI_SOUND_DIR} "]" )
MESSAGE( " Folder DALI_DATA_READ_ONLY_DIR : [" ${DALI_DATA_READ_ONLY_DIR} "]" )

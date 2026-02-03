CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables

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

# Register clang hook
if(NOT GBS_BUILD)
    set(HOOK_SOURCE "${ROOT_SRC_DIR}/scripts/pre-commit")
    set(HOOK_DEST "${ROOT_SRC_DIR}/.git/hooks/pre-commit")

    if(EXISTS "${ROOT_SRC_DIR}/.git" AND EXISTS "${HOOK_SOURCE}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${HOOK_SOURCE} ${HOOK_DEST})
        execute_process(COMMAND chmod +x ${HOOK_DEST})
        message(STATUS "Git pre-commit hook registered automatically.")
    endif()
endif()

# Define options to CMake
OPTION(ENABLE_EXPORTALL          "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG              "Enable Debug" OFF)
OPTION(ENABLE_TRACE              "Enable Trace" OFF)
OPTION(ENABLE_COVERAGE           "Coverage" OFF)
OPTION(ENABLE_PKG_CONFIGURE      "Use pkgconfig" ON)
OPTION(ENABLE_LINK_TEST          "Enable the link test" ON)
OPTION(ENABLE_GPU_MEMORY_PROFILE "Enable GPU memory profiling" OFF)
OPTION(GBS_BUILD                 "Whether it is for GBS build" OFF)

IF( ENABLE_PKG_CONFIGURE )
  FIND_PACKAGE( PkgConfig REQUIRED )
  PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
  PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
  PKG_CHECK_MODULES(DALITOOLKIT REQUIRED dali2-toolkit)
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
  ADD_COMPILE_DEFINITIONS(DEBUG_ENABLED)
  SET( ENABLE_EXPORTALL ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

ADD_COMPILE_DEFINITIONS(PIC)
ADD_COMPILE_DEFINITIONS(STDC_HEADERS)

IF("${ARCH}" STREQUAL "arm")
  ADD_COMPILE_DEFINITIONS(TARGET)
ENDIF("${ARCH}" STREQUAL "arm")

IF( NOT ${ENABLE_EXPORTALL} )
  ADD_COMPILE_DEFINITIONS(HIDE_DALI_INTERNALS)
ENDIF( NOT ${ENABLE_EXPORTALL} )

IF( ENABLE_TRACE )
  ADD_COMPILE_DEFINITIONS(TRACE_ENABLED)
ENDIF()

# Remove below when thorvg is ready
ADD_COMPILE_DEFINITIONS(NO_THORVG)

IF( ENABLE_GPU_MEMORY_PROFILE )
  ADD_COMPILE_DEFINITIONS(ENABLE_GPU_MEMORY_PROFILE)
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

# Add libraries
ADD_SUBDIRECTORY(dali-ui-foundation)
ADD_SUBDIRECTORY(dali-ui-elements)
ADD_SUBDIRECTORY(dali-ui-components)

#INCLUDE_DIRECTORIES(
#  ${ROOT_SRC_DIR}
#  ${DALICORE_INCLUDE_DIRS}
#  ${DALIADAPTOR_INCLUDE_DIRS}
#  ${DALITOOLKIT_INCLUDE_DIRS}
#  ${VCPKG_INCLUDE_DIR}
#  ${INCLUDE_DIR}
#)

#SET(SOURCES "")

# could use INCLUDE to get the source lists a different way;
# would then need to specify a build folder somehow...
#INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/sounds/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/styles/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/styles/images-common/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/dali-ui-foundation/internal/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/dali-ui-foundation/public-api/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/dali-ui-foundation/integration-api/file.list )
#INCLUDE( ${ROOT_SRC_DIR}/doc/file.list )

#SET(LIBTYPE SHARED)
#IF(DEFINED STATIC)
#  SET(LIBTYPE STATIC)
#ENDIF()

#ADD_LIBRARY( ${name} ${LIBTYPE} ${SOURCES} )

SET(CUSTOM_COMPILE_OPTIONS "")

CHECK_CXX_COMPILER_FLAG(-Wno-class-memaccess HAVE_NO_CLASS_MEMACCESS)
IF (HAVE_NO_CLASS_MEMACCESS)
  ADD_COMPILE_OPTIONS(-Wno-class-memaccess)
ENDIF()

#TARGET_LINK_LIBRARIES( ${name}
#  ${DALICORE_LDFLAGS}
#  ${DALIADAPTOR_LDFLAGS}
#  ${DALITOOLKIT_LDFLAGS}
#  ${COVERAGE}
#)

#SET_TARGET_PROPERTIES( ${name}
#  PROPERTIES
#  VERSION ${DALI_UI_VERSION}
#  SOVERSION ${${name}_VERSION_MAJOR}
#  CLEAN_DIRECT_OUPUT 1
#)

#IF( INSTALL_CMAKE_MODULES )
#  IF( ENABLE_DEBUG )
#    SET( BIN_DIR "${BIN_DIR}/debug" )
#    SET( LIB_DIR "${LIB_DIR}/debug" )
#  ENDIF()
#
#  # Install the library files
#  INSTALL( TARGETS ${name}
#    EXPORT ${name}-targets
#    LIBRARY DESTINATION ${LIB_DIR}
#    ARCHIVE DESTINATION ${LIB_DIR}
#    RUNTIME DESTINATION ${BIN_DIR}
#  )
#
#  # Install the cmake modules.
#  INSTALL(
#    EXPORT ${name}-targets
#    NAMESPACE ${name}::
#    FILE ${name}-targets.cmake
#    DESTINATION share/${name}
#  )
#
#  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake "
#    include(CMakeFindDependencyMacro)
#    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-targets.cmake)
#  ")
#
#  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake DESTINATION share/${name})
#
#ELSE()
#  # Install the library so file and symlinks
#  INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )
#ENDIF()

# Install the pkg-config file
#IF( ENABLE_PKG_CONFIGURE )
#  INSTALL( FILES ${UI_FOUNDATION_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )
#  INSTALL( FILES ${UI_FOUNDATION_INTEGRATION_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )
#ENDIF()

# Glob for resources
#MACRO( COPY_RESOURCES FILE_LIST SRC_RELATIVE_DIRECTORY DESTINATION EXPLICIT_DESTINATION )
#  FOREACH( PATTERN ${FILE_LIST} )
#    FILE(GLOB FILES ${PATTERN} )
#    FOREACH( FILE ${FILES} )
#      STRING(REPLACE ${SRC_RELATIVE_DIRECTORY} ${DESTINATION} NEWFILE ${FILE} )
#      GET_FILENAME_COMPONENT(NEWDIR ${NEWFILE} DIRECTORY)
#      IF( "${EXPLICIT_DESTINATION}" STREQUAL "" )
#        STRING( REPLACE "dali-ui-foundation" "ui-foundation" NEWDIR2 ${NEWDIR} )
#        INSTALL( FILES ${FILE} DESTINATION ${NEWDIR2} )
#      ELSE()
#        INSTALL( FILES ${FILE} DESTINATION ${DESTINATION}/${EXPLICIT_DESTINATION} )
#      ENDIF()
#    ENDFOREACH()
#  ENDFOREACH()
#ENDMACRO()

# macro for installing headers by replacing prefix. (TODO, investigate
# if there is a CMAKE way of doing this automatically)
#MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST STRIP_PREFIX REPLACE_PREFIX)
#  GET_FILENAME_COMPONENT( SPREFIX ${STRIP_PREFIX} ABSOLUTE )
#  FOREACH(HEADER ${${HEADER_LIST}})
#    STRING(REGEX MATCH "(.*)[/]" DIR ${HEADER})
#    STRING(REPLACE ${SPREFIX} ${REPLACE_PREFIX} NEWDIR ${DIR})
#    INSTALL(FILES ${HEADER} DESTINATION ${INCLUDE_DIR}/${NEWDIR})
#  ENDFOREACH(HEADER)
#ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

# Install headers using lists defined by ADD_SUBDIRECTORY
#INSTALL_HEADERS_WITH_DIRECTORY(PUBLIC_API_HEADERS ${ROOT_SRC_DIR}/dali-ui-foundation "dali-ui-foundation")
#INSTALL_HEADERS_WITH_DIRECTORY(DEVEL_API_HEADERS ${ROOT_SRC_DIR}/dali-ui-foundation "dali-ui-foundation")
#INSTALL_HEADERS_WITH_DIRECTORY(INTEGRATION_API_HEADERS ${ROOT_SRC_DIR}/dali-ui-foundation "dali-ui-foundation")
#INSTALL_HEADERS_WITH_DIRECTORY(PACKAGE_DOXY_HEADERS ${PACKAGE_DOXY_SRC_DIR} "dali-ui-foundation/doc" )

# Install dali-ui-foundation.h
#INSTALL( FILES ${ROOT_SRC_DIR}/dali-ui-foundation/dali-ui-foundation.h DESTINATION ${INCLUDE_DIR}/dali-ui-foundation )

# From the top-level CMake script using ADD_SUBDIRECTORY() to avoid
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
MESSAGE( STATUS "Use pkg configure:             " ${ENABLE_PKG_CONFIGURE} )
MESSAGE( STATUS "Enable link test:              " ${ENABLE_LINK_TEST} )
MESSAGE( STATUS "GPU memory profiling:          " ${ENABLE_GPU_MEMORY_PROFILE} )
MESSAGE( STATUS "CXXFLAGS:                      " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:                       " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )
MESSAGE( " Folder       INSTALL_SHARE_DIR : [" ${INSTALL_SHARE_DIR} "]" )
MESSAGE( " Folder          DALI_IMAGE_DIR : [" ${DALI_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_STYLE_DIR : [" ${DALI_STYLE_DIR} "]" )
MESSAGE( " Folder    DALI_STYLE_IMAGE_DIR : [" ${DALI_STYLE_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_SOUND_DIR : [" ${DALI_SOUND_DIR} "]" )
MESSAGE( " Folder DALI_DATA_READ_ONLY_DIR : [" ${DALI_DATA_READ_ONLY_DIR} "]" )

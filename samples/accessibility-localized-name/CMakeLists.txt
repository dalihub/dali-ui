CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(accessibility-localized-name.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(accessibility-localized-name.example accessibility-localized-name-example.cpp)

# Build the real gettext catalogs used by the sample. DALI_ADD_SAMPLE installs
# the res directory and defines RESOURCES_DIR for desktop, Windows, and Tizen.
IF(DEFINED ENV{DALI_MSGFMT_EXECUTABLE}
   AND EXISTS "$ENV{DALI_MSGFMT_EXECUTABLE}")
  SET(MSGFMT_EXECUTABLE "$ENV{DALI_MSGFMT_EXECUTABLE}" CACHE FILEPATH
      "Path to the gettext msgfmt executable" FORCE)
ELSE()
  FIND_PROGRAM(MSGFMT_EXECUTABLE NAMES msgfmt msgfmt.exe)
ENDIF()

IF(MSGFMT_EXECUTABLE)
  SET(ACCESSIBILITY_LOCALIZATION_DOMAIN accessibility-localized-name)
  SET(ACCESSIBILITY_LOCALIZATION_LOCALES en_US ko_KR ar_AE)
  SET(ACCESSIBILITY_LOCALIZATION_OUTPUTS)

  FOREACH(LOCALE ${ACCESSIBILITY_LOCALIZATION_LOCALES})
    SET(PO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/po/${LOCALE}.po")
    SET(MO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/locale/${LOCALE}/LC_MESSAGES")
    SET(MO_FILE "${MO_DIR}/${ACCESSIBILITY_LOCALIZATION_DOMAIN}.mo")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${MO_FILE}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${MO_DIR}
      COMMAND ${MSGFMT_EXECUTABLE} -o ${MO_FILE} ${PO_FILE}
      DEPENDS ${PO_FILE}
      COMMENT "Generating ${MO_FILE} from ${PO_FILE}"
      VERBATIM
    )
    LIST(APPEND ACCESSIBILITY_LOCALIZATION_OUTPUTS ${MO_FILE})
  ENDFOREACH()

  ADD_CUSTOM_TARGET(accessibility-localized-name-resources ALL
                    DEPENDS ${ACCESSIBILITY_LOCALIZATION_OUTPUTS})
  ADD_DEPENDENCIES(accessibility-localized-name.example
                   accessibility-localized-name-resources)
ELSE()
  MESSAGE(WARNING
    "msgfmt was not found; accessibility-localized-name.example will build, "
    "but untranslated resource IDs will be returned at runtime")
ENDIF()

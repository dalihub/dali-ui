CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)

PROJECT(text.example)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

IF(NOT COMMAND DALI_ADD_SAMPLE)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../common.cmake")
ENDIF()

DALI_ADD_SAMPLE(text.example text-example.cpp)
DALI_ADD_SAMPLE(text-layout-direction.example text-layout-direction-example.cpp)
DALI_ADD_SAMPLE(text-markup.example text-markup-example.cpp)
DALI_ADD_SAMPLE(text-marquee.example text-marquee-example.cpp)
DALI_ADD_SAMPLE(text-style.example text-style-example.cpp)
DALI_ADD_SAMPLE(text-style-clear.example text-style-clear-example.cpp)
DALI_ADD_SAMPLE(text-style-bevel.example text-style-bevel-example.cpp)
DALI_ADD_SAMPLE(text-fit.example text-fit-example.cpp)
DALI_ADD_SAMPLE(text-fit-candidate.example text-fit-candidate-example.cpp)
DALI_ADD_SAMPLE(text-scale.example text-scale-example.cpp)
DALI_ADD_SAMPLE(text-font-variation.example text-font-variation-example.cpp)
DALI_ADD_SAMPLE(text-cutout-mask.example text-cutout-mask-example.cpp)
DALI_ADD_SAMPLE(text-effect.example text-effect-example.cpp)
DALI_ADD_SAMPLE(text-gradient.example text-gradient-example.cpp)
DALI_ADD_SAMPLE(text-input-gradient.example text-input-gradient-example.cpp)
DALI_ADD_SAMPLE(text-styled-text.example text-styled-text-example.cpp)
DALI_ADD_SAMPLE(text-styled-text-builder.example text-styled-text-builder-example.cpp)
DALI_ADD_SAMPLE(text-styled-text-simple.example text-styled-text-simple-example.cpp)
DALI_ADD_SAMPLE(text-image-span-simple.example text-image-span-simple-example.cpp)
DALI_ADD_SAMPLE(text-gradient-simple.example text-gradient-simple-example.cpp)
DALI_ADD_SAMPLE(text-gradient-perf.example text-gradient-perf-example.cpp)
DALI_ADD_SAMPLE(text-colrv1.example text-colrv1-example.cpp)
DALI_ADD_SAMPLE(text-emoji.example text-emoji-example.cpp)
DALI_ADD_SAMPLE(text-render-scale.example text-render-scale-example.cpp)
DALI_ADD_SAMPLE(text-input-field.example text-input-field-example.cpp)
DALI_ADD_SAMPLE(text-clipboard.example text-clipboard-example.cpp)
DALI_ADD_SAMPLE(text-typing-style.example text-typing-style-example.cpp)
DALI_ADD_SAMPLE(text-input-editor.example text-input-editor-example.cpp)
DALI_ADD_SAMPLE(text-input-method-context.example text-input-method-context-example.cpp)
DALI_ADD_SAMPLE(text-localization.example text-localization-example.cpp)
DALI_ADD_SAMPLE(text-localization-po.example text-localization-po-example.cpp)
DALI_ADD_SAMPLE(text-localization-custom-component.example text-localization-custom-component-example.cpp)
DALI_ADD_SAMPLE(text-gradient-localization.example text-gradient-localization-example.cpp)
DALI_ADD_SAMPLE(text-image-span.example text-image-span-example.cpp)
DALI_ADD_SAMPLE(text-input-image-span.example text-input-image-span-example.cpp)
DALI_ADD_SAMPLE(text-image-span-perf.example text-image-span-perf-example.cpp)

# Generate runtime message catalogs when gettext is available.
FIND_PROGRAM(MSGFMT_EXECUTABLE msgfmt)
IF(MSGFMT_EXECUTABLE)
  SET(PO_MO_FILES)
  SET(TEXT_LOCALIZATION_LOCALES en_US ko_KR ar_AE)

  SET(TEXT_LOCALIZATION_DEFAULT_DOMAIN text-localization-po)
  SET(TEXT_LOCALIZATION_DEFAULT_PO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/po/default")
  SET(TEXT_LOCALIZATION_DEFAULT_LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/locale/default")
  FOREACH(LOCALE ${TEXT_LOCALIZATION_LOCALES})
    SET(PO_FILE "${TEXT_LOCALIZATION_DEFAULT_PO_DIR}/${LOCALE}.po")
    SET(MO_DIR "${TEXT_LOCALIZATION_DEFAULT_LOCALE_DIR}/${LOCALE}/LC_MESSAGES")
    SET(MO_FILE "${MO_DIR}/${TEXT_LOCALIZATION_DEFAULT_DOMAIN}.mo")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${MO_FILE}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${MO_DIR}
      COMMAND ${MSGFMT_EXECUTABLE} -o ${MO_FILE} ${PO_FILE}
      DEPENDS ${PO_FILE}
      COMMENT "Generating ${MO_FILE} from ${PO_FILE}"
    )
    LIST(APPEND PO_MO_FILES ${MO_FILE})
  ENDFOREACH()

  SET(TEXT_LOCALIZATION_ALT_DOMAIN text-localization-po-alt)
  SET(TEXT_LOCALIZATION_ALT_PO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/po/alternate")
  SET(TEXT_LOCALIZATION_ALT_LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/locale/alternate")
  FOREACH(LOCALE ${TEXT_LOCALIZATION_LOCALES})
    SET(PO_FILE "${TEXT_LOCALIZATION_ALT_PO_DIR}/${LOCALE}.po")
    SET(MO_DIR "${TEXT_LOCALIZATION_ALT_LOCALE_DIR}/${LOCALE}/LC_MESSAGES")
    SET(MO_FILE "${MO_DIR}/${TEXT_LOCALIZATION_ALT_DOMAIN}.mo")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${MO_FILE}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${MO_DIR}
      COMMAND ${MSGFMT_EXECUTABLE} -o ${MO_FILE} ${PO_FILE}
      DEPENDS ${PO_FILE}
      COMMENT "Generating ${MO_FILE} from ${PO_FILE}"
    )
    LIST(APPEND PO_MO_FILES ${MO_FILE})
  ENDFOREACH()

  SET(TEXT_GRADIENT_LOCALIZATION_DOMAIN text-gradient-localization-po)
  SET(TEXT_GRADIENT_LOCALIZATION_PO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/po/gradient")
  SET(TEXT_GRADIENT_LOCALIZATION_LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/locale/gradient")
  FOREACH(LOCALE ${TEXT_LOCALIZATION_LOCALES})
    SET(PO_FILE "${TEXT_GRADIENT_LOCALIZATION_PO_DIR}/${LOCALE}.po")
    SET(MO_DIR "${TEXT_GRADIENT_LOCALIZATION_LOCALE_DIR}/${LOCALE}/LC_MESSAGES")
    SET(MO_FILE "${MO_DIR}/${TEXT_GRADIENT_LOCALIZATION_DOMAIN}.mo")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${MO_FILE}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${MO_DIR}
      COMMAND ${MSGFMT_EXECUTABLE} -o ${MO_FILE} ${PO_FILE}
      DEPENDS ${PO_FILE}
      COMMENT "Generating ${MO_FILE} from ${PO_FILE}"
    )
    LIST(APPEND PO_MO_FILES ${MO_FILE})
  ENDFOREACH()

  ADD_CUSTOM_TARGET(text-localization-po-resources ALL DEPENDS ${PO_MO_FILES})
  ADD_DEPENDENCIES(text-localization-po.example text-localization-po-resources)
  ADD_DEPENDENCIES(text-localization-custom-component.example text-localization-po-resources)
  ADD_DEPENDENCIES(text-gradient-localization.example text-localization-po-resources)
ELSE()
  MESSAGE(WARNING "msgfmt not found; localization .mo files will not be generated")
ENDIF()

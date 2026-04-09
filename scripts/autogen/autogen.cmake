find_package(Python3 REQUIRED)

# Path to the code generator script (co-located with this cmake file)
set(DALI_UI_CHAIN_GENERATOR "${CMAKE_CURRENT_LIST_DIR}/gen-chaining-macro.py")

# @brief Registers headers for method chaining autogen.
# @param TARGET_NAME The name of the target (e.g., dali-ui-components)
# @param HEADER_DIR The root directory to scan headers from
function(dali_ui_autogen_chaining_macro TARGET_NAME HEADER_DIR)
  # 1. 대상 디렉토리의 모든 헤더 수집
  file(GLOB_RECURSE HEADERS "${HEADER_DIR}/*.h")

  set(AUTOGEN_STAMPS "")

  foreach(HEADER ${HEADERS})
    get_filename_component(HEADER_NAME ${HEADER} NAME_WLE)

    # 타임스탬프 파일을 빌드 디렉토리에 생성하여 소스 트리 오염 방지
    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/stamps/${HEADER_NAME}.stamp")

    add_custom_command(
      OUTPUT "${STAMP_FILE}"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/stamps"
      COMMAND Python3::Interpreter "${DALI_UI_CHAIN_GENERATOR}" "${HEADER}"
      COMMAND ${CMAKE_COMMAND} -E touch "${STAMP_FILE}"
      DEPENDS "${HEADER}" "${DALI_UI_CHAIN_GENERATOR}"
      COMMENT "Chaining Sync: ${HEADER_NAME} for ${TARGET_NAME}"
      VERBATIM
    )

    list(APPEND AUTOGEN_STAMPS "${STAMP_FILE}")
  endforeach()

  # 2. 타겟별로 독립적인 제너레이터 타겟 생성
  set(GEN_TARGET "${TARGET_NAME}_autogen")
  add_custom_target(${GEN_TARGET} DEPENDS ${AUTOGEN_STAMPS})
  add_dependencies(${TARGET_NAME} ${GEN_TARGET})
endfunction()

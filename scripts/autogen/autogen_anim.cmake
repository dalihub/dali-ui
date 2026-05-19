find_package(Python3 REQUIRED)

# Path to the animation code generator script (co-located with this cmake file)
set(DALI_UI_ANIMATION_GENERATOR "${CMAKE_CURRENT_LIST_DIR}/gen-animation-spec.py")

# Template files used by the generator. Evaluated at file scope so
# CMAKE_CURRENT_LIST_DIR resolves to this file's directory (not the caller's
# listfile dir when the function below is invoked).
set(DALI_UI_ANIMATION_TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/templates")
file(GLOB DALI_UI_ANIMATION_TEMPLATES "${DALI_UI_ANIMATION_TEMPLATE_DIR}/*.tmpl")

# @brief Registers a public-api directory for animation autogen.
# Scans all headers for @ANIMATION_CONFIG tags, resolves dependencies,
# and generates Bridge/Spec/Impl files.
# @param TARGET_NAME The name of the target (e.g., dali-ui-foundation)
# @param HEADER_DIR The root directory to scan headers from (e.g., public-api/)
function(dali_ui_autogen_animation_spec TARGET_NAME HEADER_DIR)
  # 대상 디렉토리의 모든 헤더 수집 (의존성 추적용)
  file(GLOB_RECURSE HEADERS "${HEADER_DIR}/*.h")

  set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/stamps/animation-spec.stamp")

  add_custom_command(
    OUTPUT "${STAMP_FILE}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/stamps"
    COMMAND Python3::Interpreter "${DALI_UI_ANIMATION_GENERATOR}" "--scan-dir" "${HEADER_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${STAMP_FILE}"
    DEPENDS ${HEADERS} "${DALI_UI_ANIMATION_GENERATOR}" ${DALI_UI_ANIMATION_TEMPLATES}
    COMMENT "Animation Spec Sync: scanning ${HEADER_DIR} for ${TARGET_NAME}"
    VERBATIM
  )

  set(GEN_TARGET "${TARGET_NAME}_autogen_anim")
  add_custom_target(${GEN_TARGET} DEPENDS "${STAMP_FILE}")
  add_dependencies(${TARGET_NAME} ${GEN_TARGET})
endfunction()

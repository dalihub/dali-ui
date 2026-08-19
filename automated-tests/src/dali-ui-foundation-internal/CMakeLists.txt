SET(PKG_NAME "dali-ui-foundation-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

# Keep the test source declarations at the top so the Windows runner can
# include this file in source-list-only mode without configuring Unix targets.
SET(TC_SOURCES
    utc-Dali-FocusManager-internal.cpp
    utc-Dali-ImageUrl-internal.cpp
    utc-Dali-InteractiveTrait-internal.cpp
    utc-Dali-InteractiveView-internal.cpp
    utc-Dali-SelectableTrait-internal.cpp
    utc-Dali-GradientFactory-internal.cpp
    utc-Dali-GradientVisualSemantics-internal.cpp
    utc-Dali-TextGradientBaseline-internal.cpp
    utc-Dali-TextGradientGlyphClassification-internal.cpp
    utc-Dali-MarkupToStyledText-internal.cpp
    utc-Dali-StyledTextApplier-internal.cpp
    utc-Dali-StyledTextController-internal.cpp
    utc-Dali-TextAlignment-internal.cpp
    utc-Dali-TextEllipsis-internal.cpp
    utc-Dali-ReplacementProjection-internal.cpp
    utc-Dali-ReplacementEditNormalizer-internal.cpp
    utc-Dali-TextGradientStyle-internal.cpp
    utc-Dali-TextAtlasGradientState-internal.cpp
    utc-Dali-TextGradientShaderComposition-internal.cpp
    utc-Dali-TextGradientTypesetterMask-internal.cpp
    utc-Dali-TextReveal-internal.cpp
    utc-Dali-TextGradientPropertyData-internal.cpp
    utc-Dali-TextGradientVisualMaskPlumbing-internal.cpp
    utc-Dali-TextVisualAsyncPublication-internal.cpp
    utc-Dali-WindowsWarningCoverage-internal.cpp
    utc-Dali-ViewAccessibility-internal.cpp
    utc-Dali-Accessibility-Text-internal.cpp
    utc-Dali-ImageView-internal.cpp
    utc-Dali-LottieAnimationView-internal.cpp
    utc-Dali-ViewFittingMode-internal.cpp
    utc-Dali-View-order-internal.cpp
)

SET(TC_SOURCE_LIST ${TC_SOURCES})

LIST(APPEND TC_SOURCES
    replacement-layout-test-adapter.cpp
    ../dali-test-suite-utils/mesh-builder.cpp
    ../dali-test-suite-utils/dali-test-suite-utils.cpp
    ../dali-test-suite-utils/test-actor-utils.cpp
    ../dali-test-suite-utils/test-custom-actor.cpp
    ../dali-test-suite-utils/test-harness.cpp
    ../dali-test-suite-utils/test-application.cpp
    ../dali-test-suite-utils/test-gesture-generator.cpp
    ../dali-test-suite-utils/test-gl-abstraction.cpp
    ../dali-test-suite-utils/test-graphics-buffer.cpp
    ../dali-test-suite-utils/test-graphics-command-buffer.cpp
    ../dali-test-suite-utils/test-graphics-controller.cpp
    ../dali-test-suite-utils/test-graphics-framebuffer.cpp
    ../dali-test-suite-utils/test-graphics-pipeline.cpp
    ../dali-test-suite-utils/test-graphics-program.cpp
    ../dali-test-suite-utils/test-graphics-reflection.cpp
    ../dali-test-suite-utils/test-graphics-shader.cpp
    ../dali-test-suite-utils/test-graphics-texture.cpp
    ../dali-test-suite-utils/test-graphics-sampler.cpp
    ../dali-test-suite-utils/test-graphics-sync-impl.cpp
    ../dali-test-suite-utils/test-graphics-sync-object.cpp
    ../dali-test-suite-utils/test-native-image.cpp
    ../dali-test-suite-utils/test-platform-abstraction.cpp
    ../dali-test-suite-utils/test-render-controller.cpp
    ../dali-test-suite-utils/test-render-surface.cpp
    ../dali-test-suite-utils/test-trace-call-stack.cpp
    ../dali-test-suite-utils/test-addon-manager.cpp
    ../dali-test-suite-utils/test-render-surface-if.cpp
    ../dali-test-suite-utils/dali-ui/ui-adaptor.cpp
    ../dali-test-suite-utils/dali-ui/ui-async-task-manager.cpp
    ../dali-test-suite-utils/dali-ui/ui-environment-variable.cpp
    ../dali-test-suite-utils/dali-ui/ui-event-thread-callback.cpp
    ../dali-test-suite-utils/dali-ui/ui-lifecycle-controller.cpp
    ../dali-test-suite-utils/dali-ui/ui-scene-holder.cpp
    ../dali-test-suite-utils/dali-ui/ui-test-application.cpp
    ../dali-test-suite-utils/dali-ui/ui-text-abstraction.cpp
    ../dali-test-suite-utils/dali-ui/ui-timer.cpp
    ../dali-test-suite-utils/dali-ui/ui-vector-animation-renderer.cpp
    ../dali-test-suite-utils/dali-ui/ui-window.cpp
)

IF(DALI_TEST_SOURCE_LIST_ONLY)
    RETURN()
ENDIF()

# This internal UTC target builds dali-ui-foundation sources directly.
# It does not link against libdali2-ui-foundation, because many internal
# symbols are hidden in the production shared library.
SET(FOUNDATION_ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../dali-ui-foundation)

# ---------------------------------------------------------
# 1. Shader Generator Configuration
# ---------------------------------------------------------
SET(SHADER_GENERATOR_NAME dali-shader-generator-internal)
SET(SHADER_GENERATOR_SOURCES ${FOUNDATION_ROOT_SRC_DIR}/shader-generator/shader-generator.cpp)

ADD_EXECUTABLE(${SHADER_GENERATOR_NAME} ${SHADER_GENERATOR_SOURCES})

SET(SHADER_SOURCE_DIR "${FOUNDATION_ROOT_SRC_DIR}/internal/graphics/shaders")
SET(SHADER_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
SET(BUILT_IN_SHADER_GEN_CPP "${SHADER_GENERATED_DIR}/builtin-shader-gen.cpp")

FILE(MAKE_DIRECTORY ${SHADER_GENERATED_DIR})

FILE(GLOB SHADERS_SRC
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${BUILT_IN_SHADER_GEN_CPP}
    DEPENDS ${SHADER_GENERATOR_NAME} ${SHADERS_SRC}
    COMMAND ${SHADER_GENERATOR_NAME} ${SHADER_SOURCE_DIR} ${SHADER_GENERATED_DIR}
    COMMENT "Generating built-in shader source for foundation internal UTC"
)

ADD_CUSTOM_TARGET(generate_builtin_shaders_internal
    DEPENDS ${BUILT_IN_SHADER_GEN_CPP}
)

# ---------------------------------------------------------
# 2. Collect Foundation Library Source Files
# ---------------------------------------------------------
FILE(GLOB_RECURSE ALL_LIBRARY_SOURCES "${FOUNDATION_ROOT_SRC_DIR}/*.cpp")

SET(LIBRARY_SOURCES "")

FOREACH(SRC ${ALL_LIBRARY_SOURCES})
    # Exclude the shader generator itself and any generated shader cpp from
    # the source tree. This internal UTC target uses its own generated shader
    # cpp under the build directory.
    IF(NOT SRC MATCHES "shader-generator.cpp" AND NOT SRC MATCHES "builtin-shader-gen.cpp")
        LIST(APPEND LIBRARY_SOURCES ${SRC})
    ENDIF()
ENDFOREACH()

# Add shader source generated for this internal UTC target.
LIST(APPEND LIBRARY_SOURCES ${BUILT_IN_SHADER_GEN_CPP})

# ---------------------------------------------------------
# 5. Package Dependencies
# ---------------------------------------------------------
PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)

# ---------------------------------------------------------
# 6. Compiler Options
# ---------------------------------------------------------
ADD_COMPILE_OPTIONS(-rdynamic)
ADD_COMPILE_OPTIONS(-O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED)
ADD_COMPILE_OPTIONS(${DALICORE_CFLAGS_OTHER})
ADD_COMPILE_OPTIONS(${DALIADAPTOR_CFLAGS_OTHER})

FOREACH(directory ${DALICORE_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${DALICORE_LIBRARY_DIRS})

FOREACH(directory ${DALIADAPTOR_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${DALIADAPTOR_LIBRARY_DIRS})

STRING(STRIP "${CMAKE_CXX_LINK_FLAGS}" CMAKE_CXX_LINK_FLAGS)

# ---------------------------------------------------------
# 7. Include Directories
# ---------------------------------------------------------
INCLUDE_DIRECTORIES(
    ../../../
    ${FOUNDATION_ROOT_SRC_DIR}
    ${FOUNDATION_ROOT_SRC_DIR}/third-party
    ${FOUNDATION_ROOT_SRC_DIR}/internal
    ${FOUNDATION_ROOT_SRC_DIR}/integration-api
    ${DALICORE_INCLUDE_DIRS}
    ${DALIADAPTOR_INCLUDE_DIRS}
    ../dali-test-suite-utils
    ../dali-test-suite-utils/dali-ui
)

# ---------------------------------------------------------
# 8. Resource Directory Definitions
#    These definitions are normally provided by the main dali-ui build.
#    This internal UTC target compiles foundation sources directly, so it
#    defines the same resource path macros here.
# ---------------------------------------------------------
IF(DEFINED ENV{DALI_DATA_RO_DIR})
    SET(dataReadOnlyDir $ENV{DALI_DATA_RO_DIR})
ELSE()
    SET(dataReadOnlyDir ${CMAKE_INSTALL_PREFIX}/share/dali)
ENDIF()

ADD_COMPILE_DEFINITIONS(
    DALI_IMAGE_DIR="${dataReadOnlyDir}/ui/images/"
    DALI_DATA_READ_ONLY_DIR="${dataReadOnlyDir}"
    DALI_SOUND_DIR="${dataReadOnlyDir}/toolkit/sounds/"
    DALI_STYLE_DIR="${dataReadOnlyDir}/toolkit/styles/"
    DALI_STYLE_IMAGE_DIR="${dataReadOnlyDir}/toolkit/styles/images/"
    DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)

ADD_DEFINITIONS(-DADDON_LIBS_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\")

# The test harness may try to read addons.txt. This internal UTC target does
# not build addons, so provide an empty addon list.
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/addons.txt "")

# ---------------------------------------------------------
# 9. Generate Test Table Header
# ---------------------------------------------------------
ADD_CUSTOM_COMMAND(
    COMMAND ../../scripts/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCE_LIST}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT ${EXEC_NAME}.h
    COMMENT "Generating test tables for ${EXEC_NAME}"
)

# ---------------------------------------------------------
# 10. Build Internal UTC Executable
# ---------------------------------------------------------
SET(ALL_SOURCES
    ${LIBRARY_SOURCES}
    ${TC_SOURCES}
)

ADD_EXECUTABLE(${EXEC_NAME}
    ${EXEC_NAME}.h
    ${EXEC_NAME}.cpp
    ${ALL_SOURCES}
)

ADD_DEPENDENCIES(${EXEC_NAME} generate_builtin_shaders_internal)

TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
    -rdynamic
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

# ---------------------------------------------------------
# 11. Manual Unicode Emoji Sweep Tool
#     This is intentionally excluded from the default build/test path.
#     Build explicitly with:
#       cmake --build <build-dir> --target dali-emoji-sequence-sweep
# ---------------------------------------------------------
SET(EMOJI_SEQUENCE_SWEEP_NAME dali-emoji-sequence-sweep)
SET(EMOJI_SEQUENCE_SWEEP_SOURCES
    ${LIBRARY_SOURCES}
    emoji-sequence-sweep-manual.cpp
)

ADD_EXECUTABLE(${EMOJI_SEQUENCE_SWEEP_NAME} EXCLUDE_FROM_ALL
    ${EMOJI_SEQUENCE_SWEEP_SOURCES}
)

ADD_DEPENDENCIES(${EMOJI_SEQUENCE_SWEEP_NAME} generate_builtin_shaders_internal)

TARGET_LINK_LIBRARIES(${EMOJI_SEQUENCE_SWEEP_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
    -rdynamic
)

# ---------------------------------------------------------
# 12. Manual Unicode Emoji Visual Viewer
#     This is intentionally excluded from the default build/test path.
#     Build explicitly with:
#       cmake --build <build-dir> --target dali-emoji-sequence-visual
# ---------------------------------------------------------
SET(EMOJI_SEQUENCE_VISUAL_NAME dali-emoji-sequence-visual)
SET(EMOJI_SEQUENCE_VISUAL_SOURCES
    ${LIBRARY_SOURCES}
    emoji-sequence-visual-manual.cpp
)

ADD_EXECUTABLE(${EMOJI_SEQUENCE_VISUAL_NAME} EXCLUDE_FROM_ALL
    ${EMOJI_SEQUENCE_VISUAL_SOURCES}
)

ADD_DEPENDENCIES(${EMOJI_SEQUENCE_VISUAL_NAME} generate_builtin_shaders_internal)

TARGET_LINK_LIBRARIES(${EMOJI_SEQUENCE_VISUAL_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
    -rdynamic
)

# ---------------------------------------------------------
# 13. Manual Unicode Emoji Label Performance Tool
#     This is intentionally excluded from the default build/test path.
#     Build explicitly with:
#       cmake --build <build-dir> --target dali-emoji-label-perf
# ---------------------------------------------------------
SET(EMOJI_LABEL_PERF_NAME dali-emoji-label-perf)
SET(EMOJI_LABEL_PERF_SOURCES
    ${LIBRARY_SOURCES}
    emoji-label-perf-manual.cpp
)

ADD_EXECUTABLE(${EMOJI_LABEL_PERF_NAME} EXCLUDE_FROM_ALL
    ${EMOJI_LABEL_PERF_SOURCES}
)

ADD_DEPENDENCIES(${EMOJI_LABEL_PERF_NAME} generate_builtin_shaders_internal)

TARGET_LINK_LIBRARIES(${EMOJI_LABEL_PERF_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
    -rdynamic
)

# ---------------------------------------------------------
# 14. Replacement real text-abstraction diagnostic
#     The normal UTC executable intentionally interposes lightweight text
#     abstraction fakes. This separate target omits that harness so Hebrew,
#     bidi reorder and END ellipsis exercise the production Adaptor services.
#     Build explicitly with:
#       cmake --build <build-dir> --target dali-replacement-real-layout-diagnostic
# ---------------------------------------------------------
SET(REPLACEMENT_REAL_LAYOUT_NAME dali-replacement-real-layout-diagnostic)

ADD_EXECUTABLE(${REPLACEMENT_REAL_LAYOUT_NAME} EXCLUDE_FROM_ALL
    ${LIBRARY_SOURCES}
    replacement-layout-test-adapter.cpp
    replacement-real-layout-diagnostic.cpp
)

ADD_DEPENDENCIES(${REPLACEMENT_REAL_LAYOUT_NAME} generate_builtin_shaders_internal)

TARGET_LINK_LIBRARIES(${REPLACEMENT_REAL_LAYOUT_NAME}
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
    -rdynamic
)

# ---------------------------------------------------------
# 15. Ordinary ellipsis performance tool
#     This target links the installed production library and is intentionally
#     excluded from the normal UTC build.
# ---------------------------------------------------------
SET(ORDINARY_ELLIPSIS_PERF_NAME dali-ordinary-ellipsis-perf)

ADD_EXECUTABLE(${ORDINARY_ELLIPSIS_PERF_NAME} EXCLUDE_FROM_ALL
    ordinary-ellipsis-perf-manual.cpp
)

TARGET_LINK_LIBRARIES(${ORDINARY_ELLIPSIS_PERF_NAME}
    dali2-ui-foundation
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    -lpthread
    -ldl
    --coverage
)

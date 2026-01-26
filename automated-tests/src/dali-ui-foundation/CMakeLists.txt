SET(PKG_NAME "dali-ui-foundation")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali2-ui-foundation")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
  utc-Dali-View.cpp
  utc-Dali-Unit.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_SOURCES
  dali-ui-foundation-test-utils/ui-foundation-test-application.cpp
  dali-ui-foundation-test-utils/ui-foundation-adaptor.cpp
  dali-ui-foundation-test-utils/ui-foundation-application.cpp
  dali-ui-foundation-test-utils/ui-foundation-async-task-manager.cpp
  dali-ui-foundation-test-utils/ui-foundation-canvas-renderer.cpp
  dali-ui-foundation-test-utils/ui-foundation-clipboard.cpp
  dali-ui-foundation-test-utils/ui-foundation-direct-rendering-egl.cpp
  dali-ui-foundation-test-utils/ui-foundation-event-thread-callback.cpp
  dali-ui-foundation-test-utils/ui-foundation-environment-variable.cpp
  #dali-ui-foundation-test-utils/ui-foundation-icu.cpp
  dali-ui-foundation-test-utils/ui-foundation-input-method-context.cpp
  dali-ui-foundation-test-utils/ui-foundation-input-method-options.cpp
  dali-ui-foundation-test-utils/ui-foundation-lifecycle-controller.cpp
  dali-ui-foundation-test-utils/ui-foundation-orientation.cpp
  dali-ui-foundation-test-utils/ui-foundation-physical-keyboard.cpp
  dali-ui-foundation-test-utils/ui-foundation-style-monitor.cpp
  dali-ui-foundation-test-utils/ui-foundation-texture-upload-manager.cpp
  dali-ui-foundation-test-utils/ui-foundation-timer.cpp
  dali-ui-foundation-test-utils/ui-foundation-trigger-event-factory.cpp
  dali-ui-foundation-test-utils/ui-foundation-tts-player.cpp
  dali-ui-foundation-test-utils/ui-foundation-native-image-source.cpp
  dali-ui-foundation-test-utils/ui-foundation-vector-animation-renderer.cpp
  dali-ui-foundation-test-utils/ui-foundation-vector-image-renderer.cpp
  #dali-ui-foundation-test-utils/ui-foundation-video-player.cpp
  dali-ui-foundation-test-utils/ui-foundation-web-engine.cpp
  dali-ui-foundation-test-utils/ui-foundation-window.cpp
  dali-ui-foundation-test-utils/ui-foundation-scene-holder.cpp
  dali-ui-foundation-test-utils/dali-test-suite-utils.cpp
  dali-ui-foundation-test-utils/dali-ui-foundation-test-suite-utils.cpp
  #dali-ui-foundation-test-utils/dummy-control.cpp
  dali-ui-foundation-test-utils/mesh-builder.cpp
  dali-ui-foundation-test-utils/test-actor-utils.cpp
  dali-ui-foundation-test-utils/test-addon-manager.cpp
  dali-ui-foundation-test-utils/test-animation-data.cpp
  dali-ui-foundation-test-utils/test-application.cpp
  dali-ui-foundation-test-utils/test-button.cpp
  dali-ui-foundation-test-utils/test-encoded-image-buffer.cpp
  dali-ui-foundation-test-utils/test-gesture-generator.cpp
  dali-ui-foundation-test-utils/test-gl-abstraction.cpp
  dali-ui-foundation-test-utils/test-graphics-sync-impl.cpp
  dali-ui-foundation-test-utils/test-graphics-sync-object.cpp
  dali-ui-foundation-test-utils/test-graphics-buffer.cpp
  dali-ui-foundation-test-utils/test-graphics-command-buffer.cpp
  dali-ui-foundation-test-utils/test-graphics-controller.cpp
  dali-ui-foundation-test-utils/test-graphics-framebuffer.cpp
  dali-ui-foundation-test-utils/test-graphics-texture.cpp
  dali-ui-foundation-test-utils/test-graphics-pipeline.cpp
  dali-ui-foundation-test-utils/test-graphics-program.cpp
  dali-ui-foundation-test-utils/test-graphics-reflection.cpp
  dali-ui-foundation-test-utils/test-graphics-sampler.cpp
  dali-ui-foundation-test-utils/test-graphics-shader.cpp
  dali-ui-foundation-test-utils/test-platform-abstraction.cpp
  dali-ui-foundation-test-utils/test-render-controller.cpp
  dali-ui-foundation-test-utils/test-render-surface.cpp
  dali-ui-foundation-test-utils/test-trace-call-stack.cpp
  dali-ui-foundation-test-utils/test-native-image.cpp
  dali-ui-foundation-test-utils/test-harness.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
  dali2-core
  dali2-adaptor
  dali2-toolkit
  dali2-ui-foundation
  glesv2
  egl
)

ADD_COMPILE_OPTIONS(
  -O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED
)

ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
  ../../../
  ${${CAPI_LIB}_INCLUDE_DIRS}
  dali-ui-foundation-test-utils
)

ADD_CUSTOM_COMMAND(
  COMMAND ${SCRIPT_DIR}/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT ${EXEC_NAME}.h
  COMMENT "Generating test tables"
)

ADD_EXECUTABLE(${EXEC_NAME}
  ${EXEC_NAME}.h
  ${EXEC_NAME}.cpp
  ${TC_SOURCES}
  ${TEST_HARNESS_SOURCES}
)

TARGET_LINK_LIBRARIES(${EXEC_NAME}
  -Wl,--no-as-needed # Force linking to library even if linker thinks we don't need it, this ensures EGL libraries are loaded at test startup
  ${${CAPI_LIB}_LIBRARIES}
  -lpthread --coverage -ldl
)

INSTALL(PROGRAMS ${EXEC_NAME}
  DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

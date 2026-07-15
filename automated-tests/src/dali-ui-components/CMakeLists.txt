SET(PKG_NAME "dali-ui-components")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-ui-components")

SET(TC_SOURCES
    utc-Dali-Components.cpp
    utc-Dali-DummyComponent.cpp
    utc-Dali-TextButton.cpp
    utc-Dali-CheckBox.cpp
    utc-Dali-ChartView.cpp
    utc-Dali-ChartSeries.cpp
    utc-Dali-PieSeries.cpp
    utc-Dali-ChartAxis.cpp
    utc-Dali-ChartSection.cpp
    utc-Dali-Navigator.cpp
    utc-Dali-Dialog.cpp
    utc-Dali-DialogContainer.cpp
    utc-Dali-AlertDialog.cpp
)

LIST(APPEND TC_SOURCES
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
    ../dali-test-suite-utils/dali-ui/ui-canvas-renderer.cpp
    ../dali-test-suite-utils/dali-ui/ui-environment-variable.cpp
    ../dali-test-suite-utils/dali-ui/ui-event-thread-callback.cpp
    ../dali-test-suite-utils/dali-ui/ui-lifecycle-controller.cpp
    ../dali-test-suite-utils/dali-ui/ui-scene-holder.cpp
    ../dali-test-suite-utils/dali-ui/ui-test-application.cpp
    ../dali-test-suite-utils/dali-ui/ui-text-abstraction.cpp
    ../dali-test-suite-utils/dali-ui/ui-timer.cpp
    ../dali-test-suite-utils/dali-ui/ui-vector-animation-renderer.cpp
    ../dali-test-suite-utils/dali-ui/ui-vector-image-renderer.cpp
    ../dali-test-suite-utils/dali-ui/ui-window.cpp
)

ADD_LIBRARY(dali-ui-components-test-md4c STATIC
    ../../../third-party/md4c/entity.c
    ../../../third-party/md4c/md4c.c
)
SET_TARGET_PROPERTIES(dali-ui-components-test-md4c
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)
TARGET_INCLUDE_DIRECTORIES(dali-ui-components-test-md4c
    PRIVATE
    ../../../third-party/md4c
)
TARGET_COMPILE_OPTIONS(dali-ui-components-test-md4c
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-fvisibility=hidden>
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali2-core
     dali2-adaptor
     dali2-ui-foundation
     dali2-ui-components
)

ADD_COMPILE_OPTIONS( -rdynamic )
ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror )
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
  ../../../
  .
  ${${CAPI_LIB}_INCLUDE_DIRS}
  ../dali-test-suite-utils
  ../dali-test-suite-utils/dali-ui
)

ADD_DEFINITIONS( -DADDON_LIBS_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\" )

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    dali-ui-components-test-md4c
    -lpthread -ldl --coverage -rdynamic
)

# Path to installed precompiled addons

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

# build addons
MESSAGE( STATUS "BINDIR: ${CMAKE_CURRENT_BINARY_DIR}")
SET(ADDON_NAME SampleAddOn )
SET(ADDON_SOURCES test-sample-addon.cpp )
ADD_LIBRARY( ${ADDON_NAME} SHARED ${ADDON_SOURCES} )
TARGET_LINK_LIBRARIES(${ADDON_NAME}
    -lpthread -ldl --coverage
)

INSTALL( TARGETS ${ADDON_NAME} DESTINATION ${BIN_DIR} )

# store AddOn list
FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/addons.txt lib${ADDON_NAME}.so )

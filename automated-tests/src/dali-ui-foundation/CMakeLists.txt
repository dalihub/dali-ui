SET(PKG_NAME "dali-ui-foundation")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-ui-foundation")

SET(TC_SOURCES
        utc-Dali-UniqueAny.cpp
        utc-Dali-UiStyle.cpp
        utc-Dali-ScrollView.cpp
        utc-Dali-ScrollBar.cpp
        utc-Dali-AnimatedImageVisual.cpp
        utc-Dali-BorderVisual.cpp
        utc-Dali-Unit.cpp
        utc-Dali-FocusManager.cpp
        utc-Dali-View.cpp
        utc-Dali-ViewLayoutBoundary.cpp
        utc-Dali-Label.cpp
        utc-Dali-LabelTextGradient.cpp
        utc-Dali-InputField.cpp
        utc-Dali-InputEditor.cpp
        utc-Dali-TextValueTypes.cpp
        utc-Dali-StyledText.cpp
        utc-Dali-ImageView.cpp
        utc-Dali-ImageVisual.cpp
        utc-Dali-RenderEffect.cpp
        utc-Dali-AnimatedImageView.cpp
        utc-Dali-LottieAnimationView.cpp
        utc-Dali-LottieAnimationVisual.cpp
        utc-Dali-UiColor.cpp
        utc-Dali-UiColorManager.cpp
        utc-Dali-UiLocalizationManager.cpp
        utc-Dali-UiThemeManager.cpp
        utc-Dali-InteractiveTrait.cpp
        utc-Dali-InteractiveView.cpp
        utc-Dali-StateEffect.cpp
        utc-Dali-SelectableTrait.cpp
        utc-Dali-GroupSelectableTrait.cpp
        utc-Dali-SelectionGroup.cpp
        utc-Dali-ViewState.cpp
        utc-Dali-ViewStateHandler.cpp
        utc-Dali-ViewState.cpp
        utc-Dali-Layout.cpp
        utc-Dali-StackLayout.cpp
        utc-Dali-FlexLayout.cpp
        utc-Dali-GridLayout.cpp
        utc-Dali-GradientValue.cpp
        utc-Dali-GradientVisual.cpp
        utc-Dali-AbsoluteLayout.cpp
        utc-Dali-LayoutController.cpp
        utc-Dali-LayoutTransition.cpp
        utc-Dali-LayoutTypes.cpp
        utc-Dali-FontVariation.cpp
        utc-Dali-Shadow.cpp
        utc-Dali-ColorVisual.cpp
        utc-Dali-TextVisual.cpp
        utc-Dali-VisualBase.cpp
        utc-Dali-WebView.cpp
        utc-Dali-CanvasView.cpp
)

ADD_CUSTOM_COMMAND(
    COMMAND ../../scripts/tcheadgen.sh ${EXEC_NAME}.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT ${EXEC_NAME}.h
    DEPENDS ${TC_SOURCES} CMakeLists.txt ../../scripts/tcheadgen.sh ../../scripts/retriever.sh
    COMMENT "Generating test tables for ${EXEC_NAME}"
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
    ../dali-test-suite-utils/dali-ui/ui-environment-variable.cpp
    ../dali-test-suite-utils/dali-ui/ui-event-thread-callback.cpp
    ../dali-test-suite-utils/dali-ui/ui-lifecycle-controller.cpp
    ../dali-test-suite-utils/dali-ui/ui-scene-holder.cpp
    ../dali-test-suite-utils/dali-ui/ui-test-application.cpp
    ../dali-test-suite-utils/dali-ui/ui-text-abstraction.cpp
    ../dali-test-suite-utils/dali-ui/ui-timer.cpp
    ../dali-test-suite-utils/dali-ui/ui-window.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
     dali2-core
     dali2-adaptor
     dali2-ui-foundation
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

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
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

#pragma once

/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "window-frame-interaction-controller.h"
#include "window-frame-layout-engine.h"
#include "window-frame-overlay-controller.h"
#include "window-frame-presentation-controller.h"
#include "window-frame-state-controller.h"
#include "window-frame-view-tree.h"
#include "window-operation-adapter.h"

#include <dali-ui-components/public-api/window/window-frame.h>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/adaptor-framework/window.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Dali::Ui
{
namespace Internal
{

class WindowFrameImpl : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  WindowFrameImpl(Dali::Window                            window,
                  WindowFrame::CloseCallback              closeCallback,
                  const WindowFrameOptions&               options,
                  std::unique_ptr<WindowOperationAdapter> windowOperations = {});

  WindowFrameImpl(const WindowFrameImpl&)            = delete;
  WindowFrameImpl(WindowFrameImpl&&)                 = delete;
  WindowFrameImpl& operator=(const WindowFrameImpl&) = delete;
  WindowFrameImpl& operator=(WindowFrameImpl&&)      = delete;

  bool Attach();
  void Detach();
  bool IsAttached() const;

  Dali::Ui::View                  GetContentRoot() const;
  const WindowFrameFeatureConfig& GetFeatureConfig() const;
  bool                            IsFeatureEnabled(WindowFrameFeature feature) const;
  WindowFrame::WindowState        GetWindowState() const;
  bool                            IsMaximized() const;
  bool                            IsMinimized() const;
  WindowFrameGeometry             GetGeometry() const;

  /**
   * @brief Returns the decoration insets a layout actually produced.
   */
  static Dali::Ui::Insets GetLaidOutDecorationInsets(const WindowFrameLayoutResult& layout);

  WindowFrameDecorationResult SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
  WindowFrameDecorationResult SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
  void                        SetDecorationLayout(const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
  void                        ClearDecoration();
  void                        SetDecorationVisible(bool visible);
  bool                        IsDecorationVisible() const;

  void                    SetFrameStyle(const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
  const WindowFrameStyle& GetFrameStyle() const;

  void                               SetOverlayEnabled(bool enabled);
  bool                               IsOverlayEnabled() const;
  void                               SetOverlayAutoHideDelay(uint32_t milliseconds);
  uint32_t                           GetOverlayAutoHideDelay() const;
  void                               ShowOverlayTemporarily();
  bool                               IsOverlayAutoHidden() const;
  void                               SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy policy);
  WindowFrameInteractiveResizePolicy GetInteractiveResizePolicy() const;

  bool          SetMinimumFrameSize(const Dali::Vector2& size);
  bool          SetMaximumFrameSize(const Dali::Vector2& size);
  void          ClearMinimumFrameSize();
  void          ClearMaximumFrameSize();
  bool          GetMinimumFrameSize(Dali::Vector2& size) const;
  bool          GetMaximumFrameSize(Dali::Vector2& size) const;
  bool          RequestFrameResize(const Dali::Vector2& size);
  bool          RequestContentResize(const Dali::Vector2& size);
  Dali::Vector2 GetFrameSize() const;
  Dali::Vector2 GetContentSize() const;

  WindowFrameCommandResult RequestMinimize();
  WindowFrameCommandResult RequestMaximizedState(bool maximize);
  WindowFrameCommandResult ToggleMaximize();
  WindowFrameCommandResult RequestClose();

  void SetCommandInterceptor(WindowFrame::CommandInterceptor interceptor);

  WindowFrame::WindowStateChangedSignalType&          WindowStateChangedSignal();
  WindowFrame::WindowStatePresentedSignalType&        WindowStatePresentedSignal();
  WindowFrame::GeometryChangedSignalType&             GeometryChangedSignal();
  WindowFrame::GeometryChangeCompletedSignalType&     GeometryChangeCompletedSignal();
  WindowFrame::DecorationVisibilityChangedSignalType& DecorationVisibilityChangedSignal();
  WindowFrame::InteractionChangedSignalType&          InteractionChangedSignal();
  WindowFrame::CommandProcessedSignalType&            CommandProcessedSignal();

protected:
  ~WindowFrameImpl() override;

private:
  std::optional<WindowFrame::WindowState> ResolveInitialWindowState();
  void                                    SynchronizeWindowState();
  void                                    UpdateStateDependentVisuals();
  std::optional<Dali::Vector2>            GetMinimumSurfaceSize() const;
  std::optional<Dali::Vector2>            GetMaximumSurfaceSize() const;
  Dali::Ui::Insets                        GetNormalShadowOutsets() const;
  bool                                    AreSizeConstraintsValid() const;
  Dali::Vector2                           ClampSurfaceSize(const Dali::Vector2& size) const;
  void                                    ApplyContentSize(const Dali::Vector2& size);
  void                                    ScheduleContentSize(const Dali::Vector2& size);
  Dali::Vector2                           GetContentSizeForPreservation() const;
  void                                    ConnectDecorationInput();
  void                                    ConnectWindowSignals();
  void                                    ApplyLayout();
  void                                    ApplyNativeSizeConstraint(const std::optional<Dali::Vector2>& surfaceSize, bool& applied, void (Dali::Window::*setter)(Dali::Window::WindowSize));
  void                                    ApplySizeConstraints();
  void                                    ClearNativeSizeConstraints();
  void                                    ApplyDecorationVisibility();
  void                                    ApplyResizeHandleState();
  void                                    ApplyResizeAspectRatioPolicy();
  void                                    RestoreResizeAspectRatioPolicy();
  void                                    ApplyMoveRegionState();
  void                                    ApplyWindowControlState();
  void                                    DisconnectDecorationInput();
  void                                    ApplyOverlayStateUpdate(const OverlayStateUpdate& update);
  void                                    StartOverlayAutoHideTimer();
  void                                    StopOverlayAutoHideTimer();
  void                                    EmitInteraction(const std::optional<WindowFrameInteraction>& interaction);
  void                                    CancelInteraction();
  void                                    LogEvent(const char* event);
  void                                    EmitWindowStateChanged(const std::optional<WindowFrame::WindowState>& state);
  void                                    RequestWindowStatePresentation(const std::optional<WindowFrame::WindowState>& state);
  void                                    EmitGeometryChangedIfNeeded();
  void                                    EmitGeometryChangeCompleted();
  void                                    EmitDecorationVisibilityChangedIfNeeded();
  WindowFrameCommandResult                EmitCommandProcessed(const WindowFrameCommandRequest& request, WindowFrameCommandResult result);
  WindowFrameCommandDisposition           InvokeCommandInterceptor(const WindowFrameCommandRequest& request);
  WindowFrameCommandResult                RequestMove();
  WindowFrameCommandResult                RequestResize(Dali::WindowResizeDirection direction);

  bool OnMoveRegionTouched(Dali::Actor actor, Dali::TouchEvent event);
  bool OnResizeHandleTouched(Dali::WindowResizeDirection direction, Dali::TouchEvent event);
  bool OnBottomLeftResizeHandleTouched(Dali::Actor actor, Dali::TouchEvent event);
  bool OnBottomRightResizeHandleTouched(Dali::Actor actor, Dali::TouchEvent event);
  void OnMinimizeControlClicked(Dali::Ui::View view, Dali::Ui::InputEvent event);
  void OnMaximizeRestoreControlClicked(Dali::Ui::View view, Dali::Ui::InputEvent event);
  void OnCloseControlClicked(Dali::Ui::View view, Dali::Ui::InputEvent event);
  bool OnOverlayAutoHideTimerTick();
  bool OnDeferredResizeTimerTick();
  bool OnSurfaceInterceptTouched(Dali::Actor actor, Dali::TouchEvent event);
  void OnWindowResized(Dali::Window window, Dali::Window::WindowSize size);
  void OnWindowMoved(Dali::Window window, Dali::Window::WindowPosition position);
  void OnMoveCompleted(Dali::Window window, Dali::Window::WindowPosition position);
  void OnResizeCompleted(Dali::Window window, Dali::Window::WindowSize size);
  void OnFocusChanged(Dali::Window window, bool focused);
  void OnVisibilityChanged(Dali::Window window, bool visible);

private:
  Dali::Window                                       mWindow;
  std::unique_ptr<WindowOperationAdapter>            mWindowOperations;
  WindowFrameStateController                         mWindowState;
  std::optional<Dali::Vector2>                       mInitialRestoreFrameSize;
  WindowFrame::WindowStateChangedSignalType          mWindowStateChangedSignal;
  WindowFrame::WindowStatePresentedSignalType        mWindowStatePresentedSignal;
  WindowFrame::GeometryChangedSignalType             mGeometryChangedSignal;
  WindowFrame::GeometryChangeCompletedSignalType     mGeometryChangeCompletedSignal;
  WindowFrame::DecorationVisibilityChangedSignalType mDecorationVisibilityChangedSignal;
  WindowFrame::InteractionChangedSignalType          mInteractionChangedSignal;
  WindowFrame::CommandProcessedSignalType            mCommandProcessedSignal;
  WindowFrameViewTree                                mFrame;
  WindowFrameLayout                                  mLayout;
  WindowFrameStyle                                   mFrameStyle;
  std::optional<Dali::Vector2>                       mMinimumFrameSize;
  std::optional<Dali::Vector2>                       mMaximumFrameSize;
  WindowFrameInteractiveResizePolicy                 mInteractiveResizePolicy{WindowFrameInteractiveResizePolicy::FREE};
  WindowFrame::CommandInterceptor                    mCommandInterceptor;
  WindowFrameInteractionController                   mInteractionController;
  WindowFrameOverlayController                       mOverlayController;
  WindowFramePresentationController                  mPresentationController;
  Dali::Timer                                        mOverlayAutoHideTimer;
  Dali::Timer                                        mDeferredResizeTimer;
  std::optional<Dali::Vector2>                       mDeferredContentSize;
  bool                                               mAttached{false};
  bool                                               mNativeMinimumConstraintApplied{false};
  bool                                               mNativeMaximumConstraintApplied{false};
  std::optional<WindowFrameGeometry>                 mLastGeometry;
  std::optional<bool>                                mLastDecorationVisibility;
};

} // namespace Internal

inline Internal::WindowFrameImpl& GetImplementation(WindowFrame& handle)
{
  DALI_ASSERT_ALWAYS(handle && "WindowFrame handle is empty");
  return static_cast<Internal::WindowFrameImpl&>(handle.GetBaseObject());
}

inline const Internal::WindowFrameImpl& GetImplementation(const WindowFrame& handle)
{
  DALI_ASSERT_ALWAYS(handle && "WindowFrame handle is empty");
  return static_cast<const Internal::WindowFrameImpl&>(handle.GetBaseObject());
}

} // namespace Dali::Ui

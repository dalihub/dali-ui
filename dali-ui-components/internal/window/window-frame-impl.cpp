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

#include "window-frame-impl.h"
#include "window-frame-decoration-validator.h"
#include "window-frame-layout-engine.h"
#include "window-frame-style-resolver.h"

#include <dali/integration-api/debug.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

namespace Dali::Ui
{
namespace Internal
{
namespace
{
Dali::Vector2 ClampSize(const Dali::Vector2& value, const std::optional<Dali::Vector2>& minimum, const std::optional<Dali::Vector2>& maximum)
{
  Dali::Vector2 result(std::max(0.0f, value.width), std::max(0.0f, value.height));
  if(maximum)
  {
    result.width  = std::min(result.width, maximum->width);
    result.height = std::min(result.height, maximum->height);
  }
  if(minimum)
  {
    result.width  = std::max(result.width, minimum->width);
    result.height = std::max(result.height, minimum->height);
  }
  return result;
}

Dali::Window::WindowSize ToWindowSize(const Dali::Vector2& size)
{
  return Dali::Window::WindowSize(static_cast<int32_t>(std::ceil(std::max(0.0f, size.width))),
                                  static_cast<int32_t>(std::ceil(std::max(0.0f, size.height))));
}

using WindowFrameAttachmentRegistry = std::map<const Dali::BaseObject*, WindowFrameImpl*>;

WindowFrameAttachmentRegistry& GetWindowFrameAttachmentRegistry()
{
  // WindowFrame is event-thread only. Keying by the window body keeps copied
  // Dali::Window handles in the same ownership domain.
  static WindowFrameAttachmentRegistry registry;
  return registry;
}

bool TryRegisterAttachedFrame(Dali::Window window, WindowFrameImpl* frame)
{
  const Dali::BaseObject* key = &window.GetBaseObject();
  auto [iterator, inserted]   = GetWindowFrameAttachmentRegistry().emplace(key, frame);
  return inserted || iterator->second == frame;
}

void UnregisterAttachedFrame(Dali::Window window, WindowFrameImpl* frame)
{
  WindowFrameAttachmentRegistry& registry = GetWindowFrameAttachmentRegistry();
  const auto                     iterator = registry.find(&window.GetBaseObject());
  if(iterator != registry.end() && iterator->second == frame)
  {
    registry.erase(iterator);
  }
}

} // unnamed namespace

WindowFrameImpl::WindowFrameImpl(Dali::Window                            window,
                                 WindowFrame::CloseCallback              closeCallback,
                                 const WindowFrameOptions&               options,
                                 std::unique_ptr<WindowOperationAdapter> windowOperations)
: mWindow(window),
  mWindowOperations(windowOperations ? std::move(windowOperations) : CreateDaliWindowOperationAdapter(window, std::move(closeCallback), options)),
  mWindowState(options.GetInitialStatePolicy()),
  mInitialRestoreFrameSize()
{
  Dali::Vector2 initialRestoreFrameSize;
  if(options.GetInitialRestoreFrameSize(initialRestoreFrameSize))
  {
    mInitialRestoreFrameSize = initialRestoreFrameSize;
  }

  mLayout.SetDecorationInsets(Dali::Ui::Insets());
  mLayout.SetBottomLeftResizeHandleSize(Dali::Vector2::ZERO);
  mLayout.SetBottomRightResizeHandleSize(Dali::Vector2::ZERO);
  mFrame.Create(mFrameStyle);
  mPresentationController.SetPresentedCallback([this](WindowFrame::WindowState state)
  {
    WindowFrame self(this);
    if(!mWindowStatePresentedSignal.Empty())
    {
      mWindowStatePresentedSignal.Emit(self, state);
    }
  });
}

WindowFrameImpl::~WindowFrameImpl()
{
  Detach();
  mFrame.Destroy();
}

bool WindowFrameImpl::Attach()
{
  if(mAttached)
  {
    return true;
  }
  if(!mWindow || !TryRegisterAttachedFrame(mWindow, this))
  {
    DALI_LOG_ERROR("[WindowFrame] another WindowFrame is already attached to this window\n");
    return false;
  }

  const std::optional<WindowFrame::WindowState> initialState = ResolveInitialWindowState();
  ConnectWindowSignals();
  ConnectDecorationInput();
  mWindow.Add(mFrame.GetSurfaceRoot());
  mAttached = true;
  ApplyResizeAspectRatioPolicy();
  ApplyOverlayStateUpdate(mOverlayController.SetAttached(true));
  ApplySizeConstraints();
  UpdateStateDependentVisuals();
  EmitWindowStateChanged(initialState);
  RequestWindowStatePresentation(initialState);
  SynchronizeWindowState();
  LogEvent("attached");
  return true;
}

void WindowFrameImpl::Detach()
{
  if(!mAttached)
  {
    return;
  }

  CancelInteraction();
  RestoreResizeAspectRatioPolicy();
  mPresentationController.CancelPending();
  ApplyOverlayStateUpdate(mOverlayController.SetAttached(false));
  DisconnectDecorationInput();
  DisconnectAll();
  ClearNativeSizeConstraints();
  mFrame.Unparent();
  mAttached = false;
  UnregisterAttachedFrame(mWindow, this);
  mOverlayAutoHideTimer.Reset();
  mDeferredResizeTimer.Reset();
  mDeferredContentSize.reset();
  mLastGeometry.reset();
  mLastDecorationVisibility.reset();
}

bool WindowFrameImpl::IsAttached() const
{
  return mAttached;
}

Dali::Ui::View WindowFrameImpl::GetContentRoot() const
{
  return mFrame.GetContentRoot();
}

const WindowFrameFeatureConfig& WindowFrameImpl::GetFeatureConfig() const
{
  return mWindowOperations->GetFeatureConfig();
}

bool WindowFrameImpl::IsFeatureEnabled(WindowFrameFeature feature) const
{
  const WindowFrameFeatureConfig& featureConfig = GetFeatureConfig();
  switch(feature)
  {
    case WindowFrameFeature::MOVE:
      return featureConfig.moveEnabled;
    case WindowFrameFeature::RESIZE:
      return featureConfig.resizeEnabled;
    case WindowFrameFeature::MINIMIZE:
      return featureConfig.minimizeEnabled;
    case WindowFrameFeature::MAXIMIZE_RESTORE:
      return featureConfig.maximizeRestoreEnabled;
  }
  return false;
}

WindowFrame::WindowState WindowFrameImpl::GetWindowState() const
{
  return mWindowState.GetState();
}

bool WindowFrameImpl::IsMaximized() const
{
  return mWindowState.IsMaximized();
}

bool WindowFrameImpl::IsMinimized() const
{
  return mWindowState.IsMinimized();
}

Dali::Ui::Insets WindowFrameImpl::GetLaidOutDecorationInsets(const WindowFrameLayoutResult& layout)
{
  // Report what decoration was actually laid out, not what was requested.  A
  // small frame clamps the configured insets, so the requested values would
  // overstate how much content is covered.
  return Dali::Ui::Insets(layout.GetLeft().width,
                          layout.GetRight().width,
                          layout.GetTop().height,
                          layout.GetBottom().height);
}

WindowFrameGeometry WindowFrameImpl::GetGeometry() const
{
  if(!mWindow)
  {
    return {};
  }

  const Dali::PositionSize      positionSize = mWindow.GetPositionSize();
  const bool                    maximized    = IsMaximized();
  const bool                    overlay      = mOverlayController.IsOverlayEnabled() && maximized;
  const WindowFrameLayoutResult layout       = WindowFrameLayoutEngine::Calculate(static_cast<float>(positionSize.width),
                                                                                  static_cast<float>(positionSize.height),
                                                                                  mLayout,
                                                                                  WindowFrameLayoutOptions{maximized, overlay, GetNormalShadowOutsets()});
  return WindowFrameGeometry{Dali::Ui::LayoutRect(static_cast<float>(positionSize.x),
                                                  static_cast<float>(positionSize.y),
                                                  static_cast<float>(positionSize.width),
                                                  static_cast<float>(positionSize.height)),
                             layout.GetFrame(),
                             layout.GetContent(),
                             overlay ? GetLaidOutDecorationInsets(layout) : Dali::Ui::Insets()};
}

WindowFrameDecorationResult WindowFrameImpl::SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy)
{
  return SetDecoration(decoration, layout, mFrameStyle, sizePolicy);
}

WindowFrameDecorationResult WindowFrameImpl::SetDecoration(const WindowFrameDecoration& decoration,
                                                           const WindowFrameLayout&     layout,
                                                           const WindowFrameStyle&      style,
                                                           WindowFrameSizePolicy        sizePolicy)
{
  // Slots already held by this frame are accepted so that the installed
  // decoration can be resubmitted with new layout or style.
  const WindowFrameDecorationResult validation = ValidateWindowFrameDecoration(decoration, mFrame.GetDecorationRoot());
  if(validation != WindowFrameDecorationResult::INSTALLED)
  {
    DALI_LOG_DEBUG_INFO("[WindowFrame] frame-rejected:%s\n", WindowFrameDecorationResultName(validation));
    return validation;
  }

  const bool          preserveContentSize = mWindow && !IsMaximized() && sizePolicy == WindowFrameSizePolicy::KEEP_CONTENT_SIZE;
  const Dali::Vector2 previousContentSize = preserveContentSize ? GetContentSizeForPreservation() : Dali::Vector2::ZERO;

  // Frame-size constraints are stored in frame space and compared in surface
  // space, where both bounds gain the same shadow outsets, so neither layout
  // nor style can invert them. Only the constraint setters need to validate.
  mLayout     = layout;
  mFrameStyle = style;

  DisconnectDecorationInput();

  mFrame.SetDecoration(decoration);
  mFrame.SetFrameStyle(mFrameStyle);

  if(mAttached)
  {
    ConnectDecorationInput();
    ApplySizeConstraints();
    UpdateStateDependentVisuals();
  }
  if(preserveContentSize)
  {
    RequestContentResize(previousContentSize);
  }

  LogEvent("frame-changed");
  return WindowFrameDecorationResult::INSTALLED;
}

void WindowFrameImpl::SetDecorationLayout(const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy)
{
  const bool          preserveContentSize = mWindow && !IsMaximized() && sizePolicy == WindowFrameSizePolicy::KEEP_CONTENT_SIZE;
  const Dali::Vector2 previousContentSize = preserveContentSize ? GetContentSizeForPreservation() : Dali::Vector2::ZERO;

  mLayout = layout;

  if(mAttached)
  {
    UpdateStateDependentVisuals();
  }
  if(preserveContentSize)
  {
    RequestContentResize(previousContentSize);
  }
  LogEvent("layout-changed");
}

void WindowFrameImpl::ClearDecoration()
{
  WindowFrameLayout emptyLayout;
  emptyLayout.SetDecorationInsets(Dali::Ui::Insets());
  emptyLayout.SetBottomLeftResizeHandleSize(Dali::Vector2::ZERO);
  emptyLayout.SetBottomRightResizeHandleSize(Dali::Vector2::ZERO);
  SetDecoration(WindowFrameDecoration{}, emptyLayout, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
}

void WindowFrameImpl::SetDecorationVisible(bool visible)
{
  const OverlayStateUpdate update = mOverlayController.SetRequestedVisible(visible);
  ApplyDecorationVisibility();
  ApplyOverlayStateUpdate(update);
  LogEvent(visible ? "decoration-visible" : "decoration-hidden");
}

bool WindowFrameImpl::IsDecorationVisible() const
{
  return mOverlayController.IsEffectivelyVisible();
}

void WindowFrameImpl::SetFrameStyle(const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy)
{
  const bool          preserveContentSize = mWindow && !IsMaximized() && sizePolicy == WindowFrameSizePolicy::KEEP_CONTENT_SIZE;
  const Dali::Vector2 previousContentSize = preserveContentSize ? GetContentSizeForPreservation() : Dali::Vector2::ZERO;

  mFrameStyle = style;
  mFrame.SetFrameStyle(mFrameStyle);

  if(mAttached)
  {
    ApplySizeConstraints();
    UpdateStateDependentVisuals();
  }
  if(preserveContentSize)
  {
    RequestContentResize(previousContentSize);
  }
  LogEvent("frame-style-changed");
}

const WindowFrameStyle& WindowFrameImpl::GetFrameStyle() const
{
  return mFrameStyle;
}

void WindowFrameImpl::SetOverlayEnabled(bool enabled)
{
  if(mOverlayController.IsOverlayEnabled() == enabled)
  {
    return;
  }
  const OverlayStateUpdate update = mOverlayController.SetOverlayEnabled(enabled);
  ApplyLayout();
  ApplyDecorationVisibility();
  ApplyOverlayStateUpdate(update);
  LogEvent(enabled ? "overlay-enabled" : "overlay-disabled");
}

bool WindowFrameImpl::IsOverlayEnabled() const
{
  return mOverlayController.IsOverlayEnabled();
}

void WindowFrameImpl::SetOverlayAutoHideDelay(uint32_t milliseconds)
{
  if(!mOverlayController.SetAutoHideDelay(milliseconds))
  {
    return;
  }

  if(mOverlayController.ShouldTimerRun())
  {
    // Restart with the new interval.
    StartOverlayAutoHideTimer();
  }
  else
  {
    // A zero delay turns auto-hide off, which can also reveal decoration that
    // it had hidden.
    StopOverlayAutoHideTimer();
    ApplyDecorationVisibility();
    EmitDecorationVisibilityChangedIfNeeded();
  }
}

uint32_t WindowFrameImpl::GetOverlayAutoHideDelay() const
{
  return mOverlayController.GetAutoHideDelay();
}

void WindowFrameImpl::ShowOverlayTemporarily()
{
  const OverlayStateUpdate update = mOverlayController.RevealTemporarily();
  ApplyDecorationVisibility();
  ApplyOverlayStateUpdate(update);
}

bool WindowFrameImpl::IsOverlayAutoHidden() const
{
  return mOverlayController.IsAutoHidden();
}

void WindowFrameImpl::SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy policy)
{
  mInteractiveResizePolicy = policy;
  ApplyResizeAspectRatioPolicy();
  if(policy == WindowFrameInteractiveResizePolicy::DISABLED && mInteractionController.IsResizeActive())
  {
    CancelInteraction();
  }
  ApplyResizeHandleState();
  switch(policy)
  {
    case WindowFrameInteractiveResizePolicy::FREE:
      LogEvent("resize-free");
      break;
    case WindowFrameInteractiveResizePolicy::DISABLED:
      LogEvent("resize-disabled");
      break;
    case WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO:
      LogEvent("resize-keep-aspect-ratio");
      break;
  }
}

WindowFrameInteractiveResizePolicy WindowFrameImpl::GetInteractiveResizePolicy() const
{
  return mInteractiveResizePolicy;
}

bool WindowFrameImpl::SetMinimumFrameSize(const Dali::Vector2& size)
{
  const std::optional<Dali::Vector2> previous = mMinimumFrameSize;
  mMinimumFrameSize                           = Dali::Vector2(std::max(0.0f, size.width), std::max(0.0f, size.height));
  if(!AreSizeConstraintsValid())
  {
    mMinimumFrameSize = previous;
    return false;
  }
  ApplySizeConstraints();
  return true;
}

bool WindowFrameImpl::SetMaximumFrameSize(const Dali::Vector2& size)
{
  const std::optional<Dali::Vector2> previous = mMaximumFrameSize;
  mMaximumFrameSize                           = Dali::Vector2(std::max(0.0f, size.width), std::max(0.0f, size.height));
  if(!AreSizeConstraintsValid())
  {
    mMaximumFrameSize = previous;
    return false;
  }
  ApplySizeConstraints();
  return true;
}

void WindowFrameImpl::ClearMinimumFrameSize()
{
  mMinimumFrameSize.reset();
  ApplySizeConstraints();
}

void WindowFrameImpl::ClearMaximumFrameSize()
{
  mMaximumFrameSize.reset();
  ApplySizeConstraints();
}

bool WindowFrameImpl::GetMinimumFrameSize(Dali::Vector2& size) const
{
  if(!mMinimumFrameSize)
  {
    return false;
  }
  size = *mMinimumFrameSize;
  return true;
}

bool WindowFrameImpl::GetMaximumFrameSize(Dali::Vector2& size) const
{
  if(!mMaximumFrameSize)
  {
    return false;
  }
  size = *mMaximumFrameSize;
  return true;
}

bool WindowFrameImpl::RequestFrameResize(const Dali::Vector2& size)
{
  return RequestContentResize(WindowFrameLayoutEngine::FrameToContentSize(size, mLayout));
}

bool WindowFrameImpl::RequestContentResize(const Dali::Vector2& size)
{
  // Maximized geometry is compositor-owned, so reject rather than queue a
  // request that ApplyContentSize() would silently drop later.
  if(!mWindow || IsMaximized())
  {
    LogEvent("size-request-ignored");
    return false;
  }

  if(mAttached)
  {
    ScheduleContentSize(size);
    return true;
  }
  ApplyContentSize(size);
  return true;
}

void WindowFrameImpl::ApplyContentSize(const Dali::Vector2& size)
{
  // Maximized geometry is compositor-owned, so a client size request is ignored.
  if(!mWindow || IsMaximized())
  {
    return;
  }

  const Dali::Vector2 contentSize(std::max(0.0f, size.width), std::max(0.0f, size.height));
  const Dali::Vector2 nativeSize = ClampSurfaceSize(WindowFrameLayoutEngine::ContentToSurfaceSize(contentSize, mLayout, GetNormalShadowOutsets()));

  Dali::PositionSize positionSize = mWindow.GetPositionSize();
  positionSize.width              = static_cast<int32_t>(std::ceil(nativeSize.width));
  positionSize.height             = static_cast<int32_t>(std::ceil(nativeSize.height));
  mWindow.SetPositionSize(positionSize);
}

void WindowFrameImpl::ScheduleContentSize(const Dali::Vector2& size)
{
  mDeferredContentSize = size;
  if(!mDeferredResizeTimer)
  {
    mDeferredResizeTimer = Dali::Timer::New(1u);
    mDeferredResizeTimer.TickSignal().Connect(this, &WindowFrameImpl::OnDeferredResizeTimerTick);
  }
  else if(mDeferredResizeTimer.IsRunning())
  {
    mDeferredResizeTimer.Stop();
  }
  mDeferredResizeTimer.Start();
}

Dali::Vector2 WindowFrameImpl::GetContentSizeForPreservation() const
{
  return mDeferredContentSize.value_or(GetContentSize());
}

Dali::Vector2 WindowFrameImpl::GetFrameSize() const
{
  if(!mWindow)
  {
    return Dali::Vector2::ZERO;
  }

  const Dali::PositionSize positionSize = mWindow.GetPositionSize();
  return WindowFrameLayoutEngine::SurfaceToFrameSize(Dali::Vector2(static_cast<float>(positionSize.width), static_cast<float>(positionSize.height)), GetNormalShadowOutsets(), IsMaximized());
}

Dali::Vector2 WindowFrameImpl::GetContentSize() const
{
  if(!mWindow)
  {
    return Dali::Vector2::ZERO;
  }

  const Dali::PositionSize       positionSize = mWindow.GetPositionSize();
  const bool                     maximized    = IsMaximized();
  const WindowFrameLayoutOptions options{maximized, mOverlayController.IsOverlayEnabled() && maximized, GetNormalShadowOutsets()};
  return WindowFrameLayoutEngine::SurfaceToContentSize(Dali::Vector2(static_cast<float>(positionSize.width), static_cast<float>(positionSize.height)), mLayout, options);
}

WindowFrameCommandResult WindowFrameImpl::RequestMinimize()
{
  // An explicit request always reaches the window system. Suppressing it when
  // the observed state already matches would drop a request sent before the
  // previous one was reported, losing the caller's last intent.
  const WindowFrameCommandRequest request{WindowFrameCommand::MINIMIZE};
  if(InvokeCommandInterceptor(request) == WindowFrameCommandDisposition::HANDLED)
  {
    LogEvent("minimize-handled");
    return EmitCommandProcessed(request, WindowFrameCommandResult::HANDLED);
  }

  const bool dispatched = mWindowOperations && mWindowOperations->RequestMinimize();
  if(dispatched)
  {
    CancelInteraction();
  }
  LogEvent(dispatched ? "minimize-requested" : "minimize-not-dispatched");
  return EmitCommandProcessed(request, dispatched ? WindowFrameCommandResult::DISPATCHED : WindowFrameCommandResult::NOT_DISPATCHED);
}

WindowFrameCommandResult WindowFrameImpl::RequestMaximizedState(bool maximize)
{
  // See RequestMinimize(): the observed state does not gate an explicit request.
  const WindowFrameCommandRequest request{maximize ? WindowFrameCommand::MAXIMIZE : WindowFrameCommand::RESTORE};
  if(InvokeCommandInterceptor(request) == WindowFrameCommandDisposition::HANDLED)
  {
    LogEvent(maximize ? "maximize-handled" : "restore-handled");
    return EmitCommandProcessed(request, WindowFrameCommandResult::HANDLED);
  }

  // A window that launched already covering the screen has no earlier normal
  // geometry to come back to, so its first restore carries the size to use. Sent
  // here rather than with the launch maximize because the window system keeps a
  // restore size across an unmaximize but resets it on a maximize.
  std::optional<Dali::Window::WindowSize> restoreSurfaceSize;
  if(!maximize)
  {
    restoreSurfaceSize = mWindowState.TakeInitialRestoreSize();
  }

  const bool dispatched = mWindowOperations && mWindowOperations->RequestMaximize(maximize, restoreSurfaceSize);
  if(dispatched)
  {
    CancelInteraction();
  }
  LogEvent(dispatched ? (maximize ? "maximize-requested" : "restore-requested") : (maximize ? "maximize-not-dispatched" : "restore-not-dispatched"));
  return EmitCommandProcessed(request, dispatched ? WindowFrameCommandResult::DISPATCHED : WindowFrameCommandResult::NOT_DISPATCHED);
}

WindowFrameCommandResult WindowFrameImpl::ToggleMaximize()
{
  const std::optional<bool> target = mWindowState.GetToggleTarget();
  if(!target)
  {
    const WindowFrameCommandRequest request{IsMaximized() ? WindowFrameCommand::RESTORE : WindowFrameCommand::MAXIMIZE};
    LogEvent("maximize-not-dispatched");
    return EmitCommandProcessed(request, WindowFrameCommandResult::NOT_DISPATCHED);
  }
  return RequestMaximizedState(*target);
}

WindowFrameCommandResult WindowFrameImpl::RequestClose()
{
  const WindowFrameCommandRequest request{WindowFrameCommand::CLOSE};
  if(InvokeCommandInterceptor(request) == WindowFrameCommandDisposition::HANDLED)
  {
    LogEvent("close-handled");
    return EmitCommandProcessed(request, WindowFrameCommandResult::HANDLED);
  }
  const bool dispatched = mWindowOperations && mWindowOperations->RequestClose();
  return EmitCommandProcessed(request, dispatched ? WindowFrameCommandResult::DISPATCHED : WindowFrameCommandResult::NOT_DISPATCHED);
}

void WindowFrameImpl::SetCommandInterceptor(WindowFrame::CommandInterceptor interceptor)
{
  mCommandInterceptor = std::move(interceptor);
}

WindowFrame::WindowStateChangedSignalType& WindowFrameImpl::WindowStateChangedSignal()
{
  return mWindowStateChangedSignal;
}

WindowFrame::WindowStatePresentedSignalType& WindowFrameImpl::WindowStatePresentedSignal()
{
  return mWindowStatePresentedSignal;
}

WindowFrame::GeometryChangedSignalType& WindowFrameImpl::GeometryChangedSignal()
{
  return mGeometryChangedSignal;
}

WindowFrame::GeometryChangeCompletedSignalType& WindowFrameImpl::GeometryChangeCompletedSignal()
{
  return mGeometryChangeCompletedSignal;
}

WindowFrame::DecorationVisibilityChangedSignalType& WindowFrameImpl::DecorationVisibilityChangedSignal()
{
  return mDecorationVisibilityChangedSignal;
}

WindowFrame::InteractionChangedSignalType& WindowFrameImpl::InteractionChangedSignal()
{
  return mInteractionChangedSignal;
}

WindowFrame::CommandProcessedSignalType& WindowFrameImpl::CommandProcessedSignal()
{
  return mCommandProcessedSignal;
}

std::optional<WindowFrame::WindowState> WindowFrameImpl::ResolveInitialWindowState()
{
  if(!mWindow || !mWindowOperations)
  {
    return std::nullopt;
  }

  std::optional<Dali::Window::WindowSize> restoreSurfaceSize;
  if(mInitialRestoreFrameSize)
  {
    restoreSurfaceSize = ToWindowSize(WindowFrameLayoutEngine::FrameToSurfaceSize(*mInitialRestoreFrameSize, GetNormalShadowOutsets()));
  }

  const Dali::PositionSize                      positionSize = mWindow.GetPositionSize();
  const LaunchStateResolution                   before       = mWindowState.GetLaunchResolution();
  const std::optional<WindowFrame::WindowState> state        = mWindowState.ResolveInitialState(Dali::Window::WindowSize(positionSize.width, positionSize.height),
                                                                                                *mWindowOperations,
                                                                                                restoreSurfaceSize);
  const LaunchStateResolution                   after        = mWindowState.GetLaunchResolution();
  if(before != after)
  {
    DALI_LOG_DEBUG_INFO("[WindowFrame] launch-state:%s\n", LaunchStateResolutionName(after));
  }
  return state;
}

void WindowFrameImpl::SynchronizeWindowState()
{
  if(!mWindowOperations)
  {
    return;
  }

  std::optional<Dali::Window::WindowSize> restoreSurfaceSize;
  if(mInitialRestoreFrameSize)
  {
    restoreSurfaceSize = ToWindowSize(WindowFrameLayoutEngine::FrameToSurfaceSize(*mInitialRestoreFrameSize, GetNormalShadowOutsets()));
  }
  const Dali::PositionSize                      positionSize = mWindow.GetPositionSize();
  const std::optional<WindowFrame::WindowState> state        = mWindowState.SynchronizeNativeState(Dali::Window::WindowSize(positionSize.width, positionSize.height),
                                                                                                   *mWindowOperations,
                                                                                                   restoreSurfaceSize);
  UpdateStateDependentVisuals();
  EmitWindowStateChanged(state);
  RequestWindowStatePresentation(state);
}

void WindowFrameImpl::UpdateStateDependentVisuals()
{
  if(IsMaximized())
  {
    CancelInteraction();
  }
  const OverlayStateUpdate overlayUpdate = mOverlayController.SetEffectiveMaximized(IsMaximized());
  ApplyLayout();
  ApplyDecorationVisibility();
  ApplyWindowControlState();
  ApplyOverlayStateUpdate(overlayUpdate);
}

std::optional<Dali::Vector2> WindowFrameImpl::GetMinimumSurfaceSize() const
{
  return mMinimumFrameSize ? std::optional<Dali::Vector2>(WindowFrameLayoutEngine::FrameToSurfaceSize(*mMinimumFrameSize, GetNormalShadowOutsets())) : std::nullopt;
}

std::optional<Dali::Vector2> WindowFrameImpl::GetMaximumSurfaceSize() const
{
  return mMaximumFrameSize ? std::optional<Dali::Vector2>(WindowFrameLayoutEngine::FrameToSurfaceSize(*mMaximumFrameSize, GetNormalShadowOutsets())) : std::nullopt;
}

Dali::Ui::Insets WindowFrameImpl::GetNormalShadowOutsets() const
{
  return ResolveWindowFrameStyle(mFrameStyle, false).shadowOutsets;
}

bool WindowFrameImpl::AreSizeConstraintsValid() const
{
  const std::optional<Dali::Vector2> minimum = GetMinimumSurfaceSize();
  const std::optional<Dali::Vector2> maximum = GetMaximumSurfaceSize();
  return !minimum || !maximum || (minimum->width <= maximum->width && minimum->height <= maximum->height);
}

Dali::Vector2 WindowFrameImpl::ClampSurfaceSize(const Dali::Vector2& size) const
{
  return ClampSize(size, GetMinimumSurfaceSize(), GetMaximumSurfaceSize());
}

void WindowFrameImpl::ConnectDecorationInput()
{
  WindowFrameDecoration& decoration = mFrame.GetDecoration();
  if(decoration.GetMoveRegion())
  {
    decoration.GetMoveRegion().TouchEventSignal().Connect(this, &WindowFrameImpl::OnMoveRegionTouched);
  }
  if(decoration.GetBottomLeftResizeHandle())
  {
    decoration.GetBottomLeftResizeHandle().TouchEventSignal().Connect(this, &WindowFrameImpl::OnBottomLeftResizeHandleTouched);
  }
  if(decoration.GetBottomRightResizeHandle())
  {
    decoration.GetBottomRightResizeHandle().TouchEventSignal().Connect(this, &WindowFrameImpl::OnBottomRightResizeHandleTouched);
  }
  if(decoration.GetMinimizeControl())
  {
    decoration.GetMinimizeControl().AsInteractive().ClickedSignal().Connect(this, &WindowFrameImpl::OnMinimizeControlClicked);
  }
  if(decoration.GetMaximizeRestoreControl())
  {
    decoration.GetMaximizeRestoreControl().AsInteractive().ClickedSignal().Connect(this, &WindowFrameImpl::OnMaximizeRestoreControlClicked);
  }
  if(decoration.GetCloseControl())
  {
    decoration.GetCloseControl().AsInteractive().ClickedSignal().Connect(this, &WindowFrameImpl::OnCloseControlClicked);
  }
}

void WindowFrameImpl::ConnectWindowSignals()
{
  mWindow.ResizedSignal().Connect(this, &WindowFrameImpl::OnWindowResized);
  mWindow.MovedSignal().Connect(this, &WindowFrameImpl::OnWindowMoved);
  mWindow.MoveCompletedSignal().Connect(this, &WindowFrameImpl::OnMoveCompleted);
  mWindow.ResizeCompletedSignal().Connect(this, &WindowFrameImpl::OnResizeCompleted);
  mWindow.FocusChangedSignal().Connect(this, &WindowFrameImpl::OnFocusChanged);
  mWindow.VisibilityChangedSignal().Connect(this, &WindowFrameImpl::OnVisibilityChanged);
  mFrame.GetSurfaceRoot().InterceptTouchEventSignal().Connect(this, &WindowFrameImpl::OnSurfaceInterceptTouched);
}

void WindowFrameImpl::ApplyLayout()
{
  if(!mAttached || !mWindow)
  {
    return;
  }

  const Dali::PositionSize positionSize = mWindow.GetPositionSize();
  const bool               maximized    = IsMaximized();
  const bool               overlay      = mOverlayController.IsOverlayEnabled() && maximized;
  mFrame.ApplyLayout(positionSize, mLayout, maximized, overlay, mFrameStyle);

  ApplyResizeHandleState();
  ApplyMoveRegionState();
  ApplyWindowControlState();
  EmitGeometryChangedIfNeeded();
}

void WindowFrameImpl::ApplyNativeSizeConstraint(const std::optional<Dali::Vector2>& surfaceSize, bool& applied, void (Dali::Window::*setter)(Dali::Window::WindowSize))
{
  if(surfaceSize)
  {
    (mWindow.*setter)(ToWindowSize(*surfaceSize));
    applied = true;
  }
  else if(applied)
  {
    // Only clear a constraint this WindowFrame set, so an unrelated
    // constraint on the same window is left alone.
    (mWindow.*setter)(Dali::Window::WindowSize(0, 0));
    applied = false;
  }
}

void WindowFrameImpl::ApplySizeConstraints()
{
  if(!mAttached || !mWindow)
  {
    return;
  }
  ApplyNativeSizeConstraint(GetMinimumSurfaceSize(), mNativeMinimumConstraintApplied, &Dali::Window::SetMinimumSize);
  ApplyNativeSizeConstraint(GetMaximumSurfaceSize(), mNativeMaximumConstraintApplied, &Dali::Window::SetMaximumSize);
}

void WindowFrameImpl::ClearNativeSizeConstraints()
{
  if(!mWindow)
  {
    return;
  }
  ApplyNativeSizeConstraint(std::nullopt, mNativeMinimumConstraintApplied, &Dali::Window::SetMinimumSize);
  ApplyNativeSizeConstraint(std::nullopt, mNativeMaximumConstraintApplied, &Dali::Window::SetMaximumSize);
}

void WindowFrameImpl::ApplyDecorationVisibility()
{
  const bool visible = IsDecorationVisible();
  mFrame.SetDecorationVisible(visible);
  ApplyResizeHandleState();
  ApplyMoveRegionState();
  ApplyWindowControlState();
}

void WindowFrameImpl::ApplyResizeHandleState()
{
  WindowFrameDecoration& decoration        = mFrame.GetDecoration();
  const bool             decorationVisible = IsDecorationVisible();
  const bool             visible           = decorationVisible && mInteractiveResizePolicy != WindowFrameInteractiveResizePolicy::DISABLED && mWindow && !IsMaximized();
  const bool             sensitive         = visible && GetFeatureConfig().resizeEnabled;
  if(decoration.GetBottomLeftResizeHandle())
  {
    decoration.GetBottomLeftResizeHandle().SetSensitive(sensitive);
    decoration.GetBottomLeftResizeHandle().SetVisible(visible);
  }
  if(decoration.GetBottomRightResizeHandle())
  {
    decoration.GetBottomRightResizeHandle().SetSensitive(sensitive);
    decoration.GetBottomRightResizeHandle().SetVisible(visible);
  }
}

void WindowFrameImpl::ApplyResizeAspectRatioPolicy()
{
  if(!mAttached || !mWindowOperations)
  {
    return;
  }

  if(mInteractiveResizePolicy != WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO)
  {
    mWindowOperations->RestoreResizeAspectRatioHint();
    return;
  }

  if(!mWindowOperations->ApplyResizeAspectRatioHint(true))
  {
    DALI_LOG_RELEASE_INFO("[WindowFrame] Native aspect-ratio resize hint is unsupported\n");
  }
}

void WindowFrameImpl::RestoreResizeAspectRatioPolicy()
{
  if(mWindowOperations)
  {
    mWindowOperations->RestoreResizeAspectRatioHint();
  }
}

void WindowFrameImpl::ApplyMoveRegionState()
{
  WindowFrameDecoration& decoration = mFrame.GetDecoration();
  if(decoration.GetMoveRegion())
  {
    decoration.GetMoveRegion().SetSensitive(IsDecorationVisible() && GetFeatureConfig().moveEnabled && !IsMaximized());
  }
}

void WindowFrameImpl::ApplyWindowControlState()
{
  WindowFrameDecoration&          decoration    = mFrame.GetDecoration();
  const WindowFrameFeatureConfig& featureConfig = GetFeatureConfig();
  if(decoration.GetMinimizeControl())
  {
    decoration.GetMinimizeControl().SetSensitive(featureConfig.minimizeEnabled);
  }
  if(decoration.GetMaximizeRestoreControl())
  {
    decoration.GetMaximizeRestoreControl().SetSensitive(featureConfig.maximizeRestoreEnabled);
  }
  if(decoration.GetCloseControl())
  {
    decoration.GetCloseControl().SetSensitive(true);
  }
}

void WindowFrameImpl::DisconnectDecorationInput()
{
  WindowFrameDecoration& decoration = mFrame.GetDecoration();
  if(decoration.GetMoveRegion())
  {
    decoration.GetMoveRegion().TouchEventSignal().Disconnect(this, &WindowFrameImpl::OnMoveRegionTouched);
  }
  if(decoration.GetBottomLeftResizeHandle())
  {
    decoration.GetBottomLeftResizeHandle().TouchEventSignal().Disconnect(this, &WindowFrameImpl::OnBottomLeftResizeHandleTouched);
  }
  if(decoration.GetBottomRightResizeHandle())
  {
    decoration.GetBottomRightResizeHandle().TouchEventSignal().Disconnect(this, &WindowFrameImpl::OnBottomRightResizeHandleTouched);
  }
  if(decoration.GetMinimizeControl())
  {
    decoration.GetMinimizeControl().AsInteractive().ClickedSignal().Disconnect(this, &WindowFrameImpl::OnMinimizeControlClicked);
  }
  if(decoration.GetMaximizeRestoreControl())
  {
    decoration.GetMaximizeRestoreControl().AsInteractive().ClickedSignal().Disconnect(this, &WindowFrameImpl::OnMaximizeRestoreControlClicked);
  }
  if(decoration.GetCloseControl())
  {
    decoration.GetCloseControl().AsInteractive().ClickedSignal().Disconnect(this, &WindowFrameImpl::OnCloseControlClicked);
  }
}

void WindowFrameImpl::ApplyOverlayStateUpdate(const OverlayStateUpdate& update)
{
  if(update.stopTimer)
  {
    StopOverlayAutoHideTimer();
  }
  if(update.restartTimer && mOverlayController.ShouldTimerRun())
  {
    StartOverlayAutoHideTimer();
  }
  EmitDecorationVisibilityChangedIfNeeded();
}

void WindowFrameImpl::StartOverlayAutoHideTimer()
{
  if(!mOverlayAutoHideTimer)
  {
    mOverlayAutoHideTimer = Dali::Timer::New(mOverlayController.GetAutoHideDelay());
    mOverlayAutoHideTimer.TickSignal().Connect(this, &WindowFrameImpl::OnOverlayAutoHideTimerTick);
  }
  else
  {
    mOverlayAutoHideTimer.Stop();
    mOverlayAutoHideTimer.SetInterval(mOverlayController.GetAutoHideDelay(), false);
  }
  mOverlayAutoHideTimer.Start();
}

void WindowFrameImpl::StopOverlayAutoHideTimer()
{
  if(mOverlayAutoHideTimer && mOverlayAutoHideTimer.IsRunning())
  {
    mOverlayAutoHideTimer.Stop();
  }
}

void WindowFrameImpl::EmitInteraction(const std::optional<WindowFrameInteraction>& interaction)
{
  if(!interaction)
  {
    return;
  }

  if(!mInteractionChangedSignal.Empty())
  {
    WindowFrame self(this);
    mInteractionChangedSignal.Emit(self, *interaction);
  }
  DALI_LOG_DEBUG_INFO("[WindowFrame] interaction:%s:%s\n",
                      WindowFrameInteractionTypeName(interaction->GetType()),
                      WindowFrameInteractionStateName(interaction->GetState()));
}

void WindowFrameImpl::CancelInteraction()
{
  EmitInteraction(mInteractionController.Cancel());
}

void WindowFrameImpl::LogEvent(const char* event)
{
  DALI_LOG_DEBUG_INFO("[WindowFrame] %s\n", event);
}

void WindowFrameImpl::EmitWindowStateChanged(const std::optional<WindowFrame::WindowState>& state)
{
  if(state && !mWindowStateChangedSignal.Empty())
  {
    WindowFrame self(this);
    mWindowStateChangedSignal.Emit(self, *state);
  }
}

void WindowFrameImpl::RequestWindowStatePresentation(const std::optional<WindowFrame::WindowState>& state)
{
  if(state && !mWindowStatePresentedSignal.Empty())
  {
    mPresentationController.RequestPresentation(mWindow, *state);
  }
}

void WindowFrameImpl::EmitGeometryChangedIfNeeded()
{
  const WindowFrameGeometry geometry = GetGeometry();
  if(mLastGeometry && IsSameWindowFrameGeometry(*mLastGeometry, geometry))
  {
    return;
  }

  mLastGeometry = geometry;
  if(!mGeometryChangedSignal.Empty())
  {
    WindowFrame self(this);
    mGeometryChangedSignal.Emit(self, geometry);
  }
}

void WindowFrameImpl::EmitGeometryChangeCompleted()
{
  const WindowFrameGeometry geometry = GetGeometry();
  if(!mGeometryChangeCompletedSignal.Empty())
  {
    WindowFrame self(this);
    mGeometryChangeCompletedSignal.Emit(self, geometry);
  }
}

void WindowFrameImpl::EmitDecorationVisibilityChangedIfNeeded()
{
  const bool visible = mOverlayController.IsEffectivelyVisible();
  if(mLastDecorationVisibility && *mLastDecorationVisibility == visible)
  {
    return;
  }

  mLastDecorationVisibility = visible;
  if(!mDecorationVisibilityChangedSignal.Empty())
  {
    WindowFrame self(this);
    mDecorationVisibilityChangedSignal.Emit(self, visible);
  }
}

WindowFrameCommandResult WindowFrameImpl::EmitCommandProcessed(const WindowFrameCommandRequest& request, WindowFrameCommandResult result)
{
  if(!mCommandProcessedSignal.Empty())
  {
    WindowFrame self(this);
    mCommandProcessedSignal.Emit(self, request, result);
  }
  return result;
}

WindowFrameCommandDisposition WindowFrameImpl::InvokeCommandInterceptor(const WindowFrameCommandRequest& request)
{
  if(!mCommandInterceptor)
  {
    return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
  }

  WindowFrame self(this);
  return mCommandInterceptor.Invoke(self, request);
}

WindowFrameCommandResult WindowFrameImpl::RequestMove()
{
  const WindowFrameCommandRequest     request{WindowFrameCommand::MOVE};
  const WindowFrameCommandDisposition disposition = InvokeCommandInterceptor(request);
  if(disposition == WindowFrameCommandDisposition::HANDLED)
  {
    return EmitCommandProcessed(request, WindowFrameCommandResult::HANDLED);
  }
  const bool dispatched = mWindowOperations && mWindowOperations->RequestMove();
  return EmitCommandProcessed(request, dispatched ? WindowFrameCommandResult::DISPATCHED : WindowFrameCommandResult::NOT_DISPATCHED);
}

WindowFrameCommandResult WindowFrameImpl::RequestResize(Dali::WindowResizeDirection direction)
{
  const WindowFrameCommandRequest     request{WindowFrameCommand::RESIZE, direction};
  const WindowFrameCommandDisposition disposition = InvokeCommandInterceptor(request);
  if(disposition == WindowFrameCommandDisposition::HANDLED)
  {
    return EmitCommandProcessed(request, WindowFrameCommandResult::HANDLED);
  }
  const bool dispatched = mWindowOperations && mWindowOperations->RequestResize(direction);
  return EmitCommandProcessed(request, dispatched ? WindowFrameCommandResult::DISPATCHED : WindowFrameCommandResult::NOT_DISPATCHED);
}

bool WindowFrameImpl::OnMoveRegionTouched(Dali::Actor /*actor*/, Dali::TouchEvent event)
{
  // Every native or input callback below holds a handle to itself while it
  // runs, so an application that releases its last WindowFrame handle from a
  // signal handler cannot destroy this object mid-callback.
  Dali::BaseHandle self(this);
  if(event.GetPointCount() == 0u)
  {
    return false;
  }

  const Dali::PointState::Type state = event.GetState(0u);
  if(state == Dali::PointState::DOWN && mInteractionController.CanBegin() && mWindow && !IsMaximized())
  {
    const WindowFrameCommandResult result = RequestMove();
    if(result == WindowFrameCommandResult::DISPATCHED)
    {
      EmitInteraction(mInteractionController.Begin(WindowFrameInteractionType::MOVE));
    }
  }
  else if(state == Dali::PointState::UP || state == Dali::PointState::INTERRUPTED)
  {
    EmitInteraction(mInteractionController.PointerEnded(WindowFrameInteractionType::MOVE, state == Dali::PointState::INTERRUPTED));
  }
  return true;
}

bool WindowFrameImpl::OnResizeHandleTouched(Dali::WindowResizeDirection direction, Dali::TouchEvent event)
{
  Dali::BaseHandle self(this);
  if(event.GetPointCount() == 0u)
  {
    return false;
  }

  const Dali::PointState::Type     state = event.GetState(0u);
  const WindowFrameInteractionType type  = direction == Dali::WindowResizeDirection::BOTTOM_LEFT ? WindowFrameInteractionType::RESIZE_BOTTOM_LEFT : WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT;
  if(state == Dali::PointState::DOWN && mInteractionController.CanBegin() && mInteractiveResizePolicy != WindowFrameInteractiveResizePolicy::DISABLED && mWindow && !IsMaximized())
  {
    ApplyResizeAspectRatioPolicy();
    const WindowFrameCommandResult result = RequestResize(direction);
    if(result == WindowFrameCommandResult::DISPATCHED)
    {
      EmitInteraction(mInteractionController.Begin(type));
    }
  }
  else if(state == Dali::PointState::UP || state == Dali::PointState::INTERRUPTED)
  {
    EmitInteraction(mInteractionController.PointerEnded(type, state == Dali::PointState::INTERRUPTED));
  }
  return true;
}

// Dali::Actor::TouchEventSignal() cannot carry the resize direction, so each
// handle gets a thin wrapper that supplies it.
bool WindowFrameImpl::OnBottomLeftResizeHandleTouched(Dali::Actor actor, Dali::TouchEvent event)
{
  static_cast<void>(actor);
  return OnResizeHandleTouched(Dali::WindowResizeDirection::BOTTOM_LEFT, event);
}

bool WindowFrameImpl::OnBottomRightResizeHandleTouched(Dali::Actor actor, Dali::TouchEvent event)
{
  static_cast<void>(actor);
  return OnResizeHandleTouched(Dali::WindowResizeDirection::BOTTOM_RIGHT, event);
}

void WindowFrameImpl::OnMinimizeControlClicked(Dali::Ui::View /*view*/, Dali::Ui::InputEvent /*event*/)
{
  Dali::BaseHandle self(this);
  RequestMinimize();
}

void WindowFrameImpl::OnMaximizeRestoreControlClicked(Dali::Ui::View /*view*/, Dali::Ui::InputEvent /*event*/)
{
  Dali::BaseHandle self(this);
  ToggleMaximize();
}

void WindowFrameImpl::OnCloseControlClicked(Dali::Ui::View /*view*/, Dali::Ui::InputEvent /*event*/)
{
  Dali::BaseHandle self(this);
  RequestClose();
}

bool WindowFrameImpl::OnOverlayAutoHideTimerTick()
{
  Dali::BaseHandle         self(this);
  const bool               wasVisible = mOverlayController.IsEffectivelyVisible();
  const OverlayStateUpdate update     = mOverlayController.OnAutoHideTimeout();
  ApplyDecorationVisibility();
  ApplyOverlayStateUpdate(update);
  if(wasVisible && !mOverlayController.IsEffectivelyVisible())
  {
    LogEvent("overlay-decoration-hidden");
  }
  return false;
}

bool WindowFrameImpl::OnDeferredResizeTimerTick()
{
  Dali::BaseHandle                   self(this);
  const std::optional<Dali::Vector2> contentSize = mDeferredContentSize;
  mDeferredContentSize.reset();
  if(contentSize && mAttached)
  {
    ApplyContentSize(*contentSize);
  }
  return false;
}

bool WindowFrameImpl::OnSurfaceInterceptTouched(Dali::Actor /*actor*/, Dali::TouchEvent event)
{
  Dali::BaseHandle self(this);
  if(event.GetPointCount() > 0u && event.GetState(0u) == Dali::PointState::DOWN)
  {
    const bool               wasHidden = mOverlayController.IsAutoHidden();
    const OverlayStateUpdate update    = mOverlayController.RevealTemporarily();
    ApplyDecorationVisibility();
    ApplyOverlayStateUpdate(update);
    if(wasHidden && mOverlayController.IsEffectivelyVisible())
    {
      LogEvent("overlay-decoration-visible");
    }
  }
  return false;
}

void WindowFrameImpl::OnWindowResized(Dali::Window /*window*/, Dali::Window::WindowSize /*size*/)
{
  Dali::BaseHandle self(this);
  SynchronizeWindowState();
  LogEvent("resized");
}

void WindowFrameImpl::OnWindowMoved(Dali::Window /*window*/, Dali::Window::WindowPosition /*position*/)
{
  Dali::BaseHandle self(this);
  EmitGeometryChangedIfNeeded();
  LogEvent("moved");
}

void WindowFrameImpl::OnMoveCompleted(Dali::Window /*window*/, Dali::Window::WindowPosition /*position*/)
{
  Dali::BaseHandle self(this);
  EmitGeometryChangedIfNeeded();
  EmitInteraction(mInteractionController.CompleteMove());
  EmitGeometryChangeCompleted();
}

void WindowFrameImpl::OnResizeCompleted(Dali::Window /*window*/, Dali::Window::WindowSize /*size*/)
{
  Dali::BaseHandle self(this);
  EmitInteraction(mInteractionController.CompleteResize());
  SynchronizeWindowState();
  EmitGeometryChangeCompleted();
  LogEvent("resize-completed");
}

void WindowFrameImpl::OnFocusChanged(Dali::Window /*window*/, bool focused)
{
  WindowFrame self(this);
  // A compositor move or resize grab can take window focus while the pointer is
  // still down.  Cancelling there would drop an interaction that is still
  // running and its completion event would then be ignored, so only an
  // interaction whose pointer was already released is cancelled here.
  if(!focused && mInteractionController.IsPointerReleased())
  {
    CancelInteraction();
  }
  SynchronizeWindowState();
  LogEvent(focused ? "focus-gained" : "focus-lost");
}

void WindowFrameImpl::OnVisibilityChanged(Dali::Window /*window*/, bool visible)
{
  Dali::BaseHandle         self(this);
  const OverlayStateUpdate update = mOverlayController.SetWindowVisible(visible);
  SynchronizeWindowState();
  if(visible)
  {
    ApplyDecorationVisibility();
  }
  else
  {
    CancelInteraction();
    ApplyDecorationVisibility();
  }
  ApplyOverlayStateUpdate(update);
  LogEvent(visible ? "window-visible" : "window-hidden");
}

} // namespace Internal
} // namespace Dali::Ui

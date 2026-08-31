/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-components/public-api/window/default-window-decoration.h>
#include <dali-ui-components/public-api/window/window-frame.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>

#include <optional>

namespace Test
{
void EmitGlobalTimerSignal();
}

using namespace Dali;
using namespace Dali::Ui;

static_assert(static_cast<int>(WindowFrameFeature::MOVE) == 0);
static_assert(static_cast<int>(WindowFrameFeature::RESIZE) == 1);
static_assert(static_cast<int>(WindowFrameFeature::MINIMIZE) == 2);
static_assert(static_cast<int>(WindowFrameFeature::MAXIMIZE_RESTORE) == 3);
static_assert(static_cast<int>(WindowFrameCommandDisposition::CONTINUE_DEFAULT) == 0);
static_assert(static_cast<int>(WindowFrameCommandDisposition::HANDLED) == 1);
static_assert(static_cast<int>(WindowFrameCommandResult::DISPATCHED) == 0);
static_assert(static_cast<int>(WindowFrameCommandResult::NOT_DISPATCHED) == 1);
static_assert(static_cast<int>(WindowFrameCommandResult::HANDLED) == 2);
static_assert(static_cast<int>(WindowFrameInteractiveResizePolicy::FREE) == 0);
static_assert(static_cast<int>(WindowFrameInteractiveResizePolicy::DISABLED) == 1);
static_assert(static_cast<int>(WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO) == 2);
static_assert(static_cast<int>(WindowFrameInteractionType::NONE) == 0);
static_assert(static_cast<int>(WindowFrameInteractionType::MOVE) == 1);
static_assert(static_cast<int>(WindowFrameInteractionType::RESIZE_BOTTOM_LEFT) == 2);
static_assert(static_cast<int>(WindowFrameInteractionType::RESIZE_BOTTOM_RIGHT) == 3);
static_assert(static_cast<int>(WindowFrameInteractionState::STARTED) == 0);
static_assert(static_cast<int>(WindowFrameInteractionState::POINTER_RELEASED) == 1);
static_assert(static_cast<int>(WindowFrameInteractionState::COMPLETED) == 2);
static_assert(static_cast<int>(WindowFrameInteractionState::CANCELLED) == 3);
static_assert(static_cast<int>(WindowFrameShadowSource::NONE) == 0);
static_assert(static_cast<int>(WindowFrameShadowSource::COLOR) == 1);
static_assert(static_cast<int>(WindowFrameShadowSource::IMAGE) == 2);
static_assert(sizeof(WindowFrameOptions) == sizeof(void*));
static_assert(sizeof(WindowFrameDecoration) == sizeof(void*));
static_assert(sizeof(WindowFrameLayout) == sizeof(void*));
static_assert(sizeof(WindowFrameStyle) == sizeof(void*));
static_assert(sizeof(WindowFrameCommandRequest) == sizeof(void*));
static_assert(sizeof(WindowFrameGeometry) == sizeof(void*));
static_assert(sizeof(WindowFrameInteraction) == sizeof(void*));
static_assert(sizeof(DefaultWindowDecorationOptions) == sizeof(void*));

namespace
{
void NoOpClose()
{
}

// Slot bounds in window space, so control and handle columns can be compared.
float Left(Dali::Ui::View view)
{
  return view.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION).x - view.GetCurrentProperty<Vector3>(Actor::Property::SIZE).x * 0.5f;
}

float Right(Dali::Ui::View view)
{
  return view.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION).x + view.GetCurrentProperty<Vector3>(Actor::Property::SIZE).x * 0.5f;
}

float Width(Dali::Ui::View view)
{
  return view.GetCurrentProperty<Vector3>(Actor::Property::SIZE).x;
}

WindowFrameCommandDisposition InterceptMaximize(WindowFrame /*sender*/, const WindowFrameCommandRequest& request)
{
  return request.GetCommand() == WindowFrameCommand::MAXIMIZE ? WindowFrameCommandDisposition::HANDLED : WindowFrameCommandDisposition::CONTINUE_DEFAULT;
}

struct InterceptorObserver
{
  WindowFrameCommandDisposition OnCommand(WindowFrame sender, const WindowFrameCommandRequest& request)
  {
    observedSender = sender;
    return request.GetCommand() == WindowFrameCommand::CLOSE ? WindowFrameCommandDisposition::HANDLED : WindowFrameCommandDisposition::CONTINUE_DEFAULT;
  }

  WindowFrame observedSender;
};

struct ActionObserver
{
  void OnAction()
  {
    ++count;
  }

  int count{0};
};

// Releases the last DefaultWindowDecoration handle from inside the action callback, which
// destroys the border while its own click handler is still running.
struct ReleasingActionObserver
{
  void OnAction()
  {
    called = true;
    owner.Reset();
  }

  DefaultWindowDecoration owner;
  bool                    called{false};
};

struct CloseObserver
{
  void OnClose()
  {
    called = true;
  }

  bool called{false};
};

struct ReleasingCloseObserver
{
  void OnClose()
  {
    called = true;
    owner.Reset();
  }

  WindowFrame owner;
  bool        called{false};
};

struct CommandObserver : public ConnectionTracker
{
  void OnCommandProcessed(WindowFrame sender, const WindowFrameCommandRequest& request, WindowFrameCommandResult result)
  {
    observedSender  = sender;
    observedRequest = request;
    observedResult  = result;
    emitted         = true;
  }

  WindowFrame                              observedSender;
  std::optional<WindowFrameCommandRequest> observedRequest;
  WindowFrameCommandResult                 observedResult{WindowFrameCommandResult::NOT_DISPATCHED};
  bool                                     emitted{false};
};

struct StateObserver : public ConnectionTracker
{
  void OnStateChanged(WindowFrame sender, WindowFrame::WindowState state)
  {
    observedSender = sender;
    observedState  = state;
    emitted        = true;
  }

  WindowFrame              observedSender;
  WindowFrame::WindowState observedState{WindowFrame::WindowState::NORMAL};
  bool                     emitted{false};
};

} // unnamed namespace

void utc_dali_window_frame_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_window_frame_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliWindowFrameConstructorP(void)
{
  UiTestApplication application;
  WindowFrame       windowFrame;
  DALI_TEST_CHECK(!windowFrame);
  END_TEST;
}

int UtcDaliWindowFrameNewCopyMoveDownCastP(void)
{
  UiTestApplication application;
  WindowFrame       windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose));
  DALI_TEST_CHECK(windowFrame);
  DALI_TEST_CHECK(!windowFrame.IsAttached());
  windowFrame.IsFeatureEnabled(WindowFrameFeature::MAXIMIZE_RESTORE);

  WindowFrame copy(windowFrame);
  DALI_TEST_CHECK(copy == windowFrame);

  BaseHandle  base(windowFrame);
  WindowFrame downCast = WindowFrame::DownCast(base);
  DALI_TEST_CHECK(downCast == windowFrame);

  WindowFrame moved(std::move(copy));
  DALI_TEST_CHECK(moved == windowFrame);
  DALI_TEST_CHECK(!copy);
  END_TEST;
}

int UtcDaliWindowFrameDownCastN(void)
{
  UiTestApplication application;
  WindowFrame       windowFrame = WindowFrame::DownCast(BaseHandle());
  DALI_TEST_CHECK(!windowFrame);
  END_TEST;
}

int UtcDaliWindowFrameNewWithEmptyWindowN(void)
{
  UiTestApplication application;
  WindowFrame       windowFrame = WindowFrame::New(Window(), WindowFrame::CloseCallback::New(&NoOpClose));
  DALI_TEST_CHECK(!windowFrame);
  END_TEST;
}

int UtcDaliWindowFrameAttachDetachP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  options.SetFeatureEnabled(WindowFrameFeature::MAXIMIZE_RESTORE, false);
  options.SetFeatureEnabled(WindowFrameFeature::MINIMIZE, false);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);

  View contentHost = windowFrame.GetContentRoot();
  View content     = View::New();
  contentHost.Add(content);
  DALI_TEST_CHECK(contentHost);

  DALI_TEST_CHECK(windowFrame.Attach());
  DALI_TEST_CHECK(windowFrame.IsAttached());
  DALI_TEST_CHECK(windowFrame.Attach());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);

  windowFrame.Detach();
  DALI_TEST_CHECK(!windowFrame.IsAttached());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);
  DALI_TEST_CHECK(content.GetParent() == contentHost);

  DALI_TEST_CHECK(windowFrame.Attach());
  DALI_TEST_CHECK(windowFrame.IsAttached());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);
  DALI_TEST_CHECK(content.GetParent() == contentHost);
  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameRejectsSecondOwnerN(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window = application.GetWindow();
  WindowFrame first  = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);
  WindowFrame second = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);

  DALI_TEST_CHECK(first.Attach());
  DALI_TEST_CHECK(!second.Attach());
  DALI_TEST_CHECK(first.IsAttached());
  DALI_TEST_CHECK(!second.IsAttached());

  first.Detach();
  DALI_TEST_CHECK(second.Attach());
  second.Detach();
  END_TEST;
}

int UtcDaliWindowFrameRuntimeOptionsP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  options.SetInitialRestoreFrameSize(Vector2(400.0f, 260.0f));
  constexpr WindowFrameFeature FEATURES[] = {
    WindowFrameFeature::MOVE,
    WindowFrameFeature::RESIZE,
    WindowFrameFeature::MINIMIZE,
    WindowFrameFeature::MAXIMIZE_RESTORE};
  for(WindowFrameFeature feature : FEATURES)
  {
    DALI_TEST_CHECK(options.IsFeatureEnabled(feature));
    options.SetFeatureEnabled(feature, false);
    DALI_TEST_CHECK(!options.IsFeatureEnabled(feature));
    options.SetFeatureEnabled(feature, true);
    DALI_TEST_CHECK(options.IsFeatureEnabled(feature));
    options.SetFeatureEnabled(feature, false);
  }

  WindowFrameOptions copy = options;
  options.ClearInitialRestoreFrameSize();
  Vector2 restoreFrameSize(1.0f, 2.0f);
  DALI_TEST_CHECK(!options.GetInitialRestoreFrameSize(restoreFrameSize));
  DALI_TEST_EQUALS(restoreFrameSize, Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_CHECK(copy.GetInitialRestoreFrameSize(restoreFrameSize));
  DALI_TEST_EQUALS(restoreFrameSize, Vector2(400.0f, 260.0f), TEST_LOCATION);
  for(WindowFrameFeature feature : FEATURES)
  {
    DALI_TEST_CHECK(!copy.IsFeatureEnabled(feature));
  }

  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), copy);
  for(WindowFrameFeature feature : FEATURES)
  {
    DALI_TEST_CHECK(!windowFrame.IsFeatureEnabled(feature));
  }

  windowFrame.SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO);
  DALI_TEST_EQUALS(windowFrame.GetInteractiveResizePolicy(), WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO, TEST_LOCATION);
  windowFrame.SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy::DISABLED);
  windowFrame.SetOverlayEnabled(true);
  windowFrame.SetDecorationVisible(false);
  windowFrame.SetOverlayAutoHideDelay(1250u);

  DALI_TEST_EQUALS(windowFrame.GetInteractiveResizePolicy(), WindowFrameInteractiveResizePolicy::DISABLED, TEST_LOCATION);
  DALI_TEST_CHECK(windowFrame.IsOverlayEnabled());
  DALI_TEST_CHECK(!windowFrame.IsDecorationVisible());
  DALI_TEST_EQUALS(windowFrame.GetOverlayAutoHideDelay(), 1250u, TEST_LOCATION);

  DALI_TEST_CHECK(windowFrame.SetMinimumFrameSize(Vector2(200.0f, 130.0f)));
  DALI_TEST_CHECK(!windowFrame.SetMaximumFrameSize(Vector2(100.0f, 100.0f)));
  Vector2 configuredSize;
  DALI_TEST_CHECK(windowFrame.GetMinimumFrameSize(configuredSize));
  DALI_TEST_EQUALS(configuredSize, Vector2(200.0f, 130.0f), TEST_LOCATION);
  DALI_TEST_CHECK(!windowFrame.GetMaximumFrameSize(configuredSize));
  DALI_TEST_CHECK(windowFrame.SetMaximumFrameSize(Vector2(500.0f, 500.0f)));
  windowFrame.ClearMinimumFrameSize();
  windowFrame.ClearMaximumFrameSize();
  DALI_TEST_CHECK(!windowFrame.GetMinimumFrameSize(configuredSize));
  DALI_TEST_CHECK(!windowFrame.GetMaximumFrameSize(configuredSize));
  END_TEST;
}

int UtcDaliWindowFrameDecorationOwnershipP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  options.SetFeatureEnabled(WindowFrameFeature::MAXIMIZE_RESTORE, false);
  options.SetFeatureEnabled(WindowFrameFeature::MINIMIZE, false);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);

  View                  top    = View::New();
  View                  bottom = View::New();
  WindowFrameDecoration decoration;
  decoration.SetTopSlot(top);
  decoration.SetBottomSlot(bottom);

  DALI_TEST_EQUALS(windowFrame.SetDecoration(decoration, WindowFrameLayout{}), WindowFrameDecorationResult::INSTALLED, TEST_LOCATION);
  WindowFrameStyle frameStyle;
  frameStyle.SetFrameCornerRadius(Vector4(12.0f, 12.0f, 12.0f, 12.0f));
  windowFrame.SetFrameStyle(frameStyle, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
  windowFrame.Attach();

  View contentHost = windowFrame.GetContentRoot();
  View frameRoot   = View::DownCast(contentHost.GetParent());
  DALI_TEST_CHECK(contentHost);
  DALI_TEST_CHECK(frameRoot);
  DALI_TEST_EQUALS(frameRoot.GetCornerRadius(), Vector4(12.0f, 12.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_CHECK(top.GetParent());
  DALI_TEST_CHECK(bottom.GetParent());

  View                  replacementTop = View::New();
  WindowFrameDecoration replacement;
  replacement.SetTopSlot(replacementTop);
  DALI_TEST_EQUALS(windowFrame.SetDecoration(replacement, WindowFrameLayout{}), WindowFrameDecorationResult::INSTALLED, TEST_LOCATION);
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);
  DALI_TEST_CHECK(!top.GetParent());
  DALI_TEST_CHECK(!bottom.GetParent());
  DALI_TEST_CHECK(replacementTop.GetParent());

  windowFrame.Detach();
  DALI_TEST_CHECK(replacementTop.GetParent());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);
  windowFrame.Attach();
  DALI_TEST_CHECK(replacementTop.GetParent());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);

  windowFrame.ClearDecoration();
  DALI_TEST_CHECK(!replacementTop.GetParent());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);
  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameCommandInterceptorP(void)
{
  UiTestApplication application;
  CloseObserver     closeObserver;
  WindowFrame       windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&closeObserver, &CloseObserver::OnClose));

  CommandObserver     observer;
  InterceptorObserver interceptor;
  windowFrame.CommandProcessedSignal().Connect(&observer, &CommandObserver::OnCommandProcessed);
  windowFrame.SetCommandInterceptor(WindowFrame::CommandInterceptor::New(&interceptor, &InterceptorObserver::OnCommand));

  const WindowFrameCommandResult result = windowFrame.RequestClose();
  DALI_TEST_EQUALS(result, WindowFrameCommandResult::HANDLED, TEST_LOCATION);
  DALI_TEST_CHECK(interceptor.observedSender == windowFrame);
  DALI_TEST_CHECK(observer.emitted);
  DALI_TEST_CHECK(observer.observedSender == windowFrame);
  DALI_TEST_CHECK(observer.observedRequest);
  DALI_TEST_EQUALS(observer.observedRequest->GetCommand(), WindowFrameCommand::CLOSE, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.observedResult, WindowFrameCommandResult::HANDLED, TEST_LOCATION);
  WindowResizeDirection direction = WindowResizeDirection::TOP;
  DALI_TEST_CHECK(!observer.observedRequest->GetResizeDirection(direction));
  DALI_TEST_EQUALS(direction, WindowResizeDirection::TOP, TEST_LOCATION);

  WindowFrameCommandRequest resizeRequest(WindowFrameCommand::RESIZE, WindowResizeDirection::BOTTOM_LEFT);
  DALI_TEST_CHECK(resizeRequest.GetResizeDirection(direction));
  DALI_TEST_EQUALS(direction, WindowResizeDirection::BOTTOM_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!closeObserver.called);
  END_TEST;
}

int UtcDaliWindowFrameCloseCallbackCanReleaseOwnerP(void)
{
  UiTestApplication      application;
  ReleasingCloseObserver closeObserver;
  closeObserver.owner = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&closeObserver, &ReleasingCloseObserver::OnClose));

  CommandObserver observer;
  closeObserver.owner.CommandProcessedSignal().Connect(&observer, &CommandObserver::OnCommandProcessed);

  const WindowFrameCommandResult result = closeObserver.owner.RequestClose();
  DALI_TEST_EQUALS(result, WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  DALI_TEST_CHECK(closeObserver.called);
  DALI_TEST_CHECK(!closeObserver.owner);
  DALI_TEST_CHECK(observer.emitted);
  DALI_TEST_CHECK(observer.observedSender);
  DALI_TEST_CHECK(observer.observedRequest);
  DALI_TEST_EQUALS(observer.observedRequest->GetCommand(), WindowFrameCommand::CLOSE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameToggleMaximizeP(void)
{
  UiTestApplication application;
  WindowFrame       windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose));
  windowFrame.SetCommandInterceptor(WindowFrame::CommandInterceptor::New(&InterceptMaximize));

  CommandObserver observer;
  windowFrame.CommandProcessedSignal().Connect(&observer, &CommandObserver::OnCommandProcessed);

  // An interceptor that takes the command over reports HANDLED and no native
  // request is dispatched.
  DALI_TEST_EQUALS(windowFrame.ToggleMaximize(), WindowFrameCommandResult::HANDLED, TEST_LOCATION);
  DALI_TEST_CHECK(observer.observedSender == windowFrame);
  DALI_TEST_CHECK(observer.observedRequest);
  DALI_TEST_EQUALS(observer.observedRequest->GetCommand(), WindowFrameCommand::MAXIMIZE, TEST_LOCATION);
  DALI_TEST_CHECK(!windowFrame.IsMaximized());
  END_TEST;
}

int UtcDaliWindowFrameSetDecorationLayoutP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);

  View                  top = View::New();
  WindowFrameDecoration decoration;
  decoration.SetTopSlot(top);

  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(0.0f, 0.0f, 64.0f, 0.0f));
  DALI_TEST_EQUALS(windowFrame.SetDecoration(decoration, layout), WindowFrameDecorationResult::INSTALLED, TEST_LOCATION);
  windowFrame.Attach();

  View contentHost = windowFrame.GetContentRoot();

  WindowFrameLayout taller;
  taller.SetDecorationInsets(Insets(0.0f, 0.0f, 96.0f, 0.0f));
  windowFrame.SetDecorationLayout(taller, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);

  // The installed slot actors and the content root survive a layout change.
  DALI_TEST_CHECK(top.GetParent());
  DALI_TEST_CHECK(windowFrame.GetContentRoot() == contentHost);

  // The installed decoration can also be resubmitted with new layout.
  DALI_TEST_EQUALS(windowFrame.SetDecoration(decoration, layout), WindowFrameDecorationResult::INSTALLED, TEST_LOCATION);
  DALI_TEST_CHECK(top.GetParent());

  // A slot owned by anything else is still rejected.
  View                  foreignParent = View::New();
  View                  foreignTop    = View::New();
  WindowFrameDecoration foreign;
  foreign.SetTopSlot(foreignTop);
  foreignParent.Add(foreignTop);
  DALI_TEST_EQUALS(windowFrame.SetDecoration(foreign, layout), WindowFrameDecorationResult::SLOT_ALREADY_PARENTED, TEST_LOCATION);

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameKeepContentSizeBeforeAttachP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 500, 500));
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose));

  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(0.0f, 0.0f, 50.0f, 0.0f));
  windowFrame.SetDecorationLayout(layout);

  DALI_TEST_EQUALS(windowFrame.GetContentSize(), Vector2(500.0f, 500.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(window.GetPositionSize().height, 550, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameKeepContentSizeAcrossDeferredChangesP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 500, 500));
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);
  windowFrame.Attach();

  WindowFrameLayout first;
  first.SetDecorationInsets(Insets(0.0f, 0.0f, 50.0f, 0.0f));
  windowFrame.SetDecorationLayout(first);

  WindowFrameLayout second;
  second.SetDecorationInsets(Insets(0.0f, 0.0f, 80.0f, 0.0f));
  windowFrame.SetDecorationLayout(second);
  Test::EmitGlobalTimerSignal();

  DALI_TEST_EQUALS(windowFrame.GetContentSize(), Vector2(500.0f, 500.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(window.GetPositionSize().height, 580, TEST_LOCATION);

  WindowFrameStyle style;
  style.SetShadow(Dali::Ui::Shadow());
  style.SetShadowOutsets(Insets(10.0f, 10.0f, 10.0f, 10.0f));
  windowFrame.SetFrameStyle(style, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);

  // Size policy applies only to the call that receives it. The two layout
  // calls above preserve content, while this independent style call preserves
  // the current 500x580 surface and lets the content shrink around the shadow.
  DALI_TEST_EQUALS(window.GetPositionSize().width, 500, TEST_LOCATION);
  DALI_TEST_EQUALS(window.GetPositionSize().height, 580, TEST_LOCATION);
  DALI_TEST_EQUALS(windowFrame.GetContentSize(), Vector2(480.0f, 480.0f), TEST_LOCATION);
  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameMaximizeRestoreRoundTripP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window      = application.GetWindow();
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);
  windowFrame.Attach();

  StateObserver observer;
  StateObserver presented;
  windowFrame.WindowStateChangedSignal().Connect(&observer, &StateObserver::OnStateChanged);
  windowFrame.WindowStatePresentedSignal().Connect(&presented, &StateObserver::OnStateChanged);

  // A request is only dispatched here; the compositor still owns confirmation.
  DALI_TEST_EQUALS(windowFrame.RequestMaximize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);

  // The window system reports the new state with its geometry.
  window.ResizedSignal().Emit(window, Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());
  DALI_TEST_EQUALS(windowFrame.GetWindowState(), WindowFrame::WindowState::MAXIMIZED, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.observedState, WindowFrame::WindowState::MAXIMIZED, TEST_LOCATION);
  DALI_TEST_CHECK(observer.observedSender == windowFrame);

  // Confirmation and presentation are separate steps.
  DALI_TEST_CHECK(!presented.emitted);
  Test::EmitFramePresented(window);
  DALI_TEST_CHECK(presented.emitted);
  DALI_TEST_CHECK(presented.observedSender == windowFrame);
  DALI_TEST_EQUALS(presented.observedState, WindowFrame::WindowState::MAXIMIZED, TEST_LOCATION);

  // Repeating a request the window is already in still reaches the window
  // system, and reports no state change of its own.
  DALI_TEST_EQUALS(windowFrame.RequestMaximize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  DALI_TEST_CHECK(windowFrame.IsMaximized());

  DALI_TEST_EQUALS(windowFrame.RequestRestore(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  window.ResizedSignal().Emit(window, Window::WindowSize(400, 260));
  DALI_TEST_CHECK(!windowFrame.IsMaximized());
  DALI_TEST_EQUALS(observer.observedState, WindowFrame::WindowState::NORMAL, TEST_LOCATION);

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameSizeConstraintConflictN(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);
  windowFrame.Attach();

  DALI_TEST_CHECK(windowFrame.SetMinimumFrameSize(Vector2(200.0f, 130.0f)));
  DALI_TEST_CHECK(windowFrame.SetMaximumFrameSize(Vector2(1400.0f, 900.0f)));

  // A maximum below the configured minimum is rejected without changing the
  // constraints that are already in effect.
  DALI_TEST_CHECK(!windowFrame.SetMaximumFrameSize(Vector2(100.0f, 100.0f)));
  Vector2 maximum;
  DALI_TEST_CHECK(windowFrame.GetMaximumFrameSize(maximum));
  DALI_TEST_EQUALS(maximum, Vector2(1400.0f, 900.0f), TEST_LOCATION);

  DALI_TEST_CHECK(!windowFrame.SetMinimumFrameSize(Vector2(2000.0f, 2000.0f)));
  Vector2 minimum;
  DALI_TEST_CHECK(windowFrame.GetMinimumFrameSize(minimum));
  DALI_TEST_EQUALS(minimum, Vector2(200.0f, 130.0f), TEST_LOCATION);

  windowFrame.ClearMaximumFrameSize();
  DALI_TEST_CHECK(!windowFrame.GetMaximumFrameSize(maximum));
  DALI_TEST_CHECK(windowFrame.SetMinimumFrameSize(Vector2(2000.0f, 2000.0f)));

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliWindowFrameOverlayInsetsAndAutoHideP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window      = application.GetWindow();
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);

  View                  top = View::New();
  WindowFrameDecoration decoration;
  decoration.SetTopSlot(top);
  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(0.0f, 0.0f, 64.0f, 0.0f));
  DALI_TEST_EQUALS(windowFrame.SetDecoration(decoration, layout), WindowFrameDecorationResult::INSTALLED, TEST_LOCATION);
  windowFrame.Attach();

  // Decoration sits outside the content bounds, so it obscures nothing.
  DALI_TEST_EQUALS(windowFrame.GetGeometry().GetDecorationOverlayInsets().top, 0.0f, TEST_LOCATION);

  windowFrame.SetOverlayEnabled(true);
  DALI_TEST_CHECK(windowFrame.IsOverlayEnabled());
  DALI_TEST_CHECK(!windowFrame.IsMaximized());
  // Overlay layout only takes effect while maximized.
  DALI_TEST_EQUALS(windowFrame.GetGeometry().GetDecorationOverlayInsets().top, 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(windowFrame.RequestMaximize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  window.ResizedSignal().Emit(window, Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());

  // Overlay layout gives the content the whole frame and draws decoration on
  // top of it. Anything that shrinks the content by the decoration insets here
  // has turned overlay layout back into ordinary inset layout.
  WindowFrameGeometry geometry = windowFrame.GetGeometry();
  DALI_TEST_EQUALS(geometry.GetDecorationOverlayInsets().top, 64.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(geometry.GetContentBounds().height, geometry.GetFrameBounds().height, TEST_LOCATION);
  DALI_TEST_EQUALS(geometry.GetContentBounds().y, geometry.GetFrameBounds().y, TEST_LOCATION);

  // Decoration larger than the frame is clamped when it is laid out, so the
  // reported insets must follow the layout rather than the requested layout.
  WindowFrameLayout oversized;
  oversized.SetDecorationInsets(Insets(0.0f, 0.0f, 100000.0f, 100.0f));
  windowFrame.SetDecorationLayout(oversized, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
  WindowFrameGeometry clamped = windowFrame.GetGeometry();
  DALI_TEST_EQUALS(clamped.GetDecorationOverlayInsets().top, clamped.GetFrameBounds().height, TEST_LOCATION);
  DALI_TEST_EQUALS(clamped.GetDecorationOverlayInsets().bottom, 0.0f, TEST_LOCATION);
  windowFrame.SetDecorationLayout(layout, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);

  // The same window without overlay keeps the decoration outside the content.
  windowFrame.SetOverlayEnabled(false);
  WindowFrameGeometry inset = windowFrame.GetGeometry();
  DALI_TEST_EQUALS(inset.GetDecorationOverlayInsets().top, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(inset.GetContentBounds().height, inset.GetFrameBounds().height - 64.0f, TEST_LOCATION);
  windowFrame.SetOverlayEnabled(true);
  DALI_TEST_EQUALS(windowFrame.GetGeometry().GetContentBounds().height, geometry.GetFrameBounds().height, TEST_LOCATION);

  // Maximized geometry is compositor-owned, so a size request is refused.
  DALI_TEST_CHECK(!windowFrame.RequestContentResize(Vector2(300.0f, 200.0f)));

  // A zero delay turns auto-hide off and reveals decoration it had hidden.
  windowFrame.SetOverlayAutoHideDelay(0u);
  DALI_TEST_EQUALS(windowFrame.GetOverlayAutoHideDelay(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!windowFrame.IsOverlayAutoHidden());
  DALI_TEST_CHECK(windowFrame.IsDecorationVisible());

  windowFrame.SetOverlayAutoHideDelay(1u);
  DALI_TEST_EQUALS(windowFrame.GetOverlayAutoHideDelay(), 1u, TEST_LOCATION);

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window      = application.GetWindow();
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);

  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);
  DALI_TEST_CHECK(defaultDecoration);
  DALI_TEST_CHECK(DefaultWindowDecoration::DownCast(BaseHandle(defaultDecoration)) == defaultDecoration);

  // A complete frame is installed: bottom bar, side edges, both resize handles
  // and all three controls.
  WindowFrameDecoration decoration = defaultDecoration.GetDecoration();
  DALI_TEST_CHECK(decoration.GetBottomSlot());
  DALI_TEST_CHECK(decoration.GetTopSlot());
  DALI_TEST_CHECK(decoration.GetLeftSlot());
  DALI_TEST_CHECK(decoration.GetRightSlot());
  DALI_TEST_CHECK(decoration.GetMoveRegion());
  DALI_TEST_CHECK(decoration.GetMinimizeControl());
  DALI_TEST_CHECK(decoration.GetMaximizeRestoreControl());
  DALI_TEST_CHECK(decoration.GetCloseControl());
  DALI_TEST_CHECK(decoration.GetBottomLeftResizeHandle());
  DALI_TEST_CHECK(decoration.GetBottomRightResizeHandle());
  DALI_TEST_CHECK(defaultDecoration.GetMoveRegion() == decoration.GetMoveRegion());

  windowFrame.Attach();
  DALI_TEST_CHECK(decoration.GetBottomSlot().GetParent());

  // The frame encloses the content on every side, and the area the application
  // draws into is left transparent by the border.
  WindowFrameGeometry geometry = windowFrame.GetGeometry();
  DALI_TEST_CHECK(geometry.GetContentBounds().y > geometry.GetFrameBounds().y);
  DALI_TEST_CHECK(geometry.GetContentBounds().height < geometry.GetFrameBounds().height);
  DALI_TEST_EQUALS(windowFrame.GetFrameStyle().GetFrameBackgroundColor().GetRgba().a, 0.0f, TEST_LOCATION);

  // The shadow is cast into surface space outside the frame, and it is cut out
  // so a transparent frame does not let it darken the content area.
  WindowFrameStyle style = windowFrame.GetFrameStyle();
  DALI_TEST_CHECK(style.GetShadowSource() == WindowFrameShadowSource::COLOR);
  DALI_TEST_CHECK(style.GetShadowOutsets().top > 0.0f);
  DALI_TEST_CHECK(style.GetShadow().GetCutoutPolicy() == CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  DALI_TEST_CHECK(geometry.GetFrameBounds().x > 0.0f);
  DALI_TEST_CHECK(geometry.GetFrameBounds().y > 0.0f);

  // Non-interactive title content can be added to the move region.
  View title = View::New();
  defaultDecoration.GetMoveRegion().Add(title);
  DALI_TEST_CHECK(title.GetParent() == defaultDecoration.GetMoveRegion());

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationOptionsP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  DefaultWindowDecorationOptions options;
  DALI_TEST_EQUALS(options.GetBarHeight(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetEdgeThickness(), 5.0f, TEST_LOCATION);
  DALI_TEST_CHECK(options.GetTheme() == DefaultWindowDecorationTheme::DARK);
  DALI_TEST_CHECK(options.IsMinimizeControlEnabled() && options.IsMaximizeRestoreControlEnabled() &&
                  options.IsCloseControlEnabled() && options.IsResizeHandlesEnabled());

  options.SetTheme(DefaultWindowDecorationTheme::LIGHT);
  options.SetBarHeight(36.0f);
  options.SetEdgeThickness(0.0f);
  options.SetMinimizeControlEnabled(false);
  options.SetResizeHandlesEnabled(false);
  options.SetShadowEnabled(false);
  options.SetBackgroundColor(UiColor(0x102030));
  DALI_TEST_EQUALS(options.GetBarHeight(), 36.0f, TEST_LOCATION);

  // A negative size is clamped rather than producing an inverted frame.
  options.SetBarHeight(-10.0f);
  DALI_TEST_EQUALS(options.GetBarHeight(), 0.0f, TEST_LOCATION);
  options.SetBarHeight(36.0f);

  DefaultWindowDecorationOptions copy(options);
  DALI_TEST_EQUALS(copy.GetBarHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_CHECK(copy.GetTheme() == DefaultWindowDecorationTheme::LIGHT);

  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame, options);
  DALI_TEST_CHECK(defaultDecoration);

  WindowFrameDecoration decoration = defaultDecoration.GetDecoration();
  DALI_TEST_CHECK(!decoration.GetMinimizeControl());
  DALI_TEST_CHECK(!decoration.GetBottomLeftResizeHandle());
  DALI_TEST_CHECK(!decoration.GetBottomRightResizeHandle());
  DALI_TEST_CHECK(!decoration.GetLeftSlot());
  DALI_TEST_CHECK(!decoration.GetTopSlot());
  DALI_TEST_CHECK(decoration.GetMaximizeRestoreControl());
  DALI_TEST_CHECK(decoration.GetCloseControl());
  DALI_TEST_CHECK(windowFrame.GetFrameStyle().GetShadowSource() == WindowFrameShadowSource::NONE);
  END_TEST;
}

int UtcDaliDefaultWindowDecorationTracksWindowStateP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window      = application.GetWindow();
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);

  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);
  windowFrame.Attach();

  View      maximizeRestoreControl = defaultDecoration.GetDecoration().GetMaximizeRestoreControl();
  ImageView maximizeIcon           = ImageView::DownCast(maximizeRestoreControl.GetChildAt(0u));
  DALI_TEST_CHECK(maximizeIcon);

  const auto endsWith = [](const Dali::String& url, const char* name)
  {
    const std::string text(url.CStr());
    const std::string suffix(name);
    return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  DALI_TEST_CHECK(endsWith(maximizeIcon.GetResourceUrl(), "maximize.png"));

  // The icon follows the state the window system reports, so it still shows
  // maximize until that state arrives.
  DALI_TEST_EQUALS(windowFrame.RequestMaximize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  DALI_TEST_CHECK(endsWith(maximizeIcon.GetResourceUrl(), "maximize.png"));

  window.ResizedSignal().Emit(window, Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());
  DALI_TEST_CHECK(endsWith(maximizeIcon.GetResourceUrl(), "restore.png"));

  // A maximized window is square, and the decoration paints the outer corners
  // itself, so its radius has to follow the state too. Leaving it rounded cuts
  // transparent notches out of the corners of a maximized frame.
  View bottom = defaultDecoration.GetDecoration().GetBottomSlot();
  View top    = defaultDecoration.GetDecoration().GetTopSlot();
  DALI_TEST_EQUALS(bottom.GetCornerRadius(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(top.GetCornerRadius(), Vector4::ZERO, TEST_LOCATION);

  DALI_TEST_EQUALS(windowFrame.RequestRestore(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  window.ResizedSignal().Emit(window, Window::WindowSize(400, 260));
  DALI_TEST_CHECK(endsWith(maximizeIcon.GetResourceUrl(), "maximize.png"));
  DALI_TEST_CHECK(bottom.GetCornerRadius() != Vector4::ZERO);
  DALI_TEST_CHECK(top.GetCornerRadius() != Vector4::ZERO);

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationMinimumFrameSizeP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame             windowFrame       = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);

  // Two 50 handle columns, three 44 control columns, and a 44 move region. The
  // side edges do not consume bottom-bar width.
  const Vector2 minimum = defaultDecoration.GetMinimumFrameSize();
  DALI_TEST_EQUALS(minimum.width, 276.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(minimum.height, 55.0f, TEST_LOCATION);

  windowFrame.Attach();
  DALI_TEST_CHECK(windowFrame.SetMinimumFrameSize(minimum));

  // At the reported minimum the controls and the corner handles do not overlap,
  // and the move region still has a draggable width.
  application.GetWindow().SetPositionSize(PositionSize(0, 0, static_cast<int>(minimum.width) + 24, 200));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  WindowFrameDecoration decoration = defaultDecoration.GetDecoration();
  const float           closeRight = Right(decoration.GetCloseControl());
  const float           handleLeft = Left(decoration.GetBottomRightResizeHandle());
  DALI_TEST_CHECK(closeRight <= handleLeft + Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(Width(decoration.GetMoveRegion()) > 0.0f);
  END_TEST;
}

int UtcDaliDefaultWindowDecorationMinimumFrameSizeFollowsOptionsP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  // Removing controls and handles removes their reserved columns, so a product
  // that trims the bar is not held to the full-width minimum.
  DefaultWindowDecorationOptions options;
  options.SetMinimizeControlEnabled(false);
  options.SetMaximizeRestoreControlEnabled(false);
  options.SetResizeHandlesEnabled(false);
  options.SetEdgeThickness(0.0f);
  DefaultWindowDecoration trimmed = DefaultWindowDecoration::New(windowFrame, options);

  const Vector2 minimum = trimmed.GetMinimumFrameSize();
  DALI_TEST_EQUALS(minimum.width, 88.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(minimum.height, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameOppositeCommandBeforeStateArrivesP(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), options);
  windowFrame.Attach();

  // Both requests reach the window system even though the maximized state has
  // not been reported when the restore is sent. Gating on the observed state
  // would drop the restore and lose the application's last intent.
  DALI_TEST_EQUALS(windowFrame.RequestMaximize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  DALI_TEST_EQUALS(windowFrame.RequestRestore(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);

  // The restore is what lands, so the window is not maximized.
  application.GetWindow().SetPositionSize(PositionSize(0, 0, 480, 800));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!windowFrame.IsMaximized());

  // The same holds for minimize, which has no state to compare against either.
  DALI_TEST_EQUALS(windowFrame.RequestMinimize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  DALI_TEST_EQUALS(windowFrame.RequestMinimize(), WindowFrameCommandResult::DISPATCHED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameMoveRegionChildInputWhileMaximizedN(void)
{
  UiTestApplication  application;
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  Window      window      = application.GetWindow();
  WindowFrame windowFrame = WindowFrame::New(window, WindowFrame::CloseCallback::New(&NoOpClose), options);

  WindowFrameDecoration decoration;
  Dali::Ui::View        bottom     = Dali::Ui::View::New();
  Dali::Ui::View        moveRegion = Dali::Ui::View::New();
  Dali::Ui::View        appButton  = Dali::Ui::View::New();
  moveRegion.Add(appButton);
  bottom.Add(moveRegion);
  decoration.SetBottomSlot(bottom);
  decoration.SetMoveRegion(moveRegion);

  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(0.0f, 0.0f, 0.0f, 50.0f));
  DALI_TEST_CHECK(windowFrame.SetDecoration(decoration, layout) == WindowFrameDecorationResult::INSTALLED);
  windowFrame.Attach();
  DALI_TEST_CHECK(moveRegion.IsSensitive());

  // A maximized window cannot be moved, so the move region stops taking input.
  // An insensitive actor also hides its children from hit testing, so anything
  // an application parented to the move region goes with it. Decoration a
  // product wants clickable while maximized therefore belongs in its own slot
  // area, beside the move region rather than inside it.
  windowFrame.RequestMaximize();
  window.ResizedSignal().Emit(window, Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());
  DALI_TEST_CHECK(!moveRegion.IsSensitive());

  // The child's own flag is untouched, which is what makes this hard to spot:
  // the button still reports itself sensitive while receiving nothing.
  DALI_TEST_CHECK(appButton.IsSensitive());

  windowFrame.RequestRestore();
  window.ResizedSignal().Emit(window, Window::WindowSize(400, 260));
  DALI_TEST_CHECK(moveRegion.IsSensitive());

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationTopAreaHeightP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  DefaultWindowDecorationOptions options;
  DALI_TEST_EQUALS(options.GetTopAreaHeight(), 0.0f, TEST_LOCATION);

  options.SetTopAreaHeight(40.0f);
  DALI_TEST_EQUALS(options.GetTopAreaHeight(), 40.0f, TEST_LOCATION);

  // A negative height is clamped rather than inverting the frame.
  options.SetTopAreaHeight(-10.0f);
  DALI_TEST_EQUALS(options.GetTopAreaHeight(), 0.0f, TEST_LOCATION);
  options.SetTopAreaHeight(40.0f);

  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame, options);
  DALI_TEST_CHECK(defaultDecoration);
  windowFrame.Attach();

  // The top inset follows the area, so the content loses that height instead of
  // the 5 the thin edge would have taken.
  const Vector2 frame   = windowFrame.GetFrameSize();
  const Vector2 content = windowFrame.GetContentSize();
  DALI_TEST_EQUALS(frame.height - content.height, 40.0f + options.GetBarHeight(), TEST_LOCATION);

  // The minimum reports the area too.
  DALI_TEST_EQUALS(defaultDecoration.GetMinimumFrameSize().height, options.GetBarHeight() + 40.0f, TEST_LOCATION);

  // The area is reachable, is the installed top slot, and takes input, unlike
  // the thin edge it replaces.
  Dali::Ui::View topArea = defaultDecoration.GetTopArea();
  DALI_TEST_CHECK(topArea);
  DALI_TEST_CHECK(topArea == defaultDecoration.GetDecoration().GetTopSlot());
  DALI_TEST_CHECK(topArea.IsSensitive());
  DALI_TEST_CHECK(topArea != defaultDecoration.GetMoveRegion());

  // Its children stay hittable while maximized, which separates it from the
  // move region.
  windowFrame.RequestMaximize();
  application.GetWindow().ResizedSignal().Emit(application.GetWindow(), Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());
  DALI_TEST_CHECK(topArea.IsSensitive());
  DALI_TEST_CHECK(!defaultDecoration.GetMoveRegion().IsSensitive());

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationTopAreaAbsentWithoutHeightN(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  // The default top is a thin decorative edge, so there is no area to hand out
  // and it must not swallow input.
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);
  DALI_TEST_CHECK(!defaultDecoration.GetTopArea());
  DALI_TEST_CHECK(defaultDecoration.GetDecoration().GetTopSlot());
  DALI_TEST_CHECK(!defaultDecoration.GetDecoration().GetTopSlot().IsSensitive());
  END_TEST;
}

int UtcDaliDefaultWindowDecorationTopAreaHeightBelowEdgeP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  // The frame still needs its border, so a smaller area is raised to the edge.
  DefaultWindowDecorationOptions options;
  options.SetEdgeThickness(5.0f);
  options.SetTopAreaHeight(2.0f);
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame, options);

  DALI_TEST_EQUALS(defaultDecoration.GetMinimumFrameSize().height, options.GetBarHeight() + 5.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDefaultWindowDecorationAddBarActionP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame             windowFrame       = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);

  // Whatever the application already put in the move region survives, because
  // the bar is relaid out rather than rebuilt.
  Dali::Ui::View title = Dali::Ui::View::New();
  defaultDecoration.GetMoveRegion().Add(title);

  const Vector2 before = defaultDecoration.GetMinimumFrameSize();

  ActionObserver observer;
  Dali::Ui::View action = defaultDecoration.AddBarAction("theme.png", Dali::Ui::Callback<void()>::New(&observer, &ActionObserver::OnAction));
  DALI_TEST_CHECK(action);
  DALI_TEST_CHECK(title.GetParent() == defaultDecoration.GetMoveRegion());

  // The column widens what the bar needs.
  DALI_TEST_EQUALS(defaultDecoration.GetMinimumFrameSize().width, before.width + 44.0f, TEST_LOCATION);

  // Clicking it reaches the supplied callback.
  Property::Map attributes;
  action.DoAction("activate", attributes);
  DALI_TEST_EQUALS(observer.count, 1, TEST_LOCATION);

  windowFrame.Attach();
  windowFrame.SetMinimumFrameSize(defaultDecoration.GetMinimumFrameSize());
  application.GetWindow().SetPositionSize(PositionSize(0, 0, 420, 220));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // It sits between the move region and the window controls, and never under
  // the corner handle.
  WindowFrameDecoration decoration = defaultDecoration.GetDecoration();
  DALI_TEST_CHECK(Left(action) >= Right(decoration.GetMoveRegion()) - Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(Right(action) <= Left(decoration.GetMinimizeControl()) + Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(Right(decoration.GetCloseControl()) <= Left(decoration.GetBottomRightResizeHandle()) + Math::MACHINE_EPSILON_1000);

  // Unlike the move region it keeps taking input while maximized.
  windowFrame.RequestMaximize();
  application.GetWindow().ResizedSignal().Emit(application.GetWindow(), Window::WindowSize(480, 800));
  DALI_TEST_CHECK(windowFrame.IsMaximized());
  DALI_TEST_CHECK(action.IsSensitive());

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationAddBarActionOrderP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame             windowFrame       = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame);

  ActionObserver first;
  ActionObserver second;
  Dali::Ui::View firstAction  = defaultDecoration.AddBarAction("theme.png", Dali::Ui::Callback<void()>::New(&first, &ActionObserver::OnAction));
  Dali::Ui::View secondAction = defaultDecoration.AddBarAction("theme.png", Dali::Ui::Callback<void()>::New(&second, &ActionObserver::OnAction));

  windowFrame.Attach();
  windowFrame.SetMinimumFrameSize(defaultDecoration.GetMinimumFrameSize());
  application.GetWindow().SetPositionSize(PositionSize(0, 0, 460, 220));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Added left to right, and the controls moved along to make room.
  DALI_TEST_CHECK(Right(firstAction) <= Left(secondAction) + Math::MACHINE_EPSILON_1000);
  DALI_TEST_CHECK(Right(secondAction) <= Left(defaultDecoration.GetDecoration().GetMinimizeControl()) + Math::MACHINE_EPSILON_1000);

  // Each button reaches its own callback, so the index bookkeeping is right.
  Property::Map attributes;
  secondAction.DoAction("activate", attributes);
  DALI_TEST_EQUALS(first.count, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(second.count, 1, TEST_LOCATION);
  firstAction.DoAction("activate", attributes);
  DALI_TEST_EQUALS(first.count, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(second.count, 1, TEST_LOCATION);

  windowFrame.Detach();
  END_TEST;
}

int UtcDaliDefaultWindowDecorationBarActionIconIsTheCallersP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  // A light theme tints the shipped white glyphs. An application icon is not a
  // shipped glyph, so neither the asset directory nor the tint applies to it.
  DefaultWindowDecorationOptions options;
  options.SetTheme(DefaultWindowDecorationTheme::LIGHT);
  DefaultWindowDecoration defaultDecoration = DefaultWindowDecoration::New(windowFrame, options);

  ActionObserver observer;
  const char*    url    = "/opt/usr/apps/example/res/share.png";
  Dali::Ui::View action = defaultDecoration.AddBarAction(url, Dali::Ui::Callback<void()>::New(&observer, &ActionObserver::OnAction));
  DALI_TEST_CHECK(action);

  ImageView icon = ImageView::DownCast(action.GetChildAt(0u));
  DALI_TEST_CHECK(icon);
  DALI_TEST_EQUALS(icon.GetResourceUrl().CStr(), url, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<Vector4>(Actor::Property::COLOR), Color::WHITE, TEST_LOCATION);

  // The shipped controls still resolve against the component directory and do
  // follow the theme.
  ImageView closeIcon = ImageView::DownCast(defaultDecoration.GetDecoration().GetCloseControl().GetChildAt(0u));
  DALI_TEST_CHECK(closeIcon);
  DALI_TEST_CHECK(std::string(closeIcon.GetResourceUrl().CStr()).find("close.png") != std::string::npos);
  DALI_TEST_CHECK(closeIcon.GetResourceUrl().CStr() != std::string(url));
  DALI_TEST_CHECK(closeIcon.GetProperty<Vector4>(Actor::Property::COLOR) != Color::WHITE);
  END_TEST;
}

int UtcDaliDefaultWindowDecorationBarActionReleasesOwnerP(void)
{
  UiTestApplication  application;
  WindowFrameOptions windowOptions;
  windowOptions.SetInitialStatePolicy(WindowFrameInitialStatePolicy::USE_CURRENT);
  WindowFrame windowFrame = WindowFrame::New(application.GetWindow(), WindowFrame::CloseCallback::New(&NoOpClose), windowOptions);

  ReleasingActionObserver observer;
  observer.owner        = DefaultWindowDecoration::New(windowFrame);
  Dali::Ui::View action = observer.owner.AddBarAction("theme.png", Dali::Ui::Callback<void()>::New(&observer, &ReleasingActionObserver::OnAction));

  // The callback drops the last handle, so the border is destroyed while its own
  // click handler is still on the stack. The handler holds one on itself for the
  // duration, so this returns rather than using freed memory.
  Property::Map attributes;
  action.DoAction("activate", attributes);
  DALI_TEST_CHECK(observer.called);
  DALI_TEST_CHECK(!observer.owner);
  END_TEST;
}

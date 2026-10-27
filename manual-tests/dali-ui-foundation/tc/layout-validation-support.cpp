/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-support.h"
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/locale-numeric-guard.h>
#include <dali/public-api/adaptor-framework/capture.h>
#include <dali/public-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/adaptor-framework/ui-context.h>
#include <dali/public-api/images/pixel.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

namespace LayoutValidation
{
namespace
{
constexpr std::size_t MAX_SHOWN_FAILURES = 8;
constexpr uint32_t    TICK_MS             = 30;
constexpr double      SETTLE_TOLERANCE = 0.01;
template<std::size_t N>
void Copy(char (&out)[N], const char* value)
{
  std::snprintf(out, N, "%s", value ? value : "");
}
std::string Escape(const char* value)
{
  std::string result;
  for(const unsigned char ch : std::string(value))
  {
    if(ch == '\\' || ch == '"')
    {
      result += '\\';
      result += static_cast<char>(ch);
    }
    else if(ch == '\n')
      result += "\\n";
    else if(ch == '\r')
      result += "\\r";
    else if(ch == '\t')
      result += "\\t";
    else if(ch < 32)
    {
      char hex[7];
      std::snprintf(hex, sizeof(hex), "\\u%04x", ch);
      result += hex;
    }
    else
      result += static_cast<char>(ch);
  }
  return result;
}
Label TextLabel(const char* text, float size)
{
  Label label = Label::New(text);
  label.SetFontSize(size);
  label.SetTextColor(UiColor(0x202124u));
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}
std::string RgbText(const UiColor& color)
{
  const Vector4 rgba = color.GetRgba();
  char          text[48];
  std::snprintf(text, sizeof(text), "%ld,%ld,%ld", std::lround(rgba.r * 255.0f), std::lround(rgba.g * 255.0f), std::lround(rgba.b * 255.0f));
  return text;
}
double ChannelError(const UiColor& a, const UiColor& b)
{
  const Vector4 x = a.GetRgba(), y = b.GetRgba();
  return std::max({std::abs(std::lround(x.r * 255.0f) - std::lround(y.r * 255.0f)),
                   std::abs(std::lround(x.g * 255.0f) - std::lround(y.g * 255.0f)),
                   std::abs(std::lround(x.b * 255.0f) - std::lround(y.b * 255.0f))});
}
constexpr uint32_t PALETTE[] = {0xE53935u, 0x1E88E5u, 0x43A047u, 0xFB8C00u, 0x8E24AAu, 0x00ACC1u, 0xFDD835u, 0x6D4C41u, 0x3949ABu, 0xD81B60u, 0x7CB342u, 0x00897Bu};
constexpr uint32_t STAGE_COLOR = 0xCFD8DCu;
constexpr uint32_t FIXTURE_PALETTE[] = {0xFFE0B2u, 0xC8E6C9u, 0xBBDEFBu, 0xF8BBD0u, 0xD1C4E9u, 0xB2EBF2u, 0xFFF9C4u, 0xD7CCC8u};
constexpr uint32_t HOST_COLOR      = 0xFFFFFFu;
bool HasBackground(View view)
{
  return view.GetProperty<Property::Map>(Dali::Ui::Integration::View::Property::BACKGROUND).Count() > 0;
}
// Views that draw their own content keep their visuals untouched: a background visual
// on an ImageView adds a second ResourceReady emission, and text is visible as is.
bool DrawsOwnContent(View view)
{
  return ImageView::DownCast(view) || AnimatedImageView::DownCast(view) || LottieAnimationView::DownCast(view) || Label::DownCast(view);
}
void ColorizeSubtree(View view, uint32_t depth, uint32_t index)
{
  if(!HasBackground(view) && !DrawsOwnContent(view)) view.SetBackgroundColor(LayoutValidation::FixtureColor(depth * 3u + index));
  const uint32_t count = view.GetChildViewCount();
  for(uint32_t i = 0; i < count; ++i) ColorizeSubtree(view.GetChildViewAt(i), depth + 1u, i);
}
} // namespace

Run::Run(View host, Window window)
: mHost(host),
  mWindow(window),
  mRows(new Result[MAX_RESULT_ROWS])
{
  mOwned.reserve(1024);
  mTimers.reserve(32);
  mCleanup.reserve(64);
  mRequiredViews.reserve(1024);
}
Run::~Run()
{
  Cleanup();
}
bool Run::Cleanup()
{
  if(mCleaned) return mCleanupPassed;
  mCleaned = true;
  mPending = false;
  ++mActionEpoch;
  auto guard = [this](const std::function<void()>& action) {
    try
    {
      action();
    }
    catch(...)
    {
      mCleanupPassed = false;
      const std::string record = "LR_CLEANUP_ERROR {\"tc\":\"" + Escape(mCaseId.c_str()) + "\",\"pid\":" + std::to_string(getpid()) + ",\"run\":" + std::to_string(mRunId) + ",\"scenario\":\"" + Escape(mScenarioId.c_str()) + "\"}\n";
      std::fputs(record.c_str(), stdout);
      std::fflush(stdout);
      std::fputs(record.c_str(), stderr);
      std::fflush(stderr);
    }
  };
  guard([this] { CancelFrame(); ReleaseFrameConnections(); mFenceConnections.DisconnectAll(); DisconnectAll(); });
  guard([this] { DisarmFenceWatchdog(); if(mSettleTimer) mSettleTimer.Stop(); if(mCaptureTimer) mCaptureTimer.Stop(); mCapture.Reset(); });
  for(auto& timer : mTimers) guard([&timer] { if(timer) timer.Stop(); });
  for(auto it = mCleanup.rbegin(); it != mCleanup.rend(); ++it) guard(*it);
  mAfterLayout = {};
  mState.reset();
  guard([this] { if(mFrameSentinel && mFrameSentinel.GetParent()) mFrameSentinel.Unparent(); mFrameSentinel.Reset(); });
  for(auto& view : mOwned) guard([&view] { if(view && view.GetParent()) view.Unparent(); });
  mOwned.clear();
  return mCleanupPassed;
}
void Run::SetIdentity(const char* tc, uint64_t run, const char* scenario, const char* step, uint64_t sequence)
{
  mCaseId         = tc;
  mRunId          = run;
  mScenarioId     = scenario;
  mStepId         = step;
  mActionSequence = sequence;
}
void Run::BeginAction(std::size_t requiredChecks)
{
  CancelFrame();
  ReleaseFrameConnections();
  mFenceConnections.DisconnectAll();
  DisarmFenceWatchdog();
  if(mSettleTimer) mSettleTimer.Stop();
  mAfterLayout = {};
  mRequiredViews.clear();
  mSeen.fill(false);
  ++mActionEpoch;
  mActionStart           = mCount;
  mRequired              = requiredChecks;
  mAutomaticPresentation = true;
  mPending               = false;
  mFinished              = false;
  mFenceTimeoutMs        = DEFAULT_FENCE_MS;
  if(requiredChecks == 0) Fail("protocol.required_checks", "zero required checks");
}
void Run::Finish()
{
  if(mFinished) return;
  CancelFrame();
  if(mCount - mActionStart != mRequired) Fail("protocol.required_checks", "executed assertion count differs from the declared count");
  mPending  = false;
  mFinished = true;
  DisarmFenceWatchdog();
  if(mSettleTimer) mSettleTimer.Stop();
  Dali::LocaleNumericGuard locale;
  std::printf("LR_READY tc=%s pid=%d run=%llu scenario=%s step=%s action_seq=%llu checks=%zu failures=%zu\n",
              mCaseId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), mScenarioId.c_str(), mStepId.c_str(), static_cast<unsigned long long>(mActionSequence), mCount - mActionStart, mFailures);
  std::fflush(stdout);
  if(mOnFinished)
  {
    auto callback = mOnFinished;
    callback();
  }
}
void Run::SetOnFinished(std::function<void()> callback)
{
  mOnFinished = std::move(callback);
}
const std::string& Run::ScenarioId() const
{
  return mScenarioId;
}
const std::string& Run::StepId() const
{
  return mStepId;
}
void Run::Pending()
{
  mPending = true;
}
bool Run::IsPending() const
{
  return mPending;
}
bool Run::IsFinished() const
{
  return mFinished;
}
bool Run::HasFailed() const
{
  return mFailures != 0 || mOverflow;
}
std::size_t Run::ActionStart() const
{
  return mActionStart;
}
std::size_t Run::RowCount() const
{
  return mCount;
}
std::size_t Run::FailureCount() const
{
  return mFailures;
}
const Result& Run::Row(std::size_t index) const
{
  if(index >= mCount) throw std::out_of_range("layout result index");
  return mRows[index];
}
bool Run::HasOverflow() const
{
  return mOverflow;
}
bool Run::NeedsExternalVerification() const
{
  return mExternalVerification;
}
void Run::SuppressAutomaticPresentation()
{
  mAutomaticPresentation = false;
}
bool Run::AllowsAutomaticPresentation() const
{
  return mAutomaticPresentation;
}
void Run::RequireExternalVerification(const char* reason)
{
  mExternalVerification = true;
  std::printf("LR_EXTERNAL_REQUIRED %s\n", reason);
  std::fflush(stdout);
}
Result* Run::Append(const char* id)
{
  if(mFinished || mCount >= MAX_RESULT_ROWS)
  {
    mOverflow = true;
    ++mFailures;
    return nullptr;
  }
  Result& row = mRows[mCount++];
  row         = Result{};
  Copy(row.id, id);
  Copy(row.step, mStepId.c_str());
  row.actionSequence = mActionSequence;
  if(!id || std::strlen(id) >= sizeof(row.id))
  {
    mOverflow = true;
    ++mFailures;
  }
  return &row;
}
void Run::Near(const char* id, double actual, double expected, double tolerance)
{
  auto* row = Append(id);
  if(!row) return;
  Dali::LocaleNumericGuard locale;
  row->kind = "numeric";
  std::snprintf(row->actual, sizeof(row->actual), "%.17g", actual);
  std::snprintf(row->expected, sizeof(row->expected), "%.17g", expected);
  row->error     = std::abs(actual - expected);
  row->tolerance = tolerance;
  row->passed    = std::isfinite(actual) && std::isfinite(expected) && std::isfinite(tolerance) && tolerance >= 0 && row->error <= tolerance;
  if(!row->passed) ++mFailures;
}
void Run::Equal(const char* id, int64_t actual, int64_t expected)
{
  auto* row = Append(id);
  if(!row) return;
  row->kind = "integer";
  std::snprintf(row->actual, sizeof(row->actual), "%lld", static_cast<long long>(actual));
  std::snprintf(row->expected, sizeof(row->expected), "%lld", static_cast<long long>(expected));
  row->passed = actual == expected;
  if(!row->passed) ++mFailures;
}
void Run::Truth(const char* id, bool actual)
{
  Equal(id, actual ? 1 : 0, 1);
}
void Run::Text(const char* id, const std::string& actual, const std::string& expected)
{
  auto* row = Append(id);
  if(!row) return;
  row->kind = "text";
  Copy(row->actual, actual.c_str());
  Copy(row->expected, expected.c_str());
  row->passed = actual == expected && actual.size() < sizeof(row->actual) && expected.size() < sizeof(row->expected);
  if(!row->passed) ++mFailures;
}
void Run::Fail(const char* id, const char* reason)
{
  auto* row = Append(id);
  if(!row) return;
  Copy(row->actual, reason);
  Copy(row->expected, "required condition satisfied");
  ++mFailures;
}
void Run::Rect(const char* id, const LayoutRect& actual, const LayoutRect& expected, double tolerance)
{
  char field[160];
  std::snprintf(field, sizeof(field), "%s.x", id);
  Near(field, actual.x, expected.x, tolerance);
  std::snprintf(field, sizeof(field), "%s.y", id);
  Near(field, actual.y, expected.y, tolerance);
  std::snprintf(field, sizeof(field), "%s.width", id);
  Near(field, actual.width, expected.width, tolerance);
  std::snprintf(field, sizeof(field), "%s.height", id);
  Near(field, actual.height, expected.height, tolerance);
}
void Run::Rect(const char* id, View actual, const LayoutRect& expected, double tolerance)
{
  Rect(id, Bounds(actual), expected, tolerance);
}
void Run::Size(const char* id, const MeasuredSize& actual, const MeasuredSize& expected, double tolerance)
{
  char field[160];
  std::snprintf(field, sizeof(field), "%s.width", id);
  Near(field, actual.width, expected.width, tolerance);
  std::snprintf(field, sizeof(field), "%s.height", id);
  Near(field, actual.height, expected.height, tolerance);
}
void Run::Color(const char* id, const UiColor& actual, const UiColor& expected, double tolerance)
{
  auto* row = Append(id);
  if(!row) return;
  Dali::LocaleNumericGuard locale;
  row->kind = "color";
  Copy(row->actual, RgbText(actual).c_str());
  Copy(row->expected, RgbText(expected).c_str());
  row->error     = ChannelError(actual, expected);
  row->tolerance = tolerance;
  row->passed    = row->error <= tolerance;
  if(!row->passed) ++mFailures;
}
View Run::Host() const
{
  return mHost;
}
Window Run::GetWindow() const
{
  return mWindow;
}
void Run::Attach(View root)
{
  ColorizeTree(root);
  Keep(root);
  mHost.Add(root);
}
void Run::AttachToWindow(View root)
{
  // A View under the Window is its own layout root and takes its position from the
  // requested X/Y, so it is placed where the fixture host currently is on screen.
  const Dali::Bounds host = DevelActor::CalculateCurrentScreenExtents(mHost);
  root.SetRequestedX(host.x);
  root.SetRequestedY(host.y);
  ColorizeTree(root);
  Keep(root);
  mWindow.Add(root);
}
void Run::Keep(View view)
{
  mOwned.push_back(view);
}
void Run::OnCleanup(std::function<void()> action)
{
  mCleanup.push_back(std::move(action));
}
View Run::Stage(float width, float height, float x, float y)
{
  View stage = View::New();
  stage.SetRequestedWidth(width);
  stage.SetRequestedHeight(height);
  stage.SetRequestedX(x);
  stage.SetRequestedY(y);
  stage.SetBackgroundColor(UiColor(STAGE_COLOR));
  stage.SetProperty(Actor::Property::NAME, Dali::String((mCaseId + ".stage").c_str()));
  Attach(stage);
  return stage;
}
View Run::StageOnWindow(float width, float height)
{
  // Same visible stage as Stage(), but attached to the Window as its OWN layout root instead of
  // under the weighted fixture-host stack. Before measure modes a weight child was measured twice per
  // pass by the stack manager (at its natural extent in Measure, at its share in Arrange), which
  // doubled every leaf producer run the work steps count on the mounted subtree. With measure
  // modes the HUD's content area is measured EXACTLY and distributes its weights in Measure, so that
  // double measure is gone; the isolated root is kept so that no HUD pass takes part in a counted
  // pass. As an isolated root the workload is drained once per pass; the on-screen position (host
  // extents) and every parent-relative rendered rect are unchanged.
  View stage = View::New();
  stage.SetRequestedWidth(width);
  stage.SetRequestedHeight(height);
  stage.SetBackgroundColor(UiColor(STAGE_COLOR));
  stage.SetProperty(Actor::Property::NAME, Dali::String((mCaseId + ".stage").c_str()));
  AttachToWindow(stage);
  return stage;
}
void Run::AfterLayout(const std::vector<View>& views, std::function<void(Run&)> action)
{
  AfterLayout(mWindow, views, std::move(action));
}
void Run::AfterLayout(Window window, const std::vector<View>& views, std::function<void(Run&)> action)
{
  mFenceConnections.DisconnectAll();
  DisarmFenceWatchdog();
  if(views.empty() || views.size() > mSnapshots.size())
  {
    Fail("observer.required_views", "required view set is empty or exceeds capacity");
    return;
  }
  mRequiredViews = views;
  mSeen.fill(false);
  mAfterLayout = std::move(action);
  mPending     = true;
  const auto epoch = mActionEpoch;
  for(std::size_t i = 0; i < views.size(); ++i)
  {
    mRequiredViews[i].LayoutFinishedSignal().Connect(&mFenceConnections, [this, epoch, i](View, const LayoutRect& bounds) {
      if(!mPending || mFinished || epoch != mActionEpoch) return;
      mSnapshots[i] = bounds;
      mSeen[i]      = true;
    });
  }
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&mFenceConnections, [this](Window target) { OnWindowFinished(target); });
  ArmFenceWatchdog();
}
void Run::ArmFenceWatchdog()
{
  DisarmFenceWatchdog();
  mFenceTimer      = Timer::New(std::max(1u, mFenceTimeoutMs));
  const auto epoch = mActionEpoch;
  mFenceTimer.TickSignal().Connect(this, [this, epoch]() {
    if(epoch != mActionEpoch || mFinished || !mPending) return false;
    Fail("observer.timeout", "Window layout fence was not reached within the timeout");
    mPending     = false;
    mAfterLayout = {};
    Finish();
    return false;
  });
  mFenceTimer.Start();
}
void Run::DisarmFenceWatchdog()
{
  if(mFenceTimer)
  {
    mFenceTimer.Stop();
    mFenceTimer.Reset();
  }
}
void Run::SetFenceTimeout(uint32_t milliseconds)
{
  mFenceTimeoutMs = std::max(1u, milliseconds);
}
void Run::RunAction(const std::function<void(Run&)>& action)
{
  try
  {
    action(*this);
  }
  catch(const Dali::DaliException& error)
  {
    Fail("observer.exception", error.condition);
  }
  catch(const std::exception& error)
  {
    Fail("observer.exception", error.what());
  }
  catch(...)
  {
    Fail("observer.exception", "unexpected non-standard exception");
  }
}
void Run::OnWindowFinished(Window)
{
  if(!mPending || mFinished || !mAfterLayout) return;
  for(std::size_t i = 0; i < mRequiredViews.size(); ++i)
  {
    if(!mSeen[i])
    {
      Fail("observer.required_snapshot", "Window settled without all required View snapshots");
      DisarmFenceWatchdog();
      Finish();
      return;
    }
  }
  DisarmFenceWatchdog();
  mPending    = false;
  auto action = std::move(mAfterLayout);
  RunAction(action);
  if(!mPending) Finish();
}
LayoutRect Run::Snapshot(View view) const
{
  for(std::size_t i = 0; i < mRequiredViews.size(); ++i)
    if(mRequiredViews[i] == view && mSeen[i]) return mSnapshots[i];
  const float invalid = std::numeric_limits<float>::quiet_NaN();
  return LayoutRect(invalid, invalid, invalid, invalid);
}
void Run::AfterRender(const std::vector<View>& views, std::function<void(Run&)> action)
{
  AfterRender(mWindow, views, std::move(action));
}
void Run::AfterRender(Window window, const std::vector<View>& views, std::function<void(Run&)> action)
{
  AfterLayout(window, views, [this, views, action = std::move(action)](Run&) mutable
  { AfterFrame([this, action = std::move(action)](Run&) mutable { StartSettle(std::move(action)); }, {}, views.front()); });
}
Actor Run::FrameSentinel()
{
  // A plain Actor, NOT a Ui::View: it is neither a layout root nor part of the HUD, and it
  // carries no renderer, so it draws nothing and costs the render pass a single empty node.
  if(!mFrameSentinel)
  {
    mFrameSentinel = Actor::New();
    mFrameSentinel.SetProperty(Actor::Property::SIZE, Vector2(1, 1));
    mFrameSentinel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mFrameSentinel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mFrameSentinel.SetProperty(Actor::Property::POSITION, Vector2(0, 0));
    mFrameSentinel.SetProperty(Actor::Property::NAME, Dali::String((mCaseId + ".frame-sentinel").c_str()));
    mWindow.Add(mFrameSentinel);
  }
  return mFrameSentinel;
}
void Run::ReleaseFrameConnections()
{
  // Only ever called from BeginAction and Cleanup, where no fence callback is executing.
  // CancelFrame must NOT do this: DisconnectAll deletes the callback of the closure that
  // may be the one currently running.
  mFrameConnections.DisconnectAll();
}
void Run::CancelFrame()
{
  ++mFrameSequence;
  mFrameActive = false;
  if(mFrameWatchdog) mFrameWatchdog.Stop();
  mFrameWatchdog.Reset();
  if(mFrameTask)
  {
    mWindow.GetRenderTaskList().RemoveTask(mFrameTask);
    mFrameTask.Reset();
  }
  mAfterFrame = {};
  mFrameReady = {};
  mFrameSource.Reset();
}
void Run::AfterFrame(std::function<void(Run&)> action, std::function<bool()> ready, View source)
{
  CancelFrame();
  mAfterFrame         = std::move(action);
  mFrameReady         = std::move(ready);
  mFrameSource        = source ? source : mHost;
  mFrameStarted       = std::chrono::steady_clock::now();
  mFrameDeadline      = mFrameStarted + std::chrono::milliseconds(FRAME_DEADLINE_MS);
  mFrameAttempt       = 0;
  mFrameActive        = true;
  mPending            = true;
  const auto epoch    = mActionEpoch;
  const auto sequence = mFrameSequence;
  mFrameWatchdog      = Timer::New(SETTLE_POLL_MS);
  mFrameWatchdog.TickSignal().Connect(&mFrameConnections, [this, epoch, sequence]()
  {
    if(epoch != mActionEpoch || sequence != mFrameSequence || mFinished || !mFrameActive) return false;
    if(std::chrono::steady_clock::now() < mFrameDeadline) return true;
    CompleteFrame(false, "timeout", "frame watchdog reached the total deadline");
    return false;
  });
  mFrameWatchdog.Start();
  RequestFrame();
}
void Run::RequestFrame()
{
  if(!mFrameSource || !mFrameSource.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    CompleteFrame(false, "disconnected", "frame source is disconnected");
    return;
  }
  if(std::chrono::steady_clock::now() >= mFrameDeadline)
  {
    CompleteFrame(false, "timeout", "frame request reached the total deadline");
    return;
  }
  // A completion callback can acknowledge input after an older frame was drawn. Only a task
  // created AFTER that acknowledgement can prove the input was applied to the drawn frame.
  bool inputReadyAtRequest = false;
  try
  {
    inputReadyAtRequest = !mFrameReady || mFrameReady();
  }
  catch(...)
  {
    CompleteFrame(false, "predicate-exception", "input readiness predicate threw before the frame request");
    return;
  }
  const auto epoch    = mActionEpoch;
  const auto sequence = mFrameSequence;
  const auto attempt  = ++mFrameAttempt;
  // The previous task is removed here rather than in its own callback's CancelFrame: a task
  // outlives the emit of its own Finished signal, so removing it from inside is safe too.
  RenderTaskList tasks = mWindow.GetRenderTaskList();
  if(mFrameTask)
  {
    tasks.RemoveTask(mFrameTask);
    mFrameTask.Reset();
  }
  mFrameTask = tasks.CreateTask();
  mFrameTask.SetSourceActor(FrameSentinel());
  mFrameTask.SetExclusive(false);
  mFrameTask.SetInputEnabled(false);
  mFrameTask.SetClearEnabled(false);
  mFrameTask.FinishedSignal().Connect(&mFrameConnections, [this, epoch, sequence, attempt, inputReadyAtRequest](Dali::RenderTask)
  {
    if(epoch != mActionEpoch || sequence != mFrameSequence || attempt != mFrameAttempt || mFinished || !mFrameActive) return;
    if(std::chrono::steady_clock::now() >= mFrameDeadline)
    {
      CompleteFrame(false, "timeout", "frame completed after the total deadline");
      return;
    }
    // Readiness only acknowledges input delivery. Geometry is asserted independently once
    // the new task and the input acknowledgement have both completed.
    bool ready = false;
    try
    {
      ready = !mFrameReady || mFrameReady();
    }
    catch(...)
    {
      CompleteFrame(false, "predicate-exception", "input readiness predicate threw");
      return;
    }
    if(inputReadyAtRequest && ready)
      CompleteFrame(true);
    else
      RequestFrame();
  });
  // No framebuffer, so completion is decided by the task state machine alone: no GPU sync,
  // no pixel read-back, and therefore no per-attempt timeout that could expire on its own.
  mFrameTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
}
void Run::CompleteFrame(bool success, const char* result, const char* reason)
{
  if(!mFrameActive) return;
  const double   elapsed  = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - mFrameStarted).count();
  const uint64_t attempts = mFrameAttempt;
  auto           action   = std::move(mAfterFrame);
  CancelFrame();
  mPending = false;
  if(!success)
  {
    // Machine-visible record of an abnormal fence completion: the failing rows alone do not
    // say why. A successful fence emits nothing, so this adds no row and no per-frame noise.
    //
    // ONE guard covers both formats below. The row's free text and the record's elapsed_ms
    // field carry the same value, so formatting them under different locales would make the
    // row read elapsed_ms=4001,2 while the record stayed parsable - the two would disagree
    // about the number they report.
    Dali::LocaleNumericGuard locale;
    char                     detail[192];
    std::snprintf(detail, sizeof(detail), "%s; elapsed_ms=%.1f attempts=%llu",
                  reason ? reason : "fresh frame completion failed", elapsed,
                  static_cast<unsigned long long>(attempts));
    char elapsedText[32];
    std::snprintf(elapsedText, sizeof(elapsedText), "%.1f", elapsed);
    const std::string record = "LR_FRAME {\"tc\":\"" + Escape(mCaseId.c_str()) + "\",\"pid\":" + std::to_string(getpid()) +
                               ",\"run\":" + std::to_string(mRunId) + ",\"scenario\":\"" + Escape(mScenarioId.c_str()) +
                               "\",\"step\":\"" + Escape(mStepId.c_str()) + "\",\"action_seq\":" + std::to_string(mActionSequence) +
                               ",\"result\":\"" + Escape(result ? result : "timeout") + "\",\"reason\":\"" +
                               Escape(reason ? reason : "fresh frame completion failed") + "\",\"attempts\":" + std::to_string(attempts) +
                               ",\"elapsed_ms\":" + elapsedText + ",\"deadline_ms\":" + std::to_string(FRAME_DEADLINE_MS) + "}\n";
    std::fputs(record.c_str(), stdout);
    std::fflush(stdout);
    std::fputs(record.c_str(), stderr);
    std::fflush(stderr);
    Fail("render.frame-timeout", detail);
    Finish();
    return;
  }
  RunAction(action);
  if(!mPending) Finish();
}
bool Run::IsSettled() const
{
  for(const auto& view : mRequiredViews)
  {
    if(!view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE)) return false;
    View parent = View::DownCast(view.GetParent());
    if(!parent) return false;
    const LayoutRect target   = Bounds(view);
    const LayoutRect rendered = RenderedBounds(view, parent);
    if(std::abs(rendered.x - target.x) > SETTLE_TOLERANCE || std::abs(rendered.y - target.y) > SETTLE_TOLERANCE ||
       std::abs(rendered.width - target.width) > SETTLE_TOLERANCE || std::abs(rendered.height - target.height) > SETTLE_TOLERANCE)
    {
      return false;
    }
  }
  return true;
}
void Run::StartSettle(std::function<void(Run&)> action)
{
  mPending         = true;
  mSettleElapsedMs = 0;
  if(mSettleTimer) mSettleTimer.Stop();
  mSettleTimer     = Timer::New(SETTLE_POLL_MS);
  const auto epoch = mActionEpoch;
  mSettleTimer.TickSignal().Connect(this, [this, epoch, action = std::move(action)]() mutable {
    if(epoch != mActionEpoch || mFinished) return false;
    mSettleElapsedMs += SETTLE_POLL_MS;
    // IsSettled() compares each required view against its PARENT-relative target, so a
    // required view whose parent is not a View can never settle. Report that on the first
    // poll instead of waiting out the whole timeout; the action still runs, exactly as on
    // the timeout path, so the row count of a correct TC is unaffected.
    if(mSettleElapsedMs == SETTLE_POLL_MS)
    {
      for(const auto& view : mRequiredViews)
      {
        if(view && !View::DownCast(view.GetParent()))
        {
          Fail("render.unsupported-parent", "a required view of AfterRender has no View parent, so the rendered geometry can never match the parent-relative target");
          mPending = false;
          RunAction(action);
          if(!mPending) Finish();
          return false;
        }
      }
    }
    const bool settled = IsSettled();
    if(!settled && mSettleElapsedMs < DEFAULT_SETTLE_MS) return true;
    if(!settled) Fail("render.settle-timeout", "scene graph did not apply the layout targets within the settle timeout");
    mPending = false;
    RunAction(action);
    if(!mPending) Finish();
    return false;
  });
  mSettleTimer.Start();
}
LayoutRect Run::RenderedBounds(View view, View base)
{
  if(!base) base = View::DownCast(view.GetParent());
  const Dali::Bounds current = DevelActor::CalculateCurrentScreenExtents(view);
  const Dali::Bounds origin  = base ? DevelActor::CalculateCurrentScreenExtents(base) : Dali::Bounds(0.0f, 0.0f, 0.0f, 0.0f);
  return LayoutRect(current.x - origin.x, current.y - origin.y, current.width, current.height);
}
void Run::Rendered(const char* id, View view, const LayoutRect& expected, double tolerance)
{
  Rect(id, RenderedBounds(view, View()), expected, tolerance);
}
void Run::Rendered(const char* id, View view, View base, const LayoutRect& expected, double tolerance)
{
  Rect(id, RenderedBounds(view, base), expected, tolerance);
}
void Run::CapturePixels(View source, const std::vector<PixelProbe>& probes, std::function<void(Run&)> next)
{
  mPending     = true;
  mCaptureDone = false;
  if(mCaptureTimer) mCaptureTimer.Stop();
  mCapture           = Dali::Capture::New();
  const auto   epoch = mActionEpoch;
  const Dali::Bounds area = DevelActor::CalculateCurrentScreenExtents(source);
  auto finish = [this, epoch, probes, next](bool failedReason, const char* reason, Dali::Capture capture) mutable {
    if(epoch != mActionEpoch || mFinished || mCaptureDone) return;
    mCaptureDone = true;
    if(mCaptureTimer) mCaptureTimer.Stop();
    if(failedReason)
    {
      for(const auto& probe : probes) Fail((probe.id + ".pixel").c_str(), reason);
    }
    else
    {
      Dali::PixelBuffer buffer = capture.GetCapturedBuffer();
      const uint8_t*    data   = buffer ? buffer.GetBuffer() : nullptr;
      if(!data)
      {
        for(const auto& probe : probes) Fail((probe.id + ".pixel").c_str(), "captured buffer is empty");
      }
      else
      {
        const uint32_t      width  = buffer.GetWidth();
        const uint32_t      height = buffer.GetHeight();
        const uint32_t      stride = buffer.GetStrideBytes();
        const Pixel::Format format = buffer.GetPixelFormat();
        const uint32_t      bpp    = Pixel::GetBytesPerPixel(format);
        const bool          bgr    = format == Pixel::BGRA8888 || format == Pixel::BGR8888;
        for(const auto& probe : probes)
        {
          const std::string id = probe.id + ".pixel";
          if(bpp < 3 || probe.x < 0 || probe.y < 0 || probe.x >= width || probe.y >= height)
          {
            Fail(id.c_str(), "probe outside the captured area or unsupported pixel format");
            continue;
          }
          const uint8_t* pixel = data + static_cast<uint32_t>(probe.y) * stride + static_cast<uint32_t>(probe.x) * bpp;
          const float    r     = static_cast<float>(bgr ? pixel[2] : pixel[0]) / 255.0f;
          const float    g     = static_cast<float>(pixel[1]) / 255.0f;
          const float    b     = static_cast<float>(bgr ? pixel[0] : pixel[2]) / 255.0f;
          Color(id.c_str(), UiColor(r, g, b), probe.color);
        }
      }
    }
    mCapture.Reset();
    mPending = false;
    RunAction(next);
    if(!mPending) Finish();
  };
  mCapture.FinishedSignal().Connect(&mFenceConnections, [finish](Dali::Capture capture, Dali::Capture::FinishState state) mutable {
    finish(state != Dali::Capture::FinishState::SUCCEEDED, "capture finished with FAILED state", capture);
  });
  mCaptureTimer = Timer::New(DEFAULT_CAPTURE_MS);
  mCaptureTimer.TickSignal().Connect(this, [finish]() mutable {
    finish(true, "capture did not finish within the timeout", Dali::Capture());
    return false;
  });
  mCaptureTimer.Start();
  mCapture.Start(source, Vector2(area.x, area.y), Vector2(area.width, area.height), Dali::String(""), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}
void Run::Delay(uint32_t milliseconds, std::function<void(Run&)> action)
{
  Timer      timer = Timer::New(std::max(1u, milliseconds));
  const auto epoch = mActionEpoch;
  mPending         = true;
  timer.TickSignal().Connect(this, [this, epoch, action = std::move(action)]() mutable {
    if(epoch != mActionEpoch || mFinished) return false;
    mPending = false;
    try
    {
      action(*this);
    }
    catch(const Dali::DaliException& error)
    {
      Fail("timer.exception", error.condition);
    }
    catch(const std::exception& error)
    {
      Fail("timer.exception", error.what());
    }
    catch(...)
    {
      Fail("timer.exception", "unexpected non-standard exception");
    }
    if(!mPending) Finish();
    return false;
  });
  mTimers.push_back(timer);
  timer.Start();
}
namespace
{
// On-screen TC number: the id without its "TC" prefix ("TC95" -> "95"), as the other TC names
// show it. Object names and log lines keep the full id.
std::string DisplayNumber(const std::string& id)
{
  return id.compare(0, 2, "TC") == 0 ? id.substr(2) : id;
}
} // namespace

Case::Case(const char* id, const char* title, const char* description)
: mId(id),
  mTitle(title),
  mDescription(description)
{
}
Case::~Case()
{
  OnExit();
}
Dali::String Case::GetName() const
{
  return Dali::String((DisplayNumber(mId) + ". Layout: " + mTitle).c_str());
}
Dali::String Case::GetDescription() const
{
  return Dali::String(mDescription.c_str());
}
void Case::OnEnter(View contentArea)
{
  mContent          = contentArea;
  mPresentationRoot = contentArea;
  for(View parent = View::DownCast(mPresentationRoot.GetParent()); parent; parent = View::DownCast(mPresentationRoot.GetParent())) mPresentationRoot = parent;
  mSavedRootScalePolicy = Dali::Ui::Extension::View::GetUiScalePolicy(mPresentationRoot);
  Dali::Ui::Extension::View::SetUiScalePolicy(mPresentationRoot, Dali::Ui::Extension::UiScalePolicy::DISABLED);
  mWindow        = UiContext::Get().GetDefaultWindow();
  auto scale     = Dali::Ui::Extension::LayoutScaleManager::Get();
  mSavedScale    = scale.GetUiScale();
  mSavedScalable = scale.IsScalable();
  {
    Dali::LocaleNumericGuard locale;
    mScenarios = BuildScenarios();
  }
  auto row1 = StackLayout::New(StackOrientation::HORIZONTAL);
  row1.SetRequestedWidth(MATCH_PARENT);
  row1.SetRequestedHeight(36);
  row1.Add(Button("Reset run", [this] { Reset(); }));
  row1.Add(Button("Run all", [this] { RunAll(); }));
  row1.Add(Button("Run scenario", [this] { RunScenario(); }));
  contentArea.Add(row1);
  auto row2 = StackLayout::New(StackOrientation::HORIZONTAL);
  row2.SetRequestedWidth(MATCH_PARENT);
  row2.SetRequestedHeight(36);
  row2.Add(Button("Run step", [this] { RunStep(); }));
  row2.Add(Button("Next scenario", [this] { Next(); }));
  contentArea.Add(row2);
  mStatus = TextLabel("", 12);
  mStatus.SetRequestedHeight(96);
  mStatus.SetProperty(Actor::Property::NAME, Dali::String((mId + ".status").c_str()));
  mStatus.SetAccessibilityName(Dali::String((mId + ".status").c_str()));
  contentArea.Add(mStatus);
  mResult = TextLabel("", 11);
  mResult.SetRequestedHeight(240);
  mResult.SetProperty(Actor::Property::NAME, Dali::String((mId + ".result").c_str()));
  mResult.SetAccessibilityName(Dali::String((mId + ".result").c_str()));
  contentArea.Add(mResult);
  mFixtureHost = View::New();
  Dali::Ui::Extension::View::SetUiScalePolicy(mFixtureHost, Dali::Ui::Extension::UiScalePolicy::ENABLED);
  mFixtureHost.SetRequestedWidth(MATCH_PARENT);
  mFixtureHost.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
  mFixtureHost.SetBackgroundColor(UiColor(HOST_COLOR));
  mFixtureHost.SetProperty(Actor::Property::NAME, Dali::String((mId + ".fixture-host").c_str()));
  contentArea.Add(mFixtureHost);
  Reset();
}
View Case::Button(const char* label, std::function<void()> action)
{
  auto button = StackLayout::New(StackOrientation::VERTICAL);
  button.SetRequestedHeight(34);
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
  button.SetMargin(Insets(2, 2, 1, 1));
  button.SetBackgroundColor(UiColor(0xDCEAF8u));
  button.SetProperty(Actor::Property::NAME, Dali::String((mId + "." + label).c_str()));
  button.SetAccessibilityName(Dali::String((mId + "." + label).c_str()));
  button.Add(TextLabel(label, 11));
  button.AsInteractive().ClickedSignal().Connect(this, [action = std::move(action)](View, InputEvent) { action(); return true; });
  return button;
}
void Case::OnExit()
{
  if(!mContent) return;
  DisconnectAll();
  EndRun("exit");
  mTimer.Reset();
  auto scale = Dali::Ui::Extension::LayoutScaleManager::Get();
  scale.SetScalable(false);
  scale.SetUiScale(mSavedScale);
  scale.SetScalable(mSavedScalable);
  if(mPresentationRoot) Dali::Ui::Extension::View::SetUiScalePolicy(mPresentationRoot, mSavedRootScalePolicy);
  mPresentationRoot.Reset();
  mScenarios.clear();
  mContent.Reset();
  mFixtureHost.Reset();
  mStatus.Reset();
  mResult.Reset();
  mWindow.Reset();
}
void Case::StopAuto()
{
  mAuto = AutoRun::NONE;
  if(mTimer) mTimer.Stop();
}
void Case::CloseScenario()
{
  if(!mRun) return;
  if(!mRun->Cleanup())
  {
    ++mTotalFailures;
    mCleanupPassed = false;
  }
  mRun.reset();
}
void Case::EndRun(const char* reason)
{
  if(!mRun) return;
  StopAuto();
  FlushDeferredDump();
  CollectFinished();
  CloseScenario();
  Dali::LocaleNumericGuard locale;
  // required_scenarios makes the record self-contained: a reader decides whether the run
  // COMPLETED without holding the contract. CloseScenario above may still fail cleanup, but
  // mScenarios is cleared only by Reset()/StopCase(), both of which run after this.
  std::printf("LR_CASE_END {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"completed_scenarios\":%zu,\"required_scenarios\":%zu,\"completed_steps\":%zu,\"failures\":%zu,\"cleanup_passed\":%s,\"reason\":\"%s\"}\n",
              mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), mCompletedScenarios, mScenarios.size(), mCompletedSteps, mTotalFailures, mCleanupPassed ? "true" : "false", reason);
  std::fflush(stdout);
}
void Case::Ignored(const char* button, const char* reason)
{
  Dali::LocaleNumericGuard locale;
  std::printf("LR_NOOP {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"button\":\"%s\",\"reason\":\"%s\"}\n",
              mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), button, reason);
  std::fflush(stdout);
  Publish(false);
}
void Case::NewRun()
{
  mRun = std::make_unique<Run>(mFixtureHost, mWindow);
  mRun->SetOnFinished([this] { OnStepFinished(); });
  mRun->SetIdentity(mId.c_str(), mRunId, mScenarios.empty() ? "" : mScenarios[mScenario].id.c_str(), "", mCompletedSteps);
}
void Case::Reset()
{
  EndRun("reset");
  mFixtureHost.RemoveAll();
  Dali::Ui::Extension::LayoutScaleManager::Get().SetScalable(false);
  Dali::Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0f);
  mScenario = mStep = mCompletedScenarios = mCompletedSteps = mTotalFailures = mTotalChecks = 0;
  mLastCollectedRows = mLastCollectedFailures = 0;
  mStepInFlight = mHasExternalVerification = mDumpPending = false;
  mCleanupPassed                                          = true;
  mRunId                                                  = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
  NewRun();
  Dali::LocaleNumericGuard locale;
  std::printf("LR_CASE_CONTRACT {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"scenarios\":[", mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId));
  for(std::size_t i = 0; i < mScenarios.size(); ++i)
  {
    if(i) std::printf(",");
    std::printf("{\"id\":\"%s\",\"steps\":[", Escape(mScenarios[i].id.c_str()).c_str());
    for(std::size_t j = 0; j < mScenarios[i].steps.size(); ++j)
    {
      const auto& step = mScenarios[i].steps[j];
      if(j) std::printf(",");
      std::printf("{\"id\":\"%s\",\"required_checks\":%zu}", Escape(step.id.c_str()).c_str(), step.requiredChecks);
    }
    std::printf("]}");
  }
  std::printf("]}\n");
  std::fflush(stdout);
  Publish(true);
}
// Runs the next step of the current scenario. Rows of a step whose dump was deferred
// (SuppressAutomaticPresentation) are printed first so every action is dumped in order.
bool Case::StartStep(const char* button)
{
  FlushDeferredDump();
  CollectFinished();
  if(mScenarios.empty() || mScenario >= mScenarios.size())
  {
    Ignored(button, "no scenario");
    return false;
  }
  if(mStepInFlight)
  {
    Ignored(button, "step still running");
    return false;
  }
  if(mStep >= mScenarios[mScenario].steps.size())
  {
    Ignored(button, "scenario complete; use Next scenario");
    return false;
  }
  const auto& step = mScenarios[mScenario].steps[mStep];
  mRun->SetIdentity(mId.c_str(), mRunId, mScenarios[mScenario].id.c_str(), step.id.c_str(), mCompletedSteps + 1);
  mRun->BeginAction(step.requiredChecks);
  mStepInFlight = true;
  {
    Dali::LocaleNumericGuard locale;
    std::printf("LR_ACTION {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"scenario\":\"%s\",\"step\":\"%s\",\"action_seq\":%zu,\"required_checks\":%zu}\n",
                mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), mScenarios[mScenario].id.c_str(), step.id.c_str(), mCompletedSteps + 1, step.requiredChecks);
    std::fflush(stdout);
  }
  try
  {
    step.action(*mRun);
  }
  catch(const Dali::DaliException& error)
  {
    mRun->Fail("action.exception", error.condition);
  }
  catch(const std::exception& error)
  {
    mRun->Fail("action.exception", error.what());
  }
  catch(...)
  {
    mRun->Fail("action.exception", "unexpected non-standard exception");
  }
  if(!mRun->IsPending()) mRun->Finish();
  return true;
}
// Called by Run::Finish, possibly inside a LayoutFinished fence: only bookkeeping happens
// here; the HUD/stdout dump and any automatic continuation run on the next timer tick so
// that no Label invalidation lands inside the observed layout pass.
void Case::OnStepFinished()
{
  CollectFinished();
  mDumpPending = true;
  ScheduleTick();
}
void Case::ScheduleTick()
{
  mTimer = Timer::New(TICK_MS);
  mTimer.TickSignal().Connect(this, [this]() {
    OnTick();
    return false;
  });
  mTimer.Start();
}
void Case::OnTick()
{
  if(!mRun) return;
  if(mDumpPending && mRun->AllowsAutomaticPresentation())
  {
    mDumpPending = false;
    Publish(true);
  }
  if(mAuto != AutoRun::NONE) Continue();
}
void Case::Continue()
{
  if(mAuto == AutoRun::NONE || !mRun) return;
  CollectFinished();
  if(mStepInFlight) return;
  if(mStep < mScenarios[mScenario].steps.size())
  {
    if(!StartStep(mAuto == AutoRun::ALL ? "Run all" : "Run scenario")) mAuto = AutoRun::NONE;
    return;
  }
  if(mAuto == AutoRun::ALL && AdvanceScenario())
  {
    if(!StartStep("Run all")) mAuto = AutoRun::NONE;
    return;
  }
  mAuto = AutoRun::NONE;
  Publish(false);
}
void Case::FlushDeferredDump()
{
  if(!mDumpPending || !mRun) return;
  mDumpPending = false;
  Publish(true);
}
void Case::CollectFinished()
{
  if(!mRun || !mRun->IsFinished()) return;
  mTotalChecks += mRun->RowCount() - mLastCollectedRows;
  mTotalFailures += mRun->FailureCount() - mLastCollectedFailures;
  mLastCollectedRows     = mRun->RowCount();
  mLastCollectedFailures = mRun->FailureCount();
  mHasExternalVerification |= mRun->NeedsExternalVerification();
  if(!mStepInFlight) return;
  mStepInFlight = false;
  ++mStep;
  ++mCompletedSteps;
  if(mStep == mScenarios[mScenario].steps.size()) ++mCompletedScenarios;
}
// Closes the current scenario and opens the next one with a fresh Run. False when the
// current scenario is the last one.
bool Case::AdvanceScenario()
{
  if(mScenarios.empty() || mScenario + 1 >= mScenarios.size()) return false;
  FlushDeferredDump();
  CloseScenario();
  mFixtureHost.RemoveAll();
  Dali::Ui::Extension::LayoutScaleManager::Get().SetScalable(false);
  Dali::Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0f);
  ++mScenario;
  mStep = mLastCollectedRows = mLastCollectedFailures = 0;
  NewRun();
  Publish(true);
  return true;
}
void Case::RunStep()
{
  if(mAuto != AutoRun::NONE)
  {
    Ignored("Run step", "run in progress");
    return;
  }
  StartStep("Run step");
}
void Case::RunScenario()
{
  if(mAuto != AutoRun::NONE)
  {
    Ignored("Run scenario", "run in progress");
    return;
  }
  mAuto = AutoRun::SCENARIO;
  if(!StartStep("Run scenario")) mAuto = AutoRun::NONE;
}
void Case::RunAll()
{
  if(mAuto != AutoRun::NONE)
  {
    Ignored("Run all", "run in progress");
    return;
  }
  CollectFinished();
  if(mStepInFlight)
  {
    Ignored("Run all", "step still running");
    return;
  }
  if(!mScenarios.empty() && mStep >= mScenarios[mScenario].steps.size() && !AdvanceScenario())
  {
    Ignored("Run all", "last scenario already reached");
    return;
  }
  mAuto = AutoRun::ALL;
  if(!StartStep("Run all")) mAuto = AutoRun::NONE;
}
void Case::Next()
{
  if(mAuto != AutoRun::NONE)
  {
    Ignored("Next scenario", "run in progress");
    return;
  }
  CollectFinished();
  if(mStepInFlight)
  {
    Ignored("Next scenario", "step still running");
    return;
  }
  if(mScenarios.empty() || mStep != mScenarios[mScenario].steps.size())
  {
    Ignored("Next scenario", "current scenario has remaining steps; use Run scenario or Run step");
    return;
  }
  if(!AdvanceScenario()) Ignored("Next scenario", "last scenario already reached");
}
// Refreshes the HUD (unless a step is still observing) and, with dump, prints LR_RESULT
// followed by every LR_CHECK row of the latest action.
void Case::Publish(bool dump)
{
  if(!mRun) return;
  Dali::LocaleNumericGuard locale;
  const bool all = !mScenarios.empty() && mCompletedScenarios == mScenarios.size();
  // FAIL, PASS and EXTERNAL_REQUIRED are TERMINAL and are only ever reported once
  // completed_scenarios == required_scenarios. Before that the run has not finished, so a
  // failure so far reads FAILING, not FAIL, and an operator must keep running the case.
  // FAILING, INCOMPLETE and PENDING are the three non-terminal verdicts.
  const bool  failed  = mTotalFailures || mRun->HasFailed();
  const char* verdict = failed ? (all ? "FAIL" : "FAILING") : mStepInFlight ? "PENDING" : all ? (mHasExternalVerification ? "EXTERNAL_REQUIRED" : "PASS") : "INCOMPLETE";
  const std::size_t scenarioCount = mScenarios.size();
  const std::size_t stepCount     = scenarioCount ? mScenarios[mScenario].steps.size() : 0;
  const std::string scenarioId    = scenarioCount ? mScenarios[mScenario].id : "NONE";
  const std::size_t rows          = mRun->RowCount();
  const std::size_t first         = mRun->ActionStart();
  const char*       autoName      = mAuto == AutoRun::ALL ? "all" : mAuto == AutoRun::SCENARIO ? "scenario" : "none";
  std::ostringstream status;
  status << DisplayNumber(mId) << " " << mTitle << "  run=" << mRunId << "  " << verdict
         << "\nscenario " << (scenarioCount ? mScenario + 1 : 0) << "/" << scenarioCount << "  " << scenarioId << "  |  steps " << mStep << "/" << stepCount;
  if(mStepInFlight)
    status << "  running " << mRun->StepId();
  else if(mStep < stepCount)
    status << "  next " << mScenarios[mScenario].steps[mStep].id;
  status << "\nchecks=" << mTotalChecks << " failures=" << mTotalFailures << " completed scenarios=" << mCompletedScenarios << "/" << scenarioCount << " auto=" << autoName;
  std::ostringstream display;
  if(mStepInFlight)
    display << mRun->ScenarioId() << " / " << mRun->StepId() << ": running";
  else if(rows == 0)
    display << "no step has run in this scenario yet";
  else
  {
    std::size_t failures = 0;
    for(std::size_t i = first; i < rows; ++i)
      if(!mRun->Row(i).passed) ++failures;
    display << mRun->ScenarioId() << " / " << mRun->StepId() << ": " << (rows - first) << " checks, " << failures << " failures" << (failures ? "" : " (all rows PASS)");
    std::size_t shown = 0;
    for(std::size_t i = first; i < rows && shown < MAX_SHOWN_FAILURES; ++i)
    {
      const auto& row = mRun->Row(i);
      if(row.passed) continue;
      display << "\n" << (i + 1) << " FAIL " << row.id << " a=" << row.actual << " e=" << row.expected << " tol=" << row.tolerance;
      ++shown;
    }
    if(failures > shown) display << "\n(+" << (failures - shown) << " more failing rows in stdout)";
  }
  // A running step must not be disturbed by a result-label invalidation.
  if(!mStepInFlight)
  {
    mStatus.SetText(Dali::String(status.str().c_str()));
    mStatus.SetAccessibilityValue(Dali::String(status.str().c_str()));
    mResult.SetText(Dali::String(display.str().c_str()));
    mResult.SetAccessibilityValue(Dali::String(display.str().c_str()));
  }
  if(!dump) return;
  std::printf("LR_RESULT {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"scenario\":\"%s\",\"scenario_index\":%zu,\"scenario_count\":%zu,\"step\":\"%s\",\"step_index\":%zu,\"step_count\":%zu,\"completed_steps\":%zu,\"completed_scenarios\":%zu,\"required_scenarios\":%zu,\"checks\":%zu,\"failures\":%zu,\"verdict\":\"%s\",\"rows\":%zu}\n",
              mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), Escape(mRun->ScenarioId().c_str()).c_str(), scenarioCount ? mScenario + 1 : 0, scenarioCount, Escape(mRun->StepId().c_str()).c_str(), mStep, stepCount, mCompletedSteps, mCompletedScenarios, scenarioCount, mTotalChecks, mTotalFailures, verdict, rows);
  for(std::size_t i = first; i < rows; ++i)
  {
    const auto& row = mRun->Row(i);
    std::printf("LR_CHECK {\"tc\":\"%s\",\"pid\":%d,\"run\":%llu,\"scenario\":\"%s\",\"row\":%zu,\"step\":\"%s\",\"action_seq\":%llu,\"kind\":\"%s\",\"id\":\"%s\",\"actual\":\"%s\",\"expected\":\"%s\",\"tolerance\":%.17g,\"passed\":%s}\n",
                mId.c_str(), getpid(), static_cast<unsigned long long>(mRunId), Escape(mRun->ScenarioId().c_str()).c_str(), i + 1, Escape(row.step).c_str(), static_cast<unsigned long long>(row.actionSequence), row.kind, Escape(row.id).c_str(), Escape(row.actual).c_str(), Escape(row.expected).c_str(), row.tolerance, row.passed ? "true" : "false");
  }
  std::fflush(stdout);
}

UiColor Palette(uint32_t index)
{
  return UiColor(PALETTE[index % (sizeof(PALETTE) / sizeof(PALETTE[0]))]);
}
UiColor FixtureColor(uint32_t index)
{
  return UiColor(FIXTURE_PALETTE[index % (sizeof(FIXTURE_PALETTE) / sizeof(FIXTURE_PALETTE[0]))]);
}
void ColorizeTree(View root)
{
  if(root) ColorizeSubtree(root, 0u, 0u);
}
View Leaf(float width, float height, const char* name)
{
  View view = View::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetProperty(Actor::Property::NAME, Dali::String(name));
  uint32_t hash = 2166136261u;
  for(const char* p = name; p && *p; ++p) hash = (hash ^ static_cast<unsigned char>(*p)) * 16777619u;
  view.SetBackgroundColor(Palette(hash));
  return view;
}
LayoutRect Bounds(View view)
{
  return LayoutRect(view.GetProperty<float>(Actor::Property::POSITION_X), view.GetProperty<float>(Actor::Property::POSITION_Y), view.GetProperty<float>(Actor::Property::SIZE_WIDTH), view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
}
LayoutRect ArrangedBounds(View view)
{
  return Dali::Ui::GetImpl(view).GetArrangedBounds();
}
Scenario Single(const char* id, const char* description, std::size_t requiredChecks, std::function<void(Run&)> action)
{
  return {id, description, {{"run", requiredChecks, std::move(action)}}};
}
Scenario Steps(const std::string& id, const char* description, std::vector<Step> steps)
{
  return {id, description, std::move(steps)};
}
std::string Id(const char* prefix, uint32_t index)
{
  return std::string(prefix) + "." + std::to_string(index);
}
std::string FloatId(float value)
{
  Dali::LocaleNumericGuard locale;
  char                     text[64];
  std::snprintf(text, sizeof(text), "%f", static_cast<double>(value));
  return text;
}
} // namespace LayoutValidation

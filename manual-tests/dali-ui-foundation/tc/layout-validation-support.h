/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once

#include "manual-test-case.h"
#include <dali-ui-foundation/extension-api/view.h>
#include <dali/public-api/adaptor-framework/capture.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace LayoutValidation
{
using namespace Dali;
using namespace Dali::Ui;

constexpr double      GEOMETRY_TOLERANCE  = 0.001;
constexpr double      COLOR_TOLERANCE     = 3.0;   ///< Per-channel 8-bit tolerance for captured pixels.
constexpr std::size_t MAX_RESULT_ROWS     = 32768;
constexpr uint32_t    DEFAULT_FENCE_MS    = 10000; ///< Window fence watchdog.
constexpr uint32_t    DEFAULT_SETTLE_MS   = 4000;  ///< Scene-graph settle watchdog after the frame fence (16 ms polling). Not the frame deadline.
constexpr uint32_t    DEFAULT_CAPTURE_MS  = 4000;  ///< Capture completion watchdog.
constexpr uint32_t    FRAME_DEADLINE_MS   = 4000;  ///< Frame fence deadline: a new REFRESH_ONCE render task must finish within this; independent of DEFAULT_SETTLE_MS.
constexpr uint32_t    SETTLE_POLL_MS      = 16;

struct Result
{
  const char* kind{"failure"};
  char        id[160]{};
  char        step[96]{};
  uint64_t    actionSequence{0};
  char        actual[192]{};
  char        expected[192]{};
  double      error{0};
  double      tolerance{0};
  bool        passed{false};
};

/// A rendered-pixel probe: stage-local integer pixel coordinate and the expected opaque colour.
struct PixelProbe
{
  std::string id;
  float       x{0};
  float       y{0};
  UiColor     color;
};

class Run : public ConnectionTracker
{
public:
  Run(View host, Window window);
  ~Run() override;
  Run(const Run&)            = delete;
  Run& operator=(const Run&) = delete;

  void          SetIdentity(const char* tc, uint64_t run, const char* scenario, const char* step, uint64_t actionSequence);
  void          BeginAction(std::size_t requiredChecks);
  void          Finish();
  void          Pending();
  bool          IsPending() const;
  bool          IsFinished() const;
  bool          HasFailed() const;
  std::size_t   ActionStart() const;
  std::size_t   RowCount() const;
  std::size_t   FailureCount() const;
  const Result& Row(std::size_t index) const;
  bool          HasOverflow() const;
  bool          NeedsExternalVerification() const;
  void          RequireExternalVerification(const char* reason);
  void          SuppressAutomaticPresentation();
  bool          AllowsAutomaticPresentation() const;
  /// Invoked once when the current action has finished (after LR_READY is printed).
  void               SetOnFinished(std::function<void()> callback);
  const std::string& ScenarioId() const;
  const std::string& StepId() const;

  void Near(const char* id, double actual, double expected, double tolerance = GEOMETRY_TOLERANCE);
  void Equal(const char* id, int64_t actual, int64_t expected);
  void Truth(const char* id, bool actual);
  void Text(const char* id, const std::string& actual, const std::string& expected);
  void Fail(const char* id, const char* reason);
  void Rect(const char* id, const LayoutRect& actual, const LayoutRect& expected, double tolerance = GEOMETRY_TOLERANCE);
  void Rect(const char* id, View actual, const LayoutRect& expected, double tolerance = GEOMETRY_TOLERANCE);
  void Size(const char* id, const MeasuredSize& actual, const MeasuredSize& expected, double tolerance = GEOMETRY_TOLERANCE);
  void Color(const char* id, const UiColor& actual, const UiColor& expected, double tolerance = COLOR_TOLERANCE);

  View   Host() const;
  Window GetWindow() const;
  void   Attach(View root);
  void   Keep(View view);
  void   OnCleanup(std::function<void()> action);
  bool   Cleanup();

  // --- Window-attached (rendered) verification ---------------------------------------
  /// Creates an opaque fixed-size stage View, places it under the host at the requested
  /// offset and keeps it for the scenario. Children of the stage are laid out by the
  /// LayoutController with the stage content box as their constraint.
  View Stage(float width, float height, float x = 0.0f, float y = 0.0f);
  /// Like Stage() but attaches the stage to the Window as its own isolated layout root, so the
  /// leaf producer counts of a mounted workload see a single-drain pass that no HUD pass takes
  /// part in. Used by the work-count workloads (TC152/153) and the producer-count scenario TC115 K12;
  /// other mounts stay under the HUD.
  View StageOnWindow(float width, float height);
  /// Attaches `root` directly to the Window as its own layout root, placed at the fixture
  /// host's on-screen position so it renders in the fixture area rather than over the HUD.
  /// Use only when the fixture must stay outside the HUD's layout subtree (TC132/TC135).
  void AttachToWindow(View root);
  /// Waits for the Window LayoutFinished fence that includes every required view, then
  /// runs `action`. Snapshot() returns the rect the controller reported for each view: its final
  /// parent-local actor bounds after the pass (post-RTL, pre-transition).
  void AfterLayout(const std::vector<View>& requiredViews, std::function<void(Run&)> action);
  void AfterLayout(Window window, const std::vector<View>& requiredViews, std::function<void(Run&)> action);
  /// AfterLayout, then a fresh non-exclusive render task, then waits until the update thread
  /// applies every required view's event-side target (current extents equal parent-relative
  /// targets). This observes current scene geometry, not OS Window presentation.
  /// PRECONDITION: every required view must have a View parent, because the settle test
  /// compares against the PARENT-relative target. A required view whose parent is not a View
  /// (an AttachToWindow/StageOnWindow root, whose parent is a Layer) can never settle; the
  /// first poll reports render.unsupported-parent instead of waiting out the timeout.
  void AfterRender(const std::vector<View>& requiredViews, std::function<void(Run&)> action);
  void AfterRender(Window window, const std::vector<View>& requiredViews, std::function<void(Run&)> action);
  /// Waits for a new REFRESH_ONCE render task, with no framebuffer, over a 1x1 renderer-less
  /// sentinel actor in the window. The task is created AFTER every pending event-side write,
  /// so its completion means one update and one render pass have consumed them: fresh scene
  /// state, not Window presentation/composition. No pixels are read back and no GPU sync is
  /// awaited, so there is no per-attempt timeout to race - the only bound is
  /// FRAME_DEADLINE_MS for the whole fence. Optional readiness describes input delivery (for
  /// example animation progress), never expected layout geometry; it must hold both before
  /// the task is requested and after it completes, otherwise a further task is requested.
  /// An abnormal completion emits one LR_FRAME record and one render.frame-timeout row.
  void AfterFrame(std::function<void(Run&)> action, std::function<bool()> ready = {}, View source = View());
  /// Controller-reported final actor bounds (parent-local, post-RTL, pre-transition) captured by the fence.
  LayoutRect Snapshot(View view) const;
  /// Rendered rectangle of `view` from the current scene-graph state, relative to the
  /// rendered origin of `base` (defaults to the parent). Window coordinates, post-RTL.
  static LayoutRect RenderedBounds(View view, View base = View());
  /// Four rows id.x/.y/.width/.height from RenderedBounds(view, base).
  void Rendered(const char* id, View view, const LayoutRect& expected, double tolerance = GEOMETRY_TOLERANCE);
  void Rendered(const char* id, View view, View base, const LayoutRect& expected, double tolerance = GEOMETRY_TOLERANCE);
  /// Renders the source subtree into a separate framebuffer and checks one colour row per
  /// probe before `next`. This does not capture the Window compositor's complete output.
  void CapturePixels(View source, const std::vector<PixelProbe>& probes, std::function<void(Run&)> next);
  void SetFenceTimeout(uint32_t milliseconds);

  void Delay(uint32_t milliseconds, std::function<void(Run&)> action);

  template<typename T>
  void SetState(std::shared_ptr<T> value)
  {
    mState = std::move(value);
  }
  template<typename T>
  std::shared_ptr<T> State() const
  {
    return std::static_pointer_cast<T>(mState);
  }

private:
  Result* Append(const char* id);
  void    OnWindowFinished(Window window);
  void    ArmFenceWatchdog();
  void    DisarmFenceWatchdog();
  bool    IsSettled() const;
  void    StartSettle(std::function<void(Run&)> action);
  void    RequestFrame();
  void    CompleteFrame(bool success, const char* result = nullptr, const char* reason = nullptr);
  void    CancelFrame();
  void    ReleaseFrameConnections();
  Actor   FrameSentinel();
  void    RunAction(const std::function<void(Run&)>& action);

  std::string                        mCaseId, mScenarioId, mStepId;
  uint64_t                           mRunId{0}, mActionSequence{0};
  View                               mHost;
  Window                             mWindow;
  std::unique_ptr<Result[]>          mRows;
  std::size_t                        mCount{0};
  std::size_t                        mFailures{0};
  std::size_t                        mActionStart{0};
  std::size_t                        mRequired{0};
  bool                               mOverflow{false};
  bool                               mPending{false};
  bool                               mFinished{false};
  bool                               mExternalVerification{false};
  bool                               mAutomaticPresentation{true};
  bool                               mCleaned{false};
  bool                               mCleanupPassed{true};
  uint64_t                           mActionEpoch{0};
  uint32_t                           mFenceTimeoutMs{DEFAULT_FENCE_MS};
  std::shared_ptr<void>              mState;
  std::vector<View>                  mOwned;
  std::vector<Timer>                 mTimers;
  std::vector<std::function<void()>> mCleanup;
  ConnectionTracker                  mFenceConnections;
  std::vector<View>                  mRequiredViews;
  std::array<LayoutRect, 1024>       mSnapshots{};
  std::array<bool, 1024>             mSeen{};
  std::function<void(Run&)>          mAfterLayout;
  std::function<void()>              mOnFinished;
  Timer                              mFenceTimer;
  Timer                              mSettleTimer;
  uint32_t                           mSettleElapsedMs{0};
  Timer                              mCaptureTimer;
  Dali::Capture                      mCapture;
  bool                               mCaptureDone{false};
  ConnectionTracker                     mFrameConnections;
  Dali::RenderTask                      mFrameTask;
  Actor                                 mFrameSentinel;
  Timer                                 mFrameWatchdog;
  std::function<void(Run&)>             mAfterFrame;
  std::function<bool()>                 mFrameReady;
  View                                  mFrameSource;
  std::chrono::steady_clock::time_point mFrameStarted;
  std::chrono::steady_clock::time_point mFrameDeadline;
  uint64_t                              mFrameSequence{0};
  uint64_t                              mFrameAttempt{0};
  bool                                  mFrameActive{false};
};

struct Step
{
  std::string               id;
  std::size_t               requiredChecks;
  std::function<void(Run&)> action;
};

struct Scenario
{
  std::string       id;
  std::string       description;
  std::vector<Step> steps;
};

class Case : public ManualTest::TestCase, public ConnectionTracker
{
public:
  /// Launcher name "<N>. Layout: <title>" (N = id without "TC"); the line under it is "<description>".
  Case(const char* id, const char* title, const char* description);
  ~Case() override;
  Dali::String                  GetName() const override;
  Dali::String                  GetDescription() const override;
  void                          OnEnter(View contentArea) override;
  void                          OnExit() override;
  virtual std::vector<Scenario> BuildScenarios() = 0;

private:
  enum class AutoRun
  {
    NONE,
    SCENARIO,
    ALL
  };
  View        Button(const char* label, std::function<void()> action);
  void        Reset();
  void        RunStep();
  void        RunScenario();
  void        RunAll();
  void        Next();
  void        NewRun();
  bool        StartStep(const char* button);
  void        OnStepFinished();
  void        ScheduleTick();
  void        OnTick();
  void        Continue();
  void        StopAuto();
  void        FlushDeferredDump();
  bool        AdvanceScenario();
  void        Publish(bool dump);
  void        CollectFinished();
  void        CloseScenario();
  void        EndRun(const char* reason);
  void        Ignored(const char* button, const char* reason);
  std::string mId;
  std::string mTitle;
  std::string mDescription;
  View        mContent;
  View        mFixtureHost;
  View        mPresentationRoot;
  Dali::Ui::Extension::UiScalePolicy mSavedRootScalePolicy{Dali::Ui::Extension::UiScalePolicy::INHERIT};
  Label       mStatus;
  Label       mResult;
  Window      mWindow;
  std::vector<Scenario> mScenarios;
  std::unique_ptr<Run>  mRun;
  std::size_t mScenario{0};
  std::size_t mStep{0};
  AutoRun     mAuto{AutoRun::NONE};
  Timer       mTimer;
  bool        mDumpPending{false};
  std::size_t mCompletedScenarios{0};
  std::size_t mCompletedSteps{0};
  std::size_t mTotalFailures{0};
  std::size_t mTotalChecks{0};
  std::size_t mLastCollectedRows{0};
  std::size_t mLastCollectedFailures{0};
  bool        mStepInFlight{false};
  bool        mHasExternalVerification{false};
  bool        mCleanupPassed{true};
  uint64_t    mRunId{0};
  float       mSavedScale{1};
  bool        mSavedScalable{false};
};

/// Fixed-size, opaque leaf View with a palette colour chosen by `name` so it is visible on screen.
View       Leaf(float width, float height, const char* name = "leaf");
/// Event-side target rect (POSITION_X/Y, SIZE_WIDTH/HEIGHT) written by the last Arrange.
LayoutRect Bounds(View view);
/// Logical (pre-RTL), parent-local bounds the last Arrange applied to `view`:
/// ViewImpl::GetArrangedBounds() through the public Ui::GetImpl() helper.
LayoutRect ArrangedBounds(View view);
/// Deterministic opaque palette colour for fixture views.
UiColor    Palette(uint32_t index);
/// Deterministic muted colour for containers, probes and stages; distinct from the Leaf palette.
UiColor    FixtureColor(uint32_t index);
/// Gives every View in the subtree of `root` that has no background a muted colour so the
/// whole fixture is visible on screen. Existing backgrounds (Leaf, explicit colours) are kept.
/// A colour background has zero natural size, so measured and arranged geometry is unchanged.
void       ColorizeTree(View root);
Scenario   Single(const char* id, const char* description, std::size_t requiredChecks, std::function<void(Run&)> action);
/// Multi-step scenario; each step is run with one "Run step" and verified separately.
Scenario   Steps(const std::string& id, const char* description, std::vector<Step> steps);
std::string Id(const char* prefix, uint32_t index);
/// Locale-independent fixed-point text of a float (always '.' decimal point, six digits),
/// identical to std::to_string(float) under the "C" locale.
std::string FloatId(float value);
} // namespace LayoutValidation

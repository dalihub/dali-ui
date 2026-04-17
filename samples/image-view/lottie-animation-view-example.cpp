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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * LottieAnimationView sample.
 *
 * Demonstrates:
 *   - Play / Pause / Stop / JumpToFrame
 *   - LoopCount  (infinite / 3 / 1)
 *   - LoopingMode  (RESTART / AUTO_REVERSE)
 *   - FrameSpeedFactor  (0.5× / 1.0× / 2.0×)
 *   - StopBehavior  (CURRENT / FIRST / LAST)
 *   - SetMinMaxFrame  (full / first-half / second-half)
 *   - RenderScale  (0.5× / 1.0× / 2.0×)
 *   - GetPlayState / GetCurrentFrame / GetTotalFrame  (logged on each button press)
 *   - AnimationFinishedSignal
 *
 * Resources (from dali-demo):
 *   - done.json        (short check-mark completion animation)
 *   - jolly_walker.json  (looping character walk cycle)
 *
 * Press Escape or Back to quit.
 */
class LottieAnimationViewSampleController : public ConnectionTracker
{
  static constexpr int JUMP_FRAME = 10;

public:
  explicit LottieAnimationViewSampleController(Application& application)
  : mApplication(application),
    mLoopIndex(0),
    mLoopingModeIndex(0),
    mSpeedIndex(1),
    mStopBehaviorIndex(0),
    mFrameRangeIndex(0),
    mRenderScaleIndex(1)
  {
    mMonitorTimer = Timer::New(100);
    mMonitorTimer.TickSignal().Connect(this, &LottieAnimationViewSampleController::OnMonitorTimerTick);

    mApplication.InitSignal().Connect(this, &LottieAnimationViewSampleController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetSpacing(4.0f)
        .SetPadding(Extents(8, 8, 8, 8))
        .Children({
          CreateAnimationArea(),
          CreateStatusLabel(),
          CreatePlaybackRow(),
          CreateLoopRow(),
          CreateLoopingModeRow(),
          CreateSpeedRow(),
          CreateStopBehaviorRow(),
          CreateFrameRangeRow(),
          CreateRenderScaleRow(),
        }));

    window.KeyEventSignal().Connect(this, &LottieAnimationViewSampleController::OnKeyEvent);

    mMonitorTimer.Start();
  }

  // ── Widgets ─────────────────────────────────────────────────────────────

  View CreateAnimationArea()
  {
    LottieAnimationView::New(RESOURCES_DIR "jolly_walker.json")
      .SetRequestedWidth(300.0f)
      .SetRequestedHeight(300.0f)
      .SetLoopCount(-1)
      .As(mLottieView);

    mLottieView.AnimationFinishedSignal().Connect(this, &LottieAnimationViewSampleController::OnAnimationFinished);
    mLottieView.ResourceReadySignal().Connect(this, &LottieAnimationViewSampleController::OnResourceReady);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x2A2A2A))
      .Children({mLottieView});
  }

  View CreateStatusLabel()
  {
    return Label::New("Status: loading...")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(28.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mStatusLabel);
  }

  View CreatePlaybackRow()
  {
    return CreateButtonRow({
      CreateButton("PLAY",    [this](View, const InputEvent&) { OnPlay(); }),
      CreateButton("PAUSE",   [this](View, const InputEvent&) { OnPause(); }),
      CreateButton("STOP",    [this](View, const InputEvent&) { OnStop(); }),
      CreateButton("JUMP→10", [this](View, const InputEvent&) { OnJumpTo(); }),
    });
  }

  View CreateLoopRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("Loop:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(LOOP_LABELS[mLoopIndex], [this](View, const InputEvent&) { OnLoopToggle(); }, mLoopButton),
      });
  }

  View CreateLoopingModeRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("Mode:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(LOOPING_MODE_LABELS[mLoopingModeIndex], [this](View, const InputEvent&) { OnLoopingModeToggle(); }, mLoopingModeButton),
      });
  }

  View CreateSpeedRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("Speed:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(SPEED_LABELS[mSpeedIndex], [this](View, const InputEvent&) { OnSpeedToggle(); }, mSpeedButton),
      });
  }

  View CreateStopBehaviorRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("StopAt:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(STOP_BEHAVIOR_LABELS[mStopBehaviorIndex], [this](View, const InputEvent&) { OnStopBehaviorToggle(); }, mStopBehaviorButton),
      });
  }

  View CreateFrameRangeRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("Range:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(FRAME_RANGE_LABELS[mFrameRangeIndex], [this](View, const InputEvent&) { OnFrameRangeToggle(); }, mFrameRangeButton),
      });
  }

  View CreateRenderScaleRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(44.0f)
      .Children({
        Label::New("Scale:")
          .SetRequestedWidth(80.0f)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(12.0f)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateToggleButton(RENDER_SCALE_LABELS[mRenderScaleIndex], [this](View, const InputEvent&) { OnRenderScaleToggle(); }, mRenderScaleButton),
      });
  }

  // ── Button helpers ───────────────────────────────────────────────────────

  using ClickCallback = std::function<void(View, const InputEvent&)>;

  View CreateButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(52.0f);
    for(auto& btn : buttons)
    {
      row.Add(btn);
    }
    return row;
  }

  View CreateButton(const char* label, ClickCallback callback)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(UiColor(0x444444))
                           .Children({
                             Label::New(label)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(12.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });
    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, callback);
    return button;
  }

  View CreateToggleButton(const char* label, ClickCallback callback, View& outHandle)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(UiColor(0x1565C0))
                           .Children({
                             Label::New(label)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(12.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(outHandle),
                           });
    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, callback);
    return button;
  }

  // ── Callbacks ────────────────────────────────────────────────────────────

  void OnPlay()
  {
    mLottieView.Play();
    UpdateStatus("Playing");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] Play() — state=%d frame=%d/%d\n",
                          static_cast<int>(mLottieView.GetPlayState()),
                          mLottieView.GetCurrentFrame(),
                          mLottieView.GetTotalFrame());
  }

  void OnPause()
  {
    mLottieView.Pause();
    UpdateStatus("Paused");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] Pause() — state=%d frame=%d/%d\n",
                          static_cast<int>(mLottieView.GetPlayState()),
                          mLottieView.GetCurrentFrame(),
                          mLottieView.GetTotalFrame());
  }

  void OnStop()
  {
    mLottieView.Stop();
    UpdateStatus("Stopped");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] Stop() — state=%d frame=%d/%d\n",
                          static_cast<int>(mLottieView.GetPlayState()),
                          mLottieView.GetCurrentFrame(),
                          mLottieView.GetTotalFrame());
  }

  void OnJumpTo()
  {
    mLottieView.JumpToFrame(JUMP_FRAME);
    UpdateStatus("Jumped to frame " + std::to_string(JUMP_FRAME));
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] JumpToFrame(%d) — currentFrame=%d\n",
                          JUMP_FRAME,
                          mLottieView.GetCurrentFrame());
  }

  void OnLoopToggle()
  {
    mLoopIndex = (mLoopIndex + 1) % 3;
    mLottieView.SetLoopCount(LOOP_COUNTS[mLoopIndex]);
    Label::DownCast(mLoopButton).SetText(LOOP_LABELS[mLoopIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetLoopCount(%d)\n", LOOP_COUNTS[mLoopIndex]);
  }

  void OnLoopingModeToggle()
  {
    mLoopingModeIndex = (mLoopingModeIndex + 1) % 2;
    mLottieView.SetLoopingMode(LOOPING_MODES[mLoopingModeIndex]);
    Label::DownCast(mLoopingModeButton).SetText(LOOPING_MODE_LABELS[mLoopingModeIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetLoopingMode(%d)\n", mLoopingModeIndex);
  }

  void OnSpeedToggle()
  {
    mSpeedIndex = (mSpeedIndex + 1) % 3;
    mLottieView.SetFrameSpeedFactor(SPEED_FACTORS[mSpeedIndex]);
    Label::DownCast(mSpeedButton).SetText(SPEED_LABELS[mSpeedIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetFrameSpeedFactor(%.1f)\n", SPEED_FACTORS[mSpeedIndex]);
  }

  void OnStopBehaviorToggle()
  {
    mStopBehaviorIndex = (mStopBehaviorIndex + 1) % 3;
    mLottieView.SetStopBehavior(STOP_BEHAVIORS[mStopBehaviorIndex]);
    Label::DownCast(mStopBehaviorButton).SetText(STOP_BEHAVIOR_LABELS[mStopBehaviorIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetStopBehavior(%d)\n", mStopBehaviorIndex);
  }

  void OnFrameRangeToggle()
  {
    mFrameRangeIndex = (mFrameRangeIndex + 1) % 3;
    int total = mLottieView.GetTotalFrame();
    switch(mFrameRangeIndex)
    {
      case 0: // full range
        mLottieView.SetMinMaxFrame(0, total > 0 ? total - 1 : 0);
        break;
      case 1: // first half
        mLottieView.SetMinMaxFrame(0, total > 0 ? total / 2 : 0);
        break;
      case 2: // second half
        mLottieView.SetMinMaxFrame(total > 0 ? total / 2 : 0, total > 0 ? total - 1 : 0);
        break;
    }
    Label::DownCast(mFrameRangeButton).SetText(FRAME_RANGE_LABELS[mFrameRangeIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetMinMaxFrame — range=%s total=%d\n",
                          FRAME_RANGE_LABELS[mFrameRangeIndex], total);
  }

  void OnRenderScaleToggle()
  {
    mRenderScaleIndex = (mRenderScaleIndex + 1) % 3;
    mLottieView.SetRenderScale(RENDER_SCALES[mRenderScaleIndex]);
    Label::DownCast(mRenderScaleButton).SetText(RENDER_SCALE_LABELS[mRenderScaleIndex]);
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetRenderScale(%.1f)\n", RENDER_SCALES[mRenderScaleIndex]);
  }

  void OnResourceReady(View view)
  {
    int total = mLottieView.GetTotalFrame();
    UpdateStatus("Ready — total frames: " + std::to_string(total));
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] ResourceReady — totalFrame=%d\n", total);

    // Log content info (layer info)
    Dali::Property::Map contentInfo = mLottieView.GetContentInfo();
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] ContentInfo: %zu layers\n", contentInfo.Count());
  }

  void OnAnimationFinished(View view)
  {
    UpdateStatus("Animation Finished!");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] AnimationFinishedSignal fired\n");
  }

  void UpdateStatus(const std::string& text)
  {
    if(mStatusLabel)
    {
      mStatusLabel.SetText(text.c_str());
    }
  }

  bool OnMonitorTimerTick()
  {
    if(mLottieView && mLottieView.GetPlayState() == LottieAnimationView::PlayState::PLAYING)
    {
      DALI_LOG_RELEASE_INFO("[LottieAnimationView] Monitor — frame=%d/%d state=%d\n",
                            mLottieView.GetCurrentFrame(),
                            mLottieView.GetTotalFrame(),
                            static_cast<int>(mLottieView.GetPlayState()));
    }
    return true; // continuous
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  static constexpr int   LOOP_COUNTS[3]   = {-1, 3, 1};
  static constexpr float SPEED_FACTORS[3] = {0.5f, 1.0f, 2.0f};
  static constexpr float RENDER_SCALES[3] = {0.5f, 1.0f, 2.0f};
  static const char*     LOOP_LABELS[3];
  static const char*     LOOPING_MODE_LABELS[2];
  static const char*     SPEED_LABELS[3];
  static const char*     STOP_BEHAVIOR_LABELS[3];
  static const char*     FRAME_RANGE_LABELS[3];
  static const char*     RENDER_SCALE_LABELS[3];
  static const LottieAnimationView::LoopingMode::Type  LOOPING_MODES[2];
  static const LottieAnimationView::StopBehavior::Type STOP_BEHAVIORS[3];

  Application&        mApplication;
  LottieAnimationView mLottieView;
  Label               mStatusLabel;
  View                mLoopButton;
  View                mLoopingModeButton;
  View                mSpeedButton;
  View                mStopBehaviorButton;
  View                mFrameRangeButton;
  View                mRenderScaleButton;
  Timer               mMonitorTimer;

  int mLoopIndex;
  int mLoopingModeIndex;
  int mSpeedIndex;
  int mStopBehaviorIndex;
  int mFrameRangeIndex;
  int mRenderScaleIndex;
};

constexpr int   LottieAnimationViewSampleController::LOOP_COUNTS[];
constexpr float LottieAnimationViewSampleController::SPEED_FACTORS[];
constexpr float LottieAnimationViewSampleController::RENDER_SCALES[];

const char* LottieAnimationViewSampleController::LOOP_LABELS[]         = {"Loop: ∞", "Loop: 3", "Loop: 1"};
const char* LottieAnimationViewSampleController::LOOPING_MODE_LABELS[] = {"Mode: RESTART", "Mode: REVERSE"};
const char* LottieAnimationViewSampleController::SPEED_LABELS[]        = {"Speed: 0.5×", "Speed: 1.0×", "Speed: 2.0×"};
const char* LottieAnimationViewSampleController::STOP_BEHAVIOR_LABELS[] = {"StopAt: CURRENT", "StopAt: FIRST", "StopAt: LAST"};
const char* LottieAnimationViewSampleController::FRAME_RANGE_LABELS[]   = {"Range: FULL", "Range: 1st HALF", "Range: 2nd HALF"};
const char* LottieAnimationViewSampleController::RENDER_SCALE_LABELS[]  = {"Scale: 0.5×", "Scale: 1.0×", "Scale: 2.0×"};

const LottieAnimationView::LoopingMode::Type LottieAnimationViewSampleController::LOOPING_MODES[] = {
  LottieAnimationView::LoopingMode::RESTART,
  LottieAnimationView::LoopingMode::AUTO_REVERSE,
};

const LottieAnimationView::StopBehavior::Type LottieAnimationViewSampleController::STOP_BEHAVIORS[] = {
  LottieAnimationView::StopBehavior::CURRENT_FRAME,
  LottieAnimationView::StopBehavior::FIRST_FRAME,
  LottieAnimationView::StopBehavior::LAST_FRAME,
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  LottieAnimationViewSampleController controller(application);
  application.MainLoop();
  return 0;
}

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
 *   - PlaceholderUrl  (show placeholder while new URL loads)
 *   - GetPlayState / GetCurrentFrame / GetTotalFrame  (logged on each button press)
 *   - AnimationFinishedSignal
 *
 * Resources:
 *   - jolly_walker.json     (looping character walk cycle)
 *   - placeholder_image.png (placeholder shown while Lottie loads)
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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(4.0f);
    root.SetPadding(Extents(8, 8, 8, 8));
    root.AddChildren({
      CreateAnimationArea(),
      CreateStatusLabel(),
      CreatePlaybackRow(),
      CreateLoopRow(),
      CreateLoopingModeRow(),
      CreateSpeedRow(),
      CreateStopBehaviorRow(),
      CreateFrameRangeRow(),
      CreateRenderScaleRow(),
      CreatePlaceholderRow(),
    });
    window.Add(root);

    window.KeyEventSignal().Connect(this, &LottieAnimationViewSampleController::OnKeyEvent);

    mMonitorTimer.Start();
  }

  // ── Widgets ─────────────────────────────────────────────────────────────

  View CreateAnimationArea()
  {
    mLottieView = LottieAnimationView::New(RESOURCES_DIR "jolly_walker.json");
    mLottieView.SetRequestedWidth(MATCH_PARENT);
    mLottieView.SetRequestedHeight(MATCH_PARENT);
    mLottieView.SetLoopCount(-1);

    mLottieView.AnimationFinishedSignal().Connect(this, &LottieAnimationViewSampleController::OnAnimationFinished);
    mLottieView.ResourceReadySignal().Connect(this, &LottieAnimationViewSampleController::OnResourceReady);

    StackLayout area = StackLayout::New(StackOrientation::VERTICAL);
    area.SetRequestedWidth(MATCH_PARENT);
    area.SetRequestedHeight(WRAP_CONTENT);
    area.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    area.SetBackgroundColor(UiColor(0x2A2A2A));
    area.Add(mLottieView);
    return area;
  }

  View CreateStatusLabel()
  {
    mStatusLabel = Label::New("Status: loading...");
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(28.0f);
    mStatusLabel.SetFontSize(13.0f);
    mStatusLabel.SetTextColor(UiColor(0xCCCCCC));
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mStatusLabel;
  }

  View CreatePlaybackRow()
  {
    return CreateButtonRow({
      CreateButton("PLAY",    [this](View, InputEvent) { OnPlay(); }),
      CreateButton("PAUSE",   [this](View, InputEvent) { OnPause(); }),
      CreateButton("STOP",    [this](View, InputEvent) { OnStop(); }),
      CreateButton("JUMP→10", [this](View, InputEvent) { OnJumpTo(); }),
    });
  }

  View CreateLoopRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Loop:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(LOOP_LABELS[mLoopIndex], [this](View, InputEvent) { OnLoopToggle(); }, mLoopButton)});
    return row;
  }

  View CreateLoopingModeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Mode:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(LOOPING_MODE_LABELS[mLoopingModeIndex], [this](View, InputEvent) { OnLoopingModeToggle(); }, mLoopingModeButton)});
    return row;
  }

  View CreateSpeedRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Speed:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(SPEED_LABELS[mSpeedIndex], [this](View, InputEvent) { OnSpeedToggle(); }, mSpeedButton)});
    return row;
  }

  View CreateStopBehaviorRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("StopAt:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(STOP_BEHAVIOR_LABELS[mStopBehaviorIndex], [this](View, InputEvent) { OnStopBehaviorToggle(); }, mStopBehaviorButton)});
    return row;
  }

  View CreateFrameRangeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Range:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(FRAME_RANGE_LABELS[mFrameRangeIndex], [this](View, InputEvent) { OnFrameRangeToggle(); }, mFrameRangeButton)});
    return row;
  }

  View CreateRenderScaleRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Scale:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(RENDER_SCALE_LABELS[mRenderScaleIndex], [this](View, InputEvent) { OnRenderScaleToggle(); }, mRenderScaleButton)});
    return row;
  }

  View CreatePlaceholderRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Holder:");
    label.SetRequestedWidth(80.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({
      label,
      CreateButton("Set Placeholder", [this](View, const InputEvent&) { OnSetPlaceholder(); }),
      CreateButton("Clear URL", [this](View, const InputEvent&) { OnClearUrl(); }),
    });
    return row;
  }

  // ── Button helpers ───────────────────────────────────────────────────────

  using ClickCallback = std::function<void(View, InputEvent)>;

  View CreateButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(52.0f);
    for(auto& btn : buttons)
    {
      row.Add(btn);
    }
    return row;
  }

  View CreateButton(const char* label, ClickCallback callback)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(0x444444));
    Label buttonLabel = Label::New(label);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(12.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);
    button.AsInteractive().ClickedSignal().Connect(this, callback);
    return button;
  }

  View CreateToggleButton(const char* label, ClickCallback callback, View& outHandle)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(0x1565C0));
    Label buttonLabel = Label::New(label);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(12.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    outHandle = buttonLabel;
    button.Add(buttonLabel);
    button.AsInteractive().ClickedSignal().Connect(this, callback);
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

  void OnSetPlaceholder()
  {
    mLottieView.SetPlaceholderUrl(RESOURCES_DIR "placeholder_image.png");
    mLottieView.SetResourceUrl(RESOURCES_DIR "jolly_walker.json");
    UpdateStatus("Placeholder set — reloading");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] SetPlaceholderUrl + reload\n");
  }

  void OnClearUrl()
  {
    mLottieView.SetResourceUrl("");
    UpdateStatus("URL cleared");
    DALI_LOG_RELEASE_INFO("[LottieAnimationView] URL cleared\n");
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
    if(mLottieView && mLottieView.GetPlayState() == Ui::AnimatedImage::PlayState::PLAYING)
    {
      DALI_LOG_RELEASE_INFO("[LottieAnimationView] Monitor — frame=%d/%d state=%d\n",
                            mLottieView.GetCurrentFrame(),
                            mLottieView.GetTotalFrame(),
                            static_cast<int>(mLottieView.GetPlayState()));
    }
    return true; // continuous
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
  static const Ui::LottieAnimation::LoopingMode  LOOPING_MODES[2];
  static const Ui::AnimatedImage::StopBehavior   STOP_BEHAVIORS[3];

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

const Ui::LottieAnimation::LoopingMode LottieAnimationViewSampleController::LOOPING_MODES[] = {
  Ui::LottieAnimation::LoopingMode::RESTART,
  Ui::LottieAnimation::LoopingMode::AUTO_REVERSE,
};

const Ui::AnimatedImage::StopBehavior LottieAnimationViewSampleController::STOP_BEHAVIORS[] = {
  Ui::AnimatedImage::StopBehavior::CURRENT_FRAME,
  Ui::AnimatedImage::StopBehavior::FIRST_FRAME,
  Ui::AnimatedImage::StopBehavior::LAST_FRAME,
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  LottieAnimationViewSampleController controller(application);
  application.MainLoop();
  return 0;
}

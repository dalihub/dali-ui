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
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * AnimatedImageView sample.
 *
 * Demonstrates:
 *   - Play / Pause / Stop / JumpToFrame
 *   - Change Image  (cycles through WebP, GIF×2, URL array)
 *   - LoopCount  (infinite / 3 / 1)
 *   - FrameSpeedFactor  (0.5× / 1.0× / 2.0×)
 *   - StopBehavior  (CURRENT / FIRST / LAST)
 *   - SetFrameDelay  (200ms / 500ms / 1000ms / 2000ms)
 *   - GetPlayState / GetCurrentFrame / GetTotalFrame  (logged on each button press)
 *   - AnimationFinishedSignal
 *
 * Image sources:
 *   0. dog-anim.webp          (animated WebP)
 *   1. dali-logo-anim.gif     (animated GIF)
 *   2. animatedLoading.gif    (animated GIF)
 *   3. URL Array: dog-anim-001~008.png     (8 PNG frames via SetResourceUrls)
 *   4. URL Array: dali-logo-anim-001~015.png  (15 PNG frames via SetResourceUrls)
 *
 * Press Escape or Back to quit.
 */
class AnimatedImageViewSampleController : public ConnectionTracker
{
  static constexpr int JUMP_FRAME    = 5;
  static constexpr int IMAGE_COUNT   = 5;

public:
  explicit AnimatedImageViewSampleController(Application& application)
  : mApplication(application),
    mImageIndex(0),
    mLoopIndex(0),
    mSpeedIndex(1),
    mStopBehaviorIndex(0),
    mFrameDelayIndex(1)
  {
    mMonitorTimer = Timer::New(100);
    mMonitorTimer.TickSignal().Connect(this, &AnimatedImageViewSampleController::OnMonitorTimerTick);

    mApplication.InitSignal().Connect(this, &AnimatedImageViewSampleController::OnInit);
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
      CreateImageSourceLabel(),
      CreateAnimationArea(),
      CreateStatusLabel(),
      CreatePlaybackRow(),
      CreateChangeImageRow(),
      CreateLoopRow(),
      CreateSpeedRow(),
      CreateStopBehaviorRow(),
      CreateFrameDelayRow(),
    });
    window.Add(root);

    window.KeyEventSignal().Connect(this, &AnimatedImageViewSampleController::OnKeyEvent);

    // Apply initial image
    ApplyCurrentImage();

    mMonitorTimer.Start();
  }

  // ── Widgets ─────────────────────────────────────────────────────────────

  View CreateImageSourceLabel()
  {
    Label imageSourceLabel = Label::New(IMAGE_NAMES[mImageIndex]);
    imageSourceLabel.SetRequestedWidth(MATCH_PARENT);
    imageSourceLabel.SetRequestedHeight(24.0f);
    imageSourceLabel.SetFontSize(12.0f);
    imageSourceLabel.SetTextColor(UiColor(0x888888));
    imageSourceLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    imageSourceLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mImageSourceLabel = imageSourceLabel;
    return imageSourceLabel;
  }

  View CreateAnimationArea()
  {
    mAnimatedImageView = AnimatedImageView::New("");
    mAnimatedImageView.SetRequestedWidth(300.0f);
    mAnimatedImageView.SetRequestedHeight(300.0f);
    mAnimatedImageView.SetLoopCount(-1);

    mAnimatedImageView.AnimationFinishedSignal().Connect(this, &AnimatedImageViewSampleController::OnAnimationFinished);
    mAnimatedImageView.ResourceReadySignal().Connect(this, &AnimatedImageViewSampleController::OnResourceReady);

    StackLayout area = StackLayout::New(StackOrientation::HORIZONTAL);
    area.SetRequestedWidth(MATCH_PARENT);
    area.SetRequestedHeight(WRAP_CONTENT);
    area.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    area.SetBackgroundColor(UiColor(0x2A2A2A));
    area.Add(mAnimatedImageView);
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
      CreateButton("PLAY",   [this](View, InputEvent) { OnPlay(); }),
      CreateButton("PAUSE",  [this](View, InputEvent) { OnPause(); }),
      CreateButton("STOP",   [this](View, InputEvent) { OnStop(); }),
      CreateButton("JUMP→5", [this](View, InputEvent) { OnJumpTo(); }),
    });
  }

  View CreateChangeImageRow()
  {
    return CreateButtonRow({
      CreateButton("◀ PREV", [this](View, InputEvent) { OnPrevImage(); }),
      CreateButton("NEXT ▶", [this](View, InputEvent) { OnNextImage(); }),
    });
  }

  View CreateLoopRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Loop:");
    label.SetRequestedWidth(64.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(LOOP_LABELS[mLoopIndex], [this](View, InputEvent) { OnLoopToggle(); }, mLoopButton)});
    return row;
  }

  View CreateSpeedRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Speed:");
    label.SetRequestedWidth(64.0f);
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
    label.SetRequestedWidth(64.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(STOP_BEHAVIOR_LABELS[mStopBehaviorIndex], [this](View, InputEvent) { OnStopBehaviorToggle(); }, mStopBehaviorButton)});
    return row;
  }

  View CreateFrameDelayRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(44.0f);
    Label label = Label::New("Delay:");
    label.SetRequestedWidth(64.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(0xAAAAAA));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.AddChildren({label, CreateToggleButton(FRAME_DELAY_LABELS[mFrameDelayIndex], [this](View, InputEvent) { OnFrameDelayToggle(); }, mFrameDelayButton)});
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

  // ── Image switching ──────────────────────────────────────────────────────

  void ApplyCurrentImage()
  {
    Label::DownCast(mImageSourceLabel).SetText(IMAGE_NAMES[mImageIndex]);
    UpdateStatus("Loading...");

    if(mImageIndex >= 3)
    {
      // URL array: PNG frame sequence (same resources as dali-demo animated-images example)
      const char* fmt        = URL_ARRAY_FORMATS[mImageIndex - 3];
      int         frameCount = URL_ARRAY_FRAME_COUNTS[mImageIndex - 3];

      Dali::Vector<Dali::String> urls;
      for(int i = 1; i <= frameCount; ++i)
      {
        char buf[512];
        snprintf(buf, sizeof(buf), fmt, i);
        urls.PushBack(Dali::String(buf));
      }
      mAnimatedImageView.SetBatchSize(4);
      mAnimatedImageView.SetCacheSize(10);
      mAnimatedImageView.SetFrameDelay(FRAME_DELAYS[mFrameDelayIndex]);
      mAnimatedImageView.SetResourceUrls(urls);
    }
    else
    {
      // Single animated file
      mAnimatedImageView.SetResourceUrls(Dali::Vector<Dali::String>{});
      mAnimatedImageView.SetResourceUrl(IMAGE_URLS[mImageIndex]);
    }

    DALI_LOG_RELEASE_INFO("[AnimatedImageView] Image changed to: %s\n", IMAGE_NAMES[mImageIndex]);
  }

  void OnPrevImage()
  {
    mImageIndex = (mImageIndex + IMAGE_COUNT - 1) % IMAGE_COUNT;
    ApplyCurrentImage();
  }

  void OnNextImage()
  {
    mImageIndex = (mImageIndex + 1) % IMAGE_COUNT;
    ApplyCurrentImage();
  }

  // ── Callbacks ────────────────────────────────────────────────────────────

  void OnPlay()
  {
    mAnimatedImageView.Play();
    UpdateStatus("Playing");
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] Play() — state=%d frame=%d/%d\n",
                          static_cast<int>(mAnimatedImageView.GetPlayState()),
                          mAnimatedImageView.GetCurrentFrame(),
                          mAnimatedImageView.GetTotalFrame());
  }

  void OnPause()
  {
    mAnimatedImageView.Pause();
    UpdateStatus("Paused");
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] Pause() — state=%d frame=%d/%d\n",
                          static_cast<int>(mAnimatedImageView.GetPlayState()),
                          mAnimatedImageView.GetCurrentFrame(),
                          mAnimatedImageView.GetTotalFrame());
  }

  void OnStop()
  {
    mAnimatedImageView.Stop();
    UpdateStatus("Stopped");
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] Stop() — state=%d frame=%d/%d\n",
                          static_cast<int>(mAnimatedImageView.GetPlayState()),
                          mAnimatedImageView.GetCurrentFrame(),
                          mAnimatedImageView.GetTotalFrame());
  }

  void OnJumpTo()
  {
    mAnimatedImageView.JumpToFrame(JUMP_FRAME);
    UpdateStatus("Jumped to frame 5");
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] JumpToFrame(%d) — currentFrame=%d\n",
                          JUMP_FRAME,
                          mAnimatedImageView.GetCurrentFrame());
  }

  void OnLoopToggle()
  {
    mLoopIndex = (mLoopIndex + 1) % 3;
    mAnimatedImageView.SetLoopCount(LOOP_COUNTS[mLoopIndex]);
    Label::DownCast(mLoopButton).SetText(LOOP_LABELS[mLoopIndex]);
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] SetLoopCount(%d)\n", LOOP_COUNTS[mLoopIndex]);
  }

  void OnSpeedToggle()
  {
    mSpeedIndex = (mSpeedIndex + 1) % 3;
    mAnimatedImageView.SetFrameSpeedFactor(SPEED_FACTORS[mSpeedIndex]);
    Label::DownCast(mSpeedButton).SetText(SPEED_LABELS[mSpeedIndex]);
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] SetFrameSpeedFactor(%.1f)\n", SPEED_FACTORS[mSpeedIndex]);
  }

  void OnStopBehaviorToggle()
  {
    mStopBehaviorIndex = (mStopBehaviorIndex + 1) % 3;
    mAnimatedImageView.SetStopBehavior(STOP_BEHAVIORS[mStopBehaviorIndex]);
    Label::DownCast(mStopBehaviorButton).SetText(STOP_BEHAVIOR_LABELS[mStopBehaviorIndex]);
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] SetStopBehavior(%d)\n", mStopBehaviorIndex);
  }

  void OnFrameDelayToggle()
  {
    mFrameDelayIndex = (mFrameDelayIndex + 1) % 4;
    mAnimatedImageView.SetFrameDelay(FRAME_DELAYS[mFrameDelayIndex]);
    Label::DownCast(mFrameDelayButton).SetText(FRAME_DELAY_LABELS[mFrameDelayIndex]);
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] SetFrameDelay(%d ms)\n", FRAME_DELAYS[mFrameDelayIndex]);
  }

  void OnResourceReady(View view)
  {
    int total = mAnimatedImageView.GetTotalFrame();
    UpdateStatus("Ready — total frames: " + std::to_string(total));
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] ResourceReady — totalFrame=%d\n", total);
  }

  void OnAnimationFinished(View view)
  {
    UpdateStatus("Animation Finished!");
    DALI_LOG_RELEASE_INFO("[AnimatedImageView] AnimationFinishedSignal fired\n");
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
    if(mAnimatedImageView && mAnimatedImageView.GetPlayState() == Ui::AnimatedImage::PlayState::PLAYING)
    {
      DALI_LOG_RELEASE_INFO("[AnimatedImageView] Monitor — frame=%d/%d state=%d\n",
                            mAnimatedImageView.GetCurrentFrame(),
                            mAnimatedImageView.GetTotalFrame(),
                            static_cast<int>(mAnimatedImageView.GetPlayState()));
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
  static constexpr int   FRAME_DELAYS[4]  = {200, 500, 1000, 2000};

  static const char* IMAGE_NAMES[IMAGE_COUNT];
  static const char* IMAGE_URLS[3]; // indices 0-2: single animated file
  static const char* URL_ARRAY_FORMATS[2];
  static const int   URL_ARRAY_FRAME_COUNTS[2];
  static const char* LOOP_LABELS[3];
  static const char* SPEED_LABELS[3];
  static const char* STOP_BEHAVIOR_LABELS[3];
  static const char* FRAME_DELAY_LABELS[4];
  static const Ui::AnimatedImage::StopBehavior STOP_BEHAVIORS[3];

  Application&      mApplication;
  AnimatedImageView mAnimatedImageView;
  Label             mStatusLabel;
  View              mImageSourceLabel;
  View              mLoopButton;
  View              mSpeedButton;
  View              mStopBehaviorButton;
  View              mFrameDelayButton;
  Timer             mMonitorTimer;

  int mImageIndex;
  int mLoopIndex;
  int mSpeedIndex;
  int mStopBehaviorIndex;
  int mFrameDelayIndex;
};

constexpr int   AnimatedImageViewSampleController::LOOP_COUNTS[];
constexpr float AnimatedImageViewSampleController::SPEED_FACTORS[];
constexpr int   AnimatedImageViewSampleController::FRAME_DELAYS[];

const char* AnimatedImageViewSampleController::IMAGE_NAMES[] = {
  "dog-anim.webp (WebP)",
  "dali-logo-anim.gif (GIF)",
  "animatedLoading.gif (GIF)",
  "URL Array: dog-anim-001~008.png (8 frames)",
  "URL Array: dali-logo-anim-001~015.png (15 frames)",
};
const char* AnimatedImageViewSampleController::IMAGE_URLS[] = {
  RESOURCES_DIR "dog-anim.webp",
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "animatedLoading.gif",
};
const char* AnimatedImageViewSampleController::URL_ARRAY_FORMATS[] = {
  RESOURCES_DIR "dog-anim-%03d.png",
  RESOURCES_DIR "dali-logo-anim-%03d.png",
};
const int AnimatedImageViewSampleController::URL_ARRAY_FRAME_COUNTS[] = {8, 15};

const char* AnimatedImageViewSampleController::LOOP_LABELS[]          = {"Loop: ∞", "Loop: 3", "Loop: 1"};
const char* AnimatedImageViewSampleController::SPEED_LABELS[]         = {"Speed: 0.5×", "Speed: 1.0×", "Speed: 2.0×"};
const char* AnimatedImageViewSampleController::STOP_BEHAVIOR_LABELS[] = {"StopAt: CURRENT", "StopAt: FIRST", "StopAt: LAST"};
const char* AnimatedImageViewSampleController::FRAME_DELAY_LABELS[]   = {"Delay: 200ms", "Delay: 500ms", "Delay: 1000ms", "Delay: 2000ms"};

const Ui::AnimatedImage::StopBehavior AnimatedImageViewSampleController::STOP_BEHAVIORS[] = {
  Ui::AnimatedImage::StopBehavior::CURRENT_FRAME,
  Ui::AnimatedImage::StopBehavior::FIRST_FRAME,
  Ui::AnimatedImage::StopBehavior::LAST_FRAME,
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  AnimatedImageViewSampleController controller(application);
  application.MainLoop();
  return 0;
}

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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/devel-api/adaptor-framework/bitmap-saver.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t BLUR_RADIUS             = 100u;
constexpr float    BLUR_DOWNSCALE          = 0.15f;
constexpr float    DISPLAY_OPACITY         = 0.55f;
constexpr float    EPSILON_OPACITY         = 0.02f;
constexpr float    FADE_DURATION_SECONDS   = 3.0f;
constexpr uint32_t START_DELAY_MILLISECONDS = 600u;
constexpr uint32_t STATUS_TICK_MILLISECONDS = 100u;

const char* const IMAGE_URLS[] = {
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-large-1.jpg",
};
constexpr uint32_t IMAGE_COUNT = sizeof(IMAGE_URLS) / sizeof(IMAGE_URLS[0]);

enum class Scenario
{
  ORIGINAL_EARLY_ZERO,
  READY_ATTACH_ZERO,
  READY_ATTACH_OPAQUE,
  READY_ATTACH_EPSILON,
  EARLY_ATTACH_EPSILON,
  EARLY_ZERO_STATIC,
  CONTINUOUS_BLUR,
};

struct ScenarioConfig
{
  const char* name;
  const char* detail;
  bool        attachOnReady;
  bool        blurOnce;
  float       initialOpacity;
  bool        fadeImmediately;
  bool        fadeOnReady;
};

ScenarioConfig GetScenarioConfig(Scenario scenario)
{
  switch(scenario)
  {
    case Scenario::ORIGINAL_EARLY_ZERO:
      return {"Original request", "attach before scene / exact 0 / immediate 0 -> 0.55", false, true, 0.0f, true, false};
    case Scenario::READY_ATTACH_ZERO:
      return {"ResourceReady + zero", "attach from ResourceReady / exact 0 / then fade", true, true, 0.0f, false, true};
    case Scenario::READY_ATTACH_OPAQUE:
      return {"Opaque baseline", "attach from ResourceReady while opacity is 1", true, true, 1.0f, false, false};
    case Scenario::READY_ATTACH_EPSILON:
      return {"Ready + epsilon", "attach from ResourceReady at 0.02 / then fade", true, true, EPSILON_OPACITY, false, true};
    case Scenario::EARLY_ATTACH_EPSILON:
      return {"Early + epsilon", "attach before scene at 1 / set 0.02 / immediate fade", false, true, EPSILON_OPACITY, true, false};
    case Scenario::EARLY_ZERO_STATIC:
      return {"Zero static FBO", "attach before scene / stay at exact 0 / inspect FBO PNG", false, true, 0.0f, false, false};
    case Scenario::CONTINUOUS_BLUR:
      return {"BlurOnce OFF", "continuous blur / exact 0 / immediate fade", false, false, 0.0f, true, false};
  }
  return {"Unknown", "", false, true, 0.0f, false, false};
}
} // unnamed namespace

/**
 * Interactive Gaussian blur lab for reproducing the ImageBox/ImageView request order.
 *
 * Each scenario creates a fresh ImageView. Early-attach scenarios install and activate
 * the effect before the view is added to the on-scene image layer. ResourceReady
 * scenarios deliberately wait for ResourceReadySignal before installing the effect.
 *
 * Every blur execution calls KeepRenderResult() for the effect RenderTasks that own an
 * FBO. FinishedSignal then reads the pixels back and writes:
 *
 *   /tmp/gaussian-blur-run-<run>-fbo-<index>.png
 *
 * This is the actual Gaussian blur pipeline output, not a second Capture render.
 */
class GaussianBlurImageController : public ConnectionTracker
{
public:
  GaussianBlurImageController(Application& application, Scenario initialScenario)
  : mApplication(application),
    mScenario(initialScenario)
  {
    mApplication.InitSignal().Connect(this, &GaussianBlurImageController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetPositionSize(PositionSize(80, 60, 1440, 900));
    mWindow.SetBackgroundColor(UiColor(0x0B1020));
    mWindow.KeyEventSignal().Connect(this, &GaussianBlurImageController::OnKeyEvent);

    BuildUserInterface();

    mStatusTimer = Timer::New(STATUS_TICK_MILLISECONDS);
    mStatusTimer.TickSignal().Connect(this, &GaussianBlurImageController::OnStatusTick);
    mStatusTimer.Start();

    mStartTimer = Timer::New(START_DELAY_MILLISECONDS);
    mStartTimer.TickSignal().Connect(this, &GaussianBlurImageController::OnStartTimer);
    mStartTimer.Start();
  }

  void BuildUserInterface()
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(28.0f, 28.0f, 22.0f, 22.0f));
    root.SetSpacing(16.0f);

    StackLayout header = StackLayout::New(StackOrientation::VERTICAL);
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(74.0f);
    header.SetSpacing(3.0f);

    Label title = NewLabel("Gaussian Blur / Opacity Lab", 28.0f, 0xF8FAFC);
    title.SetRequestedHeight(40.0f);
    title.SetVerticalTextAlignment(Text::Alignment::CENTER);

    Label subtitle = NewLabel("BlurOnce source rendering, ResourceReady timing, visible fades, and direct FBO read-back", 14.0f, 0x94A3B8);
    subtitle.SetRequestedHeight(28.0f);
    subtitle.SetVerticalTextAlignment(Text::Alignment::CENTER);
    header.Add(title);
    header.Add(subtitle);

    StackLayout mainRow = StackLayout::New(StackOrientation::HORIZONTAL);
    mainRow.SetRequestedWidth(MATCH_PARENT);
    mainRow.SetRequestedHeight(WRAP_CONTENT);
    mainRow.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mainRow.SetSpacing(18.0f);

    BuildPreview();
    BuildControlPanel();
    mainRow.Add(mPreview);
    mainRow.Add(mControlPanel);

    mStatusLabel = NewLabel("Waiting for the first scenario...", 14.0f, 0xDDE7F5);
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(74.0f);
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetBackgroundColor(UiColor(0x111A2E));
    mStatusLabel.SetCornerRadius(12.0f);
    mStatusLabel.SetPadding(Insets(16.0f, 16.0f, 8.0f, 8.0f));
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    root.Add(header);
    root.Add(mainRow);
    root.Add(mStatusLabel);
    mWindow.Add(root);
  }

  void BuildPreview()
  {
    mPreview = AbsoluteLayout::New();
    mPreview.SetRequestedWidth(WRAP_CONTENT);
    mPreview.SetRequestedHeight(MATCH_PARENT);
    mPreview.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mPreview.SetBackgroundColor(Color::BLACK);
    mPreview.SetCornerRadius(18.0f);

    mImageLayer = AbsoluteLayout::New();
    mImageLayer.SetLayoutParams(AbsoluteLayoutParams::New()
                                 .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
                                 .SetFlags(AbsoluteLayoutFlags::ALL));
    mImageLayer.SetBackgroundColor(Color::BLACK);
    mPreview.Add(mImageLayer);

    AbsoluteLayout referenceCard = AbsoluteLayout::New();
    referenceCard.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(20.0f, 20.0f, 284.0f, 196.0f)));
    referenceCard.SetBackgroundColor(UiColor(0x0F172A, 0.94f));
    referenceCard.SetCornerRadius(12.0f);

    Label referenceTitle = NewLabel("SHARP SOURCE", 12.0f, 0xA5B4FC);
    referenceTitle.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(12.0f, 5.0f, 260.0f, 28.0f)));

    mSharpReference = ImageView::New();
    mSharpReference.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mSharpReference.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(12.0f, 36.0f, 260.0f, 146.0f)));
    mSharpReference.SetCornerRadius(8.0f);
    referenceCard.Add(referenceTitle);
    referenceCard.Add(mSharpReference);
    mPreview.Add(referenceCard);

    mPreviewBadge = NewLabel("PREVIEW", 13.0f, 0xFFFFFF);
    mPreviewBadge.SetBackgroundColor(UiColor(0x4F46E5, 0.92f));
    mPreviewBadge.SetCornerRadius(8.0f);
    mPreviewBadge.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mPreviewBadge.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mPreviewBadge.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.02f, 0.91f, 0.54f, 0.065f))
                                   .SetFlags(AbsoluteLayoutFlags::ALL));
    mPreview.Add(mPreviewBadge);
  }

  void BuildControlPanel()
  {
    mControlPanel = StackLayout::New(StackOrientation::VERTICAL);
    mControlPanel.SetRequestedWidth(430.0f);
    mControlPanel.SetRequestedHeight(MATCH_PARENT);
    mControlPanel.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    mControlPanel.SetSpacing(9.0f);
    mControlPanel.SetBackgroundColor(UiColor(0x111A2E));
    mControlPanel.SetCornerRadius(18.0f);

    Label scenarioTitle = NewLabel("REPRODUCTION SCENARIOS", 14.0f, 0xA5B4FC);
    scenarioTitle.SetRequestedHeight(28.0f);
    scenarioTitle.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mControlPanel.Add(scenarioTitle);

    mControlPanel.Add(NewButtonRow("Original: early + 0", Scenario::ORIGINAL_EARLY_ZERO,
                                   "Ready attach + 0", Scenario::READY_ATTACH_ZERO));
    mControlPanel.Add(NewButtonRow("Ready attach + 1", Scenario::READY_ATTACH_OPAQUE,
                                   "Ready attach + .02", Scenario::READY_ATTACH_EPSILON));
    mControlPanel.Add(NewButtonRow("Early attach + .02", Scenario::EARLY_ATTACH_EPSILON,
                                   "Early + 0 static", Scenario::EARLY_ZERO_STATIC));

    StackLayout continuousRow = StackLayout::New(StackOrientation::HORIZONTAL);
    continuousRow.SetRequestedWidth(MATCH_PARENT);
    continuousRow.SetRequestedHeight(48.0f);
    continuousRow.Add(NewScenarioButton("BlurOnce OFF + 0 -> .55", Scenario::CONTINUOUS_BLUR));
    mControlPanel.Add(continuousRow);

    Label animationTitle = NewLabel("DISPLAY OPACITY ACTIONS", 14.0f, 0x67E8F9);
    animationTitle.SetRequestedHeight(30.0f);
    animationTitle.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mControlPanel.Add(animationTitle);

    StackLayout fadeRow = NewActionRow();
    fadeRow.Add(NewActionButton("Fade in\n0 -> .55", [this] { StartManualFadeIn(); }, 0x0E7490));
    fadeRow.Add(NewActionButton("Fade out\n.55 -> 0", [this] { StartManualFadeOut(); }, 0x0E7490));
    fadeRow.Add(NewActionButton("Slow cycle\n0 -> .55 -> 0", [this] { StartFadeCycle(); }, 0x0E7490));
    mControlPanel.Add(fadeRow);

    Label diagnosticTitle = NewLabel("RESOURCE / DIAGNOSTICS", 14.0f, 0xFBBF24);
    diagnosticTitle.SetRequestedHeight(30.0f);
    diagnosticTitle.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mControlPanel.Add(diagnosticTitle);

    StackLayout resourceRow = NewActionRow();
    resourceRow.Add(NewActionButton("Next image\n+ Refresh", [this] { ChangeImageAndRefresh(); }, 0x92400E));
    resourceRow.Add(NewActionButton("Refresh blur\n+ dump FBO", [this] { RefreshAndDump(); }, 0x92400E));
    mControlPanel.Add(resourceRow);

    Label help = NewLabel("Keyboard: 1-7 scenario  |  I fade-in  O fade-out  C cycle\nSpace next image  |  R refresh + FBO dump  |  Esc quit", 12.0f, 0x94A3B8);
    help.SetRequestedWidth(MATCH_PARENT);
    help.SetRequestedHeight(WRAP_CONTENT);
    help.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    help.SetMultiLine(true);
    help.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mControlPanel.Add(help);
  }

  Label NewLabel(const char* text, float fontSize, uint32_t color) const
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(color));
    return label;
  }

  StackLayout NewButtonRow(const char* leftText, Scenario leftScenario, const char* rightText, Scenario rightScenario)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(48.0f);
    row.SetSpacing(8.0f);
    row.Add(NewScenarioButton(leftText, leftScenario));
    row.Add(NewScenarioButton(rightText, rightScenario));
    return row;
  }

  Label NewScenarioButton(const char* text, Scenario scenario)
  {
    Label button = NewButton(text, 0x3730A3);
    button.AsInteractive().ClickedSignal().Connect(this, [this, scenario](View, InputEvent)
    {
      RunScenario(scenario, true);
    });
    return button;
  }

  StackLayout NewActionRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(62.0f);
    row.SetSpacing(8.0f);
    return row;
  }

  template<typename Callback>
  Label NewActionButton(const char* text, Callback callback, uint32_t color)
  {
    Label button = NewButton(text, color);
    button.SetMultiLine(true);
    button.AsInteractive().ClickedSignal().Connect(this, [callback](View, InputEvent)
    {
      callback();
    });
    return button;
  }

  Label NewButton(const char* text, uint32_t color)
  {
    Label button = NewLabel(text, 12.0f, 0xFFFFFF);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    button.SetBackgroundColor(UiColor(color));
    button.SetCornerRadius(9.0f);
    button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return button;
  }

  bool OnStartTimer()
  {
    RunScenario(mScenario, false);
    return false;
  }

  bool OnStatusTick()
  {
    UpdateStatus();
    return true;
  }

  void RunScenario(Scenario scenario, bool advanceImage)
  {
    StopFade();
    ++mRunId;
    mScenario        = scenario;
    mConfig          = GetScenarioConfig(scenario);
    mResourceReady   = false;
    mAttachPending   = mConfig.attachOnReady;
    mFadeOnNextReady = mConfig.fadeOnReady;
    mFboDumpCount    = 0u;
    mDebugRenderTasks.clear();

    if(mLoadTimer)
    {
      mLoadTimer.Stop();
      mLoadTimer.Reset();
    }
    if(mResourceTimer)
    {
      mResourceTimer.Stop();
      mResourceTimer.Reset();
    }

    if(advanceImage)
    {
      mImageIndex = (mImageIndex + 1u) % IMAGE_COUNT;
    }

    // Recreate the source View so early-attach really happens before it is on scene.
    mImageLayer.SetVisible(false);
    if(mBlurredImage)
    {
      mImageLayer.Remove(mBlurredImage);
      mBlurredImage.Reset();
    }
    mBlurEffect.Reset();

    mBlurredImage = ImageView::New();
    mBlurredImage.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mBlurredImage.SetOpacity(1.0f); // Matches ImageBox construction before the later fade reset.
    mBlurredImage.SetLayoutParams(AbsoluteLayoutParams::New()
                                   .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
                                   .SetFlags(AbsoluteLayoutFlags::ALL));
    mBlurredImage.ResourceReadySignal().Connect(this, &GaussianBlurImageController::OnResourceReady);

    if(!mConfig.attachOnReady)
    {
      ApplyGpuBlur(); // Deliberately before Add().
    }

    mImageLayer.Add(mBlurredImage);
    // Leave the empty View attached for a few frames. This mirrors the real backdrop,
    // which is constructed hidden well before a thumbnail URL is assigned, and lets
    // layout/effect RenderTasks exist before ResourceReady can fire from the cache.
    mLoadTimer = Timer::New(300u);
    mLoadTimer.TickSignal().Connect(this, &GaussianBlurImageController::OnPrepareLoadTimer);
    mLoadTimer.Start();

    DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] run=%u scenario=%s attach=%s blurOnce=%d initialOpacity=%.2f\n",
                          mRunId,
                          mConfig.name,
                          mConfig.attachOnReady ? "ResourceReady" : "before-scene",
                          static_cast<int>(mConfig.blurOnce),
                          mConfig.initialOpacity);
    UpdateStatus();
  }

  bool OnPrepareLoadTimer()
  {
    mImageLayer.SetVisible(true);
    if(mConfig.attachOnReady)
    {
      // Make the requested opacity current before assigning a possibly cached URL.
      // This reproduces the application state at the eventual ResourceReady callback.
      mBlurredImage.SetOpacity(mConfig.initialOpacity);
      mResourceTimer = Timer::New(150u);
      mResourceTimer.TickSignal().Connect(this, &GaussianBlurImageController::OnResourceLoadTimer);
      mResourceTimer.Start();
    }
    else
    {
      LoadResource();
    }
    return false;
  }

  bool OnResourceLoadTimer()
  {
    LoadResource();
    return false;
  }

  void LoadResource()
  {
    // Early modes intentionally keep the application's URL -> opacity setter order.
    mBlurredImage.SetResourceUrl(IMAGE_URLS[mImageIndex]);
    mSharpReference.SetResourceUrl(IMAGE_URLS[mImageIndex]);
    if(!mConfig.attachOnReady)
    {
      mBlurredImage.SetOpacity(mConfig.initialOpacity);
    }
    if(mConfig.fadeImmediately)
    {
      StartFade(DISPLAY_OPACITY, FADE_DURATION_SECONDS, false);
    }
  }

  void ApplyGpuBlur()
  {
    mBlurEffect = GaussianBlurEffect::New(BLUR_RADIUS);
    mBlurEffect.SetBlurOnce(mConfig.blurOnce);
    mBlurEffect.SetBlurDownscaleFactor(BLUR_DOWNSCALE);
    mBlurEffect.FinishedSignal().Connect(this, &GaussianBlurImageController::OnBlurFinished);
    mBlurredImage.SetRenderEffect(mBlurEffect);
    mBlurEffect.Activate();

    DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] ApplyGpuBlur activated=%d requestedDownscale=%.2f actualDownscale=%.2f\n",
                          static_cast<int>(mBlurEffect.IsActivated()),
                          BLUR_DOWNSCALE,
                          mBlurEffect.GetBlurDownscaleFactor());
  }

  void OnResourceReady(View view)
  {
    if(view != mBlurredImage)
    {
      return;
    }

    mResourceReady = true;
    const Vector3 size = mBlurredImage.GetCurrentSize();
    DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] run=%u ResourceReady opacity=%.3f size=%.1fx%.1f attachOnReady=%d\n",
                          mRunId,
                          mBlurredImage.GetCurrentProperty<float>(Actor::Property::OPACITY),
                          size.width,
                          size.height,
                          static_cast<int>(mConfig.attachOnReady));

    if(mAttachPending)
    {
      ApplyGpuBlur();
      mAttachPending = false;
    }
    else if(mBlurEffect)
    {
      mBlurEffect.Refresh();
    }

    ArmFboReadback();
    if(mFadeOnNextReady)
    {
      mFadeOnNextReady = false;
      StartFade(DISPLAY_OPACITY, FADE_DURATION_SECONDS, false);
    }
    UpdateStatus();
  }

  void ArmFboReadback()
  {
    mDebugRenderTasks.clear();
    RenderTaskList taskList = mWindow.GetRenderTaskList();
    for(uint32_t index = 0u; index < taskList.GetTaskCount(); ++index)
    {
      RenderTask task = taskList.GetTask(index);
      if(task.GetFrameBuffer())
      {
        task.KeepRenderResult();
        mDebugRenderTasks.push_back(task);
      }
    }
    DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] run=%u keeping %zu FBO RenderTask result(s)\n",
                          mRunId,
                          mDebugRenderTasks.size());
  }

  void OnBlurFinished()
  {
    SaveFboResults();
    UpdateStatus();
  }

  void SaveFboResults()
  {
    uint32_t savedCount = 0u;
    for(size_t index = 0u; index < mDebugRenderTasks.size(); ++index)
    {
      PixelData pixelData = mDebugRenderTasks[index].GetRenderResult();
      if(!pixelData)
      {
        DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] FBO[%zu] has no retained result\n", index);
        continue;
      }

      const auto        pixelBuffer = Dali::Integration::GetPixelDataBuffer(pixelData);
      const std::string path        = "/tmp/gaussian-blur-run-" + std::to_string(mRunId) +
                               "-fbo-" + std::to_string(index) + ".png";
      const bool saved = EncodeToFile(pixelBuffer.buffer,
                                      path,
                                      pixelData.GetPixelFormat(),
                                      pixelData.GetWidth(),
                                      pixelData.GetHeight());
      savedCount += saved ? 1u : 0u;
      DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] FBO[%zu] %ux%u saved=%d path=%s\n",
                            index,
                            pixelData.GetWidth(),
                            pixelData.GetHeight(),
                            static_cast<int>(saved),
                            path.c_str());
    }
    mFboDumpCount = savedCount;
  }

  void StartManualFadeIn()
  {
    if(!mBlurredImage)
    {
      return;
    }
    mBlurredImage.SetOpacity(0.0f);
    StartFade(DISPLAY_OPACITY, FADE_DURATION_SECONDS, false);
  }

  void StartManualFadeOut()
  {
    if(!mBlurredImage)
    {
      return;
    }
    mBlurredImage.SetOpacity(DISPLAY_OPACITY);
    StartFade(0.0f, FADE_DURATION_SECONDS, false);
  }

  void StartFadeCycle()
  {
    if(!mBlurredImage)
    {
      return;
    }
    mBlurredImage.SetOpacity(0.0f);
    StartFade(DISPLAY_OPACITY, FADE_DURATION_SECONDS, true);
  }

  void StartFade(float target, float duration, bool reverseAtPeak)
  {
    StopFade();
    mReverseAtPeak = reverseAtPeak;
    mFadeAnimation = Animation::New(duration);
    mFadeAnimation.AnimateTo(Property(mBlurredImage, Actor::Property::OPACITY), target, AlphaFunction::EASE_IN_OUT);
    mFadeAnimation.FinishedSignal().Connect(this, &GaussianBlurImageController::OnFadeFinished);
    mFadeAnimation.Play();
  }

  void OnFadeFinished(Animation animation)
  {
    if(animation != mFadeAnimation)
    {
      return;
    }
    const bool reverse = mReverseAtPeak;
    mFadeAnimation.Reset();
    mReverseAtPeak = false;
    if(reverse)
    {
      StartFade(0.0f, FADE_DURATION_SECONDS, false);
    }
  }

  void StopFade()
  {
    if(mFadeAnimation)
    {
      mFadeAnimation.Stop();
      mFadeAnimation.Clear();
      mFadeAnimation.Reset();
    }
    mReverseAtPeak = false;
  }

  void ChangeImageAndRefresh()
  {
    if(!mBlurredImage)
    {
      return;
    }
    mImageIndex = (mImageIndex + 1u) % IMAGE_COUNT;
    mResourceReady = false;
    mAttachPending = false;
    mFadeOnNextReady = false;
    mBlurredImage.SetResourceUrl(IMAGE_URLS[mImageIndex]);
    mSharpReference.SetResourceUrl(IMAGE_URLS[mImageIndex]);
    UpdateStatus();
  }

  void RefreshAndDump()
  {
    if(!mBlurEffect)
    {
      return;
    }
    ++mRunId;
    mFboDumpCount = 0u;
    mBlurEffect.Refresh();
    ArmFboReadback();
    DALI_LOG_RELEASE_INFO("[GAUSSIAN-BLUR-SAMPLE] run=%u manual Refresh + FBO read-back\n", mRunId);
    UpdateStatus();
  }

  void UpdateStatus()
  {
    if(!mStatusLabel || !mBlurredImage)
    {
      return;
    }

    const float opacity = mBlurredImage.GetCurrentProperty<float>(Actor::Property::OPACITY);
    std::ostringstream status;
    status << "Run " << mRunId << "  |  " << mConfig.name << "  |  opacity "
           << std::fixed << std::setprecision(3) << opacity
           << "  |  ResourceReady " << (mResourceReady ? "yes" : "waiting")
           << "  |  effect " << (mBlurEffect && mBlurEffect.IsActivated() ? "active" : "none")
           << "  |  FBO PNG " << mFboDumpCount << "\n"
           << mConfig.detail << "  |  output: /tmp/gaussian-blur-run-" << mRunId << "-fbo-*.png";
    mStatusLabel.SetText(status.str().c_str());

    std::ostringstream badge;
    badge << mConfig.name << "    opacity " << std::fixed << std::setprecision(2) << opacity;
    mPreviewBadge.SetText(badge.str().c_str());
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    const std::string key = event.GetKeyName().CStr();
    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(key == "1") RunScenario(Scenario::ORIGINAL_EARLY_ZERO, true);
    else if(key == "2") RunScenario(Scenario::READY_ATTACH_ZERO, true);
    else if(key == "3") RunScenario(Scenario::READY_ATTACH_OPAQUE, true);
    else if(key == "4") RunScenario(Scenario::READY_ATTACH_EPSILON, true);
    else if(key == "5") RunScenario(Scenario::EARLY_ATTACH_EPSILON, true);
    else if(key == "6") RunScenario(Scenario::EARLY_ZERO_STATIC, true);
    else if(key == "7") RunScenario(Scenario::CONTINUOUS_BLUR, true);
    else if(key == "i" || key == "I") StartManualFadeIn();
    else if(key == "o" || key == "O") StartManualFadeOut();
    else if(key == "c" || key == "C") StartFadeCycle();
    else if(key == "space" || key == "Space") ChangeImageAndRefresh();
    else if(key == "r" || key == "R") RefreshAndDump();
  }

private:
  Application&            mApplication;
  Window                  mWindow;
  AbsoluteLayout          mPreview;
  AbsoluteLayout          mImageLayer;
  StackLayout             mControlPanel;
  ImageView               mBlurredImage;
  ImageView               mSharpReference;
  Label                   mPreviewBadge;
  Label                   mStatusLabel;
  GaussianBlurEffect      mBlurEffect;
  Animation               mFadeAnimation;
  Timer                   mStartTimer;
  Timer                   mStatusTimer;
  Timer                   mLoadTimer;
  Timer                   mResourceTimer;
  std::vector<RenderTask> mDebugRenderTasks;
  Scenario                mScenario{Scenario::ORIGINAL_EARLY_ZERO};
  ScenarioConfig          mConfig{GetScenarioConfig(Scenario::ORIGINAL_EARLY_ZERO)};
  uint32_t                mImageIndex{0u};
  uint32_t                mRunId{0u};
  uint32_t                mFboDumpCount{0u};
  bool                    mResourceReady{false};
  bool                    mReverseAtPeak{false};
  bool                    mAttachPending{false};
  bool                    mFadeOnNextReady{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Scenario initialScenario = Scenario::ORIGINAL_EARLY_ZERO;
  for(int index = 1; index < argc; ++index)
  {
    const std::string argument = argv[index];
    if(argument == "--resource-ready-attach") initialScenario = Scenario::READY_ATTACH_ZERO;
    else if(argument == "--resource-ready-opaque") initialScenario = Scenario::READY_ATTACH_OPAQUE;
    else if(argument == "--resource-ready-epsilon") initialScenario = Scenario::READY_ATTACH_EPSILON;
    else if(argument == "--early-epsilon") initialScenario = Scenario::EARLY_ATTACH_EPSILON;
    else if(argument == "--early-zero-static") initialScenario = Scenario::EARLY_ZERO_STATIC;
    else if(argument == "--continuous") initialScenario = Scenario::CONTINUOUS_BLUR;
  }

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  GaussianBlurImageController controller(application, initialScenario);
  application.MainLoop();
  return 0;
}

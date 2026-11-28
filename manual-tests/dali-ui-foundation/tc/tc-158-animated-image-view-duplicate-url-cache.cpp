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

#include "manual-test-case.h"

#include <dali-ui-components/public-api/check-box.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMAGE_URLS[] = {
  TEST_RESOURCE_DIR "/dog-anim-001.png",
  TEST_RESOURCE_DIR "/dog-anim-002.png",
  TEST_RESOURCE_DIR "/dog-anim-003.png",
};

constexpr float    PREVIEW_SIZE   = 200.0f;
constexpr float    SLIDER_WIDTH   = 324.0f;
constexpr float    SLIDER_HEIGHT  = 40.0f;
constexpr float    SLIDER_STEP    = 60.0f;
constexpr float    SLIDER_MARGIN  = 12.0f;
constexpr float    BUTTON_HEIGHT  = 54.0f;
constexpr uint32_t BACKGROUND     = 0x20242A;
constexpr uint32_t BUTTON_COLOR   = 0x455A64;
constexpr uint32_t SELECTED_COLOR = 0x1565C0;
constexpr uint32_t TEXT_COLOR     = 0xFFFFFF;

const char* PlayStateName(AnimatedImage::PlayState state)
{
  switch(state)
  {
    case AnimatedImage::PlayState::PLAYING:
      return "PLAYING";
    case AnimatedImage::PlayState::PAUSED:
      return "PAUSED";
    case AnimatedImage::PlayState::STOPPED:
      return "STOPPED";
  }
  return "UNKNOWN";
}
} // namespace

/**
 * @brief Exercises duplicate image URLs with fixed and rolling animated image caches.
 *
 * Changing the URL order or either slider destroys the current view. A one-second timer
 * then adds a new view, so pending loads and observer connections from the old view
 * can finish before the next sequence starts.
 */
class TcAnimatedImageViewDuplicateUrlCache : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "158. AnimatedImageView: duplicate URL cache";
  }

  Dali::String GetDescription() const override
  {
    return "Compare 1-2-3-1-2-3 and 1-1-2-2-3-3 with batch/cache sliders and playback controls";
  }

  void OnEnter(View contentArea) override
  {
    mBatchSize     = 2;
    mCacheSize     = 3;
    mAdjacentUrls  = false;
    mReloadPending = false;

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetSpacing(12.0f);
    content.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    content.SetBackgroundColor(UiColor(BACKGROUND));

    mPreview = StackLayout::New(StackOrientation::HORIZONTAL);
    mPreview.SetRequestedWidth(MATCH_PARENT);
    mPreview.SetRequestedHeight(PREVIEW_SIZE + 16.0f);
    mPreview.Add(ManualTest::MakeWeightedSpacer());
    mRightSpacer = ManualTest::MakeWeightedSpacer();
    mPreview.Add(mRightSpacer);
    content.Add(mPreview);

    mStatus = MakeLabel("", 17.0f);
    mStatus.SetRequestedWidth(MATCH_PARENT);
    mStatus.SetRequestedHeight(66.0f);
    mStatus.SetMultiLine(true);
    content.Add(mStatus);

    StackLayout orders = StackLayout::New(StackOrientation::HORIZONTAL);
    orders.SetRequestedWidth(MATCH_PARENT);
    orders.SetRequestedHeight(BUTTON_HEIGHT);
    orders.Add(MakeButton("1-2-3-1-2-3", [this]
    {
      SetOrder(false);
    }));
    orders.Add(MakeButton("1-1-2-2-3-3", [this]
    {
      SetOrder(true);
    }));
    content.Add(orders);

    content.Add(MakeSliderRow("Batch size", true));
    content.Add(MakeSliderRow("Cache size", false));

    mAutoPlay = CheckBox::New("Play automatically after re-add");
    mAutoPlay.SetRequestedWidth(MATCH_PARENT);
    mAutoPlay.SetRequestedHeight(52.0f);
    mAutoPlay.SetSelected(false);
    content.Add(mAutoPlay);

    StackLayout playback = StackLayout::New(StackOrientation::HORIZONTAL);
    playback.SetRequestedWidth(MATCH_PARENT);
    playback.SetRequestedHeight(BUTTON_HEIGHT);
    playback.Add(MakeButton("Play", [this]
    {
      if(mView)
      {
        mView.Play();
        UpdateStatus();
      }
    }));
    playback.Add(MakeButton("Pause", [this]
    {
      if(mView)
      {
        mView.Pause();
        UpdateStatus();
      }
    }));
    playback.Add(MakeButton("Stop", [this]
    {
      if(mView)
      {
        mView.Stop();
        UpdateStatus();
      }
    }));
    content.Add(playback);

    mReloadTimer = Timer::New(1000u);
    mReloadTimer.TickSignal().Connect(this, &TcAnimatedImageViewDuplicateUrlCache::OnReloadTick);
    mStatusTimer = Timer::New(100u);
    mStatusTimer.TickSignal().Connect(this, &TcAnimatedImageViewDuplicateUrlCache::OnStatusTick);
    mStatusTimer.Start();

    contentArea.Add(content);
    AddView();
  }

  void OnExit() override
  {
    if(mReloadTimer)
    {
      mReloadTimer.Stop();
      mReloadTimer.Reset();
    }
    if(mStatusTimer)
    {
      mStatusTimer.Stop();
      mStatusTimer.Reset();
    }
    if(mView)
    {
      mView.Unparent();
      mView.Reset();
    }
    mPreview.Reset();
    mRightSpacer.Reset();
    mStatus.Reset();
    mAutoPlay.Reset();
    mBatchSlider.Reset();
    mCacheSlider.Reset();
    mBatchThumb.Reset();
    mCacheThumb.Reset();
  }

private:
  Label MakeLabel(const Dali::String& text, float fontSize)
  {
    Label label = Label::New(text);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(TEXT_COLOR));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  View MakeButton(const Dali::String& text, std::function<void()> callback)
  {
    InteractiveView button = InteractiveView::New();
    button.SetRequestedHeight(BUTTON_HEIGHT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(BUTTON_COLOR));
    Label label = MakeLabel(text, 16.0f);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    button.ConnectClickedSignal(this, [callback = std::move(callback)](View, InputEvent) -> bool
    {
      callback();
      return true;
    });
    return button;
  }

  StackLayout MakeSliderRow(const Dali::String& name, bool batch)
  {
    StackLayout row = StackLayout::New(StackOrientation::VERTICAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(92.0f);

    Label label = MakeLabel(name, 16.0f);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(32.0f);
    row.Add(label);

    AbsoluteLayout slider = AbsoluteLayout::New();
    slider.SetRequestedWidth(SLIDER_WIDTH);
    slider.SetRequestedHeight(SLIDER_HEIGHT);
    slider.SetBackgroundColor(UiColor(BUTTON_COLOR));
    for(int index = 0; index < 6; ++index)
    {
      View tick = View::New();
      tick.SetBackgroundColor(UiColor(TEXT_COLOR));
      tick.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(SLIDER_MARGIN + index * SLIDER_STEP - 2.0f, 18.0f, 4.0f, 4.0f)));
      tick.SetSensitive(false);
      slider.Add(tick);
    }

    View thumb = View::New();
    thumb.SetBackgroundColor(UiColor(SELECTED_COLOR));
    thumb.SetSensitive(false);
    slider.Add(thumb);
    if(batch)
    {
      mBatchSlider = slider;
      mBatchThumb  = thumb;
    }
    else
    {
      mCacheSlider = slider;
      mCacheThumb  = thumb;
    }
    slider.TouchEventSignal().Connect(this, [this, batch](Actor, TouchEvent event) -> bool
    {
      if(event.GetPointCount() == 0u)
      {
        return false;
      }
      PointState::Type state = event.GetState(0u);
      if(state == PointState::DOWN || state == PointState::MOTION)
      {
        int value = std::clamp(static_cast<int>(std::lround((event.GetLocalPosition(0u).x - SLIDER_MARGIN) / SLIDER_STEP)) + 1, 1, 6);
        SetSize(batch, value);
      }
      return true;
    });
    row.Add(slider);
    Label values = MakeLabel("1        2        3        4        5        6", 12.0f);
    values.SetRequestedWidth(SLIDER_WIDTH);
    values.SetRequestedHeight(20.0f);
    row.Add(values);
    UpdateThumb(batch);
    return row;
  }

  void UpdateThumb(bool batch)
  {
    View thumb = batch ? mBatchThumb : mCacheThumb;
    int  value = batch ? mBatchSize : mCacheSize;
    thumb.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
      LayoutRect(SLIDER_MARGIN + (value - 1) * SLIDER_STEP - 10.0f, 10.0f, 20.0f, 20.0f)));
  }

  void SetSize(bool batch, int value)
  {
    int& size = batch ? mBatchSize : mCacheSize;
    if(size == value)
    {
      return;
    }
    size = value;
    UpdateThumb(batch);
    ScheduleReload();
  }

  void SetOrder(bool adjacentUrls)
  {
    if(mAdjacentUrls == adjacentUrls)
    {
      return;
    }
    mAdjacentUrls = adjacentUrls;
    ScheduleReload();
  }

  void ScheduleReload()
  {
    mReloadTimer.Stop();
    if(mView)
    {
      mView.Unparent();
      mView.Reset();
    }
    mReloadPending = true;
    UpdateStatus();
    mReloadTimer.Start();
  }

  bool OnReloadTick()
  {
    mReloadPending = false;
    AddView();
    return false;
  }

  bool OnStatusTick()
  {
    UpdateStatus();
    return true;
  }

  void AddView()
  {
    mView = AnimatedImageView::New();
    Dali::Vector<Dali::String> urls;
    if(mAdjacentUrls)
    {
      for(const char* url : IMAGE_URLS)
      {
        urls.PushBack(url);
        urls.PushBack(url);
      }
    }
    else
    {
      for(int repeat = 0; repeat < 2; ++repeat)
      {
        for(const char* url : IMAGE_URLS)
        {
          urls.PushBack(url);
        }
      }
    }
    mView.SetBatchSize(mBatchSize);
    mView.SetCacheSize(mCacheSize);
    mView.SetFrameDelay(300);
    mView.SetReleasePolicy(Image::ReleasePolicy::DETACHED);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);
    mView.SetResourceUrlList(urls);
    mRightSpacer.Unparent();
    mPreview.Add(mView);
    mPreview.Add(mRightSpacer);
    if(mAutoPlay.IsSelected())
    {
      mView.Play();
    }
    UpdateStatus();
  }

  void UpdateStatus()
  {
    std::string status = mAdjacentUrls ? "Order: 1-1-2-2-3-3" : "Order: 1-2-3-1-2-3";
    status += " | Batch: " + std::to_string(mBatchSize) + " | Cache: " + std::to_string(mCacheSize);
    if(mReloadPending)
    {
      status += " | Cleared; re-add in 1 s";
    }
    else if(mView)
    {
      status += " | " + std::string(PlayStateName(mView.GetPlayState()));
      status += " | Frame: " + std::to_string(mView.GetCurrentFrameNumber());
      status += "/" + std::to_string(mView.GetTotalFrameCount());
    }
    mStatus.SetText(Dali::String(status.c_str()));
  }

private:
  StackLayout       mPreview;
  View              mRightSpacer;
  AnimatedImageView mView;
  AbsoluteLayout    mBatchSlider;
  AbsoluteLayout    mCacheSlider;
  View              mBatchThumb;
  View              mCacheThumb;
  CheckBox          mAutoPlay;
  Label             mStatus;
  Timer             mReloadTimer;
  Timer             mStatusTimer;
  int               mBatchSize{2};
  int               mCacheSize{3};
  bool              mAdjacentUrls{false};
  bool              mReloadPending{false};
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewDuplicateUrlCache)

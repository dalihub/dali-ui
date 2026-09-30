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
 *
 */

#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace TextSample
{
/**
 * A horizontal gradient slider shared by the text samples, not a platform control.
 * Include "sample-slider.h" and keep the Slider alive while its view is in use.
 * Values use the caller's range and step; only the track position is normalized.
 * The default range is 0 to 1 in steps of 0.01.
 *
 * Connect Committed to apply settings on release, or Changed for live updates.
 * SetValue updates the control silently, so application feedback does not trigger
 * another edit. Cancelled reports the original value for rolling back live updates.
 *
 * The owner handles application-specific edit state: use Started to pause playback
 * or parent scrolling, restore scrolling on Committed/Cancelled, and call Cancel
 * on window focus loss. The slider does not own a window, scroll view or animation.
 *
 * @code
 * // Member of the sample's ConnectionTracker-derived controller:
 * TextSample::Slider mOpacity{"Opacity"};
 *
 * // During UI setup:
 * row.Add(mOpacity.GetView());
 * mOpacity.Committed.Connect(this, [this](float value)
 * {
 *   mLabel.SetOpacity(value);
 * });
 * @endcode
 */
class Slider : public Dali::ConnectionTracker
{
public:
  /**
   * Creates a slider with an input/accessibility name, numeric range, step,
   * displayed decimal places and initial value. Minimum must be less than maximum,
   * step must be positive, and numeric values must be finite.
   */
  explicit Slider(const char* name, float minimum = 0.0f, float maximum = 1.0f,
                  float step = 0.01f, int decimals = 2, float value = 0.0f)
  : mMinimum(minimum),
    mMaximum(maximum),
    mStep(step),
    mDecimals(decimals)
  {
    using namespace Dali;
    using namespace Dali::Ui;
    mRoot = StackLayout::New(StackOrientation::HORIZONTAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(HEIGHT);
    mRoot.SetSpacing(8.0f);
    mRoot.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mArea = View::New();
    mArea.SetName(name);
    mArea.SetAccessibilityName(name);
    mArea.SetRequestedWidth(MATCH_PARENT);
    mArea.SetRequestedHeight(HEIGHT);
    mArea.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mArea.SetBackgroundColor(Color::TRANSPARENT);
    mArea.SetFocusable(true);
    mArea.SetProperty(Actor::Property::ALLOW_SELF_INITIATED_TOUCH_ONLY, true);
    mArea.SetArrangeCallback(ArrangeCallback::New(this, &Slider::Arrange));
    mArea.TouchEventSignal().Connect(this, &Slider::OnTouch);
    mArea.KeyEventSignal().Connect(this, &Slider::OnKey);
    mRoot.Add(mArea);

    mTrack = View::New();
    mTrack.SetBackgroundColor(UiColor(0x334155));
    mTrack.SetCornerRadius(3.0f);
    mTrack.SetSensitive(false);
    mArea.Add(mTrack);

    mFill = View::New();
    Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(0xA78BFA)),
                           Gradient::StopNode(0.50f, UiColor(0x609CFF)),
                           Gradient::StopNode(1.0f, UiColor(0x45DFD4))});
    mFill.SetBackgroundGradient(gradient);
    mFill.SetCornerRadius(3.0f);
    mFill.SetSensitive(false);
    mArea.Add(mFill);

    mThumb = View::New();
    mThumb.SetBackgroundColor(UiColor(0xF8FAFC));
    mThumb.SetCornerRadius(8.0f);
    mThumb.SetBorderlineWidth(2.0f);
    mThumb.SetBorderlineOffset(-1.0f);
    mThumb.SetBorderlineColor(UiColor(0x8BBAFF));
    mThumb.SetSensitive(false);
    mArea.Add(mThumb);

    mNumber = Label::New();
    mNumber.SetFontFamily("SamsungOneUI_500");
    mNumber.SetFontSize(12.0f);
    mNumber.SetTextColor(UiColor(0xE2E8F0));
    mNumber.SetRequestedWidth(54.0f);
    mNumber.SetRequestedHeight(HEIGHT);
    mNumber.SetMultiLine(false);
    mNumber.SetHorizontalTextAlignment(Text::Alignment::END);
    mNumber.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mNumber.SetSensitive(false);
    mRoot.Add(mNumber);
    SetValue(value);
  }

  ~Slider()
  {
    // Layout callbacks are not signal connections and do not track our lifetime.
    mArea.SetArrangeCallback({});
  }

  /**
   * Returns the row containing the input track and numeric value for adding to a layout.
   */
  Dali::Ui::View GetView() const
  {
    return mRoot;
  }

  float GetValue() const
  {
    return mValue;
  }

  /**
   * Clamps and snaps to the configured range/step without emitting edit signals.
   */
  void SetValue(float value)
  {
    value = std::clamp(value, mMinimum, mMaximum);
    value = std::clamp(mMinimum + std::round((value - mMinimum) / mStep) * mStep, mMinimum, mMaximum);
    if(mHasValue && Dali::Equals(mValue, value))
    {
      return;
    }
    mHasValue = true;
    mValue    = value;
    std::ostringstream number;
    number << std::fixed << std::setprecision(mDecimals) << value;
    mNumber.SetText(number.str().c_str());
    // Re-arrange the track only; applying the value is the caller's responsibility.
    mArea.InvalidateArrange();
  }

  /**
   * Enables input or cancels an active pointer edit before disabling input.
   */
  void SetEnabled(bool enabled)
  {
    if(!enabled)
    {
      Cancel();
    }
    mEnabled = enabled;
    mArea.SetSensitive(enabled);
    mArea.SetFocusable(enabled);
    mRoot.SetOpacity(enabled ? 1.0f : 0.35f);
  }

  /**
   * Rolls back an active pointer edit and emits Cancelled; otherwise does nothing.
   */
  void Cancel()
  {
    if(mDragging)
    {
      mDragging = false;
      SetValue(mEditStart);
      Cancelled.Emit(mValue);
    }
  }

  /**
   * Emitted before a pointer edit or a Left/Right/Home/End key adjustment.
   */
  Dali::Signal<void()> Started;

  /**
   * Reports the current value during pointer edits or keyboard adjustments.
   */
  Dali::Signal<void(float)> Changed;

  /**
   * Reports the final value on pointer release or after a keyboard adjustment.
   */
  Dali::Signal<void(float)> Committed;

  /**
   * Reports the restored value when a pointer edit is cancelled or interrupted.
   */
  Dali::Signal<void(float)> Cancelled;

private:
  Dali::Ui::LayoutRect Arrange(Dali::Ui::View, const Dali::Ui::LayoutRect& bounds)
  {
    // Arrange receives visual units. Deriving all geometry from the row height
    // keeps hit coordinates and drawing in the same space at fractional scale.
    const float scale  = bounds.height / HEIGHT;
    const float inset  = 8.0f * scale;
    const float width  = std::max(0.0f, bounds.width - 2.0f * inset);
    const float filled = width * (mValue - mMinimum) / (mMaximum - mMinimum);
    mTrack.Arrange({inset, (bounds.height - 6.0f * scale) * 0.5f, width, 6.0f * scale});
    mFill.Arrange({inset, (bounds.height - 6.0f * scale) * 0.5f, filled, 6.0f * scale});
    mThumb.Arrange({filled, (bounds.height - 16.0f * scale) * 0.5f, 16.0f * scale, 16.0f * scale});
    return bounds;
  }

  void SetFromTouch(const Dali::TouchEvent& event, uint32_t point)
  {
    const auto  size   = mArea.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);
    const auto  screen = event.GetScreenPosition(point);
    float       x = 0.0f, y = 0.0f;
    const float inset = 8.0f * size.y / HEIGHT;
    const float width = size.x - 2.0f * inset;
    if(width > 0.0f && mArea.ScreenToLocal(x, y, screen.x, screen.y))
    {
      SetValue(mMinimum + std::clamp((x - inset) / width, 0.0f, 1.0f) * (mMaximum - mMinimum));
      Changed.Emit(mValue);
    }
  }

  bool OnTouch(Dali::Actor, Dali::TouchEvent event)
  {
    if(!mEnabled || event.GetPointCount() == 0u)
    {
      return false;
    }
    for(uint32_t point = 0u; point < event.GetPointCount(); ++point)
    {
      const auto state = event.GetState(point);
      if(state == Dali::PointState::DOWN && !mDragging)
      {
        mDragging  = true;
        mDevice    = event.GetDeviceId(point);
        mEditStart = mValue;
        Started.Emit();
        Dali::Ui::FocusManager::Get().SetCurrentFocusView(mArea);
        SetFromTouch(event, point);
        return true;
      }
      if(!mDragging || event.GetDeviceId(point) != mDevice)
      {
        continue;
      }
      if(state == Dali::PointState::MOTION || state == Dali::PointState::UP)
      {
        SetFromTouch(event, point);
        if(state == Dali::PointState::UP)
        {
          mDragging = false;
          Committed.Emit(mValue);
        }
      }
      else if(state == Dali::PointState::INTERRUPTED)
      {
        Cancel();
      }
      return true;
    }
    return mDragging;
  }

  bool OnKey(Dali::Ui::View, Dali::KeyEvent event)
  {
    const auto& key = event.GetKeyName();
    if(!mEnabled || (key != "Left" && key != "Right" && key != "Home" && key != "End"))
    {
      return false;
    }
    if(event.GetState() == Dali::KeyEvent::DOWN)
    {
      Cancel();
      Started.Emit();
      SetValue(key == "Home" ? mMinimum : key == "End" ? mMaximum
                                                       : mValue + (key == "Left" ? -mStep : mStep));
      Changed.Emit(mValue);
      Committed.Emit(mValue);
    }
    return true;
  }

  static constexpr float HEIGHT = 32.0f;
  Dali::Ui::StackLayout  mRoot;
  Dali::Ui::View         mArea;
  Dali::Ui::View         mTrack;
  Dali::Ui::View         mFill;
  Dali::Ui::View         mThumb;
  Dali::Ui::Label        mNumber;
  float                  mMinimum;
  float                  mMaximum;
  float                  mStep;
  int                    mDecimals;
  float                  mValue{0.0f};
  float                  mEditStart{0.0f};
  int32_t                mDevice{-1};
  bool                   mHasValue{false};
  bool                   mDragging{false};
  bool                   mEnabled{true};
};
} // namespace TextSample

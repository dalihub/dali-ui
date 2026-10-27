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
#include "chart-number-transition.h"
#include <algorithm>
#include <cmath>

using namespace Dali;
using namespace Dali::Ui;

namespace ChartStandard
{
namespace
{
void PlaceNumber(View view, float x, float y, float width, float height)
{
  view.SetLayoutParams(AbsoluteLayoutParams::New().SetX(x).SetY(y).SetWidth(width).SetHeight(height));
}
} //namespace

NumberTransition::NumberTransition(AbsoluteLayout parent, float x, float y, float width, const Vector4& color)
: mColor(color),
  mRoot(AbsoluteLayout::New()),
  mCurrent(Label::New()),
  mNext(Label::New()),
  mWidth(width)
{
  PlaceNumber(mRoot, x, y, width, 44.0f);
  mRoot.SetProperty(Actor::Property::CLIPPING_MODE, static_cast<int>(ClippingMode::CLIP_CHILDREN));
  for(auto label : {mCurrent, mNext})
  {
    label.SetFontSize(28.0f);
    label.SetFontWeight(Text::FontWeight::BOLD);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetTextColor(UiColor(color));
    PlaceNumber(label, 0.0f, 0.0f, width, 44.0f);
    mRoot.Add(label);
  }
  parent.Add(mRoot);
  SetValue(0.0, 0);
}

void NumberTransition::SetValue(double value, int digits)
{
  mMode  = NumberMode::NONE;
  mValue = mTarget = mFrom = value;
  mDigits                  = digits;
  for(auto label : mOldDigits)
  {
    label.Unparent();
  }
  for(auto label : mNewDigits)
  {
    label.Unparent();
  }
  mOldDigits.clear();
  mNewDigits.clear();
  mCurrent.SetText(Dali::String(FormatNumber(value, digits).c_str()));
  mCurrent.SetProperty(Actor::Property::VISIBLE, true);
  mCurrent.SetProperty(Actor::Property::COLOR_MULTIPLIER_ALPHA, 1.0f);
  mNext.SetProperty(Actor::Property::VISIBLE, false);
}

void NumberTransition::Start(double target, NumberMode mode, int digits, float duration)
{
  const double start = mValue;
  SetValue(start, digits);
  mFrom     = start;
  mTarget   = target;
  mMode     = mode;
  mDuration = std::max(0.001f, duration);
  mElapsed  = 0.0f;
  if(mode == NumberMode::FADE)
  {
    mNext.SetText(Dali::String(FormatNumber(target, digits).c_str()));
    mNext.SetProperty(Actor::Property::VISIBLE, true);
    mNext.SetProperty(Actor::Property::COLOR_MULTIPLIER_ALPHA, 0.0f);
  }
  else if(mode == NumberMode::REVOLVE)
  {
    RebuildDigits();
  }
}

void NumberTransition::RebuildDigits()
{
  auto         before = FormatNumber(mFrom, mDigits);
  auto         after  = FormatNumber(mTarget, mDigits);
  const size_t count  = std::max(before.size(), after.size());
  before.insert(0, count - before.size(), ' ');
  after.insert(0, count - after.size(), ' ');
  mCurrent.SetProperty(Actor::Property::VISIBLE, false);
  for(size_t i = 0; i < count; ++i)
  {
    auto oldLabel = Label::New(Dali::String(std::string(1, before[i]).c_str()));
    auto newLabel = Label::New(Dali::String(std::string(1, after[i]).c_str()));
    for(auto label : {oldLabel, newLabel})
    {
      label.SetFontSize(28.0f);
      label.SetFontWeight(Text::FontWeight::BOLD);
      label.SetTextColor(UiColor(mColor));
      mRoot.Add(label);
    }
    const float left = (mWidth - count * 20.0f) * 0.5f + i * 20.0f;
    PlaceNumber(oldLabel, left, 0.0f, 22.0f, 44.0f);
    PlaceNumber(newLabel, left, mTarget >= mFrom ? 44.0f : -44.0f, 22.0f, 44.0f);
    mOldDigits.push_back(oldLabel);
    mNewDigits.push_back(newLabel);
  }
}

void NumberTransition::Advance(float seconds)
{
  if(mMode == NumberMode::NONE)
  {
    return;
  }
  mElapsed += std::max(0.0f, seconds);
  const float progress = std::min(1.0f, mElapsed / mDuration);
  const float eased    = 1.0f - std::pow(1.0f - progress, 3.0f);
  mValue               = mFrom + (mTarget - mFrom) * eased;
  if(mMode == NumberMode::COUNT)
  {
    mCurrent.SetText(Dali::String(FormatNumber(mValue, mDigits).c_str()));
  }
  else if(mMode == NumberMode::FADE)
  {
    mCurrent.SetProperty(Actor::Property::COLOR_MULTIPLIER_ALPHA, 1.0f - progress);
    mNext.SetProperty(Actor::Property::COLOR_MULTIPLIER_ALPHA, progress);
  }
  else
  {
    const float direction = mTarget >= mFrom ? 1.0f : -1.0f;
    for(size_t i = 0; i < mOldDigits.size(); ++i)
    {
      const float left = (mWidth - mOldDigits.size() * 20.0f) * 0.5f + i * 20.0f;
      PlaceNumber(mOldDigits[i], left, -direction * 44.0f * eased, 22.0f, 44.0f);
      PlaceNumber(mNewDigits[i], left, direction * 44.0f * (1.0f - eased), 22.0f, 44.0f);
    }
  }
  if(progress >= 1.0f)
  {
    SetValue(mTarget, mDigits);
  }
}

double NumberTransition::GetValue() const
{
  return mValue;
}

bool NumberTransition::IsRunning() const
{
  return mMode != NumberMode::NONE;
}

View NumberTransition::GetView() const
{
  return mRoot;
}
} //namespace ChartStandard

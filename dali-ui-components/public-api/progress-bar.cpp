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

#include <dali-ui-components/public-api/progress-bar.h>

#include <dali-ui-components/internal/progress-bar-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

ProgressBar::ProgressBar() = default;

ProgressBar ProgressBar::New()
{
  return New(0.0f, 1.0f, ProgressBarStyle::Default());
}

ProgressBar ProgressBar::New(ProgressBarStyle style)
{
  return New(0.0f, 1.0f, style);
}

ProgressBar ProgressBar::New(float minimumValue, float maximumValue)
{
  return New(minimumValue, maximumValue, ProgressBarStyle::Default());
}

ProgressBar ProgressBar::New(float minimumValue, float maximumValue, ProgressBarStyle style)
{
  return Internal::ProgressBarImpl::New(minimumValue, maximumValue, style);
}

ProgressBar ProgressBar::DownCast(BaseHandle handle)
{
  return View::DownCast<ProgressBar, Internal::ProgressBarImpl>(handle);
}

ProgressBar::ProgressBar(const ProgressBar& progressBar) = default;

ProgressBar::ProgressBar(ProgressBar&& rhs) noexcept = default;

ProgressBar::~ProgressBar() = default;

void ProgressBar::SetTrackThickness(float thickness)
{
  GetImpl(*this).SetTrackThickness(thickness);
}

float ProgressBar::GetTrackThickness() const
{
  return GetImpl(*this).GetTrackThickness();
}

void ProgressBar::SetTrackColor(const UiColor& color)
{
  GetImpl(*this).SetTrackColor(color);
}

UiColor ProgressBar::GetTrackColor() const
{
  return GetImpl(*this).GetTrackColor();
}

void ProgressBar::SetTrailColor(const UiColor& color)
{
  GetImpl(*this).SetTrailColor(color);
}

UiColor ProgressBar::GetTrailColor() const
{
  return GetImpl(*this).GetTrailColor();
}

void ProgressBar::SetDividerStepCount(int32_t count)
{
  GetImpl(*this).SetDividerStepCount(count);
}

int32_t ProgressBar::GetDividerStepCount() const
{
  return GetImpl(*this).GetDividerStepCount();
}

void ProgressBar::SetReversed(bool reversed)
{
  GetImpl(*this).SetReversed(reversed);
}

bool ProgressBar::IsReversed() const
{
  return GetImpl(*this).IsReversed();
}

ProgressBar::TrailEndPositionChangedSignalType& ProgressBar::TrailEndPositionChangedSignal()
{
  return GetImpl(*this).TrailEndPositionChangedSignal();
}

ProgressBar::DividersChangedSignalType& ProgressBar::DividersChangedSignal()
{
  return GetImpl(*this).DividersChangedSignal();
}

Vector2 ProgressBar::GetTrailEndPosition() const
{
  return GetImpl(*this).GetTrailEndPosition();
}

uint32_t ProgressBar::GetDividerCount() const
{
  return GetImpl(*this).GetDividerCount();
}

Vector2 ProgressBar::GetDividerPosition(uint32_t index) const
{
  return GetImpl(*this).GetDividerPosition(index);
}

ProgressBar::ProgressBar(Extension::ProgressViewImpl& implementation)
: ProgressView(implementation)
{
  DALI_ASSERT_ALWAYS(dynamic_cast<Internal::ProgressBarImpl*>(&implementation));
}

ProgressBar::ProgressBar(Dali::Internal::CustomActor* internal)
: ProgressView(internal)
{
  VerifyCustomActorPointer<Internal::ProgressBarImpl>(internal);
}

} // namespace Ui
} //namespace DALI_NAMESPACE

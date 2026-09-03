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

#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/public-api/math/math-utils.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>

#include <dali-ui-foundation/extension-api/progress-view-impl.h>
#include <dali-ui-foundation/extension-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::ProgressView, Ui::Extension::ProgressViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

void AssertValidRange(float minimumValue, float maximumValue)
{
  DALI_ASSERT_ALWAYS(std::isfinite(minimumValue) && std::isfinite(maximumValue) &&
                     maximumValue > minimumValue &&
                     "ProgressView range endpoints must be finite and maximum must exceed minimum");
}

float ClampValue(float value, float minimumValue, float maximumValue)
{
  return std::max(minimumValue, std::min(maximumValue, value));
}

double CalculateRatioDouble(float value, float minimumValue, float maximumValue)
{
  const double numerator   = static_cast<double>(value) - static_cast<double>(minimumValue);
  const double denominator = static_cast<double>(maximumValue) - static_cast<double>(minimumValue);
  return std::max(0.0, std::min(1.0, numerator / denominator));
}

float CalculateRatio(float value, float minimumValue, float maximumValue)
{
  return static_cast<float>(CalculateRatioDouble(value, minimumValue, maximumValue));
}

bool ShouldSnapValue(int32_t count)
{
  return count > 0;
}

float SnapValue(float value, float minimumValue, float maximumValue, int32_t count)
{
  if(!ShouldSnapValue(count))
  {
    return value;
  }

  const double  ratio   = CalculateRatioDouble(value, minimumValue, maximumValue);
  const double  rounded = std::floor(ratio * static_cast<double>(count) + 0.5);
  const int32_t index   = static_cast<int32_t>(std::max(0.0, std::min(static_cast<double>(count), rounded)));
  if(index == 0)
  {
    return minimumValue;
  }
  if(index == count)
  {
    return maximumValue;
  }

  const double intervalRatio = static_cast<double>(index) / static_cast<double>(count);
  const double result        = static_cast<double>(minimumValue) +
                        (static_cast<double>(maximumValue) - static_cast<double>(minimumValue)) * intervalRatio;
  return ClampValue(static_cast<float>(result), minimumValue, maximumValue);
}

float CoerceValue(float value, float minimumValue, float maximumValue, int32_t count)
{
  const float clamped = ClampValue(value, minimumValue, maximumValue);
  return SnapValue(clamped, minimumValue, maximumValue, count);
}

void NotifyDefaultAccessibilityValueChanged(Ui::ProgressView view)
{
  if(view.GetAccessibilityValue().Empty())
  {
    Dali::Ui::Extension::View::NotifyAccessibilityValueChanged(view);
  }
}

} // namespace

struct ProgressViewImpl::Impl
{
  Impl(float minimumValue, float maximumValue)
  : minimum(minimumValue),
    maximum(maximumValue),
    value(minimumValue)
  {
  }

  float                                      minimum;
  float                                      maximum;
  float                                      value;
  float                                      ratio{0.0f};
  int32_t                                    valueStepCount{0};
  bool                                       determinate{true};
  ProgressView::ValueChangedSignalType       valueChangedSignal;
  ProgressView::DeterminateChangedSignalType determinateChangedSignal;
};

ProgressViewImpl::ProgressViewImpl(float minimumValue, float maximumValue)
: mImpl(Dali::MakeUnique<Impl>(minimumValue, maximumValue))
{
  AssertValidRange(minimumValue, maximumValue);
}

ProgressViewImpl::~ProgressViewImpl() = default;

void ProgressViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Ui::View::DownCast(Self()).SetAccessibilityRole(Accessibility::Role::PROGRESS_BAR);
}

ProgressView::ValueChangedSignalType& ProgressViewImpl::ValueChangedSignal()
{
  return mImpl->valueChangedSignal;
}

ProgressView::DeterminateChangedSignalType& ProgressViewImpl::DeterminateChangedSignal()
{
  return mImpl->determinateChangedSignal;
}

void ProgressViewImpl::SetValue(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && "ProgressView value must be finite");

  const float effectiveValue = CoerceValue(value, mImpl->minimum, mImpl->maximum, mImpl->valueStepCount);
  const float ratio          = CalculateRatio(effectiveValue, mImpl->minimum, mImpl->maximum);
  const bool  valueChanged   = !Dali::Equals(effectiveValue, mImpl->value);
  const bool  ratioChanged   = !Dali::Equals(ratio, mImpl->ratio);
  if(!valueChanged && !ratioChanged)
  {
    return;
  }

  mImpl->value = effectiveValue;
  mImpl->ratio = ratio;

  if(ratioChanged)
  {
    OnValueRatioChanged(ratio);
    if(mImpl->determinate)
    {
      NotifyDefaultAccessibilityValueChanged(Ui::ProgressView::DownCast(Self()));
    }
  }

  if(valueChanged)
  {
    Ui::ProgressView self = Ui::ProgressView::DownCast(Self());
    mImpl->valueChangedSignal.Emit(self, effectiveValue);
  }
}

float ProgressViewImpl::GetValue() const
{
  return mImpl->value;
}

void ProgressViewImpl::SetRange(float minimumValue, float maximumValue)
{
  AssertValidRange(minimumValue, maximumValue);

  const float effectiveValue = CoerceValue(mImpl->value, minimumValue, maximumValue, mImpl->valueStepCount);
  const float ratio          = CalculateRatio(effectiveValue, minimumValue, maximumValue);
  const bool  rangeChanged   = !Dali::Equals(minimumValue, mImpl->minimum) || !Dali::Equals(maximumValue, mImpl->maximum);
  const bool  valueChanged   = !Dali::Equals(effectiveValue, mImpl->value);
  const bool  ratioChanged   = !Dali::Equals(ratio, mImpl->ratio);
  if(!rangeChanged && !valueChanged && !ratioChanged)
  {
    return;
  }

  mImpl->minimum = minimumValue;
  mImpl->maximum = maximumValue;
  mImpl->value   = effectiveValue;
  mImpl->ratio   = ratio;

  if(ratioChanged)
  {
    OnValueRatioChanged(ratio);
    if(mImpl->determinate)
    {
      NotifyDefaultAccessibilityValueChanged(Ui::ProgressView::DownCast(Self()));
    }
  }

  if(valueChanged)
  {
    Ui::ProgressView self = Ui::ProgressView::DownCast(Self());
    mImpl->valueChangedSignal.Emit(self, effectiveValue);
  }
}

void ProgressViewImpl::GetRange(float& minimumValue, float& maximumValue) const
{
  minimumValue = mImpl->minimum;
  maximumValue = mImpl->maximum;
}

void ProgressViewImpl::SetValueStepCount(int32_t count)
{
  DALI_ASSERT_ALWAYS(count >= 0 && "ProgressView value step count must be non-negative");
  if(count == mImpl->valueStepCount)
  {
    return;
  }

  const float effectiveValue = CoerceValue(mImpl->value, mImpl->minimum, mImpl->maximum, count);
  const float ratio          = CalculateRatio(effectiveValue, mImpl->minimum, mImpl->maximum);
  const bool  valueChanged   = !Dali::Equals(effectiveValue, mImpl->value);
  const bool  ratioChanged   = !Dali::Equals(ratio, mImpl->ratio);

  mImpl->valueStepCount = count;
  mImpl->value          = effectiveValue;
  mImpl->ratio          = ratio;

  if(ratioChanged)
  {
    OnValueRatioChanged(ratio);
    if(mImpl->determinate)
    {
      NotifyDefaultAccessibilityValueChanged(Ui::ProgressView::DownCast(Self()));
    }
  }

  if(valueChanged)
  {
    Ui::ProgressView self = Ui::ProgressView::DownCast(Self());
    mImpl->valueChangedSignal.Emit(self, effectiveValue);
  }
}

int32_t ProgressViewImpl::GetValueStepCount() const
{
  return mImpl->valueStepCount;
}

void ProgressViewImpl::SetDeterminate(bool determinate)
{
  if(determinate == mImpl->determinate)
  {
    return;
  }

  mImpl->determinate = determinate;
  OnDeterminateChanged(determinate);

  Ui::ProgressView self = Ui::ProgressView::DownCast(Self());
  NotifyDefaultAccessibilityValueChanged(self);
  mImpl->determinateChangedSignal.Emit(self, determinate);
}

bool ProgressViewImpl::IsDeterminate() const
{
  return mImpl->determinate;
}

float ProgressViewImpl::GetValueRatio() const
{
  return mImpl->ratio;
}

bool ProgressViewImpl::OnAccessibilityRequestValue(Dali::String& value)
{
  Ui::View           self          = Ui::View::DownCast(Self());
  const Dali::String explicitValue = self.GetAccessibilityValue();
  if(!explicitValue.Empty())
  {
    value = explicitValue;
    return true;
  }

  if(!mImpl->determinate)
  {
    return false;
  }

  const long        percentage     = std::lround(static_cast<double>(mImpl->ratio) * 100.0);
  const std::string percentageText = std::to_string(percentage) + "%";
  value                            = Dali::String(percentageText.c_str());
  return true;
}

} // namespace Extension
} // namespace Ui
} // namespace Dali

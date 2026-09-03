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

#include <dali-ui-foundation/extension-api/progress-view-impl.h>
#include <dali-ui-foundation/public-api/views/progress-view.h>

namespace Dali
{
namespace Ui
{

ProgressView::ProgressView() = default;

ProgressView ProgressView::DownCast(BaseHandle handle)
{
  return View::DownCast<ProgressView, Extension::ProgressViewImpl>(handle);
}

ProgressView::ProgressView(const ProgressView& progressView) = default;

ProgressView::ProgressView(ProgressView&& rhs) noexcept = default;

ProgressView::~ProgressView() = default;

ProgressView::ValueChangedSignalType& ProgressView::ValueChangedSignal()
{
  return Extension::GetImpl(*this).ValueChangedSignal();
}

ProgressView::DeterminateChangedSignalType& ProgressView::DeterminateChangedSignal()
{
  return Extension::GetImpl(*this).DeterminateChangedSignal();
}

void ProgressView::SetValue(float value)
{
  Extension::GetImpl(*this).SetValue(value);
}

float ProgressView::GetValue() const
{
  return Extension::GetImpl(*this).GetValue();
}

void ProgressView::SetRange(float minimumValue, float maximumValue)
{
  Extension::GetImpl(*this).SetRange(minimumValue, maximumValue);
}

void ProgressView::GetRange(float& minimumValue, float& maximumValue) const
{
  Extension::GetImpl(*this).GetRange(minimumValue, maximumValue);
}

void ProgressView::SetValueStepCount(int32_t count)
{
  Extension::GetImpl(*this).SetValueStepCount(count);
}

int32_t ProgressView::GetValueStepCount() const
{
  return Extension::GetImpl(*this).GetValueStepCount();
}

void ProgressView::SetDeterminate(bool determinate)
{
  Extension::GetImpl(*this).SetDeterminate(determinate);
}

bool ProgressView::IsDeterminate() const
{
  return Extension::GetImpl(*this).IsDeterminate();
}

ProgressView::ProgressView(Extension::ProgressViewImpl& implementation)
: View(implementation)
{
}

ProgressView::ProgressView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Extension::ProgressViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali

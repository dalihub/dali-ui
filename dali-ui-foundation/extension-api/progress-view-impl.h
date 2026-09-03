#pragma once

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

#include <dali/public-api/common/unique-ptr.h>

#include <dali-ui-foundation/public-api/views/progress-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{

/**
 * @brief ABI-stable implementation base for rendered ProgressView components.
 *
 * Derived renderers receive normalized value and determinate-state changes through the two
 * protected virtual hooks. Range coercion, public signals and accessibility values remain
 * common non-virtual behavior.
 */
class DALI_UI_API ProgressViewImpl : public ViewImpl
{
public:
  ProgressView::ValueChangedSignalType&       ValueChangedSignal();
  ProgressView::DeterminateChangedSignalType& DeterminateChangedSignal();

  void    SetValue(float value);
  float   GetValue() const;
  void    SetRange(float minimumValue, float maximumValue);
  void    GetRange(float& minimumValue, float& maximumValue) const;
  void    SetValueStepCount(int32_t count);
  int32_t GetValueStepCount() const;
  void    SetDeterminate(bool determinate);
  bool    IsDeterminate() const;

protected:
  ProgressViewImpl(float minimumValue, float maximumValue);
  ~ProgressViewImpl() override;

  void OnInitialize() override;
  bool OnAccessibilityRequestValue(Dali::String& value) override;

  virtual void OnValueRatioChanged(float valueRatio)  = 0;
  virtual void OnDeterminateChanged(bool determinate) = 0;

  float GetValueRatio() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

inline ProgressViewImpl& GetImpl(ProgressView& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<ProgressViewImpl&>(handle);
}

inline const ProgressViewImpl& GetImpl(const ProgressView& view)
{
  DALI_ASSERT_ALWAYS(view);
  const Dali::RefObject& handle = view.GetImplementation();
  return static_cast<const ProgressViewImpl&>(handle);
}

} // namespace Extension
} // namespace Ui
} // namespace Dali

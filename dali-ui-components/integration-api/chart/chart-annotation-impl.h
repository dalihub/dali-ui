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

#include <dali-ui-components/public-api/chart/chart-annotation.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
/**
 * @brief Owns annotation content and notifies every chart sharing the annotation.
 */
class ChartAnnotation : public BaseObject
{
public:
  /**
   * @brief Creates annotation state with an immutable content type.
   *
   * @param[in] type The content type
   */
  explicit ChartAnnotation(Ui::ChartAnnotation::Type type)
  : mType(type)
  {
  }

  ~ChartAnnotation() override                                 = default;
  ChartAnnotation(const ChartAnnotation&)                     = delete;
  ChartAnnotation(ChartAnnotation&&)                          = delete;
  ChartAnnotation&          operator=(ChartAnnotation&&)      = delete;
  ChartAnnotation&          operator=(const ChartAnnotation&) = delete;
  Dali::String              mText;
  Dali::String              mImageUrl;
  UiColor                   mColor{0.22f, 0.48f, 1.0f, 1.0f};
  Ui::ChartSeries           mSeries;
  Signal<void()>            mChanged;
  Vector2                   mPosition{0.0f, 0.0f};
  Vector2                   mOffset{0.0f, 0.0f};
  Vector2                   mSize{12.0f, 12.0f};
  Ui::ChartAnnotation::Type mType;
  uint32_t                  mPointIndex{0u};
  float                     mTextMaximumWidth{0.0f};
  Text::OverflowMode        mTextOverflowMode{Text::OverflowMode::CLIP};
};
} // namespace Integration

inline Integration::ChartAnnotation& GetImplementation(Ui::ChartAnnotation& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  return static_cast<Integration::ChartAnnotation&>(handle.GetBaseObject());
}

inline const Integration::ChartAnnotation& GetImplementation(const Ui::ChartAnnotation& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  return static_cast<const Integration::ChartAnnotation&>(handle.GetBaseObject());
}
} // namespace Ui
} // namespace DALI_NAMESPACE

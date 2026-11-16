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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/span-impl.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Implementation for immutable foreground gradient span payloads.
 */
class GradientSpan : public Span
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL GradientSpan* NewForBaseHandle(const Dali::Ui::Gradient::Base& gradient, Dali::Ui::Text::GradientSpan::BoundsMode boundsMode)
  {
    return new GradientSpan(gradient, std::move(boundsMode));
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<GradientSpan> New(const Dali::Ui::Gradient::Base& gradient, Dali::Ui::Text::GradientSpan::BoundsMode boundsMode)
  {
    return IntrusivePtr<GradientSpan>(new GradientSpan(gradient, std::move(boundsMode)));
  }

  const Dali::Ui::Gradient::Base&          GetGradient() const;
  Dali::Ui::Text::GradientSpan::BoundsMode GetBoundsMode() const;

protected:
  GradientSpan(const Dali::Ui::Gradient::Base& gradient, Dali::Ui::Text::GradientSpan::BoundsMode boundsMode);
  ~GradientSpan() override;

private:
  Dali::Ui::Gradient::Base                 mGradient;
  Dali::Ui::Text::GradientSpan::BoundsMode mBoundsMode;
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

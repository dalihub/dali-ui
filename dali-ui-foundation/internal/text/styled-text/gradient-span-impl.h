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

namespace Dali
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
  GradientSpan(const Dali::Ui::Gradient::Base& gradient, Dali::Ui::Text::GradientSpan::BoundsMode boundsMode);
  ~GradientSpan() override;

  const Dali::Ui::Gradient::Base&          GetGradient() const;
  Dali::Ui::Text::GradientSpan::BoundsMode GetBoundsMode() const;

private:
  Dali::Ui::Gradient::Base                 mGradient;
  Dali::Ui::Text::GradientSpan::BoundsMode mBoundsMode;
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

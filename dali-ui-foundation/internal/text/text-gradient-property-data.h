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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

struct TextGradientPropertyData
{
  Gradient::Base textGradient;
  Gradient::Base textGradientOverlay;

  Ui::Text::GradientBoundsMode  textGradientBoundsMode{Ui::Text::GradientBoundsMode::CONTENT_BOUND};
  Ui::Text::GradientBoundsMode  textGradientOverlayBoundsMode{Ui::Text::GradientBoundsMode::CONTENT_BOUND};
  Ui::Text::GradientOverlayMode textGradientOverlayMode{Ui::Text::GradientOverlayMode::SRC_OVER};

  Property::Index gradientAnimOffsetIndex{Property::INVALID_INDEX};
  Property::Index gradientOverlayAnimOffsetIndex{Property::INVALID_INDEX};

  int gradientAnimCount{0};
  int gradientOverlayAnimCount{0};
};

using TextGradientPropertyDataPtr = std::unique_ptr<TextGradientPropertyData>;

inline TextGradientPropertyData* GetTextGradientPropertyData(TextGradientPropertyDataPtr& data)
{
  return data.get();
}

inline const TextGradientPropertyData* GetTextGradientPropertyData(const TextGradientPropertyDataPtr& data)
{
  return data.get();
}

TextGradientPropertyData& GetOrCreateTextGradientPropertyData(TextGradientPropertyDataPtr& data);

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

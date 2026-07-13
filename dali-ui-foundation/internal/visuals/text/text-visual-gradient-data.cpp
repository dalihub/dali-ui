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

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/text/text-visual-gradient-data.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

TextVisualGradientData::TextVisualGradientData()
: mTextGradientBoundsMode(Ui::Text::GradientBoundsMode::CONTENT_BOUND),
  mTextGradientOverlayBoundsMode(Ui::Text::GradientBoundsMode::CONTENT_BOUND),
  mTextGradientOverlayMode(Ui::Text::GradientOverlayMode::SRC_OVER),
  mLastGradientCoordSize(Vector2::ZERO),
  mLastGradientBounds(Vector4::ZERO),
  mLastGradientOverlayCoordSize(Vector2::ZERO),
  mLastGradientOverlayBounds(Vector4::ZERO),
  mGradientAnimOffsetIndex(Property::INVALID_INDEX),
  mGradientOverlayAnimOffsetIndex(Property::INVALID_INDEX),
  mGradientAnimApplyAlways(false),
  mGradientOverlayAnimApplyAlways(false),
  mHasGradientContext(false),
  mHasGradientOverlayContext(false)
{
}

TextVisualGradientData& GetOrCreateTextVisualGradientData(TextVisualGradientDataPtr& data)
{
  if(!data)
  {
    data = std::make_unique<TextVisualGradientData>();
  }

  return *data;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

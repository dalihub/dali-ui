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
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/property.h>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

struct TextVisualGradientData
{
  TextVisualGradientData();

  Ui::Text::Internal::Gradient::Style mTextGradientStyle;
  Ui::Text::GradientBoundsMode        mTextGradientBoundsMode;
  Ui::Text::Internal::Gradient::Style mTextGradientOverlayStyle;
  Ui::Text::GradientBoundsMode        mTextGradientOverlayBoundsMode;
  Ui::Text::GradientOverlayMode       mTextGradientOverlayMode;

  PixelData      mTextGradientMaskPixelData;
  VisualRenderer mGradientRenderer;
  Vector2        mLastGradientCoordSize;
  Vector4        mLastGradientBounds;
  VisualRenderer mGradientOverlayRenderer;
  Vector2        mLastGradientOverlayCoordSize;
  Vector4        mLastGradientOverlayBounds;

  Property::Index mGradientAnimOffsetIndex;
  Property::Index mGradientOverlayAnimOffsetIndex;

  std::vector<Constraint> mGradientAnimConstraints;
  std::vector<Constraint> mGradientOverlayAnimConstraints;

  bool mGradientAnimApplyAlways : 1;
  bool mGradientOverlayAnimApplyAlways : 1;
  bool mHasGradientContext : 1;
  bool mHasGradientOverlayContext : 1;
};

using TextVisualGradientDataPtr = std::unique_ptr<TextVisualGradientData>;

inline TextVisualGradientData* GetTextVisualGradientData(TextVisualGradientDataPtr& data)
{
  return data.get();
}

inline const TextVisualGradientData* GetTextVisualGradientData(const TextVisualGradientDataPtr& data)
{
  return data.get();
}

TextVisualGradientData& GetOrCreateTextVisualGradientData(TextVisualGradientDataPtr& data);

} // namespace Internal
} // namespace Ui
} // namespace Dali

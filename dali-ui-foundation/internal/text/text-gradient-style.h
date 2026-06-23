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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_STYLE_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_STYLE_H

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

struct TextGradientStop
{
  float   offset{0.0f};
  Vector4 color{};
};

struct TextGradientStyle
{
  bool enabled{false};

  Dali::Ui::Gradient::Type         type{Dali::Ui::Gradient::Type::NONE};
  Dali::Ui::Gradient::Units        units{Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX};
  Dali::Ui::Gradient::SpreadMethod spreadMethod{Dali::Ui::Gradient::SpreadMethod::PAD};
  float                            startOffset{0.0f};

  Vector2 linearStart{Vector2::ZERO};
  Vector2 linearEnd{Vector2::ONE};

  Vector2 radialCenter{Vector2::ZERO};
  float   radialRadius{0.0f};

  Vector2 conicCenter{Vector2::ZERO};
  Radian  conicStartAngle{0.0f};

  Dali::Vector<TextGradientStop> stops;
};

/**
 * @brief Creates an immutable text rendering snapshot from a public authored gradient value.
 *
 * The snapshot owns only CPU-side values. It does not create GPU resources,
 * lookup textures, or Internal::Gradient objects.
 *
 * @param[in] gradient The public authored gradient value.
 * @return A disabled style for non-renderable gradients, otherwise a deep-copied snapshot.
 */
TextGradientStyle CreateTextGradientStyle(const Dali::Ui::Gradient::Base& gradient);

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_STYLE_H

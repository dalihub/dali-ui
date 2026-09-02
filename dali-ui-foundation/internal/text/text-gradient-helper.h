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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_HELPER_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_HELPER_H

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-style.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
namespace Gradient
{

bool IsRenderable(const Dali::Ui::Gradient::Base& gradient);

bool IsRenderable(const Style& style);

/**
 * @brief Compares two immutable gradient rendering snapshots by value.
 */
bool EqualStyle(const Style& lhs, const Style& rhs);

Dali::WrapMode::Type GetWrapMode(Dali::Ui::Gradient::SpreadMethod spread);

struct RenderData
{
  bool                     enabled{false};
  Dali::Ui::Gradient::Type type{Dali::Ui::Gradient::Type::NONE};

  Vector2 startPosition{Vector2::ZERO};
  Vector2 endPosition{Vector2::ONE};

  Vector2 radialCenter{Vector2::ZERO};
  Vector2 radialScale{Vector2::ZERO};

  Vector2 conicCenter{Vector2::ZERO};
  Vector2 conicScale{Vector2::ONE};
  float   conicStartAngle{0.0f};

  float   startOffset{0.0f};
  Vector4 bounds{0.0f, 0.0f, 1.0f, 1.0f};
};

RenderData ResolveRenderData(const Style&   style,
                             const Vector4& bounds,
                             const Vector2& coordinateSize);

Dali::Texture CreateLookupTexture(const Style& style);

void SetLookupTexture(TextureSet& textureSet, uint32_t textureSetIndex, const Style& style);

void AddLookupTexture(TextureSet& textureSet, uint32_t& textureSetIndex, const Style& style);

template<typename RendererType>
Property::Index SetRendererProperty(RendererType renderer, const char* name, const Property::Value& value)
{
  Property::Index index = renderer.GetPropertyIndex(name);
  if(index == Property::INVALID_INDEX)
  {
    index = renderer.RegisterProperty(name, value);
  }
  else
  {
    renderer.SetProperty(index, value);
  }
  return index;
}

} // namespace Gradient
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_HELPER_H

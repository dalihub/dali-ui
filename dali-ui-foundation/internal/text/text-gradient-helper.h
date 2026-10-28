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
#include <dali-ui-foundation/internal/visuals/gradient/gradient-texture-cache.h>

namespace DALI_NAMESPACE
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

/**
 * @brief Acquires a synchronous shared lookup texture for a text gradient.
 * @param[in] style The text gradient rendering style
 * @param[in,out] textureId Optional owned ID to replace with the acquired reference
 * @return The lookup texture, or an empty handle if there are fewer than two stops
 * @note If textureId is omitted, the acquired reference is queued for release immediately.
 */
Dali::Texture AcquireLookupTexture(const Style& style, Dali::Ui::Internal::GradientTextureCache::Id* textureId = nullptr);

/**
 * @brief Requests release of an owned lookup texture and resets its ID.
 * @param[in,out] textureId The owned ID to release and replace with INVALID_ID
 */
void RequestReleaseLookupTexture(Dali::Ui::Internal::GradientTextureCache::Id& textureId);

/**
 * @brief Retains a lookup texture when prepared content is handed to a live scroller.
 * @param[in] textureId The lookup texture ID to retain
 */
void ReferenceLookupTexture(Dali::Ui::Internal::GradientTextureCache::Id textureId);

/**
 * @brief Assigns a shared lookup texture and gradient sampler to a texture slot.
 * @param[in,out] textureSet The texture set receiving the lookup texture
 * @param[in] textureSetIndex The texture slot to update
 * @param[in] style The text gradient rendering style
 * @param[in,out] textureId Optional owned lookup ID to replace
 */
void SetLookupTexture(TextureSet& textureSet, uint32_t textureSetIndex, const Style& style, Dali::Ui::Internal::GradientTextureCache::Id* textureId = nullptr);

/**
 * @brief Assigns a lookup texture and advances the next texture slot index.
 * @param[in,out] textureSet The texture set receiving the lookup texture
 * @param[in,out] textureSetIndex The slot to update, incremented after assignment
 * @param[in] style The text gradient rendering style
 * @param[in,out] textureId Optional owned lookup ID to replace
 */
void AddLookupTexture(TextureSet& textureSet, uint32_t& textureSetIndex, const Style& style, Dali::Ui::Internal::GradientTextureCache::Id* textureId = nullptr);

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
} //namespace DALI_NAMESPACE

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_HELPER_H

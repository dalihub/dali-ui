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
namespace TextGradient
{

bool IsRenderable(const Gradient::Base& gradient);

bool IsLinearRenderable(const Gradient::Base& gradient);

Dali::WrapMode::Type GetWrapMode(Gradient::SpreadMethod spread);

Dali::Texture CreateLookupTexture(const TextGradientStyle& style);

void SetLookupTexture(TextureSet& textureSet, uint32_t textureSetIndex, const TextGradientStyle& style);

void AddLookupTexture(TextureSet& textureSet, uint32_t& textureSetIndex, const TextGradientStyle& style);

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

} // namespace TextGradient
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_HELPER_H

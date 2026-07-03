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
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/visuals/gradient/linear-gradient.h>

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

bool IsRenderable(const Dali::Ui::Gradient::Base& gradient)
{
  if(gradient.GetType() == Dali::Ui::Gradient::Type::NONE)
  {
    return false;
  }

  switch(gradient.GetType())
  {
    case Dali::Ui::Gradient::Type::LINEAR:
    case Dali::Ui::Gradient::Type::RADIAL:
    case Dali::Ui::Gradient::Type::CONIC:
    {
      return gradient.GetStopNodes().Count() >= 2u;
    }

    case Dali::Ui::Gradient::Type::NONE:
    default:
    {
      return false;
    }
  }
}

bool IsRenderable(const Style& style)
{
  if(!style.enabled || style.stops.Count() < 2u)
  {
    return false;
  }

  switch(style.type)
  {
    case Dali::Ui::Gradient::Type::LINEAR:
    {
      const Vector2 gradientVector = style.linearEnd - style.linearStart;
      return gradientVector.LengthSquared() > Math::MACHINE_EPSILON_1000;
    }

    case Dali::Ui::Gradient::Type::RADIAL:
    {
      return std::fabs(style.radialRadius) > Math::MACHINE_EPSILON_1000;
    }

    case Dali::Ui::Gradient::Type::CONIC:
    {
      return true;
    }

    case Dali::Ui::Gradient::Type::NONE:
    default:
    {
      return false;
    }
  }
}

Dali::WrapMode::Type GetWrapMode(Dali::Ui::Gradient::SpreadMethod spread)
{
  switch(spread)
  {
    case Dali::Ui::Gradient::SpreadMethod::REPEAT:
    {
      return Dali::WrapMode::REPEAT;
    }
    case Dali::Ui::Gradient::SpreadMethod::REFLECT:
    {
      return Dali::WrapMode::MIRRORED_REPEAT;
    }
    case Dali::Ui::Gradient::SpreadMethod::PAD:
    default:
    {
      return Dali::WrapMode::CLAMP_TO_EDGE;
    }
  }
}

RenderData ResolveRenderData(const Style&   style,
                             const Vector4& bounds,
                             const Vector2& coordinateSize)
{
  RenderData data;
  if(!IsRenderable(style))
  {
    return data;
  }

  data.enabled     = true;
  data.type        = style.type;
  data.startOffset = style.startOffset;
  data.bounds      = bounds;

  data.startPosition =
    Text::Internal::ResolveGradientPosition(style.units, style.linearStart, bounds, coordinateSize);
  data.endPosition =
    Text::Internal::ResolveGradientPosition(style.units, style.linearEnd, bounds, coordinateSize);

  if(style.type == Dali::Ui::Gradient::Type::RADIAL)
  {
    data.radialCenter =
      Text::Internal::ResolveGradientPosition(style.units, style.radialCenter, bounds, coordinateSize);
    data.radialScale =
      Text::Internal::ResolveRadialGradientScale(style.units, style.radialRadius, bounds, coordinateSize);
  }
  else if(style.type == Dali::Ui::Gradient::Type::CONIC)
  {
    data.conicCenter =
      Text::Internal::ResolveGradientPosition(style.units, style.conicCenter, bounds, coordinateSize);
    data.conicScale =
      Text::Internal::ResolveConicGradientScale(style.units, bounds, coordinateSize);
    data.conicStartAngle = style.conicStartAngle.radian;
  }

  return data;
}

Dali::Texture CreateLookupTexture(const Style& style)
{
  if(style.stops.Count() < 2u)
  {
    return Dali::Texture();
  }

  Dali::IntrusivePtr<Dali::Ui::Internal::LinearGradient> gradient =
    new Dali::Ui::Internal::LinearGradient(style.linearStart, style.linearEnd, nullptr);
  gradient->SetUnits(style.units);
  gradient->SetSpreadMethod(style.spreadMethod);

  Dali::Vector<float>   offsets;
  Dali::Vector<Vector4> colors;
  offsets.Reserve(style.stops.Count());
  colors.Reserve(style.stops.Count());

  for(auto i = 0u; i < style.stops.Count(); ++i)
  {
    offsets.PushBack(style.stops[i].offset);
    colors.PushBack(style.stops[i].color);
  }

  gradient->ApplyStops(offsets, colors);
  return gradient->GenerateLookupTexture();
}

void SetLookupTexture(TextureSet& textureSet, uint32_t textureSetIndex, const Style& style)
{
  Texture lookupTexture   = CreateLookupTexture(style);
  Sampler gradientSampler = Sampler::New();
  gradientSampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  gradientSampler.SetWrapMode(GetWrapMode(style.spreadMethod), Dali::WrapMode::CLAMP_TO_EDGE);

  textureSet.SetTexture(textureSetIndex, lookupTexture);
  textureSet.SetSampler(textureSetIndex, gradientSampler);
}

void AddLookupTexture(TextureSet& textureSet, uint32_t& textureSetIndex, const Style& style)
{
  SetLookupTexture(textureSet, textureSetIndex, style);
  ++textureSetIndex;
}

} // namespace Gradient
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

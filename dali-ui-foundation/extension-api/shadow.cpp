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
#include <dali-ui-foundation/extension-api/shadow.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace Shadow
{

ColorVisual CreateVisual(const Ui::Shadow& shadow)
{
  if(shadow == Ui::Shadow::None())
  {
    return ColorVisual();
  }

  ColorVisual visual = ColorVisual::New();
  visual.SetColor(shadow.GetColor());
  visual.SetBlurRadius(shadow.GetBlurRadius());
  visual.SetCutoutPolicy(shadow.GetCutoutPolicy());
  visual.SetOffsetX(shadow.GetOffset().x);
  visual.SetOffsetY(shadow.GetOffset().y);
  visual.SetExtraWidth(shadow.GetExtents().width);
  visual.SetExtraHeight(shadow.GetExtents().height);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  return visual;
}

Ui::Shadow CreateShadow(const Property::Map& map)
{
  if(map.Empty())
  {
    return Ui::Shadow::None();
  }

  Ui::Shadow shadow;

  const Property::Value* colorValue = map.Find(VisualBasePropertyIndex::MIX_COLOR);
  if(colorValue)
  {
    Vector4 color;
    if(colorValue->Get(color))
    {
      shadow.SetColor(UiColor(color));
    }
  }

  const Property::Value* blurRadiusValue = map.Find(ColorVisualPropertyIndex::BLUR_RADIUS);
  if(blurRadiusValue)
  {
    float blurRadius = 0.0f;
    if(blurRadiusValue->Get(blurRadius))
    {
      shadow.SetBlurRadius(std::max(0.0f, blurRadius));
    }
  }

  const Property::Value* cutoutPolicyValue = map.Find(ColorVisualPropertyIndex::CUTOUT_POLICY);
  if(cutoutPolicyValue)
  {
    int cutoutPolicy = static_cast<int>(CutoutPolicy::NONE);
    if(cutoutPolicyValue->Get(cutoutPolicy))
    {
      shadow.SetCutoutPolicy(static_cast<CutoutPolicy>(cutoutPolicy));
    }
  }

  const Property::Value* transformValue = map.Find(VisualBasePropertyIndex::TRANSFORM);
  const Property::Map*   transformMap   = transformValue ? transformValue->GetMap() : nullptr;
  if(transformMap)
  {
    const Property::Value* offsetValue = transformMap->Find(Visual::Transform::Property::OFFSET);
    if(offsetValue)
    {
      Vector2 offset;
      if(offsetValue->Get(offset))
      {
        shadow.SetOffset(offset);
      }
    }

    const Property::Value* extraSizeValue = transformMap->Find(Dali::Ui::Integration::Visual::Transform::Property::EXTRA_SIZE);
    if(extraSizeValue)
    {
      Vector2 extents;
      if(extraSizeValue->Get(extents))
      {
        shadow.SetExtents(extents);
      }
    }
  }

  return shadow;
}

Property::Map CreatePropertyMap(const Ui::Shadow& shadow)
{
  if(shadow == Ui::Shadow::None())
  {
    return Property::Map();
  }

  Property::Map transform;
  transform.Add(Visual::Transform::Property::OFFSET, shadow.GetOffset())
    .Add(Visual::Transform::Property::OFFSET_POLICY,
         Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE))
    .Add(Visual::Transform::Property::SIZE, Vector2::ONE)
    .Add(Visual::Transform::Property::SIZE_POLICY,
         Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE))
    .Add(Dali::Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, shadow.GetExtents());

  Property::Map map;
  map.Add(VisualBasePropertyIndex::TYPE, VisualType::COLOR)
    .Add(VisualBasePropertyIndex::MIX_COLOR, shadow.GetColor().GetRgba())
    .Add(ColorVisualPropertyIndex::BLUR_RADIUS, shadow.GetBlurRadius())
    .Add(ColorVisualPropertyIndex::CUTOUT_POLICY, static_cast<int>(shadow.GetCutoutPolicy()))
    .Add(VisualBasePropertyIndex::TRANSFORM, transform);

  return map;
}

} // namespace Shadow
} // namespace Extension
} // namespace Ui
} // namespace Dali

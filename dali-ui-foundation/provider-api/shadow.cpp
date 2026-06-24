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
#include <dali-ui-foundation/provider-api/shadow.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Provider
{
namespace Shadow
{

ColorVisual CreateVisual(const Ui::Shadow& shadow)
{
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

Property::Map CreatePropertyMap(const Ui::Shadow& shadow)
{
  Property::Map transform;
  transform.Add(Visual::Transform::Property::OFFSET, shadow.GetOffset())
    .Add(Visual::Transform::Property::OFFSET_POLICY,
         Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE))
    .Add(Visual::Transform::Property::SIZE, Vector2::ONE)
    .Add(Visual::Transform::Property::SIZE_POLICY,
         Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE))
    .Add(DevelVisual::Transform::Property::EXTRA_SIZE, shadow.GetExtents());

  Property::Map map;
  map.Add(VisualBasePropertyIndex::TYPE, VisualType::COLOR)
    .Add(VisualBasePropertyIndex::MIX_COLOR, shadow.GetColor().GetRgba())
    .Add(ColorVisualPropertyIndex::BLUR_RADIUS, shadow.GetBlurRadius())
    .Add(ColorVisualPropertyIndex::CUTOUT_POLICY, static_cast<int>(shadow.GetCutoutPolicy()))
    .Add(VisualBasePropertyIndex::TRANSFORM, transform);

  return map;
}

} // namespace Shadow
} // namespace Provider
} // namespace Ui
} // namespace Dali

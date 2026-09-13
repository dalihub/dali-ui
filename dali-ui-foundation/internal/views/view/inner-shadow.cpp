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
#include "inner-shadow.h"

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/color-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace InnerShadow
{
namespace
{
constexpr float INNER_SHADOW_MARGIN = 1.0f;
constexpr float BORDERLINE_OFFSET   = -1.0f;

struct VisualParameters
{
  float   shadowWidth;
  Vector2 offset;
  Vector2 extraSize;
};

VisualParameters CalculateVisualParameters(const Ui::InnerShadow& innerShadow)
{
  const Insets& insets   = innerShadow.GetInsets();
  const float   maxInset = std::max(std::max(insets.start, insets.end), std::max(insets.top, insets.bottom));

  VisualParameters parameters;
  parameters.shadowWidth = 2.0f * (std::max(maxInset, 0.0f) + innerShadow.GetBlurRadius() + INNER_SHADOW_MARGIN);
  parameters.offset      = Vector2((insets.start - insets.end) * 0.5f, (insets.top - insets.bottom) * 0.5f);
  parameters.extraSize   = Vector2(parameters.shadowWidth * 2.0f - insets.start - insets.end,
                                   parameters.shadowWidth * 2.0f - insets.top - insets.bottom);
  return parameters;
}
} // namespace

ColorVisual CreateVisual(const Ui::InnerShadow& innerShadow)
{
  if(innerShadow == Ui::InnerShadow::None())
  {
    return ColorVisual();
  }

  const VisualParameters parameters = CalculateVisualParameters(innerShadow);

  ColorVisual visual = ColorVisual::New();
  visual.SetColor(UiColor(0.0f, 0.0f, 0.0f, 0.0f));
  visual.SetBlurRadius(innerShadow.GetBlurRadius());
  visual.SetOffsetX(parameters.offset.x);
  visual.SetOffsetY(parameters.offset.y);
  visual.SetExtraWidth(parameters.extraSize.width);
  visual.SetExtraHeight(parameters.extraSize.height);
  visual.SetOrigin(VisualOrigin::CENTER);
  visual.SetPivot(VisualPivot::CENTER);
  visual.SetTransformProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetBorderlineColor(innerShadow.GetColor());
  visual.SetBorderlineWidth(parameters.shadowWidth);
  visual.SetBorderlineOffset(BORDERLINE_OFFSET);
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS);
  return visual;
}

Property::Map CreatePropertyMap(const Ui::InnerShadow& innerShadow)
{
  if(innerShadow == Ui::InnerShadow::None())
  {
    return Property::Map();
  }

  const VisualParameters parameters = CalculateVisualParameters(innerShadow);

  Property::Map transform;
  transform.Add(Dali::Ui::Integration::Visual::Transform::Property::ORIGIN, VisualOrigin::CENTER)
    .Add(Dali::Ui::Integration::Visual::Transform::Property::PIVOT, VisualPivot::CENTER);
  if(parameters.offset != Vector2::ZERO)
  {
    transform.Add(Dali::Ui::Integration::Visual::Transform::Property::OFFSET_POLICY,
                  Vector2(Dali::Ui::Integration::Visual::Transform::Policy::ABSOLUTE, Dali::Ui::Integration::Visual::Transform::Policy::ABSOLUTE))
      .Add(Dali::Ui::Integration::Visual::Transform::Property::OFFSET, parameters.offset);
  }
  if(parameters.extraSize != Vector2::ZERO)
  {
    transform.Add(Dali::Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, parameters.extraSize);
  }

  Property::Map map;
  map.Add(Dali::Ui::Integration::Visual::Property::TYPE, VisualType::COLOR)
    .Add(Dali::Ui::Integration::Visual::Property::MIX_COLOR, Color::TRANSPARENT)
    .Add(Dali::Ui::Integration::ColorVisual::Property::BLUR_RADIUS, innerShadow.GetBlurRadius())
    .Add(Dali::Ui::Integration::ColorVisual::Property::CUTOUT_POLICY, static_cast<int>(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS))
    .Add(Dali::Ui::Integration::Visual::Property::BORDERLINE_COLOR, innerShadow.GetColor().GetRgba())
    .Add(Dali::Ui::Integration::Visual::Property::BORDERLINE_WIDTH, parameters.shadowWidth)
    .Add(Dali::Ui::Integration::Visual::Property::BORDERLINE_OFFSET, BORDERLINE_OFFSET)
    .Add(Dali::Ui::Integration::Visual::Property::TRANSFORM, transform);
  return map;
}

} // namespace InnerShadow
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

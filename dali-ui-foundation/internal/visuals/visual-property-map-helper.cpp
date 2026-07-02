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
#include <dali-ui-foundation/internal/visuals/visual-property-map-helper.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

Property::Map CreateColorVisualPropertyMap(const Vector4& color)
{
  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::COLOR);
  map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, color);
  return map;
}

Property::Map CreateColorVisualPropertyMap(const UiColor& color)
{
  return CreateColorVisualPropertyMap(color.GetRgba());
}

Property::Map CreateImageVisualPropertyMap(const Dali::String& url)
{
  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::ImageVisualPropertyIndex::URL, url);
  return map;
}

Property::Map CreateGradientVisualPropertyMap(const Gradient::Base& gradient)
{
  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::GRADIENT);
  map.Insert(Ui::GradientVisualPropertyIndex::START_OFFSET, gradient.GetStartOffset());
  map.Insert(Ui::GradientVisualPropertyIndex::UNITS, gradient.GetUnits());
  map.Insert(Ui::GradientVisualPropertyIndex::SPREAD_METHOD, gradient.GetSpreadMethod());

  const Vector<Gradient::StopNode> stopNodes = gradient.GetStopNodes();

  Property::Array offsets;
  Property::Array colors;
  offsets.Reserve(stopNodes.Count());
  colors.Reserve(stopNodes.Count());

  for(auto i = 0u; i < stopNodes.Count(); ++i)
  {
    offsets.PushBack(stopNodes[i].GetOffset());
    colors.PushBack(stopNodes[i].GetColor().GetRgba());
  }

  map.Insert(Ui::GradientVisualPropertyIndex::STOP_OFFSET, offsets);
  map.Insert(Ui::GradientVisualPropertyIndex::STOP_COLOR, colors);

  switch(gradient.GetType())
  {
    case Gradient::Type::LINEAR:
    {
      Gradient::Linear linear = Gradient::Linear::DownCast(gradient);
      map.Insert(Ui::GradientVisualPropertyIndex::START_POSITION, linear.GetStartPosition());
      map.Insert(Ui::GradientVisualPropertyIndex::END_POSITION, linear.GetEndPosition());
      break;
    }

    case Gradient::Type::RADIAL:
    {
      Gradient::Radial radial = Gradient::Radial::DownCast(gradient);
      map.Insert(Ui::GradientVisualPropertyIndex::CENTER, radial.GetCenter());
      map.Insert(Ui::GradientVisualPropertyIndex::RADIUS, radial.GetRadius());
      break;
    }

    case Gradient::Type::CONIC:
    {
      Gradient::Conic conic = Gradient::Conic::DownCast(gradient);
      map.Insert(Ui::GradientVisualPropertyIndex::CENTER, conic.GetCenter());
      map.Insert(Ui::GradientVisualPropertyIndex::START_ANGLE, conic.GetStartAngle().radian);
      break;
    }

    case Gradient::Type::NONE:
    default:
    {
      break;
    }
  }

  return map;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

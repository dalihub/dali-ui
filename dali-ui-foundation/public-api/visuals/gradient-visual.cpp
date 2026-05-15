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
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
GradientVisual GradientVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::Visual::GRADIENT);
  return GradientVisual(internal.Get());
}

// =============================================================================
// Properties
// =============================================================================

float GradientVisual::GetStartOffset() const
{
  return VisualBase::GetProperty<float>(GradientVisual::Property::START_OFFSET);
}

GradientVisual& GradientVisual::SetStartOffset(float startOffset)
{
  VisualBase::SetProperty(GradientVisual::Property::START_OFFSET, startOffset);
  return *this;
}

Dali::Vector2 GradientVisual::GetStartPosition() const
{
  return VisualBase::GetProperty<Dali::Vector2>(GradientVisual::Property::START_POSITION);
}

GradientVisual& GradientVisual::SetStartPosition(const Dali::Vector2& startPosition)
{
  VisualBase::SetProperty(GradientVisual::Property::START_POSITION, startPosition);
  return *this;
}

Dali::Vector2 GradientVisual::GetEndPosition() const
{
  return VisualBase::GetProperty<Dali::Vector2>(GradientVisual::Property::END_POSITION);
}

GradientVisual& GradientVisual::SetEndPosition(const Dali::Vector2& endPosition)
{
  VisualBase::SetProperty(GradientVisual::Property::END_POSITION, endPosition);
  return *this;
}

Dali::Vector2 GradientVisual::GetCenter() const
{
  return VisualBase::GetProperty<Dali::Vector2>(GradientVisual::Property::CENTER);
}

GradientVisual& GradientVisual::SetCenter(const Dali::Vector2& center)
{
  VisualBase::SetProperty(GradientVisual::Property::CENTER, center);
  return *this;
}

float GradientVisual::GetRadius() const
{
  return VisualBase::GetProperty<float>(GradientVisual::Property::RADIUS);
}

GradientVisual& GradientVisual::SetRadius(float radius)
{
  VisualBase::SetProperty(GradientVisual::Property::RADIUS, radius);
  return *this;
}

Dali::Radian GradientVisual::GetStartAngle() const
{
  return Dali::Radian(VisualBase::GetProperty<float>(GradientVisual::Property::START_ANGLE));
}

GradientVisual& GradientVisual::SetStartAngle(Dali::Radian startAngle)
{
  VisualBase::SetProperty(GradientVisual::Property::START_ANGLE, startAngle.radian);
  return *this;
}

Dali::Vector<Ui::Gradient::StopNode> GradientVisual::GetStopNodes() const
{
  Dali::Vector<Ui::Gradient::StopNode> convertedStopNodes;

  // TODO : We need to support string color for the stop node color
  auto offsetArray = VisualBase::GetProperty<Dali::Property::Array>(GradientVisual::Property::STOP_OFFSET);
  auto colorArray  = VisualBase::GetProperty<Dali::Property::Array>(GradientVisual::Property::STOP_COLOR);

  const uint32_t nodesCount = std::min(offsetArray.Count(), colorArray.Count());
  convertedStopNodes.Reserve(nodesCount);

  for(uint32_t i = 0; i < nodesCount; ++i)
  {
    float   offset = offsetArray[i].Get<float>();
    Vector4 color  = colorArray[i].Get<Dali::Vector4>();

    convertedStopNodes.PushBack(Ui::Gradient::StopNode(offset, color));
  }

  return convertedStopNodes;
}

GradientVisual& GradientVisual::SetStopNodes(const Dali::Vector<Ui::Gradient::StopNode>& stopNodes)
{
  // TODO : We need to binding for the stop node color
  Dali::Property::Array offsetArray;
  Dali::Property::Array colorArray;

  offsetArray.Reserve(stopNodes.Count());
  colorArray.Reserve(stopNodes.Count());
  for(uint32_t i = 0; i < stopNodes.Count(); ++i)
  {
    offsetArray.PushBack(stopNodes[i].GetOffset());
    colorArray.PushBack(stopNodes[i].GetColor().GetRgba());
  }
  VisualBase::SetProperty(GradientVisual::Property::STOP_OFFSET, offsetArray);
  VisualBase::SetProperty(GradientVisual::Property::STOP_COLOR, colorArray);
  return *this;
}

Ui::Gradient::Units GradientVisual::GetUnits() const
{
  return VisualBase::GetProperty<Ui::Gradient::Units>(GradientVisual::Property::UNITS);
}

GradientVisual& GradientVisual::SetUnits(Ui::Gradient::Units gradientUnits)
{
  VisualBase::SetProperty(GradientVisual::Property::UNITS, gradientUnits);
  return *this;
}

Ui::Gradient::SpreadMethod GradientVisual::GetSpreadMethod() const
{
  return VisualBase::GetProperty<Ui::Gradient::SpreadMethod>(GradientVisual::Property::SPREAD_METHOD);
}

GradientVisual& GradientVisual::SetSpreadMethod(Ui::Gradient::SpreadMethod spreadMethod)
{
  VisualBase::SetProperty(GradientVisual::Property::SPREAD_METHOD, spreadMethod);
  return *this;
}

// =============================================================================
// Internal API
// =============================================================================

GradientVisual::GradientVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali

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
#include <dali-ui-foundation/integration-api/visuals/gradient-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
GradientVisual GradientVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::GRADIENT);
  return GradientVisual(internal.Get());
}

GradientVisual GradientVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::GRADIENT)
  {
    return GradientVisual(visualBaseImpl);
  }
  return GradientVisual();
}

// =============================================================================
// Properties
// =============================================================================

void GradientVisual::SetLinearGradient(const Dali::Vector2& startPosition, const Dali::Vector2& endPosition)
{
  // Remove other caches first if exist
  auto& visualBaseImpl = GetImplementation(*this);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::CENTER);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::RADIUS);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE);

  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_POSITION, startPosition);
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::END_POSITION, endPosition);
}

void GradientVisual::SetRadialGradient(const Dali::Vector2& center, float radius)
{
  // Remove other caches first if exist
  auto& visualBaseImpl = GetImplementation(*this);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::START_POSITION);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::END_POSITION);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE);

  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::CENTER, center);
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::RADIUS, radius);
}

void GradientVisual::SetConicGradient(const Dali::Vector2& center, Dali::Radian startAngle)
{
  // Remove other caches first if exist
  auto& visualBaseImpl = GetImplementation(*this);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::START_POSITION);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::END_POSITION);
  visualBaseImpl.RemoveCache(Dali::Ui::Integration::GradientVisual::Property::RADIUS);

  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::CENTER, center);
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE, startAngle.radian);
}

float GradientVisual::GetStartOffset() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET);
}

void GradientVisual::SetStartOffset(float startOffset)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET, startOffset);
}

Dali::Vector2 GradientVisual::GetStartPosition() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::START_POSITION);
}

Dali::Vector2 GradientVisual::GetEndPosition() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::END_POSITION);
}

Dali::Vector2 GradientVisual::GetCenter() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::CENTER);
}

float GradientVisual::GetRadius() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::RADIUS);
}

Dali::Radian GradientVisual::GetStartAngle() const
{
  return Dali::Radian(GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE));
}

Dali::Vector<Ui::Gradient::StopNode> GradientVisual::GetStopNodes() const
{
  Dali::Vector<Ui::Gradient::StopNode> convertedStopNodes;

  // TODO : We need to support string color for the stop node color
  auto offsetArray = GetImplementation(*this).GetProperty<Dali::Property::Array>(Dali::Ui::Integration::GradientVisual::Property::STOP_OFFSET);
  auto colorArray  = GetImplementation(*this).GetProperty<Dali::Property::Array>(Dali::Ui::Integration::GradientVisual::Property::STOP_COLOR);

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

void GradientVisual::SetStopNodes(const Dali::Vector<Ui::Gradient::StopNode>& stopNodes)
{
  // TODO : We need to binding for the stop node color
  Dali::Property::Array offsetArray;
  Dali::Property::Array colorArray;

  offsetArray.Reserve(static_cast<Dali::Property::Array::SizeType>(stopNodes.Count()));
  colorArray.Reserve(static_cast<Dali::Property::Array::SizeType>(stopNodes.Count()));
  for(uint32_t i = 0; i < stopNodes.Count(); ++i)
  {
    offsetArray.PushBack(stopNodes[i].GetOffset());
    colorArray.PushBack(stopNodes[i].GetColor().GetRgba());
  }
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::STOP_OFFSET, offsetArray);
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::STOP_COLOR, colorArray);
}

Ui::Gradient::Units GradientVisual::GetUnits() const
{
  return GetImplementation(*this).GetProperty<Ui::Gradient::Units>(Dali::Ui::Integration::GradientVisual::Property::UNITS);
}

void GradientVisual::SetUnits(Ui::Gradient::Units gradientUnits)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::UNITS, gradientUnits);
}

Ui::Gradient::SpreadMethod GradientVisual::GetSpreadMethod() const
{
  return GetImplementation(*this).GetProperty<Ui::Gradient::SpreadMethod>(Dali::Ui::Integration::GradientVisual::Property::SPREAD_METHOD);
}

void GradientVisual::SetSpreadMethod(Ui::Gradient::SpreadMethod spreadMethod)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::GradientVisual::Property::SPREAD_METHOD, spreadMethod);
}

// =============================================================================
// Internal API
// =============================================================================

GradientVisual::GradientVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} //namespace DALI_NAMESPACE

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/object/property-array.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/gradient-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace
{
/**
 * @brief The geometry properties of every gradient type.
 * Only the ones belonging to the current type are kept, so the type can be resolved
 * from the properties that are present.
 */
constexpr Dali::Property::Index GEOMETRY_PROPERTIES[] =
  {
    Dali::Ui::Integration::GradientVisual::Property::START_POSITION,
    Dali::Ui::Integration::GradientVisual::Property::END_POSITION,
    Dali::Ui::Integration::GradientVisual::Property::CENTER,
    Dali::Ui::Integration::GradientVisual::Property::RADIUS,
    Dali::Ui::Integration::GradientVisual::Property::START_ANGLE,
};

} // unnamed namespace

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

void GradientVisual::SetGradient(const Gradient::Base& gradient)
{
  auto& visualBaseImpl = GetImplementation(*this);

  if(gradient.GetType() == Gradient::Type::NONE)
  {
    // A gradient visual has nothing to fall back to, unlike a view background or a text
    // gradient. Keep whatever is set instead of leaving the visual in an undefined state.
    DALI_LOG_ERROR("GradientVisual::SetGradient() ignored. Gradient::Type::NONE has no geometry to render.\n");
    return;
  }

  // Discard the geometry of the previous type first, so only the geometry of the
  // type being set remains.
  for(auto index : GEOMETRY_PROPERTIES)
  {
    visualBaseImpl.RemoveCache(index);
  }

  switch(gradient.GetType())
  {
    case Gradient::Type::LINEAR:
    {
      Gradient::Linear linear = Gradient::Linear::DownCast(gradient);
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_POSITION, linear.GetStartPosition());
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::END_POSITION, linear.GetEndPosition());
      break;
    }

    case Gradient::Type::RADIAL:
    {
      Gradient::Radial radial = Gradient::Radial::DownCast(gradient);
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::CENTER, radial.GetCenter());
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::RADIUS, radial.GetRadius());
      break;
    }

    case Gradient::Type::CONIC:
    {
      Gradient::Conic conic = Gradient::Conic::DownCast(gradient);
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::CENTER, conic.GetCenter());
      visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE, conic.GetStartAngle().radian);
      break;
    }

    case Gradient::Type::NONE:
    default:
    {
      // Already returned above.
      break;
    }
  }

  // TODO : We need to binding for the stop node color
  const Dali::Vector<Gradient::StopNode> stopNodes = gradient.GetStopNodes();

  Dali::Property::Array offsetArray;
  Dali::Property::Array colorArray;

  offsetArray.Reserve(static_cast<Dali::Property::Array::SizeType>(stopNodes.Count()));
  colorArray.Reserve(static_cast<Dali::Property::Array::SizeType>(stopNodes.Count()));
  for(uint32_t i = 0; i < stopNodes.Count(); ++i)
  {
    offsetArray.PushBack(stopNodes[i].GetOffset());
    colorArray.PushBack(stopNodes[i].GetColor().GetRgba());
  }

  visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::STOP_OFFSET, offsetArray);
  visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::STOP_COLOR, colorArray);
  visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::UNITS, gradient.GetUnits());
  visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::SPREAD_METHOD, gradient.GetSpreadMethod());
  visualBaseImpl.SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET, gradient.GetStartOffset());
}

Gradient::Base GradientVisual::GetGradient() const
{
  const auto& visualBaseImpl = GetImplementation(*this);

  // Resolve the geometry first. HasCachedProperty() never queries the visual, so the
  // geometry of a previous type cannot come back while the update is still pending, and
  // reading the common properties below cannot disturb the resolved type.
  // Keep the same precedence as the internal visual, which prefers RADIUS over
  // START_ANGLE over START/END_POSITION.
  Gradient::Base gradient;

  if(visualBaseImpl.HasCachedProperty(Dali::Ui::Integration::GradientVisual::Property::RADIUS))
  {
    gradient = Gradient::Radial(visualBaseImpl.GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::CENTER),
                                visualBaseImpl.GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::RADIUS));
  }
  else if(visualBaseImpl.HasCachedProperty(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE))
  {
    gradient = Gradient::Conic(visualBaseImpl.GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::CENTER),
                               Dali::Radian(visualBaseImpl.GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE)));
  }
  else if(visualBaseImpl.HasCachedProperty(Dali::Ui::Integration::GradientVisual::Property::START_POSITION) &&
          visualBaseImpl.HasCachedProperty(Dali::Ui::Integration::GradientVisual::Property::END_POSITION))
  {
    gradient = Gradient::Linear(visualBaseImpl.GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::START_POSITION),
                                visualBaseImpl.GetProperty<Dali::Vector2>(Dali::Ui::Integration::GradientVisual::Property::END_POSITION));
  }

  // TODO : We need to support string color for the stop node color
  auto offsetArray = visualBaseImpl.GetProperty<Dali::Property::Array>(Dali::Ui::Integration::GradientVisual::Property::STOP_OFFSET);
  auto colorArray  = visualBaseImpl.GetProperty<Dali::Property::Array>(Dali::Ui::Integration::GradientVisual::Property::STOP_COLOR);

  const uint32_t nodesCount = std::min(offsetArray.Count(), colorArray.Count());

  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.Reserve(nodesCount);

  for(uint32_t i = 0; i < nodesCount; ++i)
  {
    float   offset = offsetArray[i].Get<float>();
    Vector4 color  = colorArray[i].Get<Dali::Vector4>();

    stopNodes.PushBack(Gradient::StopNode(offset, color));
  }

  gradient.SetStopNodes(stopNodes);
  gradient.SetUnits(visualBaseImpl.GetProperty<Ui::Gradient::Units>(Dali::Ui::Integration::GradientVisual::Property::UNITS));
  gradient.SetSpreadMethod(visualBaseImpl.GetProperty<Ui::Gradient::SpreadMethod>(Dali::Ui::Integration::GradientVisual::Property::SPREAD_METHOD));
  gradient.SetStartOffset(visualBaseImpl.GetProperty<float>(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET));

  return gradient;
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

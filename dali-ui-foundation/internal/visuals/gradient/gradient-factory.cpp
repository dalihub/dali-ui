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
#include <dali-ui-foundation/internal/visuals/gradient/gradient-factory.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/gradient/conic-gradient.h>
#include <dali-ui-foundation/internal/visuals/gradient/linear-gradient.h>
#include <dali-ui-foundation/internal/visuals/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace GradientFactory
{
namespace
{

void ApplyCommonProperties(Gradient& internalGradient, const Dali::Ui::Gradient::Base& gradient)
{
  internalGradient.SetUnits(gradient.GetUnits());
  internalGradient.SetSpreadMethod(gradient.GetSpreadMethod());
  internalGradient.SetStartOffset(gradient.GetStartOffset());
}

void ApplyStopNodes(Gradient& internalGradient, const Dali::Vector<Dali::Ui::Gradient::StopNode>& stopNodes)
{
  Dali::Vector<float>   offsets;
  Dali::Vector<Vector4> colors;

  offsets.Reserve(stopNodes.Count());
  colors.Reserve(stopNodes.Count());

  for(auto i = 0u; i < stopNodes.Count(); ++i)
  {
    offsets.PushBack(stopNodes[i].GetOffset());
    colors.PushBack(stopNodes[i].GetColor().GetRgba());
  }

  internalGradient.ApplyStops(offsets, colors);
}

} // namespace

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

Dali::IntrusivePtr<Gradient> Create(const Dali::Ui::Gradient::Base& gradient)
{
  if(!IsRenderable(gradient))
  {
    return nullptr;
  }

  Dali::IntrusivePtr<Gradient> internalGradient;

  switch(gradient.GetType())
  {
    case Dali::Ui::Gradient::Type::LINEAR:
    {
      auto linear      = Dali::Ui::Gradient::Linear::DownCast(gradient);
      internalGradient = new LinearGradient(linear.GetStartPosition(), linear.GetEndPosition(), nullptr);
      break;
    }

    case Dali::Ui::Gradient::Type::RADIAL:
    {
      auto radial      = Dali::Ui::Gradient::Radial::DownCast(gradient);
      internalGradient = new RadialGradient(radial.GetCenter(), radial.GetRadius(), nullptr);
      break;
    }

    case Dali::Ui::Gradient::Type::CONIC:
    {
      auto conic       = Dali::Ui::Gradient::Conic::DownCast(gradient);
      internalGradient = new ConicGradient(conic.GetCenter(), conic.GetStartAngle(), nullptr);
      break;
    }

    case Dali::Ui::Gradient::Type::NONE:
    default:
    {
      return nullptr;
    }
  }

  ApplyCommonProperties(*internalGradient, gradient);
  ApplyStopNodes(*internalGradient, gradient.GetStopNodes());

  return internalGradient;
}

} // namespace GradientFactory
} // namespace Internal
} // namespace Ui
} // namespace Dali

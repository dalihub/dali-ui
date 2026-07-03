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

// FILE HEADER
#include <dali-ui-foundation/internal/text/text-gradient-style.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

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

Style CreateStyle(const Dali::Ui::Gradient::Base& gradient)
{
  Style style;

  if(gradient.GetType() == Dali::Ui::Gradient::Type::NONE)
  {
    return style;
  }

  const Dali::Vector<Dali::Ui::Gradient::StopNode> stopNodes = gradient.GetStopNodes();
  if(stopNodes.Count() < 2u)
  {
    return style;
  }

  style.enabled      = true;
  style.type         = gradient.GetType();
  style.units        = gradient.GetUnits();
  style.spreadMethod = gradient.GetSpreadMethod();
  style.startOffset  = gradient.GetStartOffset();

  style.stops.Reserve(stopNodes.Count());
  for(uint32_t index = 0u; index < stopNodes.Count(); ++index)
  {
    Stop stop;
    stop.offset = stopNodes[index].GetOffset();
    stop.color  = stopNodes[index].GetColor().GetRgba();
    style.stops.PushBack(stop);
  }

  switch(style.type)
  {
    case Dali::Ui::Gradient::Type::LINEAR:
    {
      const auto linear = Dali::Ui::Gradient::Linear::DownCast(gradient);
      style.linearStart = linear.GetStartPosition();
      style.linearEnd   = linear.GetEndPosition();
      break;
    }

    case Dali::Ui::Gradient::Type::RADIAL:
    {
      const auto radial  = Dali::Ui::Gradient::Radial::DownCast(gradient);
      style.radialCenter = radial.GetCenter();
      style.radialRadius = radial.GetRadius();
      break;
    }

    case Dali::Ui::Gradient::Type::CONIC:
    {
      const auto conic      = Dali::Ui::Gradient::Conic::DownCast(gradient);
      style.conicCenter     = conic.GetCenter();
      style.conicStartAngle = conic.GetStartAngle();
      break;
    }

    case Dali::Ui::Gradient::Type::NONE:
    default:
    {
      style = Style();
      break;
    }
  }

  return style;
}

} // namespace Gradient
} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali

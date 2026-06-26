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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-style-helper.h>

namespace Dali
{

namespace Ui
{

namespace Text
{

bool ApplyUnderlineStyle(ControllerPtr controller, const Underline& underline)
{
  bool update = false;

  const Underline::Type type       = underline.GetType();
  const float           thickness  = underline.GetThickness();
  const float           dashLength = underline.GetDashLength();
  const float           dashGap    = underline.GetDashGap();

  if(!controller->IsUnderlineEnabled())
  {
    controller->SetUnderlineEnabled(true);
    update = true;
  }

  if(!Equals(controller->GetUnderlineHeight(), thickness, Math::MACHINE_EPSILON_1000))
  {
    controller->SetUnderlineHeight(thickness);
    update = true;
  }

  if(controller->GetUnderlineType() != type)
  {
    controller->SetUnderlineType(type);
    update = true;
  }

  if(!Equals(controller->GetDashedUnderlineWidth(), dashLength, Math::MACHINE_EPSILON_1000))
  {
    controller->SetDashedUnderlineWidth(dashLength);
    update = true;
  }

  if(!Equals(controller->GetDashedUnderlineGap(), dashGap, Math::MACHINE_EPSILON_1000))
  {
    controller->SetDashedUnderlineGap(dashGap);
    update = true;
  }

  return update;
}

bool ApplyShadowStyle(ControllerPtr controller, const Shadow& shadow)
{
  bool update = false;

  const Vector2 offset     = shadow.GetOffset();
  const float   blurRadius = shadow.GetBlurRadius();

  if(!controller->IsShadowEnabled())
  {
    controller->SetShadowEnabled(true);
    update = true;
  }

  if(controller->GetShadowOffset() != offset)
  {
    controller->SetShadowOffset(offset);
    update = true;
  }

  if(!Equals(controller->GetShadowBlurRadius(), blurRadius, Math::MACHINE_EPSILON_1000))
  {
    controller->SetShadowBlurRadius(blurRadius);
    update = true;
  }

  return update;
}

bool ApplyOutlineStyle(ControllerPtr controller, const Outline& outline)
{
  bool update = false;

  const float   width      = outline.GetWidth();
  const Vector2 offset     = outline.GetOffset();
  const float   blurRadius = outline.GetBlurRadius();

  if(!controller->IsOutlineEnabled())
  {
    controller->SetOutlineEnabled(true);
    update = true;
  }

  if(controller->GetOutlineWidth() != static_cast<uint16_t>(width))
  {
    controller->SetOutlineWidth(static_cast<uint16_t>(width));
    update = true;
  }

  if(controller->GetOutlineOffset() != offset)
  {
    controller->SetOutlineOffset(offset);
    update = true;
  }

  if(!Equals(controller->GetOutlineBlurRadius(), blurRadius, Math::MACHINE_EPSILON_1000))
  {
    controller->SetOutlineBlurRadius(blurRadius);
    update = true;
  }

  return update;
}

bool ApplyLineThroughStyle(ControllerPtr controller, const LineThrough& lineThrough)
{
  bool update = false;

  const float thickness = lineThrough.GetThickness();

  if(!controller->IsStrikethroughEnabled())
  {
    controller->SetStrikethroughEnabled(true);
    update = true;
  }

  if(!Equals(controller->GetStrikethroughHeight(), thickness, Math::MACHINE_EPSILON_1000))
  {
    controller->SetStrikethroughHeight(thickness);
    update = true;
  }

  return update;
}

bool ApplyBevelStyle(ControllerPtr controller, const Bevel& bevel)
{
  bool update = false;

  const Vector2 direction = bevel.GetDirection();
  const float   intensity = bevel.GetIntensity();

  if(!controller->IsEmbossEnabled())
  {
    controller->SetEmbossEnabled(true);
    update = true;
  }

  if(controller->GetEmbossDirection() != direction)
  {
    controller->SetEmbossDirection(direction);
    update = true;
  }

  if(fabsf(controller->GetEmbossStrength() - intensity) > Math::MACHINE_EPSILON_1000)
  {
    controller->SetEmbossStrength(intensity);
    update = true;
  }

  return update;
}

} // namespace Text

} // namespace Ui

} // namespace Dali

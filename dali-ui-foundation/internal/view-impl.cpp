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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/view-impl.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

UI::View View::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Internal::View> impl = new Internal::View();

  // Pass ownership to CustomActor handle
  UI::View handle = UI::View(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

View::View()
: Toolkit::Internal::Control(Toolkit::Internal::Control::CONTROL_BEHAVIOUR_DEFAULT)
{
}

View::~View()
{
}

void View::OnInitialize()
{
  // Call base class initialization
  Toolkit::Internal::Control::OnInitialize();
  Self().SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
}

bool View::OnKeyEvent(const KeyEvent& event)
{
  // Call base class implementation
  return Toolkit::Internal::Control::OnKeyEvent(event);
}

float View::GetSizeWidth() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

void View::SetSizeWidth(float width)
{
  Self().SetProperty(Actor::Property::SIZE_WIDTH, width);
}

float View::GetSizeHeight() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

void View::SetSizeHeight(float height)
{
  Self().SetProperty(Actor::Property::SIZE_HEIGHT, height);
}

float View::GetPositionX() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_X);
}

void View::SetPositionX(float x)
{
  Self().SetProperty(Actor::Property::POSITION_X, x);
}

float View::GetPositionY() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_Y);
}

void View::SetPositionY(float y)
{
  Self().SetProperty(Actor::Property::POSITION_Y, y);
}

Vector3 View::GetParentOrigin() const
{
  return Self().GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN);
}

void View::SetParentOrigin(const Vector3& point)
{
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, point);
}

Vector3 View::GetPivotPoint() const
{
  return Self().GetProperty<Vector3>(Actor::Property::ANCHOR_POINT);
}

void View::SetPivotPoint(const Vector3& point)
{
  Self().SetProperty(Actor::Property::ANCHOR_POINT, point);
}

} // namespace Internal

} // namespace UI

} // namespace Dali

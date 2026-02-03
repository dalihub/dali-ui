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
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

UI::View ViewImpl::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Integration::ViewImpl> impl = new Integration::ViewImpl();

  // Pass ownership to CustomActor handle
  UI::View handle = UI::View(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

ViewImpl::ViewImpl()
  : Toolkit::Internal::Control(Toolkit::Internal::Control::CONTROL_BEHAVIOUR_DEFAULT)
{
}

ViewImpl::~ViewImpl()
{
}

void ViewImpl::OnInitialize()
{
  // Call base class initialization
  Toolkit::Internal::Control::OnInitialize();
  Self().SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
}

bool ViewImpl::OnKeyEvent(const KeyEvent& event)
{
  // Call base class implementation
  return Toolkit::Internal::Control::OnKeyEvent(event);
}

float ViewImpl::GetSizeWidth() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

void ViewImpl::SetSizeWidth(float width)
{
  Self().SetProperty(Actor::Property::SIZE_WIDTH, width);
}

float ViewImpl::GetSizeHeight() const
{
  return Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

void ViewImpl::SetSizeHeight(float height)
{
  Self().SetProperty(Actor::Property::SIZE_HEIGHT, height);
}

float ViewImpl::GetPositionX() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_X);
}

void ViewImpl::SetPositionX(float x)
{
  Self().SetProperty(Actor::Property::POSITION_X, x);
}

float ViewImpl::GetPositionY() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_Y);
}

void ViewImpl::SetPositionY(float y)
{
  Self().SetProperty(Actor::Property::POSITION_Y, y);
}

Vector3 ViewImpl::GetParentOrigin() const
{
  return Self().GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN);
}

void ViewImpl::SetParentOrigin(const Vector3& point)
{
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, point);
}

Vector3 ViewImpl::GetPivotPoint() const
{
  return Self().GetProperty<Vector3>(Actor::Property::ANCHOR_POINT);
}

void ViewImpl::SetPivotPoint(const Vector3& point)
{
  Self().SetProperty(Actor::Property::ANCHOR_POINT, point);
}

} // namespace Integration

} // namespace UI

} // namespace Dali

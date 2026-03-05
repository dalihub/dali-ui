/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include "push-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>
#include <dali-ui-foundation/public-api/controls/text-controls/text-label.h>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gLogButtonFilter;
#endif

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return UI::PushButton::New();
}

// Properties

DALI_TYPE_REGISTRATION_BEGIN(UI::PushButton, UI::Button, Create)

DALI_PROPERTY_REGISTRATION(UI, PushButton, "labelPadding", STRING, LABEL_PADDING)
DALI_PROPERTY_REGISTRATION(UI, PushButton, "iconPadding", STRING, ICON_PADDING)

DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

namespace
{
} // unnamed namespace

Dali::UI::PushButton PushButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<PushButton> internalPushButton = new PushButton();

  // Pass ownership to CustomActor
  Dali::UI::PushButton pushButton(*internalPushButton);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPushButton->Initialize();

  return pushButton;
}

PushButton::PushButton()
  : Button(),
    mIconAlignment(RIGHT)
{
}

PushButton::~PushButton()
{
}

void PushButton::OnInitialize()
{
  Button::OnInitialize();

  // Push button requires the Leave event.
  Actor self = Self();
  self.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::PUSH_BUTTON);
}

DevelControl::ControlAccessible* PushButton::CreateAccessibleObject()
{
  return new PushButtonAccessible(Self());
}

void PushButton::SetIconAlignment(const PushButton::IconAlignment iconAlignment)
{
  mIconAlignment = iconAlignment;
  Button::Align labelAlignment;
  switch (iconAlignment)
  {
    case RIGHT:
    {
      labelAlignment = Button::BEGIN;
      break;
    }
    case TOP:
    {
      labelAlignment = Button::BOTTOM;
      break;
    }
    case BOTTOM:
    {
      labelAlignment = Button::TOP;
      break;
    }
    case LEFT:
    default:
      labelAlignment = Button::END;
      break;
  }

  Button::SetLabelAlignment(labelAlignment);
}

const PushButton::IconAlignment PushButton::GetIconAlignment() const
{
  return mIconAlignment;
}

void PushButton::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  UI::PushButton pushButton = UI::PushButton::DownCast(Dali::BaseHandle(object));

  if (pushButton)
  {
    PushButton& pushButtonImpl(GetImplementation(pushButton));

    // Properties remain here for Tizen 3.0 legacy requirements. Are now in Button base class

    switch (propertyIndex)
    {
      case UI::PushButton::Property::LABEL_PADDING:
      {
        Vector4 padding(value.Get<Vector4>());
        pushButtonImpl.Button::SetLabelPadding(Padding(padding.x, padding.y, padding.z, padding.w));
        break;
      }
      case UI::PushButton::Property::ICON_PADDING:
      {
        Vector4 padding(value.Get<Vector4>());
        pushButtonImpl.Button::SetForegroundPadding(Padding(padding.x, padding.y, padding.z, padding.w));
        break;
      }
    }
  }
}

Property::Value PushButton::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  UI::PushButton pushButton = UI::PushButton::DownCast(Dali::BaseHandle(object));

  if (pushButton)
  {
    PushButton& pushButtonImpl(GetImplementation(pushButton));

    switch (propertyIndex)
    {
      case UI::PushButton::Property::LABEL_PADDING:
      {
        Padding padding = pushButtonImpl.Button::GetLabelPadding();
        value = Vector4(padding.x, padding.y, padding.top, padding.bottom);
        break;
      }
      case UI::PushButton::Property::ICON_PADDING:
      {
        Padding padding = pushButtonImpl.Button::GetForegroundPadding();
        value = Vector4(padding.x, padding.y, padding.top, padding.bottom);
        break;
      }
    }
  }

  return value;
}

Dali::Accessibility::States PushButton::PushButtonAccessible::CalculateStates()
{
  auto state = Button::ButtonAccessible::CalculateStates();
  auto self = UI::Button::DownCast(Self());
  state[Dali::Accessibility::State::PRESSED] = self.GetProperty<bool>(UI::Button::Property::SELECTED);
  return state;
}

void PushButton::OnStateChange(State newState)
{
  // TODO: replace it with OnPropertySet hook once Button::Property::SELECTED will be consistently used
  if (newState == SELECTED_STATE || newState == UNSELECTED_STATE)
  {
    auto accessible = std::dynamic_pointer_cast<Dali::Accessibility::ActorAccessible>(GetAccessibleObject());
    if (DALI_LIKELY(accessible) && accessible->IsHighlighted())
    {
      accessible->EmitStateChanged(Dali::Accessibility::State::PRESSED, newState == SELECTED_STATE ? 1 : 0, 0);

      if (Self().GetProperty<bool>(UI::Button::Property::TOGGLABLE))
      {
        accessible->EmitStateChanged(Dali::Accessibility::State::CHECKED, newState == SELECTED_STATE ? 1 : 0, 0);
      }
    }
  }
}

} // namespace Internal

} // namespace UI

} // namespace Dali

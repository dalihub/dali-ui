/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/internal/controls/scrollable/scrollable-impl.h>

using namespace Dali;

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
  // empty handle as we cannot create Scrollable (but type registered for scroll signal)
  return BaseHandle();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(UI::Scrollable, UI::Control, Create);

DALI_PROPERTY_REGISTRATION(UI, Scrollable, "overshootEffectColor", VECTOR4, OVERSHOOT_EFFECT_COLOR)
DALI_PROPERTY_REGISTRATION(UI, Scrollable, "overshootAnimationSpeed", FLOAT, OVERSHOOT_ANIMATION_SPEED)
DALI_PROPERTY_REGISTRATION(UI, Scrollable, "overshootEnabled", BOOLEAN, OVERSHOOT_ENABLED)
DALI_PROPERTY_REGISTRATION(UI, Scrollable, "overshootSize", VECTOR2, OVERSHOOT_SIZE)
DALI_PROPERTY_REGISTRATION(UI, Scrollable, "scrollToAlphaFunction", INTEGER, SCROLL_TO_ALPHA_FUNCTION)

DALI_ANIMATABLE_PROPERTY_REGISTRATION(UI, Scrollable, "scrollRelativePosition", VECTOR2, SCROLL_RELATIVE_POSITION)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(UI, Scrollable, "scrollPositionMin", VECTOR2, SCROLL_POSITION_MIN)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(UI, Scrollable, "scrollPositionMinX", SCROLL_POSITION_MIN_X,
                                                SCROLL_POSITION_MIN, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(UI, Scrollable, "scrollPositionMinY", SCROLL_POSITION_MIN_Y,
                                                SCROLL_POSITION_MIN, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(UI, Scrollable, "scrollPositionMax", VECTOR2, SCROLL_POSITION_MAX)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(UI, Scrollable, "scrollPositionMaxX", SCROLL_POSITION_MAX_X,
                                                SCROLL_POSITION_MAX, 0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(UI, Scrollable, "scrollPositionMaxY", SCROLL_POSITION_MAX_Y,
                                                SCROLL_POSITION_MAX, 1)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(UI, Scrollable, "canScrollVertical", BOOLEAN, CAN_SCROLL_VERTICAL)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(UI, Scrollable, "canScrollHorizontal", BOOLEAN, CAN_SCROLL_HORIZONTAL)

DALI_SIGNAL_REGISTRATION(UI, Scrollable, "scrollStarted", SIGNAL_SCROLL_STARTED)
DALI_SIGNAL_REGISTRATION(UI, Scrollable, "scrollCompleted", SIGNAL_SCROLL_COMPLETED)
DALI_SIGNAL_REGISTRATION(UI, Scrollable, "scrollUpdated", SIGNAL_SCROLL_UPDATED)

DALI_TYPE_REGISTRATION_END()

const Vector4 DEFAULT_OVERSHOOT_COLOUR(0.0f, 0.64f, 0.85f, 0.25f);
const float DEFAULT_OVERSHOOT_ANIMATION_SPEED(120.0f); // 120 pixels per second
const Vector2 OVERSHOOT_DEFAULT_SIZE(720.0f, 42.0f);

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// Scrollable
///////////////////////////////////////////////////////////////////////////////////////////////////

Scrollable::Scrollable(ControlBehaviour behaviourFlags)
  : Control(ControlBehaviour(behaviourFlags)),
    mOvershootEffectColor(DEFAULT_OVERSHOOT_COLOUR),
    mOvershootAnimationSpeed(DEFAULT_OVERSHOOT_ANIMATION_SPEED),
    mOvershootSize(OVERSHOOT_DEFAULT_SIZE),
    mScrollToAlphaFunction(AlphaFunction::EASE_OUT),
    mScrollStartedSignal(),
    mScrollUpdatedSignal(),
    mScrollCompletedSignal(),
    mOvershootEnabled(true)
{
}

Scrollable::~Scrollable()
{
}

bool Scrollable::ScrollableAccessible::IsScrollable() const
{
  return true;
}

void Scrollable::OnInitialize()
{
  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::SCROLL_PANE);
}

DevelControl::ControlAccessible* Scrollable::CreateAccessibleObject()
{
  return new ScrollableAccessible(Self());
}

bool Scrollable::IsOvershootEnabled() const
{
  return mOvershootEnabled;
}

void Scrollable::SetOvershootEnabled(bool enable)
{
  EnableScrollOvershoot(enable);
  mOvershootEnabled = enable;
}

Vector4 Scrollable::GetOvershootEffectColor() const
{
  return mOvershootEffectColor;
};

void Scrollable::SetOvershootAnimationSpeed(float pixelsPerSecond)
{
  mOvershootAnimationSpeed = pixelsPerSecond;
}

float Scrollable::GetOvershootAnimationSpeed() const
{
  return mOvershootAnimationSpeed;
};

const Vector2& Scrollable::GetOvershootSize() const
{
  return mOvershootSize;
}

UI::Scrollable::ScrollStartedSignalType& Scrollable::ScrollStartedSignal()
{
  return mScrollStartedSignal;
}

UI::Scrollable::ScrollUpdatedSignalType& Scrollable::ScrollUpdatedSignal()
{
  return mScrollUpdatedSignal;
}

UI::Scrollable::ScrollCompletedSignalType& Scrollable::ScrollCompletedSignal()
{
  return mScrollCompletedSignal;
}

bool Scrollable::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName,
                                 FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool connected(true);
  UI::Scrollable scrollable = UI::Scrollable::DownCast(handle);

  if (0 == strcmp(signalName.c_str(), SIGNAL_SCROLL_STARTED))
  {
    scrollable.ScrollStartedSignal().Connect(tracker, functor);
  }
  else if (0 == strcmp(signalName.c_str(), SIGNAL_SCROLL_UPDATED))
  {
    scrollable.ScrollUpdatedSignal().Connect(tracker, functor);
  }
  else if (0 == strcmp(signalName.c_str(), SIGNAL_SCROLL_COMPLETED))
  {
    scrollable.ScrollCompletedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Scrollable::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  UI::Scrollable scrollable = UI::Scrollable::DownCast(Dali::BaseHandle(object));

  if (scrollable)
  {
    Scrollable& scrollableImpl(GetImpl(scrollable));
    switch (index)
    {
      case UI::Scrollable::Property::OVERSHOOT_EFFECT_COLOR:
      {
        scrollableImpl.SetOvershootEffectColor(value.Get<Vector4>());
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED:
      {
        scrollableImpl.SetOvershootAnimationSpeed(value.Get<float>());
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_ENABLED:
      {
        scrollableImpl.SetOvershootEnabled(value.Get<bool>());
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_SIZE:
      {
        scrollableImpl.SetOvershootSize(value.Get<Vector2>());
        break;
      }
      case UI::Scrollable::Property::SCROLL_TO_ALPHA_FUNCTION:
      {
        int alphaFunction = value.Get<int>();

        if (alphaFunction >= AlphaFunction::DEFAULT && alphaFunction < AlphaFunction::COUNT)
        {
          scrollableImpl.mScrollToAlphaFunction = static_cast<AlphaFunction::BuiltinFunction>(alphaFunction);
        }
        break;
      }
    }
  }
}

Property::Value Scrollable::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  UI::Scrollable scrollable = UI::Scrollable::DownCast(Dali::BaseHandle(object));

  if (scrollable)
  {
    Scrollable& scrollableImpl(GetImpl(scrollable));
    switch (index)
    {
      case UI::Scrollable::Property::OVERSHOOT_EFFECT_COLOR:
      {
        value = scrollableImpl.GetOvershootEffectColor();
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_ANIMATION_SPEED:
      {
        value = scrollableImpl.GetOvershootAnimationSpeed();
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_ENABLED:
      {
        value = scrollableImpl.IsOvershootEnabled();
        break;
      }
      case UI::Scrollable::Property::OVERSHOOT_SIZE:
      {
        value = scrollableImpl.mOvershootSize;
        break;
      }
      case UI::Scrollable::Property::SCROLL_TO_ALPHA_FUNCTION:
      {
        value = static_cast<int>(scrollableImpl.mScrollToAlphaFunction);
        break;
      }
    }
  }

  return value;
}

} // namespace Internal

} // namespace UI

} // namespace Dali

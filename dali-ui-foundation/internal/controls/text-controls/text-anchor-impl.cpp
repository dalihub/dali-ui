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
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry-helper.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>

// DEVEL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>

using namespace Dali::UI::Text;

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

// Type registration
BaseHandle Create()
{
  return UI::TextAnchor::New();
}

// clang-format off
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(UI::TextAnchor, UI::Control, Create);

DALI_PROPERTY_REGISTRATION(UI, TextAnchor, "startCharacterIndex", INTEGER, START_CHARACTER_INDEX)
DALI_PROPERTY_REGISTRATION(UI, TextAnchor, "endCharacterIndex",   INTEGER, END_CHARACTER_INDEX  )
DALI_PROPERTY_REGISTRATION(UI, TextAnchor, "uri",                 STRING,  URI        )

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

UI::TextAnchor TextAnchor::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextAnchor> impl = new TextAnchor();

  // Pass ownership to CustomActor handle
  UI::TextAnchor handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

Property::Value TextAnchor::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  UI::TextAnchor anchor = UI::TextAnchor::DownCast(Dali::BaseHandle(object));

  if (anchor)
  {
    TextAnchor& impl(GetImpl(anchor));

    switch (index)
    {
      case UI::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value = impl.mStartCharacterIndex;
        break;
      }
      case UI::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value = impl.mEndCharacterIndex;
        break;
      }
      case UI::TextAnchor::Property::URI:
      {
        value = impl.mUri;
        break;
      }
    }
  }

  return value;
}

void TextAnchor::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  UI::TextAnchor anchor = UI::TextAnchor::DownCast(Dali::BaseHandle(object));

  if (anchor)
  {
    TextAnchor& impl(GetImpl(anchor));
    switch (index)
    {
      case UI::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value.Get(impl.mStartCharacterIndex);
        break;
      }

      case UI::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value.Get(impl.mEndCharacterIndex);
        break;
      }

      case UI::TextAnchor::Property::URI:
      {
        value.Get(impl.mUri);
        break;
      }
    }
  }
}

void TextAnchor::OnInitialize()
{
  Actor self = Self();

  // Accessibility
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, DevelControl::AccessibilityRole::LINK);
}

DevelControl::ControlAccessible* TextAnchor::CreateAccessibleObject()
{
  return new TextAnchorAccessible(Self());
}

TextAnchor::TextAnchor()
  : Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
    mStartCharacterIndex(0),
    mEndCharacterIndex(0),
    mUri()
{
}

TextAnchor::~TextAnchor()
{
}

void TextAnchor::TextAnchorAccessible::InitDefaultFeatures()
{
  DevelControl::ControlAccessible::InitDefaultFeatures();
  AddFeature<Dali::Accessibility::Hyperlink>(shared_from_this());
}

int32_t TextAnchor::TextAnchorAccessible::GetEndIndex() const
{
  auto self = UI::TextAnchor::DownCast(Self());
  return self.GetProperty(UI::TextAnchor::Property::END_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchor::TextAnchorAccessible::GetStartIndex() const
{
  auto self = UI::TextAnchor::DownCast(Self());
  return self.GetProperty(UI::TextAnchor::Property::START_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchor::TextAnchorAccessible::GetAnchorCount() const
{
  return 1;
}

Dali::Accessibility::Accessible* TextAnchor::TextAnchorAccessible::GetAnchorAccessible(int32_t anchorIndex) const
{
  return const_cast<TextAnchorAccessible*>(this);
}

std::string TextAnchor::TextAnchorAccessible::GetAnchorUri(int32_t anchorIndex) const
{
  auto self = UI::TextAnchor::DownCast(Self());
  return self.GetProperty(UI::TextAnchor::Property::URI).Get<std::string>();
}

bool TextAnchor::TextAnchorAccessible::IsValid() const
{
  return !GetAnchorUri(0).empty();
}

bool TextAnchor::OnAccessibilityActivated()
{
  Dali::Actor current = Self();
  Dali::UI::Text::AnchorControlInterface* parentImplementationAnchorInterface = nullptr;
  while (!current.GetProperty<bool>(Actor::Property::IS_ROOT) && !parentImplementationAnchorInterface)
  {
    Dali::Actor parentAsActor = current.GetParent();
    Dali::CustomActor parentAsCustomActor = Dali::CustomActor::DownCast(parentAsActor);
    Dali::CustomActorImpl& parentImplementation = parentAsCustomActor.GetImplementation();
    parentImplementationAnchorInterface = dynamic_cast<Dali::UI::Text::AnchorControlInterface*>(&parentImplementation);
    current = parentAsActor;
  }

  if (parentImplementationAnchorInterface)
  {
    std::string href;
    std::string uri = Self().GetProperty(UI::TextAnchor::Property::URI).Get<std::string>();
    parentImplementationAnchorInterface->AnchorClicked(mStartCharacterIndex, href);
    parentImplementationAnchorInterface->EmitAnchorClickedSignal(uri);
    return true;
  }

  DALI_LOG_ERROR(
      "TextAnchor::OnAccessibilityActivate cannot find ancestor actor implementing "
      "Dali::UI::Text::AnchorControlInterface.\n");
  return false;
}

} // namespace Internal

} // namespace UI

} // namespace Dali

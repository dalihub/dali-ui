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
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessibility.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

// Type registration
BaseHandle Create()
{
  return Ui::TextAnchor::New();
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

#define DALI_UI_TOKEN_PASTE_EXPAND(x, y) x##y
#define DALI_UI_TOKEN_PASTE(x, y) DALI_UI_TOKEN_PASTE_EXPAND(x, y)
#define DALI_UI_PROPERTY_REGISTRATION(viewType, implType, text, valueType, propertyIndex) \
  Dali::PropertyRegistration DALI_UI_TOKEN_PASTE(property, __COUNTER__)(                  \
    typeRegistration,                                                                     \
    text,                                                                                 \
    viewType::Property::propertyIndex,                                                    \
    Dali::Property::valueType,                                                            \
    &implType::SetProperty,                                                               \
    &implType::GetProperty);

// clang-format off
DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::TextAnchor, Ui::Internal::TextAnchorImpl, Ui::View, Create);

DALI_UI_PROPERTY_REGISTRATION(TextAnchor, TextAnchorImpl, "startCharacterIndex", INTEGER, START_CHARACTER_INDEX)
DALI_UI_PROPERTY_REGISTRATION(TextAnchor, TextAnchorImpl, "endCharacterIndex",   INTEGER, END_CHARACTER_INDEX  )
DALI_UI_PROPERTY_REGISTRATION(TextAnchor, TextAnchorImpl, "uri",                 STRING,  URI                  )

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

TextAnchorImplPtr TextAnchorImpl::New()
{
  return TextAnchorImplPtr(new TextAnchorImpl());
}

TextAnchorImpl::TextAnchorImpl()
: ViewImpl(),
  mStartCharacterIndex(0),
  mEndCharacterIndex(0),
  mUri()
{
  Dali::Ui::Integration::ViewAccessibility::SetAccessibleObjectCreator(
    *this,
    [](Dali::Ui::View view) -> ViewAccessible*
  {
    return new TextAnchorAccessible(view);
  });
}

TextAnchorImpl::~TextAnchorImpl()
{
}

Dali::Property::Value TextAnchorImpl::GetProperty(BaseObject* object, Dali::Property::Index index)
{
  Property::Value value;

  Ui::TextAnchor anchor = Ui::TextAnchor::DownCast(Dali::BaseHandle(object));

  if(anchor)
  {
    TextAnchorImpl& impl(GetImpl(anchor));

    switch(index)
    {
      case Ui::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value = impl.mStartCharacterIndex;
        break;
      }
      case Ui::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value = impl.mEndCharacterIndex;
        break;
      }
      case Ui::TextAnchor::Property::URI:
      {
        value = ToPropertyValue(impl.mUri);
        break;
      }
    }
  }

  return value;
}

void TextAnchorImpl::SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::TextAnchor anchor = Ui::TextAnchor::DownCast(Dali::BaseHandle(object));

  if(anchor)
  {
    TextAnchorImpl& impl(GetImpl(anchor));
    switch(index)
    {
      case Ui::TextAnchor::Property::START_CHARACTER_INDEX:
      {
        value.Get(impl.mStartCharacterIndex);
        break;
      }

      case Ui::TextAnchor::Property::END_CHARACTER_INDEX:
      {
        value.Get(impl.mEndCharacterIndex);
        break;
      }

      case Ui::TextAnchor::Property::URI:
      {
        GetStdString(value, impl.mUri);
        break;
      }
    }
  }
}

void TextAnchorImpl::OnInitialize()
{
  Actor self = Self();

  // Accessibility
  Ui::View::DownCast(self).SetAccessibilityRole(Accessibility::Role::LINK);
}

void TextAnchorImpl::TextAnchorAccessible::InitDefaultFeatures()
{
  ViewAccessible::InitDefaultFeatures();
  AddFeature<Dali::Accessibility::Hyperlink>(SharedFromThis());
}

int32_t TextAnchorImpl::TextAnchorAccessible::GetEndIndex() const
{
  auto self = Ui::TextAnchor::DownCast(Self());
  return self.GetProperty(Ui::TextAnchor::Property::END_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchorImpl::TextAnchorAccessible::GetStartIndex() const
{
  auto self = Ui::TextAnchor::DownCast(Self());
  return self.GetProperty(Ui::TextAnchor::Property::START_CHARACTER_INDEX).Get<int>();
}

int32_t TextAnchorImpl::TextAnchorAccessible::GetAnchorCount() const
{
  return 1;
}

Dali::Accessibility::Accessible* TextAnchorImpl::TextAnchorAccessible::GetAnchorAccessible(int32_t anchorIndex) const
{
  return const_cast<TextAnchorAccessible*>(this);
}

std::string TextAnchorImpl::TextAnchorAccessible::GetAnchorUri(int32_t anchorIndex) const
{
  auto self = Ui::TextAnchor::DownCast(Self());
  return ToStdString(self.GetProperty(Ui::TextAnchor::Property::URI));
}

bool TextAnchorImpl::TextAnchorAccessible::IsValid() const
{
  return !GetAnchorUri(0).empty();
}

bool TextAnchorImpl::OnAccessibilityActivate()
{
  Dali::Actor                                          current                             = Self();
  Dali::Ui::Integration::Text::AnchorControlInterface* parentImplementationAnchorInterface = nullptr;
  while(!current.GetProperty<bool>(Actor::Property::IS_ROOT) && !parentImplementationAnchorInterface)
  {
    Dali::Actor            parentAsActor        = current.GetParent();
    Dali::CustomActor      parentAsCustomActor  = Dali::CustomActor::DownCast(parentAsActor);
    Dali::CustomActorImpl& parentImplementation = parentAsCustomActor.GetImplementation();
    parentImplementationAnchorInterface         = dynamic_cast<Dali::Ui::Integration::Text::AnchorControlInterface*>(&parentImplementation);
    current                                     = parentAsActor;
  }

  if(parentImplementationAnchorInterface)
  {
    std::string href;
    std::string uri = ToStdString(Self().GetProperty(Ui::TextAnchor::Property::URI));
    parentImplementationAnchorInterface->AnchorClicked(mStartCharacterIndex, href);
    parentImplementationAnchorInterface->EmitAnchorClicked(uri);
    return true;
  }

  DALI_LOG_ERROR(
    "TextAnchor::OnAccessibilityActivate cannot find ancestor actor implementing "
    "Dali::Ui::Integration::Text::AnchorControlInterface.\n");
  return false;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali

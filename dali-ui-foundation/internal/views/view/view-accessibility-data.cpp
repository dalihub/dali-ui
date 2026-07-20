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
#include "view-accessibility-data.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

#include <sstream>

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
static constexpr const char* READING_INFO_TYPE_NAME               = "name";
static constexpr const char* READING_INFO_TYPE_ATTRIBUTE_NAME     = "reading_info_type";
static constexpr const char* READING_INFO_TYPE_ROLE               = "role";
static constexpr const char* READING_INFO_TYPE_DESCRIPTION        = "description";
static constexpr const char* READING_INFO_TYPE_STATE              = "state";
static constexpr const char* READING_INFO_TYPE_SEPARATOR          = "|";
static constexpr const char* NAME_LANGUAGE_SPANS_ATTRIBUTE        = "a11y.name.spans";
static constexpr const char* DESCRIPTION_LANGUAGE_SPANS_ATTRIBUTE = "a11y.description.spans";

std::string EscapeJsonString(const std::string& value)
{
  std::string escaped;
  escaped.reserve(value.size());
  for(char character : value)
  {
    switch(character)
    {
      case '\\':
        escaped += "\\\\";
        break;
      case '"':
        escaped += "\\\"";
        break;
      case '\b':
        escaped += "\\b";
        break;
      case '\f':
        escaped += "\\f";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        escaped += character;
        break;
    }
  }
  return escaped;
}

template<typename Container>
std::string SerializeLanguageSpans(const Container& spans)
{
  std::ostringstream stream;
  stream << '[';
  for(std::size_t index = 0u; index < spans.size(); ++index)
  {
    if(index != 0u)
    {
      stream << ',';
    }
    const auto& span = spans[index];
    stream << "{\"start\":" << span.start
           << ",\"length\":" << span.length
           << ",\"locale\":\"" << EscapeJsonString(span.locale) << "\"}";
  }
  stream << ']';
  return stream.str();
}

Dali::Bounds GetShowingGeometry(Dali::Bounds rect, Dali::Ui::ViewAccessible* accessible)
{
  Bounds  parentRect;
  Vector2 currentPosition;
  auto    parent = dynamic_cast<Ui::ViewAccessible*>(accessible->GetParent());

  while(parent)
  {
    parentRect = parent->GetExtents(Dali::Devel::Accessibility::CoordinateType::WINDOW); // LCOV_EXCL_LINE

    currentPosition.x = rect.x;
    currentPosition.y = rect.y;

    rect.x      = rect.x > parentRect.x ? rect.x : parentRect.x;
    rect.y      = rect.y > parentRect.y ? rect.y : parentRect.y;
    rect.width  = currentPosition.x + rect.width < parentRect.x + parentRect.width
                    ? currentPosition.x + rect.width - rect.x
                    : parentRect.x + parentRect.width - rect.x;
    rect.height = currentPosition.y + rect.height < parentRect.y + parentRect.height
                    ? currentPosition.y + rect.height - rect.y
                    : parentRect.y + parentRect.height - rect.y;

    if(rect.width < 0 || rect.height < 0)
    {
      return rect;
    }

    parent = dynamic_cast<Ui::ViewAccessible*>(parent->GetParent());
  }

  return rect;
}
static bool IsShowingGeometryOnScreen(Dali::Bounds rect)
{
  return rect.width > 0 && rect.height > 0;
}

static constexpr uint32_t DEFAULT_READING_INFO_TYPES_RAW_DATA =
  (1u << static_cast<uint32_t>(Dali::Integration::Accessibility::ReadingInfoType::NAME)) |        // LCOV_EXCL_LINE
  (1u << static_cast<uint32_t>(Dali::Integration::Accessibility::ReadingInfoType::ROLE)) |        // LCOV_EXCL_LINE
  (1u << static_cast<uint32_t>(Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION)) | // LCOV_EXCL_LINE
  (1u << static_cast<uint32_t>(Dali::Integration::Accessibility::ReadingInfoType::STATE));        // LCOV_EXCL_LINE

static constexpr uint32_t DEFAULT_DEVEL_VIEW_ACCESSIBILITY_STATES_RAW_DATA =
  (1u << static_cast<uint32_t>(Accessibility::State::ENABLED)) |
  (0u << static_cast<uint32_t>(Accessibility::State::SELECTED)) |
  (0u << static_cast<uint32_t>(Accessibility::State::CHECKED)) |
  (0u << static_cast<uint32_t>(Accessibility::State::BUSY)) |
  (0u << static_cast<uint32_t>(Accessibility::State::EXPANDED));

} // unnamed namespace

ViewDataImpl::AccessibilityData::AccessibilityData(ViewImpl& viewImpl)
: mAccessibilityDoGestureSignal(),
  mViewImpl(viewImpl),
  mIsAccessibilityPositionPropertyNotificationSet(false),
  mIsAccessibilityPropertySetSignalRegistered(false)
{
  mAccessibilityProps.states = GetDefaultViewAccessibilityStates();
}

void ViewDataImpl::AccessibilityData::AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value)
{
  Property::Value* checkedValue = mAccessibilityProps.extraAttributes.Find(key);
  if(checkedValue)
  {
    mAccessibilityProps.extraAttributes[key] = value;
  }
  else
  {
    mAccessibilityProps.extraAttributes.Insert(key, value);
  }
}

void ViewDataImpl::AccessibilityData::CheckHighlightedObjectGeometry()
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    auto lastPosition   = accessible->GetLastPosition();
    auto accessibleRect = accessible->GetExtents(Dali::Devel::Accessibility::CoordinateType::WINDOW); // LCOV_EXCL_LINE
    auto rect           = GetShowingGeometry(accessibleRect, accessible.Get());

    switch(mAccessibilityLastScreenRelativeMoveType)
    {
      case Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTSIDE: // LCOV_EXCL_LINE
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Devel::Accessibility::ScreenRelativeMoveType::INSIDE; // LCOV_EXCL_LINE
        }
        break;
      }
      case Dali::Devel::Accessibility::ScreenRelativeMoveType::INSIDE: // LCOV_EXCL_LINE
      {
        if(rect.width < 0 && !Dali::Equals(accessibleRect.x, lastPosition.x))
        {
          mAccessibilityLastScreenRelativeMoveType =
            (accessibleRect.x < lastPosition.x) ? Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT      // LCOV_EXCL_LINE
                                                : Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT; // LCOV_EXCL_LINE
        }
        if(rect.height < 0 && !Dali::Equals(accessibleRect.y, lastPosition.y))
        {
          mAccessibilityLastScreenRelativeMoveType =
            (accessibleRect.y < lastPosition.y) ? Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT      // LCOV_EXCL_LINE
                                                : Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT; // LCOV_EXCL_LINE
        }
        // notify AT-clients on outgoing moves only
        if(mAccessibilityLastScreenRelativeMoveType != Dali::Devel::Accessibility::ScreenRelativeMoveType::INSIDE) // LCOV_EXCL_LINE
        {
          accessible->EmitMovedOutOfScreen(mAccessibilityLastScreenRelativeMoveType);
        }
        break;
      }
      case Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_TOP_LEFT:     // LCOV_EXCL_LINE
      case Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTGOING_BOTTOM_RIGHT: // LCOV_EXCL_LINE
      {
        if(IsShowingGeometryOnScreen(rect))
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Devel::Accessibility::ScreenRelativeMoveType::INSIDE; // LCOV_EXCL_LINE
        }
        else
        {
          mAccessibilityLastScreenRelativeMoveType = Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTSIDE; // LCOV_EXCL_LINE
        }
        break;
      }
      default:
      {
        break;
      }
    }

    accessible->SetLastPosition(Vector2(accessibleRect.x, accessibleRect.y));
  }
}

void ViewDataImpl::AccessibilityData::RegisterAccessibilityPositionPropertyNotification()
{
  if(mIsAccessibilityPositionPropertyNotificationSet)
  {
    return;
  }
  // set default value until first move of object is detected
  mAccessibilityLastScreenRelativeMoveType = Dali::Devel::Accessibility::ScreenRelativeMoveType::OUTSIDE; // LCOV_EXCL_LINE
  // recalculate mAccessibilityLastScreenRelativeMoveType accordingly to the initial position
  CheckHighlightedObjectGeometry();
  mAccessibilityPositionNotification =
    mViewImpl.Self().AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mAccessibilityPositionNotification.SetNotifyMode(PropertyNotification::NOTIFY_ON_CHANGED);
  mAccessibilityPositionNotification.NotifySignal().Connect(
    this, [this](PropertyNotification)
  { CheckHighlightedObjectGeometry(); });
  mIsAccessibilityPositionPropertyNotificationSet = true;
}

void ViewDataImpl::AccessibilityData::UnregisterAccessibilityPositionPropertyNotification()
{
  mViewImpl.Self().RemovePropertyNotification(mAccessibilityPositionNotification);
  mIsAccessibilityPositionPropertyNotificationSet = false;
}

void ViewDataImpl::AccessibilityData::RegisterAccessibilityPropertySetSignal()
{
  if(mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mViewImpl.Self().PropertySetSignal().Connect(this, &ViewDataImpl::AccessibilityData::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = true;
}

void ViewDataImpl::AccessibilityData::UnregisterAccessibilityPropertySetSignal()
{
  if(!mIsAccessibilityPropertySetSignalRegistered)
  {
    return;
  }
  mViewImpl.Self().PropertySetSignal().Disconnect(this,
                                                  &ViewDataImpl::AccessibilityData::OnAccessibilityPropertySet);
  mIsAccessibilityPropertySetSignalRegistered = false;
}

void ViewDataImpl::AccessibilityData::OnAccessibilityPropertySet(Dali::Handle handle, Dali::Property::Index index,
                                                                 const Dali::Property::Value& value)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    if(mAccessibilityProps.name.empty() && index == accessible->GetNamePropertyIndex())
    {
      accessible->Emit(Dali::Devel::Accessibility::ObjectPropertyChangeEvent::NAME); // LCOV_EXCL_LINE
      return;
    }

    if(mAccessibilityProps.description.empty() && index == accessible->GetDescriptionPropertyIndex())
    {
      accessible->Emit(Dali::Devel::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION); // LCOV_EXCL_LINE
      return;
    }
  }
}

Dali::Integration::Accessibility::ReadingInfoTypes ViewDataImpl::AccessibilityData::GetAccessibilityReadingInfoType() const
{
  std::string value{};
  auto        place = mAccessibilityProps.extraAttributes.Find(READING_INFO_TYPE_ATTRIBUTE_NAME);
  if(place)
  {
    GetStdString(*place, value);
  }
  else
  {
    return GetDefaultReadingInfoTypes();
  }

  if(value.empty())
  {
    return {};
  }

  Dali::Integration::Accessibility::ReadingInfoTypes types;

  if(value.find(READING_INFO_TYPE_NAME) != std::string::npos)
  {
    types[Dali::Integration::Accessibility::ReadingInfoType::NAME] = true;
  }
  if(value.find(READING_INFO_TYPE_ROLE) != std::string::npos)
  {
    types[Dali::Integration::Accessibility::ReadingInfoType::ROLE] = true;
  }
  if(value.find(READING_INFO_TYPE_DESCRIPTION) != std::string::npos)
  {
    types[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  }
  if(value.find(READING_INFO_TYPE_STATE) != std::string::npos)
  {
    types[Dali::Integration::Accessibility::ReadingInfoType::STATE] = true;
  }

  return types;
}

void ViewDataImpl::AccessibilityData::RemoveAccessibilityAttribute(const Dali::String& key)
{
  mAccessibilityProps.extraAttributes.Remove(key);
}

void ViewDataImpl::AccessibilityData::ClearAccessibilityAttributes()
{
  mAccessibilityProps.extraAttributes.Clear();
  mAccessibilityProps.nameLanguageSpans.clear();
  mAccessibilityProps.descriptionLanguageSpans.clear();
}

void ViewDataImpl::AccessibilityData::EmitPropertyChanged(Dali::Devel::Accessibility::ObjectPropertyChangeEvent event)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    accessible->Emit(event); // LCOV_EXCL_LINE
  }
}

void ViewDataImpl::AccessibilityData::UpdateLanguageSpanAttribute(bool nameSpans)
{
  const char* key   = nameSpans ? NAME_LANGUAGE_SPANS_ATTRIBUTE : DESCRIPTION_LANGUAGE_SPANS_ATTRIBUTE;
  const auto& spans = nameSpans ? mAccessibilityProps.nameLanguageSpans : mAccessibilityProps.descriptionLanguageSpans;
  if(spans.empty())
  {
    RemoveAccessibilityAttribute(key);
  }
  else
  {
    AppendAccessibilityAttribute(key, ToDaliString(SerializeLanguageSpans(spans)));
  }
}

void ViewDataImpl::AccessibilityData::SetAccessibilityReadingInfoType(
  const Dali::Integration::Accessibility::ReadingInfoTypes types)
{
  std::string value{};
  if(types[Dali::Integration::Accessibility::ReadingInfoType::NAME])
  {
    value += READING_INFO_TYPE_NAME;
  }
  if(types[Dali::Integration::Accessibility::ReadingInfoType::ROLE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_ROLE;
  }
  if(types[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_DESCRIPTION;
  }
  if(types[Dali::Integration::Accessibility::ReadingInfoType::STATE])
  {
    if(!value.empty())
    {
      value += READING_INFO_TYPE_SEPARATOR;
    }
    value += READING_INFO_TYPE_STATE;
  }
  AppendAccessibilityAttribute(READING_INFO_TYPE_ATTRIBUTE_NAME, ToDaliString(value));
}

SharedPtr<Ui::ViewAccessible> ViewDataImpl::AccessibilityData::GetAccessibleObject()
{
  return DynamicPointerCast<ViewAccessible>(Dali::Accessibility::Accessible::GetOwningPtr(mViewImpl.Self()));
}

Dali::Integration::Accessibility::ReadingInfoTypes ViewDataImpl::AccessibilityData::GetDefaultReadingInfoTypes()
{
  return Dali::Integration::Accessibility::ReadingInfoTypes{DEFAULT_READING_INFO_TYPES_RAW_DATA};
}

AccessibilityStates ViewDataImpl::AccessibilityData::GetDefaultViewAccessibilityStates()
{
  return DEFAULT_DEVEL_VIEW_ACCESSIBILITY_STATES_RAW_DATA;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

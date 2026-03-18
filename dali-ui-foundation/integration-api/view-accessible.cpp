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
#include "view-accessible.h"

// EXTERNAL INCLUDES
#ifdef DGETTEXT_ENABLED
#include <libintl.h>
#endif

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/type-info.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/devel-api/property-bridge/property-bridge.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>
#include <dali-ui-foundation/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-ui-foundation/public-api/view.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToStdString;

namespace Dali::Ui
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_ACCESSIBLE");
#endif

constexpr const char* ATTR_IMG_SRC_KEY = "imgSrc";

std::string GetLocaleText(std::string string, const char* domain = "dali-toolkit")
{
#ifdef DGETTEXT_ENABLED
  /*TODO: currently non-localized string is used as a key for translation lookup. In case the lookup key formatting is
     forced consider calling utility function for converting non-localized string into well-formatted key before lookup.
   */
  return dgettext(domain, string.c_str());
#else
  return string;
#endif
}

Dali::Actor CreateHighlightIndicatorActor()
{
  std::string focusBorderImagePath(AssetManager::GetDaliImagePath());
  focusBorderImagePath += "/keyboard_focus.9.png";

  // Create the default if it hasn't been set and one that's shared by all the
  // keyboard focusable actors
  auto imageView = Ui::ImageView::New(ToDaliString(focusBorderImagePath));
  imageView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  imageView.AppendAccessibilityAttribute("highlight", Dali::String());
  imageView.SetProperty(Ui::View::Property::ACCESSIBILITY_HIGHLIGHTABLE, false);

  return imageView;
}

std::string FetchImageSrcFromMap(const Dali::Property::Map& imageMap)
{
  auto urlVal = imageMap.Find(Ui::ImageVisual::Property::URL);
  if(urlVal)
  {
    if(urlVal->GetType() == Dali::Property::STRING)
    {
      return ToStdString(*urlVal);
    }
    else if(urlVal->GetType() == Dali::Property::ARRAY)
    {
      auto urlArray = urlVal->GetArray();
      if(urlArray && !urlArray->Empty())
      {
        // Returns first element if url is an array
        return ToStdString((*urlArray)[0].Get<Dali::String>());
      }
    }
  }
  return {};
}

std::string FetchImageSrc(const Ui::ImageView& imageView)
{
  Dali::Property::Value urlValue = imageView.GetProperty(Ui::ImageView::Property::IMAGE);
  std::string           url;
  if(GetStdString(urlValue, url))
  {
    return url;
  }

  const auto imageMap = imageView.GetProperty<Dali::Property::Map>(Ui::ImageView::Property::IMAGE);
  if(!imageMap.Empty())
  {
    return FetchImageSrcFromMap(imageMap);
  }
  return {};
}

bool IsAtspiRole(int32_t rawRole)
{
  return rawRole >= static_cast<int32_t>(Dali::Accessibility::Role::INVALID) &&
         rawRole < static_cast<int32_t>(Dali::Accessibility::Role::MAX_COUNT);
}

bool IsRoleV2(int32_t rawRole)
{
  return rawRole >= static_cast<int32_t>(ROLE_START_INDEX) &&
         rawRole < static_cast<int32_t>(AccessibilityRole::MAX_COUNT);
}

#define TO_V1_ROLE_TYPE(v2RoleType, v1RoleType) \
  case AccessibilityRole::v2RoleType:           \
  {                                             \
    return Role::v1RoleType;                    \
  }
#define TO_SAME_ROLE_TYPE(roleType) \
  case AccessibilityRole::roleType: \
  {                                 \
    return Role::roleType;          \
  }

Dali::Accessibility::Role ConvertV2RoleToAtspiRole(AccessibilityRole role)
{
  using Dali::Accessibility::Role;
  switch(role)
  {
    TO_V1_ROLE_TYPE(ADJUSTABLE, SLIDER)
    TO_SAME_ROLE_TYPE(ALERT)
    TO_V1_ROLE_TYPE(BUTTON, PUSH_BUTTON)
    TO_SAME_ROLE_TYPE(CHECK_BOX)
    TO_SAME_ROLE_TYPE(COMBO_BOX)
    TO_V1_ROLE_TYPE(CONTAINER, FILLER)
    TO_SAME_ROLE_TYPE(DIALOG)
    TO_SAME_ROLE_TYPE(ENTRY)
    TO_SAME_ROLE_TYPE(HEADER)
    TO_SAME_ROLE_TYPE(IMAGE)
    TO_SAME_ROLE_TYPE(LINK)
    TO_SAME_ROLE_TYPE(LIST)
    TO_SAME_ROLE_TYPE(LIST_ITEM)
    TO_SAME_ROLE_TYPE(MENU)
    TO_SAME_ROLE_TYPE(MENU_BAR)
    TO_SAME_ROLE_TYPE(MENU_ITEM)
    TO_V1_ROLE_TYPE(NONE, UNKNOWN)
    TO_SAME_ROLE_TYPE(NOTIFICATION)
    TO_SAME_ROLE_TYPE(PASSWORD_TEXT)
    TO_SAME_ROLE_TYPE(POPUP_MENU)
    TO_SAME_ROLE_TYPE(PROGRESS_BAR)
    TO_SAME_ROLE_TYPE(RADIO_BUTTON)
    TO_SAME_ROLE_TYPE(SCROLL_BAR)
    TO_SAME_ROLE_TYPE(SPIN_BUTTON)
    TO_V1_ROLE_TYPE(TAB, PAGE_TAB)
    TO_V1_ROLE_TYPE(TAB_LIST, PAGE_TAB_LIST)
    TO_V1_ROLE_TYPE(TEXT, LABEL)
    TO_SAME_ROLE_TYPE(TOGGLE_BUTTON)
    TO_SAME_ROLE_TYPE(TOOL_BAR)
    TO_V1_ROLE_TYPE(SCENE_3D, FILLER)
    TO_V1_ROLE_TYPE(MODEL, IMAGE)
    default:
    {
      return Role::UNKNOWN;
    }
  }
}

Dali::Accessibility::Role ConvertRawRoleToAtspiRole(int32_t rawRole)
{
  if(IsAtspiRole(rawRole))
  {
    return static_cast<Dali::Accessibility::Role>(rawRole);
  }
  else if(IsRoleV2(rawRole))
  {
    return ConvertV2RoleToAtspiRole(static_cast<AccessibilityRole>(rawRole));
  }
  else
  {
    return Dali::Accessibility::Role::UNKNOWN;
  }
}

bool IsModalRole(int32_t rawRole)
{
  return IsRoleV2(rawRole) && (static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::ALERT ||
                               static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::DIALOG ||
                               static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::POPUP_MENU);
}

bool IsScene3DRole(int32_t rawRole)
{
  return IsRoleV2(rawRole) && (static_cast<AccessibilityRole>(rawRole) == AccessibilityRole::SCENE_3D);
}

bool IsHighlightableRole(int32_t rawRole)
{
  return IsRoleV2(rawRole) && static_cast<AccessibilityRole>(rawRole) != AccessibilityRole::NONE;
}

using Dali::Ui::Internal::TriStateProperty;
bool IsHighlightable(TriStateProperty highlightable, int32_t rawRole)
{
  switch(highlightable)
  {
    case TriStateProperty::AUTO:
    {
      return IsHighlightableRole(rawRole);
    }
    case TriStateProperty::TRUE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

} // unnamed namespace

ViewAccessible::ViewAccessible(Dali::Actor self)
: ActorAccessible(self)
{
}

std::string ViewAccessible::GetName() const
{
  auto view = Dali::Ui::View::DownCast(Self());

  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);
  Dali::String            name;

  auto* accessibilityData = viewImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData) && !accessibilityData->mAccessibilityGetNameSignal.Empty())
  {
    accessibilityData->mAccessibilityGetNameSignal.Emit(name);
  }
  else if(DALI_LIKELY(accessibilityData) && !accessibilityData->mAccessibilityProps.name.empty())
  {
    name = ToDaliString(accessibilityData->mAccessibilityProps.name);
  }
  else if(auto raw = GetNameRaw(); !raw.first.empty() || raw.second)
  {
    name = ToDaliString(raw.first);
  }
  else
  {
    name = Self().GetProperty<Dali::String>(Actor::Property::NAME);
  }

  return GetLocaleText(ToStdString(name));
}

std::pair<std::string, bool> ViewAccessible::GetNameRaw() const
{
  return {"", false};
}

std::string ViewAccessible::GetDescription() const
{
  auto view = Dali::Ui::View::DownCast(Self());

  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);
  Dali::String            description;

  auto* accessibilityData = viewImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData) && !accessibilityData->mAccessibilityGetDescriptionSignal.Empty())
  {
    accessibilityData->mAccessibilityGetDescriptionSignal.Emit(description);
  }
  else if(DALI_LIKELY(accessibilityData) && !accessibilityData->mAccessibilityProps.description.empty())
  {
    description = ToDaliString(accessibilityData->mAccessibilityProps.description);
  }
  else
  {
    description = ToDaliString(GetDescriptionRaw());
  }

  return GetLocaleText(ToStdString(description));
}

std::string ViewAccessible::GetDescriptionRaw() const
{
  return {};
}

std::string ViewAccessible::GetValue() const
{
  return ToStdString(Self().GetProperty(Ui::View::Property::ACCESSIBILITY_VALUE));
}

Dali::Accessibility::Role ViewAccessible::GetRole() const
{
  int32_t rawRole = Self().GetProperty<int32_t>(Ui::View::Property::ACCESSIBILITY_ROLE);
  return ConvertRawRoleToAtspiRole(rawRole);
}

std::string ViewAccessible::GetLocalizedRoleName() const
{
  return GetLocaleText(GetRoleName());
}

bool ViewAccessible::IsShowing()
{
  Dali::Actor self = Self();
  if(Dali::EqualsZero(self.GetProperty<Vector4>(Actor::Property::WORLD_COLOR).a) ||
     self.GetProperty<bool>(Dali::DevelActor::Property::CULLED) || !DevelActor::IsEffectivelyVisible(self))
  {
    return false;
  }

  return true;
}

void ViewAccessible::ApplyAccessibilityProps(Dali::Accessibility::States& states)
{
  using Dali::Accessibility::State;
  auto view = Dali::Ui::View::DownCast(Self());

  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  AccessibilityStates viewStates;

  int32_t rawRole = view.GetProperty<int32_t>(Ui::View::Property::ACCESSIBILITY_ROLE);

  bool             isModal       = false;
  TriStateProperty highlightable = TriStateProperty::AUTO;

  const auto* accessibilityData = viewImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& props = accessibilityData->mAccessibilityProps;

    viewStates    = props.states;
    isModal       = props.isModal;
    highlightable = props.isHighlightable;
  }
  else
  {
    // Default states
    // TODO : Couldn't we get this value from
    // Internal::ViewDataImpl::AccessibilityData::GetDefaultViewAccessibilityStates();
    viewStates[AccessibilityState::ENABLED] = true;
  }

  // Apply states
  states[State::ENABLED]  = viewStates[AccessibilityState::ENABLED];
  states[State::SELECTED] = viewStates[AccessibilityState::SELECTED];
  states[State::CHECKED]  = viewStates[AccessibilityState::CHECKED];
  states[State::BUSY]     = viewStates[AccessibilityState::BUSY];
  states[State::EXPANDED] = viewStates[AccessibilityState::EXPANDED];

  // Apply traits
  states[State::MODAL]         = isModal || IsModalRole(rawRole);
  states[State::HIGHLIGHTABLE] = IsHighlightable(highlightable, rawRole);
}

Dali::Accessibility::States ViewAccessible::CalculateStates()
{
  using Dali::Accessibility::State;

  Dali::Actor                 self = Self();
  Dali::Accessibility::States states;

  states[State::FOCUSABLE]   = self.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
  states[State::FOCUSED]     = Ui::KeyboardFocusManager::Get().GetCurrentFocusActor() == self;
  states[State::HIGHLIGHTED] = IsHighlighted();
  states[State::SENSITIVE]   = (Dali::DevelActor::IsHittable(self) && Dali::DevelActor::GetTouchRequired(self));
  states[State::VISIBLE]     = self.GetProperty<bool>(Actor::Property::VISIBLE);
  states[State::SHOWING]     = IsShowing();
  states[State::DEFUNCT]     = !self.GetProperty(Dali::DevelActor::Property::CONNECTED_TO_SCENE).Get<bool>();

  ApplyAccessibilityProps(states);

  return states;
}

Dali::Accessibility::States ViewAccessible::GetStates()
{
  return CalculateStates();
}

Dali::Accessibility::Attributes ViewAccessible::GetAttributes() const
{
  static const std::string automationIdKey = "automationId";
  static const std::string classKey        = "class";

  Accessibility::Attributes result;
  Ui::View                  view           = Ui::View::DownCast(Self());
  Dali::Property::Value     property       = view.GetProperty(View::Property::ACCESSIBILITY_ATTRIBUTES);
  Dali::Property::Map*      attributeMap   = property.GetMap();
  std::size_t               attributeCount = attributeMap ? attributeMap->Count() : 0U;

  for(std::size_t i = 0; i < attributeCount; i++)
  {
    Dali::Property::Key mapKey = attributeMap->GetKeyAt(i);
    std::string         mapValue;

    if(mapKey.type == Dali::Property::Key::STRING && GetStdString(attributeMap->GetValue(i), mapValue))
    {
      result.emplace(ToStdString(mapKey.stringKey), std::move(mapValue));
    }
  }

  auto automationId = ToStdString(view.GetProperty(View::Property::AUTOMATION_ID));
  if(!automationId.empty())
  {
    result.emplace(automationIdKey, std::move(automationId));
  }

  if(auto imageView = Ui::ImageView::DownCast(Self()))
  {
    auto imageSrc = FetchImageSrc(imageView);
    if(!imageSrc.empty())
    {
      result.emplace(ATTR_IMG_SRC_KEY, std::move(imageSrc));
    }
  }

  // Add "class" if not present already
  if(result.find(classKey) == result.end())
  {
    Dali::TypeInfo typeInfo;
    Self().GetTypeInfo(typeInfo);
    if(typeInfo)
    {
      const std::string& typeName = ToStdString(typeInfo.GetName());

      result.emplace(classKey, typeName);

      // Save the 'typeName' so we don't have to calculate it again
      view.AppendAccessibilityAttribute(ToDaliString(classKey), ToDaliString(typeName));
    }
  }

  return result;
}

void ViewAccessible::InitDefaultFeatures()
{
  Dali::Accessibility::ActorAccessible::InitDefaultFeatures();
  AddFeature<Dali::Accessibility::Action>(shared_from_this());
}

bool ViewAccessible::IsHidden() const
{
  auto view = Dali::Ui::View::DownCast(Self());

  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  const auto* accessibilityData = viewImpl.GetAccessibilityData();
  return DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
}

bool ViewAccessible::GrabFocus()
{
  return Ui::KeyboardFocusManager::Get().SetCurrentFocusActor(Self());
}

void ViewAccessible::ScrollToSelf()
{
  auto* child  = this;
  auto* parent = dynamic_cast<Ui::ViewAccessible*>(child->GetParent());

  while(parent)
  {
    if(parent->IsScrollable())
    {
      parent->ScrollToChild(child->Self());
    }

    parent = dynamic_cast<Ui::ViewAccessible*>(parent->GetParent());
  }
}

void ViewAccessible::RegisterPositionPropertyNotification()
{
  auto                    view         = Dali::Ui::View::DownCast(Self());
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  viewImpl.GetOrCreateAccessibilityData().RegisterAccessibilityPositionPropertyNotification();
}

void ViewAccessible::UnregisterPositionPropertyNotification()
{
  auto                    view         = Dali::Ui::View::DownCast(Self());
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  viewImpl.GetOrCreateAccessibilityData().UnregisterAccessibilityPositionPropertyNotification();
}

void ViewAccessible::RegisterPropertySetSignal()
{
  auto                    view         = Dali::Ui::View::DownCast(Self());
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  auto& accessibilityData = viewImpl.GetOrCreateAccessibilityData();
  accessibilityData.RegisterAccessibilityPropertySetSignal();

  mStatesSnapshot = accessibilityData.mAccessibilityProps.states;
}

void ViewAccessible::UnregisterPropertySetSignal()
{
  auto                    view         = Dali::Ui::View::DownCast(Self());
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  viewImpl.GetOrCreateAccessibilityData().UnregisterAccessibilityPropertySetSignal();

  mStatesSnapshot = {};
}

bool ViewAccessible::GrabHighlight()
{
  Dali::Actor self                = Self();
  auto        oldHighlightedActor = GetCurrentlyHighlightedActor();

  if(!Dali::Accessibility::IsUp())
  {
    return false;
  }

  if(self == oldHighlightedActor)
  {
    return true;
  }

  // Clear the old highlight.
  if(oldHighlightedActor)
  {
    auto oldHighlightedObject = Accessible::Get(oldHighlightedActor);
    if(oldHighlightedObject)
    {
      oldHighlightedObject->ClearHighlight();
    }
  }

  auto highlight = GetHighlightActor();
  if(!highlight)
  {
    highlight = CreateHighlightIndicatorActor();
    SetHighlightActor(highlight);
  }

  highlight.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  highlight.SetProperty(Actor::Property::POSITION_Z, 1.0f);
  highlight.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Need to set resize policy again, to update SIZE property which is set by
  // NUIViewAccessible. The highlight could move from NUIViewAccessible to
  // ViewAccessible. In this case, highlight has incorrect size.
  highlight.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  // Remember the highlight actor, so that when the default is changed with
  // SetHighlightActor(), the currently displayed highlight can still be cleared.
  mCurrentHighlightActor = highlight;
  ScrollToSelf();
  self.Add(highlight);
  SetCurrentlyHighlightedActor(self);
  EmitHighlighted(true);
  RegisterPositionPropertyNotification();
  RegisterPropertySetSignal();

  auto view = Dali::Ui::View::DownCast(self);
  if(!view.AccessibilityHighlightedSignal().Empty())
  {
    view.AccessibilityHighlightedSignal().Emit(true);
  }

  mHighlightOverlay.UpdateOverlay(highlight);

  return true;
}

bool ViewAccessible::ClearHighlight()
{
  if(!Dali::Accessibility::IsUp())
  {
    return false;
  }

  if(IsHighlighted())
  {
    Dali::Actor self = Self();
    UnregisterPropertySetSignal();
    UnregisterPositionPropertyNotification();
    self.Remove(mCurrentHighlightActor.GetHandle());
    mCurrentHighlightActor = {};
    SetCurrentlyHighlightedActor({});
    EmitHighlighted(false);
    auto view = Dali::Ui::View::DownCast(self);
    if(!view.AccessibilityHighlightedSignal().Empty())
    {
      view.AccessibilityHighlightedSignal().Emit(false);
    }
    mHighlightOverlay.HideOverlay();
    return true;
  }
  return false;
}

std::string ViewAccessible::GetActionName(size_t index) const
{
  if(index >= GetActionCount())
  {
    return {};
  }

  Dali::TypeInfo type;
  Self().GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return ToStdString(type.GetActionName(index));
}

std::string ViewAccessible::GetLocalizedActionName(size_t index) const
{
  return GetLocaleText(GetActionName(index));
}

std::string ViewAccessible::GetActionDescription(size_t index) const
{
  return {};
}

size_t ViewAccessible::GetActionCount() const
{
  Dali::TypeInfo type;
  Self().GetTypeInfo(type);
  DALI_ASSERT_ALWAYS(type && "no TypeInfo object");
  return type.GetActionCount();
}

std::string ViewAccessible::GetActionKeyBinding(size_t index) const
{
  return {};
}

bool ViewAccessible::DoAction(size_t index)
{
  std::string actionName = GetActionName(index);
  return Self().DoAction(ToDaliStringView(actionName), {});
}

bool ViewAccessible::DoAction(const std::string& name)
{
  return Self().DoAction(ToDaliStringView(name), {});
}

bool ViewAccessible::DoGesture(const Dali::Accessibility::GestureInfo& gestureInfo)
{
  auto view = Dali::Ui::View::DownCast(Self());

  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(view);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  auto* accessibilityData = viewImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData) && !accessibilityData->mAccessibilityDoGestureSignal.Empty())
  {
    auto ret = std::make_pair(gestureInfo, false);
    accessibilityData->mAccessibilityDoGestureSignal.Emit(ret);
    return ret.second;
  }

  return false;
}

std::vector<Dali::Accessibility::Relation> ViewAccessible::GetRelationSet()
{
  auto view = Dali::Ui::View::DownCast(Self());

  return view.GetAccessibilityRelations();
}

std::string ViewAccessible::GetStringProperty(std::string propertyName) const
{
  return Dali::Ui::PropertyBridge::Get().GetStringProperty(Self(), propertyName);
}

bool ViewAccessible::IsScrollable() const
{
  return Self().GetProperty<bool>(Ui::View::Property::ACCESSIBILITY_SCROLLABLE);
}

bool ViewAccessible::ScrollToChild(Actor child)
{
  auto view    = Dali::Ui::View::DownCast(Self());
  bool success = false;

  if(!view.AccessibilityActionSignal().Empty())
  {
    success = view.AccessibilityActionSignal().Emit({Accessibility::ActionType::SCROLL_TO_CHILD, child});
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: scrollToChild, success : %d\n", success);
  }

  return success;
}

Dali::Property::Index ViewAccessible::GetNamePropertyIndex()
{
  return Actor::Property::NAME;
}

Dali::Property::Index ViewAccessible::GetDescriptionPropertyIndex()
{
  return Dali::Property::INVALID_INDEX;
}

void ViewAccessible::SetLastPosition(Vector2 position)
{
  mLastPosition = position;
}

Vector2 ViewAccessible::GetLastPosition() const
{
  return mLastPosition;
}

void ViewAccessible::OnStatePropertySet(AccessibilityStates newStates)
{
  int32_t rawRole = Self().GetProperty<int32_t>(Ui::View::Property::ACCESSIBILITY_ROLE);
  if(IsRoleV2(rawRole))
  {
    AccessibilityRole role = static_cast<AccessibilityRole>(rawRole);

    if(newStates[AccessibilityState::CHECKED] != mStatesSnapshot[AccessibilityState::CHECKED] &&
       (role == AccessibilityRole::CHECK_BOX || role == AccessibilityRole::RADIO_BUTTON ||
        role == AccessibilityRole::TOGGLE_BUTTON))
    {
      EmitStateChanged(Accessibility::State::CHECKED, newStates[AccessibilityState::CHECKED]);
    }

    if(newStates[AccessibilityState::SELECTED] != mStatesSnapshot[AccessibilityState::SELECTED] &&
       (role == AccessibilityRole::BUTTON || role == AccessibilityRole::LIST_ITEM ||
        role == AccessibilityRole::MENU_ITEM || role == AccessibilityRole::TAB ||
        role == AccessibilityRole::SCROLL_BAR))
    {
      EmitStateChanged(Accessibility::State::SELECTED, newStates[AccessibilityState::SELECTED]);
    }
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                  "With V1 role, state change events are emitted manually by the app component.");
  }

  mStatesSnapshot = newStates;
}

bool ViewAccessible::IsModal(Actor actor)
{
  bool isModalPropertySet = actor.GetProperty<bool>(Ui::View::Property::ACCESSIBILITY_IS_MODAL);
  if(isModalPropertySet)
  {
    return true;
  }

  int32_t rawRole = actor.GetProperty<int32_t>(Ui::View::Property::ACCESSIBILITY_ROLE);
  return IsModalRole(rawRole);
}

bool ViewAccessible::IsScene3D(Actor actor)
{
  int32_t rawRole = static_cast<uint32_t>(AccessibilityRole::MAX_COUNT);
  if(actor.GetProperty(Ui::View::Property::ACCESSIBILITY_ROLE).Get(rawRole))
  {
    return IsScene3DRole(rawRole);
  }
  return false;
}

void ViewAccessible::SetCustomHighlightOverlay(Vector2 position, Vector2 size)
{
  mHighlightOverlay.SetCustomHighlight(position, size);
}

void ViewAccessible::ResetCustomHighlightOverlay()
{
  mHighlightOverlay.ResetCustomHighlight();
}
} // namespace Dali::Ui

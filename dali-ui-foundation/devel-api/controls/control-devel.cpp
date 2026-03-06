/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "control-devel.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/control/control-accessibility-data.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali-ui-foundation/public-api/controls/control.h>

namespace
{
Dali::Ui::Internal::Control::Impl& GetControlImplementation(Dali::Ui::Control control)
{
  auto& internalControl = Dali::Ui::Internal::GetImplementation(control);

  return Dali::Ui::Internal::Control::Impl::Get(internalControl);
}

} // unnamed namespace

namespace Dali
{
namespace Ui
{
namespace DevelControl
{
void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Ui::Visual::Base& visual)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, depthIndex);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled,
                    int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled, depthIndex);
}

void UnregisterVisual(Internal::Control& control, Dali::Property::Index index)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.UnregisterVisual(index);
}

Ui::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.GetVisual(index);
}

void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.EnableVisual(index, enable);
}

bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.IsVisualEnabled(index);
}

void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId,
              const Dali::Property::Value& attributes)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void DoActionExtension(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                       const Dali::Any& attributes)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoActionExtension(visualIndex, actionId, attributes);
}

void EnableCornerPropertiesOverridden(Internal::Control& control, Visual::Base& visual, bool enable,
                                      Dali::Constraint cornerRadiusConstraint)
{
  Internal::Control::Impl::Get(control).EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
}

void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext)
{
  Internal::Control::Impl::Get(control).SetInputMethodContext(inputMethodContext);
}

VisualEventSignalType& VisualEventSignal(Control control)
{
  Internal::Control& internalControl = Ui::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

Dali::Property GetVisualProperty(Control control, Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Internal::Control& internalControl = Ui::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.GetVisualProperty(index, visualPropertyKey);
}

Ui::DevelControl::AccessibilityActivateSignalType& AccessibilityActivateSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityActivateSignal;
}

Ui::DevelControl::AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal;
}

Ui::DevelControl::AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal;
}

Ui::DevelControl::AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal;
}

Ui::DevelControl::AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal;
}

Ui::DevelControl::AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal;
}

Ui::DevelControl::AccessibilityGetNameSignalType& AccessibilityGetNameSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityGetNameSignal;
}

Ui::DevelControl::AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityGetDescriptionSignal;
}

Ui::DevelControl::AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityDoGestureSignal;
}

Ui::DevelControl::AccessibilityActionSignalType& AccessibilityActionSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityActionSignal;
}

Ui::DevelControl::AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal(Ui::Control control)
{
  return GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityHighlightedSignal;
}

void AppendAccessibilityRelation(Ui::Control control, Dali::Actor destination,
                                 Dali::Accessibility::RelationType relation)
{
  if (auto destinationAccessible = Accessibility::Accessible::Get(destination))
  {
    GetControlImplementation(control).GetOrCreateAccessibilityData().mAccessibilityProps.relations[relation].insert(
        destinationAccessible);
  }
}

void RemoveAccessibilityRelation(Ui::Control control, Dali::Actor destination,
                                 Dali::Accessibility::RelationType relation)
{
  auto& controlImpl = GetControlImplementation(control);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    if (auto destinationAccessible = Accessibility::Accessible::Get(destination))
    {
      auto& relations = accessibilityData->mAccessibilityProps.relations;

      relations[relation].erase(destinationAccessible);

      if (relations[relation].empty())
      {
        relations.erase(relation);
      }
    }
  }
}

std::vector<Accessibility::Relation> GetAccessibilityRelations(Ui::Control control)
{
  std::vector<Accessibility::Relation> result;

  auto& controlImpl = GetControlImplementation(control);

  const auto* accessibilityData = controlImpl.GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for (const auto& relation : relations)
    {
      const auto& targets = relation.second;

      result.emplace_back(Accessibility::Relation{relation.first, {}});
      std::copy(targets.begin(), targets.end(), std::back_inserter(result.back().mTargets));
    }
  }

  return result;
}

void ClearAccessibilityRelations(Ui::Control control)
{
  auto& controlImpl = GetControlImplementation(control);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if (DALI_LIKELY(accessibilityData))
  {
    accessibilityData->mAccessibilityProps.relations.clear();
  }
}

void AppendAccessibilityAttribute(Ui::Control control, const std::string& key, const std::string& value)
{
  GetControlImplementation(control).AppendAccessibilityAttribute(key, value);
}

void RemoveAccessibilityAttribute(Ui::Control control, const std::string& key)
{
  GetControlImplementation(control).RemoveAccessibilityAttribute(key);
}

void ClearAccessibilityAttributes(Ui::Control control)
{
  GetControlImplementation(control).ClearAccessibilityAttributes();
}

void SetAccessibilityReadingInfoType(Ui::Control control, const Dali::Accessibility::ReadingInfoTypes types)
{
  GetControlImplementation(control).SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(Ui::Control control)
{
  return GetControlImplementation(control).GetAccessibilityReadingInfoType();
}

bool ClearAccessibilityHighlight(Ui::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if (DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->ClearHighlight();
  }
  return false;
}

bool GrabAccessibilityHighlight(Ui::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if (DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GrabHighlight();
  }
  return false;
}

Dali::Accessibility::States GetAccessibilityStates(Ui::Control control)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if (DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GetStates();
  }
  return Dali::Accessibility::States{};
}

void NotifyAccessibilityStateChange(Ui::Control control, Dali::Accessibility::States states, bool recurse)
{
  auto controlAccessible = GetControlImplementation(control).GetAccessibleObject();
  if (DALI_LIKELY(controlAccessible))
  {
    controlAccessible->NotifyAccessibilityStateChange(std::move(states), recurse);
  }
}

bool IsAccessibleCreated(Ui::Control control)
{
  return GetControlImplementation(control).IsAccessibleCreated();
}

void EnableCreateAccessible(Ui::Control control, bool enable)
{
  GetControlImplementation(control).EnableCreateAccessible(enable);
}

bool IsCreateAccessibleEnabled(Ui::Control control)
{
  return GetControlImplementation(control).IsCreateAccessibleEnabled();
}

void EmitAccessibilityStateChanged(Dali::Actor actor, Accessibility::State state, int newValue)
{
  auto bridge = Accessibility::Bridge::GetCurrentBridge();
  auto control = Ui::Control::DownCast(actor);
  if (DALI_LIKELY(control && bridge))
  {
    if (state == Accessibility::State::SHOWING)
    {
      bool isModal = ControlAccessible::IsModal(control);
      if (isModal)
      {
        if (newValue == 1)
        {
          bridge->RegisterDefaultLabel(actor);
        }
        else
        {
          bridge->UnregisterDefaultLabel(actor);
        }
      }
    }
  }

  if (bridge && bridge->IsUp())
  {
    auto accessible = dynamic_cast<Accessibility::ActorAccessible*>(Accessibility::Accessible::Get(actor));
    if (DALI_LIKELY(accessible))
    {
      accessible->EmitStateChanged(state, newValue, 0);
    }
  }
}

} // namespace DevelControl

} // namespace Ui

} // namespace Dali

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
#include <dali-ui-foundation/public-api/controls/control.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/control-accessible.h>
#include <dali-ui-foundation/internal/controls/control/control-accessibility-data.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/internal/controls/control/control-renderers.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>

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
Control Control::New()
{
  return Ui::Internal::Control::New();
}

Control Control::New(ControlBehaviour additionalBehaviour)
{
  return Ui::Internal::Control::New(static_cast<Ui::Internal::Control::ControlBehaviour>(additionalBehaviour));
}

Control::Control()
{
}

Control::Control(const Control& uiControl) = default;

Control::Control(Control&& rhs) noexcept = default;

Control::~Control()
{
}

Control& Control::operator=(const Control& handle) = default;

Control& Control::operator=(Control&& rhs) noexcept = default;

Control Control::DownCast(BaseHandle handle)
{
  return DownCast<Control, Internal::Control>(handle);
}

void Control::SetKeyInputFocus()
{
  Internal::GetImplementation(*this).SetKeyInputFocus();
}

bool Control::HasKeyInputFocus()
{
  return Internal::GetImplementation(*this).HasKeyInputFocus();
}

void Control::ClearKeyInputFocus()
{
  Internal::GetImplementation(*this).ClearKeyInputFocus();
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return Internal::GetImplementation(*this).GetPinchGestureDetector();
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return Internal::GetImplementation(*this).GetPanGestureDetector();
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return Internal::GetImplementation(*this).GetTapGestureDetector();
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return Internal::GetImplementation(*this).GetLongPressGestureDetector();
}

void Control::SetBackgroundColor(const Vector4& color)
{
  Internal::GetImplementation(*this).SetBackgroundColor(color);
}

void Control::ClearBackground()
{
  Internal::GetImplementation(*this).ClearBackground();
}

void Control::SetRenderEffect(Ui::RenderEffect effect)
{
  Internal::GetImplementation(*this).SetRenderEffect(effect);
}

Ui::RenderEffect Control::GetRenderEffect() const
{
  return Internal::GetImplementation(*this).GetRenderEffect();
}

void Control::ClearRenderEffect()
{
  Internal::GetImplementation(*this).ClearRenderEffect();
}

Dali::Renderer Control::CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc);
}

Dali::Renderer Control::CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Shader::Hint::Value hints,
                                       const std::string& shaderName, Uint16Pair gridSize)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc, hints, shaderName, gridSize);
}

void Control::SetRendererTexture(Renderer renderer, Texture texture)
{
  Internal::SetRendererTexture(renderer, texture);
}

void Control::SetRendererTexture(Renderer renderer, FrameBuffer frameBuffer)
{
  Internal::SetRendererTexture(renderer, frameBuffer);
}

bool Control::IsResourceReady() const
{
  return Internal::GetImplementation(*this).IsResourceReady();
}

Visual::ResourceStatus Control::GetVisualResourceStatus(const Dali::Property::Index index)
{
  const Internal::Control&       internalControl = Ui::Internal::GetImplementation(*this);
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.GetVisualResourceStatus(index);
}

Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return Internal::GetImplementation(*this).KeyEventSignal();
}

Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return Internal::GetImplementation(*this).KeyInputFocusGainedSignal();
}

Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return Internal::GetImplementation(*this).KeyInputFocusLostSignal();
}

Control::ResourceReadySignalType& Control::ResourceReadySignal()
{
  Internal::Control&       internalControl = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  return controlImpl.mResourceReadySignal;
}

Control::OffScreenRenderingFinishedSignalType& Control::OffScreenRenderingFinishedSignal()
{
  Internal::Control&       internalControl = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlImpl     = Internal::Control::Impl::Get(internalControl);

  return controlImpl.mOffScreenRenderingFinishedSignal;
}

// Visual Management

void Control::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).RegisterVisual(index, visual);
}

void Control::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).RegisterVisual(index, visual, depthIndex);
}

void Control::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).RegisterVisual(index, visual, enabled);
}

void Control::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).RegisterVisual(index, visual, enabled, depthIndex);
}

void Control::UnregisterVisual(Dali::Property::Index index)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).UnregisterVisual(index);
}

Ui::Visual::Base Control::GetVisual(Dali::Property::Index index)
{
  const Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  return Internal::Control::Impl::Get(controlInternal).GetVisual(index);
}

void Control::EnableVisual(Dali::Property::Index index, bool enable)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).EnableVisual(index, enable);
}

bool Control::IsVisualEnabled(Dali::Property::Index index)
{
  const Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  return Internal::Control::Impl::Get(controlInternal).IsVisualEnabled(index);
}

void Control::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                       const Dali::Property::Value& attributes)
{
  Internal::Control&       controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void Control::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                const Dali::Any& attributes)
{
  Internal::Control&       controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoActionExtension(visualIndex, actionId, attributes);
}

void Control::EnableCornerPropertiesOverridden(Visual::Base& visual, bool enable,
                                               Dali::Constraint cornerRadiusConstraint)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal)
    .EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
}

void Control::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  Internal::Control& controlInternal = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl::Get(controlInternal).SetInputMethodContext(inputMethodContext);
}

Ui::Control::VisualEventSignalType& Control::VisualEventSignal()
{
  Internal::Control&       internalControl = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

Dali::Property Control::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Internal::Control&       internalControl = Ui::Internal::GetImplementation(*this);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.GetVisualProperty(index, visualPropertyKey);
}

// Accessibility

Control::AccessibilityActivateSignalType& Control::AccessibilityActivateSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityActivateSignal;
}

Control::AccessibilityReadingSkippedSignalType& Control::AccessibilityReadingSkippedSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal;
}

Control::AccessibilityReadingPausedSignalType& Control::AccessibilityReadingPausedSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal;
}

Control::AccessibilityReadingResumedSignalType& Control::AccessibilityReadingResumedSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal;
}

Control::AccessibilityReadingCancelledSignalType& Control::AccessibilityReadingCancelledSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal;
}

Control::AccessibilityReadingStoppedSignalType& Control::AccessibilityReadingStoppedSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal;
}

Control::AccessibilityGetNameSignalType& Control::AccessibilityGetNameSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityGetNameSignal;
}

Control::AccessibilityGetDescriptionSignalType& Control::AccessibilityGetDescriptionSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityGetDescriptionSignal;
}

Control::AccessibilityDoGestureSignalType& Control::AccessibilityDoGestureSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityDoGestureSignal;
}

Control::AccessibilityActionSignalType& Control::AccessibilityActionSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityActionSignal;
}

Control::AccessibilityHighlightedSignalType& Control::AccessibilityHighlightedSignal()
{
  return GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityHighlightedSignal;
}

void Control::AppendAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  if(auto destinationAccessible = Accessibility::Accessible::Get(destination))
  {
    GetControlImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityProps.relations[relation].insert(
      destinationAccessible);
  }
}

void Control::RemoveAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  auto& controlImpl = GetControlImplementation(*this);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    if(auto destinationAccessible = Accessibility::Accessible::Get(destination))
    {
      auto& relations = accessibilityData->mAccessibilityProps.relations;

      relations[relation].erase(destinationAccessible);

      if(relations[relation].empty())
      {
        relations.erase(relation);
      }
    }
  }
}

std::vector<Accessibility::Relation> Control::GetAccessibilityRelations()
{
  std::vector<Accessibility::Relation> result;

  auto& controlImpl = GetControlImplementation(*this);

  const auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for(const auto& relation : relations)
    {
      const auto& targets = relation.second;

      result.emplace_back(Accessibility::Relation{relation.first, {}});
      std::copy(targets.begin(), targets.end(), std::back_inserter(result.back().mTargets));
    }
  }

  return result;
}

void Control::ClearAccessibilityRelations()
{
  auto& controlImpl = GetControlImplementation(*this);

  auto* accessibilityData = controlImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->mAccessibilityProps.relations.clear();
  }
}

void Control::AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value)
{
  GetControlImplementation(*this).AppendAccessibilityAttribute(key, value);
}

void Control::RemoveAccessibilityAttribute(const Dali::String& key)
{
  GetControlImplementation(*this).RemoveAccessibilityAttribute(key);
}

void Control::ClearAccessibilityAttributes()
{
  GetControlImplementation(*this).ClearAccessibilityAttributes();
}

void Control::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  GetControlImplementation(*this).SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes Control::GetAccessibilityReadingInfoType()
{
  return GetControlImplementation(*this).GetAccessibilityReadingInfoType();
}

bool Control::ClearAccessibilityHighlight()
{
  auto controlAccessible = GetControlImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->ClearHighlight();
  }
  return false;
}

bool Control::GrabAccessibilityHighlight()
{
  auto controlAccessible = GetControlImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GrabHighlight();
  }
  return false;
}

Dali::Accessibility::States Control::GetAccessibilityStates()
{
  auto controlAccessible = GetControlImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    return controlAccessible->GetStates();
  }
  return Dali::Accessibility::States{};
}

void Control::NotifyAccessibilityStateChange(Dali::Accessibility::States states, bool recurse)
{
  auto controlAccessible = GetControlImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(controlAccessible))
  {
    controlAccessible->NotifyAccessibilityStateChange(std::move(states), recurse);
  }
}

bool Control::IsAccessibleCreated()
{
  return GetControlImplementation(*this).IsAccessibleCreated();
}

void Control::EnableCreateAccessible(bool enable)
{
  GetControlImplementation(*this).EnableCreateAccessible(enable);
}

bool Control::IsCreateAccessibleEnabled()
{
  return GetControlImplementation(*this).IsCreateAccessibleEnabled();
}

void Control::EmitAccessibilityStateChanged(Accessibility::State state, int newValue)
{
  GetControlImplementation(*this).EmitAccessibilityStateChanged(state, newValue);
}

Control::Control(Internal::Control& implementation)
: CustomActor(implementation)
{
}

Control::Control(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<Internal::Control>(internal);
}

} // namespace Ui

} // namespace Dali
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
#include <dali-ui-foundation/public-api/view.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/control-accessible.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/public-api/clickable-trait.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace
{
Dali::Ui::Internal::ViewDataImpl& GetViewImplementation(Dali::Ui::View view)
{
  auto& internalView = Dali::Ui::Integration::GetImpl(view);

  return Dali::Ui::Internal::ViewDataImpl::Get(internalView);
}

} // unnamed namespace

namespace Dali
{

namespace Ui
{

View::View()
{
}

View View::New()
{
  Integration::ViewImplPtr impl = Integration::ViewImpl::New();

  // Pass ownership to handle
  View handle(*impl);

  impl->Initialize();

  return handle;
}

View::View(const View& view) = default;

View::View(View&& rhs) noexcept = default;

View::~View()
{
}

View View::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<View, Integration::ViewImpl>(handle);
}

View::View(Integration::ViewImpl& implementation)
: CustomActor(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<Integration::ViewImpl>(internal);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize View::Measure(float widthConstraint, float heightConstraint)
{
  return Integration::GetImpl(*this).Measure(widthConstraint, heightConstraint);
}

MeasuredSize View::Arrange(const LayoutRect& bounds)
{
  return Integration::GetImpl(*this).Arrange(bounds);
}

void View::InvalidateMeasure()
{
  Integration::GetImpl(*this).InvalidateMeasure();
}

void View::InvalidateArrange()
{
  Integration::GetImpl(*this).InvalidateArrange();
}

MeasuredSize View::GetDesiredSize() const
{
  return Integration::GetImpl(*this).GetDesiredSize();
}

bool View::IsMeasureValid() const
{
  return Integration::GetImpl(*this).IsMeasureValid();
}

bool View::IsArrangeValid() const
{
  return Integration::GetImpl(*this).IsArrangeValid();
}

// =============================================================================
// Properties
// =============================================================================

View& View::SetSizeWidth(float width)
{
  Integration::GetImpl(*this).SetSizeWidth(width);
  return *this;
}

float View::GetSizeWidth() const
{
  return Integration::GetImpl(*this).GetSizeWidth();
}

View& View::SetSizeHeight(float height)
{
  Integration::GetImpl(*this).SetSizeHeight(height);
  return *this;
}

float View::GetSizeHeight() const
{
  return Integration::GetImpl(*this).GetSizeHeight();
}

View& View::SetPositionX(float x)
{
  Integration::GetImpl(*this).SetPositionX(x);
  return *this;
}

float View::GetPositionX() const
{
  return Integration::GetImpl(*this).GetPositionX();
}

View& View::SetPositionY(float y)
{
  Integration::GetImpl(*this).SetPositionY(y);
  return *this;
}

float View::GetPositionY() const
{
  return Integration::GetImpl(*this).GetPositionY();
}

View& View::SetParentOrigin(const Vector3& point)
{
  Integration::GetImpl(*this).SetParentOrigin(point);
  return *this;
}

Vector3 View::GetParentOrigin() const
{
  return Integration::GetImpl(*this).GetParentOrigin();
}

View& View::SetPivotPoint(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivotPoint(point);
  return *this;
}

Vector3 View::GetPivotPoint() const
{
  return Integration::GetImpl(*this).GetPivotPoint();
}

View& View::SetLayoutWidth(float width)
{
  Integration::GetImpl(*this).SetLayoutWidth(width);
  return *this;
}

float View::GetLayoutWidth() const
{
  return Integration::GetImpl(*this).GetLayoutWidth();
}

View& View::SetLayoutHeight(float height)
{
  Integration::GetImpl(*this).SetLayoutHeight(height);
  return *this;
}

float View::GetLayoutHeight() const
{
  return Integration::GetImpl(*this).GetLayoutHeight();
}

View& View::SetMinimumWidth(float width)
{
  Integration::GetImpl(*this).SetMinimumWidth(width);
  return *this;
}

float View::GetMinimumWidth() const
{
  return Integration::GetImpl(*this).GetMinimumWidth();
}

View& View::SetMinimumHeight(float height)
{
  Integration::GetImpl(*this).SetMinimumHeight(height);
  return *this;
}

float View::GetMinimumHeight() const
{
  return Integration::GetImpl(*this).GetMinimumHeight();
}

View& View::SetMaximumWidth(float width)
{
  Integration::GetImpl(*this).SetMaximumWidth(width);
  return *this;
}

float View::GetMaximumWidth() const
{
  return Integration::GetImpl(*this).GetMaximumWidth();
}

View& View::SetMaximumHeight(float height)
{
  Integration::GetImpl(*this).SetMaximumHeight(height);
  return *this;
}

float View::GetMaximumHeight() const
{
  return Integration::GetImpl(*this).GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

View& View::SetViewMargin(const Extents& margin)
{
  Integration::GetImpl(*this).SetViewMargin(margin);
  return *this;
}

Extents View::GetViewMargin() const
{
  return Integration::GetImpl(*this).GetViewMargin();
}

View& View::SetViewPadding(const Extents& padding)
{
  Integration::GetImpl(*this).SetViewPadding(padding);
  return *this;
}

Extents View::GetViewPadding() const
{
  return Integration::GetImpl(*this).GetViewPadding();
}

View& View::SetHorizontalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetHorizontalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetHorizontalAlignment() const
{
  return Integration::GetImpl(*this).GetHorizontalAlignment();
}

View& View::SetVerticalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetVerticalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetVerticalAlignment() const
{
  return Integration::GetImpl(*this).GetVerticalAlignment();
}

bool View::IsFocusable() const
{
  return Integration::GetImpl(*this).IsFocusable();
}

View& View::SetFocusable(bool focusable)
{
  Integration::GetImpl(*this).SetFocusable(focusable);
  return *this;
}

bool View::IsTouchFocusable() const
{
  return Integration::GetImpl(*this).IsTouchFocusable();
}

View& View::SetTouchFocusable(bool touchFocusable)
{
  Integration::GetImpl(*this).SetTouchFocusable(touchFocusable);
  return *this;
}

View& View::SetBackgroundColor(const UiColor& color)
{
  Integration::GetImpl(*this).SetBackgroundColor(color);
  return *this;
}

ClickableTrait View::GetOrAttachClickableTrait()
{
  auto&                      impl = Integration::GetImpl(*this);
  const Integration::TraitId interactionTraitId(Integration::ReservedTraitId::INTERACTION_TRAIT);
  Trait                      existing = impl.GetTrait(interactionTraitId);

  if(!existing)
  {
    ClickableTrait clickable = ClickableTrait::New();
    impl.SetTrait(interactionTraitId, clickable);
    return clickable;
  }

  ClickableTrait clickable = ClickableTrait::DownCast(existing);
  if(!clickable)
  {
    DALI_ASSERT_ALWAYS(false && "View already has a different interaction trait; cannot attach ClickableTrait");
    return ClickableTrait();
  }

  return clickable;
}

ClickableTrait View::GetClickableTrait() const
{
  const auto& impl  = Integration::GetImpl(*this);
  Trait       trait = impl.GetTrait(Integration::TraitId(Integration::ReservedTraitId::INTERACTION_TRAIT));
  return ClickableTrait::DownCast(trait);
}

BaseHandle View::GetLayoutParamsTrait(LayoutParamsType type) const
{
  return Integration::GetImpl(*this).GetLayoutParamsTrait(type);
}

View& View::SetLayoutParams(LayoutParams params)
{
  Integration::GetImpl(*this).SetLayoutParams(params);
  return *this;
}

// From control.cpp
void View::SetKeyInputFocus()
{
  Integration::GetImpl(*this).SetKeyInputFocus();
}

bool View::HasKeyInputFocus()
{
  return Integration::GetImpl(*this).HasKeyInputFocus();
}

void View::ClearKeyInputFocus()
{
  Integration::GetImpl(*this).ClearKeyInputFocus();
}

PinchGestureDetector View::GetPinchGestureDetector() const
{
  return Integration::GetImpl(*this).GetPinchGestureDetector();
}

PanGestureDetector View::GetPanGestureDetector() const
{
  return Integration::GetImpl(*this).GetPanGestureDetector();
}

TapGestureDetector View::GetTapGestureDetector() const
{
  return Integration::GetImpl(*this).GetTapGestureDetector();
}

LongPressGestureDetector View::GetLongPressGestureDetector() const
{
  return Integration::GetImpl(*this).GetLongPressGestureDetector();
}

void View::SetBackgroundColor(const Vector4& color)
{
  Integration::GetImpl(*this).SetBackgroundColor(color);
}

void View::ClearBackground()
{
  Integration::GetImpl(*this).ClearBackground();
}

void View::SetRenderEffect(Ui::RenderEffect effect)
{
  Integration::GetImpl(*this).SetRenderEffect(effect);
}

Ui::RenderEffect View::GetRenderEffect() const
{
  return Integration::GetImpl(*this).GetRenderEffect();
}

void View::ClearRenderEffect()
{
  Integration::GetImpl(*this).ClearRenderEffect();
}

Dali::Renderer View::CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc);
}

Dali::Renderer View::CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Shader::Hint::Value hints,
                                    const std::string& shaderName, Uint16Pair gridSize)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc, hints, shaderName, gridSize);
}

void View::SetRendererTexture(Renderer renderer, Texture texture)
{
  Internal::SetRendererTexture(renderer, texture);
}

void View::SetRendererTexture(Renderer renderer, FrameBuffer frameBuffer)
{
  Internal::SetRendererTexture(renderer, frameBuffer);
}

bool View::IsResourceReady() const
{
  return Integration::GetImpl(*this).IsResourceReady();
}

Visual::ResourceStatus View::GetVisualResourceStatus(const Dali::Property::Index index)
{
  const Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(*this);
  const Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(internalView);
  return viewDataImpl.GetVisualResourceStatus(index);
}

View::KeyEventSignalType& View::KeyEventSignal()
{
  return Integration::GetImpl(*this).KeyEventSignal();
}

View::KeyInputFocusSignalType& View::KeyInputFocusGainedSignal()
{
  return Integration::GetImpl(*this).KeyInputFocusGainedSignal();
}

View::KeyInputFocusSignalType& View::KeyInputFocusLostSignal()
{
  return Integration::GetImpl(*this).KeyInputFocusLostSignal();
}

View::ResourceReadySignalType& View::ResourceReadySignal()
{
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  return viewImpl.mResourceReadySignal;
}

View::OffScreenRenderingFinishedSignalType& View::OffScreenRenderingFinishedSignal()
{
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewImpl     = Internal::ViewDataImpl::Get(internalView);

  return viewImpl.mOffScreenRenderingFinishedSignal;
}

// Visual Management

void View::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).RegisterVisual(index, visual);
}

void View::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).RegisterVisual(index, visual, depthIndex);
}

void View::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).RegisterVisual(index, visual, enabled);
}

void View::RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).RegisterVisual(index, visual, enabled, depthIndex);
}

void View::UnregisterVisual(Dali::Property::Index index)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).UnregisterVisual(index);
}

Ui::Visual::Base View::GetVisual(Dali::Property::Index index)
{
  const Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  return Internal::ViewDataImpl::Get(viewInternal).GetVisual(index);
}

void View::EnableVisual(Dali::Property::Index index, bool enable)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).EnableVisual(index, enable);
}

bool View::IsVisualEnabled(Dali::Property::Index index)
{
  const Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  return Internal::ViewDataImpl::Get(viewInternal).IsVisualEnabled(index);
}

void View::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                    const Dali::Property::Value& attributes)
{
  Integration::ViewImpl&  viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewInternal);
  viewDataImpl.DoAction(visualIndex, actionId, attributes);
}

void View::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                             const Dali::Any& attributes)
{
  Integration::ViewImpl&  viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewInternal);
  viewDataImpl.DoActionExtension(visualIndex, actionId, attributes);
}

void View::EnableCornerPropertiesOverridden(Visual::Base& visual, bool enable,
                                            Dali::Constraint cornerRadiusConstraint)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal)
    .EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
}

void View::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  Integration::ViewImpl& viewInternal = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl::Get(viewInternal).SetInputMethodContext(inputMethodContext);
}

Ui::View::VisualEventSignalType& View::VisualEventSignal()
{
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(internalView);
  return viewDataImpl.VisualEventSignal();
}

Dali::Property View::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  Integration::ViewImpl&  internalView = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(internalView);
  return viewDataImpl.GetVisualProperty(index, visualPropertyKey);
}

// Accessibility

View::AccessibilityActivateSignalType& View::AccessibilityActivateSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityActivateSignal;
}

View::AccessibilityReadingSkippedSignalType& View::AccessibilityReadingSkippedSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal;
}

View::AccessibilityReadingPausedSignalType& View::AccessibilityReadingPausedSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal;
}

View::AccessibilityReadingResumedSignalType& View::AccessibilityReadingResumedSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal;
}

View::AccessibilityReadingCancelledSignalType& View::AccessibilityReadingCancelledSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal;
}

View::AccessibilityReadingStoppedSignalType& View::AccessibilityReadingStoppedSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal;
}

View::AccessibilityGetNameSignalType& View::AccessibilityGetNameSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityGetNameSignal;
}

View::AccessibilityGetDescriptionSignalType& View::AccessibilityGetDescriptionSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityGetDescriptionSignal;
}

View::AccessibilityDoGestureSignalType& View::AccessibilityDoGestureSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityDoGestureSignal;
}

View::AccessibilityActionSignalType& View::AccessibilityActionSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityActionSignal;
}

View::AccessibilityHighlightedSignalType& View::AccessibilityHighlightedSignal()
{
  return GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityHighlightedSignal;
}

void View::AppendAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  if(auto destinationAccessible = Accessibility::Accessible::Get(destination))
  {
    GetViewImplementation(*this).GetOrCreateAccessibilityData().mAccessibilityProps.relations[relation].insert(
      destinationAccessible);
  }
}

void View::RemoveAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation)
{
  auto& viewImpl = GetViewImplementation(*this);

  auto* accessibilityData = viewImpl.GetAccessibilityData();
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

std::vector<Accessibility::Relation> View::GetAccessibilityRelations()
{
  std::vector<Accessibility::Relation> result;

  auto& viewImpl = GetViewImplementation(*this);

  const auto* accessibilityData = viewImpl.GetAccessibilityData();
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

void View::ClearAccessibilityRelations()
{
  auto& viewImpl = GetViewImplementation(*this);

  auto* accessibilityData = viewImpl.GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->mAccessibilityProps.relations.clear();
  }
}

void View::AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value)
{
  GetViewImplementation(*this).AppendAccessibilityAttribute(key, value);
}

void View::RemoveAccessibilityAttribute(const Dali::String& key)
{
  GetViewImplementation(*this).RemoveAccessibilityAttribute(key);
}

void View::ClearAccessibilityAttributes()
{
  GetViewImplementation(*this).ClearAccessibilityAttributes();
}

void View::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  GetViewImplementation(*this).SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes View::GetAccessibilityReadingInfoType()
{
  return GetViewImplementation(*this).GetAccessibilityReadingInfoType();
}

bool View::ClearAccessibilityHighlight()
{
  auto viewAccessible = GetViewImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(viewAccessible))
  {
    return viewAccessible->ClearHighlight();
  }
  return false;
}

bool View::GrabAccessibilityHighlight()
{
  auto viewAccessible = GetViewImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(viewAccessible))
  {
    return viewAccessible->GrabHighlight();
  }
  return false;
}

Dali::Accessibility::States View::GetAccessibilityStates()
{
  auto viewAccessible = GetViewImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(viewAccessible))
  {
    return viewAccessible->GetStates();
  }
  return Dali::Accessibility::States{};
}

void View::NotifyAccessibilityStateChange(Dali::Accessibility::States states, bool recurse)
{
  auto viewAccessible = GetViewImplementation(*this).GetAccessibleObject();
  if(DALI_LIKELY(viewAccessible))
  {
    viewAccessible->NotifyAccessibilityStateChange(std::move(states), recurse);
  }
}

bool View::IsAccessibleCreated()
{
  return GetViewImplementation(*this).IsAccessibleCreated();
}

void View::EnableCreateAccessible(bool enable)
{
  GetViewImplementation(*this).EnableCreateAccessible(enable);
}

bool View::IsCreateAccessibleEnabled()
{
  return GetViewImplementation(*this).IsCreateAccessibleEnabled();
}

void View::EmitAccessibilityStateChanged(Accessibility::State state, int newValue)
{
  GetViewImplementation(*this).EmitAccessibilityStateChanged(state, newValue);
}

} // namespace Ui

} // namespace Dali

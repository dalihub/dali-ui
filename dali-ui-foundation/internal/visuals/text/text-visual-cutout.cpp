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
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/base-object-observer.h>
#include <dali/integration-api/constraint-integ.h>
#include <memory>
#include <optional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
/**
 * @brief Keeps the source alive so unbinding can queue removal before the uniform reset.
 *
 * Source destruction would disconnect the event-side constraint too early.
 * Binding destruction releases the source on the event thread; update functors
 * can outlive it with an empty observer. Neither the visual nor target renderer is retained.
 * Cloning the constraint shares this guard; it does not observe the new binding.
 */
class CutoutBackgroundSource : public BaseObjectObserver
{
public:
  CutoutBackgroundSource(Constraint binding, Handle source)
  : BaseObjectObserver(binding),
    mSource(source)
  {
    StartObservingDestruction();
  }

  void ObjectDestroyed() override
  {
    mSource.Reset();
  }

private:
  Handle mSource; ///< Source property owner, released on the event thread with the binding.
};

void CutoutBackgroundConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  current = inputs[0]->GetVector4();
  current.r *= current.a;
  current.g *= current.a;
  current.b *= current.a;
}
} // namespace

void TextVisual::SetCutoutBackground(Ui::Integration::Visual::Base visual, Ui::Integration::Visual::Base background)
{
  GetVisualObject(visual).SetCutoutBackground(background);
}

void TextVisual::SetCutoutBackground(Ui::Integration::Visual::Base background)
{
  Handle          sourceObject;
  Property::Index sourceIndex = Property::INVALID_INDEX;
  if(background && (mIsCutoutRenderer || IsCutoutRenderingEnabled()) &&
     Ui::GetImplementation(background).GetType() == Ui::Integration::InternalVisualType::COLOR)
  {
    auto source  = background.GetPropertyObject(Ui::Integration::Visual::Property::MIX_COLOR);
    sourceObject = source.object;
    sourceIndex  = source.propertyIndex;
  }
  const bool hasSource  = sourceObject && sourceIndex != Property::INVALID_INDEX;
  auto       constraint = GetCutoutBackgroundConstraint();
  if(constraint && hasSource && constraint.GetSourceCount() == 1u)
  {
    const auto bound = constraint.GetSourceAt(0u);
    const auto state = constraint.GetState();
    if(bound.object == sourceObject && bound.propertyIndex == sourceIndex &&
       (state == Constraint::APPLIED || state == Constraint::APPLIED_OBJECT_OFF_SCENE))
    {
      return;
    }
  }
  if(!constraint && !hasSource)
  {
    return;
  }
  auto renderer = mImpl->mRenderer;
  if(constraint)
  {
    // Release the target's binding ownership as well as the local list entry.
    // The source remains alive until the removal message has been queued.
    Dali::Integration::HandleRemoveConstraints(renderer, TEXT_VISUAL_CUTOUT_BACKGROUND_CONSTRAINT_TAG);
    if(!hasSource)
    {
      // Release the cutout-only slot and spare capacity without retaining it
      // on labels that return to ordinary text.
      ConstraintContainer colors(mColorConstraintList.begin() + 1, mColorConstraintList.end());
      mColorConstraintList.swap(colors);
    }
  }
  auto index = renderer.GetPropertyIndex("uCutoutBackground");
  if(index == Property::INVALID_INDEX)
  {
    index = renderer.RegisterProperty("uCutoutBackground", Vector4::ZERO);
  }
  renderer.SetProperty(index, Vector4::ZERO);
  if(hasSource)
  {
    // The functor needs the owner before the binding exists. Emplace its observer
    // after Apply; observation never retains the binding.
    auto sourceOwner = std::make_shared<std::optional<CutoutBackgroundSource>>();
    auto binding     = Constraint::New<Vector4>(renderer, index, [sourceOwner](Vector4& current, const PropertyInputContainer& inputs)
        {
      // The capture owns lifetime only; update evaluation never accesses it.
      (void)sourceOwner;
      CutoutBackgroundConstraint(current, inputs);
    });
    binding.AddSource(Source(sourceObject, sourceIndex));
    // Keep the last evaluated color as the base value so an alpha-zero
    // transition is compared with the previous output, not a reset zero.
    // SetCutoutBackground explicitly restores zero when unbinding.
    binding.SetRemoveAction(Constraint::BAKE);
    Dali::Integration::ConstraintSetInternalTag(binding, TEXT_VISUAL_CUTOUT_BACKGROUND_CONSTRAINT_TAG);
    binding.Apply();
    sourceOwner->emplace(binding, sourceObject);
    if(constraint)
    {
      mColorConstraintList.front() = binding;
    }
    else
    {
      mColorConstraintList.insert(mColorConstraintList.begin(), binding);
    }
  }
}

void TextVisual::UpdateCutoutBackgroundVisibility(Ui::View owner)
{
  if(!owner)
  {
    return;
  }
  auto& data           = ViewDataImpl::Get(GetImpl(owner));
  bool  ownsBackground = false;
  if(mIsCutoutRenderer)
  {
    auto background = data.GetVisual(Ui::Integration::View::Property::BACKGROUND);
    // Keep fallback image/gradient backgrounds on scene while text is pending.
    // Detaching an image resets loading and can repeat ResourceReady.
    ownsBackground = background && Ui::GetImplementation(background).GetType() == Ui::Integration::InternalVisualType::COLOR;
    SetCutoutBackground(background);
  }
  else
  {
    SetCutoutBackground(Ui::Integration::Visual::Base());
  }
  if(data.GetVisual(Ui::Integration::View::Property::BACKGROUND) &&
     data.IsVisualEnabled(Ui::Integration::View::Property::BACKGROUND) == ownsBackground)
  {
    data.EnableVisual(Ui::Integration::View::Property::BACKGROUND, !ownsBackground);
  }
}

} // namespace Internal

} // namespace Ui

} // namespace DALI_NAMESPACE

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <dali-ui-foundation/internal/text/text-atlas-gradient-state.h>

#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

#include <atomic>

namespace Dali::Ui::Text::Internal::Gradient
{
namespace
{
uint64_t AllocateResourceId()
{
  static std::atomic<uint64_t> nextResourceId{1u};
  for(;;)
  {
    const uint64_t resourceId = nextResourceId.fetch_add(1u, std::memory_order_relaxed);
    if(resourceId != 0u)
    {
      return resourceId;
    }
  }
}

bool EqualStops(const Style& lhs, const Style& rhs)
{
  if(lhs.stops.Count() != rhs.stops.Count())
  {
    return false;
  }

  for(uint32_t index = 0u; index < lhs.stops.Count(); ++index)
  {
    if(lhs.stops[index].offset != rhs.stops[index].offset || lhs.stops[index].color != rhs.stops[index].color)
    {
      return false;
    }
  }
  return true;
}

bool EqualUniformStyle(const Style& lhs, const Style& rhs)
{
  return lhs.enabled == rhs.enabled &&
         lhs.type == rhs.type &&
         lhs.units == rhs.units &&
         lhs.startOffset == rhs.startOffset &&
         lhs.linearStart == rhs.linearStart &&
         lhs.linearEnd == rhs.linearEnd &&
         lhs.radialCenter == rhs.radialCenter &&
         lhs.radialRadius == rhs.radialRadius &&
         lhs.conicCenter == rhs.conicCenter &&
         lhs.conicStartAngle == rhs.conicStartAngle;
}

bool EqualAuthoredStops(const Dali::Vector<Dali::Ui::Gradient::StopNode>& lhs,
                        const Dali::Vector<Dali::Ui::Gradient::StopNode>& rhs)
{
  if(lhs.Count() != rhs.Count())
  {
    return false;
  }

  for(uint32_t index = 0u; index < lhs.Count(); ++index)
  {
    if(lhs[index].GetOffset() != rhs[index].GetOffset() ||
       lhs[index].GetColor() != rhs[index].GetColor())
    {
      return false;
    }
  }
  return true;
}

bool EqualAuthoredGradient(const Dali::Ui::Gradient::Base& lhs,
                           const Dali::Ui::Gradient::Base& rhs)
{
  if(lhs.GetType() != rhs.GetType())
  {
    return false;
  }

  if(lhs.GetType() == Dali::Ui::Gradient::Type::NONE)
  {
    return true;
  }

  if(lhs.GetUnits() != rhs.GetUnits() ||
     lhs.GetSpreadMethod() != rhs.GetSpreadMethod() ||
     lhs.GetStartOffset() != rhs.GetStartOffset() ||
     !EqualAuthoredStops(lhs.GetStopNodes(), rhs.GetStopNodes()))
  {
    return false;
  }

  switch(lhs.GetType())
  {
    case Dali::Ui::Gradient::Type::LINEAR:
    {
      const auto lhsLinear = Dali::Ui::Gradient::Linear::DownCast(lhs);
      const auto rhsLinear = Dali::Ui::Gradient::Linear::DownCast(rhs);
      return lhsLinear.GetStartPosition() == rhsLinear.GetStartPosition() &&
             lhsLinear.GetEndPosition() == rhsLinear.GetEndPosition();
    }

    case Dali::Ui::Gradient::Type::RADIAL:
    {
      const auto lhsRadial = Dali::Ui::Gradient::Radial::DownCast(lhs);
      const auto rhsRadial = Dali::Ui::Gradient::Radial::DownCast(rhs);
      return lhsRadial.GetCenter() == rhsRadial.GetCenter() &&
             lhsRadial.GetRadius() == rhsRadial.GetRadius();
    }

    case Dali::Ui::Gradient::Type::CONIC:
    {
      const auto lhsConic = Dali::Ui::Gradient::Conic::DownCast(lhs);
      const auto rhsConic = Dali::Ui::Gradient::Conic::DownCast(rhs);
      return lhsConic.GetCenter() == rhsConic.GetCenter() &&
             lhsConic.GetStartAngle() == rhsConic.GetStartAngle();
    }

    case Dali::Ui::Gradient::Type::NONE:
    default:
    {
      return true;
    }
  }
}

Sampler CreateSampler(Dali::Ui::Gradient::SpreadMethod spread)
{
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
  sampler.SetWrapMode(GetWrapMode(spread), WrapMode::CLAMP_TO_EDGE);
  return sampler;
}
} // namespace

bool AtlasRendererState::IsEnabled() const
{
  return IsRenderable(style) && lookupTexture && lookupSampler;
}

bool AtlasApplyState::Matches(const AtlasRendererState& state) const
{
  return initialized &&
         resourceId == state.resourceId &&
         styleRevision == state.styleRevision &&
         lookupRevision == state.lookupRevision;
}

void AtlasApplyState::Set(const AtlasRendererState& state)
{
  status         = AtlasApplyStatus::APPLIED;
  initialized    = true;
  enabled        = state.IsEnabled();
  resourceId     = state.resourceId;
  styleRevision  = state.styleRevision;
  lookupRevision = state.lookupRevision;
}

void AtlasApplyState::SetSolidFallback(const AtlasRendererState& state)
{
  status         = AtlasApplyStatus::SOLID_FALLBACK;
  initialized    = true;
  enabled        = state.IsEnabled();
  resourceId     = state.resourceId;
  styleRevision  = state.styleRevision;
  lookupRevision = state.lookupRevision;
}

bool AtlasApplyState::IsGradientApplied() const
{
  return initialized && enabled && status == AtlasApplyStatus::APPLIED;
}

bool AtlasApplyState::IsSolidFallback() const
{
  return initialized && status == AtlasApplyStatus::SOLID_FALLBACK;
}

void AtlasApplyState::Reset()
{
  *this = AtlasApplyState{};
}

AtlasResource::AtlasResource()
{
  mRendererState.resourceId = AllocateResourceId();
}

bool AtlasResource::Set(const Dali::Ui::Gradient::Base& gradient)
{
  const Style                    newStyle       = CreateStyle(gradient);
  const Dali::Ui::Gradient::Base newAuthored    = IsRenderable(gradient) ? gradient : Dali::Ui::Gradient::Base::None();
  const bool                     authoredChange = !EqualAuthoredGradient(mAuthored, newAuthored);
  const bool                     stopsChange    = !EqualStops(mRendererState.style, newStyle);
  const bool                     spreadChange   = mRendererState.style.spreadMethod != newStyle.spreadMethod;
  const bool                     uniformChange  = !EqualUniformStyle(mRendererState.style, newStyle);

  if(!authoredChange && !stopsChange && !spreadChange && !uniformChange)
  {
    return false;
  }

  // Match Label's authored-value policy: Type::NONE and gradients with fewer
  // than two stops are stored as None, while render-path limitations such as a
  // degenerate radius remain an authored value that may render as solid text.
  if(authoredChange)
  {
    mAuthored = newAuthored;
  }

  if(uniformChange)
  {
    ++mRendererState.styleRevision;
  }

  bool lookupChange = false;

  if(stopsChange)
  {
    mRendererState.lookupTexture = newStyle.stops.Count() >= 2u ? CreateLookupTexture(newStyle) : Texture();
    lookupChange                 = true;
    if(mRendererState.lookupTexture)
    {
      ++mLookupGenerationCount;
    }
  }

  if(IsRenderable(newStyle) && (spreadChange || !mRendererState.lookupSampler))
  {
    mRendererState.lookupSampler = CreateSampler(newStyle.spreadMethod);
    lookupChange                 = true;
  }
  else if(!IsRenderable(newStyle) && mRendererState.lookupSampler)
  {
    mRendererState.lookupSampler.Reset();
    lookupChange = true;
  }

  mRendererState.style = newStyle;
  if(lookupChange)
  {
    ++mRendererState.lookupRevision;
  }
  return true;
}

const Dali::Ui::Gradient::Base& AtlasResource::Get() const
{
  return mAuthored;
}

bool AtlasResource::IsEnabled() const
{
  return mRendererState.IsEnabled();
}

uint64_t AtlasResource::GetStyleRevision() const
{
  return mRendererState.styleRevision;
}

uint64_t AtlasResource::GetLookupRevision() const
{
  return mRendererState.lookupRevision;
}

uint64_t AtlasResource::GetLookupGenerationCount() const
{
  return mLookupGenerationCount;
}

const AtlasRendererState& AtlasResource::GetRendererState() const
{
  return mRendererState;
}

bool EditableAtlasResources::SetTextGradient(const Dali::Ui::Gradient::Base& gradient)
{
  return mText.Set(gradient);
}

bool EditableAtlasResources::SetPlaceholderGradient(const Dali::Ui::Gradient::Base& gradient)
{
  return mPlaceholder.Set(gradient);
}

bool EditableAtlasResources::SetBoundsMode(Text::GradientBoundsMode mode)
{
  if(mBoundsMode == mode)
  {
    return false;
  }
  mBoundsMode = mode;
  return true;
}

const Dali::Ui::Gradient::Base& EditableAtlasResources::GetTextGradient() const
{
  return mText.Get();
}

const Dali::Ui::Gradient::Base& EditableAtlasResources::GetPlaceholderGradient() const
{
  return mPlaceholder.Get();
}

Text::GradientBoundsMode EditableAtlasResources::GetBoundsMode() const
{
  return mBoundsMode;
}

const AtlasRendererState& EditableAtlasResources::GetRendererState(bool placeholder) const
{
  return (placeholder ? mPlaceholder : mText).GetRendererState();
}

AtlasFrameState EditableAtlasResources::GetFrameState(bool placeholder) const
{
  return AtlasFrameState{GetRendererState(placeholder).IsEnabled(), mBoundsMode};
}

AtlasFrameState EditableAtlasResources::GetFrameState(bool placeholder, const AtlasApplyState& applied) const
{
  AtlasFrameState frameState = GetFrameState(placeholder);
  frameState.enabled         = frameState.enabled &&
                       applied.Matches(GetRendererState(placeholder)) &&
                       applied.IsGradientApplied();
  return frameState;
}

} // namespace Dali::Ui::Text::Internal::Gradient

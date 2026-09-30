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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture-set.h>
#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-image-reveal-shader.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace Text
{
namespace
{
constexpr const char* INLINE_REPLACEMENT_REVEAL_BASE_OPACITY = "__dali_ui_inline_replacement_reveal_base_opacity";
constexpr const char* INLINE_REPLACEMENT_REVEAL_PROGRESS     = "uInlineReplacementRevealProgress";
constexpr const char* INLINE_REPLACEMENT_REVEAL_TIMING       = "uInlineReplacementRevealTiming";

struct ReplacementRevealProgressConstraint
{
  void operator()(float& current, const PropertyInputContainer& inputs)
  {
    current = std::max(0.0f, std::min(1.0f, inputs[0]->GetFloat()));
  }
};

struct ReplacementRevealOpacityConstraint
{
  ReplacementRevealOpacityConstraint(float start, float fadeDuration)
  : start(start),
    fadeDuration(fadeDuration)
  {
  }

  void operator()(float& current, const PropertyInputContainer& inputs)
  {
    const float baseOpacity = std::max(0.0f, std::min(1.0f, inputs[0]->GetFloat()));
    const float progress    = Ui::Text::Internal::Reveal::ResolveRenderProgress(inputs[1]->GetFloat());
    float       reveal      = 0.0f;
    if(progress >= 1.0f)
    {
      reveal = 1.0f;
    }
    else if(progress > 0.0f)
    {
      reveal = fadeDuration > 0.0f
                 ? std::max(0.0f, std::min(1.0f, (progress - start) / fadeDuration))
                 : (progress >= start ? 1.0f : 0.0f);
    }
    current = baseOpacity * reveal;
  }

  float start;
  float fadeDuration;
};

void DiscardVisual(Ui::Integration::Visual::Base& visual)
{
  if(Dali::Adaptor::IsAvailable() && visual)
  {
    Ui::Integration::VisualFactory::Get().DiscardVisual(visual);
  }
  visual.Reset();
}

bool IsSupportedStaticImageSource(const std::string& source)
{
  // The factory's static-only option keeps GIF/WebP on ImageVisual (first
  // frame), but JSON would still create AnimatedVectorImageVisual.
  return Ui::Internal::VisualUrl(source).GetType() != Ui::Internal::VisualUrl::JSON;
}

bool CanUsePixelRevealCustomShader(const std::string& source)
{
  const Ui::Internal::VisualUrl::Type type = Ui::Internal::VisualUrl(source).GetType();
  return type == Ui::Internal::VisualUrl::REGULAR_IMAGE ||
         type == Ui::Internal::VisualUrl::GIF ||
         type == Ui::Internal::VisualUrl::WEBP;
}

bool UsesMultiPlaneYuvTexture(const VisualRenderer& renderer)
{
  const TextureSet textures = renderer ? renderer.GetTextures() : TextureSet{};
  if(!textures || textures.GetTextureCount() < 3u)
  {
    return false;
  }
  const Texture luminance    = textures.GetTexture(0u);
  const Texture chrominanceU = textures.GetTexture(1u);
  const Texture chrominanceV = textures.GetTexture(2u);
  return luminance && chrominanceU && chrominanceV &&
         luminance.GetPixelFormat() == Pixel::L8 &&
         chrominanceU.GetPixelFormat() == Pixel::CHROMINANCE_U &&
         chrominanceV.GetPixelFormat() == Pixel::CHROMINANCE_V;
}

Property::Map CreatePixelRevealCustomShaderMap()
{
  Property::Map shaderMap;
  shaderMap.Insert(Ui::Integration::Visual::Shader::Property::FRAGMENT_SHADER,
                   Dali::String(INLINE_REPLACEMENT_IMAGE_REVEAL_FRAGMENT_SHADER));
  shaderMap.Insert(Ui::Integration::Visual::Shader::Property::HINTS,
                   static_cast<int>(Dali::Shader::Hint::OUTPUT_IS_TRANSPARENT));
  shaderMap.Insert(Ui::Integration::Visual::Shader::Property::NAME,
                   Dali::String("INLINE_REPLACEMENT_IMAGE_PIXEL_REVEAL"));
  return shaderMap;
}

bool RectanglesIntersect(const Vector2& firstOffset,
                         const Vector2& firstSize,
                         const Vector2& secondOffset,
                         const Vector2& secondSize)
{
  return firstSize.x > 0.0f && firstSize.y > 0.0f && secondSize.x > 0.0f && secondSize.y > 0.0f &&
         firstOffset.x < secondOffset.x + secondSize.x && firstOffset.x + firstSize.x > secondOffset.x &&
         firstOffset.y < secondOffset.y + secondSize.y && firstOffset.y + firstSize.y > secondOffset.y;
}
} // unnamed namespace

InlineReplacementViewHost::InlineReplacementViewHost(Ui::View owner, int contentDepth)
: mOwner(owner),
  mContentDepth(contentDepth)
{
}

Property::Index InlineReplacementViewHost::AllocateVisualSlot()
{
  if(!mFreeVisualSlots.empty())
  {
    const Property::Index index = mFreeVisualSlots.back();
    mFreeVisualSlots.pop_back();
    return index;
  }

  Ui::View owner = mOwner.GetHandle();
  if(!owner)
  {
    return Property::INVALID_INDEX;
  }

  const std::string name = "__dali_ui_inline_replacement_" + std::to_string(mNextSlotName++);
  return owner.RegisterProperty(Dali::String(name.c_str()), 0);
}

void InlineReplacementViewHost::ReleaseVisualSlot(Property::Index index)
{
  if(index != Property::INVALID_INDEX)
  {
    mFreeVisualSlots.push_back(index);
  }
}

void InlineReplacementViewHost::RegisterVisual(Property::Index                index,
                                               Ui::Integration::Visual::Base& visual)
{
  Ui::View owner = mOwner.GetHandle();
  if(owner && index != Property::INVALID_INDEX)
  {
    ViewDataImpl::Get(GetImpl(owner)).RegisterVisual(index, visual, mContentDepth);
  }
}

void InlineReplacementViewHost::UnregisterVisual(Property::Index index)
{
  Ui::View owner = mOwner.GetHandle();
  if(owner && index != Property::INVALID_INDEX)
  {
    ViewDataImpl::Get(GetImpl(owner)).UnregisterVisual(index);
  }
}

Ui::View InlineReplacementViewHost::GetOwner() const
{
  return mOwner.GetHandle();
}

InlineReplacementManager::InlineReplacementManager() = default;

InlineReplacementManager::CaptureUpdateScope::CaptureUpdateScope(InlineReplacementManager& manager)
: mState(manager.mUpdateState)
{
  ++mState->depth;
  if(manager.mBlurCapture)
  {
    manager.ReleaseBlurCapture(manager.mBlurCapture->client.GetHandle());
  }
}

InlineReplacementManager::CaptureUpdateScope::~CaptureUpdateScope()
{
  --mState->depth;
}

InlineReplacementManager::~InlineReplacementManager()
{
  Clear();
  mUpdateState->alive = false;
}

std::vector<InlineReplacementManager::Entry>::iterator InlineReplacementManager::RemoveEntry(
  std::vector<Entry>::iterator iterator)
{
  UpdateGuard current(mUpdateState);
  ReleaseEntryVisual(*iterator);
  if(!current)
  {
    return {};
  }
  return mEntries.erase(iterator);
}

void InlineReplacementManager::ReleaseEntryVisual(Entry& entry)
{
  UpdateGuard current(mUpdateState);
  if(mBlurCapture)
  {
    if(auto retention = mBlurCapture->retention.GetHandle())
    {
      for(auto& source : mBlurCapture->sources)
      {
        if(source.occurrenceIdentity == entry.occurrenceIdentity)
        {
          retention.RemoveRenderer(source.renderer);
        }
      }
    }
    auto& sources = mBlurCapture->sources;
    sources.erase(std::remove_if(sources.begin(), sources.end(), [&](const auto& source)
    {
      return source.occurrenceIdentity == entry.occurrenceIdentity;
    }),
                  sources.end());
  }
  RemoveEntryRevealConstraint(entry);
  if(!current)
  {
    return;
  }
  if(mHost && entry.propertyIndex != Property::INVALID_INDEX)
  {
    mHost->UnregisterVisual(entry.propertyIndex);
    if(!current)
    {
      return;
    }
    mHost->ReleaseVisualSlot(entry.propertyIndex);
  }
  DiscardVisual(entry.visual);
  if(!current)
  {
    return;
  }
  entry.propertyIndex = Property::INVALID_INDEX;
  ResetEntryResourceState(entry);
}

bool InlineReplacementManager::CreateEntryVisual(InlineReplacementViewHost& host, Entry& entry)
{
  UpdateGuard current(mUpdateState);
  const auto  propertyIndex = host.AllocateVisualSlot();
  if(!current)
  {
    return false;
  }
  entry.propertyIndex = propertyIndex;
  if(entry.propertyIndex == Property::INVALID_INDEX)
  {
    return false;
  }

  Property::Map visualMap;
  visualMap.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  visualMap.Insert(Ui::Integration::ImageVisual::Property::URL, Dali::String(entry.descriptor.source.c_str()));
  visualMap.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, entry.descriptor.desiredWidth);
  visualMap.Insert(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, entry.descriptor.desiredHeight);
  visualMap.Insert(Ui::Integration::ImageVisual::Property::FITTING_MODE,
                   static_cast<int>(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO));
  visualMap.Insert(Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION, true);
  const bool requestPixelRevealShader = mPixelRevealRequested &&
                                        CanUsePixelRevealCustomShader(entry.descriptor.source);
  if(requestPixelRevealShader)
  {
    visualMap.Insert(Ui::Integration::Visual::Property::SHADER, CreatePixelRevealCustomShaderMap());
  }
  auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(
    visualMap,
    Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
  if(!current)
  {
    DiscardVisual(visual);
    return false;
  }
  entry.visual = std::move(visual);
  if(!entry.visual)
  {
    host.ReleaseVisualSlot(entry.propertyIndex);
    entry.propertyIndex = Property::INVALID_INDEX;
    return false;
  }

  // Inline geometry is authoritative. View-level image fitting would use the
  // whole Label as bounds and overwrite the reserved inline box.
  auto& visualImpl = Ui::GetImplementation(entry.visual);
  visualImpl.SetFittingModeRequired(false);
  visualImpl.SetResourceReadyRelayoutRequired(false);
  visualImpl.SetTransformMapUsageForFittingMode(true);
  Property::Map opacityMap;
  opacityMap.Insert(Ui::Integration::Visual::Property::OPACITY, 0.0f);
  entry.visual.SetProperties(opacityMap);
  if(!current)
  {
    return false;
  }
  ResetEntryResourceState(entry);
  host.RegisterVisual(entry.propertyIndex, entry.visual);
  return true;
}

InlineReplacementManager::RuntimeImageDescriptor InlineReplacementManager::BuildRuntimeImageDescriptor(
  const Ui::Text::ReplacementRunSnapshot& run,
  float                                   effectiveScale)
{
  RuntimeImageDescriptor descriptor;
  descriptor.source = run.image.source;

  const float safeScale    = std::isfinite(effectiveScale) && effectiveScale > 0.0f ? effectiveScale : 1.0f;
  const float maxDimension = static_cast<float>(std::numeric_limits<int32_t>::max());
  descriptor.desiredWidth  = static_cast<int32_t>(std::min(maxDimension,
                                                           std::max(1.0f, std::ceil(run.metrics.width * safeScale))));
  descriptor.desiredHeight = static_cast<int32_t>(std::min(maxDimension,
                                                           std::max(1.0f, std::ceil(run.metrics.height * safeScale))));
  return descriptor;
}

bool InlineReplacementManager::IsSameRuntimeImageDescriptor(const RuntimeImageDescriptor& lhs,
                                                            const RuntimeImageDescriptor& rhs)
{
  return lhs.source == rhs.source && lhs.desiredWidth == rhs.desiredWidth &&
         lhs.desiredHeight == rhs.desiredHeight;
}

void InlineReplacementManager::ResetEntryResourceState(Entry& entry)
{
  entry.naturalSize         = Vector2::ZERO;
  entry.lastTransformOffset = Vector2::ZERO;
  entry.lastTransformSize   = Vector2::ZERO;
  entry.lastOwnerSize       = Vector2::ZERO;
  entry.lastPixelArea       = Vector4::ZERO;
  entry.lastEffectiveScale  = 0.0f;
  entry.aspectResolved      = false;
  entry.currentlyVisible    = false;
  entry.transformApplied    = false;
  entry.pixelAreaApplied    = false;
}

void InlineReplacementManager::SetEntryVisible(Entry& entry, bool visible)
{
  if(!entry.visual || entry.currentlyVisible == visible)
  {
    return;
  }

  entry.currentlyVisible  = visible;
  VisualRenderer renderer = entry.visual.GetRenderer();
  if(entry.revealConstraint && !entry.revealPixelSpatial && renderer &&
     entry.revealBaseOpacityIndex != Property::INVALID_INDEX)
  {
    renderer.SetProperty(entry.revealBaseOpacityIndex, visible ? 1.0f : 0.0f);
  }
  else
  {
    Property::Map opacityMap;
    opacityMap.Insert(Ui::Integration::Visual::Property::OPACITY, visible ? 1.0f : 0.0f);
    entry.visual.SetProperties(opacityMap);
  }
}

void InlineReplacementManager::UpdateEntryVisibility(Entry& entry)
{
  const bool resourceReady = entry.visual &&
                             Ui::GetImplementation(entry.visual).GetResourceStatus() == Ui::Visual::ResourceStatus::READY;
  const bool geometryReady      = entry.transformApplied && entry.pixelAreaApplied;
  const bool revealBindingReady = !mRevealBindingRequired ||
                                  (mRevealSourceRevision == mEntrySourceRevision && entry.revealConstraint &&
                                   entry.revealProgressPropertyIndex == mRevealProgressPropertyIndex);
  SetEntryVisible(entry, resourceReady && geometryReady && revealBindingReady);
}

void InlineReplacementManager::RemoveEntryRevealConstraint(Entry& entry, bool removePixelShader)
{
  UpdateGuard current(mUpdateState);
  if(entry.revealConstraint)
  {
    entry.revealConstraint.Remove();
    entry.revealConstraint.Reset();
  }
  if(removePixelShader && entry.visual && Ui::GetImplementation(entry.visual).IsUsingCustomShader())
  {
    SetEntryPixelRevealShader(entry, false);
    if(!current)
    {
      return;
    }
  }
  entry.revealPixelSpatial          = false;
  entry.revealPixelProgressIndex    = Property::INVALID_INDEX;
  entry.revealProgressPropertyIndex = Property::INVALID_INDEX;
  if(entry.visual)
  {
    Property::Map opacityMap;
    opacityMap.Insert(Ui::Integration::Visual::Property::OPACITY, entry.currentlyVisible ? 1.0f : 0.0f);
    entry.visual.SetProperties(opacityMap);
  }
}

bool InlineReplacementManager::SetEntryPixelRevealShader(Entry& entry, bool enabled)
{
  UpdateGuard current(mUpdateState);
  if(!entry.visual)
  {
    return !enabled;
  }
  auto& visualImpl = Ui::GetImplementation(entry.visual);
  if(enabled)
  {
    if(visualImpl.GetType() != Ui::Integration::InternalVisualType::IMAGE ||
       !CanUsePixelRevealCustomShader(entry.descriptor.source))
    {
      return false;
    }
    if(UsesMultiPlaneYuvTexture(entry.visual.GetRenderer()))
    {
      if(visualImpl.IsUsingCustomShader())
      {
        Property::Array emptyShaderArray;
        Property::Map   visualMap;
        visualMap.Insert(Ui::Integration::Visual::Property::SHADER, emptyShaderArray);
        entry.visual.SetProperties(visualMap);
      }
      return false;
    }
    if(!visualImpl.IsUsingCustomShader())
    {
      Property::Map visualMap;
      visualMap.Insert(Ui::Integration::Visual::Property::SHADER, CreatePixelRevealCustomShaderMap());
      entry.visual.SetProperties(visualMap);
      if(!current)
      {
        return false;
      }
    }
    return visualImpl.IsUsingCustomShader();
  }

  if(visualImpl.IsUsingCustomShader())
  {
    Property::Array emptyShaderArray;
    Property::Map   visualMap;
    visualMap.Insert(Ui::Integration::Visual::Property::SHADER, emptyShaderArray);
    entry.visual.SetProperties(visualMap);
    if(!current)
    {
      return false;
    }
  }
  return !visualImpl.IsUsingCustomShader();
}

bool InlineReplacementManager::UpdateEntryPixelRevealTiming(Entry& entry)
{
  if(!entry.revealPixelSpatial || !entry.visual || !std::isfinite(entry.reservedSize.x) ||
     entry.reservedSize.x <= Math::MACHINE_EPSILON_1 || !entry.pixelAreaApplied ||
     !std::isfinite(entry.lastPixelArea.x) || !std::isfinite(entry.lastPixelArea.z) ||
     entry.lastPixelArea.z <= Math::MACHINE_EPSILON_1)
  {
    return false;
  }
  VisualRenderer renderer = entry.visual.GetRenderer();
  if(!renderer)
  {
    return false;
  }

  const float reservedLeft  = entry.reservedOffset.x;
  const float reservedWidth = entry.reservedSize.x;
  const float visibleLeft   = std::max(0.0f,
                                       std::min(1.0f,
                                                (entry.lastTransformOffset.x - reservedLeft) / reservedWidth));
  const float visibleRight =
    std::max(visibleLeft,
             std::min(1.0f,
                      (entry.lastTransformOffset.x + entry.lastTransformSize.x - reservedLeft) / reservedWidth));
  const float unitStart   = entry.revealStart - 0.5f * entry.revealProgressionSpan;
  const float startAtLeft = unitStart + entry.revealProgressionSpan *
                                          (entry.revealRightToLeft ? 1.0f - visibleLeft : visibleLeft);
  const float startDelta = entry.revealProgressionSpan * (visibleRight - visibleLeft) *
                           (entry.revealRightToLeft ? -1.0f : 1.0f);
  // The standard ImageVisual vertex shader maps the quad coordinate q to
  // vTexCoord.x = pixelArea.x + pixelArea.z * q. Precompose the inverse here
  // so the fragment does not redeclare the vertex-owned pixelArea uniform.
  const float timingSlope     = startDelta / entry.lastPixelArea.z;
  const float timingIntercept = startAtLeft - timingSlope * entry.lastPixelArea.x;
  if(!std::isfinite(timingIntercept) || !std::isfinite(timingSlope))
  {
    return false;
  }
  const Vector3 timing(timingIntercept, timingSlope, entry.revealFadeDuration);

  Property::Index timingIndex = renderer.GetPropertyIndex(INLINE_REPLACEMENT_REVEAL_TIMING);
  if(timingIndex == Property::INVALID_INDEX)
  {
    timingIndex = renderer.RegisterProperty(INLINE_REPLACEMENT_REVEAL_TIMING, timing);
  }
  else
  {
    renderer.SetProperty(timingIndex, timing);
  }
  return timingIndex != Property::INVALID_INDEX;
}

InlineReplacementManager::RevealBindingResult InlineReplacementManager::ApplyEntryRevealConstraint(Entry& entry)
{
  UpdateGuard current(mUpdateState);
  if(!mHost || !entry.visual || mRevealProgressPropertyIndex == Property::INVALID_INDEX)
  {
    RemoveEntryRevealConstraint(entry);
    return RevealBindingResult::INVALID;
  }
  const auto timing = mRevealTimings.find(entry.occurrenceIdentity);
  if(timing == mRevealTimings.end())
  {
    RemoveEntryRevealConstraint(entry);
    return RevealBindingResult::INVALID;
  }
  Ui::View owner = mHost->GetOwner();
  if(!owner)
  {
    RemoveEntryRevealConstraint(entry);
    return RevealBindingResult::INVALID;
  }
  VisualRenderer renderer = entry.visual.GetRenderer();
  if(!renderer)
  {
    SetEntryVisible(entry, false);
    return RevealBindingResult::DEFERRED;
  }

  const float start           = timing->second.start;
  const float fadeDuration    = timing->second.fadeDuration;
  const float progressionSpan = timing->second.progressionSpan;
  const bool  rightToLeft     = timing->second.rightToLeft;
  const auto& visualImpl      = Ui::GetImplementation(entry.visual);
  const bool  pixelSpatial =
    mPixelRevealRequested && progressionSpan > 0.0f &&
    visualImpl.GetType() == Ui::Integration::InternalVisualType::IMAGE &&
    CanUsePixelRevealCustomShader(entry.descriptor.source) &&
    !UsesMultiPlaneYuvTexture(renderer);
  if(entry.revealConstraint && entry.revealProgressPropertyIndex == mRevealProgressPropertyIndex &&
     Dali::Equals(entry.revealStart, start) && Dali::Equals(entry.revealFadeDuration, fadeDuration) &&
     Dali::Equals(entry.revealProgressionSpan, progressionSpan) && entry.revealRightToLeft == rightToLeft &&
     entry.revealPixelSpatial == pixelSpatial)
  {
    UpdateEntryVisibility(entry);
    return RevealBindingResult::APPLIED;
  }

  // A creation-time PIXEL shader is already the desired shader. Preserve it
  // while replacing only the binding so fast-ready resources never force a
  // READY-state shader remove/reinstall cycle.
  const bool preservePixelShader = pixelSpatial && visualImpl.IsUsingCustomShader();
  RemoveEntryRevealConstraint(entry, !preservePixelShader);
  if(!current)
  {
    return RevealBindingResult::INVALID;
  }
  entry.revealStart           = start;
  entry.revealFadeDuration    = fadeDuration;
  entry.revealProgressionSpan = progressionSpan;
  entry.revealRightToLeft     = rightToLeft;

  const bool pixelShader = pixelSpatial && SetEntryPixelRevealShader(entry, true);
  if(!current)
  {
    return RevealBindingResult::INVALID;
  }
  if(pixelShader)
  {
    entry.revealPixelSpatial       = true;
    entry.revealPixelProgressIndex = renderer.GetPropertyIndex(INLINE_REPLACEMENT_REVEAL_PROGRESS);
    if(entry.revealPixelProgressIndex == Property::INVALID_INDEX)
    {
      const auto progressIndex = renderer.RegisterProperty(
        INLINE_REPLACEMENT_REVEAL_PROGRESS,
        std::max(0.0f, std::min(1.0f, owner.GetCurrentProperty<float>(mRevealProgressPropertyIndex))));
      if(!current)
      {
        return RevealBindingResult::INVALID;
      }
      entry.revealPixelProgressIndex = progressIndex;
    }
    if(!current)
    {
      return RevealBindingResult::INVALID;
    }
    const bool validTiming = entry.revealPixelProgressIndex != Property::INVALID_INDEX && UpdateEntryPixelRevealTiming(entry);
    if(!current)
    {
      return RevealBindingResult::INVALID;
    }
    if(validTiming)
    {
      auto constraint = Constraint::New<float>(renderer,
                                               entry.revealPixelProgressIndex,
                                               ReplacementRevealProgressConstraint());
      if(!current)
      {
        return RevealBindingResult::INVALID;
      }
      entry.revealConstraint = std::move(constraint);
      entry.revealConstraint.AddSource(Source(owner, mRevealProgressPropertyIndex));
      entry.revealConstraint.SetRemoveAction(Constraint::DISCARD);
      entry.revealConstraint.SetApplyRate(Dali::Constraint::APPLY_ALWAYS);
      entry.revealConstraint.Apply();
      entry.revealProgressPropertyIndex = mRevealProgressPropertyIndex;
      UpdateEntryVisibility(entry);
      return RevealBindingResult::APPLIED;
    }
    // Geometry and pixel-area publication can legitimately follow timing
    // publication. Keep the accepted timing, custom shader and registered
    // progress property so Update()/Refresh() can complete the same binding.
    SetEntryVisible(entry, false);
    return RevealBindingResult::DEFERRED;
  }

  entry.revealBaseOpacityIndex = renderer.GetPropertyIndex(INLINE_REPLACEMENT_REVEAL_BASE_OPACITY);
  if(entry.revealBaseOpacityIndex == Property::INVALID_INDEX)
  {
    const auto baseIndex = renderer.RegisterProperty(INLINE_REPLACEMENT_REVEAL_BASE_OPACITY,
                                                     entry.currentlyVisible ? 1.0f : 0.0f);
    if(!current)
    {
      return RevealBindingResult::INVALID;
    }
    entry.revealBaseOpacityIndex = baseIndex;
  }
  else
  {
    renderer.SetProperty(entry.revealBaseOpacityIndex, entry.currentlyVisible ? 1.0f : 0.0f);
  }
  if(!current)
  {
    return RevealBindingResult::INVALID;
  }
  if(entry.revealBaseOpacityIndex == Property::INVALID_INDEX)
  {
    SetEntryVisible(entry, false);
    return RevealBindingResult::DEFERRED;
  }

  auto constraint = Constraint::New<float>(renderer,
                                           Dali::DevelRenderer::Property::OPACITY,
                                           ReplacementRevealOpacityConstraint(start, fadeDuration));
  if(!current)
  {
    return RevealBindingResult::INVALID;
  }
  entry.revealConstraint = std::move(constraint);
  entry.revealConstraint.AddSource(Source(renderer, entry.revealBaseOpacityIndex));
  entry.revealConstraint.AddSource(Source(owner, mRevealProgressPropertyIndex));
  entry.revealConstraint.SetRemoveAction(Constraint::DISCARD);
  entry.revealConstraint.SetApplyRate(Dali::Constraint::APPLY_ALWAYS);
  entry.revealConstraint.Apply();
  entry.revealProgressPropertyIndex = mRevealProgressPropertyIndex;
  UpdateEntryVisibility(entry);
  return RevealBindingResult::APPLIED;
}

bool InlineReplacementManager::ApplyEntryTransform(Entry& entry)
{
  UpdateGuard current(mUpdateState);
  if(!entry.visual)
  {
    return true;
  }
  auto&                            visualImpl = Ui::GetImplementation(entry.visual);
  const Ui::Visual::ResourceStatus status     = visualImpl.GetResourceStatus();
  if(status == Ui::Visual::ResourceStatus::FAILED)
  {
    return false;
  }
  const bool readyToReveal = status == Ui::Visual::ResourceStatus::READY;
  if(readyToReveal && entry.revealPixelSpatial && UsesMultiPlaneYuvTexture(entry.visual.GetRenderer()))
  {
    SetEntryPixelRevealShader(entry, false);
    if(!current)
    {
      return false;
    }
    entry.revealPixelSpatial = false;
    ApplyEntryRevealConstraint(entry);
    if(!current)
    {
      return false;
    }
  }
  if(!readyToReveal)
  {
    SetEntryVisible(entry, false);
    if(!current)
    {
      return false;
    }
  }
  else if(!entry.aspectResolved)
  {
    entry.visual.GetNaturalSize(entry.naturalSize);
    if(!std::isfinite(entry.naturalSize.x) || !std::isfinite(entry.naturalSize.y) ||
       entry.naturalSize.x <= 0.0f || entry.naturalSize.y <= 0.0f)
    {
      SetEntryVisible(entry, false);
      return true;
    }
    entry.aspectResolved = true;
  }

  Vector2 visualOffset = entry.reservedOffset;
  Vector2 visualSize   = entry.reservedSize;
  if(readyToReveal && visualSize.x > 0.0f && visualSize.y > 0.0f)
  {
    const float aspectScale = std::min(visualSize.x / entry.naturalSize.x,
                                       visualSize.y / entry.naturalSize.y);
    if(std::isfinite(aspectScale) && aspectScale > 0.0f)
    {
      const Vector2 fittedSize = entry.naturalSize * aspectScale;
      visualOffset += (visualSize - fittedSize) * 0.5f;
      visualSize = fittedSize;
    }
  }
  const Vector2 unclippedOffset = visualOffset;
  const Vector2 unclippedSize   = visualSize;
  Vector4       pixelArea(0.0f, 0.0f, 1.0f, 1.0f);
  if(RectanglesIntersect(unclippedOffset, unclippedSize, entry.clipOffset, entry.clipSize))
  {
    const float clippedLeft   = std::max(unclippedOffset.x, entry.clipOffset.x);
    const float clippedTop    = std::max(unclippedOffset.y, entry.clipOffset.y);
    const float clippedRight  = std::min(unclippedOffset.x + unclippedSize.x,
                                         entry.clipOffset.x + entry.clipSize.x);
    const float clippedBottom = std::min(unclippedOffset.y + unclippedSize.y,
                                         entry.clipOffset.y + entry.clipSize.y);
    visualOffset              = Vector2(clippedLeft, clippedTop);
    visualSize                = Vector2(clippedRight - clippedLeft, clippedBottom - clippedTop);
    pixelArea                 = Vector4((clippedLeft - unclippedOffset.x) / unclippedSize.x,
                                        (clippedTop - unclippedOffset.y) / unclippedSize.y,
                                        visualSize.x / unclippedSize.x,
                                        visualSize.y / unclippedSize.y);
  }
  else
  {
    visualOffset = entry.clipOffset;
    visualSize   = Vector2::ZERO;
  }

  // Registered visuals are not child actors and therefore do not inherit the
  // text bitmap's clip. Crop the sampled image and its quad to the Label's
  // content box so CENTER/CLIP and vertically overflowing layouts match text.
  if(!entry.pixelAreaApplied || entry.lastPixelArea != pixelArea)
  {
    Property::Map pixelAreaMap;
    pixelAreaMap.Insert(Ui::Integration::ImageVisual::Property::PIXEL_AREA, pixelArea);
    entry.visual.SetProperties(pixelAreaMap);
    if(!current)
    {
      return false;
    }
    entry.lastPixelArea    = pixelArea;
    entry.pixelAreaApplied = true;
  }

  if(!entry.transformApplied || entry.lastTransformOffset != visualOffset || entry.lastTransformSize != visualSize ||
     entry.lastOwnerSize != entry.ownerSize || entry.lastEffectiveScale != entry.effectiveScale)
  {
    Property::Map transform;
    transform.Add(Ui::Integration::Visual::Transform::Property::SIZE, visualSize)
      .Add(Ui::Integration::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Integration::Visual::Transform::Policy::ABSOLUTE, Ui::Integration::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Integration::Visual::Transform::Property::OFFSET, visualOffset)
      .Add(Ui::Integration::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Integration::Visual::Transform::Policy::ABSOLUTE, Ui::Integration::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Integration::Visual::Transform::Property::ORIGIN, Ui::VisualOrigin::TOP_LEFT)
      .Add(Ui::Integration::Visual::Transform::Property::PIVOT, Ui::VisualPivot::TOP_LEFT);
    visualImpl.SetTransformAndSize(transform, entry.ownerSize, entry.effectiveScale);
    if(!current)
    {
      return false;
    }
    entry.lastTransformOffset = visualOffset;
    entry.lastTransformSize   = visualSize;
    entry.lastOwnerSize       = entry.ownerSize;
    entry.lastEffectiveScale  = entry.effectiveScale;
    entry.transformApplied    = true;
  }

  if(entry.revealPixelSpatial)
  {
    UpdateEntryPixelRevealTiming(entry);
    if(!current)
    {
      return false;
    }
  }

  // Geometry, sampling and an authored PIXEL binding are committed before
  // the first visible frame, independently of resource completion order.
  UpdateEntryVisibility(entry);
  return true;
}

bool InlineReplacementManager::Update(InlineReplacementViewHost&                    host,
                                      const Ui::Text::ReplacementSourceSnapshot&    source,
                                      const Vector<Ui::Text::ReplacementPlacement>& placements,
                                      const Vector2&                                contentOffset,
                                      const Vector2&                                contentSize,
                                      const Vector2&                                ownerSize,
                                      float                                         effectiveScale,
                                      uint64_t                                      expectedSourceRevision,
                                      bool                                          pixelRevealRequested)
{
  return Update(host,
                source,
                placements,
                contentOffset,
                contentOffset,
                contentSize,
                ownerSize,
                effectiveScale,
                expectedSourceRevision,
                pixelRevealRequested);
}

bool InlineReplacementManager::Update(InlineReplacementViewHost&                    host,
                                      const Ui::Text::ReplacementSourceSnapshot&    source,
                                      const Vector<Ui::Text::ReplacementPlacement>& placements,
                                      const Vector2&                                placementOffset,
                                      const Vector2&                                clipOffset,
                                      const Vector2&                                contentSize,
                                      const Vector2&                                ownerSize,
                                      float                                         effectiveScale,
                                      uint64_t                                      expectedSourceRevision,
                                      bool                                          pixelRevealRequested)
{
  Ui::View owner = host.GetOwner();
  if(!owner || source.sourceRevision != expectedSourceRevision)
  {
    return false;
  }
  const auto state = mUpdateState;
  ++state->generation;
  CaptureUpdateScope captureUpdate(*this);
  if(!mRevealTimings.empty() && mRevealSourceRevision != expectedSourceRevision)
  {
    const auto expected = state->generation + 1u;
    ClearReveal();
    if(!state->alive || state->generation != expected)
    {
      return false;
    }
  }
  if(mHost && mHost != &host)
  {
    const auto expected = state->generation + 1u;
    Clear();
    if(!state->alive || state->generation != expected)
    {
      return false;
    }
  }
  UpdateGuard current(state);
  mHost                              = &host;
  mEntrySourceRevision               = expectedSourceRevision;
  mPixelRevealRequested              = pixelRevealRequested;
  mRevealBindingRequired             = pixelRevealRequested;
  const std::size_t requiredCapacity = mEntries.size() + placements.Count();
  if(requiredCapacity > mEntries.capacity())
  {
    mEntries.reserve(requiredCapacity);
  }

  ++mUpdateGeneration;
  if(mUpdateGeneration == 0u)
  {
    mUpdateGeneration = 1u;
    for(Entry& entry : mEntries)
    {
      entry.lastSeenGeneration = 0u;
    }
  }
  const uint64_t updateGeneration = mUpdateGeneration;

  // Linear lookup is cheaper for the common one-to-few image case. Build an
  // index only for larger updates, avoiding O(N^2) behavior in stress content.
  constexpr std::size_t INDEX_THRESHOLD = 8u;
  const bool            useEntryIndex =
    std::max(mEntries.size(), static_cast<std::size_t>(placements.Count())) > INDEX_THRESHOLD;
  mEntryIndex.clear();
  if(useEntryIndex)
  {
    mEntryIndex.reserve(requiredCapacity);
    for(std::size_t index = 0u; index < mEntries.size(); ++index)
    {
      mEntryIndex.emplace(mEntries[index].occurrenceIdentity, index);
    }
  }

  auto findEntry = [&](uint64_t occurrenceIdentity) -> Entry*
  {
    if(useEntryIndex)
    {
      const auto iterator = mEntryIndex.find(occurrenceIdentity);
      return iterator == mEntryIndex.end() ? nullptr : &mEntries[iterator->second];
    }
    const auto iterator = std::find_if(mEntries.begin(), mEntries.end(), [occurrenceIdentity](const Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    return iterator == mEntries.end() ? nullptr : &*iterator;
  };

  for(const Ui::Text::ReplacementPlacement& placement : placements)
  {
    if(!placement.visible || placement.elided || placement.sourceRunIndex >= source.runs.Count())
    {
      continue;
    }

    const Ui::Text::ReplacementRunSnapshot& run = source.runs[placement.sourceRunIndex];
    if(run.type != Ui::Text::ReplacementType::IMAGE || run.image.source.empty() ||
       run.occurrenceIdentity != placement.occurrenceIdentity || !IsSupportedStaticImageSource(run.image.source))
    {
      continue;
    }

    const Vector2 reservedOffset = placementOffset + placement.position;
    if(!RectanglesIntersect(reservedOffset, placement.size, clipOffset, contentSize))
    {
      continue;
    }
    const RuntimeImageDescriptor descriptor = BuildRuntimeImageDescriptor(run, effectiveScale);
    Entry*                       entry      = findEntry(run.occurrenceIdentity);
    bool                         entryCreated{false};
    if(entry && !IsSameRuntimeImageDescriptor(entry->descriptor, descriptor))
    {
      ReleaseEntryVisual(*entry);
      if(!current)
      {
        return false;
      }
      entry->descriptor = descriptor;
    }

    if(!entry)
    {
      Entry created;
      created.occurrenceIdentity = run.occurrenceIdentity;
      created.descriptor         = descriptor;
      mEntries.push_back(created);
      entry        = &mEntries.back();
      entryCreated = true;
      if(useEntryIndex)
      {
        mEntryIndex.emplace(run.occurrenceIdentity, mEntries.size() - 1u);
      }
    }

    // A texture-backed ImageUrl can report READY synchronously from
    // RegisterVisual(). Publish authoritative placement first so its reentrant
    // Refresh() never observes an entry with zero/default geometry.
    entry->lastSeenGeneration = updateGeneration;
    entry->lineIndex          = placement.lineIndex;
    entry->reservedOffset     = reservedOffset;
    entry->reservedSize       = placement.size;
    entry->clipOffset         = clipOffset;
    entry->clipSize           = contentSize;
    entry->ownerSize          = ownerSize;
    entry->effectiveScale     = effectiveScale;
    const bool created        = entry->visual || CreateEntryVisual(host, *entry);
    if(!current)
    {
      return false;
    }
    if(!created)
    {
      if(entryCreated)
      {
        if(useEntryIndex)
        {
          mEntryIndex.erase(run.occurrenceIdentity);
        }
        mEntries.pop_back();
      }
      else
      {
        entry->lastSeenGeneration = 0u;
      }
      continue;
    }

    // Timing can precede placement in async publication. Keep the shader
    // paired with an existing binding: authored PIXEL may resolve to scalar
    // opacity when its progression span is zero.
    if(mPixelRevealRequested && !entry->revealConstraint)
    {
      SetEntryPixelRevealShader(*entry, true);
      if(!current)
      {
        return false;
      }
    }

    const bool transformed = ApplyEntryTransform(*entry);
    if(!current)
    {
      return false;
    }
    if(!transformed)
    {
      ReleaseEntryVisual(*entry);
      if(!current)
      {
        return false;
      }
    }
    else if(mRevealSourceRevision == expectedSourceRevision && !mRevealTimings.empty())
    {
      const auto bound = ApplyEntryRevealConstraint(*entry);
      if(!current)
      {
        return false;
      }
      if(bound == RevealBindingResult::INVALID)
      {
        ClearReveal();
        return false;
      }
    }
  }

  for(auto iterator = mEntries.begin(); iterator != mEntries.end();)
  {
    if(iterator->lastSeenGeneration != updateGeneration)
    {
      iterator = RemoveEntry(iterator);
      if(!current)
      {
        return false;
      }
    }
    else
    {
      ++iterator;
    }
  }
  return true;
}

bool InlineReplacementManager::ApplyRevealTimings(
  const Vector<Ui::Text::ReplacementRevealTiming>& timings,
  uint64_t                                         sourceRevision,
  Property::Index                                  progressPropertyIndex)
{
  ++mUpdateState->generation;
  UpdateGuard        current(mUpdateState);
  CaptureUpdateScope captureUpdate(*this);
  if(sourceRevision == 0u || progressPropertyIndex == Property::INVALID_INDEX || timings.Empty())
  {
    ClearReveal();
    return timings.Empty();
  }

  std::unordered_map<uint64_t, Ui::Text::ReplacementRevealTiming> validated;
  validated.reserve(timings.Count());
  bool pixelRevealRequested = false;
  for(const Ui::Text::ReplacementRevealTiming& timing : timings)
  {
    if(timing.occurrenceIdentity == 0u || !std::isfinite(timing.start) || !std::isfinite(timing.fadeDuration) ||
       !std::isfinite(timing.progressionSpan) ||
       timing.start < 0.0f || timing.start > 1.0f || timing.fadeDuration < 0.0f || timing.fadeDuration > 1.0f ||
       timing.progressionSpan < 0.0f || timing.progressionSpan > 1.0f ||
       !validated.emplace(timing.occurrenceIdentity, timing).second)
    {
      ClearReveal();
      return false;
    }
    pixelRevealRequested = pixelRevealRequested || timing.progressionSpan > 0.0f;
  }

  mRevealTimings               = std::move(validated);
  mRevealSourceRevision        = sourceRevision;
  mRevealProgressPropertyIndex = progressPropertyIndex;
  mPixelRevealRequested        = pixelRevealRequested;
  mRevealBindingRequired       = pixelRevealRequested;
  if(!mEntries.empty() && mEntrySourceRevision != sourceRevision)
  {
    // Async publication may arrive before the event-thread placement update.
    // Retain the validated timing payload, but never bind it to old entries.
    for(Entry& entry : mEntries)
    {
      RemoveEntryRevealConstraint(entry);
      if(!current)
      {
        return false;
      }
    }
    return true;
  }
  bool valid = true;
  for(Entry& entry : mEntries)
  {
    valid = ApplyEntryRevealConstraint(entry) != RevealBindingResult::INVALID && valid;
    if(!current)
    {
      return false;
    }
  }
  if(!valid)
  {
    ClearReveal();
  }
  return valid;
}

void InlineReplacementManager::RequireRevealBinding(Property::Index progressPropertyIndex, bool fullFade)
{
  const auto state = mUpdateState;
  ++state->generation;
  if(fullFade && mRevealTimings.empty() && mEntrySourceRevision != 0u && !mEntries.empty())
  {
    // All units start together and fade for the whole timeline. These timings
    // are exact for the already published occurrences, not guessed glyph data.
    // Use the existing binding/teardown path, including renderer-ready refresh.
    Vector<Ui::Text::ReplacementRevealTiming> timings;
    timings.Reserve(static_cast<uint32_t>(mEntries.size()));
    for(const auto& entry : mEntries)
    {
      Ui::Text::ReplacementRevealTiming timing;
      timing.occurrenceIdentity = entry.occurrenceIdentity;
      timing.fadeDuration       = 1.0f;
      timings.PushBack(timing);
    }
    if(!ApplyRevealTimings(timings, mEntrySourceRevision, progressPropertyIndex) || !state->alive)
    {
      return;
    }
  }
  UpdateGuard current(state);
  mRevealBindingRequired = true;
  for(Entry& entry : mEntries)
  {
    UpdateEntryVisibility(entry);
    if(!current)
    {
      return;
    }
  }
}

void InlineReplacementManager::ClearReveal()
{
  ++mUpdateState->generation;
  UpdateGuard        current(mUpdateState);
  CaptureUpdateScope captureUpdate(*this);
  mRevealBindingRequired = false;
  for(Entry& entry : mEntries)
  {
    RemoveEntryRevealConstraint(entry);
    if(!current)
    {
      return;
    }
    UpdateEntryVisibility(entry);
    if(!current)
    {
      return;
    }
  }
  mRevealTimings.clear();
  mRevealSourceRevision        = 0u;
  mRevealProgressPropertyIndex = Property::INVALID_INDEX;
}

void InlineReplacementManager::Refresh()
{
  UpdateGuard        current(mUpdateState);
  CaptureUpdateScope captureUpdate(*this);
  if(!mHost || !mHost->GetOwner())
  {
    return;
  }

  for(Entry& entry : mEntries)
  {
    const bool transformed = ApplyEntryTransform(entry);
    if(!current)
    {
      return;
    }
    if(!transformed)
    {
      ReleaseEntryVisual(entry);
      if(!current)
      {
        return;
      }
    }
    else if(!mRevealTimings.empty())
    {
      const auto bound = ApplyEntryRevealConstraint(entry);
      if(!current)
      {
        return;
      }
      if(bound == RevealBindingResult::INVALID)
      {
        ClearReveal();
        break;
      }
    }
  }
}

void InlineReplacementManager::PrepareOwnerDestruction()
{
  ++mUpdateState->generation;
  // ViewDataImpl owns another handle to every registered visual and clears it
  // after the CustomActor implementation has finished destruction. Avoid the
  // normal unregister path here because it requires CustomActorImpl::Self(),
  // and leave each visual to be discarded once by ViewDataImpl.
  if(mBlurCapture)
  {
    if(auto retention = mBlurCapture->retention.GetHandle())
    {
      for(auto& source : mBlurCapture->sources)
      {
        retention.RemoveRenderer(source.renderer);
      }
    }
    mBlurCapture.reset();
  }
  mHost = nullptr;
  for(Entry& entry : mEntries)
  {
    if(entry.revealConstraint)
    {
      entry.revealConstraint.Remove();
      entry.revealConstraint.Reset();
    }
    entry.visual.Reset();
    entry.propertyIndex = Property::INVALID_INDEX;
  }
  mEntries.clear();
  mEntryIndex.clear();
  mRevealTimings.clear();
  mUpdateGeneration            = 0u;
  mEntrySourceRevision         = 0u;
  mRevealSourceRevision        = 0u;
  mRevealProgressPropertyIndex = Property::INVALID_INDEX;
  mPixelRevealRequested        = false;
  mRevealBindingRequired       = false;
}

void InlineReplacementManager::Clear()
{
  const auto state    = mUpdateState;
  const auto expected = state->generation + 1u;
  ClearReveal();
  if(!state->alive || state->generation != expected)
  {
    return;
  }
  UpdateGuard current(state);
  while(!mEntries.empty())
  {
    RemoveEntry(mEntries.begin());
    if(!current)
    {
      return;
    }
  }
  mBlurCapture.reset();
  mHost = nullptr;
  mEntryIndex.clear();
  mUpdateGeneration      = 0u;
  mEntrySourceRevision   = 0u;
  mPixelRevealRequested  = false;
  mRevealBindingRequired = false;
}

std::vector<InlineReplacementManager::BlurCaptureSource> InlineReplacementManager::GetReadyBlurCaptureSources(uint64_t sourceRevision) const
{
  std::vector<BlurCaptureSource> sources;
  if(sourceRevision == 0u || sourceRevision != mEntrySourceRevision || sourceRevision != mRevealSourceRevision)
  {
    return sources;
  }
  for(const auto& entry : mEntries)
  {
    if(entry.visual && entry.currentlyVisible && entry.transformApplied && entry.pixelAreaApplied && entry.revealConstraint &&
       Ui::GetImplementation(entry.visual).GetResourceStatus() == Ui::Visual::ResourceStatus::READY)
    {
      auto renderer = entry.visual.GetRenderer();
      if(renderer && renderer.GetTextures() && renderer.GetTextures().GetTextureCount() > 0u)
      {
        sources.push_back({entry.occurrenceIdentity, entry.lineIndex, renderer});
      }
    }
  }
  return sources;
}

bool InlineReplacementManager::CaptureBlurRenderer(Actor client, uint64_t sourceRevision, const BlurCaptureSource& source, Actor retention)
{
  if(mBlurCapture && !mBlurCapture->client.GetHandle())
  {
    ReleaseBlurCapture({});
  }
  auto owner = mHost ? mHost->GetOwner() : Ui::View();
  if(!client || !owner || !retention || mUpdateState->depth != 0u || sourceRevision != mEntrySourceRevision || sourceRevision != mRevealSourceRevision ||
     (mBlurCapture && mBlurCapture->client.GetHandle() != client))
  {
    return false;
  }
  const auto entry = std::find_if(mEntries.begin(), mEntries.end(), [&](const auto& candidate)
  {
    return candidate.occurrenceIdentity == source.occurrenceIdentity && candidate.visual &&
           candidate.visual.GetRenderer() == source.renderer;
  });
  if(entry == mEntries.end())
  {
    return false;
  }
  if(!mBlurCapture)
  {
    mBlurCapture            = std::make_unique<BlurCaptureState>();
    mBlurCapture->client    = client;
    mBlurCapture->retention = retention;
  }
  auto& sources = mBlurCapture->sources;
  auto  found   = std::find_if(sources.begin(), sources.end(), [&](const auto& candidate)
     {
    return candidate.occurrenceIdentity == source.occurrenceIdentity;
  });
  if(found == sources.end())
  {
    sources.push_back(source);
  }
  else
  {
    if(found->renderer != source.renderer)
    {
      retention.RemoveRenderer(found->renderer);
    }
    *found = source;
  }
  // Visual mutations release capture first; repeated capture remains idempotent.
  auto renderer = source.renderer;
  owner.RemoveRenderer(renderer);
  retention.AddRenderer(renderer);
  return true;
}

void InlineReplacementManager::ReleaseBlurCapture(Actor client)
{
  if(!mBlurCapture || mBlurCapture->client.GetHandle() != client)
  {
    return;
  }
  auto capture   = std::move(mBlurCapture);
  auto owner     = mHost ? mHost->GetOwner() : Ui::View();
  auto retention = capture->retention.GetHandle();
  for(auto& source : capture->sources)
  {
    if(retention)
    {
      retention.RemoveRenderer(source.renderer);
    }
    const auto entry = std::find_if(mEntries.begin(), mEntries.end(), [&](const auto& candidate)
    {
      return candidate.occurrenceIdentity == source.occurrenceIdentity && candidate.visual &&
             candidate.visual.GetRenderer() == source.renderer;
    });
    if(owner && entry != mEntries.end())
    {
      auto renderer = source.renderer;
      owner.AddRenderer(renderer);
    }
  }
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

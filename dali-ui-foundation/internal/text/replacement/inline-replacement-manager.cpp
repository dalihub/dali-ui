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
#include <dali/public-api/math/math-utils.h>
#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
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
    const float progress    = std::max(0.0f, std::min(1.0f, inputs[1]->GetFloat()));
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

InlineReplacementManager::~InlineReplacementManager()
{
  Clear();
}

std::vector<InlineReplacementManager::Entry>::iterator InlineReplacementManager::RemoveEntry(
  std::vector<Entry>::iterator iterator)
{
  ReleaseEntryVisual(*iterator);
  return mEntries.erase(iterator);
}

void InlineReplacementManager::ReleaseEntryVisual(Entry& entry)
{
  RemoveEntryRevealConstraint(entry);
  if(mHost && entry.propertyIndex != Property::INVALID_INDEX)
  {
    mHost->UnregisterVisual(entry.propertyIndex);
    mHost->ReleaseVisualSlot(entry.propertyIndex);
  }
  DiscardVisual(entry.visual);
  entry.propertyIndex = Property::INVALID_INDEX;
  ResetEntryResourceState(entry);
}

bool InlineReplacementManager::CreateEntryVisual(InlineReplacementViewHost& host, Entry& entry)
{
  entry.propertyIndex = host.AllocateVisualSlot();
  if(entry.propertyIndex == Property::INVALID_INDEX)
  {
    return false;
  }

  Property::Map visualMap;
  visualMap.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  visualMap.Insert(Ui::ImageVisualPropertyIndex::URL, Dali::String(entry.descriptor.source.c_str()));
  visualMap.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, entry.descriptor.desiredWidth);
  visualMap.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, entry.descriptor.desiredHeight);
  visualMap.Insert(Ui::ImageVisualPropertyIndex::FITTING_MODE,
                   static_cast<int>(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO));
  visualMap.Insert(Ui::ImageVisualPropertyIndex::ORIENTATION_CORRECTION, true);
  entry.visual = Ui::Integration::VisualFactory::Get().CreateVisual(
    visualMap,
    Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
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
  opacityMap.Insert(Ui::VisualBasePropertyIndex::OPACITY, 0.0f);
  entry.visual.SetProperties(opacityMap);
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
  if(entry.revealConstraint && renderer && entry.revealBaseOpacityIndex != Property::INVALID_INDEX)
  {
    renderer.SetProperty(entry.revealBaseOpacityIndex, visible ? 1.0f : 0.0f);
  }
  else
  {
    Property::Map opacityMap;
    opacityMap.Insert(Ui::VisualBasePropertyIndex::OPACITY, visible ? 1.0f : 0.0f);
    entry.visual.SetProperties(opacityMap);
  }
}

void InlineReplacementManager::RemoveEntryRevealConstraint(Entry& entry)
{
  if(entry.revealConstraint)
  {
    entry.revealConstraint.Remove();
    entry.revealConstraint.Reset();
  }
  entry.revealProgressPropertyIndex = Property::INVALID_INDEX;
  if(entry.visual)
  {
    Property::Map opacityMap;
    opacityMap.Insert(Ui::VisualBasePropertyIndex::OPACITY, entry.currentlyVisible ? 1.0f : 0.0f);
    entry.visual.SetProperties(opacityMap);
  }
}

bool InlineReplacementManager::ApplyEntryRevealConstraint(Entry& entry)
{
  if(!mHost || !entry.visual || mRevealProgressPropertyIndex == Property::INVALID_INDEX)
  {
    RemoveEntryRevealConstraint(entry);
    return false;
  }
  const auto timing = mRevealTimings.find(entry.occurrenceIdentity);
  if(timing == mRevealTimings.end())
  {
    RemoveEntryRevealConstraint(entry);
    return false;
  }
  Ui::View       owner    = mHost->GetOwner();
  VisualRenderer renderer = entry.visual.GetRenderer();
  if(!owner || !renderer)
  {
    RemoveEntryRevealConstraint(entry);
    return false;
  }

  const float start        = timing->second.start;
  const float fadeDuration = timing->second.fadeDuration;
  if(entry.revealConstraint && entry.revealProgressPropertyIndex == mRevealProgressPropertyIndex &&
     Dali::Equals(entry.revealStart, start) && Dali::Equals(entry.revealFadeDuration, fadeDuration))
  {
    return true;
  }
  RemoveEntryRevealConstraint(entry);
  entry.revealBaseOpacityIndex = renderer.GetPropertyIndex(INLINE_REPLACEMENT_REVEAL_BASE_OPACITY);
  if(entry.revealBaseOpacityIndex == Property::INVALID_INDEX)
  {
    entry.revealBaseOpacityIndex = renderer.RegisterProperty(INLINE_REPLACEMENT_REVEAL_BASE_OPACITY,
                                                             entry.currentlyVisible ? 1.0f : 0.0f);
  }
  else
  {
    renderer.SetProperty(entry.revealBaseOpacityIndex, entry.currentlyVisible ? 1.0f : 0.0f);
  }
  if(entry.revealBaseOpacityIndex == Property::INVALID_INDEX)
  {
    return false;
  }

  entry.revealConstraint = Constraint::New<float>(renderer,
                                                  Dali::DevelRenderer::Property::OPACITY,
                                                  ReplacementRevealOpacityConstraint(start, fadeDuration));
  entry.revealConstraint.AddSource(Source(renderer, entry.revealBaseOpacityIndex));
  entry.revealConstraint.AddSource(Source(owner, mRevealProgressPropertyIndex));
  entry.revealConstraint.SetApplyRate(Dali::Constraint::APPLY_ALWAYS);
  entry.revealConstraint.Apply();
  entry.revealProgressPropertyIndex = mRevealProgressPropertyIndex;
  entry.revealStart                 = start;
  entry.revealFadeDuration          = fadeDuration;
  return true;
}

bool InlineReplacementManager::ApplyEntryTransform(Entry& entry)
{
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
  if(!readyToReveal)
  {
    SetEntryVisible(entry, false);
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
    pixelAreaMap.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, pixelArea);
    entry.visual.SetProperties(pixelAreaMap);
    entry.lastPixelArea    = pixelArea;
    entry.pixelAreaApplied = true;
  }

  if(!entry.transformApplied || entry.lastTransformOffset != visualOffset || entry.lastTransformSize != visualSize ||
     entry.lastOwnerSize != entry.ownerSize || entry.lastEffectiveScale != entry.effectiveScale)
  {
    Property::Map transform;
    transform.Add(Ui::Visual::Transform::Property::SIZE, visualSize)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::OFFSET, visualOffset)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::PIVOT, Ui::Align::TOP_BEGIN);
    visualImpl.SetTransformAndSize(transform, entry.ownerSize, entry.effectiveScale);
    entry.lastTransformOffset = visualOffset;
    entry.lastTransformSize   = visualSize;
    entry.lastOwnerSize       = entry.ownerSize;
    entry.lastEffectiveScale  = entry.effectiveScale;
    entry.transformApplied    = true;
  }

  // Geometry and sampling are committed before the first visible frame.
  SetEntryVisible(entry, readyToReveal);
  return true;
}

bool InlineReplacementManager::Update(InlineReplacementViewHost&                    host,
                                      const Ui::Text::ReplacementSourceSnapshot&    source,
                                      const Vector<Ui::Text::ReplacementPlacement>& placements,
                                      const Vector2&                                contentOffset,
                                      const Vector2&                                contentSize,
                                      const Vector2&                                ownerSize,
                                      float                                         effectiveScale,
                                      uint64_t                                      expectedSourceRevision)
{
  return Update(host,
                source,
                placements,
                contentOffset,
                contentOffset,
                contentSize,
                ownerSize,
                effectiveScale,
                expectedSourceRevision);
}

bool InlineReplacementManager::Update(InlineReplacementViewHost&                    host,
                                      const Ui::Text::ReplacementSourceSnapshot&    source,
                                      const Vector<Ui::Text::ReplacementPlacement>& placements,
                                      const Vector2&                                placementOffset,
                                      const Vector2&                                clipOffset,
                                      const Vector2&                                contentSize,
                                      const Vector2&                                ownerSize,
                                      float                                         effectiveScale,
                                      uint64_t                                      expectedSourceRevision)
{
  Ui::View owner = host.GetOwner();
  if(!owner || source.sourceRevision != expectedSourceRevision)
  {
    return false;
  }
  if(!mRevealTimings.empty() && mRevealSourceRevision != expectedSourceRevision)
  {
    ClearReveal();
  }
  if(mHost && mHost != &host)
  {
    Clear();
  }
  mHost                              = &host;
  mEntrySourceRevision               = expectedSourceRevision;
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
    if(entry && !IsSameRuntimeImageDescriptor(entry->descriptor, descriptor))
    {
      ReleaseEntryVisual(*entry);
      entry->descriptor = descriptor;
      if(!CreateEntryVisual(host, *entry))
      {
        continue;
      }
    }

    if(!entry)
    {
      Entry created;
      created.occurrenceIdentity = run.occurrenceIdentity;
      created.descriptor         = descriptor;
      mEntries.push_back(created);
      entry = &mEntries.back();
      if(!CreateEntryVisual(host, *entry))
      {
        mEntries.pop_back();
        continue;
      }
      if(useEntryIndex)
      {
        mEntryIndex.emplace(run.occurrenceIdentity, mEntries.size() - 1u);
      }
    }

    entry->lastSeenGeneration = updateGeneration;
    entry->reservedOffset     = reservedOffset;
    entry->reservedSize       = placement.size;
    entry->clipOffset         = clipOffset;
    entry->clipSize           = contentSize;
    entry->ownerSize          = ownerSize;
    entry->effectiveScale     = effectiveScale;
    if(!ApplyEntryTransform(*entry))
    {
      ReleaseEntryVisual(*entry);
    }
    else if(mRevealSourceRevision == expectedSourceRevision && !mRevealTimings.empty())
    {
      ApplyEntryRevealConstraint(*entry);
    }
  }

  for(auto iterator = mEntries.begin(); iterator != mEntries.end();)
  {
    if(iterator->lastSeenGeneration != updateGeneration)
    {
      iterator = RemoveEntry(iterator);
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
  if(sourceRevision == 0u || progressPropertyIndex == Property::INVALID_INDEX || timings.Empty())
  {
    ClearReveal();
    return timings.Empty();
  }

  std::unordered_map<uint64_t, Ui::Text::ReplacementRevealTiming> validated;
  validated.reserve(timings.Count());
  for(const Ui::Text::ReplacementRevealTiming& timing : timings)
  {
    if(timing.occurrenceIdentity == 0u || !std::isfinite(timing.start) || !std::isfinite(timing.fadeDuration) ||
       timing.start < 0.0f || timing.start > 1.0f || timing.fadeDuration < 0.0f || timing.fadeDuration > 1.0f ||
       !validated.emplace(timing.occurrenceIdentity, timing).second)
    {
      ClearReveal();
      return false;
    }
  }

  mRevealTimings               = std::move(validated);
  mRevealSourceRevision        = sourceRevision;
  mRevealProgressPropertyIndex = progressPropertyIndex;
  if(!mEntries.empty() && mEntrySourceRevision != sourceRevision)
  {
    // Async publication may arrive before the event-thread placement update.
    // Retain the validated timing payload, but never bind it to old entries.
    for(Entry& entry : mEntries)
    {
      RemoveEntryRevealConstraint(entry);
    }
    return true;
  }
  bool complete = true;
  for(Entry& entry : mEntries)
  {
    complete = ApplyEntryRevealConstraint(entry) && complete;
  }
  if(!complete)
  {
    ClearReveal();
  }
  return complete;
}

void InlineReplacementManager::ClearReveal()
{
  for(Entry& entry : mEntries)
  {
    RemoveEntryRevealConstraint(entry);
  }
  mRevealTimings.clear();
  mRevealSourceRevision        = 0u;
  mRevealProgressPropertyIndex = Property::INVALID_INDEX;
}

void InlineReplacementManager::Refresh()
{
  if(!mHost || !mHost->GetOwner())
  {
    return;
  }

  for(Entry& entry : mEntries)
  {
    if(!ApplyEntryTransform(entry))
    {
      ReleaseEntryVisual(entry);
    }
    else if(!mRevealTimings.empty())
    {
      ApplyEntryRevealConstraint(entry);
    }
  }
}

void InlineReplacementManager::PrepareOwnerDestruction()
{
  // ViewDataImpl owns another handle to every registered visual and clears it
  // after the CustomActor implementation has finished destruction. Avoid the
  // normal unregister path here because it requires CustomActorImpl::Self(),
  // and leave each visual to be discarded once by ViewDataImpl.
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
}

void InlineReplacementManager::Clear()
{
  ClearReveal();
  while(!mEntries.empty())
  {
    RemoveEntry(mEntries.begin());
  }
  mHost = nullptr;
  mEntryIndex.clear();
  mUpdateGeneration    = 0u;
  mEntrySourceRevision = 0u;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

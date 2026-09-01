#pragma once

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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/weak-handle.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{
class InlineReplacementManagerTestAccessor;

/**
 * @brief Registers inline replacement visuals for an owning view.
 */
class InlineReplacementViewHost
{
public:
  /**
   * @brief Creates a registered-visual host.
   *
   * @param[in] owner The view that owns the visuals.
   * @param[in] contentDepth The depth used for registered visuals.
   */
  InlineReplacementViewHost(Ui::View owner, int contentDepth);

  /**
   * @brief Destroys the registered-visual host.
   */
  ~InlineReplacementViewHost() = default;

  InlineReplacementViewHost(const InlineReplacementViewHost&)            = delete;
  InlineReplacementViewHost& operator=(const InlineReplacementViewHost&) = delete;

  /**
   * @brief Allocates a reusable visual property index.
   *
   * @return The allocated property index.
   */
  Property::Index AllocateVisualSlot();

  /**
   * @brief Releases a visual property index.
   *
   * @param[in] index The property index to release.
   */
  void ReleaseVisualSlot(Property::Index index);

  /**
   * @brief Registers a visual at a property index.
   *
   * @param[in] index The property index.
   * @param[in] visual The visual to register.
   */
  void RegisterVisual(Property::Index index, Ui::Integration::Visual::Base& visual);

  /**
   * @brief Unregisters a visual.
   *
   * @param[in] index The property index of the visual.
   */
  void UnregisterVisual(Property::Index index);

  /**
   * @brief Gets the visual owner.
   *
   * @return The owner, or an empty handle if it no longer exists.
   */
  Ui::View GetOwner() const;

private:
  WeakHandle<Ui::View>         mOwner;
  std::vector<Property::Index> mFreeVisualSlots;
  uint64_t                     mNextSlotName{0u};
  int                          mContentDepth{0};
};

/**
 * @brief Manages inline replacement visuals for final text placements.
 */
class InlineReplacementManager
{
public:
  /**
   * @brief Creates an inline replacement manager.
   */
  InlineReplacementManager();

  /**
   * @brief Destroys the inline replacement manager.
   */
  ~InlineReplacementManager();

  InlineReplacementManager(const InlineReplacementManager&)            = delete;
  InlineReplacementManager& operator=(const InlineReplacementManager&) = delete;

  /**
   * @brief Applies a current final-layout snapshot on the event thread.
   *
   * Mismatched source/layout generations are rejected without changing the
   * current visuals. Elided and invisible occurrences are not materialized.
   *
   * @param[in] host The owner used to register replacement visuals.
   * @param[in] source The authored replacement source.
   * @param[in] placements The placements produced by the final layout.
   * @param[in] contentOffset The text content offset in owner coordinates.
   * @param[in] contentSize The visible text content size.
   * @param[in] ownerSize The size of the visual owner.
   * @param[in] effectiveScale The effective visual scale.
   * @param[in] expectedSourceRevision The source revision accepted by the owner.
   * @return true if the source revision is valid.
   */
  bool Update(InlineReplacementViewHost&                    host,
              const Ui::Text::ReplacementSourceSnapshot&    source,
              const Vector<Ui::Text::ReplacementPlacement>& placements,
              const Vector2&                                contentOffset,
              const Vector2&                                contentSize,
              const Vector2&                                ownerSize,
              float                                         effectiveScale,
              uint64_t                                      expectedSourceRevision);

  /**
   * @brief Applies a shared Label progress binding to visible image visuals.
   *
   * The source revision and occurrence identities form the publication key.
   * Empty or incomplete timing clears all replacement Reveal bindings.
   */
  bool ApplyRevealTimings(const Vector<Ui::Text::ReplacementRevealTiming>& timings,
                          uint64_t                                         sourceRevision,
                          Property::Index                                  progressPropertyIndex);

  /**
   * @brief Removes replacement Reveal constraints and restores resource-ready visibility.
   */
  void ClearReveal();

  /**
   * @brief Applies a current final-layout snapshot with independent placement and clip offsets.
   *
   * @param[in] host The owner used to register replacement visuals.
   * @param[in] source The authored replacement source.
   * @param[in] placements The placements produced by the final layout.
   * @param[in] placementOffset The placement offset in owner coordinates.
   * @param[in] clipOffset The visible content offset in owner coordinates.
   * @param[in] contentSize The visible text content size.
   * @param[in] ownerSize The size of the visual owner.
   * @param[in] effectiveScale The effective visual scale.
   * @param[in] expectedSourceRevision The source revision accepted by the owner.
   * @return true if the source revision is valid.
   */
  bool Update(InlineReplacementViewHost&                    host,
              const Ui::Text::ReplacementSourceSnapshot&    source,
              const Vector<Ui::Text::ReplacementPlacement>& placements,
              const Vector2&                                placementOffset,
              const Vector2&                                clipOffset,
              const Vector2&                                contentSize,
              const Vector2&                                ownerSize,
              float                                         effectiveScale,
              uint64_t                                      expectedSourceRevision);

  /**
   * @brief Removes and discards all registered visuals.
   */
  void Clear();

  /**
   * @brief Re-applies fixed reserved-box transforms after resources become ready.
   *
   * This updates image aspect fitting only; it does not request text relayout.
   */
  void Refresh();

  /**
   * @brief Releases manager ownership before the view destroys its visuals.
   */
  void PrepareOwnerDestruction();

private:
  friend class InlineReplacementManagerTestAccessor;

  struct RuntimeImageDescriptor
  {
    std::string source;
    int32_t     desiredWidth{0};
    int32_t     desiredHeight{0};
  };

  struct Entry
  {
    uint64_t                      occurrenceIdentity{0u};
    RuntimeImageDescriptor        descriptor;
    Property::Index               propertyIndex{Property::INVALID_INDEX};
    Ui::Integration::Visual::Base visual;
    Vector2                       reservedOffset{Vector2::ZERO};
    Vector2                       reservedSize{Vector2::ZERO};
    Vector2                       clipOffset{Vector2::ZERO};
    Vector2                       clipSize{Vector2::ZERO};
    Vector2                       ownerSize{Vector2::ZERO};
    float                         effectiveScale{1.0f};
    uint64_t                      lastSeenGeneration{0u};
    Vector2                       naturalSize{Vector2::ZERO};
    Vector2                       lastTransformOffset{Vector2::ZERO};
    Vector2                       lastTransformSize{Vector2::ZERO};
    Vector2                       lastOwnerSize{Vector2::ZERO};
    Vector4                       lastPixelArea{Vector4::ZERO};
    float                         lastEffectiveScale{0.0f};
    bool                          aspectResolved{false};
    bool                          currentlyVisible{false};
    bool                          transformApplied{false};
    bool                          pixelAreaApplied{false};
    Constraint                    revealConstraint;
    Property::Index               revealBaseOpacityIndex{Property::INVALID_INDEX};
    Property::Index               revealProgressPropertyIndex{Property::INVALID_INDEX};
    float                         revealStart{0.0f};
    float                         revealFadeDuration{0.0f};
  };

  std::vector<Entry>::iterator  RemoveEntry(std::vector<Entry>::iterator iterator);
  void                          ReleaseEntryVisual(Entry& entry);
  bool                          CreateEntryVisual(InlineReplacementViewHost& host, Entry& entry);
  bool                          ApplyEntryTransform(Entry& entry);
  void                          SetEntryVisible(Entry& entry, bool visible);
  void                          RemoveEntryRevealConstraint(Entry& entry);
  bool                          ApplyEntryRevealConstraint(Entry& entry);
  static void                   ResetEntryResourceState(Entry& entry);
  static RuntimeImageDescriptor BuildRuntimeImageDescriptor(const Ui::Text::ReplacementRunSnapshot& run,
                                                            float                                   effectiveScale);
  static bool                   IsSameRuntimeImageDescriptor(const RuntimeImageDescriptor& lhs,
                                                             const RuntimeImageDescriptor& rhs);

private:
  InlineReplacementViewHost*                                      mHost{nullptr};
  std::vector<Entry>                                              mEntries;
  std::unordered_map<uint64_t, std::size_t>                       mEntryIndex;
  std::unordered_map<uint64_t, Ui::Text::ReplacementRevealTiming> mRevealTimings;
  uint64_t                                                        mUpdateGeneration{0u};
  uint64_t                                                        mEntrySourceRevision{0u};
  uint64_t                                                        mRevealSourceRevision{0u};
  Property::Index                                                 mRevealProgressPropertyIndex{Property::INVALID_INDEX};
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

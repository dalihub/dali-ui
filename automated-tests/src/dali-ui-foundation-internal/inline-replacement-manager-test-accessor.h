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

#ifndef DALI_UI_INLINE_REPLACEMENT_MANAGER_TEST_ACCESSOR_H
#define DALI_UI_INLINE_REPLACEMENT_MANAGER_TEST_ACCESSOR_H

#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>

#include <algorithm>
#include <cstddef>

namespace Dali::Ui::Internal::Text
{
class InlineReplacementManagerTestAccessor
{
public:
  static std::size_t GetRevealConstraintCount(const InlineReplacementManager& manager)
  {
    return static_cast<std::size_t>(std::count_if(manager.mEntries.begin(),
                                                  manager.mEntries.end(),
                                                  [](const InlineReplacementManager::Entry& entry)
    {
      return static_cast<bool>(entry.revealConstraint);
    }));
  }

  static std::size_t GetRevealTimingCount(const InlineReplacementManager& manager)
  {
    return manager.mRevealTimings.size();
  }

  static std::size_t GetEntryCount(const InlineReplacementManager& manager)
  {
    return manager.mEntries.size();
  }

  static bool HasHost(const InlineReplacementManager& manager)
  {
    return manager.mHost != nullptr;
  }

  static Constraint GetRevealConstraint(const InlineReplacementManager& manager,
                                        uint64_t                        occurrenceIdentity)
  {
    const auto iterator = std::find_if(manager.mEntries.begin(),
                                       manager.mEntries.end(),
                                       [occurrenceIdentity](const InlineReplacementManager::Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    return iterator == manager.mEntries.end() ? Constraint{} : iterator->revealConstraint;
  }

  static Property::Index GetRevealBaseOpacityIndex(const InlineReplacementManager& manager,
                                                   uint64_t                        occurrenceIdentity)
  {
    const auto iterator = std::find_if(manager.mEntries.begin(),
                                       manager.mEntries.end(),
                                       [occurrenceIdentity](const InlineReplacementManager::Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    return iterator == manager.mEntries.end() ? Property::INVALID_INDEX : iterator->revealBaseOpacityIndex;
  }

  static bool IsRevealPixelSpatial(const InlineReplacementManager& manager,
                                   uint64_t                        occurrenceIdentity)
  {
    const auto iterator = std::find_if(manager.mEntries.begin(),
                                       manager.mEntries.end(),
                                       [occurrenceIdentity](const InlineReplacementManager::Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    return iterator != manager.mEntries.end() && iterator->revealPixelSpatial;
  }

  static bool GetRevealTiming(const InlineReplacementManager&    manager,
                              uint64_t                           occurrenceIdentity,
                              Ui::Text::ReplacementRevealTiming& timing)
  {
    const auto iterator = manager.mRevealTimings.find(occurrenceIdentity);
    if(iterator == manager.mRevealTimings.end())
    {
      return false;
    }
    timing = iterator->second;
    return true;
  }

  static uint64_t GetEntrySourceRevision(const InlineReplacementManager& manager)
  {
    return manager.mEntrySourceRevision;
  }

  static uint64_t GetRevealSourceRevision(const InlineReplacementManager& manager)
  {
    return manager.mRevealSourceRevision;
  }

  static bool IsRevealBindingRequired(const InlineReplacementManager& manager)
  {
    return manager.mRevealBindingRequired;
  }

  static bool IsEntryVisible(const InlineReplacementManager& manager,
                             uint64_t                        occurrenceIdentity)
  {
    const auto iterator = std::find_if(manager.mEntries.begin(),
                                       manager.mEntries.end(),
                                       [occurrenceIdentity](const InlineReplacementManager::Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    return iterator != manager.mEntries.end() && iterator->currentlyVisible;
  }

  static void ResetEntryGeometry(InlineReplacementManager& manager,
                                 uint64_t                  occurrenceIdentity)
  {
    const auto iterator = std::find_if(manager.mEntries.begin(),
                                       manager.mEntries.end(),
                                       [occurrenceIdentity](const InlineReplacementManager::Entry& entry)
    {
      return entry.occurrenceIdentity == occurrenceIdentity;
    });
    if(iterator != manager.mEntries.end())
    {
      iterator->transformApplied = false;
      iterator->pixelAreaApplied = false;
      iterator->lastPixelArea    = Vector4::ZERO;
    }
  }
};
} // namespace Dali::Ui::Internal::Text

#endif // DALI_UI_INLINE_REPLACEMENT_MANAGER_TEST_ACCESSOR_H

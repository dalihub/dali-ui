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
#include <dali-ui-foundation/internal/visuals/gradient/gradient-texture-cache.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/free-list.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/processor-interface.h>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/gradient/gradient.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
// Hash quantized integers, then compare original values on hash collisions.
size_t HashStops(const Vector<Gradient::GradientStop>& stops, bool repeat)
{
  size_t hash    = stops.Count();
  auto   Combine = [&hash](uint32_t value)
  {
    hash ^= value + 0x9e3779b9u + (hash << 6u) + (hash >> 2u);
  };
  Combine(repeat);
  for(const auto& stop : stops)
  {
    Combine(static_cast<uint32_t>(Clamp(stop.mOffset, 0.f, 1.f) * 65535.f));
    for(uint32_t component = 0; component < 4u; ++component)
    {
      Combine(static_cast<uint32_t>(Clamp(stop.mStopColor[component], 0.f, 1.f) * 255.f));
    }
  }
  return hash;
}

bool EqualStops(const Vector<Gradient::GradientStop>& lhs, const Vector<Gradient::GradientStop>& rhs)
{
  if(lhs.Count() != rhs.Count())
  {
    return false;
  }
  for(uint32_t i = 0u; i < lhs.Count(); ++i)
  {
    if(lhs[i].mOffset != rhs[i].mOffset)
    {
      return false;
    }
    for(uint32_t component = 0u; component < 4u; ++component)
    {
      if(lhs[i].mStopColor[component] != rhs[i].mStopColor[component])
      {
        return false;
      }
    }
  }
  return true;
}
} // namespace

/**
 * @brief Owns gradient cache entries, ID generation and deferred release processing.
 */
struct GradientTextureCache::Impl : public Dali::Integration::Processor, public std::enable_shared_from_this<Impl>
{
  /**
   * @brief Stores the lookup key, texture and reference count for one cache entry.
   */
  struct Entry
  {
    Vector<Gradient::GradientStop> stops;
    Texture                        texture;
    size_t                         hash{0u};
    Id                             id{INVALID_ID};
    uint32_t                       references{0u};
    bool                           repeat{false};
  };

  /**
   * @brief Resolves an active ID to its index in the contiguous entry container.
   * @param[in] id The lookup texture ID
   * @return The entry index, or INVALID_INDEX for an invalid or released ID
   */
  uint32_t GetEntryIndex(Id id) const
  {
    const uint32_t key = static_cast<uint32_t>(id);
    if(id == INVALID_ID || key >= mAllocatedIdCount)
    {
      return INVALID_INDEX;
    }
    const uint32_t index = mIds[key];
    if(index >= mEntries.size() || mEntries[index].id != id)
    {
      return INVALID_INDEX;
    }
    return index;
  }

  /**
   * @brief Queues a reference release and schedules post-processing if needed.
   * @param[in] id The cached lookup texture ID
   */
  void RequestRelease(Id id)
  {
    if(GetEntryIndex(id) == INVALID_INDEX)
    {
      return;
    }
    mReleaseQueue.push_back(id);
    if(!mReleaseProcessorRegistered)
    {
      mReleaseProcessorRegistered = true;
      Dali::Adaptor::Get().RegisterProcessorOnce(*this, true);
    }
  }

private:
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override
  {
    // A texture destruction observer can destroy the owning factory or enqueue
    // further releases. Keep this invocation alive and consume IDs before release.
    auto keepAlive = shared_from_this();
    while(!mReleaseQueue.empty())
    {
      const Id id = mReleaseQueue.back();
      mReleaseQueue.pop_back();
      Release(id);
    }
    mReleaseProcessorRegistered = false;
  }

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "GradientTextureCache";
  }

  /**
   * @brief Releases one reference and removes an entry when its reference count reaches zero.
   * @param[in] id The lookup texture ID queued for release
   */
  void Release(Id id)
  {
    const uint32_t index = GetEntryIndex(id);
    if(index == INVALID_INDEX)
    {
      return;
    }
    auto& entry = mEntries[index];
    if(--entry.references != 0u)
    {
      return;
    }

    // Finish removing the ID before dropping the last Texture handle: its
    // destruction observers may reenter Acquire() and reuse this slot.
    Texture retiredTexture = std::move(entry.texture);
    auto    bucket         = mHashIds.find(entry.hash);
    auto&   ids            = bucket->second;
    auto    found          = std::find(ids.begin(), ids.end(), id);
    std::iter_swap(found, ids.end() - 1);
    ids.pop_back();
    if(ids.empty())
    {
      mHashIds.erase(bucket);
    }

    // Update the moved entry's ID mapping before swapping it into the hole.
    if(index + 1u < mEntries.size())
    {
      mIds[static_cast<uint32_t>(mEntries.back().id)] = index;
      std::swap(mEntries[index], mEntries.back());
    }
    mEntries.pop_back();
    mIds.Remove(static_cast<uint32_t>(id));
  }

  friend class GradientTextureCache;

  static constexpr uint32_t INVALID_INDEX = static_cast<uint32_t>(INVALID_ID);

  std::unordered_map<size_t, std::vector<Id>> mHashIds;                           ///< Hash buckets of stable lookup IDs.
  std::vector<Entry>                          mEntries;                           ///< Dense entries; removal swaps with the final element.
  std::vector<Id>                             mReleaseQueue;                      ///< References awaiting post-processing.
  Dali::FreeList                              mIds;                               ///< Converts an active ID to its current entry index.
  uint32_t                                    mAllocatedIdCount{0u};              ///< Upper bound for IDs ever allocated by the free list.
  bool                                        mReleaseProcessorRegistered{false}; ///< Whether a post-processor is registered.
};

GradientTextureCache::GradientTextureCache()
: mImpl(std::make_shared<Impl>())
{
}

GradientTextureCache::~GradientTextureCache()
{
  if(mImpl->mReleaseProcessorRegistered && Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().UnregisterProcessorOnce(*mImpl, true);
  }
}

GradientTextureCache::Id GradientTextureCache::Acquire(Gradient& gradient)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return INVALID_ID;
  }
  gradient.NormalizeStops();
  const auto& stops = gradient.GetStops();
  if(stops.Empty())
  {
    return INVALID_ID;
  }
  const bool repeat = gradient.GetSpreadMethod() == Ui::Gradient::SpreadMethod::REPEAT &&
                      (stops[0].mOffset > 0.f || stops[stops.Count() - 1u].mOffset < 1.f);
  const size_t hash  = HashStops(stops, repeat);
  auto         found = mImpl->mHashIds.find(hash);
  if(found != mImpl->mHashIds.end())
  {
    for(Id id : found->second)
    {
      auto& entry = mImpl->mEntries[mImpl->mIds[static_cast<uint32_t>(id)]];
      if(entry.repeat == repeat && EqualStops(entry.stops, stops))
      {
        ++entry.references;
        return entry.id;
      }
    }
  }
  const auto index = static_cast<uint32_t>(mImpl->mEntries.size());
  const auto key   = mImpl->mIds.Add(index);
  const Id   id    = static_cast<Id>(key);
  DALI_ASSERT_ALWAYS(id != INVALID_ID && "Gradient texture cache ID capacity exceeded");
  mImpl->mAllocatedIdCount = std::max(mImpl->mAllocatedIdCount, key + 1u);
  mImpl->mEntries.push_back(Impl::Entry{stops, gradient.GenerateLookupTextureFromNormalizedStops(), hash, id, 1u, repeat});
  mImpl->mHashIds[hash].push_back(id);
  return id;
}

Texture GradientTextureCache::GetTexture(Id id) const
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return Texture();
  }
  const uint32_t index = mImpl->GetEntryIndex(id);
  if(index != Impl::INVALID_INDEX)
  {
    return mImpl->mEntries[index].texture;
  }
  return Texture();
}

void GradientTextureCache::RequestRelease(Id id)
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return;
  }
  mImpl->RequestRelease(id);
}

} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE

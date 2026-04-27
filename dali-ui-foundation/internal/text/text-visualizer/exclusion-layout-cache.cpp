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

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-visualizer/exclusion-layout-cache.h>

namespace Dali::Ui::Internal::TextVisualizer
{
ExclusionLayoutCache::ExclusionLayoutCache()
: mSortedRegions(),
  mVersion(0u)
{
}

void ExclusionLayoutCache::Clear()
{
  mSortedRegions.clear();
  ++mVersion;
}

void ExclusionLayoutCache::SetRegions(const Dali::Vector<Rect<float>>& regions)
{
  mSortedRegions.clear();
  mSortedRegions.reserve(regions.Count());

  for(uint32_t index = 0u; index < regions.Count(); ++index)
  {
    const Rect<float>& region = regions[index];
    const float        bottom = region.y + region.height;

    SortedExclusionRegion sortedRegion;
    sortedRegion.rect   = region;
    sortedRegion.top    = std::min(region.y, bottom);
    sortedRegion.bottom = std::max(region.y, bottom);
    mSortedRegions.push_back(sortedRegion);
  }

  std::sort(mSortedRegions.begin(), mSortedRegions.end(), [](const SortedExclusionRegion& lhs, const SortedExclusionRegion& rhs)
  {
    if(lhs.top == rhs.top)
    {
      return lhs.bottom < rhs.bottom;
    }
    return lhs.top < rhs.top;
  });

  ++mVersion;
}

bool ExclusionLayoutCache::Empty() const
{
  return mSortedRegions.empty();
}

uint32_t ExclusionLayoutCache::Count() const
{
  return static_cast<uint32_t>(mSortedRegions.size());
}

uint64_t ExclusionLayoutCache::GetVersion() const
{
  return mVersion;
}

const SortedExclusionRegions& ExclusionLayoutCache::GetSortedRegions() const
{
  return mSortedRegions;
}

} // namespace Dali::Ui::Internal::TextVisualizer

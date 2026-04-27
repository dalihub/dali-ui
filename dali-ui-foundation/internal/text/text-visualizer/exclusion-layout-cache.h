#ifndef DALI_UI_TEXT_VISUALIZER_EXCLUSION_LAYOUT_CACHE_H
#define DALI_UI_TEXT_VISUALIZER_EXCLUSION_LAYOUT_CACHE_H

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
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/rect.h>

namespace Dali::Ui::Internal::TextVisualizer
{
struct SortedExclusionRegion
{
  Rect<float> rect;
  float       top{0.0f};
  float       bottom{0.0f};
};

using SortedExclusionRegions = std::vector<SortedExclusionRegion>;

/**
 * @brief Stores y-sorted exclusion regions derived from the raw exclusion rects.
 *
 * The cache depends only on the exclusion rect set. Text/font/line height/layout
 * width changes can reuse this derived data.
 */
class ExclusionLayoutCache
{
public:
  ExclusionLayoutCache();

  void Clear();
  void SetRegions(const Dali::Vector<Rect<float>>& regions);

  bool     Empty() const;
  uint32_t Count() const;
  uint64_t GetVersion() const;

  const SortedExclusionRegions& GetSortedRegions() const;

private:
  SortedExclusionRegions mSortedRegions;
  uint64_t               mVersion;
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_EXCLUSION_LAYOUT_CACHE_H

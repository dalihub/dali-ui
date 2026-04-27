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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>
#include <cstdint>
#include <cstring>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Fast token-indexed color cache for resolved theme colors.
 *
 * Stores resolved RGBA values keyed by sequential token IDs (1-based).
 * Designed for O(1) lookup via direct array indexing — no hashing,
 * no pointer chasing, no heap allocation per lookup.
 *
 * Memory layout: a single flat array of (Vector4 + uint8_t valid flag)
 * entries, tightly packed for cache-line friendliness.
 *
 * UiColor reads from the cache on the hot path; UiColorManager
 * invalidates it on theme change or override change.
 */
class UiColorCache
{
public:
  static UiColorCache& Get()
  {
    static UiColorCache instance;
    return instance;
  }

  /**
   * @brief Attempts to read a cached color.
   *
   * @param[in]  tokenId  1-based token ID
   * @param[out] outColor The cached RGBA value (written only on hit)
   * @return True on cache hit
   */
  bool TryGet(uint32_t tokenId, Vector4& outColor) const
  {
    const uint32_t idx = tokenId - 1u;
    if(idx < mSize && mValid[idx])
    {
      outColor = mColors[idx];
      return true;
    }
    return false;
  }

  /**
   * @brief Stores a resolved color in the cache.
   *
   * Grows internal storage if needed.
   *
   * @param[in] tokenId 1-based token ID
   * @param[in] color   The resolved RGBA value to cache
   */
  void Store(uint32_t tokenId, const Vector4& color)
  {
    if(tokenId > mCapacity)
    {
      Grow(tokenId);
    }
    if(tokenId > mSize)
    {
      mSize = tokenId;
    }
    const uint32_t idx = tokenId - 1u;
    mColors[idx]       = color;
    mValid[idx]        = 1u;
  }

  /**
   * @brief Invalidates all cached entries.
   */
  void InvalidateAll()
  {
    std::memset(mValid, 0, mSize);
  }

  /**
   * @brief Invalidates a single cached entry.
   *
   * @param[in] tokenId 1-based token ID
   */
  void Invalidate(uint32_t tokenId)
  {
    const uint32_t idx = tokenId - 1u;
    if(idx < mSize)
    {
      mValid[idx] = 0u;
    }
  }

private:
  UiColorCache() = default;
  ~UiColorCache()
  {
    delete[] mColors;
    delete[] mValid;
  }

  UiColorCache(const UiColorCache&)            = delete;
  UiColorCache& operator=(const UiColorCache&) = delete;

  void Grow(uint32_t required)
  {
    // Round up to next power-of-two-like bucket to reduce reallocations.
    // Minimum 64 entries (~1 KB for colors + 64 bytes for valid flags).
    uint32_t newCap = mCapacity ? mCapacity : 64u;
    while(newCap < required)
    {
      newCap *= 2u;
    }

    auto* newColors = new Vector4[newCap];
    auto* newValid  = new uint8_t[newCap];

    if(mSize > 0u)
    {
      std::memcpy(newColors, mColors, mSize * sizeof(Vector4));
      std::memcpy(newValid, mValid, mSize);
    }
    // Zero-initialize new region
    std::memset(newValid + mSize, 0, newCap - mSize);

    delete[] mColors;
    delete[] mValid;

    mColors   = newColors;
    mValid    = newValid;
    mCapacity = newCap;
  }

  Vector4* mColors{nullptr};
  uint8_t* mValid{nullptr};
  uint32_t mSize{0u};
  uint32_t mCapacity{0u};
};

} // namespace Internal
} // namespace Ui
} // namespace Dali

#ifndef DALI_UI_TEXT_VISUALIZER_LAYOUT_TYPES_H
#define DALI_UI_TEXT_VISUALIZER_LAYOUT_TYPES_H

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
#include <cmath>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali::Ui::Internal::TextVisualizer
{
struct AvailableInterval
{
  float x{0.0f};
  float width{0.0f};
};

struct TextLineFragment
{
  uint32_t clusterStart{0u};
  uint32_t clusterEnd{0u};
  uint32_t glyphStart{0u};
  uint32_t glyphEnd{0u};
  float    x{0.0f};
  float    y{0.0f};
  float    width{0.0f};
};

struct TextLineMetrics
{
  float ascender{0.0f};
  float descender{0.0f};
  float baselineOffset{0.0f};
  float naturalLineHeight{0.0f};
  bool  valid{false};
};

struct TextLine
{
  float                          y{0.0f};
  float                          height{0.0f};
  TextLineMetrics                metrics;
  Dali::Vector<TextLineFragment> fragments;

  void Clear()
  {
    y       = 0.0f;
    height  = 0.0f;
    metrics = TextLineMetrics{};
    fragments.Clear();
  }

  bool Empty() const
  {
    return fragments.Empty();
  }
};

struct ClusterPlacement
{
  uint32_t clusterIndex{0u};
  float    x{0.0f};
  float    y{0.0f};
  float    width{0.0f};
};

struct GlyphPlacement
{
  uint32_t glyphIndex{0u};
  float    x{0.0f};
  float    y{0.0f};
  float    width{0.0f};
  float    height{0.0f};
  float    advance{0.0f};
};

struct LayoutResult
{
  Dali::Vector<TextLine>         lines;
  Dali::Vector<ClusterPlacement> clusterPlacements;
  Dali::Vector<GlyphPlacement>   glyphPlacements;
  float                          width{0.0f};
  float                          height{0.0f};

  void Clear()
  {
    lines.Clear();
    clusterPlacements.Clear();
    glyphPlacements.Clear();
    width  = 0.0f;
    height = 0.0f;
  }

  void Reserve(uint32_t lineCount, uint32_t glyphPlacementCount, uint32_t clusterPlacementCount)
  {
    lines.Reserve(lineCount);
    glyphPlacements.Reserve(glyphPlacementCount);
    clusterPlacements.Reserve(clusterPlacementCount);
  }

  bool Empty() const
  {
    return lines.Empty() && clusterPlacements.Empty() && glyphPlacements.Empty();
  }

  uint32_t GetLineCount() const
  {
    return lines.Count();
  }

  uint64_t CalculateSignature(float epsilon = 0.01f) const
  {
    uint64_t signature = 1469598103934665603ull;

    HashCombine(signature, lines.Count());
    HashCombine(signature, clusterPlacements.Count());
    HashCombine(signature, glyphPlacements.Count());
    HashCombine(signature, Quantize(width, epsilon));
    HashCombine(signature, Quantize(height, epsilon));

    for(uint32_t lineIndex = 0u; lineIndex < lines.Count(); ++lineIndex)
    {
      const TextLine& line = lines[lineIndex];
      HashCombine(signature, Quantize(line.y, epsilon));
      HashCombine(signature, Quantize(line.height, epsilon));
      HashCombine(signature, line.metrics.valid ? 1u : 0u);
      HashCombine(signature, Quantize(line.metrics.ascender, epsilon));
      HashCombine(signature, Quantize(line.metrics.descender, epsilon));
      HashCombine(signature, Quantize(line.metrics.baselineOffset, epsilon));
      HashCombine(signature, Quantize(line.metrics.naturalLineHeight, epsilon));
      HashCombine(signature, line.fragments.Count());
    }

    for(uint32_t placementIndex = 0u; placementIndex < clusterPlacements.Count(); ++placementIndex)
    {
      const ClusterPlacement& placement = clusterPlacements[placementIndex];
      HashCombine(signature, placement.clusterIndex);
      HashCombine(signature, Quantize(placement.x, epsilon));
      HashCombine(signature, Quantize(placement.y, epsilon));
      HashCombine(signature, Quantize(placement.width, epsilon));
    }

    for(uint32_t placementIndex = 0u; placementIndex < glyphPlacements.Count(); ++placementIndex)
    {
      const GlyphPlacement& placement = glyphPlacements[placementIndex];
      HashCombine(signature, placement.glyphIndex);
      HashCombine(signature, Quantize(placement.x, epsilon));
      HashCombine(signature, Quantize(placement.y, epsilon));
      HashCombine(signature, Quantize(placement.width, epsilon));
      HashCombine(signature, Quantize(placement.height, epsilon));
      HashCombine(signature, Quantize(placement.advance, epsilon));
    }

    return signature;
  }

private:
  static uint64_t Quantize(float value, float epsilon)
  {
    const float safeEpsilon = epsilon > 0.0f ? epsilon : 0.01f;
    return static_cast<uint64_t>(static_cast<int64_t>(std::llround(value / safeEpsilon)));
  }

  static void HashCombine(uint64_t& seed, uint64_t value)
  {
    seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6u) + (seed >> 2u);
  }
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_LAYOUT_TYPES_H

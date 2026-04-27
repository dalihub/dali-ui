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
#include <cmath>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-visualizer/layout-engine.h>

namespace Dali::Ui::Internal::TextVisualizer
{
namespace
{
struct BlockedInterval
{
  float start{0.0f};
  float end{0.0f};
};

struct GlyphFitResult
{
  uint32_t glyphEnd{0u};
  float    width{0.0f};
  bool     hasGlyphs{false};
  bool     forceLineBreak{false};
};

bool HasVerticalOverlap(float lineTop, float lineBottom, const Rect<float>& region)
{
  return (region.y < lineBottom) && ((region.y + region.height) > lineTop);
}

bool IsInsideLayoutWidth(const BlockedInterval& interval)
{
  return interval.end > interval.start;
}

float GetEffectiveLineHeight(const PreparedText& preparedText, float lineHeight)
{
  if(lineHeight > 0.0f)
  {
    return lineHeight;
  }

  if(preparedText.HasLineMetrics() && (preparedText.GetLineMetrics().naturalLineHeight > 0.0f))
  {
    return preparedText.GetLineMetrics().naturalLineHeight;
  }

  return LayoutEngine::GetPlaceholderLineHeight(preparedText);
}

float GetGlyphPlacementWidth(const Text::GlyphInfo& glyph)
{
  if(glyph.width > 0.0f)
  {
    return glyph.width;
  }

  return glyph.advance > 0.0f ? glyph.advance : 0.0f;
}

float GetGlyphPlacementAdvance(const Text::GlyphInfo& glyph)
{
  if(glyph.advance > 0.0f)
  {
    return glyph.advance;
  }

  return glyph.width > 0.0f ? glyph.width : 0.0f;
}

uint32_t GetEstimatedLineGuard(float lineHeight, const SortedExclusionRegions& exclusionRegions, uint32_t glyphCount)
{
  float maxBlockedBottom = 0.0f;

  for(SortedExclusionRegions::const_iterator it = exclusionRegions.begin(), endIt = exclusionRegions.end(); it != endIt; ++it)
  {
    maxBlockedBottom = std::max(maxBlockedBottom, it->bottom);
  }

  const uint32_t blockedLineCount = lineHeight > 0.0f ? static_cast<uint32_t>(std::ceil(std::max(0.0f, maxBlockedBottom) / lineHeight)) : 0u;
  return std::max(1u, glyphCount + blockedLineCount + 2u);
}

PreparedText::GlyphLayoutData BuildGlyphLayoutDataFallback(const PreparedText&                  preparedText,
                                                           const Dali::Vector<Text::GlyphInfo>& glyphs)
{
  const uint32_t                glyphCount = glyphs.Count();
  PreparedText::GlyphLayoutData cache;
  cache.advances.Resize(glyphCount);
  cache.widths.Resize(glyphCount);
  cache.prefixAdvances.Resize(glyphCount + 1u, 0.0f);
  cache.characterStarts.Resize(glyphCount);
  cache.characterEnds.Resize(glyphCount);
  cache.breakAllowedAfterGlyph.Resize(glyphCount, static_cast<uint8_t>(0u));
  cache.breakMandatoryAfterGlyph.Resize(glyphCount, static_cast<uint8_t>(0u));

  const uint32_t                            characterCount      = preparedText.GetCharacterCount();
  const Dali::Vector<Text::CharacterIndex>& glyphToCharacterMap = preparedText.GetGlyphToCharacterMap();
  const Dali::Vector<Text::Length>&         charactersPerGlyph  = preparedText.GetCharactersPerGlyph();
  const Dali::Vector<Text::LineBreakInfo>&  lineBreakInfo       = preparedText.GetLineBreakInfo();

  for(uint32_t glyphIndex = 0u; glyphIndex < glyphCount; ++glyphIndex)
  {
    const Text::GlyphInfo& glyph = glyphs[glyphIndex];
    cache.advances[glyphIndex]   = GetGlyphPlacementAdvance(glyph);
    cache.widths[glyphIndex]     = GetGlyphPlacementWidth(glyph);

    uint32_t characterStart = characterCount;
    uint32_t characterEnd   = characterCount;

    if(glyphIndex < glyphToCharacterMap.Count())
    {
      characterStart               = glyphToCharacterMap[glyphIndex];
      const uint32_t characterSpan = glyphIndex < charactersPerGlyph.Count() ? charactersPerGlyph[glyphIndex] : 0u;
      characterEnd                 = std::min(characterCount, characterStart + characterSpan);
    }

    cache.characterStarts[glyphIndex] = characterStart;
    cache.characterEnds[glyphIndex]   = characterEnd;

    if(characterEnd > 0u)
    {
      const uint32_t characterIndex = characterEnd - 1u;
      if(characterIndex < lineBreakInfo.Count())
      {
        const Text::LineBreakInfo glyphLineBreakInfo = lineBreakInfo[characterIndex];
        cache.breakAllowedAfterGlyph[glyphIndex]     = (glyphLineBreakInfo == TextAbstraction::LINE_ALLOW_BREAK) ||
                                                   (glyphLineBreakInfo == TextAbstraction::LINE_MUST_BREAK);
        cache.breakMandatoryAfterGlyph[glyphIndex] = glyphLineBreakInfo == TextAbstraction::LINE_MUST_BREAK;
      }
    }

    cache.prefixAdvances[glyphIndex + 1u] = cache.prefixAdvances[glyphIndex] + cache.advances[glyphIndex];
  }

  return cache;
}

GlyphFitResult FindGlyphRangeForInterval(const PreparedText::GlyphLayoutData& cache,
                                         uint32_t                             glyphStart,
                                         float                                availableWidth,
                                         bool                                 allowOversizedFirstGlyph)
{
  GlyphFitResult result;
  result.glyphEnd = glyphStart;

  const uint32_t glyphCount = cache.advances.Count();
  if(glyphStart >= glyphCount)
  {
    return result;
  }

  float    fragmentWidth  = 0.0f;
  uint32_t lastBreakEnd   = glyphStart;
  float    lastBreakWidth = 0.0f;

  for(uint32_t glyphIndex = glyphStart; glyphIndex < glyphCount; ++glyphIndex)
  {
    const float cursorX       = cache.prefixAdvances[glyphIndex] - cache.prefixAdvances[glyphStart];
    const float glyphAdvance  = cache.advances[glyphIndex];
    const float glyphWidth    = cache.widths[glyphIndex];
    const float requiredWidth = std::max(glyphAdvance, glyphWidth);
    const bool  canFit        = (requiredWidth <= 0.0f) || ((cursorX + requiredWidth) <= availableWidth);
    const bool  isFirstGlyph  = (glyphIndex == glyphStart);

    if(!canFit)
    {
      if(lastBreakEnd > glyphStart)
      {
        result.glyphEnd  = lastBreakEnd;
        result.width     = lastBreakWidth;
        result.hasGlyphs = true;
        return result;
      }

      if(!isFirstGlyph)
      {
        result.glyphEnd  = glyphIndex;
        result.width     = fragmentWidth;
        result.hasGlyphs = result.glyphEnd > glyphStart;
        return result;
      }

      if(!allowOversizedFirstGlyph)
      {
        return result;
      }
    }

    const float placementRight = std::max(cursorX + glyphWidth, cursorX + glyphAdvance);
    fragmentWidth              = std::max(fragmentWidth, placementRight);

    result.glyphEnd  = glyphIndex + 1u;
    result.width     = fragmentWidth;
    result.hasGlyphs = true;

    if(cache.breakAllowedAfterGlyph[glyphIndex])
    {
      lastBreakEnd   = glyphIndex + 1u;
      lastBreakWidth = fragmentWidth;
    }

    if(cache.breakMandatoryAfterGlyph[glyphIndex])
    {
      result.forceLineBreak = true;
      return result;
    }

    if(!canFit && allowOversizedFirstGlyph)
    {
      return result;
    }
  }

  return result;
}

TextLineMetrics CalculateTextLineMetrics(const PreparedText& preparedText,
                                         const TextLine&     textLine)
{
  TextLineMetrics                        metrics;
  const Dali::Vector<Text::GlyphInfo>&   glyphs          = preparedText.GetGlyphs();
  const PreparedText::LineMetrics* const fallbackMetrics = preparedText.HasLineMetrics() ? &preparedText.GetLineMetrics() : nullptr;

  for(Vector<TextLineFragment>::ConstIterator fragmentIt = textLine.fragments.Begin(), fragmentEndIt = textLine.fragments.End();
      fragmentIt != fragmentEndIt; ++fragmentIt)
  {
    for(uint32_t glyphIndex = fragmentIt->glyphStart; glyphIndex < fragmentIt->glyphEnd && glyphIndex < glyphs.Count(); ++glyphIndex)
    {
      const Text::GlyphInfo& glyph = glyphs[glyphIndex];
      metrics.ascender             = std::max(metrics.ascender, glyph.yBearing);
      metrics.descender            = std::max(metrics.descender, std::max(0.0f, glyph.height - glyph.yBearing));
      metrics.valid                = true;
    }
  }

  if(metrics.valid)
  {
    const float lineGap       = (nullptr != fallbackMetrics) ? fallbackMetrics->lineGap : 0.0f;
    metrics.baselineOffset    = metrics.ascender;
    metrics.naturalLineHeight = metrics.ascender + metrics.descender + lineGap;
  }

  return metrics;
}

Dali::Vector<AvailableInterval> BuildAvailableIntervalsFromSorted(float                         layoutWidth,
                                                                  float                         lineY,
                                                                  float                         lineHeight,
                                                                  const SortedExclusionRegions& exclusionRegions)
{
  Dali::Vector<AvailableInterval> availableIntervals;

  if(layoutWidth <= 0.0f || lineHeight <= 0.0f)
  {
    return availableIntervals;
  }

  if(exclusionRegions.empty())
  {
    availableIntervals.PushBack({0.0f, layoutWidth});
    return availableIntervals;
  }

  const float lineTop    = lineY;
  const float lineBottom = lineY + lineHeight;

  std::vector<BlockedInterval> blockedIntervals;
  blockedIntervals.reserve(exclusionRegions.size());

  for(SortedExclusionRegions::const_iterator it = exclusionRegions.begin(), endIt = exclusionRegions.end(); it != endIt; ++it)
  {
    if(it->bottom <= lineTop)
    {
      continue;
    }

    if(it->top >= lineBottom)
    {
      break;
    }

    const Rect<float>& region = it->rect;
    if(!HasVerticalOverlap(lineTop, lineBottom, region))
    {
      continue;
    }

    BlockedInterval blockedInterval;
    blockedInterval.start = std::max(0.0f, region.x);
    blockedInterval.end   = std::min(layoutWidth, region.x + region.width);

    if(IsInsideLayoutWidth(blockedInterval))
    {
      blockedIntervals.push_back(blockedInterval);
    }
  }

  if(blockedIntervals.empty())
  {
    availableIntervals.PushBack({0.0f, layoutWidth});
    return availableIntervals;
  }

  std::sort(blockedIntervals.begin(), blockedIntervals.end(), [](const BlockedInterval& lhs, const BlockedInterval& rhs)
  {
    if(lhs.start == rhs.start)
    {
      return lhs.end < rhs.end;
    }
    return lhs.start < rhs.start;
  });

  std::vector<BlockedInterval> mergedIntervals;
  mergedIntervals.reserve(blockedIntervals.size());

  for(const BlockedInterval& blockedInterval : blockedIntervals)
  {
    if(mergedIntervals.empty() || blockedInterval.start > mergedIntervals.back().end)
    {
      mergedIntervals.push_back(blockedInterval);
      continue;
    }

    mergedIntervals.back().end = std::max(mergedIntervals.back().end, blockedInterval.end);
  }

  float currentX = 0.0f;
  for(const BlockedInterval& blockedInterval : mergedIntervals)
  {
    if(blockedInterval.start > currentX)
    {
      const float availableWidth = blockedInterval.start - currentX;
      if(availableWidth > 0.0f)
      {
        availableIntervals.PushBack({currentX, availableWidth});
      }
    }

    currentX = std::max(currentX, blockedInterval.end);
  }

  if(currentX < layoutWidth)
  {
    const float availableWidth = layoutWidth - currentX;
    if(availableWidth > 0.0f)
    {
      availableIntervals.PushBack({currentX, availableWidth});
    }
  }

  return availableIntervals;
}
} // namespace

Dali::Vector<AvailableInterval> LayoutEngine::BuildAvailableIntervals(float                            layoutWidth,
                                                                      float                            lineY,
                                                                      float                            lineHeight,
                                                                      const Dali::Vector<Rect<float>>& exclusionRegions)
{
  ExclusionLayoutCache exclusionCache;
  exclusionCache.SetRegions(exclusionRegions);
  return BuildAvailableIntervals(layoutWidth, lineY, lineHeight, exclusionCache);
}

Dali::Vector<AvailableInterval> LayoutEngine::BuildAvailableIntervals(float                       layoutWidth,
                                                                      float                       lineY,
                                                                      float                       lineHeight,
                                                                      const ExclusionLayoutCache& exclusionCache)
{
  return BuildAvailableIntervalsFromSorted(layoutWidth, lineY, lineHeight, exclusionCache.GetSortedRegions());
}

float LayoutEngine::GetPlaceholderClusterAdvance(const PreparedText& preparedText)
{
  const float fontSize = preparedText.GetFontSize();
  return fontSize > 0.0f ? fontSize * 0.5f : 10.0f;
}

float LayoutEngine::GetPlaceholderLineHeight(const PreparedText& preparedText)
{
  const float fontSize = preparedText.GetFontSize();
  return fontSize > 0.0f ? fontSize * 1.2f : 20.0f;
}

void LayoutEngine::LayoutPlaceholder(const PreparedText&              preparedText,
                                     float                            layoutWidth,
                                     float                            lineHeight,
                                     const Dali::Vector<Rect<float>>& exclusionRegions,
                                     LayoutResult&                    result)
{
  ExclusionLayoutCache exclusionCache;
  exclusionCache.SetRegions(exclusionRegions);
  LayoutPlaceholder(preparedText, layoutWidth, lineHeight, exclusionCache, result);
}

void LayoutEngine::LayoutPlaceholder(const PreparedText&         preparedText,
                                     float                       layoutWidth,
                                     float                       lineHeight,
                                     const ExclusionLayoutCache& exclusionCache,
                                     LayoutResult&               result)
{
  result.Clear();

  const uint32_t clusterCount = preparedText.GetClusterCount();
  if(preparedText.Empty() || layoutWidth <= 0.0f || clusterCount == 0u)
  {
    return;
  }

  const float clusterAdvance      = GetPlaceholderClusterAdvance(preparedText);
  const float effectiveLineHeight = GetEffectiveLineHeight(preparedText, lineHeight);
  if(clusterAdvance <= 0.0f || effectiveLineHeight <= 0.0f)
  {
    return;
  }

  uint32_t                      currentCluster         = 0u;
  float                         currentY               = 0.0f;
  const uint32_t                maxLineCount           = std::max(1u, clusterCount + exclusionCache.Count() + 1u);
  const uint32_t                estimatedLineCount     = std::min(maxLineCount, clusterCount);
  const SortedExclusionRegions& sortedExclusionRegions = exclusionCache.GetSortedRegions();
  result.Reserve(estimatedLineCount, 0u, clusterCount);

  for(uint32_t lineIndex = 0u; lineIndex < maxLineCount && currentCluster < clusterCount; ++lineIndex)
  {
    const Dali::Vector<AvailableInterval> availableIntervals = BuildAvailableIntervalsFromSorted(layoutWidth, currentY, effectiveLineHeight, sortedExclusionRegions);

    TextLine textLine;
    textLine.y      = currentY;
    textLine.height = effectiveLineHeight;
    textLine.fragments.Reserve(availableIntervals.Count());

    bool lineHasPlacement = false;

    for(uint32_t intervalIndex = 0u; intervalIndex < availableIntervals.Count() && currentCluster < clusterCount; ++intervalIndex)
    {
      const AvailableInterval& availableInterval = availableIntervals[intervalIndex];
      const uint32_t           fitCount          = static_cast<uint32_t>(availableInterval.width / clusterAdvance);

      if(fitCount == 0u)
      {
        continue;
      }

      const uint32_t placeCount = std::min(fitCount, clusterCount - currentCluster);
      if(placeCount == 0u)
      {
        continue;
      }

      TextLineFragment fragment;
      fragment.clusterStart = currentCluster;
      fragment.clusterEnd   = currentCluster + placeCount;
      fragment.x            = availableInterval.x;
      fragment.y            = currentY;
      fragment.width        = static_cast<float>(placeCount) * clusterAdvance;
      textLine.fragments.PushBack(fragment);

      for(uint32_t placedIndex = 0u; placedIndex < placeCount; ++placedIndex)
      {
        ClusterPlacement placement;
        placement.clusterIndex = currentCluster + placedIndex;
        placement.x            = availableInterval.x + static_cast<float>(placedIndex) * clusterAdvance;
        placement.y            = currentY;
        placement.width        = clusterAdvance;
        result.clusterPlacements.PushBack(placement);
      }

      currentCluster += placeCount;
      lineHasPlacement = true;
      result.width     = std::max(result.width, fragment.x + fragment.width);
    }

    if(lineHasPlacement)
    {
      result.lines.PushBack(textLine);
      result.height = currentY + effectiveLineHeight;
    }

    currentY += effectiveLineHeight;
  }
}

void LayoutEngine::LayoutGlyphs(const PreparedText&              preparedText,
                                float                            layoutWidth,
                                float                            lineHeight,
                                const Dali::Vector<Rect<float>>& exclusionRegions,
                                LayoutResult&                    result)
{
  ExclusionLayoutCache exclusionCache;
  exclusionCache.SetRegions(exclusionRegions);
  LayoutGlyphs(preparedText, layoutWidth, lineHeight, exclusionCache, result);
}

void LayoutEngine::LayoutGlyphs(const PreparedText&         preparedText,
                                float                       layoutWidth,
                                float                       lineHeight,
                                const ExclusionLayoutCache& exclusionCache,
                                LayoutResult&               result)
{
  result.Clear();

  if(!preparedText.HasGlyphData() || layoutWidth <= 0.0f)
  {
    return;
  }

  const Dali::Vector<Text::GlyphInfo>& glyphs              = preparedText.GetGlyphs();
  const uint32_t                       glyphCount          = glyphs.Count();
  const float                          effectiveLineHeight = GetEffectiveLineHeight(preparedText, lineHeight);

  if(glyphCount == 0u || effectiveLineHeight <= 0.0f)
  {
    return;
  }

  PreparedText::GlyphLayoutData        fallbackGlyphLayoutData;
  const PreparedText::GlyphLayoutData* glyphLayoutData = &preparedText.GetGlyphLayoutData();

  if(!preparedText.HasGlyphLayoutData())
  {
    fallbackGlyphLayoutData = BuildGlyphLayoutDataFallback(preparedText, glyphs);
    glyphLayoutData         = &fallbackGlyphLayoutData;
  }

  uint32_t                      currentGlyph           = 0u;
  float                         currentY               = 0.0f;
  const SortedExclusionRegions& sortedExclusionRegions = exclusionCache.GetSortedRegions();
  const uint32_t                maxLineCount           = GetEstimatedLineGuard(effectiveLineHeight, sortedExclusionRegions, glyphCount);
  const uint32_t                estimatedLineCount     = std::min(maxLineCount, glyphCount);
  result.Reserve(estimatedLineCount, glyphCount, 0u);

  for(uint32_t lineIndex = 0u; lineIndex < maxLineCount && currentGlyph < glyphCount; ++lineIndex)
  {
    const Dali::Vector<AvailableInterval> availableIntervals = BuildAvailableIntervalsFromSorted(layoutWidth, currentY, effectiveLineHeight, sortedExclusionRegions);

    TextLine textLine;
    textLine.y      = currentY;
    textLine.height = effectiveLineHeight;
    textLine.fragments.Reserve(availableIntervals.Count());

    bool lineHasPlacement = false;

    for(uint32_t intervalIndex = 0u; intervalIndex < availableIntervals.Count() && currentGlyph < glyphCount; ++intervalIndex)
    {
      const AvailableInterval& availableInterval = availableIntervals[intervalIndex];
      const bool               allowOversized    = !lineHasPlacement;
      const GlyphFitResult     fitResult         = FindGlyphRangeForInterval(*glyphLayoutData, currentGlyph, availableInterval.width, allowOversized);

      if(!fitResult.hasGlyphs || fitResult.glyphEnd <= currentGlyph)
      {
        continue;
      }

      const uint32_t glyphStart = currentGlyph;
      const uint32_t glyphEnd   = fitResult.glyphEnd;
      float          cursorX    = availableInterval.x;

      TextLineFragment fragment;
      fragment.clusterStart = glyphStart < glyphLayoutData->characterStarts.Count() ? glyphLayoutData->characterStarts[glyphStart] : preparedText.GetCharacterCount();
      fragment.clusterEnd   = (glyphEnd - 1u) < glyphLayoutData->characterEnds.Count() ? glyphLayoutData->characterEnds[glyphEnd - 1u] : preparedText.GetCharacterCount();
      fragment.glyphStart   = glyphStart;
      fragment.glyphEnd     = glyphEnd;
      fragment.x            = availableInterval.x;
      fragment.y            = currentY;
      fragment.width        = fitResult.width;

      for(uint32_t glyphIndex = glyphStart; glyphIndex < glyphEnd; ++glyphIndex)
      {
        const Text::GlyphInfo& glyph        = glyphs[glyphIndex];
        const float            glyphAdvance = glyphLayoutData->advances[glyphIndex];

        GlyphPlacement placement;
        placement.glyphIndex = glyphIndex;
        placement.x          = cursorX;
        placement.y          = currentY;
        placement.width      = glyph.width;
        placement.height     = glyph.height;
        placement.advance    = glyphAdvance;
        result.glyphPlacements.PushBack(placement);

        cursorX += glyphAdvance;
      }

      textLine.fragments.PushBack(fragment);
      currentGlyph     = glyphEnd;
      lineHasPlacement = true;
      result.width     = std::max(result.width, fragment.x + fragment.width);

      if(fitResult.forceLineBreak)
      {
        break;
      }
    }

    if(lineHasPlacement)
    {
      textLine.metrics = CalculateTextLineMetrics(preparedText, textLine);
      result.lines.PushBack(textLine);
      result.height = currentY + effectiveLineHeight;
    }

    currentY += effectiveLineHeight;
  }
}

} // namespace Dali::Ui::Internal::TextVisualizer

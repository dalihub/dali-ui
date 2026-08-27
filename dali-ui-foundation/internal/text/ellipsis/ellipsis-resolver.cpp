/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-metrics.h>
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-resolver.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/text-alignment.h>

namespace Dali::Ui::Text
{
namespace
{

constexpr Character ELLIPSIS_CHARACTER     = 0x2026u;
constexpr float     FIT_EPSILON            = 0.01f;
constexpr uint32_t  INVALID_PHYSICAL_ORDER = std::numeric_limits<uint32_t>::max();

bool IsDirectionalControl(Character character)
{
  return character == 0x061Cu ||
         character == 0x200Eu || character == 0x200Fu ||
         (character >= 0x202Au && character <= 0x202Eu) ||
         (character >= 0x2066u && character <= 0x2069u);
}

struct SourceCluster
{
  CharacterIndex characterStart{0u};
  CharacterIndex characterEnd{0u};
  GlyphIndex     glyphStart{0u};
  GlyphIndex     glyphEnd{0u};
  float          advance{0.0f};
  float          sourcePen{0.0f};
  float          physicalStart{0.0f};
  float          physicalEnd{0.0f};
  uint32_t       physicalOrder{INVALID_PHYSICAL_ORDER};
};

struct ProjectedLine
{
  std::vector<Vector2> retainedGlyphPositions;
  Vector2              ellipsisPosition{Vector2::ZERO};
  float                width{0.0f};
  float                finalBoundaryCoordinate{0.0f};
};

enum class EndEllipsisBoundaryKind : uint8_t
{
  NONE,
  APPEND,
  REPLACE
};

struct EndEllipsisBoundary
{
  EndEllipsisBoundaryKind kind{EndEllipsisBoundaryKind::NONE};
  uint32_t                sourcePhysicalOrder{INVALID_PHYSICAL_ORDER};
  bool                    isRightToLeft{false};
};

CharacterIndex GetSemanticLineEnd(const LogicalModel& logical, const LineRun& line)
{
  // Vertical elision can extend the final visible LineRun through hidden
  // following paragraphs. An END boundary never crosses a paragraph
  // separator.
  const CharacterIndex lineStart = line.characterRun.characterIndex;
  const CharacterIndex lineEnd   = std::min<CharacterIndex>(lineStart + line.characterRun.numberOfCharacters,
                                                            static_cast<CharacterIndex>(logical.mText.Count()));
  for(CharacterIndex character = lineStart; character < lineEnd; ++character)
  {
    if(TextAbstraction::IsNewParagraph(logical.mText[character]))
    {
      return character;
    }
  }
  return lineEnd;
}

bool HasHiddenLogicalContinuation(const LogicalModel& logical, const LineRun& line)
{
  const CharacterIndex representedEnd = std::min<CharacterIndex>(
    line.characterRun.characterIndex + line.characterRun.numberOfCharacters,
    static_cast<CharacterIndex>(logical.mText.Count()));
  return representedEnd < logical.mText.Count();
}

LineIndex FindEllipsisLine(const VisualModel& visual)
{
  for(LineIndex index = 0u; index < visual.mLines.Count(); ++index)
  {
    if(visual.mLines[index].ellipsis)
    {
      return index;
    }
  }
  return FinalElisionResult::INVALID_LINE_INDEX;
}

GlyphIndex SourceGlyphForCharacter(const VisualModel& visual, CharacterIndex characterIndex)
{
  return characterIndex < visual.mCharactersToGlyph.Count()
           ? visual.mCharactersToGlyph[characterIndex]
           : FinalElisionResult::INVALID_GLYPH_INDEX;
}

void AssignSourcePhysicalOrder(std::vector<SourceCluster>& clusters)
{
  if(clusters.empty())
  {
    return;
  }

  // Shaping/layout has already resolved the source line into physical X
  // intervals. Preserve that topology; direction is consumed separately when
  // logical START/END is mapped to an interval edge.
  std::vector<size_t> physicalClusters(clusters.size());
  std::iota(physicalClusters.begin(), physicalClusters.end(), 0u);
  std::stable_sort(physicalClusters.begin(), physicalClusters.end(), [&clusters](size_t lhs, size_t rhs)
  {
    const SourceCluster& left  = clusters[lhs];
    const SourceCluster& right = clusters[rhs];
    if(std::fabs(left.physicalStart - right.physicalStart) > FIT_EPSILON)
    {
      return left.physicalStart < right.physicalStart;
    }
    if(std::fabs(left.physicalEnd - right.physicalEnd) > FIT_EPSILON)
    {
      return left.physicalEnd < right.physicalEnd;
    }
    // Zero-width, control and combining clusters can share an interval. Source
    // glyph identity provides a deterministic order without inventing X space.
    return left.glyphStart < right.glyphStart;
  });
  for(uint32_t physicalOrder = 0u; physicalOrder < physicalClusters.size(); ++physicalOrder)
  {
    clusters[physicalClusters[physicalOrder]].physicalOrder = physicalOrder;
  }
}

std::vector<SourceCluster> CollectSourceClusters(const Model& model, const LineRun& line)
{
  const LogicalModel&  logical      = *model.mLogicalModel;
  const VisualModel&   visual       = *model.mVisualModel;
  const GlyphIndex     lineEnd      = std::min<GlyphIndex>(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs,
                                                           static_cast<GlyphIndex>(visual.mGlyphs.Count()));
  const CharacterIndex characterEnd = GetSemanticLineEnd(logical, line);

  std::vector<SourceCluster> clusters;
  clusters.reserve(line.glyphRun.numberOfGlyphs);
  for(GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < lineEnd;)
  {
    const GlyphIndex groupStart = glyphIndex;
    while(glyphIndex < lineEnd && glyphIndex < visual.mCharactersPerGlyph.Count() &&
          visual.mCharactersPerGlyph[glyphIndex] == 0u)
    {
      ++glyphIndex;
    }
    if(glyphIndex >= lineEnd || glyphIndex >= visual.mCharactersPerGlyph.Count())
    {
      break;
    }

    const GlyphIndex     groupEnd             = glyphIndex + 1u;
    const CharacterIndex mappedCharacterStart = visual.mGlyphsToCharacters[groupStart];
    const CharacterIndex clusterCharacterEnd  = std::min<CharacterIndex>(
      mappedCharacterStart + visual.mCharactersPerGlyph[glyphIndex], characterEnd);
    CharacterIndex characterStart = mappedCharacterStart;
    while(characterStart < clusterCharacterEnd && characterStart < logical.mText.Count() &&
          IsDirectionalControl(logical.mText[characterStart]))
    {
      ++characterStart;
    }
    if(characterStart < line.characterRun.characterIndex || characterStart >= characterEnd ||
       clusterCharacterEnd <= characterStart)
    {
      glyphIndex = groupEnd;
      continue;
    }

    SourceCluster cluster;
    cluster.characterStart = characterStart;
    cluster.characterEnd   = clusterCharacterEnd;
    cluster.glyphStart     = groupStart;
    cluster.glyphEnd       = groupEnd;
    if(groupStart < visual.mGlyphPositions.Count())
    {
      cluster.sourcePen = visual.mGlyphPositions[groupStart].x - visual.mGlyphs[groupStart].xBearing;
    }
    for(GlyphIndex sourceGlyph = groupStart; sourceGlyph < groupEnd; ++sourceGlyph)
    {
      CharacterIndex sourceCharacter = visual.mGlyphsToCharacters[sourceGlyph];
      while(sourceCharacter < logical.mText.Count() && sourceCharacter < clusterCharacterEnd &&
            IsDirectionalControl(logical.mText[sourceCharacter]))
      {
        ++sourceCharacter;
      }
      if(sourceCharacter >= clusterCharacterEnd || sourceCharacter >= logical.mText.Count())
      {
        continue;
      }
      const float spacing           = GetGlyphCharacterSpacing(sourceGlyph,
                                                               visual.GetCharacterSpacingGlyphRuns(),
                                                               visual.GetCharacterSpacing());
      const float calculatedAdvance = GetCalculatedAdvance(logical.mText[sourceCharacter],
                                                           spacing,
                                                           visual.mGlyphs[sourceGlyph].advance);
      cluster.advance += calculatedAdvance;
    }
    cluster.physicalStart = std::min(cluster.sourcePen, cluster.sourcePen + cluster.advance);
    cluster.physicalEnd   = std::max(cluster.sourcePen, cluster.sourcePen + cluster.advance);
    clusters.push_back(cluster);
    glyphIndex = groupEnd;
  }

  // Candidate selection is a logical-prefix operation. Keep its vector in an
  // explicit logical domain even if a future shaper stores glyph groups in a
  // different order; physical ordering is recorded separately below.
  std::stable_sort(clusters.begin(), clusters.end(), [](const SourceCluster& left, const SourceCluster& right)
  {
    if(left.characterStart != right.characterStart)
    {
      return left.characterStart < right.characterStart;
    }
    return left.glyphStart < right.glyphStart;
  });
  AssignSourcePhysicalOrder(clusters);
  return clusters;
}

GlyphIndex ResolveStyleSourceGlyph(const Model&   model,
                                   const LineRun& line,
                                   CharacterIndex originalBoundary)
{
  const VisualModel&   visual  = *model.mVisualModel;
  const LogicalModel&  logical = *model.mLogicalModel;
  const CharacterIndex lineEnd = GetSemanticLineEnd(logical, line);
  if(originalBoundary < lineEnd && originalBoundary < logical.mText.Count() &&
     !TextAbstraction::IsNewParagraph(logical.mText[originalBoundary]))
  {
    const GlyphIndex glyph = SourceGlyphForCharacter(visual, originalBoundary);
    if(glyph < visual.mGlyphs.Count())
    {
      return glyph;
    }
  }
  if(originalBoundary > line.characterRun.characterIndex)
  {
    const GlyphIndex glyph = SourceGlyphForCharacter(visual, originalBoundary - 1u);
    if(glyph < visual.mGlyphs.Count())
    {
      return glyph;
    }
  }
  return line.glyphRun.glyphIndex < visual.mGlyphs.Count()
           ? line.glyphRun.glyphIndex
           : FinalElisionResult::INVALID_GLYPH_INDEX;
}

bool ResolveEllipsisGlyph(const Model&                 model,
                          GlyphIndex                   styleGlyph,
                          TextAbstraction::FontClient& fontClient,
                          GlyphInfo&                   ellipsisGlyph,
                          float&                       ellipsisAdvance)
{
  const VisualModel& visual = *model.mVisualModel;
  FontId             fontId = styleGlyph < visual.mGlyphs.Count() ? visual.mGlyphs[styleGlyph].fontId : 0u;
  if(!ResolveFontClientEllipsisMetrics(fontClient, fontId, true, ellipsisGlyph) || ellipsisGlyph.fontId == 0u)
  {
    return false;
  }
  if(styleGlyph < visual.mGlyphs.Count())
  {
    ellipsisGlyph.isItalicRequired = visual.mGlyphs[styleGlyph].isItalicRequired;
    ellipsisGlyph.isBoldRequired   = visual.mGlyphs[styleGlyph].isBoldRequired;
  }

  const float spacing = styleGlyph < visual.mGlyphs.Count()
                          ? GetGlyphCharacterSpacing(styleGlyph,
                                                     visual.GetCharacterSpacingGlyphRuns(),
                                                     visual.GetCharacterSpacing())
                          : visual.GetCharacterSpacing();
  ellipsisAdvance     = GetCalculatedAdvance(ELLIPSIS_CHARACTER, spacing, ellipsisGlyph.advance);
  return true;
}

EndEllipsisBoundary MakeBoundaryToken(const Model&                      model,
                                      const std::vector<SourceCluster>& clusters,
                                      size_t                            retainedClusterCount)
{
  EndEllipsisBoundary boundary;
  const LogicalModel& logical = *model.mLogicalModel;
  if(retainedClusterCount < clusters.size())
  {
    // REPLACE projects the logical start of the first removed cluster into the
    // source line's already-resolved physical topology.
    const SourceCluster& firstRemoved = clusters[retainedClusterCount];
    boundary.kind                     = EndEllipsisBoundaryKind::REPLACE;
    boundary.sourcePhysicalOrder      = firstRemoved.physicalOrder;
    boundary.isRightToLeft            = logical.GetCharacterDirection(firstRemoved.characterStart);
    return boundary;
  }
  if(!clusters.empty())
  {
    // APPEND projects the logical end of the last retained cluster and does
    // not remove a drawable source glyph.
    const SourceCluster& anchor  = clusters.back();
    boundary.kind                = EndEllipsisBoundaryKind::APPEND;
    boundary.sourcePhysicalOrder = anchor.physicalOrder;
    boundary.isRightToLeft       = logical.GetCharacterDirection(anchor.characterStart);
  }
  return boundary;
}

ProjectedLine ProjectFinalLine(const Model&                      model,
                               const std::vector<SourceCluster>& clusters,
                               size_t                            retainedClusterCount,
                               const GlyphInfo&                  ellipsisGlyph,
                               float                             ellipsisAdvance,
                               const EndEllipsisBoundary&        boundary,
                               bool                              keepPositions)
{
  const VisualModel& source = *model.mVisualModel;
  struct PhysicalItem
  {
    uint32_t physicalOrder{0u};
    size_t   clusterIndex{0u};
    bool     ellipsis{false};
  };

  std::vector<PhysicalItem> physicalItems;
  physicalItems.reserve(retainedClusterCount + 1u);
  for(size_t clusterIndex = 0u; clusterIndex < retainedClusterCount; ++clusterIndex)
  {
    physicalItems.push_back({clusters[clusterIndex].physicalOrder, clusterIndex, false});
  }
  physicalItems.push_back({boundary.sourcePhysicalOrder, retainedClusterCount, true});
  std::stable_sort(physicalItems.begin(), physicalItems.end(),
                   [&boundary](const PhysicalItem& lhs, const PhysicalItem& rhs)
  {
    if(lhs.physicalOrder != rhs.physicalOrder)
    {
      return lhs.physicalOrder < rhs.physicalOrder;
    }
    if(lhs.ellipsis == rhs.ellipsis)
    {
      return false;
    }
    if(boundary.kind == EndEllipsisBoundaryKind::APPEND && !boundary.isRightToLeft)
    {
      return !lhs.ellipsis && rhs.ellipsis;
    }
    return lhs.ellipsis && !rhs.ellipsis;
  });

  std::vector<size_t> retainedOffsets(retainedClusterCount + 1u, 0u);
  for(size_t clusterIndex = 0u; clusterIndex < retainedClusterCount; ++clusterIndex)
  {
    retainedOffsets[clusterIndex + 1u] = retainedOffsets[clusterIndex] +
                                         clusters[clusterIndex].glyphEnd - clusters[clusterIndex].glyphStart;
  }

  ProjectedLine projected;
  if(keepPositions)
  {
    projected.retainedGlyphPositions.resize(retainedOffsets.back());
  }

  float      cursor      = 0.0f;
  float      minimumX    = std::numeric_limits<float>::max();
  float      maximumX    = -std::numeric_limits<float>::max();
  float      maximumInkX = -std::numeric_limits<float>::max();
  const auto observe     = [&](float positionX, const GlyphInfo& glyph)
  {
    minimumX    = std::min(minimumX, positionX);
    maximumX    = std::max(maximumX, positionX);
    maximumInkX = std::max(maximumInkX, positionX + std::max(0.0f, glyph.width));
  };

  for(const PhysicalItem& item : physicalItems)
  {
    if(item.ellipsis)
    {
      projected.ellipsisPosition        = Vector2(cursor + ellipsisGlyph.xBearing, -ellipsisGlyph.yBearing);
      projected.finalBoundaryCoordinate = cursor + (boundary.isRightToLeft ? ellipsisAdvance : 0.0f);
      observe(projected.ellipsisPosition.x, ellipsisGlyph);
      cursor += ellipsisAdvance;
      continue;
    }

    const SourceCluster& cluster = clusters[item.clusterIndex];
    const float          shift   = cursor - cluster.sourcePen;
    for(GlyphIndex sourceGlyph = cluster.glyphStart; sourceGlyph < cluster.glyphEnd; ++sourceGlyph)
    {
      Vector2 position = sourceGlyph < source.mGlyphPositions.Count()
                           ? source.mGlyphPositions[sourceGlyph]
                           : Vector2(cluster.sourcePen + source.mGlyphs[sourceGlyph].xBearing,
                                     -source.mGlyphs[sourceGlyph].yBearing);
      position.x += shift;
      if(keepPositions)
      {
        projected.retainedGlyphPositions[retainedOffsets[item.clusterIndex] +
                                         sourceGlyph - cluster.glyphStart] = position;
      }
      observe(position.x, source.mGlyphs[sourceGlyph]);
    }
    cursor += cluster.advance;
  }

  if(minimumX == std::numeric_limits<float>::max())
  {
    return projected;
  }

  float left  = model.mRemoveFrontInset ? minimumX : std::min(0.0f, minimumX);
  left        = std::min(left, projected.finalBoundaryCoordinate);
  float right = model.mRemoveBackInset ? maximumInkX : std::max(cursor, maximumInkX);
  // Boundary and zero-width glyph positions are authoritative geometry even
  // when back-inset removal excludes unused advance.
  right = std::max(right, std::max(maximumX, projected.finalBoundaryCoordinate));
  right = std::max(right, left);

  const float translation = -left;
  projected.ellipsisPosition.x += translation;
  projected.finalBoundaryCoordinate += translation;
  if(keepPositions)
  {
    for(Vector2& position : projected.retainedGlyphPositions)
    {
      position.x += translation;
    }
  }
  projected.width = right - left;
  return projected;
}

void CopyFinalColorIndices(const VisualModel& source, FinalElisionResult& result)
{
  const Length finalCount = static_cast<Length>(result.glyphs.Count());
  if(!source.mColorIndices.Empty())
  {
    result.colorIndices.Resize(finalCount);
  }
  if(!source.mBackgroundColorIndices.Empty())
  {
    result.backgroundColorIndices.Resize(finalCount);
  }
  for(GlyphIndex finalGlyph = 0u; finalGlyph < finalCount; ++finalGlyph)
  {
    const GlyphIndex styleGlyph = result.finalToStyleGlyphIndices[finalGlyph];
    if(!result.colorIndices.Empty())
    {
      result.colorIndices[finalGlyph] = styleGlyph < source.mColorIndices.Count() ? source.mColorIndices[styleGlyph] : 0u;
    }
    if(!result.backgroundColorIndices.Empty())
    {
      result.backgroundColorIndices[finalGlyph] = styleGlyph < source.mBackgroundColorIndices.Count()
                                                    ? source.mBackgroundColorIndices[styleGlyph]
                                                    : 0u;
    }
  }
}

void BuildOmissionResult(const Model&                               sourceModel,
                         LineIndex                                  ellipsisLineIndex,
                         FinalElisionResult::EllipsisOmissionReason reason,
                         FinalElisionResult&                        result)
{
  const VisualModel& source           = *sourceModel.mVisualModel;
  const GlyphIndex   prefixGlyphCount = ellipsisLineIndex < source.mLines.Count()
                                          ? source.mLines[ellipsisLineIndex].glyphRun.glyphIndex
                                          : 0u;

  result.Clear();
  result.glyphs.Resize(prefixGlyphCount);
  result.lineLocalGlyphPositions.Resize(prefixGlyphCount);
  result.finalToSourceGlyphIndices.Resize(prefixGlyphCount);
  result.finalToStyleGlyphIndices.Resize(prefixGlyphCount);
  result.sourceToFinalGlyphIndices.Resize(source.mGlyphs.Count());
  std::fill(result.sourceToFinalGlyphIndices.Begin(),
            result.sourceToFinalGlyphIndices.End(),
            FinalElisionResult::INVALID_GLYPH_INDEX);
  for(GlyphIndex glyph = 0u; glyph < prefixGlyphCount; ++glyph)
  {
    result.glyphs[glyph]                    = source.mGlyphs[glyph];
    result.lineLocalGlyphPositions[glyph]   = source.mGlyphPositions[glyph];
    result.sourceToFinalGlyphIndices[glyph] = glyph;
    result.finalToSourceGlyphIndices[glyph] = glyph;
    result.finalToStyleGlyphIndices[glyph]  = glyph;
  }

  result.lines.Resize(ellipsisLineIndex);
  for(LineIndex line = 0u; line < ellipsisLineIndex; ++line)
  {
    result.lines[line] = source.mLines[line];
  }
  result.startIndex             = 0u;
  result.endIndex               = prefixGlyphCount == 0u ? 0u : prefixGlyphCount - 1u;
  result.firstMiddleIndex       = result.startIndex;
  result.secondMiddleIndex      = result.endIndex;
  result.ellipsisOmissionReason = reason;
  result.authoritativeLines     = true;
  result.resolved               = true;
  result.textElided             = true;
  result.applied                = false;
  result.layoutSize.width       = 0.0f;
  result.layoutSize.height      = 0.0f;
  for(LineIndex line = 0u; line < result.lines.Count(); ++line)
  {
    result.layoutSize.width = std::max(result.layoutSize.width, result.lines[line].width);
    result.layoutSize.height += GetLineHeight(result.lines[line], line + 1u == result.lines.Count());
  }
  result.layoutSize.height = std::ceil(result.layoutSize.height);
  CopyFinalColorIndices(source, result);
}

void BuildFinalResult(const Model&                      sourceModel,
                      LineIndex                         ellipsisLineIndex,
                      const std::vector<SourceCluster>& clusters,
                      size_t                            retainedClusterCount,
                      GlyphIndex                        ellipsisStyleGlyph,
                      const GlyphInfo&                  ellipsisGlyph,
                      const ProjectedLine&              projected,
                      FinalElisionResult&               result)
{
  const VisualModel& source           = *sourceModel.mVisualModel;
  const LineRun&     sourceLine       = source.mLines[ellipsisLineIndex];
  const GlyphIndex   finalPrefixCount = sourceLine.glyphRun.glyphIndex;
  Length             retainedGlyphCount{0u};
  for(size_t clusterIndex = 0u; clusterIndex < retainedClusterCount; ++clusterIndex)
  {
    retainedGlyphCount += clusters[clusterIndex].glyphEnd - clusters[clusterIndex].glyphStart;
  }
  const Length finalGlyphCount = finalPrefixCount + retainedGlyphCount + 1u;

  result.Clear();
  result.glyphs.Resize(finalGlyphCount);
  result.lineLocalGlyphPositions.Resize(finalGlyphCount);
  result.finalToSourceGlyphIndices.Resize(finalGlyphCount);
  result.finalToStyleGlyphIndices.Resize(finalGlyphCount);
  result.sourceToFinalGlyphIndices.Resize(source.mGlyphs.Count());
  std::fill(result.sourceToFinalGlyphIndices.Begin(),
            result.sourceToFinalGlyphIndices.End(),
            FinalElisionResult::INVALID_GLYPH_INDEX);

  for(GlyphIndex glyph = 0u; glyph < finalPrefixCount; ++glyph)
  {
    result.glyphs[glyph]                    = source.mGlyphs[glyph];
    result.lineLocalGlyphPositions[glyph]   = source.mGlyphPositions[glyph];
    result.sourceToFinalGlyphIndices[glyph] = glyph;
    result.finalToSourceGlyphIndices[glyph] = glyph;
    result.finalToStyleGlyphIndices[glyph]  = glyph;
  }

  GlyphIndex finalGlyph     = finalPrefixCount;
  size_t     projectedGlyph = 0u;
  for(size_t clusterIndex = 0u; clusterIndex < retainedClusterCount; ++clusterIndex)
  {
    const SourceCluster& cluster = clusters[clusterIndex];
    for(GlyphIndex sourceGlyph = cluster.glyphStart; sourceGlyph < cluster.glyphEnd;
        ++sourceGlyph, ++finalGlyph, ++projectedGlyph)
    {
      result.glyphs[finalGlyph]                     = source.mGlyphs[sourceGlyph];
      result.lineLocalGlyphPositions[finalGlyph]    = projected.retainedGlyphPositions[projectedGlyph];
      result.sourceToFinalGlyphIndices[sourceGlyph] = finalGlyph;
      result.finalToSourceGlyphIndices[finalGlyph]  = sourceGlyph;
      result.finalToStyleGlyphIndices[finalGlyph]   = sourceGlyph;
    }
  }

  result.glyphs[finalGlyph]                    = ellipsisGlyph;
  result.lineLocalGlyphPositions[finalGlyph]   = projected.ellipsisPosition;
  result.finalToSourceGlyphIndices[finalGlyph] = FinalElisionResult::INVALID_GLYPH_INDEX;
  result.finalToStyleGlyphIndices[finalGlyph]  = ellipsisStyleGlyph;

  result.lines.Resize(ellipsisLineIndex + 1u);
  for(LineIndex lineIndex = 0u; lineIndex <= ellipsisLineIndex; ++lineIndex)
  {
    result.lines[lineIndex] = source.mLines[lineIndex];
  }

  const CharacterIndex retainedCharacterEnd  = retainedClusterCount == 0u
                                                 ? sourceLine.characterRun.characterIndex
                                                 : clusters[retainedClusterCount - 1u].characterEnd;
  LineRun&             resultLine            = result.lines[ellipsisLineIndex];
  resultLine.width                           = projected.width;
  resultLine.extraLength                     = 0.0f;
  resultLine.direction                       = sourceLine.direction;
  resultLine.ellipsis                        = true;
  resultLine.glyphRun.glyphIndex             = finalPrefixCount;
  resultLine.glyphRun.numberOfGlyphs         = retainedGlyphCount + 1u;
  resultLine.characterRun.numberOfCharacters = retainedCharacterEnd - sourceLine.characterRun.characterIndex;
  resultLine.isSplitToTwoHalves              = false;
  resultLine.glyphRunSecondHalf              = GlyphRun{};
  resultLine.characterRunForSecondHalfLine   = CharacterRun{};

  result.layoutSize = Size::ZERO;
  for(LineIndex lineIndex = 0u; lineIndex < result.lines.Count(); ++lineIndex)
  {
    result.layoutSize.width = std::max(result.layoutSize.width, result.lines[lineIndex].width);
    result.layoutSize.height += GetLineHeight(result.lines[lineIndex], lineIndex + 1u == result.lines.Count());
  }
  result.layoutSize.height = std::ceil(result.layoutSize.height);

  result.startIndex              = 0u;
  result.endIndex                = finalGlyphCount - 1u;
  result.firstMiddleIndex        = result.startIndex;
  result.secondMiddleIndex       = result.endIndex;
  result.ellipsisFinalGlyphIndex = finalGlyph;
  result.ellipsisLineIndex       = ellipsisLineIndex;
  result.ellipsisUnitCount       = 1u;
  result.ellipsisOmissionReason  = FinalElisionResult::EllipsisOmissionReason::NONE;
  result.authoritativeLines      = true;
  result.resolved                = true;
  result.textElided              = true;
  result.applied                 = true;
  CopyFinalColorIndices(source, result);
}

} // unnamed namespace

bool ResolveEndEllipsis(const Model&                 model,
                        const Size&                  controlSize,
                        TextAbstraction::FontClient& fontClient,
                        FinalElisionResult&          result)
{
  result.Clear();
  if(model.mEllipsisPosition != EllipsisPosition::END || !model.mVisualModel || !model.mLogicalModel)
  {
    return false;
  }

  const VisualModel& visual            = *model.mVisualModel;
  const LineIndex    ellipsisLineIndex = FindEllipsisLine(visual);
  if(ellipsisLineIndex == FinalElisionResult::INVALID_LINE_INDEX)
  {
    if(visual.mLines.Empty() && !model.mLogicalModel->mText.Empty())
    {
      BuildOmissionResult(model,
                          0u,
                          FinalElisionResult::EllipsisOmissionReason::NO_VISIBLE_LINE,
                          result);
      return true;
    }
    return false;
  }
  const LineRun& line = visual.mLines[ellipsisLineIndex];
  if(line.glyphRun.numberOfGlyphs == 0u || line.characterRun.numberOfCharacters == 0u)
  {
    BuildOmissionResult(model,
                        ellipsisLineIndex,
                        FinalElisionResult::EllipsisOmissionReason::NO_VISIBLE_LINE,
                        result);
    return true;
  }

  const std::vector<SourceCluster> clusters = CollectSourceClusters(model, line);
  if(clusters.empty())
  {
    BuildOmissionResult(model,
                        ellipsisLineIndex,
                        FinalElisionResult::EllipsisOmissionReason::NO_VISIBLE_LINE,
                        result);
    return true;
  }

  // Style is fixed at the original source END boundary. Candidate correction
  // cannot change U+2026's authored style or font while searching.
  const CharacterIndex originalBoundary   = clusters.back().characterEnd;
  const GlyphIndex     ellipsisStyleGlyph = ResolveStyleSourceGlyph(model, line, originalBoundary);
  GlyphInfo            ellipsisGlyph;
  float                ellipsisAdvance{0.0f};
  if(!ResolveEllipsisGlyph(model,
                           ellipsisStyleGlyph,
                           fontClient,
                           ellipsisGlyph,
                           ellipsisAdvance))
  {
    BuildOmissionResult(model,
                        ellipsisLineIndex,
                        FinalElisionResult::EllipsisOmissionReason::ELLIPSIS_CANNOT_FIT,
                        result);
    return true;
  }

  std::vector<float> prefixAdvances(clusters.size() + 1u, 0.0f);
  bool               monotonicAdvances = true;
  for(size_t index = 0u; index < clusters.size(); ++index)
  {
    prefixAdvances[index + 1u] = prefixAdvances[index] + clusters[index].advance;
    monotonicAdvances          = monotonicAdvances && prefixAdvances[index + 1u] >= prefixAdvances[index];
  }
  const float  availableForPrefix = controlSize.width - ellipsisAdvance;
  const size_t maximumCandidate   = clusters.size() - 1u;
  size_t       candidate{0u};
  if(monotonicAdvances)
  {
    candidate = static_cast<size_t>(std::upper_bound(prefixAdvances.begin(),
                                                     prefixAdvances.end(),
                                                     availableForPrefix + FIT_EPSILON) -
                                    prefixAdvances.begin());
    candidate = candidate == 0u ? 0u : candidate - 1u;
  }
  else
  {
    // Character spacing can theoretically make advances non-monotonic. Keep
    // selection deterministic instead of applying an arbitrary retry limit.
    for(size_t index = 0u; index <= maximumCandidate; ++index)
    {
      if(prefixAdvances[index] <= availableForPrefix + FIT_EPSILON)
      {
        candidate = index;
      }
    }
  }
  candidate = std::min(candidate, maximumCandidate);

  const auto fits = [&](size_t retainedClusterCount)
  {
    const EndEllipsisBoundary boundary = MakeBoundaryToken(model, clusters, retainedClusterCount);
    return ProjectFinalLine(model,
                            clusters,
                            retainedClusterCount,
                            ellipsisGlyph,
                            ellipsisAdvance,
                            boundary,
                            false)
             .width <= controlSize.width + FIT_EPSILON;
  };

  // APPEND is the maximal semantic candidate: preserve every drawable source
  // cluster and project U+2026 at the last retained cluster's logical END.
  // A complete source line cannot manufacture an ellipsis this way; the
  // source LineRun must be followed by hidden logical content.
  const bool hasHiddenLogicalContinuation = HasHiddenLogicalContinuation(*model.mLogicalModel, line);
  bool       appendFits                   = false;
  if(hasHiddenLogicalContinuation)
  {
    appendFits = fits(clusters.size());
  }
  if(hasHiddenLogicalContinuation && appendFits)
  {
    EndEllipsisBoundary boundary = MakeBoundaryToken(model, clusters, clusters.size());
    DALI_ASSERT_DEBUG(boundary.kind == EndEllipsisBoundaryKind::APPEND &&
                      "APPEND requires a retained drawable anchor and removes no source glyph");
    const ProjectedLine projected = ProjectFinalLine(model,
                                                     clusters,
                                                     clusters.size(),
                                                     ellipsisGlyph,
                                                     ellipsisAdvance,
                                                     boundary,
                                                     true);
    BuildFinalResult(model,
                     ellipsisLineIndex,
                     clusters,
                     clusters.size(),
                     ellipsisStyleGlyph,
                     ellipsisGlyph,
                     projected,
                     result);
    return true;
  }

  const auto isValidCandidate = [&](size_t retainedClusterCount)
  {
    return retainedClusterCount < clusters.size() && fits(retainedClusterCount);
  };

  while(candidate > 0u && !isValidCandidate(candidate))
  {
    --candidate;
  }
  if(!isValidCandidate(candidate))
  {
    BuildOmissionResult(model,
                        ellipsisLineIndex,
                        FinalElisionResult::EllipsisOmissionReason::ELLIPSIS_CANNOT_FIT,
                        result);
    return true;
  }
  while(candidate < maximumCandidate && isValidCandidate(candidate + 1u))
  {
    ++candidate;
  }

  DALI_ASSERT_DEBUG((candidate == maximumCandidate || !isValidCandidate(candidate + 1u)) &&
                    "END ellipsis candidate is not maximal in the projected-fit domain");

  EndEllipsisBoundary boundary = MakeBoundaryToken(model, clusters, candidate);
  DALI_ASSERT_DEBUG(boundary.kind == EndEllipsisBoundaryKind::REPLACE &&
                    "Applied END ellipsis requires a drawable source boundary owner");
  const ProjectedLine projected = ProjectFinalLine(model,
                                                   clusters,
                                                   candidate,
                                                   ellipsisGlyph,
                                                   ellipsisAdvance,
                                                   boundary,
                                                   true);
  BuildFinalResult(model,
                   ellipsisLineIndex,
                   clusters,
                   candidate,
                   ellipsisStyleGlyph,
                   ellipsisGlyph,
                   projected,
                   result);
  return true;
}

void FinalizeEndEllipsisGeometry(const Model&                model,
                                 const Size&                 controlSize,
                                 Dali::LayoutDirection::Type layoutDirection,
                                 bool                        matchLayoutDirection,
                                 Layout::Engine&             layoutEngine,
                                 FinalElisionResult&         result)
{
  if(!result.resolved || !result.textElided || !model.mVisualModel)
  {
    return;
  }

  const Length characterCount = static_cast<Length>(model.mLogicalModel->mText.Count());
  AlignTextLines(layoutEngine,
                 controlSize,
                 0u,
                 characterCount,
                 model,
                 result.lines,
                 result.minimumLineOffset,
                 layoutDirection,
                 matchLayoutDirection);
  if(result.minimumLineOffset == std::numeric_limits<float>::max())
  {
    result.minimumLineOffset = 0.0f;
  }
  if(result.ellipsisLineIndex < result.lines.Count())
  {
    result.elidedOffset = result.lines[result.ellipsisLineIndex].alignmentOffset;
  }

  result.viewGlyphPositions.Resize(result.lineLocalGlyphPositions.Count());
  float lineTop = 0.0f;
  for(LineIndex lineIndex = 0u; lineIndex < result.lines.Count(); ++lineIndex)
  {
    const LineRun& line     = result.lines[lineIndex];
    const float    baseline = lineTop + line.ascender +
                           GetPreOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment());
    const GlyphIndex lineEnd = std::min<GlyphIndex>(
      line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs,
      static_cast<GlyphIndex>(result.lineLocalGlyphPositions.Count()));
    for(GlyphIndex glyph = line.glyphRun.glyphIndex; glyph < lineEnd; ++glyph)
    {
      result.viewGlyphPositions[glyph] = result.lineLocalGlyphPositions[glyph] +
                                         Vector2(line.alignmentOffset, baseline);
    }
    lineTop += GetLineHeight(line, lineIndex + 1u == result.lines.Count());
  }
}

} // namespace Dali::Ui::Text

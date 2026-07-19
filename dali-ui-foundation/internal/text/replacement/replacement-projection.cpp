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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/script.h>
#include <algorithm>
#include <cmath>

namespace Dali::Ui::Text
{
namespace
{
CharacterIndex ExclusiveEnd(const CharacterRun& run)
{
  return run.characterIndex + run.numberOfCharacters;
}

bool ContainsNewParagraph(const Vector<Character>& text, const CharacterRun& range)
{
  const CharacterIndex end = ExclusiveEnd(range);
  for(CharacterIndex index = range.characterIndex; index < end; ++index)
  {
    if(TextAbstraction::IsNewParagraph(text[index]))
    {
      return true;
    }
  }
  return false;
}

bool HasValidPayload(const ReplacementRunSnapshot& source)
{
  switch(source.type)
  {
    case ReplacementType::GENERIC:
    {
      return true;
    }
    case ReplacementType::IMAGE:
    {
      return !source.image.source.empty();
    }
  }
  return false;
}

bool IsValidReplacementSource(const Vector<Character>&      originalLogicalText,
                              const ReplacementRunSnapshot& source)
{
  const CharacterRun& range        = source.logicalCharacterRange;
  const Length        logicalCount = originalLogicalText.Count();
  return range.numberOfCharacters > 0u &&
         range.characterIndex < logicalCount &&
         range.numberOfCharacters <= logicalCount - range.characterIndex &&
         !ContainsNewParagraph(originalLogicalText, range) &&
         std::isfinite(source.metrics.width) && source.metrics.width > 0.0f &&
         std::isfinite(source.metrics.height) && source.metrics.height > 0.0f &&
         std::isfinite(source.metrics.verticalOffset) &&
         HasValidPayload(source);
}

} // unnamed namespace

ReplacementProjection ReplacementProjection::Build(const Vector<Character>&              originalLogicalText,
                                                   const Vector<ReplacementRunSnapshot>& sourceRuns,
                                                   float                                 metricScale)
{
  ReplacementProjection projection;
  projection.mLogicalText = &originalLogicalText;

  // Path A: do not sort, copy, reserve or materialize any processing buffer.
  if(sourceRuns.Empty())
  {
    return projection;
  }

  const float    safeMetricScale              = std::isfinite(metricScale) && metricScale > 0.0f ? metricScale : 1.0f;
  const Length   logicalCount                 = originalLogicalText.Count();
  bool           hasMultiCharacterReplacement = false;
  bool           acceptedInLogicalOrder       = true;
  CharacterIndex maxAcceptedEnd               = 0u;

  for(uint32_t sourceIndex = 0u; sourceIndex < sourceRuns.Count(); ++sourceIndex)
  {
    const ReplacementRunSnapshot& source = sourceRuns[sourceIndex];
    const CharacterRun&           range  = source.logicalCharacterRange;
    bool                          valid  = IsValidReplacementSource(originalLogicalText, source);

    if(valid)
    {
      const CharacterIndex rangeEnd = ExclusiveEnd(range);
      // SetSpan() calls normally follow append order. Those ranges are known
      // not to overlap once they start after the greatest accepted end.
      if(range.characterIndex < maxAcceptedEnd)
      {
        for(const ProjectedReplacementRun& accepted : projection.mReplacementRuns)
        {
          const CharacterRun& acceptedRange = accepted.logicalCharacterRange;
          if(range.characterIndex < ExclusiveEnd(acceptedRange) && acceptedRange.characterIndex < rangeEnd)
          {
            valid = false;
            break;
          }
        }
      }
    }

    if(!valid)
    {
      continue;
    }

    ProjectedReplacementRun run;
    run.logicalCharacterRange = range;
    run.metrics               = source.metrics;
    run.metrics.width *= safeMetricScale;
    run.metrics.height *= safeMetricScale;
    run.metrics.verticalOffset *= safeMetricScale;
    if(!std::isfinite(run.metrics.width) || !std::isfinite(run.metrics.height) ||
       !std::isfinite(run.metrics.verticalOffset))
    {
      continue;
    }
    hasMultiCharacterReplacement |= range.numberOfCharacters > 1u;
    acceptedInLogicalOrder &= projection.mReplacementRuns.Empty() ||
                              projection.mReplacementRuns.Back().logicalCharacterRange.characterIndex < range.characterIndex;
    maxAcceptedEnd         = std::max(maxAcceptedEnd, ExclusiveEnd(range));
    run.sourceRunIndex     = sourceIndex;
    run.occurrenceIdentity = source.occurrenceIdentity;
    projection.mReplacementRuns.PushBack(run);
  }

  if(projection.mReplacementRuns.Empty())
  {
    return projection;
  }

  // Validation honors authored insertion order; every downstream projection
  // structure is stored in logical order.
  if(!acceptedInLogicalOrder)
  {
    std::sort(projection.mReplacementRuns.Begin(), projection.mReplacementRuns.End(),
              [](const ProjectedReplacementRun& lhs, const ProjectedReplacementRun& rhs)
    {
      return lhs.logicalCharacterRange.characterIndex < rhs.logicalCharacterRange.characterIndex;
    });
  }

  if(!hasMultiCharacterReplacement)
  {
    // Path B keeps identity indices and never creates mapping tables. Canonical markup U+FFFC can retain the source
    // buffer. A programmatic one-character replacement over another code point needs a same-length processing copy so
    // its script and bidi class cannot leak into layout.
    projection.mMode                   = Mode::IDENTITY;
    bool requiresNeutralProcessingCopy = false;
    for(ProjectedReplacementRun& run : projection.mReplacementRuns)
    {
      run.projectedCharacterIndex = run.logicalCharacterRange.characterIndex;
      requiresNeutralProcessingCopy |=
        originalLogicalText[run.logicalCharacterRange.characterIndex] != OBJECT_REPLACEMENT_CHARACTER;
    }
    if(requiresNeutralProcessingCopy)
    {
      projection.mProcessingText = originalLogicalText;
      for(const ProjectedReplacementRun& run : projection.mReplacementRuns)
      {
        projection.mProcessingText[run.projectedCharacterIndex] = OBJECT_REPLACEMENT_CHARACTER;
      }
    }
    return projection;
  }

  // Path C: materialize compact processing text and both character-domain maps.
  projection.mMode       = Mode::COMPACT;
  Length processingCount = logicalCount;
  for(const ProjectedReplacementRun& run : projection.mReplacementRuns)
  {
    processingCount -= run.logicalCharacterRange.numberOfCharacters - 1u;
  }

  projection.mProcessingText.Resize(processingCount);
  projection.mLogicalToProjectedCharacters.Resize(logicalCount);
  projection.mProjectedToLogicalCharacters.Resize(processingCount);

  CharacterIndex logicalIndex     = 0u;
  CharacterIndex projectedIndex   = 0u;
  uint32_t       replacementIndex = 0u;
  while(logicalIndex < logicalCount)
  {
    ProjectedReplacementRun* replacement =
      replacementIndex < projection.mReplacementRuns.Count() ? &projection.mReplacementRuns[replacementIndex]
                                                             : nullptr;
    if(replacement != nullptr && replacement->logicalCharacterRange.characterIndex == logicalIndex)
    {
      replacement->projectedCharacterIndex                     = projectedIndex;
      projection.mProcessingText[projectedIndex]               = OBJECT_REPLACEMENT_CHARACTER;
      projection.mProjectedToLogicalCharacters[projectedIndex] = logicalIndex;

      const CharacterIndex end = ExclusiveEnd(replacement->logicalCharacterRange);
      while(logicalIndex < end)
      {
        projection.mLogicalToProjectedCharacters[logicalIndex++] = projectedIndex;
      }
      ++projectedIndex;
      ++replacementIndex;
      continue;
    }

    projection.mProcessingText[projectedIndex]               = originalLogicalText[logicalIndex];
    projection.mLogicalToProjectedCharacters[logicalIndex]   = projectedIndex;
    projection.mProjectedToLogicalCharacters[projectedIndex] = logicalIndex;
    ++logicalIndex;
    ++projectedIndex;
  }

  return projection;
}

bool ReplacementProjection::HasValidSource(const Vector<Character>&              originalLogicalText,
                                           const Vector<ReplacementRunSnapshot>& sourceRuns)
{
  for(const ReplacementRunSnapshot& source : sourceRuns)
  {
    if(IsValidReplacementSource(originalLogicalText, source))
    {
      return true;
    }
  }
  return false;
}

ReplacementProjection::Mode ReplacementProjection::GetMode() const
{
  return mMode;
}

bool ReplacementProjection::HasReplacements() const
{
  return !mReplacementRuns.Empty();
}

bool ReplacementProjection::UsesOriginalTextBuffer() const
{
  return mProcessingText.Empty();
}

const Vector<Character>& ReplacementProjection::GetLogicalText() const
{
  return *mLogicalText;
}

const Vector<Character>& ReplacementProjection::GetProcessingText() const
{
  return mProcessingText.Empty() ? *mLogicalText : mProcessingText;
}

const Vector<ProjectedReplacementRun>& ReplacementProjection::GetReplacementRuns() const
{
  return mReplacementRuns;
}

Length ReplacementProjection::GetLogicalCharacterCount() const
{
  return mLogicalText == nullptr ? 0u : mLogicalText->Count();
}

Length ReplacementProjection::GetProcessingCharacterCount() const
{
  return GetProcessingText().Count();
}

CharacterIndex ReplacementProjection::LogicalCharacterToProjected(CharacterIndex logicalIndex) const
{
  if(logicalIndex >= GetLogicalCharacterCount())
  {
    return GetProcessingCharacterCount();
  }
  return mMode == Mode::COMPACT ? mLogicalToProjectedCharacters[logicalIndex] : logicalIndex;
}

CharacterIndex ReplacementProjection::ProjectedCharacterToLogical(CharacterIndex projectedIndex) const
{
  if(projectedIndex >= GetProcessingCharacterCount())
  {
    return GetLogicalCharacterCount();
  }
  return mMode == Mode::COMPACT ? mProjectedToLogicalCharacters[projectedIndex] : projectedIndex;
}

CharacterIndex ReplacementProjection::NormalizeLogicalBoundary(CharacterIndex   logicalBoundary,
                                                               BoundaryAffinity affinity) const
{
  logicalBoundary                    = std::min(logicalBoundary, GetLogicalCharacterCount());
  const ProjectedReplacementRun* run = FindByLogicalCharacter(logicalBoundary);
  if(run != nullptr && logicalBoundary > run->logicalCharacterRange.characterIndex)
  {
    return affinity == BoundaryAffinity::LEADING ? run->logicalCharacterRange.characterIndex
                                                 : ExclusiveEnd(run->logicalCharacterRange);
  }
  return logicalBoundary;
}

CharacterIndex ReplacementProjection::LogicalBoundaryToProjected(CharacterIndex   logicalBoundary,
                                                                 BoundaryAffinity affinity) const
{
  const CharacterIndex normalized = NormalizeLogicalBoundary(logicalBoundary, affinity);
  return normalized >= GetLogicalCharacterCount() ? GetProcessingCharacterCount()
                                                  : LogicalCharacterToProjected(normalized);
}

CharacterIndex ReplacementProjection::ProjectedBoundaryToLogical(CharacterIndex projectedBoundary) const
{
  return projectedBoundary >= GetProcessingCharacterCount() ? GetLogicalCharacterCount()
                                                            : ProjectedCharacterToLogical(projectedBoundary);
}

const ProjectedReplacementRun* ReplacementProjection::FindByLogicalCharacter(CharacterIndex logicalIndex) const
{
  uint32_t first = 0u;
  uint32_t last  = mReplacementRuns.Count();
  while(first < last)
  {
    const uint32_t                 mid = first + (last - first) / 2u;
    const ProjectedReplacementRun& run = mReplacementRuns[mid];
    if(logicalIndex < run.logicalCharacterRange.characterIndex)
    {
      last = mid;
    }
    else if(logicalIndex >= ExclusiveEnd(run.logicalCharacterRange))
    {
      first = mid + 1u;
    }
    else
    {
      return &run;
    }
  }
  return nullptr;
}

const ProjectedReplacementRun* ReplacementProjection::FindByProjectedCharacter(CharacterIndex projectedIndex) const
{
  uint32_t first = 0u;
  uint32_t last  = mReplacementRuns.Count();
  while(first < last)
  {
    const uint32_t                 mid = first + (last - first) / 2u;
    const ProjectedReplacementRun& run = mReplacementRuns[mid];
    if(projectedIndex < run.projectedCharacterIndex)
    {
      last = mid;
    }
    else if(projectedIndex > run.projectedCharacterIndex)
    {
      first = mid + 1u;
    }
    else
    {
      return &run;
    }
  }
  return nullptr;
}

CharacterIndex ReplacementProjection::HitTestLogicalBoundary(CharacterIndex projectedIndex, float xWithinBox,
                                                             float boxWidth, bool visualRightToLeft) const
{
  const ProjectedReplacementRun* run = FindByProjectedCharacter(projectedIndex);
  if(run == nullptr)
  {
    const bool trailing = xWithinBox >= boxWidth * 0.5f;
    return ProjectedBoundaryToLogical(projectedIndex + (trailing ? 1u : 0u));
  }

  const bool physicalRightHalf = xWithinBox >= boxWidth * 0.5f;
  const bool logicalTrailing   = visualRightToLeft ? !physicalRightHalf : physicalRightHalf;
  return logicalTrailing ? ExclusiveEnd(run->logicalCharacterRange) : run->logicalCharacterRange.characterIndex;
}

const ProjectedReplacementRun* ReplacementProjection::FindRunEndingAt(CharacterIndex logicalBoundary) const
{
  for(uint32_t index = 0u; index < mReplacementRuns.Count(); ++index)
  {
    if(ExclusiveEnd(mReplacementRuns[index].logicalCharacterRange) == logicalBoundary)
    {
      return &mReplacementRuns[index];
    }
  }
  return nullptr;
}

const ProjectedReplacementRun* ReplacementProjection::FindRunStartingAt(CharacterIndex logicalBoundary) const
{
  for(uint32_t index = 0u; index < mReplacementRuns.Count(); ++index)
  {
    if(mReplacementRuns[index].logicalCharacterRange.characterIndex == logicalBoundary)
    {
      return &mReplacementRuns[index];
    }
  }
  return nullptr;
}

CharacterRun ReplacementProjection::GetDeletionRange(CharacterIndex logicalBoundary, bool backward) const
{
  const Length logicalCount = GetLogicalCharacterCount();
  logicalBoundary           = std::min(logicalBoundary, logicalCount);

  const ProjectedReplacementRun* run = FindByLogicalCharacter(logicalBoundary);
  if(backward)
  {
    // A cursor exactly at an object's start belongs to what precedes it. This is also important at the shared boundary
    // of two adjacent replacements: Backspace selects the left object, Delete selects the right object.
    if(run != nullptr && run->logicalCharacterRange.characterIndex == logicalBoundary)
    {
      run = nullptr;
    }
    if(run == nullptr)
    {
      run = FindRunEndingAt(logicalBoundary);
    }
  }
  else if(run == nullptr)
  {
    run = FindRunStartingAt(logicalBoundary);
  }
  if(run != nullptr)
  {
    return run->logicalCharacterRange;
  }

  if(backward)
  {
    return logicalBoundary == 0u ? CharacterRun{} : CharacterRun{logicalBoundary - 1u, 1u};
  }
  return logicalBoundary >= logicalCount ? CharacterRun{} : CharacterRun{logicalBoundary, 1u};
}

bool ReplacementProjection::ProjectGlyphStyleRuns(const Vector<CharacterRun>&    logicalRuns,
                                                  Vector<ProjectedStyleSegment>& projectedSegments) const
{
  if(!HasReplacements())
  {
    return false;
  }

  uint32_t replacementCursor = 0u;
  for(uint32_t styleIndex = 0u; styleIndex < logicalRuns.Count(); ++styleIndex)
  {
    const CharacterRun& style = logicalRuns[styleIndex];
    if(style.numberOfCharacters == 0u || style.characterIndex >= GetLogicalCharacterCount())
    {
      continue;
    }

    const CharacterIndex styleEnd =
      std::min(ExclusiveEnd(style), static_cast<CharacterIndex>(GetLogicalCharacterCount()));
    while(replacementCursor < mReplacementRuns.Count() &&
          ExclusiveEnd(mReplacementRuns[replacementCursor].logicalCharacterRange) <= style.characterIndex)
    {
      ++replacementCursor;
    }

    CharacterIndex cursor           = style.characterIndex;
    uint32_t       replacementIndex = replacementCursor;
    while(replacementIndex < mReplacementRuns.Count())
    {
      const ProjectedReplacementRun& replacement = mReplacementRuns[replacementIndex];
      if(replacement.logicalCharacterRange.characterIndex >= styleEnd)
      {
        break;
      }

      if(cursor < replacement.logicalCharacterRange.characterIndex)
      {
        const CharacterIndex segmentEnd = std::min(styleEnd, replacement.logicalCharacterRange.characterIndex);
        projectedSegments.PushBack({CharacterRun{LogicalBoundaryToProjected(cursor, BoundaryAffinity::TRAILING),
                                                 segmentEnd - cursor},
                                    styleIndex});
      }
      cursor = std::max(cursor, ExclusiveEnd(replacement.logicalCharacterRange));
      if(cursor >= styleEnd)
      {
        break;
      }
      ++replacementIndex;
    }

    if(cursor < styleEnd)
    {
      const CharacterIndex projectedStart = LogicalBoundaryToProjected(cursor, BoundaryAffinity::TRAILING);
      const CharacterIndex projectedEnd   = LogicalBoundaryToProjected(styleEnd, BoundaryAffinity::LEADING);
      projectedSegments.PushBack({CharacterRun{projectedStart, projectedEnd - projectedStart},
                                  styleIndex});
    }
  }
  return true;
}

} // namespace Dali::Ui::Text

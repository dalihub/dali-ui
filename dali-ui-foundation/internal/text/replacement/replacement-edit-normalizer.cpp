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

#include <dali-ui-foundation/internal/text/replacement/replacement-edit-normalizer.h>

#include <algorithm>
#include <cstdint>

namespace Dali::Ui::Text
{
namespace
{
CharacterIndex ExclusiveEnd(const CharacterRun& run)
{
  return run.characterIndex + run.numberOfCharacters;
}

bool Intersects(CharacterIndex firstStart,
                CharacterIndex firstEnd,
                CharacterIndex secondStart,
                CharacterIndex secondEnd)
{
  return firstStart < secondEnd && secondStart < firstEnd;
}
} // unnamed namespace

CharacterIndex ReplacementEditNormalizer::NormalizeBoundary(const Vector<ReplacementRunSnapshot>& runs,
                                                            CharacterIndex                        boundary,
                                                            BoundaryAffinity                      affinity,
                                                            Length                                textLength)
{
  boundary = std::min(boundary, static_cast<CharacterIndex>(textLength));
  for(const ReplacementRunSnapshot& run : runs)
  {
    const CharacterIndex start = run.logicalCharacterRange.characterIndex;
    const CharacterIndex end   = ExclusiveEnd(run.logicalCharacterRange);
    if(boundary > start && boundary < end)
    {
      return affinity == BoundaryAffinity::LEADING ? start : end;
    }
  }
  return boundary;
}

void ReplacementEditNormalizer::NormalizeSelection(const Vector<ReplacementRunSnapshot>& runs,
                                                   CharacterIndex&                       first,
                                                   CharacterIndex&                       second,
                                                   Length                                textLength)
{
  first  = std::min(first, static_cast<CharacterIndex>(textLength));
  second = std::min(second, static_cast<CharacterIndex>(textLength));
  if(first == second)
  {
    first = second = NormalizeBoundary(runs, first, BoundaryAffinity::LEADING, textLength);
    return;
  }

  const bool     reverse = first > second;
  CharacterIndex start   = std::min(first, second);
  CharacterIndex end     = std::max(first, second);

  bool changed = true;
  while(changed)
  {
    changed = false;
    for(const ReplacementRunSnapshot& run : runs)
    {
      const CharacterIndex runStart = run.logicalCharacterRange.characterIndex;
      const CharacterIndex runEnd   = ExclusiveEnd(run.logicalCharacterRange);
      if(Intersects(start, end, runStart, runEnd))
      {
        const CharacterIndex expandedStart = std::min(start, runStart);
        const CharacterIndex expandedEnd   = std::max(end, runEnd);
        changed                            = expandedStart != start || expandedEnd != end;
        start                              = expandedStart;
        end                                = expandedEnd;
      }
    }
  }

  first  = reverse ? end : start;
  second = reverse ? start : end;
}

CharacterRun ReplacementEditNormalizer::GetDeletionRange(const Vector<ReplacementRunSnapshot>& runs,
                                                         CharacterIndex                        boundary,
                                                         bool                                  backward,
                                                         Length                                textLength)
{
  boundary = std::min(boundary, static_cast<CharacterIndex>(textLength));
  for(const ReplacementRunSnapshot& run : runs)
  {
    const CharacterIndex start = run.logicalCharacterRange.characterIndex;
    const CharacterIndex end   = ExclusiveEnd(run.logicalCharacterRange);
    if((backward && ((boundary > start && boundary <= end) || boundary == end)) ||
       (!backward && ((boundary >= start && boundary < end) || boundary == start)))
    {
      return run.logicalCharacterRange;
    }
  }

  CharacterRun deletion;
  if(backward)
  {
    if(boundary > 0u)
    {
      deletion = {boundary - 1u, 1u};
    }
  }
  else if(boundary < textLength)
  {
    deletion = {boundary, 1u};
  }
  return NormalizeDeletion(runs, deletion, textLength);
}

CharacterRun ReplacementEditNormalizer::NormalizeDeletion(const Vector<ReplacementRunSnapshot>& runs,
                                                          CharacterRun                          deletion,
                                                          Length                                textLength)
{
  CharacterIndex start        = std::min(deletion.characterIndex, static_cast<CharacterIndex>(textLength));
  const uint64_t requestedEnd = static_cast<uint64_t>(deletion.characterIndex) + deletion.numberOfCharacters;
  CharacterIndex end          = static_cast<CharacterIndex>(std::min<uint64_t>(requestedEnd, textLength));
  if(start >= end)
  {
    return {};
  }

  bool changed = true;
  while(changed)
  {
    changed = false;
    for(const ReplacementRunSnapshot& run : runs)
    {
      const CharacterIndex runStart = run.logicalCharacterRange.characterIndex;
      const CharacterIndex runEnd   = ExclusiveEnd(run.logicalCharacterRange);
      if(Intersects(start, end, runStart, runEnd))
      {
        const CharacterIndex expandedStart = std::min(start, runStart);
        const CharacterIndex expandedEnd   = std::max(end, runEnd);
        changed                            = expandedStart != start || expandedEnd != end;
        start                              = expandedStart;
        end                                = expandedEnd;
      }
    }
  }
  end = std::min(end, static_cast<CharacterIndex>(textLength));
  return {start, end - start};
}

void ReplacementEditNormalizer::ApplyEdit(Vector<ReplacementRunSnapshot>& runs,
                                          CharacterIndex                  start,
                                          Length                          removedLength,
                                          Length                          insertedLength)
{
  const CharacterIndex           removedEnd = start + removedLength;
  Vector<ReplacementRunSnapshot> updated;
  updated.Reserve(runs.Count());

  for(const ReplacementRunSnapshot& sourceRun : runs)
  {
    ReplacementRunSnapshot run      = sourceRun;
    CharacterIndex         runStart = run.logicalCharacterRange.characterIndex;
    const CharacterIndex   runEnd   = ExclusiveEnd(run.logicalCharacterRange);

    if(removedLength > 0u && Intersects(start, removedEnd, runStart, runEnd))
    {
      continue;
    }
    if(removedLength > 0u && runStart >= removedEnd)
    {
      runStart -= removedLength;
    }
    if(insertedLength > 0u && start <= runStart)
    {
      runStart += insertedLength;
    }
    run.logicalCharacterRange.characterIndex = runStart;
    updated.PushBack(std::move(run));
  }
  runs = std::move(updated);
}

} // namespace Dali::Ui::Text

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

#include <dali-ui-foundation/internal/text/text-alignment.h>

namespace Dali::Ui::Text
{
void AlignTextLines(Layout::Engine&             layoutEngine,
                    const Size&                 controlSize,
                    CharacterIndex              startIndex,
                    Length                      numberOfCharacters,
                    const Model&                model,
                    Vector<LineRun>&            lines,
                    float&                      minimumLineOffset,
                    Dali::LayoutDirection::Type layoutDirection,
                    bool                        matchLayoutDirection)
{
  if(numberOfCharacters == 0u)
  {
    return;
  }

  const Length numberOfBoundedParagraphRuns = model.GetNumberOfBoundedParagraphRuns();
  const bool   followsControllerAlignment =
    numberOfBoundedParagraphRuns == 0u ||
    (Layout::Engine::SINGLE_LINE_BOX == layoutEngine.GetLayout() &&
     model.GetBoundedParagraphRuns()[0u].characterRun.numberOfCharacters != model.mLogicalModel->mText.Count());

  if(followsControllerAlignment)
  {
    layoutEngine.Align(controlSize,
                       startIndex,
                       numberOfCharacters,
                       model.mHorizontalAlignment,
                       lines,
                       minimumLineOffset,
                       layoutDirection,
                       matchLayoutDirection);
    return;
  }

  const Vector<BoundedParagraphRun>& boundedParagraphRuns = model.GetBoundedParagraphRuns();
  const CharacterIndex               alignEndIndex        = startIndex + numberOfCharacters - 1u;
  CharacterIndex                     alignIndex           = startIndex;
  Length                             boundedRunIndex      = 0u;

  while(alignIndex <= alignEndIndex && boundedRunIndex < numberOfBoundedParagraphRuns)
  {
    const BoundedParagraphRun& boundedRun      = boundedParagraphRuns[boundedRunIndex];
    const CharacterIndex       paragraphStart  = boundedRun.characterRun.characterIndex;
    const Length               paragraphLength = boundedRun.characterRun.numberOfCharacters;
    if(paragraphLength == 0u)
    {
      ++boundedRunIndex;
      continue;
    }
    const CharacterIndex paragraphEnd = paragraphStart + paragraphLength - 1u;

    CharacterIndex  decidedStart     = alignIndex;
    Length          decidedLength    = alignEndIndex - alignIndex + 1u;
    Text::Alignment decidedAlignment = model.mHorizontalAlignment;

    if(alignIndex < paragraphStart && paragraphStart <= alignEndIndex)
    {
      decidedLength = paragraphStart - alignIndex;
      alignIndex    = paragraphStart;
    }
    else if((paragraphStart <= alignIndex && alignIndex <= paragraphEnd) ||
            (paragraphStart <= alignEndIndex && alignEndIndex <= paragraphEnd))
    {
      decidedStart     = paragraphStart;
      decidedLength    = paragraphLength;
      decidedAlignment = boundedRun.horizontalAlignmentDefined ? boundedRun.horizontalAlignment
                                                               : model.mHorizontalAlignment;
      alignIndex       = paragraphEnd + 1u;
      ++boundedRunIndex;
    }
    else
    {
      ++boundedRunIndex;
      continue;
    }

    layoutEngine.Align(controlSize,
                       decidedStart,
                       decidedLength,
                       decidedAlignment,
                       lines,
                       minimumLineOffset,
                       layoutDirection,
                       matchLayoutDirection);
  }

  if(alignIndex <= alignEndIndex)
  {
    layoutEngine.Align(controlSize,
                       alignIndex,
                       alignEndIndex - alignIndex + 1u,
                       model.mHorizontalAlignment,
                       lines,
                       minimumLineOffset,
                       layoutDirection,
                       matchLayoutDirection);
  }
}

} // namespace Dali::Ui::Text

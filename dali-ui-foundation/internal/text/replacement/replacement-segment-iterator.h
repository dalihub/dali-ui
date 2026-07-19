#ifndef DALI_UI_TEXT_REPLACEMENT_SEGMENT_ITERATOR_H
#define DALI_UI_TEXT_REPLACEMENT_SEGMENT_ITERATOR_H

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

#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>

namespace Dali::Ui::Text
{
/**
 * @brief Iterates ordinary and replacement segments in projected text.
 *
 * The callbacks are invoked directly without materializing a segment vector.
 *
 * @param[in] characterCount The number of characters in the projected text.
 * @param[in] replacements The accepted projected replacement runs.
 * @param[in] ordinaryCallback The callback invoked for ordinary character ranges.
 * @param[in] replacementCallback The callback invoked for replacement units.
 */
template<typename OrdinaryCallback, typename ReplacementCallback>
void ForEachProcessingSegment(Length                                 characterCount,
                              const Vector<ProjectedReplacementRun>& replacements,
                              OrdinaryCallback&&                     ordinaryCallback,
                              ReplacementCallback&&                  replacementCallback)
{
  CharacterIndex processingIndex  = 0u;
  uint32_t       replacementIndex = 0u;
  while(processingIndex < characterCount)
  {
    const ProjectedReplacementRun* replacement =
      replacementIndex < replacements.Count() ? &replacements[replacementIndex] : nullptr;
    if(replacement != nullptr && replacement->projectedCharacterIndex == processingIndex)
    {
      replacementCallback(*replacement);
      ++processingIndex;
      ++replacementIndex;
      continue;
    }

    const CharacterIndex ordinaryEnd =
      replacement == nullptr ? characterCount : replacement->projectedCharacterIndex;
    if(ordinaryEnd > processingIndex)
    {
      ordinaryCallback(processingIndex, ordinaryEnd - processingIndex);
    }
    processingIndex = ordinaryEnd;
  }
}

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_SEGMENT_ITERATOR_H

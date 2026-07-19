#ifndef DALI_UI_TEXT_REPLACEMENT_PROCESSING_SOURCE_H
#define DALI_UI_TEXT_REPLACEMENT_PROCESSING_SOURCE_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores immutable source buffers consumed by a model updater.
 *
 * With no replacement every pointer aliases the original LogicalModel buffers.
 */
struct TextProcessingSource
{
  const Vector<Character>*                    text{nullptr};
  const Vector<FontDescriptionRun>*           fontDescriptionRuns{nullptr};
  const Vector<ColorRun>*                     colorRuns{nullptr};
  const Vector<ColorRun>*                     backgroundRuns{nullptr};
  const Vector<UnderlinedCharacterRun>*       underlineRuns{nullptr};
  const Vector<StrikethroughCharacterRun>*    strikethroughRuns{nullptr};
  const Vector<CharacterSpacingCharacterRun>* characterSpacingRuns{nullptr};
  const ReplacementProjection*                replacementProjection{nullptr};
  const Vector<ProjectedReplacementRun>*      replacementRuns{nullptr};

  /**
   * @brief Checks whether the source contains accepted replacements.
   *
   * @return true if the source contains replacement runs.
   */
  bool HasReplacements() const
  {
    return replacementRuns != nullptr && !replacementRuns->Empty();
  }
};

/**
 * @brief Owns the projected source buffers that differ from the original model.
 */
struct ProjectedTextProcessingSource
{
  TextProcessingSource                 source;
  Vector<FontDescriptionRun>           fontDescriptionRuns;
  Vector<ColorRun>                     colorRuns;
  Vector<ColorRun>                     backgroundRuns;
  Vector<UnderlinedCharacterRun>       underlineRuns;
  Vector<StrikethroughCharacterRun>    strikethroughRuns;
  Vector<CharacterSpacingCharacterRun> characterSpacingRuns;
};

/**
 * @brief Creates a processing source that references an ordinary model.
 *
 * @param[in] model The model containing the source buffers.
 * @return A non-owning view of the model's source buffers.
 */
TextProcessingSource MakeTextProcessingSource(const Model& model);

/**
 * @brief Prepares projected source buffers for replacement processing.
 *
 * Line breaks, scripts, validated fonts, bidi data and glyphs are produced by
 * the caller's canonical model updater.
 *
 * @param[in] originalModel The immutable logical source model.
 * @param[in] projection The accepted replacement projection.
 * @param[out] storage The storage that owns the projected source buffers.
 * @return true if the projected processing source was prepared.
 */
bool PrepareProjectedTextProcessingSource(const Model&                   originalModel,
                                          const ReplacementProjection&   projection,
                                          ProjectedTextProcessingSource& storage);

/**
 * @brief Applies processing source buffers to a target logical model.
 *
 * @param[in] source The source buffers to apply.
 * @param[in,out] target The logical model receiving the source buffers.
 */
void ApplyTextProcessingSource(const TextProcessingSource& source, LogicalModel& target);

/**
 * @brief Copies non-derived text properties to a target model.
 *
 * @param[in] source The model containing the properties to copy.
 * @param[in,out] target The model receiving the properties.
 */
void CopyTextProcessingProperties(const Model& source, Model& target);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_PROCESSING_SOURCE_H

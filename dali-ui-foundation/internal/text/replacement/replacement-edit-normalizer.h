#ifndef DALI_UI_TEXT_REPLACEMENT_EDIT_NORMALIZER_H
#define DALI_UI_TEXT_REPLACEMENT_EDIT_NORMALIZER_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>

namespace Dali::Ui::Text
{
/**
 * @brief Normalizes logical edits around atomic replacement ranges.
 */
class ReplacementEditNormalizer
{
public:
  enum class BoundaryAffinity : uint8_t
  {
    LEADING,
    TRAILING
  };

  /**
   * @brief Normalizes a cursor boundary.
   *
   * @param[in] runs The replacement runs.
   * @param[in] boundary The logical cursor boundary.
   * @param[in] affinity The boundary affinity.
   * @param[in] textLength The logical text length.
   * @return The normalized boundary.
   */
  static CharacterIndex NormalizeBoundary(const Vector<ReplacementRunSnapshot>& runs,
                                          CharacterIndex                        boundary,
                                          BoundaryAffinity                      affinity,
                                          Length                                textLength);

  /**
   * @brief Expands a selection to include intersected replacements.
   *
   * The original selection direction is retained.
   *
   * @param[in] runs The replacement runs.
   * @param[in,out] first The first selection boundary.
   * @param[in,out] second The second selection boundary.
   * @param[in] textLength The logical text length.
   */
  static void NormalizeSelection(const Vector<ReplacementRunSnapshot>& runs,
                                 CharacterIndex&                       first,
                                 CharacterIndex&                       second,
                                 Length                                textLength);

  /**
   * @brief Gets the logical range for Backspace or Delete.
   *
   * @param[in] runs The replacement runs.
   * @param[in] boundary The logical cursor boundary.
   * @param[in] backward Whether Backspace is requested.
   * @param[in] textLength The logical text length.
   * @return The normalized deletion range.
   */
  static CharacterRun GetDeletionRange(const Vector<ReplacementRunSnapshot>& runs,
                                       CharacterIndex                        boundary,
                                       bool                                  backward,
                                       Length                                textLength);

  /**
   * @brief Expands a deletion range to include intersected replacements.
   *
   * @param[in] runs The replacement runs.
   * @param[in] deletion The requested logical range.
   * @param[in] textLength The logical text length.
   * @return The normalized deletion range.
   */
  static CharacterRun NormalizeDeletion(const Vector<ReplacementRunSnapshot>& runs,
                                        CharacterRun                          deletion,
                                        Length                                textLength);

  /**
   * @brief Updates replacement ranges after a logical edit.
   *
   * The deletion range must already be normalized. A replacement intersecting
   * the removed range is discarded.
   *
   * @param[in,out] runs The replacement runs.
   * @param[in] start The edit start boundary.
   * @param[in] removedLength The removed logical length.
   * @param[in] insertedLength The inserted logical length.
   */
  static void ApplyEdit(Vector<ReplacementRunSnapshot>& runs,
                        CharacterIndex                  start,
                        Length                          removedLength,
                        Length                          insertedLength);
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_EDIT_NORMALIZER_H

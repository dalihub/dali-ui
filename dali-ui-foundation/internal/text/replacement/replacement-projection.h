#ifndef DALI_UI_TEXT_REPLACEMENT_PROJECTION_H
#define DALI_UI_TEXT_REPLACEMENT_PROJECTION_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-run.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores a replacement in the logical and processing domains.
 */
struct ProjectedReplacementRun
{
  CharacterRun       logicalCharacterRange{};
  CharacterIndex     projectedCharacterIndex{0u};
  ReplacementMetrics metrics{};
  uint32_t           sourceRunIndex{0u};
  uint64_t           occurrenceIdentity{0u};
};

/**
 * @brief Stores a projected style segment.
 */
struct ProjectedStyleSegment
{
  CharacterRun projectedCharacterRange{};
  uint32_t     sourceRunIndex{0u};
};

/**
 * @brief Projects range replacements before shaping.
 *
 * NONE retains the caller-owned logical text buffer. IDENTITY does not allocate
 * mapping tables. COMPACT owns shortened processing text and index maps.
 */
class ReplacementProjection
{
public:
  static constexpr Character OBJECT_REPLACEMENT_CHARACTER = 0xFFFcu;

  enum class Mode : uint8_t
  {
    NONE,
    IDENTITY,
    COMPACT
  };

  enum class BoundaryAffinity : uint8_t
  {
    LEADING,
    TRAILING
  };

  ReplacementProjection()                                        = default;
  ReplacementProjection(const ReplacementProjection&)            = delete;
  ReplacementProjection& operator=(const ReplacementProjection&) = delete;
  ReplacementProjection(ReplacementProjection&&)                 = default;
  ReplacementProjection& operator=(ReplacementProjection&&)      = default;

  /**
   * @brief Builds a replacement projection for immutable logical text.
   *
   * Every valid non-empty UTF-32 range is projected exactly as authored. No
   * grapheme, shaping, cursor or emoji boundary normalization is performed.
   *
   * @param[in] originalLogicalText The UTF-32 source text. It must outlive the projection.
   * @param[in] sourceRuns The authored replacement runs in attachment order.
   * @param[in] metricScale The scale applied to projected replacement metrics.
   * @return The resulting replacement projection.
   */
  static ReplacementProjection Build(const Vector<Character>&              originalLogicalText,
                                     const Vector<ReplacementRunSnapshot>& sourceRuns,
                                     float                                 metricScale = 1.0f);

  /**
   * @brief Checks whether source runs contain an applicable replacement.
   *
   * @param[in] originalLogicalText The UTF-32 source text.
   * @param[in] sourceRuns The authored replacement runs.
   * @return true if at least one replacement can be projected.
   */
  static bool HasValidSource(const Vector<Character>&              originalLogicalText,
                             const Vector<ReplacementRunSnapshot>& sourceRuns);

  /**
   * @brief Gets the projection mode.
   *
   * @return The projection mode.
   */
  Mode GetMode() const;

  /**
   * @brief Checks whether the projection contains replacements.
   *
   * @return true if replacements were projected.
   */
  bool HasReplacements() const;

  /**
   * @brief Checks whether processing uses the source text buffer.
   *
   * @return true if the source text buffer is used directly.
   */
  bool UsesOriginalTextBuffer() const;

  /**
   * @brief Gets the logical source text.
   *
   * @return The logical source text.
   */
  const Vector<Character>& GetLogicalText() const;

  /**
   * @brief Gets the processing text.
   *
   * @return The processing text.
   */
  const Vector<Character>& GetProcessingText() const;

  /**
   * @brief Gets the projected replacement runs.
   *
   * @return The projected replacement runs.
   */
  const Vector<ProjectedReplacementRun>& GetReplacementRuns() const;

  /**
   * @brief Gets the logical character count.
   *
   * @return The logical character count.
   */
  Length GetLogicalCharacterCount() const;

  /**
   * @brief Gets the processing character count.
   *
   * @return The processing character count.
   */
  Length GetProcessingCharacterCount() const;

  /**
   * @brief Maps a logical character index to the processing text.
   *
   * @param[in] logicalIndex The logical character index.
   * @return The processing character index.
   */
  CharacterIndex LogicalCharacterToProjected(CharacterIndex logicalIndex) const;

  /**
   * @brief Maps a processing character index to the logical text.
   *
   * @param[in] projectedIndex The processing character index.
   * @return The logical character index.
   */
  CharacterIndex ProjectedCharacterToLogical(CharacterIndex projectedIndex) const;

  /**
   * @brief Normalizes a logical boundary around an atomic replacement.
   *
   * @param[in] logicalBoundary The logical boundary.
   * @param[in] affinity The boundary affinity.
   * @return The normalized logical boundary.
   */
  CharacterIndex NormalizeLogicalBoundary(CharacterIndex logicalBoundary, BoundaryAffinity affinity) const;

  /**
   * @brief Maps a logical boundary to the processing text.
   *
   * @param[in] logicalBoundary The logical boundary.
   * @param[in] affinity The boundary affinity.
   * @return The processing boundary.
   */
  CharacterIndex LogicalBoundaryToProjected(CharacterIndex logicalBoundary, BoundaryAffinity affinity) const;

  /**
   * @brief Maps a processing boundary to the logical text.
   *
   * @param[in] projectedBoundary The processing boundary.
   * @return The logical boundary.
   */
  CharacterIndex ProjectedBoundaryToLogical(CharacterIndex projectedBoundary) const;

  /**
   * @brief Finds a replacement containing a logical character.
   *
   * @param[in] logicalIndex The logical character index.
   * @return The replacement run, or nullptr if none exists.
   */
  const ProjectedReplacementRun* FindByLogicalCharacter(CharacterIndex logicalIndex) const;

  /**
   * @brief Finds a replacement at a processing character.
   *
   * @param[in] projectedIndex The processing character index.
   * @return The replacement run, or nullptr if none exists.
   */
  const ProjectedReplacementRun* FindByProjectedCharacter(CharacterIndex projectedIndex) const;

  /**
   * @brief Gets the logical boundary at a position inside a replacement box.
   *
   * @param[in] projectedIndex The replacement index in the projected text.
   * @param[in] xWithinBox The horizontal position inside the replacement box.
   * @param[in] boxWidth The width of the replacement box.
   * @param[in] visualRightToLeft Whether the box is laid out right-to-left.
   * @return The corresponding boundary in the logical source text.
   */
  CharacterIndex HitTestLogicalBoundary(CharacterIndex projectedIndex, float xWithinBox,
                                        float boxWidth, bool visualRightToLeft) const;

  /**
   * @brief Gets the atomic logical range affected by a deletion operation.
   *
   * @param[in] logicalBoundary The deletion boundary in the logical source text.
   * @param[in] backward true for Backspace, or false for Delete.
   * @return The minimal logical character range to delete.
   */
  CharacterRun GetDeletionRange(CharacterIndex logicalBoundary, bool backward) const;

  /**
   * @brief Projects glyph style runs while omitting replacement units.
   *
   * The input runs must be sorted and non-overlapping. If no replacements are
   * active, the output remains unchanged.
   *
   * @param[in] logicalRuns The glyph style runs in the logical source domain.
   * @param[out] projectedSegments The projected ordinary-text style segments.
   * @return true if the style runs were projected.
   */
  bool ProjectGlyphStyleRuns(const Vector<CharacterRun>&    logicalRuns,
                             Vector<ProjectedStyleSegment>& projectedSegments) const;

private:
  const ProjectedReplacementRun* FindRunEndingAt(CharacterIndex logicalBoundary) const;
  const ProjectedReplacementRun* FindRunStartingAt(CharacterIndex logicalBoundary) const;

private:
  const Vector<Character>*        mLogicalText{nullptr};
  Mode                            mMode{Mode::NONE};
  Vector<Character>               mProcessingText;
  Vector<CharacterIndex>          mLogicalToProjectedCharacters;
  Vector<CharacterIndex>          mProjectedToLogicalCharacters;
  Vector<ProjectedReplacementRun> mReplacementRuns;
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_PROJECTION_H

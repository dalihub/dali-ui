#pragma once

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
#include <dali/public-api/common/dali-vector.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/color-run.h>
#include <dali-ui-foundation/internal/text/font-description-run.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/strikethrough-character-run.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-style-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/underlined-character-run.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
class LogicalModel;
}

namespace Internal
{
namespace Text
{

using StyledTextColorRunSnapshot       = Dali::Ui::Text::Internal::StyledTextColorRunSnapshot;
using StyledTextAnchorRunSnapshot      = Dali::Ui::Text::Internal::StyledTextAnchorRunSnapshot;
using StyledTextFontRunSnapshot        = Dali::Ui::Text::Internal::StyledTextFontRunSnapshot;
using StyledTextGradientRunSnapshot    = Dali::Ui::Text::Internal::StyledTextGradientRunSnapshot;
using StyledTextLineThroughRunSnapshot = Dali::Ui::Text::Internal::StyledTextLineThroughRunSnapshot;
using StyledTextStyleRunSnapshot       = Dali::Ui::Text::Internal::StyledTextStyleRunSnapshot;
using StyledTextUnderlineRunSnapshot   = Dali::Ui::Text::Internal::StyledTextUnderlineRunSnapshot;

struct StyledTextApplyResult
{
  /**
   * @brief Creates an empty result.
   */
  StyledTextApplyResult();

  /**
   * @brief Releases owned FontDescriptionRun family buffers.
   */
  ~StyledTextApplyResult();

  StyledTextApplyResult(const StyledTextApplyResult&)            = delete;
  StyledTextApplyResult& operator=(const StyledTextApplyResult&) = delete;

  StyledTextApplyResult(StyledTextApplyResult&& rhs) noexcept;
  StyledTextApplyResult& operator=(StyledTextApplyResult&& rhs) noexcept;

  /**
   * @brief Releases owned FontDescriptionRun family buffers and clears the runs.
   */
  void ReleaseFontDescriptionRuns();

  /**
   * @brief Transfers FontDescriptionRun ownership to the target vector.
   *
   * The target vector must not contain owned familyName buffers when called.
   *
   * @param[out] target The target vector that receives the font runs
   */
  void TransferFontDescriptionRunsTo(Dali::Vector<Dali::Ui::Text::FontDescriptionRun>& target);

  Dali::Vector<Dali::Ui::Text::Character> text;
  Dali::Vector<Dali::Ui::Text::ColorRun>  foregroundColorRuns;
  Dali::Vector<Dali::Ui::Text::ColorRun>  backgroundColorRuns;
  Dali::Vector<Dali::Ui::Text::FontDescriptionRun>
    fontDescriptionRuns;
  Dali::Vector<Dali::Ui::Text::UnderlinedCharacterRun>
    underlinedCharacterRuns;
  Dali::Vector<Dali::Ui::Text::StrikethroughCharacterRun>
    strikethroughCharacterRuns;
};

/**
 * @brief Internal helper for Phase 2/3 StyledText normalization.
 *
 * This helper normalizes plain text, foreground color and gradient fill,
 * background, font, decoration, and anchor span payloads.
 */
class StyledTextApplier
{
public:
  /**
   * @brief Checks whether a StyledText snapshot contains AnchorSpan payloads.
   *
   * @param[in] styledText The styled text snapshot to inspect
   * @return true if one or more AnchorSpan payloads are attached
   */
  static bool HasAnchorSpans(const Dali::Ui::Text::StyledText& styledText);

  /**
   * @brief Builds copy-safe style run data from a StyledText snapshot.
   *
   * Same-category runs follow SpanAttachment insertionOrder so downstream
   * style resolution keeps the authored order without merging spans.
   *
   * The returned snapshot does not contain text. Callers keep text as the
   * transport/source-of-truth and pass it to ApplySnapshotToLogicalModel().
   *
   * @param[in] styledText The styled text snapshot to normalize
   * @param[in] dpi Valid positive DPI used for pixel-to-point font size conversion
   * @param[in] anchorColor Fallback color for anchors without explicit color
   * @param[in] anchorClickedColor Fallback clicked color for anchors without explicit clicked color
   * @param[in] includeAnchorSpans Whether AnchorSpan payloads should be normalized
   * @return The copy-safe style run snapshot
   */
  static StyledTextStyleRunSnapshot BuildTextStyleRunSnapshot(const Dali::Ui::Text::StyledText& styledText,
                                                              float                             dpi,
                                                              const Vector4&                    anchorColor        = Color::MEDIUM_BLUE,
                                                              const Vector4&                    anchorClickedColor = Color::DARK_MAGENTA,
                                                              bool                              includeAnchorSpans = true);

  /**
   * @brief Extracts copy-safe ReplacementSpan values without retaining public handles.
   *
   * Invalid ImageSpan payloads are retained so the common projection validator
   * safely preserves their underlying text.
   */
  static Dali::Ui::Text::ReplacementSourceSnapshot BuildReplacementSourceSnapshot(
    const Dali::Ui::Text::StyledText& styledText,
    uint64_t                          sourceRevision);

  /**
   * @brief Applies plain UTF-8 text and supported style run snapshot data.
   *
   * Existing font description runs are cleared because family name buffers are
   * owned by LogicalModel. FontDescriptionRun::familyName buffers are deep
   * copied from the snapshot into LogicalModel.
   *
   * @param[in] snapshot The copy-safe style run snapshot
   * @param[in] utf8Text The plain text encoded in UTF-8
   * @param[in,out] logicalModel The logical model to receive normalized data
   */
  static void ApplySnapshotToLogicalModel(const StyledTextStyleRunSnapshot& snapshot, const std::string& utf8Text, Dali::Ui::Text::LogicalModel& logicalModel);

  /**
   * @brief Builds the normalized text and style run result.
   *
   * Compatibility helper for existing tests/callers. Long term canonical style
   * normalization should use BuildTextStyleRunSnapshot().
   * This result does not transport anchor metadata. Anchor metadata is applied
   * by ApplySnapshotToLogicalModel().
   *
   * Same-category runs follow SpanAttachment insertionOrder so downstream
   * style resolution keeps the authored order without merging spans.
   * The returned result owns FontDescriptionRun::familyName buffers until they
   * are explicitly transferred.
   *
   * @param[in] styledText The styled text snapshot to normalize
   * @param[in] dpi Valid positive DPI used for pixel-to-point font size conversion
   * @param[in] anchorColor Fallback color for anchors without explicit color
   * @param[in] anchorClickedColor Fallback clicked color for anchors without explicit clicked color
   * @param[in] includeAnchorSpans Whether AnchorSpan payloads should be normalized
   * @return The normalized text and style run data
   */
  static StyledTextApplyResult BuildTextStyleRunResult(const Dali::Ui::Text::StyledText& styledText,
                                                       float                             dpi,
                                                       const Vector4&                    anchorColor        = Color::MEDIUM_BLUE,
                                                       const Vector4&                    anchorClickedColor = Color::DARK_MAGENTA,
                                                       bool                              includeAnchorSpans = true);

  /**
   * @brief Applies text and supported style run data to the given LogicalModel.
   *
   * This method intentionally leaves unsupported LogicalModel style runs untouched.
   * Existing font description runs are cleared because family name buffers are
   * owned by LogicalModel. FontDescriptionRun::familyName ownership is
   * transferred from the temporary result to LogicalModel.
   *
   * @param[in] styledText The styled text snapshot to apply
   * @param[in,out] logicalModel The logical model to receive normalized data
   * @param[in] dpi Valid positive DPI used for pixel-to-point font size conversion
   * @param[in] anchorColor Fallback color for anchors without explicit color
   * @param[in] anchorClickedColor Fallback clicked color for anchors without explicit clicked color
   * @param[in] includeAnchorSpans Whether AnchorSpan payloads should be normalized
   */
  static void ApplyTextAndStyleRunsToLogicalModel(const Dali::Ui::Text::StyledText& styledText,
                                                  Dali::Ui::Text::LogicalModel&     logicalModel,
                                                  float                             dpi,
                                                  const Vector4&                    anchorColor        = Color::MEDIUM_BLUE,
                                                  const Vector4&                    anchorClickedColor = Color::DARK_MAGENTA,
                                                  bool                              includeAnchorSpans = true);
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali

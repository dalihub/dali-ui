#ifndef DALI_UI_TEXT_VIEW_H
#define DALI_UI_TEXT_VIEW_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/bounded-paragraph-run.h>
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/text-view-interface.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief View provides an interface between the Text layout engine and rendering back-end.
 */
class View : public ViewInterface
{
public:
  /**
   * @brief Create a new instance of a View.
   */
  View();

  /**
   * @brief Virtual destructor.
   */
  virtual ~View();

  /**
   * @brief Set the visual model.
   *
   * @param[in] visualModel The visual model used by the View.
   */
  void SetVisualModel(VisualModelPtr visualModel);

  /**
   * @brief Set the logical model.
   *
   * @param[in] logicalModel The logical model used by the View.
   */
  void SetLogicalModel(LogicalModelPtr logicalModel);

  /**
   * @brief Binds the final result owned by the active render model.
   *
   * A null pointer preserves the START/MIDDLE fallback and standalone
   * callers that do not publish an authoritative final result.
   *
   * @param[in] result The resolved final sequence, or nullptr to disable it.
   */
  void SetFinalElisionResult(const FinalElisionResult* result);

  /**
   * @brief Resolves the final glyph sequence for a completed layout generation.
   *
   * Repeated calls for the same generation reuse the resolved result.
   *
   * @param[in] fontClient The FontClient owned by the current text pipeline.
   * @param[in,out] result The final glyph and source-mapping result.
   * @param[in] layoutGeneration The generation of the completed layout pass.
   */
  void ResolveFinalElision(TextAbstraction::FontClient& fontClient,
                           FinalElisionResult&          result,
                           uint64_t                     layoutGeneration) const;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetControlSize()
   */
  const Vector2& GetControlSize() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetLayoutSize()
   */
  const Vector2& GetLayoutSize() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetNumberOfGlyphs()
   */
  Length GetNumberOfGlyphs() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetGlyphs()
   */
  virtual Length GetGlyphs(GlyphInfo* glyphs, Vector2* glyphPositions, float& minLineOffset, GlyphIndex glyphIndex,
                           Length numberOfGlyphs) const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetFinalGlyphStyleSourceIndices()
   */
  const GlyphIndex* GetFinalGlyphStyleSourceIndices() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetColors()
   */
  const Vector4* GetColors() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetColorIndices()
   */
  const ColorIndex* GetColorIndices() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetBackgroundColors()
   */
  const Vector4* GetBackgroundColors() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetBackgroundColorIndices()
   */
  const ColorIndex* GetBackgroundColorIndices() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsMarkupBackgroundColorSet()
   */
  bool IsMarkupBackgroundColorSet() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetTextColor()
   */
  const Vector4& GetTextColor() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetShadowOffset()
   */
  const Vector2& GetShadowOffset() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsShadowEnabled()
   */
  bool IsShadowEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetShadowColor()
   */
  const Vector4& GetShadowColor() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetUnderlineColor()
   */
  const Vector4& GetUnderlineColor() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsUnderlineEnabled()
   */
  bool IsUnderlineEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsMarkupUnderlineSet()
   */
  bool IsMarkupUnderlineSet() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetHyphens()
   */
  const GlyphInfo* GetHyphens() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetHyphens()
   */
  const Length* GetHyphenIndices() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetHyphens()
   */
  Length GetHyphensCount() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetUnderlineHeight()
   */
  float GetUnderlineHeight() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetUnderlineType()
   */
  Text::Underline::Type GetUnderlineType() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetDashedUnderlineWidth()
   */
  float GetDashedUnderlineWidth() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetDashedUnderlineGap()
   */
  float GetDashedUnderlineGap() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetNumberOfUnderlineRuns()
   */
  Length GetNumberOfUnderlineRuns() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetUnderlineRuns()
   */
  virtual void GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetOutlineOffset()
   */
  const Vector2& GetOutlineOffset() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetOutlineColor()
   */
  const Vector4& GetOutlineColor() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetOutlineWidth()
   */
  uint16_t GetOutlineWidth() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsOutlineEnabled()
   */
  bool IsOutlineEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetEllipsisPosition()
   */
  Text::EllipsisPosition::Type GetEllipsisPosition() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetStartIndexOfElidedGlyphs()
   */
  GlyphIndex GetStartIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetEndIndexOfElidedGlyphs()
   */
  GlyphIndex GetEndIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetFirstMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetSecondMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetStrikethroughColor()
   */
  const Vector4& GetStrikethroughColor() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsStrikethroughEnabled()
   */
  bool IsStrikethroughEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsMarkupStrikethroughSet()
   */
  bool IsMarkupStrikethroughSet() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetStrikethroughHeight()
   */
  float GetStrikethroughHeight() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetNumberOfStrikethroughRuns()
   */
  Length GetNumberOfStrikethroughRuns() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetStrikethroughRuns()
   */
  void GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index,
                            Length numberOfRuns) const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetNumberOfBoundedParagraphRuns()
   */
  virtual Length GetNumberOfBoundedParagraphRuns() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetBoundedParagraphRuns()
   */
  virtual const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetCharacterSpacing()
   */
  float GetCharacterSpacing() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetTextBuffer()
   */
  const Character* GetTextBuffer() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetGlyphsToCharacters()
   */
  const Vector<CharacterIndex>& GetGlyphsToCharacters() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::IsCutoutEnabled()
   */
  bool IsCutoutEnabled() const override;

  /**
   * @copydoc Dali::Ui::Text::ViewInterface::GetVerticalLineAlignment()
   */
  Alignment GetVerticalLineAlignment() const override;

private:
  Length GetGlyphsUncached(GlyphInfo*                   glyphs,
                           Vector2*                     glyphPositions,
                           TextAbstraction::FontClient* fontClient,
                           float&                       minLineOffset,
                           GlyphIndex                   glyphIndex,
                           Length                       numberOfGlyphs,
                           GlyphIndex*                  sourceGlyphIndices      = nullptr,
                           GlyphIndex*                  ellipsisFinalGlyphIndex = nullptr,
                           bool                         hasActiveReplacement    = false) const;

  // Undefined
  View(const View& handle);

  // Undefined
  View& operator=(const View& handle);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_VIEW_H

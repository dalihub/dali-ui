#ifndef DALI_UI_TEXT_VIEW_MODEL_H
#define DALI_UI_TEXT_VIEW_MODEL_H

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
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/bounded-paragraph-run.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
struct FinalElisionResult;

/**
 * @brief Responsible of creating and store temporary modifications of the text model.
 * i.e. The elide of text.
 */
class ViewModel : public ModelInterface
{
public:
  /**
   * @brief Constructor.
   *
   * Keeps the pointer to the text's model. It initializes all the members of the class to their defaults.
   *
   * @param[in] model Pointer to the text's model interface.
   */
  ViewModel(const ModelInterface* const model);

  /**
   * @brief Sets the model used to retrieve render data.
   *
   * Transient elision state associated with the previous model is cleared.
   *
   * @param[in] model Pointer to the text's data model.
   */
  void SetModel(const ModelInterface* model);

  /**
   * @brief Sets the resolved replacement glyph sequence.
   *
   * The result is not owned by ViewModel and must remain valid until it is
   * replaced or the model is reset.
   *
   * @param[in] result The replacement result.
   */
  void SetFinalElisionResult(const FinalElisionResult* result);

  /**
   * @brief Gets the final-glyph-domain style source buffer.
   *
   * @return The buffer, or nullptr when no resolved final sequence is active.
   */
  const GlyphIndex* GetFinalGlyphStyleSourceIndices() const;

  /**
   * @brief Virtual destructor.
   *
   * It's a default destructor.
   */
  virtual ~ViewModel();

  /**
   * @copydoc ModelInterface::GetControlSize()
   */
  const Size& GetControlSize() const override;

  /**
   * @copydoc ModelInterface::GetLayoutSize()
   */
  const Size& GetLayoutSize() const override;

  /**
   * @copydoc ModelInterface::GetScrollPosition()
   */
  const Vector2& GetScrollPosition() const override;

  /**
   * @copydoc ModelInterface::GetHorizontalAlignment()
   */
  Alignment GetHorizontalAlignment() const override;

  /**
   * @copydoc ModelInterface::GetVerticalAlignment()
   */
  Alignment GetVerticalAlignment() const override;

  /**
   * @copydoc ModelInterface::GetVerticalLineAlignment()
   */
  Alignment GetVerticalLineAlignment() const override;

  /**
   * @copydoc ModelInterface::GetEllipsisPosition()
   */
  Text::EllipsisPosition::Type GetEllipsisPosition() const override;

  /**
   * @copydoc ModelInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfLines()
   */
  Length GetNumberOfLines() const override;

  /**
   * @copydoc ModelInterface::GetLines()
   */
  const LineRun* GetLines() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfScripts()
   */
  Length GetNumberOfScripts() const override;

  /**
   * @copydoc ModelInterface::GetScriptRuns()
   */
  const ScriptRun* GetScriptRuns() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfCharacters()
   */
  Length GetNumberOfCharacters() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfGlyphs()
   */
  Length GetNumberOfGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetStartIndexOfElidedGlyphs()
   */
  GlyphIndex GetStartIndexOfElidedGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetEndIndexOfElidedGlyphs()
   */
  GlyphIndex GetEndIndexOfElidedGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetFirstMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetSecondMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetGlyphs()
   */
  const GlyphInfo* GetGlyphs() const override;

  /**
   * @copydoc ModelInterface::GetLayout()
   */
  const Vector2* GetLayout() const override;

  /**
   * @copydoc ModelInterface::GetColors()
   */
  const Vector4* GetColors() const override;

  /**
   * @copydoc ModelInterface::GetColorIndices()
   */
  const ColorIndex* GetColorIndices() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColors()
   */
  const Vector4* GetBackgroundColors() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColorIndices()
   */
  const ColorIndex* GetBackgroundColorIndices() const override;

  /**
   * @copydoc ModelInterface::IsMarkupBackgroundColorSet()
   */
  bool IsMarkupBackgroundColorSet() const override;

  /**
   * @copydoc ModelInterface::GetDefaultColor()
   */
  const Vector4& GetDefaultColor() const override;

  /**
   * @copydoc ModelInterface::GetShadowOffset()
   */
  const Vector2& GetShadowOffset() const override;

  /**
   * @copydoc ModelInterface::IsShadowEnabled()
   */
  bool IsShadowEnabled() const override;

  /**
   * @copydoc ModelInterface::GetShadowColor()
   */
  const Vector4& GetShadowColor() const override;

  /**
   * @copydoc ModelInterface::GetShadowBlurRadius()
   */
  const float& GetShadowBlurRadius() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineColor()
   */
  const Vector4& GetUnderlineColor() const override;

  /**
   * @copydoc ModelInterface::IsUnderlineEnabled()
   */
  bool IsUnderlineEnabled() const override;

  /**
   * @copydoc ModelInterface::IsMarkupUnderlineSet()
   */
  bool IsMarkupUnderlineSet() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineHeight()
   */
  float GetUnderlineHeight() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineType()
   */
  Text::Underline::Type GetUnderlineType() const override;

  /**
   * @copydoc ModelInterface::GetDashedUnderlineWidth()
   */
  float GetDashedUnderlineWidth() const override;

  /**
   * @copydoc ModelInterface::GetDashedUnderlineGap()
   */
  float GetDashedUnderlineGap() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfUnderlineRuns()
   */
  Length GetNumberOfUnderlineRuns() const override;

  /**
   * @copydoc ModelInterface::GetUnderlineRuns()
   */
  void GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const override;

  /**
   * @copydoc ModelInterface::GetOutlineOffset()
   */
  const Vector2& GetOutlineOffset() const override;

  /**
   * @copydoc ModelInterface::GetOutlineColor()
   */
  const Vector4& GetOutlineColor() const override;

  /**
   * @copydoc ModelInterface::GetOutlineWidth()
   */
  uint16_t GetOutlineWidth() const override;

  /**
   * @copydoc ModelInterface::IsOutlineEnabled()
   */
  bool IsOutlineEnabled() const override;

  /**
   * @copydoc ModelInterface::GetOutlineBlurRadius()
   */
  const float& GetOutlineBlurRadius() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColor()
   */
  const Vector4& GetBackgroundColor() const override;

  /**
   * @copydoc ModelInterface::IsBackgroundEnabled()
   */
  bool IsBackgroundEnabled() const override;

  /**
   * @copydoc ModelInterface::GetHyphens()
   */
  const GlyphInfo* GetHyphens() const override;

  /**
   * @copydoc ModelInterface::GetHyphens()
   */
  const Length* GetHyphenIndices() const override;

  /**
   * @copydoc ModelInterface::GetHyphens()
   */
  Length GetHyphensCount() const override;

  /**
   * @copydoc ModelInterface::GetCharacterSpacing()
   */
  float GetCharacterSpacing() const override;

  /**
   * @copydoc ModelInterface::GetTextBuffer()
   */
  const Character* GetTextBuffer() const override;

  /**
   * @copydoc ModelInterface::GetGlyphsToCharacters()
   */
  const Vector<CharacterIndex>& GetGlyphsToCharacters() const override;

  /**
   * @brief Does the text elide at the end, start or middle of text according to ellipsis position
   *
   * It stores a copy of the visible glyphs and removes as many glyphs as needed
   * from the last visible line to add the ellipsis glyph in END case,
   * from the first visible line to add the ellipsis glyph in START case,
   * between the first and last visible lines to add the ellipsis glyph.
   *
   * It stores as well a copy of the positions for each visible glyph.
   *
   * @param[in] fontClient FontClient to use in this function to obtain information about the ellipsis glyph.
   */
  void ElideGlyphs(TextAbstraction::FontClient& fontClient);

  /**
   * @brief Enables final-to-source glyph mapping for subsequent elision.
   *
   * Call this before ElideGlyphs() when a consumer needs to project source
   * semantics onto final elision output. The request is consumed by that call;
   * keeping it one-shot avoids allocating mapping storage on later ordinary
   * text updates.
   */
  void EnableFinalGlyphMapping();

  /**
   * @brief Returns the final-to-source mapping for the current elided sequence.
   *
   * The returned pointer remains valid until the model, final result, or elision
   * state changes. A nullptr indicates identity mapping.
   *
   * @return The final-to-source glyph mapping, or nullptr for identity mapping.
   */
  const GlyphIndex* GetFinalToSourceGlyphIndices() const;

  /**
   * @brief Returns the ellipsis index in the final rendered sequence.
   *
   * @return The final ellipsis glyph index, or INVALID_GLYPH_INDEX when absent.
   */
  GlyphIndex GetEllipsisFinalGlyphIndex() const;

  /**
   * @brief The horizontal offset applied ellipsis in ElideGlyphs. (horizontal align applied)
   *
   * @return horizontal offset of elided text.
   */
  const float GetElidedOffset() const;

  /**
   * @brief Gets the character's language direction.
   * @param[in] logicalIndex The logical index of the character.
   * @return true if RTL, else LTR.
   */
  const bool GetCharacterDirection(CharacterIndex logicalIndex) const;

  /**
   * @copydoc ModelInterface::GetStrikethroughHeight()
   */
  float GetStrikethroughHeight() const override;

  /**
   * @copydoc ModelInterface::GetStrikethroughColor()
   */
  const Vector4& GetStrikethroughColor() const override;

  /**
   * @copydoc ModelInterface::IsStrikethroughEnabled()
   */
  bool IsStrikethroughEnabled() const override;

  /**
   * @copydoc ModelInterface::IsMarkupStrikethroughSet()
   */
  bool IsMarkupStrikethroughSet() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfStrikethroughRuns()
   */
  Length GetNumberOfStrikethroughRuns() const override;

  /**
   * @copydoc ModelInterface::GetNumberOfBoundedParagraphRuns()
   */
  virtual Length GetNumberOfBoundedParagraphRuns() const override;

  /**
   * @copydoc ModelInterface::GetBoundedParagraphRuns()
   */
  virtual const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const override;

  /**
   * @copydoc ModelInterface::GetStrikethroughRuns()
   */
  void GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index,
                            Length numberOfRuns) const override;

  /**
   * @copydoc ModelInterface::GetNumberOfCharacterSpacingGlyphRuns()
   */
  Length GetNumberOfCharacterSpacingGlyphRuns() const override;

  /**
   * @copydoc ModelInterface::GetCharacterSpacingGlyphRuns()
   */
  const Vector<CharacterSpacingGlyphRun>& GetCharacterSpacingGlyphRuns() const override;

  /**
   * @copydoc ModelInterface::GetFontRuns()
   */
  const Vector<FontRun>& GetFontRuns() const override;

  /**
   * @copydoc ModelInterface::GetFontDescriptionRuns()
   */
  const Vector<FontDescriptionRun>& GetFontDescriptionRuns() const override;

  /**
   * @copydoc ModelInterface::IsRemoveFrontInset()
   */
  bool IsRemoveFrontInset() const override;

  /**
   * @copydoc ModelInterface::IsRemoveBackInset()
   */
  bool IsRemoveBackInset() const override;

  /**
   * @copydoc ModelInterface::IsCutoutEnabled()
   */
  bool IsCutoutEnabled() const override;

  /**
   * @copydoc ModelInterface::IsBackgroundWithCutoutEnabled()
   */
  const bool IsBackgroundWithCutoutEnabled() const override;

  /**
   * @copydoc ModelInterface::GetBackgroundColorWithCutout()
   */
  const Vector4& GetBackgroundColorWithCutout() const override;

  /**
   * @copydoc ModelInterface::GetOffsetWithCutout()
   */
  const Vector2& GetOffsetWithCutout() const override;

  /**
   * @copydoc ModelInterface::GetCharacterDirections()
   */
  const Vector<CharacterDirection>& GetCharacterDirections() const override;

private:
  const ModelInterface*     mModel;                           ///< Pointer to the current authoritative render model.
  const FinalElisionResult* mFinalElisionResult;              ///< Non-owning authoritative final glyph sequence.
  Vector<GlyphInfo>         mElidedGlyphs;                    ///< Fallback storage for unresolved ModelInterface implementations.
  Vector<Vector2>           mElidedLayout;                    ///< Fallback positions for unresolved ModelInterface implementations.
  Vector<GlyphIndex>        mElidedFinalToSourceGlyphIndices; ///< Allocated only when reveal projection is requested.
  GlyphIndex                mEllipsisFinalGlyphIndex;
  bool                      mFinalGlyphMappingRequested : 1;
  bool                      mIsTextElided : 1; ///< Whether the text has been elided.
  float                     mElidedOffset;     ///< The width of the (control - elided line). This is required for calculating the correct
                                               ///< horizontal align offset.
  GlyphIndex mStartIndexOfElidedGlyphs;        ///< The start index of elided glyphs.
  GlyphIndex mEndIndexOfElidedGlyphs;          ///< The end index of elided glyphs.
  GlyphIndex
             mFirstMiddleIndexOfElidedGlyphs;  ///< The first end index of elided glyphs, index before ellipsis of middle.
  GlyphIndex mSecondMiddleIndexOfElidedGlyphs; ///< The second end index of elided glyphs, index of ellipsis of middle.
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_VIEW_MODEL_H

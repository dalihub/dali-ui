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
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/text-view-interface.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>

namespace Dali
{
namespace UI
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
   * @copydoc Dali::UI::Text::ViewInterface::GetControlSize()
   */
  const Vector2& GetControlSize() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetLayoutSize()
   */
  const Vector2& GetLayoutSize() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetNumberOfGlyphs()
   */
  Length GetNumberOfGlyphs() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetGlyphs()
   */
  virtual Length GetGlyphs(GlyphInfo* glyphs, Vector2* glyphPositions, float& minLineOffset, GlyphIndex glyphIndex,
                           Length numberOfGlyphs) const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetColors()
   */
  const Vector4* GetColors() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetColorIndices()
   */
  const ColorIndex* GetColorIndices() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetBackgroundColors()
   */
  const Vector4* GetBackgroundColors() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetBackgroundColorIndices()
   */
  const ColorIndex* GetBackgroundColorIndices() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsMarkupBackgroundColorSet()
   */
  bool IsMarkupBackgroundColorSet() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetTextColor()
   */
  const Vector4& GetTextColor() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetShadowOffset()
   */
  const Vector2& GetShadowOffset() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetShadowColor()
   */
  const Vector4& GetShadowColor() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetUnderlineColor()
   */
  const Vector4& GetUnderlineColor() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsUnderlineEnabled()
   */
  bool IsUnderlineEnabled() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsMarkupUnderlineSet()
   */
  bool IsMarkupUnderlineSet() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetHyphens()
   */
  const GlyphInfo* GetHyphens() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetHyphens()
   */
  const Length* GetHyphenIndices() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetHyphens()
   */
  Length GetHyphensCount() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetUnderlineHeight()
   */
  float GetUnderlineHeight() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetUnderlineType()
   */
  Text::Underline::Type GetUnderlineType() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetDashedUnderlineWidth()
   */
  float GetDashedUnderlineWidth() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetDashedUnderlineGap()
   */
  float GetDashedUnderlineGap() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetNumberOfUnderlineRuns()
   */
  Length GetNumberOfUnderlineRuns() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetUnderlineRuns()
   */
  virtual void GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetOutlineOffset()
   */
  const Vector2& GetOutlineOffset() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetOutlineColor()
   */
  const Vector4& GetOutlineColor() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetOutlineWidth()
   */
  uint16_t GetOutlineWidth() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetEllipsisPosition()
   */
  DevelText::EllipsisPosition::Type GetEllipsisPosition() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsTextElideEnabled()
   */
  bool IsTextElideEnabled() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetStartIndexOfElidedGlyphs()
   */
  GlyphIndex GetStartIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetEndIndexOfElidedGlyphs()
   */
  GlyphIndex GetEndIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetFirstMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetSecondMiddleIndexOfElidedGlyphs()
   */
  GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetStrikethroughColor()
   */
  const Vector4& GetStrikethroughColor() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsStrikethroughEnabled()
   */
  bool IsStrikethroughEnabled() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsMarkupStrikethroughSet()
   */
  bool IsMarkupStrikethroughSet() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetStrikethroughHeight()
   */
  float GetStrikethroughHeight() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetNumberOfStrikethroughRuns()
   */
  Length GetNumberOfStrikethroughRuns() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetStrikethroughRuns()
   */
  void GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index,
                            Length numberOfRuns) const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetNumberOfBoundedParagraphRuns()
   */
  virtual Length GetNumberOfBoundedParagraphRuns() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetBoundedParagraphRuns()
   */
  virtual const Vector<BoundedParagraphRun>& GetBoundedParagraphRuns() const;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetCharacterSpacing()
   */
  float GetCharacterSpacing() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetTextBuffer()
   */
  const Character* GetTextBuffer() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::GetGlyphsToCharacters()
   */
  const Vector<CharacterIndex>& GetGlyphsToCharacters() const override;

  /**
   * @copydoc Dali::UI::Text::ViewInterface::IsCutoutEnabled()
   */
  bool IsCutoutEnabled() const override;

private:
  // Undefined
  View(const View& handle);

  // Undefined
  View& operator=(const View& handle);

private:
  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace UI

} // namespace Dali

#endif // DALI_UI_TEXT_VIEW_H

#ifndef DALI_UI_TEXT_VISUALIZER_VIEW_INTERFACE_H
#define DALI_UI_TEXT_VISUALIZER_VIEW_INTERFACE_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-view-interface.h>

namespace Dali::Ui::Internal::TextVisualizer
{
class AtlasViewAdapter;

/**
 * @brief Minimal Text::ViewInterface adapter for TextVisualizer.
 *
 * This class intentionally exposes only the data already prepared by TextVisualizer
 * and returns safe empty values for unsupported style and editor-related features.
 */
class TextVisualizerViewInterface : public Text::ViewInterface
{
public:
  struct Diagnostics
  {
    uint32_t getGlyphsCallCount{0u};
    uint32_t lastRequestedGlyphCount{0u};
    uint32_t lastReturnedGlyphCount{0u};
    uint32_t lastGlyphStartIndex{0u};
  };

  TextVisualizerViewInterface();
  ~TextVisualizerViewInterface() override;

  void SetAdapter(const AtlasViewAdapter* adapter);
  void Clear();
  void ResetDiagnostics() const;

  bool               HasAdapter() const;
  const Diagnostics& GetDiagnostics() const;

  const Vector2&                           GetControlSize() const override;
  const Vector2&                           GetLayoutSize() const override;
  Text::Length                             GetNumberOfGlyphs() const override;
  Text::Length                             GetGlyphs(Text::GlyphInfo* glyphs, Vector2* glyphPositions, float& minLineOffset, Text::GlyphIndex glyphIndex,
                                                     Text::Length numberOfGlyphs) const override;
  const Vector4*                           GetColors() const override;
  const Text::ColorIndex*                  GetColorIndices() const override;
  const Vector4*                           GetBackgroundColors() const override;
  const Text::ColorIndex*                  GetBackgroundColorIndices() const override;
  bool                                     IsMarkupBackgroundColorSet() const override;
  const Vector4&                           GetTextColor() const override;
  const Vector2&                           GetShadowOffset() const override;
  const Vector4&                           GetShadowColor() const override;
  const Vector4&                           GetUnderlineColor() const override;
  bool                                     IsUnderlineEnabled() const override;
  bool                                     IsMarkupUnderlineSet() const override;
  const Text::GlyphInfo*                   GetHyphens() const override;
  const Text::Length*                      GetHyphenIndices() const override;
  Text::Length                             GetHyphensCount() const override;
  float                                    GetUnderlineHeight() const override;
  Text::Underline::Type                    GetUnderlineType() const override;
  float                                    GetDashedUnderlineWidth() const override;
  float                                    GetDashedUnderlineGap() const override;
  Text::Length                             GetNumberOfUnderlineRuns() const override;
  void                                     GetUnderlineRuns(Text::UnderlinedGlyphRun* underlineRuns, Text::UnderlineRunIndex index,
                                                            Text::Length numberOfRuns) const override;
  const Vector2&                           GetOutlineOffset() const override;
  const Vector4&                           GetOutlineColor() const override;
  uint16_t                                 GetOutlineWidth() const override;
  Text::EllipsisPosition::Type             GetEllipsisPosition() const override;
  bool                                     IsTextElideEnabled() const override;
  Text::GlyphIndex                         GetStartIndexOfElidedGlyphs() const override;
  Text::GlyphIndex                         GetEndIndexOfElidedGlyphs() const override;
  Text::GlyphIndex                         GetFirstMiddleIndexOfElidedGlyphs() const override;
  Text::GlyphIndex                         GetSecondMiddleIndexOfElidedGlyphs() const override;
  const Vector4&                           GetStrikethroughColor() const override;
  bool                                     IsStrikethroughEnabled() const override;
  bool                                     IsMarkupStrikethroughSet() const override;
  float                                    GetStrikethroughHeight() const override;
  Text::Length                             GetNumberOfStrikethroughRuns() const override;
  Text::Length                             GetNumberOfBoundedParagraphRuns() const override;
  const Vector<Text::BoundedParagraphRun>& GetBoundedParagraphRuns() const override;
  void                                     GetStrikethroughRuns(Text::StrikethroughGlyphRun* strikethroughRuns,
                                                                Text::StrikethroughRunIndex  index,
                                                                Text::Length                 numberOfRuns) const override;
  float                                    GetCharacterSpacing() const override;
  const Text::Character*                   GetTextBuffer() const override;
  const Vector<Text::CharacterIndex>&      GetGlyphsToCharacters() const override;
  bool                                     IsCutoutEnabled() const override;

private:
  const AtlasViewAdapter* mAdapter;
  mutable Diagnostics     mDiagnostics;
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_VIEW_INTERFACE_H

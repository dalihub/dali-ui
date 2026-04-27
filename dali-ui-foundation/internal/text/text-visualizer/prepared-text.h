#ifndef DALI_UI_TEXT_VISUALIZER_PREPARED_TEXT_H
#define DALI_UI_TEXT_VISUALIZER_PREPARED_TEXT_H

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/font-run.h>
#include <dali-ui-foundation/internal/text/paragraph-run.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali::Ui::Internal::TextVisualizer
{
/**
 * @brief Prepared text cache for TextVisualizer.
 *
 * This stores stable prepare-stage data up to shaping and basic glyph mapping.
 * Actual glyph metrics and layout placement are connected later.
 */
class PreparedText
{
public:
  struct LineMetrics
  {
    float ascender{0.0f};
    float descender{0.0f};
    float lineGap{0.0f};
    float naturalLineHeight{0.0f};
    float baselineOffset{0.0f};
  };

  struct GlyphLayoutData
  {
    Dali::Vector<float>    advances;
    Dali::Vector<float>    widths;
    Dali::Vector<float>    prefixAdvances;
    Dali::Vector<uint32_t> characterStarts;
    Dali::Vector<uint32_t> characterEnds;
    Dali::Vector<uint8_t>  breakAllowedAfterGlyph;
    Dali::Vector<uint8_t>  breakMandatoryAfterGlyph;
  };

  PreparedText();

  void                SetOriginalText(const Dali::String& text);
  const Dali::String& GetOriginalText() const;

  void                SetFontFamily(const Dali::String& fontFamily);
  const Dali::String& GetFontFamily() const;

  void  SetFontSize(float fontSize);
  float GetFontSize() const;

  void     SetClusterCount(uint32_t clusterCount);
  uint32_t GetClusterCount() const;

  void                                 SetCharacters(const Dali::Vector<Text::Character>& characters);
  const Dali::Vector<Text::Character>& GetCharacters() const;
  uint32_t                             GetCharacterCount() const;

  void                                     SetLineBreakInfo(const Dali::Vector<Text::LineBreakInfo>& lineBreakInfo);
  const Dali::Vector<Text::LineBreakInfo>& GetLineBreakInfo() const;
  uint32_t                                 GetLineBreakCount() const;

  void                                    SetParagraphInfo(const Dali::Vector<Text::ParagraphRun>& paragraphInfo);
  const Dali::Vector<Text::ParagraphRun>& GetParagraphInfo() const;
  uint32_t                                GetParagraphCount() const;

  void                                 SetScriptRuns(const Dali::Vector<Text::ScriptRun>& scriptRuns);
  const Dali::Vector<Text::ScriptRun>& GetScriptRuns() const;
  uint32_t                             GetScriptRunCount() const;

  void                               SetFontRuns(const Dali::Vector<Text::FontRun>& fontRuns);
  const Dali::Vector<Text::FontRun>& GetFontRuns() const;
  uint32_t                           GetFontRunCount() const;

  void                                 SetGlyphs(const Dali::Vector<Text::GlyphInfo>& glyphs);
  const Dali::Vector<Text::GlyphInfo>& GetGlyphs() const;
  uint32_t                             GetGlyphCount() const;

  void                                      SetGlyphToCharacterMap(const Dali::Vector<Text::CharacterIndex>& glyphToCharacterMap);
  const Dali::Vector<Text::CharacterIndex>& GetGlyphToCharacterMap() const;

  void                              SetCharactersPerGlyph(const Dali::Vector<Text::Length>& charactersPerGlyph);
  const Dali::Vector<Text::Length>& GetCharactersPerGlyph() const;

  void                                  SetCharacterToGlyphTable(const Dali::Vector<Text::GlyphIndex>& characterToGlyphTable);
  const Dali::Vector<Text::GlyphIndex>& GetCharacterToGlyphTable() const;

  void                              SetGlyphsPerCharacterTable(const Dali::Vector<Text::Length>& glyphsPerCharacterTable);
  const Dali::Vector<Text::Length>& GetGlyphsPerCharacterTable() const;

  void                                  SetNewParagraphGlyphs(const Dali::Vector<Text::GlyphIndex>& newParagraphGlyphs);
  const Dali::Vector<Text::GlyphIndex>& GetNewParagraphGlyphs() const;

  void                   SetGlyphLayoutData(const GlyphLayoutData& glyphLayoutData);
  const GlyphLayoutData& GetGlyphLayoutData() const;
  bool                   HasGlyphLayoutData() const;
  void                   ClearGlyphLayoutData();

  bool  HasGlyphData() const;
  bool  HasGlyphMetrics() const;
  float GetTotalGlyphAdvance() const;

  void               SetLineMetrics(const LineMetrics& metrics);
  const LineMetrics& GetLineMetrics() const;
  bool               HasLineMetrics() const;

  void SetPrepared(bool prepared);
  bool IsPrepared() const;

  bool Empty() const;
  void Clear();

private:
  Dali::String                       mOriginalText;
  Dali::String                       mFontFamily;
  float                              mFontSize;
  uint32_t                           mClusterCount;
  Dali::Vector<Text::Character>      mCharacters;
  Dali::Vector<Text::LineBreakInfo>  mLineBreakInfo;
  Dali::Vector<Text::ParagraphRun>   mParagraphInfo;
  Dali::Vector<Text::ScriptRun>      mScriptRuns;
  Dali::Vector<Text::FontRun>        mFontRuns;
  Dali::Vector<Text::GlyphInfo>      mGlyphs;
  Dali::Vector<Text::CharacterIndex> mGlyphToCharacterMap;
  Dali::Vector<Text::Length>         mCharactersPerGlyph;
  Dali::Vector<Text::GlyphIndex>     mCharacterToGlyphTable;
  Dali::Vector<Text::Length>         mGlyphsPerCharacterTable;
  Dali::Vector<Text::GlyphIndex>     mNewParagraphGlyphs;
  GlyphLayoutData                    mGlyphLayoutData;
  bool                               mHasGlyphLayoutData;
  LineMetrics                        mLineMetrics;
  bool                               mHasLineMetrics;
  bool                               mPrepared;
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_PREPARED_TEXT_H

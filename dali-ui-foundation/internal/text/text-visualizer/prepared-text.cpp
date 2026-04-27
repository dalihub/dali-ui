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
#include <dali-ui-foundation/internal/text/text-visualizer/prepared-text.h>

namespace Dali::Ui::Internal::TextVisualizer
{

PreparedText::PreparedText()
: mOriginalText(),
  mFontFamily(),
  mFontSize(0.0f),
  mClusterCount(0u),
  mCharacters(),
  mLineBreakInfo(),
  mParagraphInfo(),
  mScriptRuns(),
  mFontRuns(),
  mGlyphs(),
  mGlyphToCharacterMap(),
  mCharactersPerGlyph(),
  mCharacterToGlyphTable(),
  mGlyphsPerCharacterTable(),
  mNewParagraphGlyphs(),
  mGlyphLayoutData(),
  mHasGlyphLayoutData(false),
  mLineMetrics(),
  mHasLineMetrics(false),
  mPrepared(false)
{
}

void PreparedText::SetOriginalText(const Dali::String& text)
{
  mOriginalText = text;
}

const Dali::String& PreparedText::GetOriginalText() const
{
  return mOriginalText;
}

void PreparedText::SetFontFamily(const Dali::String& fontFamily)
{
  mFontFamily = fontFamily;
}

const Dali::String& PreparedText::GetFontFamily() const
{
  return mFontFamily;
}

void PreparedText::SetFontSize(float fontSize)
{
  mFontSize = fontSize;
}

float PreparedText::GetFontSize() const
{
  return mFontSize;
}

void PreparedText::SetClusterCount(uint32_t clusterCount)
{
  mClusterCount = clusterCount;
}

uint32_t PreparedText::GetClusterCount() const
{
  return mClusterCount;
}

void PreparedText::SetCharacters(const Dali::Vector<Text::Character>& characters)
{
  mCharacters = characters;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::Character>& PreparedText::GetCharacters() const
{
  return mCharacters;
}

uint32_t PreparedText::GetCharacterCount() const
{
  return static_cast<uint32_t>(mCharacters.Count());
}

void PreparedText::SetLineBreakInfo(const Dali::Vector<Text::LineBreakInfo>& lineBreakInfo)
{
  mLineBreakInfo = lineBreakInfo;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::LineBreakInfo>& PreparedText::GetLineBreakInfo() const
{
  return mLineBreakInfo;
}

uint32_t PreparedText::GetLineBreakCount() const
{
  return static_cast<uint32_t>(mLineBreakInfo.Count());
}

void PreparedText::SetParagraphInfo(const Dali::Vector<Text::ParagraphRun>& paragraphInfo)
{
  mParagraphInfo = paragraphInfo;
}

const Dali::Vector<Text::ParagraphRun>& PreparedText::GetParagraphInfo() const
{
  return mParagraphInfo;
}

uint32_t PreparedText::GetParagraphCount() const
{
  return static_cast<uint32_t>(mParagraphInfo.Count());
}

void PreparedText::SetScriptRuns(const Dali::Vector<Text::ScriptRun>& scriptRuns)
{
  mScriptRuns = scriptRuns;
}

const Dali::Vector<Text::ScriptRun>& PreparedText::GetScriptRuns() const
{
  return mScriptRuns;
}

uint32_t PreparedText::GetScriptRunCount() const
{
  return static_cast<uint32_t>(mScriptRuns.Count());
}

void PreparedText::SetFontRuns(const Dali::Vector<Text::FontRun>& fontRuns)
{
  mFontRuns = fontRuns;
}

const Dali::Vector<Text::FontRun>& PreparedText::GetFontRuns() const
{
  return mFontRuns;
}

uint32_t PreparedText::GetFontRunCount() const
{
  return static_cast<uint32_t>(mFontRuns.Count());
}

void PreparedText::SetGlyphs(const Dali::Vector<Text::GlyphInfo>& glyphs)
{
  mGlyphs = glyphs;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::GlyphInfo>& PreparedText::GetGlyphs() const
{
  return mGlyphs;
}

uint32_t PreparedText::GetGlyphCount() const
{
  return static_cast<uint32_t>(mGlyphs.Count());
}

void PreparedText::SetGlyphToCharacterMap(const Dali::Vector<Text::CharacterIndex>& glyphToCharacterMap)
{
  mGlyphToCharacterMap = glyphToCharacterMap;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::CharacterIndex>& PreparedText::GetGlyphToCharacterMap() const
{
  return mGlyphToCharacterMap;
}

void PreparedText::SetCharactersPerGlyph(const Dali::Vector<Text::Length>& charactersPerGlyph)
{
  mCharactersPerGlyph = charactersPerGlyph;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::Length>& PreparedText::GetCharactersPerGlyph() const
{
  return mCharactersPerGlyph;
}

void PreparedText::SetCharacterToGlyphTable(const Dali::Vector<Text::GlyphIndex>& characterToGlyphTable)
{
  mCharacterToGlyphTable = characterToGlyphTable;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::GlyphIndex>& PreparedText::GetCharacterToGlyphTable() const
{
  return mCharacterToGlyphTable;
}

void PreparedText::SetGlyphsPerCharacterTable(const Dali::Vector<Text::Length>& glyphsPerCharacterTable)
{
  mGlyphsPerCharacterTable = glyphsPerCharacterTable;
  ClearGlyphLayoutData();
}

const Dali::Vector<Text::Length>& PreparedText::GetGlyphsPerCharacterTable() const
{
  return mGlyphsPerCharacterTable;
}

void PreparedText::SetNewParagraphGlyphs(const Dali::Vector<Text::GlyphIndex>& newParagraphGlyphs)
{
  mNewParagraphGlyphs = newParagraphGlyphs;
}

const Dali::Vector<Text::GlyphIndex>& PreparedText::GetNewParagraphGlyphs() const
{
  return mNewParagraphGlyphs;
}

void PreparedText::SetGlyphLayoutData(const GlyphLayoutData& glyphLayoutData)
{
  const uint32_t glyphCount = GetGlyphCount();

  mGlyphLayoutData    = glyphLayoutData;
  mHasGlyphLayoutData = (glyphCount > 0u) &&
                        (mGlyphLayoutData.advances.Count() == glyphCount) &&
                        (mGlyphLayoutData.widths.Count() == glyphCount) &&
                        (mGlyphLayoutData.prefixAdvances.Count() == (glyphCount + 1u)) &&
                        (mGlyphLayoutData.characterStarts.Count() == glyphCount) &&
                        (mGlyphLayoutData.characterEnds.Count() == glyphCount) &&
                        (mGlyphLayoutData.breakAllowedAfterGlyph.Count() == glyphCount) &&
                        (mGlyphLayoutData.breakMandatoryAfterGlyph.Count() == glyphCount);
}

const PreparedText::GlyphLayoutData& PreparedText::GetGlyphLayoutData() const
{
  return mGlyphLayoutData;
}

bool PreparedText::HasGlyphLayoutData() const
{
  return mHasGlyphLayoutData;
}

void PreparedText::ClearGlyphLayoutData()
{
  mGlyphLayoutData.advances.Clear();
  mGlyphLayoutData.widths.Clear();
  mGlyphLayoutData.prefixAdvances.Clear();
  mGlyphLayoutData.characterStarts.Clear();
  mGlyphLayoutData.characterEnds.Clear();
  mGlyphLayoutData.breakAllowedAfterGlyph.Clear();
  mGlyphLayoutData.breakMandatoryAfterGlyph.Clear();
  mHasGlyphLayoutData = false;
}

bool PreparedText::HasGlyphData() const
{
  return !mGlyphs.Empty();
}

bool PreparedText::HasGlyphMetrics() const
{
  for(Vector<Text::GlyphInfo>::ConstIterator it = mGlyphs.Begin(), endIt = mGlyphs.End(); it != endIt; ++it)
  {
    const Text::GlyphInfo& glyph = *it;
    if((glyph.advance > 0.0f) || (glyph.width > 0.0f) || (glyph.height > 0.0f))
    {
      return true;
    }
  }

  return false;
}

float PreparedText::GetTotalGlyphAdvance() const
{
  float totalGlyphAdvance = 0.0f;

  for(Vector<Text::GlyphInfo>::ConstIterator it = mGlyphs.Begin(), endIt = mGlyphs.End(); it != endIt; ++it)
  {
    totalGlyphAdvance += it->advance;
  }

  return totalGlyphAdvance;
}

void PreparedText::SetLineMetrics(const LineMetrics& metrics)
{
  mLineMetrics    = metrics;
  mHasLineMetrics = (metrics.ascender > 0.0f) ||
                    (metrics.descender > 0.0f) ||
                    (metrics.lineGap > 0.0f) ||
                    (metrics.naturalLineHeight > 0.0f) ||
                    (metrics.baselineOffset > 0.0f);
}

const PreparedText::LineMetrics& PreparedText::GetLineMetrics() const
{
  return mLineMetrics;
}

bool PreparedText::HasLineMetrics() const
{
  return mHasLineMetrics;
}

void PreparedText::SetPrepared(bool prepared)
{
  mPrepared = prepared;
}

bool PreparedText::IsPrepared() const
{
  return mPrepared;
}

bool PreparedText::Empty() const
{
  return mCharacters.Empty();
}

void PreparedText::Clear()
{
  mOriginalText.Clear();
  mFontFamily.Clear();
  mFontSize     = 0.0f;
  mClusterCount = 0u;
  mCharacters.Clear();
  mLineBreakInfo.Clear();
  mParagraphInfo.Clear();
  mScriptRuns.Clear();
  mFontRuns.Clear();
  mGlyphs.Clear();
  mGlyphToCharacterMap.Clear();
  mCharactersPerGlyph.Clear();
  mCharacterToGlyphTable.Clear();
  mGlyphsPerCharacterTable.Clear();
  mNewParagraphGlyphs.Clear();
  ClearGlyphLayoutData();
  mLineMetrics    = LineMetrics{};
  mHasLineMetrics = false;
  mPrepared       = false;
}

} // namespace Dali::Ui::Internal::TextVisualizer

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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/metrics.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/segmentation.h>
#include <dali-ui-foundation/internal/text/shaper.h>
#include <dali-ui-foundation/internal/text/text-visualizer/text-preparer.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>

namespace Dali::Ui::Internal::TextVisualizer
{
namespace
{
constexpr float FONT_SIZE_SCALE = 1.0f;
constexpr float POINTS_PER_INCH = 72.0f;

TextAbstraction::FontDescription CreateDefaultFontDescription(const Dali::String& fontFamily)
{
  TextAbstraction::FontDescription defaultFontDescription;

  if(!fontFamily.Empty())
  {
    defaultFontDescription.family = fontFamily.CStr();
  }

  return defaultFontDescription;
}

float GetDpi()
{
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  if(horizontalDpi == 0u)
  {
    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
  return static_cast<float>(horizontalDpi);
}

float ConvertPixelToPoint(float pixel)
{
  return pixel * POINTS_PER_INCH / GetDpi();
}

Text::PointSize26Dot6 GetDefaultPointSize(TextAbstraction::FontClient& fontClient, float fontSize)
{
  // TextVisualizer public FontSize is defined as a pixel size.
  // Match the existing text stack by converting public pixels to point units
  // before expanding them to the internal 26.6 point-size representation.
  if(fontSize > 0.0f)
  {
    const float effectivePointSize = ConvertPixelToPoint(fontSize);
    return static_cast<Text::PointSize26Dot6>(effectivePointSize * FONT_SIZE_SCALE *
                                              fontClient.GetNumberOfPointsPerOneUnitOfPointSize());
  }

  return static_cast<Text::PointSize26Dot6>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE * FONT_SIZE_SCALE);
}

float GetGlyphPlacementWidth(const Text::GlyphInfo& glyph)
{
  if(glyph.width > 0.0f)
  {
    return glyph.width;
  }

  return glyph.advance > 0.0f ? glyph.advance : 0.0f;
}

float GetGlyphPlacementAdvance(const Text::GlyphInfo& glyph)
{
  if(glyph.advance > 0.0f)
  {
    return glyph.advance;
  }

  return glyph.width > 0.0f ? glyph.width : 0.0f;
}

PreparedText::GlyphLayoutData BuildGlyphLayoutData(const Dali::Vector<Text::GlyphInfo>&      glyphs,
                                                   const Dali::Vector<Text::CharacterIndex>& glyphToCharacterMap,
                                                   const Dali::Vector<Text::Length>&         charactersPerGlyph,
                                                   const Dali::Vector<Text::LineBreakInfo>&  lineBreakInfo,
                                                   uint32_t                                  characterCount)
{
  const uint32_t glyphCount = glyphs.Count();

  PreparedText::GlyphLayoutData glyphLayoutData;
  glyphLayoutData.advances.Resize(glyphCount);
  glyphLayoutData.widths.Resize(glyphCount);
  glyphLayoutData.prefixAdvances.Resize(glyphCount + 1u, 0.0f);
  glyphLayoutData.characterStarts.Resize(glyphCount);
  glyphLayoutData.characterEnds.Resize(glyphCount);
  glyphLayoutData.breakAllowedAfterGlyph.Resize(glyphCount, static_cast<uint8_t>(0u));
  glyphLayoutData.breakMandatoryAfterGlyph.Resize(glyphCount, static_cast<uint8_t>(0u));

  for(uint32_t glyphIndex = 0u; glyphIndex < glyphCount; ++glyphIndex)
  {
    const Text::GlyphInfo& glyph = glyphs[glyphIndex];

    glyphLayoutData.advances[glyphIndex] = GetGlyphPlacementAdvance(glyph);
    glyphLayoutData.widths[glyphIndex]   = GetGlyphPlacementWidth(glyph);

    uint32_t characterStart = characterCount;
    uint32_t characterEnd   = characterCount;

    if(glyphIndex < glyphToCharacterMap.Count())
    {
      characterStart               = glyphToCharacterMap[glyphIndex];
      const uint32_t characterSpan = glyphIndex < charactersPerGlyph.Count() ? charactersPerGlyph[glyphIndex] : 0u;
      characterEnd                 = std::min(characterCount, characterStart + characterSpan);
    }

    glyphLayoutData.characterStarts[glyphIndex] = characterStart;
    glyphLayoutData.characterEnds[glyphIndex]   = characterEnd;

    if(characterEnd > 0u)
    {
      const uint32_t characterIndex = characterEnd - 1u;
      if(characterIndex < lineBreakInfo.Count())
      {
        const Text::LineBreakInfo glyphLineBreakInfo       = lineBreakInfo[characterIndex];
        glyphLayoutData.breakAllowedAfterGlyph[glyphIndex] = (glyphLineBreakInfo == TextAbstraction::LINE_ALLOW_BREAK) ||
                                                             (glyphLineBreakInfo == TextAbstraction::LINE_MUST_BREAK);
        glyphLayoutData.breakMandatoryAfterGlyph[glyphIndex] = glyphLineBreakInfo == TextAbstraction::LINE_MUST_BREAK;
      }
    }

    glyphLayoutData.prefixAdvances[glyphIndex + 1u] = glyphLayoutData.prefixAdvances[glyphIndex] + glyphLayoutData.advances[glyphIndex];
  }

  return glyphLayoutData;
}

bool IsNewParagraphGlyph(Text::GlyphIndex glyphIndex, const Dali::Vector<Text::GlyphIndex>& newParagraphGlyphs)
{
  for(Vector<Text::GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End(); it != endIt; ++it)
  {
    if(*it == glyphIndex)
    {
      return true;
    }
  }

  return false;
}

PreparedText::LineMetrics CalculateLineMetrics(const Dali::Vector<Text::GlyphInfo>&  glyphs,
                                               const Dali::Vector<Text::GlyphIndex>& newParagraphGlyphs,
                                               float                                 fontSize)
{
  PreparedText::LineMetrics lineMetrics;

  bool hasMeasuredGlyph = false;
  for(uint32_t glyphIndex = 0u; glyphIndex < glyphs.Count(); ++glyphIndex)
  {
    if(IsNewParagraphGlyph(glyphIndex, newParagraphGlyphs))
    {
      continue;
    }

    const Text::GlyphInfo& glyph = glyphs[glyphIndex];
    if((glyph.advance <= 0.0f) && (glyph.width <= 0.0f) && (glyph.height <= 0.0f))
    {
      continue;
    }

    lineMetrics.ascender  = std::max(lineMetrics.ascender, glyph.yBearing);
    lineMetrics.descender = std::max(lineMetrics.descender, std::max(0.0f, glyph.height - glyph.yBearing));
    hasMeasuredGlyph      = true;
  }

  const float fallbackLineGap    = std::max(fontSize * 0.2f, 0.0f);
  const float fallbackLineHeight = (fontSize > 0.0f) ? (fontSize * 1.2f) : 0.0f;

  lineMetrics.lineGap = fallbackLineGap;

  if(hasMeasuredGlyph)
  {
    lineMetrics.naturalLineHeight = lineMetrics.ascender + lineMetrics.descender + lineMetrics.lineGap;
  }
  else if(fontSize > 0.0f)
  {
    lineMetrics.ascender          = fontSize;
    lineMetrics.descender         = 0.0f;
    lineMetrics.naturalLineHeight = fallbackLineHeight;
  }

  if((lineMetrics.naturalLineHeight <= 0.0f) && (fallbackLineHeight > 0.0f))
  {
    lineMetrics.naturalLineHeight = fallbackLineHeight;
  }

  lineMetrics.baselineOffset = std::max(lineMetrics.ascender, 0.0f);

  if(lineMetrics.naturalLineHeight < (lineMetrics.baselineOffset + lineMetrics.descender))
  {
    lineMetrics.naturalLineHeight = lineMetrics.baselineOffset + lineMetrics.descender + lineMetrics.lineGap;
  }

  return lineMetrics;
}
} // unnamed namespace

PreparedText TextPreparer::Prepare(const Input& input)
{
  PreparedText                       preparedText;
  Dali::Vector<Text::Character>      characters;
  Dali::Vector<Text::LineBreakInfo>  lineBreakInfo;
  Dali::Vector<Text::ParagraphRun>   paragraphInfo;
  Dali::Vector<Text::ScriptRun>      scriptRuns;
  Dali::Vector<Text::FontRun>        fontRuns;
  Dali::Vector<Text::GlyphInfo>      glyphs;
  Dali::Vector<Text::CharacterIndex> glyphToCharacterMap;
  Dali::Vector<Text::Length>         charactersPerGlyph;
  Dali::Vector<Text::GlyphIndex>     characterToGlyphTable;
  Dali::Vector<Text::Length>         glyphsPerCharacterTable;
  Dali::Vector<Text::GlyphIndex>     newParagraphGlyphs;

  preparedText.SetOriginalText(input.text);
  preparedText.SetFontFamily(input.fontFamily);
  preparedText.SetFontSize(input.fontSize);

  if(!input.text.Empty())
  {
    const auto* const utf8               = reinterpret_cast<const uint8_t*>(input.text.CStr());
    const uint32_t    utf8ByteCount      = input.text.Size();
    const uint32_t    numberOfCharacters = Text::GetNumberOfUtf8Characters(utf8, utf8ByteCount);

    characters.Resize(numberOfCharacters);
    const uint32_t convertedCharacterCount = Text::Utf8ToUtf32(utf8, utf8ByteCount, characters.Begin());
    characters.Resize(convertedCharacterCount);

    lineBreakInfo.Resize(convertedCharacterCount, TextAbstraction::LINE_NO_BREAK);
    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::Get();
    Text::SetLineBreakInfo(segmentation, characters, 0u, convertedCharacterCount, lineBreakInfo);

    Text::LogicalModelPtr logicalModel = Text::LogicalModel::New();
    logicalModel->mLineBreakInfo       = lineBreakInfo;
    logicalModel->CreateParagraphInfo(0u, convertedCharacterCount);
    paragraphInfo = logicalModel->mParagraphInfo;

    Text::MultilanguageSupport multilanguageSupport = Text::MultilanguageSupport::Get();
    multilanguageSupport.SetScripts(characters, 0u, convertedCharacterCount, scriptRuns);

    TextAbstraction::FontClient                  fontClient = TextAbstraction::FontClient::Get();
    const Dali::Vector<Text::FontDescriptionRun> fontDescriptionRuns;
    const TextAbstraction::FontDescription       defaultFontDescription = CreateDefaultFontDescription(input.fontFamily);
    const Text::PointSize26Dot6                  defaultPointSize       = GetDefaultPointSize(fontClient, input.fontSize);

    multilanguageSupport.ValidateFonts(fontClient, characters, scriptRuns, fontDescriptionRuns,
                                       defaultFontDescription, defaultPointSize, FONT_SIZE_SCALE, 0u,
                                       convertedCharacterCount, fontRuns, nullptr);

    if(!scriptRuns.Empty() && !fontRuns.Empty())
    {
      TextAbstraction::Shaping shaping = TextAbstraction::Shaping::Get();
      Text::ShapeText(shaping, fontClient, characters, lineBreakInfo, scriptRuns, fontRuns, 0u, 0u,
                      convertedCharacterCount, glyphs, glyphToCharacterMap, charactersPerGlyph, newParagraphGlyphs);

      const Text::Length glyphCount = glyphs.Count();
      if(glyphCount > 0u)
      {
        Text::MetricsPtr metrics = Text::Metrics::New(fontClient);
        metrics->GetGlyphMetrics(glyphs.Begin(), glyphCount);

        for(Vector<Text::GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(), endIt = newParagraphGlyphs.End();
            it != endIt; ++it)
        {
          const Text::GlyphIndex index = *it;
          if(index < glyphs.Count())
          {
            Text::GlyphInfo& glyph = glyphs[index];

            glyph.xBearing = 0.0f;
            glyph.width    = 0.0f;
            glyph.advance  = 0.0f;
          }
        }
      }

      Text::VisualModelPtr visualModel = Text::VisualModel::New();
      visualModel->mGlyphs             = glyphs;
      visualModel->mGlyphsToCharacters = glyphToCharacterMap;
      visualModel->mCharactersPerGlyph = charactersPerGlyph;
      visualModel->CreateGlyphsPerCharacterTable(0u, 0u, convertedCharacterCount);
      visualModel->CreateCharacterToGlyphTable(0u, 0u, convertedCharacterCount);

      glyphsPerCharacterTable = visualModel->mGlyphsPerCharacter;
      characterToGlyphTable   = visualModel->mCharactersToGlyph;
    }
  }

  preparedText.SetCharacters(characters);
  preparedText.SetLineBreakInfo(lineBreakInfo);
  preparedText.SetParagraphInfo(paragraphInfo);
  preparedText.SetScriptRuns(scriptRuns);
  preparedText.SetFontRuns(fontRuns);
  preparedText.SetGlyphs(glyphs);
  preparedText.SetGlyphToCharacterMap(glyphToCharacterMap);
  preparedText.SetCharactersPerGlyph(charactersPerGlyph);
  preparedText.SetCharacterToGlyphTable(characterToGlyphTable);
  preparedText.SetGlyphsPerCharacterTable(glyphsPerCharacterTable);
  preparedText.SetNewParagraphGlyphs(newParagraphGlyphs);

  if(!glyphs.Empty())
  {
    preparedText.SetGlyphLayoutData(BuildGlyphLayoutData(glyphs,
                                                         glyphToCharacterMap,
                                                         charactersPerGlyph,
                                                         lineBreakInfo,
                                                         preparedText.GetCharacterCount()));
  }

  if(!glyphs.Empty())
  {
    preparedText.SetLineMetrics(CalculateLineMetrics(glyphs, newParagraphGlyphs, input.fontSize));
  }

  preparedText.SetClusterCount(preparedText.GetCharacterCount());
  preparedText.SetPrepared(true);

  return preparedText;
}

} // namespace Dali::Ui::Internal::TextVisualizer

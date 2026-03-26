#ifndef DALI_UI_TEXT_MARKUP_PROCESSOR_H
#define DALI_UI_TEXT_MARKUP_PROCESSOR_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/internal/text/anchor.h>
#include <dali-ui-foundation/internal/text/bounded-paragraph-run.h>
#include <dali-ui-foundation/internal/text/character-spacing-character-run.h>
#include <dali-ui-foundation/internal/text/color-run.h>
#include <dali-ui-foundation/internal/text/embedded-item.h>
#include <dali-ui-foundation/internal/text/font-description-run.h>
#include <dali-ui-foundation/internal/text/strikethrough-character-run.h>
#include <dali-ui-foundation/internal/text/underlined-character-run.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Keeps the plain text and references to vectors from the model which stores runs with text styles.
 */
struct MarkupProcessData
{
  MarkupProcessData(Vector<ColorRun>& colorRuns, Vector<FontDescriptionRun>& fontRuns, Vector<EmbeddedItem>& items,
                    Vector<Anchor>& anchors, Vector<UnderlinedCharacterRun>& underlinedCharacterRuns,
                    Vector<ColorRun>&                     backgroundColorRuns,
                    Vector<StrikethroughCharacterRun>&    strikethroughCharacterRuns,
                    Vector<BoundedParagraphRun>&          boundedParagraphRuns,
                    Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns)
  : colorRuns(colorRuns),
    fontRuns(fontRuns),
    items(items),
    anchors(anchors),
    underlinedCharacterRuns(underlinedCharacterRuns),
    backgroundColorRuns(backgroundColorRuns),
    strikethroughCharacterRuns(strikethroughCharacterRuns),
    boundedParagraphRuns(boundedParagraphRuns),
    characterSpacingCharacterRuns(characterSpacingCharacterRuns),
    markupProcessedText()
  {
  }

  Vector<ColorRun>&                     colorRuns;                     ///< The color runs.
  Vector<FontDescriptionRun>&           fontRuns;                      ///< The font description runs.
  Vector<EmbeddedItem>&                 items;                         ///< The embedded items.
  Vector<Anchor>&                       anchors;                       ///< The anchors.
  Vector<UnderlinedCharacterRun>&       underlinedCharacterRuns;       ///< The underlined character runs.
  Vector<ColorRun>&                     backgroundColorRuns;           ///< The background color runs.
  Vector<StrikethroughCharacterRun>&    strikethroughCharacterRuns;    ///< The strikethrough character runs.
  Vector<BoundedParagraphRun>&          boundedParagraphRuns;          ///< The bounded paragraph runs
  Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns; ///< The character-spacing runs

  std::string markupProcessedText; ///< The mark-up string.
};

struct MarkupPropertyData
{
  MarkupPropertyData(float dpi, Vector4 anchorColor, Vector4 anchorClickedColor)
  : dpi(dpi),
    anchorColor(anchorColor),
    anchorClickedColor(anchorClickedColor)
  {
  }

  float   dpi;                ///< DPI used for pixel-to-point conversion.
  Vector4 anchorColor;        ///< The anchor color
  Vector4 anchorClickedColor; ///< The anchor clicked color
};

/**
 * @brief Process the mark-up string.
 *
 * @param[in] markupString The mark-up string.
 * @param[in] markupPropertyData The property data that will be used by default in markup processing.
 * @param[out] markupProcessData The plain text and the style.
 */
void ProcessMarkupString(const std::string& markupString, MarkupPropertyData& markupPropertyData,
                         MarkupProcessData& markupProcessData);

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_MARKUP_PROCESSOR_H

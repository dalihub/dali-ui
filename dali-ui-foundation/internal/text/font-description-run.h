#ifndef DALI_UI_TEXT_FONT_DESCRIPTION_RUN_H
#define DALI_UI_TEXT_FONT_DESCRIPTION_RUN_H

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
#include <dali/devel-api/text-abstraction/font-list.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/abstract-style-character-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Run of characters with the same font.
 */
struct FontDescriptionRun : public AbstractStyleCharacterRun
{
  /**
   * Default constructor to set the default values of bitfields
   */
  FontDescriptionRun()
  : AbstractStyleCharacterRun(),
    familyName{nullptr},
    familyLength{0u},
    weight{TextAbstraction::FontWeight::NONE},
    width{TextAbstraction::FontWidth::NONE},
    slant{TextAbstraction::FontSlant::NONE},
    size{0u},
    familyDefined{false},
    weightDefined{false},
    widthDefined{false},
    slantDefined{false},
    sizeDefined{false}
  {
  }

  FontDescriptionRun(const CharacterRun& characterRun, char* familyName, Length familyLength, FontWeightType weight,
                     FontWidthType width, FontSlantType slant, PointSize26Dot6 size, bool familyDefined, bool weightDefined,
                     bool widthDefined, bool slantDefined, bool sizeDefined)
  : AbstractStyleCharacterRun(characterRun),
    familyName{familyName},
    familyLength{familyLength},
    weight{weight},
    width{width},
    slant{slant},
    size{size},
    familyDefined{familyDefined},
    weightDefined{weightDefined},
    widthDefined{widthDefined},
    slantDefined{slantDefined},
    sizeDefined{sizeDefined}
  {
  }

  char*           familyName;   ///< The font's family name.
  Length          familyLength; ///< The length of the font's family name.
  FontWeightType  weight;       ///< The font's weight.
  FontWidthType   width;        ///< The font's width.
  FontSlantType   slant;        ///< The font's slant.
  PointSize26Dot6 size;         ///< The font's size.

  bool familyDefined : 1; ///< Whether the font's family is defined.
  bool weightDefined : 1; ///< Whether the font's weight is defined.
  bool widthDefined : 1;  ///< Whether the font's width is defined.
  bool slantDefined : 1;  ///< Whether the font's slant is defined.
  bool sizeDefined : 1;   ///< Whether the font's size is defined.
};

} // namespace Text

} // namespace Ui

// Allow FontDescriptionRun to be treated as a POD type
template<>
struct TypeTraits<Dali::Ui::Text::FontDescriptionRun> : public Dali::BasicTypes<Dali::Ui::Text::FontDescriptionRun>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_UI_TEXT_FONT_DESCRIPTION_RUN_H

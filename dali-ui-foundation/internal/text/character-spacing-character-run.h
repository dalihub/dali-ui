#ifndef DALI_UI_TEXT_CHARACTER_SPACING_CHARACTER_RUN_H
#define DALI_UI_TEXT_CHARACTER_SPACING_CHARACTER_RUN_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/abstract-style-character-run.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Run of character-spacing characters with same properties.
 */
struct CharacterSpacingCharacterRun : public AbstractStyleCharacterRun
{
  /**
   * @brief Constructor.
   * Default constructor to set the default values
   */
  CharacterSpacingCharacterRun()
  : AbstractStyleCharacterRun(),
    value{0.f} // The default value is 0.f which does nothing.
  {
  }

  float value; /// The spaces between characters in Pixels. A positive value will make the characters far apart
               /// (expanded) and a negative value will bring them closer (condensed).

  // TODO: Add unit property to choose between Pixel or Scale (%)
};

} // namespace Text

} // namespace Ui

// Allow ColorRun to be treated as a POD type
template<>
struct TypeTraits<Dali::Ui::Text::CharacterSpacingCharacterRun>
: public Dali::BasicTypes<Dali::Ui::Text::CharacterSpacingCharacterRun>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_UI_TEXT_CHARACTER_SPACING_CHARACTER_RUN_H

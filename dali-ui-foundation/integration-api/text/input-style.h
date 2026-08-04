#ifndef DALI_UI_INTEGRATION_TEXT_INPUT_STYLE_H
#define DALI_UI_INTEGRATION_TEXT_INPUT_STYLE_H

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
#include <dali/devel-api/text-abstraction/font-list.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector4.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace Text
{
using FontWidthType  = TextAbstraction::FontWidth::Type;  ///< The font's width.
using FontWeightType = TextAbstraction::FontWeight::Type; ///< The font's weight.
using FontSlantType  = TextAbstraction::FontSlant::Type;  ///< The font's slant.

/**
 * @brief Stores editable input text style values and their defined flags.
 */
struct DALI_UI_API InputStyle
{
  /**
   * @brief Bitmask values used to report which input style fields changed.
   */
  enum Mask
  {
    NONE                = 0x0000, ///< No style field changed.
    INPUT_COLOR         = 0x0001, ///< The input color changed.
    INPUT_FONT_FAMILY   = 0x0002, ///< The input font family changed.
    INPUT_POINT_SIZE    = 0x0004, ///< The input point size changed.
    INPUT_FONT_WEIGHT   = 0x0008, ///< The input font weight changed.
    INPUT_FONT_WIDTH    = 0x0010, ///< The input font width changed.
    INPUT_FONT_SLANT    = 0x0020, ///< The input font slant changed.
    INPUT_LINE_SPACING  = 0x0040, ///< The input line spacing changed.
    INPUT_UNDERLINE     = 0x0080, ///< The input underline style changed.
    INPUT_SHADOW        = 0x0100, ///< The input shadow style changed.
    INPUT_EMBOSS        = 0x0200, ///< The input emboss style changed.
    INPUT_OUTLINE       = 0x0400, ///< The input outline style changed.
    INPUT_STRIKETHROUGH = 0x0800  ///< The input strikethrough style changed.
  };

  /**
   * @brief Creates a default input style.
   */
  InputStyle()
  : textColor(Color::BLACK),
    familyName(),
    weight(TextAbstraction::FontWeight::NORMAL),
    width(TextAbstraction::FontWidth::NORMAL),
    slant(TextAbstraction::FontSlant::NORMAL),
    size(0.f),
    lineSpacing(0.f),
    underlineProperties(),
    shadowProperties(),
    embossProperties(),
    outlineProperties(),
    strikethroughProperties(),
    isDefaultColor(true),
    isFamilyDefined(false),
    isWeightDefined(false),
    isWidthDefined(false),
    isSlantDefined(false),
    isSizeDefined(false),
    isLineSpacingDefined(false),
    isUnderlineDefined(false),
    isShadowDefined(false),
    isEmbossDefined(false),
    isOutlineDefined(false),
    isStrikethroughDefined(false)
  {
  }

  /**
   * @brief Destructor.
   */
  ~InputStyle() = default;

  /**
   * @brief Copies input style values from another style.
   *
   * @param[in] inputStyle The input style to copy.
   */
  void Copy(const InputStyle& inputStyle)
  {
    isDefaultColor = inputStyle.isDefaultColor;
    textColor      = inputStyle.textColor;

    isFamilyDefined = inputStyle.isFamilyDefined;
    familyName      = inputStyle.familyName;

    isWeightDefined = inputStyle.isWeightDefined;
    weight          = inputStyle.weight;

    isWidthDefined = inputStyle.isWidthDefined;
    width          = inputStyle.width;

    isSlantDefined = inputStyle.isSlantDefined;
    slant          = inputStyle.slant;

    isSizeDefined = inputStyle.isSizeDefined;
    size          = inputStyle.size;

    isLineSpacingDefined = inputStyle.isLineSpacingDefined;
    lineSpacing          = inputStyle.lineSpacing;

    isUnderlineDefined  = inputStyle.isUnderlineDefined;
    underlineProperties = inputStyle.underlineProperties;

    isShadowDefined  = inputStyle.isShadowDefined;
    shadowProperties = inputStyle.shadowProperties;

    isEmbossDefined  = inputStyle.isEmbossDefined;
    embossProperties = inputStyle.embossProperties;

    isOutlineDefined  = inputStyle.isOutlineDefined;
    outlineProperties = inputStyle.outlineProperties;

    isStrikethroughDefined  = inputStyle.isStrikethroughDefined;
    strikethroughProperties = inputStyle.strikethroughProperties;
  }

  /**
   * @brief Compares this input style with another style.
   *
   * @param[in] inputStyle The input style to compare with.
   * @return True if the tracked style values are equal.
   */
  bool Equal(const InputStyle& inputStyle) const
  {
    if((isDefaultColor != inputStyle.isDefaultColor) || (isFamilyDefined != inputStyle.isFamilyDefined) ||
       (isWeightDefined != inputStyle.isWeightDefined) || (isWidthDefined != inputStyle.isWidthDefined) ||
       (isSlantDefined != inputStyle.isSlantDefined) || (isSizeDefined != inputStyle.isSizeDefined) ||
       (isLineSpacingDefined != inputStyle.isLineSpacingDefined) ||
       (isUnderlineDefined != inputStyle.isUnderlineDefined) || (isShadowDefined != inputStyle.isShadowDefined) ||
       (isEmbossDefined != inputStyle.isEmbossDefined) || (isOutlineDefined != inputStyle.isOutlineDefined) ||
       (textColor != inputStyle.textColor) || (familyName != inputStyle.familyName) || (weight != inputStyle.weight) ||
       (width != inputStyle.width) || (slant != inputStyle.slant) || (!Dali::Equals(size, inputStyle.size)) ||
       (!Dali::Equals(lineSpacing, inputStyle.lineSpacing)) ||
       (underlineProperties != inputStyle.underlineProperties) || (shadowProperties != inputStyle.shadowProperties) ||
       (embossProperties != inputStyle.embossProperties) || (outlineProperties != inputStyle.outlineProperties) ||
       (isStrikethroughDefined != inputStyle.isStrikethroughDefined))
    {
      return false;
    }

    return true;
  }

  /**
   * @brief Gets the bitmask of fields that differ from another input style.
   *
   * @param[in] inputStyle The input style to compare with.
   * @return The input style change mask.
   */
  Mask GetInputStyleChangeMask(const InputStyle& inputStyle) const
  {
    Mask mask = NONE;

    if(textColor != inputStyle.textColor)
    {
      mask = static_cast<Mask>(mask | INPUT_COLOR);
    }
    if(familyName != inputStyle.familyName)
    {
      mask = static_cast<Mask>(mask | INPUT_FONT_FAMILY);
    }
    if(weight != inputStyle.weight)
    {
      mask = static_cast<Mask>(mask | INPUT_FONT_WEIGHT);
    }
    if(width != inputStyle.width)
    {
      mask = static_cast<Mask>(mask | INPUT_FONT_WIDTH);
    }
    if(slant != inputStyle.slant)
    {
      mask = static_cast<Mask>(mask | INPUT_FONT_SLANT);
    }
    if(!Dali::Equals(size, inputStyle.size))
    {
      mask = static_cast<Mask>(mask | INPUT_POINT_SIZE);
    }
    if(!Dali::Equals(lineSpacing, inputStyle.lineSpacing))
    {
      mask = static_cast<Mask>(mask | INPUT_LINE_SPACING);
    }
    if(underlineProperties != inputStyle.underlineProperties)
    {
      mask = static_cast<Mask>(mask | INPUT_UNDERLINE);
    }
    if(shadowProperties != inputStyle.shadowProperties)
    {
      mask = static_cast<Mask>(mask | INPUT_SHADOW);
    }
    if(embossProperties != inputStyle.embossProperties)
    {
      mask = static_cast<Mask>(mask | INPUT_EMBOSS);
    }
    if(outlineProperties != inputStyle.outlineProperties)
    {
      mask = static_cast<Mask>(mask | INPUT_OUTLINE);
    }
    if(strikethroughProperties != inputStyle.strikethroughProperties)
    {
      mask = static_cast<Mask>(mask | INPUT_STRIKETHROUGH);
    }

    return mask;
  }

  Vector4        textColor;  ///< The text's color.
  std::string    familyName; ///< The font's family name.
  FontWeightType weight;     ///< The font's weight.
  FontWidthType  width;      ///< The font's width.
  FontSlantType  slant;      ///< The font's slant.
  float          size;       ///< The font's size.

  float lineSpacing; ///< The line's spacing.

  std::string underlineProperties;     ///< The underline properties string.
  std::string shadowProperties;        ///< The shadow properties string.
  std::string embossProperties;        ///< The emboss properties string.
  std::string outlineProperties;       ///< The outline properties string.
  std::string strikethroughProperties; ///< The strikethrough properties string.

  bool isDefaultColor : 1;  ///< Whether the text's color is the default.
  bool isFamilyDefined : 1; ///< Whether the font's family is defined.
  bool isWeightDefined : 1; ///< Whether the font's weight is defined.
  bool isWidthDefined : 1;  ///< Whether the font's width is defined.
  bool isSlantDefined : 1;  ///< Whether the font's slant is defined.
  bool isSizeDefined : 1;   ///< Whether the font's size is defined.

  bool isLineSpacingDefined : 1;   ///< Whether the line spacing is defined.
  bool isUnderlineDefined : 1;     ///< Whether the underline parameters are defined.
  bool isShadowDefined : 1;        ///< Whether the shadow parameters are defined.
  bool isEmbossDefined : 1;        ///< Whether the emboss parameters are defined.
  bool isOutlineDefined : 1;       ///< Whether the outline parameters are defined.
  bool isStrikethroughDefined : 1; ///< Whether the strikethrough parameters are defined.
};

} // namespace Text

} // namespace Integration

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTEGRATION_TEXT_INPUT_STYLE_H

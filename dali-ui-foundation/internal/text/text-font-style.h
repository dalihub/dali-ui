#ifndef DALI_UI_INTERNAL_TEXT_FONT_STYLE_H
#define DALI_UI_INTERNAL_TEXT_FONT_STYLE_H

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
#include <dali/devel-api/scripting/scripting.h>
#include <dali/devel-api/text-abstraction/font-list.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
const Scripting::StringEnum FONT_WEIGHT_STRING_TABLE[] =
  {
    {"thin", static_cast<int>(FontWeight::THIN)},

    {"extraLight", static_cast<int>(FontWeight::EXTRA_LIGHT)},
    {"ultraLight", static_cast<int>(FontWeight::ULTRA_LIGHT)},

    {"light", static_cast<int>(FontWeight::LIGHT)},

    {"demiLight", static_cast<int>(FontWeight::DEMI_LIGHT)},
    {"semiLight", static_cast<int>(FontWeight::SEMI_LIGHT)},

    {"book", static_cast<int>(FontWeight::BOOK)},

    {"normal", static_cast<int>(FontWeight::NORMAL)},
    {"regular", static_cast<int>(FontWeight::REGULAR)},

    {"medium", static_cast<int>(FontWeight::MEDIUM)},

    {"semiBold", static_cast<int>(FontWeight::SEMI_BOLD)},
    {"demiBold", static_cast<int>(FontWeight::DEMI_BOLD)},

    {"bold", static_cast<int>(FontWeight::BOLD)},

    {"extraBold", static_cast<int>(FontWeight::EXTRA_BOLD)},
    {"ultraBold", static_cast<int>(FontWeight::ULTRA_BOLD)},

    {"black", static_cast<int>(FontWeight::BLACK)},
    {"heavy", static_cast<int>(FontWeight::HEAVY)}};
const unsigned int FONT_WEIGHT_STRING_TABLE_COUNT =
  sizeof(FONT_WEIGHT_STRING_TABLE) / sizeof(FONT_WEIGHT_STRING_TABLE[0]);

const Scripting::StringEnum FONT_WIDTH_STRING_TABLE[] =
  {
    {"ultraCondensed", static_cast<int>(FontWidth::ULTRA_CONDENSED)},
    {"extraCondensed", static_cast<int>(FontWidth::EXTRA_CONDENSED)},
    {"condensed", static_cast<int>(FontWidth::CONDENSED)},
    {"semiCondensed", static_cast<int>(FontWidth::SEMI_CONDENSED)},
    {"normal", static_cast<int>(FontWidth::NORMAL)},
    {"semiExpanded", static_cast<int>(FontWidth::SEMI_EXPANDED)},
    {"expanded", static_cast<int>(FontWidth::EXPANDED)},
    {"extraExpanded", static_cast<int>(FontWidth::EXTRA_EXPANDED)},
    {"ultraExpanded", static_cast<int>(FontWidth::ULTRA_EXPANDED)}};
const unsigned int FONT_WIDTH_STRING_TABLE_COUNT =
  sizeof(FONT_WIDTH_STRING_TABLE) / sizeof(FONT_WIDTH_STRING_TABLE[0]);

const Scripting::StringEnum FONT_SLANT_STRING_TABLE[] =
  {
    {"normal", static_cast<int>(FontSlant::NORMAL)},
    {"roman", static_cast<int>(FontSlant::ROMAN)},
    {"italic", static_cast<int>(FontSlant::ITALIC)},
    {"oblique", static_cast<int>(FontSlant::OBLIQUE)}};
const unsigned int FONT_SLANT_STRING_TABLE_COUNT =
  sizeof(FONT_SLANT_STRING_TABLE) / sizeof(FONT_SLANT_STRING_TABLE[0]);

namespace FontStyle
{
enum Type
{
  DEFAULT,    ///< The default font's style.
  INPUT,      ///< The input font's style.
  PLACEHOLDER ///< The placeholder text font's style.
};
} // namespace FontStyle

/**
 * @brief Gets the string name of the font weight.
 *
 * @param[in] weight The font weight.
 * @return The string name of the font weight.
 */
const char* GetFontWeightName(FontWeight weight);

/**
 * @brief Gets the string name of the font width.
 *
 * @param[in] width The font width.
 * @return The string name of the font width.
 */
const char* GetFontWidthName(FontWidth width);

/**
 * @brief Gets the string name of the font slant.
 *
 * @param[in] slant The font slant.
 * @return The string name of the font slant.
 */
const char* GetFontSlantName(FontSlant slant);

/**
 * @brief Converts a font width enum to a TextAbstraction font width enum.
 *
 * @param[in] width The font width.
 * @return The corresponding TextAbstraction font width.
 */
TextAbstraction::FontWidth::Type ToTextAbstractionFontWidth(FontWidth width);

/**
 * @brief Converts a TextAbstraction font width enum to a font width enum.
 *
 * @param[in] width The TextAbstraction font width.
 * @return The corresponding font width.
 */
FontWidth ToFontWidth(TextAbstraction::FontWidth::Type width);

/**
 * @brief Converts a font weight enum to a TextAbstraction font weight enum.
 *
 * @param[in] weight The font weight.
 * @return The corresponding TextAbstraction font weight.
 */
TextAbstraction::FontWeight::Type ToTextAbstractionFontWeight(FontWeight weight);

/**
 * @brief Converts a TextAbstraction font weight enum to a font weight enum.
 *
 * @param[in] weight The TextAbstraction font weight.
 * @return The corresponding font weight.
 */
FontWeight ToFontWeight(TextAbstraction::FontWeight::Type weight);

/**
 * @brief Converts a font slant enum to a TextAbstraction font slant enum.
 *
 * @param[in] slant The font slant.
 * @return The corresponding TextAbstraction font slant.
 */
TextAbstraction::FontSlant::Type ToTextAbstractionFontSlant(FontSlant slant);

/**
 * @brief Converts a TextAbstraction font slant enum to a font slant enum.
 *
 * @param[in] slant The TextAbstraction font slant.
 * @return The corresponding font slant.
 */
FontSlant ToFontSlant(TextAbstraction::FontSlant::Type slant);

/**
 * @brief Sets the font family property.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The value of the font's family.
 */
void SetFontFamilyProperty(ControllerPtr controller, const Property::Value& value);

/**
 * @brief Sets the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The value of the font's style.
 * @param[in] type Whether the property is for the default font's style, the input font's style or the placeholder
 * font's style.
 */
void SetFontStyleProperty(ControllerPtr controller, const Property::Value& value, FontStyle::Type type);

/**
 * @brief Retrieves the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the font's style.
 * @param[in] type Whether the property is for the default font's style, the input font's style or the placeholder
 * font's style.
 */
void GetFontStyleProperty(ControllerPtr controller, Property::Value& value, FontStyle::Type type);

/**
 * @brief Converts a weight string into @e FontWeight.
 *
 * @param[in] weightStr The weight string. Must end with '\0'.
 *
 * @return The @e FontWeight value corresponding to the string.
 */
FontWeight StringToWeight(const char* const weightStr);

/**
 * @brief Converts a width string into @e FontWidth.
 *
 * @param[in] widthStr The width string. Must end with '\0'.
 *
 * @return The @e FontWidth value corresponding to the string.
 */
FontWidth StringToWidth(const char* const widthStr);

/**
 * @brief Converts a slant string into @e FontSlant.
 *
 * @param[in] slantStr The slant string. Must end with '\0'.
 *
 * @return The @e FontSlant value corresponding to the string.
 */
FontSlant StringToSlant(const char* const slantStr);

/**
 * @brief Gets the font weight from the provided property value.
 *
 * @param[in] propertyValue The source value, which can be Property::INTEGER or Property::STRING.
 * @param[out] fontWeight The resulting FontWeight from the given source.
 * @return true if the resulting fontWeight has been updated.
 */
bool GetFontWeightEnumeration(const Property::Value& propertyValue, FontWeight& fontWeight);

/**
 * @brief Gets the font width from the provided property value.
 *
 * @param[in] propertyValue The source value, which can be Property::INTEGER or Property::STRING.
 * @param[out] fontWidth The resulting FontWidth from the given source.
 * @return true if the resulting fontWidth has been updated.
 */
bool GetFontWidthEnumeration(const Property::Value& propertyValue, FontWidth& fontWidth);

/**
 * @brief Gets the font slant from the provided property value.
 *
 * @param[in] propertyValue The source value, which can be Property::INTEGER or Property::STRING.
 * @param[out] fontSlant The resulting FontSlant from the given source.
 * @return true if the resulting fontSlant has been updated.
 */
bool GetFontSlantEnumeration(const Property::Value& propertyValue, FontSlant& fontSlant);

} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_FONT_STYLE_H

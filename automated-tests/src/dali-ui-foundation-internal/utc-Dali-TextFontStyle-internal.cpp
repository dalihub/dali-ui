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
 */

#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_text_font_style_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_font_style_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextFontStyleEnumConversionsP(void)
{
  for(uint8_t value = 0u; value <= 10u; ++value)
  {
    FontWeight weight = static_cast<FontWeight>(value);
    DALI_TEST_CHECK(GetFontWeightName(weight));
    DALI_TEST_EQUALS(ToFontWeight(ToTextAbstractionFontWeight(weight)), weight, TEST_LOCATION);
  }
  for(uint8_t value = 0u; value <= 8u; ++value)
  {
    FontWidth width = static_cast<FontWidth>(value);
    DALI_TEST_CHECK(GetFontWidthName(width));
    DALI_TEST_EQUALS(ToFontWidth(ToTextAbstractionFontWidth(width)), width, TEST_LOCATION);
  }
  for(uint8_t value = 0u; value <= 2u; ++value)
  {
    FontSlant slant = static_cast<FontSlant>(value);
    DALI_TEST_CHECK(GetFontSlantName(slant));
    DALI_TEST_EQUALS(ToFontSlant(ToTextAbstractionFontSlant(slant)), slant, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(ToTextAbstractionFontWeight(static_cast<FontWeight>(99)), TextAbstraction::FontWeight::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(ToFontWeight(static_cast<TextAbstraction::FontWeight::Type>(99)), FontWeight::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(ToTextAbstractionFontWidth(static_cast<FontWidth>(99)), TextAbstraction::FontWidth::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(ToFontWidth(static_cast<TextAbstraction::FontWidth::Type>(99)), FontWidth::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(ToTextAbstractionFontSlant(static_cast<FontSlant>(99)), TextAbstraction::FontSlant::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(ToFontSlant(static_cast<TextAbstraction::FontSlant::Type>(99)), FontSlant::NORMAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextFontStyleSetGetP(void)
{
  UiTestApplication application(UiConfig::New());
  ControllerPtr controller = Controller::New();
  Property::Map styleMap;
  styleMap.Insert("weight", "bold");
  styleMap.Insert("width", "condensed");
  styleMap.Insert("slant", "italic");

  const FontStyle::Type types[] = {FontStyle::DEFAULT, FontStyle::INPUT, FontStyle::PLACEHOLDER};
  for(FontStyle::Type type : types)
  {
    SetFontStyleProperty(controller, styleMap, type);
    Property::Value output;
    GetFontStyleProperty(controller, output, type);
    DALI_TEST_EQUALS(output.GetType(), Property::MAP, TEST_LOCATION);
  }

  const Property::Value stringStyle(R"JSON({"weight":"light","width":"expanded","slant":"oblique"})JSON");
  for(FontStyle::Type type : types)
  {
    SetFontStyleProperty(controller, stringStyle, type);
    Property::Value output;
    GetFontStyleProperty(controller, output, type);
    DALI_TEST_EQUALS(output.GetType(), Property::STRING, TEST_LOCATION);
  }

  Property::Map empty;
  for(FontStyle::Type type : types)
  {
    SetFontStyleProperty(controller, empty, type);
    Property::Value output;
    GetFontStyleProperty(controller, output, type);
  }

  SetFontStyleProperty(ControllerPtr(), styleMap, FontStyle::DEFAULT);
  Property::Value unchanged(17);
  GetFontStyleProperty(ControllerPtr(), unchanged, FontStyle::DEFAULT);
  DALI_TEST_EQUALS(unchanged.Get<int>(), 17, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextFontStyleFamilyAndParsingP(void)
{
  UiTestApplication application(UiConfig::New());
  ControllerPtr controller = Controller::New();
  SetFontFamilyProperty(controller, Property::Value(""));
  SetFontFamilyProperty(controller, Property::Value("Sans"));
  SetFontFamilyProperty(controller, Property::Value(R"JSON({"family":"Serif"})JSON"));
  SetFontFamilyProperty(controller, Property::Value(R"JSON({"family":"Monospace","type":"system"})JSON"));
  SetFontFamilyProperty(controller, Property::Value(R"JSON({"type":"custom"})JSON"));
  SetFontFamilyProperty(ControllerPtr(), Property::Value("Sans"));

  DALI_TEST_EQUALS(StringToWeight("heavy"), FontWeight::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(StringToWeight("invalid"), FontWeight::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(StringToWidth("ultraExpanded"), FontWidth::ULTRA_EXPANDED, TEST_LOCATION);
  DALI_TEST_EQUALS(StringToWidth("invalid"), FontWidth::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(StringToSlant("roman"), FontSlant::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(StringToSlant("invalid"), FontSlant::NORMAL, TEST_LOCATION);

  FontWeight weight = FontWeight::NORMAL;
  FontWidth width = FontWidth::NORMAL;
  FontSlant slant = FontSlant::NORMAL;
  DALI_TEST_CHECK(GetFontWeightEnumeration(Property::Value("semiBold"), weight));
  DALI_TEST_CHECK(GetFontWeightEnumeration(Property::Value(static_cast<int>(FontWeight::THIN)), weight));
  DALI_TEST_CHECK(!GetFontWeightEnumeration(Property::Value(Vector2::ZERO), weight));
  DALI_TEST_CHECK(GetFontWidthEnumeration(Property::Value("extraCondensed"), width));
  DALI_TEST_CHECK(GetFontWidthEnumeration(Property::Value(static_cast<int>(FontWidth::EXPANDED)), width));
  DALI_TEST_CHECK(!GetFontWidthEnumeration(Property::Value(Vector2::ZERO), width));
  DALI_TEST_CHECK(GetFontSlantEnumeration(Property::Value("oblique"), slant));
  DALI_TEST_CHECK(GetFontSlantEnumeration(Property::Value(static_cast<int>(FontSlant::ITALIC)), slant));
  DALI_TEST_CHECK(!GetFontSlantEnumeration(Property::Value(Vector2::ZERO), slant));
  END_TEST;
}

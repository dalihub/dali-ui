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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

namespace
{
Dali::Vector<FontVariationAxis> MakeAxes(std::initializer_list<std::pair<const char*, float>> values)
{
  Dali::Vector<FontVariationAxis> axes;
  for(const auto& value : values)
  {
    axes.PushBack(FontVariationAxis(value.first, value.second));
  }
  return axes;
}

bool CheckAxesEqual(const Dali::Vector<FontVariationAxis>& lhs, const Dali::Vector<FontVariationAxis>& rhs)
{
  if(lhs.Count() != rhs.Count())
  {
    return false;
  }

  for(std::size_t i = 0u; i < lhs.Count(); i++)
  {
    if(lhs[i].GetTag() != rhs[i].GetTag())
    {
      return false;
    }

    if(!Dali::Equals(lhs[i].GetValue(), rhs[i].GetValue()))
    {
      return false;
    }
  }

  return true;
}

void DALI_TEST_EQUALS_AXES(const Dali::Vector<FontVariationAxis>& actual, const Dali::Vector<FontVariationAxis>& expected, const char* location)
{
  DALI_TEST_EQUALS(CheckAxesEqual(actual, expected), true, location);
}
} // unnamed namespace

void utc_dali_font_variation_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_font_variation_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFontVariationFromStringUnquoted(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"wght", 700.0f},
    {"wdth", 90.0f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=700,wdth=90");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringDoubleQuoted(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"wght", 700.0f},
    {"wdth", 90.0f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("\"wght\" 700, \"wdth\" 90");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringSingleQuoted(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"wght", 700.0f},
    {"wdth", 90.0f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("'wght' 700, 'wdth' 90");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringWhitespace(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"wght", 700.0f},
    {"wdth", 90.0f},
    {"slnt", -7.5f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("  wght = 700 , wdth = 90 , slnt = -7.5  ");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringDuplicateLastWins(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"wght", 600.0f},
    {"wdth", 90.0f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=700,wdth=90,wght=600");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringQuotedTagWithTrailingSpaces(void)
{
  const Dali::Vector<FontVariationAxis> expected = MakeAxes({
    {"AX  ", 1.0f},
    {"FB  ", 2.0f},
  });

  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("'AX  ' 1.0, 'FB  ' 2.0");

  DALI_TEST_EQUALS_AXES(actual, expected, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringEmpty(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(FontVariation::None().Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(FontVariation::ToString(FontVariation::None()), Dali::String(""), TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidTagLengthShort(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("abc=1.0");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidTagLengthLong(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("abcde=1.0");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidQuotedFormat(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("\"wght\"=700");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidMissingSeparator(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=700 wdth=90");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidTrailingGarbage(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=700abc");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidTrailingComma(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=700,");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidNaN(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=nan");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationFromStringInvalidInf(void)
{
  const Dali::Vector<FontVariationAxis> actual = FontVariation::FromString("wght=inf");

  DALI_TEST_EQUALS(actual.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationToStringUnquoted(void)
{
  const Dali::Vector<FontVariationAxis> axes = MakeAxes({
    {"wght", 700.0f},
    {"wdth", 90.0f},
    {"slnt", -7.5f},
  });

  const Dali::String actual = FontVariation::ToString(axes);

  DALI_TEST_EQUALS(actual, Dali::String("wght=700,wdth=90,slnt=-7.5"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationToStringQuotedTagWithTrailingSpaces(void)
{
  const Dali::Vector<FontVariationAxis> axes = MakeAxes({
    {"AX  ", 1.0f},
    {"FB  ", 2.0f},
  });

  const Dali::String actual = FontVariation::ToString(axes);

  DALI_TEST_EQUALS(actual, Dali::String("\"AX  \" 1,\"FB  \" 2"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationRoundTripUnquoted(void)
{
  const Dali::String input("wght=700,wdth=90,slnt=-7.5");

  const Dali::Vector<FontVariationAxis> axes       = FontVariation::FromString(input);
  const Dali::String                    output     = FontVariation::ToString(axes);
  const Dali::Vector<FontVariationAxis> reparsed   = FontVariation::FromString(output);

  DALI_TEST_EQUALS(output, Dali::String("wght=700,wdth=90,slnt=-7.5"), TEST_LOCATION);
  DALI_TEST_EQUALS_AXES(reparsed, axes, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFontVariationRoundTripQuotedTagWithTrailingSpaces(void)
{
  const Dali::String input("'AX  ' 1.0, 'FB  ' 2.0");

  const Dali::Vector<FontVariationAxis> axes     = FontVariation::FromString(input);
  const Dali::String                    output   = FontVariation::ToString(axes);
  const Dali::Vector<FontVariationAxis> reparsed = FontVariation::FromString(output);

  DALI_TEST_EQUALS(output, Dali::String("\"AX  \" 1,\"FB  \" 2"), TEST_LOCATION);
  DALI_TEST_EQUALS_AXES(reparsed, axes, TEST_LOCATION);
  END_TEST;
}

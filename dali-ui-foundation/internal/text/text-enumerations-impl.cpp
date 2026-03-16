/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
const Dali::Scripting::StringEnum TEXT_ALIGNMENT_TYPE_TABLE[] =
  {
    {"START", static_cast<int32_t>(Alignment::START)},
    {"CENTER", static_cast<int32_t>(Alignment::CENTER)},
    {"END", static_cast<int32_t>(Alignment::END)},
};

const Dali::Scripting::StringEnum LINE_WRAP_MODE_TABLE[] =
  {
    {"WORD", static_cast<int32_t>(LineWrapMode::WORD)},
    {"CHARACTER", static_cast<int32_t>(LineWrapMode::CHARACTER)},
    {"HYPHENATION", static_cast<int32_t>(LineWrapMode::HYPHENATION)},
    {"MIXED", static_cast<int32_t>(LineWrapMode::MIXED)},
};

const uint32_t TEXT_ALIGNMENT_TYPE_TABLE_COUNT = static_cast<uint32_t>(sizeof(TEXT_ALIGNMENT_TYPE_TABLE) / sizeof(TEXT_ALIGNMENT_TYPE_TABLE[0]));
const uint32_t LINE_WRAP_MODE_TABLE_COUNT      = static_cast<uint32_t>(sizeof(LINE_WRAP_MODE_TABLE) / sizeof(LINE_WRAP_MODE_TABLE[0]));

DALI_ENUM_TO_STRING_TABLE_BEGIN(ELLIPSIS_POSITION_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::DevelText::EllipsisPosition, END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::DevelText::EllipsisPosition, START)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::DevelText::EllipsisPosition, MIDDLE)
DALI_ENUM_TO_STRING_TABLE_END(ELLIPSIS_POSITION_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(UNDERLINE_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::Underline::Type, SOLID)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::Underline::Type, DASHED)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::Underline::Type, DOUBLE)
DALI_ENUM_TO_STRING_TABLE_END(UNDERLINE_TYPE)
} // namespace

bool GetHorizontalAlignmentEnumeration(const Property::Value& propertyValue, Alignment& alignment)
{
  int        enumValue = 0;
  const bool result    = Scripting::GetEnumerationProperty(propertyValue,
                                                           TEXT_ALIGNMENT_TYPE_TABLE,
                                                           TEXT_ALIGNMENT_TYPE_TABLE_COUNT,
                                                           enumValue);
  if(result)
  {
    alignment = static_cast<Alignment>(enumValue);
  }
  return result;
}

bool GetVerticalAlignmentEnumeration(const Property::Value& propertyValue, Alignment& alignment)
{
  int        enumValue = 0;
  const bool result    = Scripting::GetEnumerationProperty(propertyValue,
                                                           TEXT_ALIGNMENT_TYPE_TABLE,
                                                           TEXT_ALIGNMENT_TYPE_TABLE_COUNT,
                                                           enumValue);
  if(result)
  {
    alignment = static_cast<Alignment>(enumValue);
  }
  return result;
}

bool GetLineWrapModeEnumeration(const Property::Value& propertyValue, LineWrapMode& lineWrapMode)
{
  int enumValue = 0;

  const bool result = Scripting::GetEnumerationProperty(propertyValue,
                                                        LINE_WRAP_MODE_TABLE,
                                                        LINE_WRAP_MODE_TABLE_COUNT,
                                                        enumValue);

  if(result)
  {
    lineWrapMode = static_cast<LineWrapMode>(enumValue);
  }

  return result;
}

const char* GetHorizontalAlignmentString(const Alignment& alignment)
{
  return Scripting::GetLinearEnumerationName(
    static_cast<int32_t>(alignment),
    TEXT_ALIGNMENT_TYPE_TABLE,
    TEXT_ALIGNMENT_TYPE_TABLE_COUNT);
}

const char* GetVerticalAlignmentString(const Alignment& alignment)
{
  return Scripting::GetLinearEnumerationName(
    static_cast<int32_t>(alignment),
    TEXT_ALIGNMENT_TYPE_TABLE,
    TEXT_ALIGNMENT_TYPE_TABLE_COUNT);
}

bool GetEllipsisPositionTypeEnumeration(const Property::Value&                 propertyValue,
                                        Ui::DevelText::EllipsisPosition::Type& ellipsisPositionType)
{
  return Scripting::GetEnumerationProperty(propertyValue, ELLIPSIS_POSITION_TYPE_TABLE,
                                           ELLIPSIS_POSITION_TYPE_TABLE_COUNT, ellipsisPositionType);
}

const char* GetUnderlineTypeToString(const Ui::Text::Underline::Type& type)
{
  return Scripting::GetLinearEnumerationName<Ui::Text::Underline::Type>(type, UNDERLINE_TYPE_TABLE,
                                                                        UNDERLINE_TYPE_TABLE_COUNT);
}

} // namespace Text

} // namespace Ui

} // namespace Dali

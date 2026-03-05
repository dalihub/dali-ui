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
namespace UI
{
namespace Text
{
namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(HORIZONTAL_ALIGNMENT_TYPE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::HorizontalAlignment, BEGIN)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::HorizontalAlignment, CENTER)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::HorizontalAlignment, END)
DALI_ENUM_TO_STRING_TABLE_END(HORIZONTAL_ALIGNMENT_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(VERTICAL_ALIGNMENT_TYPE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::VerticalAlignment, TOP)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::VerticalAlignment, CENTER)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::VerticalAlignment, BOTTOM)
DALI_ENUM_TO_STRING_TABLE_END(VERTICAL_ALIGNMENT_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(LINE_WRAP_MODE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::LineWrap, WORD)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::LineWrap, CHARACTER)
DALI_ENUM_TO_STRING_TABLE_END(LINE_WRAP_MODE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(ELLIPSIS_POSITION_TYPE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::DevelText::EllipsisPosition, END)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::DevelText::EllipsisPosition, START)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::DevelText::EllipsisPosition, MIDDLE)
DALI_ENUM_TO_STRING_TABLE_END(ELLIPSIS_POSITION_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(UNDERLINE_TYPE)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::Underline::Type, SOLID)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::Underline::Type, DASHED)
DALI_ENUM_TO_STRING_WITH_SCOPE(UI::Text::Underline::Type, DOUBLE)
DALI_ENUM_TO_STRING_TABLE_END(UNDERLINE_TYPE)
} // namespace

bool GetHorizontalAlignmentEnumeration(const Property::Value& propertyValue,
                                       UI::Text::HorizontalAlignment::Type& alignment)
{
  return Scripting::GetEnumerationProperty(propertyValue, HORIZONTAL_ALIGNMENT_TYPE_TABLE,
                                           HORIZONTAL_ALIGNMENT_TYPE_TABLE_COUNT, alignment);
}

bool GetVerticalAlignmentEnumeration(const Property::Value& propertyValue, UI::Text::VerticalAlignment::Type& alignment)
{
  return Scripting::GetEnumerationProperty(propertyValue, VERTICAL_ALIGNMENT_TYPE_TABLE,
                                           VERTICAL_ALIGNMENT_TYPE_TABLE_COUNT, alignment);
}

bool GetLineWrapModeEnumeration(const Property::Value& propertyValue, UI::Text::LineWrap::Mode& lineWrapMode)
{
  return Scripting::GetEnumerationProperty(propertyValue, LINE_WRAP_MODE_TABLE, LINE_WRAP_MODE_TABLE_COUNT,
                                           lineWrapMode);
}

const char* GetHorizontalAlignmentString(const UI::Text::HorizontalAlignment::Type& alignment)
{
  return Scripting::GetLinearEnumerationName<UI::Text::HorizontalAlignment::Type>(
      alignment, HORIZONTAL_ALIGNMENT_TYPE_TABLE, HORIZONTAL_ALIGNMENT_TYPE_TABLE_COUNT);
}

const char* GetVerticalAlignmentString(const UI::Text::VerticalAlignment::Type& alignment)
{
  return Scripting::GetLinearEnumerationName<UI::Text::VerticalAlignment::Type>(
      alignment, VERTICAL_ALIGNMENT_TYPE_TABLE, VERTICAL_ALIGNMENT_TYPE_TABLE_COUNT);
}

bool GetEllipsisPositionTypeEnumeration(const Property::Value& propertyValue,
                                        UI::DevelText::EllipsisPosition::Type& ellipsisPositionType)
{
  return Scripting::GetEnumerationProperty(propertyValue, ELLIPSIS_POSITION_TYPE_TABLE,
                                           ELLIPSIS_POSITION_TYPE_TABLE_COUNT, ellipsisPositionType);
}

const char* GetUnderlineTypeToString(const UI::Text::Underline::Type& type)
{
  return Scripting::GetLinearEnumerationName<UI::Text::Underline::Type>(type, UNDERLINE_TYPE_TABLE,
                                                                        UNDERLINE_TYPE_TABLE_COUNT);
}

} // namespace Text

} // namespace UI

} // namespace Dali

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
const Dali::Scripting::StringEnum TEXT_ALIGNMENT_TABLE[] =
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

const Dali::Scripting::StringEnum UNDERLINE_TABLE[] =
  {
    {"SOLID", static_cast<int32_t>(Underline::Type::SOLID)},
    {"DASHED", static_cast<int32_t>(Underline::Type::DASHED)},
    {"DOUBLE", static_cast<int32_t>(Underline::Type::DOUBLE)},
};

const Dali::Scripting::StringEnum LINE_HEIGHT_MODE_TABLE[] =
  {
    {"RELATIVE", static_cast<int32_t>(LineHeightMode::RELATIVE)},
    {"ABSOLUTE", static_cast<int32_t>(LineHeightMode::ABSOLUTE)},
};

const Dali::Scripting::StringEnum LAYOUT_DIRECTION_MODE_TABLE[] =
  {
    {"CONTENTS", static_cast<int32_t>(LayoutDirectionMode::CONTENTS)},
    {"INHERIT", static_cast<int32_t>(LayoutDirectionMode::INHERIT)},
    {"LOCALE", static_cast<int32_t>(LayoutDirectionMode::LOCALE)},
};

const Dali::Scripting::StringEnum MARQUEE_STOP_MODE_TABLE[] =
  {
    {"IMMEDIATE", static_cast<int32_t>(MarqueeStopMode::IMMEDIATE)},
    {"FINISH_LOOP", static_cast<int32_t>(MarqueeStopMode::FINISH_LOOP)},
};

const Dali::Scripting::StringEnum MARQUEE_ORIENTATION_TABLE[] =
  {
    {"HORIZONTAL", static_cast<int32_t>(MarqueeOrientation::HORIZONTAL)},
    {"VERTICAL", static_cast<int32_t>(MarqueeOrientation::VERTICAL)},
};

const Dali::Scripting::StringEnum OVERFLOW_MODE_TABLE[] =
  {
    {"CLIP", static_cast<int32_t>(OverflowMode::CLIP)},
    {"ELLIPSIS", static_cast<int32_t>(OverflowMode::ELLIPSIS)},
};

const uint32_t TEXT_ALIGNMENT_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(TEXT_ALIGNMENT_TABLE) / sizeof(TEXT_ALIGNMENT_TABLE[0]));
const uint32_t LINE_WRAP_MODE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(LINE_WRAP_MODE_TABLE) / sizeof(LINE_WRAP_MODE_TABLE[0]));
const uint32_t UNDERLINE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(UNDERLINE_TABLE) / sizeof(UNDERLINE_TABLE[0]));
const uint32_t LINE_HEIGHT_MODE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(LINE_HEIGHT_MODE_TABLE) / sizeof(LINE_HEIGHT_MODE_TABLE[0]));
const uint32_t LAYOUT_DIRECTION_MODE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(LAYOUT_DIRECTION_MODE_TABLE) / sizeof(LAYOUT_DIRECTION_MODE_TABLE[0]));
const uint32_t MARQUEE_STOP_MODE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(MARQUEE_STOP_MODE_TABLE) / sizeof(MARQUEE_STOP_MODE_TABLE[0]));
const uint32_t MARQUEE_ORIENTATION_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(MARQUEE_ORIENTATION_TABLE) / sizeof(MARQUEE_ORIENTATION_TABLE[0]));
const uint32_t OVERFLOW_MODE_TABLE_COUNT =
  static_cast<uint32_t>(sizeof(OVERFLOW_MODE_TABLE) / sizeof(OVERFLOW_MODE_TABLE[0]));

DALI_ENUM_TO_STRING_TABLE_BEGIN(ELLIPSIS_POSITION_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::EllipsisPosition, END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::EllipsisPosition, START)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Text::EllipsisPosition, MIDDLE)
DALI_ENUM_TO_STRING_TABLE_END(ELLIPSIS_POSITION_TYPE)

} // unnamed namespace

bool GetHorizontalAlignmentEnumeration(const Property::Value& propertyValue, Alignment& alignment)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           TEXT_ALIGNMENT_TABLE,
                                           TEXT_ALIGNMENT_TABLE_COUNT,
                                           alignment);
}

bool GetVerticalAlignmentEnumeration(const Property::Value& propertyValue, Alignment& alignment)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           TEXT_ALIGNMENT_TABLE,
                                           TEXT_ALIGNMENT_TABLE_COUNT,
                                           alignment);
}

bool GetLineWrapModeEnumeration(const Property::Value& propertyValue, LineWrapMode& lineWrapMode)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           LINE_WRAP_MODE_TABLE,
                                           LINE_WRAP_MODE_TABLE_COUNT,
                                           lineWrapMode);
}

const char* GetHorizontalAlignmentString(const Alignment& alignment)
{
  return Scripting::GetLinearEnumerationName(static_cast<int32_t>(alignment),
                                             TEXT_ALIGNMENT_TABLE,
                                             TEXT_ALIGNMENT_TABLE_COUNT);
}

const char* GetVerticalAlignmentString(const Alignment& alignment)
{
  return Scripting::GetLinearEnumerationName(static_cast<int32_t>(alignment),
                                             TEXT_ALIGNMENT_TABLE,
                                             TEXT_ALIGNMENT_TABLE_COUNT);
}

bool GetEllipsisPositionTypeEnumeration(const Property::Value&            propertyValue,
                                        Ui::Text::EllipsisPosition::Type& ellipsisPositionType)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           ELLIPSIS_POSITION_TYPE_TABLE,
                                           ELLIPSIS_POSITION_TYPE_TABLE_COUNT,
                                           ellipsisPositionType);
}

const char* GetUnderlineTypeToString(const Ui::Text::Underline::Type& type)
{
  return Scripting::GetLinearEnumerationName(static_cast<int32_t>(type),
                                             UNDERLINE_TABLE,
                                             UNDERLINE_TABLE_COUNT);
}

bool GetLineHeightModeEnumeration(const Property::Value& propertyValue, LineHeightMode& lineHeightMode)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           LINE_HEIGHT_MODE_TABLE,
                                           LINE_HEIGHT_MODE_TABLE_COUNT,
                                           lineHeightMode);
}

bool GetLayoutDirectionModeEnumeration(const Property::Value& propertyValue, LayoutDirectionMode& layoutDirectionMode)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           LAYOUT_DIRECTION_MODE_TABLE,
                                           LAYOUT_DIRECTION_MODE_TABLE_COUNT,
                                           layoutDirectionMode);
}

bool GetMarqueeStopModeEnumeration(const Property::Value& propertyValue, MarqueeStopMode& marqueeStopMode)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           MARQUEE_STOP_MODE_TABLE,
                                           MARQUEE_STOP_MODE_TABLE_COUNT,
                                           marqueeStopMode);
}

bool GetMarqueeOrientationEnumeration(const Property::Value& propertyValue, MarqueeOrientation& marqueeOrientation)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           MARQUEE_ORIENTATION_TABLE,
                                           MARQUEE_ORIENTATION_TABLE_COUNT,
                                           marqueeOrientation);
}

bool GetOverflowModeEnumeration(const Property::Value& propertyValue, OverflowMode& overflowMode)
{
  return Scripting::GetEnumerationProperty(propertyValue,
                                           OVERFLOW_MODE_TABLE,
                                           OVERFLOW_MODE_TABLE_COUNT,
                                           overflowMode);
}

} // namespace Text

} // namespace Ui

} // namespace Dali

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

#include <dali-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layout-types.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_layouttypes_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layouttypes_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliWrapContentConstantP(void)
{
  DALI_TEST_EQUALS(WRAP_CONTENT, -1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMatchParentConstantP(void)
{
  DALI_TEST_EQUALS(MATCH_PARENT, -2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeasuredSizeDefaultConstructorP(void)
{
  MeasuredSize size;
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeasuredSizeConstructorP(void)
{
  const float w = 100.0f;
  const float h = 50.0f;
  MeasuredSize size(w, h);
  DALI_TEST_EQUALS(size.GetWidth(), w, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), h, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeasuredSizeSetWidthP(void)
{
  MeasuredSize size;
  const float w = 75.0f;
  size.SetWidth(w);
  DALI_TEST_EQUALS(size.GetWidth(), w, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeasuredSizeSetHeightP(void)
{
  MeasuredSize size;
  const float h = 120.0f;
  size.SetHeight(h);
  DALI_TEST_EQUALS(size.GetHeight(), h, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMeasuredSizeToVector2P(void)
{
  MeasuredSize size(10.0f, 20.0f);
  Vector2 v = size.ToVector2();
  DALI_TEST_EQUALS(v.x, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v.y, 20.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRectDefaultConstructorP(void)
{
  LayoutRect rect;
  DALI_TEST_EQUALS(rect.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRectConstructorP(void)
{
  const float x = 1.0f, y = 2.0f, w = 3.0f, h = 4.0f;
  LayoutRect rect(x, y, w, h);
  DALI_TEST_EQUALS(rect.GetX(), x, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetY(), y, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetWidth(), w, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetHeight(), h, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRectSettersGettersP(void)
{
  LayoutRect rect;
  rect.SetX(5.0f);
  rect.SetY(10.0f);
  rect.SetWidth(100.0f);
  rect.SetHeight(200.0f);
  DALI_TEST_EQUALS(rect.GetX(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetY(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetPosition().x, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetPosition().y, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rect.GetSize().height, 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthDefaultP(void)
{
  GridLength length;
  DALI_TEST_EQUALS(static_cast<uint8_t>(length.GetType()), static_cast<uint8_t>(GridLengthType::ABSOLUTE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(length.GetValue(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthAbsoluteP(void)
{
  const float pixels = 42.0f;
  GridLength length = GridLength::Absolute(pixels);
  DALI_TEST_EQUALS(static_cast<uint8_t>(length.GetType()), static_cast<uint8_t>(GridLengthType::ABSOLUTE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(length.GetValue(), pixels, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthStarP(void)
{
  GridLength length = GridLength::Star();
  DALI_TEST_EQUALS(static_cast<uint8_t>(length.GetType()), static_cast<uint8_t>(GridLengthType::STAR), TEST_LOCATION);
  DALI_TEST_EQUALS(length.GetValue(), 1.0f, TEST_LOCATION);
  GridLength length2 = GridLength::Star(2.5f);
  DALI_TEST_EQUALS(length2.GetValue(), 2.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthAutoP(void)
{
  GridLength length = GridLength::Auto();
  DALI_TEST_EQUALS(static_cast<uint8_t>(length.GetType()), static_cast<uint8_t>(GridLengthType::AUTO), TEST_LOCATION);
  DALI_TEST_EQUALS(length.GetValue(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutAlignmentValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(LayoutAlignment::FILL), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(LayoutAlignment::START), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(LayoutAlignment::CENTER), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(LayoutAlignment::END), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackOrientationValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(StackOrientation::VERTICAL), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(StackOrientation::HORIZONTAL), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutFlagsValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::NONE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL), 2u, TEST_LOCATION);
  AbsoluteLayoutFlags all = AbsoluteLayoutFlags::POSITION_PROPORTIONAL | AbsoluteLayoutFlags::SIZE_PROPORTIONAL;
  DALI_TEST_EQUALS(static_cast<uint8_t>(all), 3u, TEST_LOCATION);
  DALI_TEST_CHECK((all & AbsoluteLayoutFlags::POSITION_PROPORTIONAL) == AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
  END_TEST;
}

int UtcDaliFlexDirectionValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexDirection::ROW), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexDirection::ROW_REVERSE), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexDirection::COLUMN), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexDirection::COLUMN_REVERSE), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexWrapValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexWrap::NO_WRAP), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexWrap::WRAP), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexWrap::WRAP_REVERSE), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexJustifyValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexJustify::FLEX_START), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexJustify::SPACE_BETWEEN), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexJustify::SPACE_EVENLY), 5u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexAlignValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexAlign::AUTO), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(FlexAlign::STRETCH), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthTypeValuesP(void)
{
  DALI_TEST_EQUALS(static_cast<uint8_t>(GridLengthType::ABSOLUTE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(GridLengthType::STAR), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(GridLengthType::AUTO), 2u, TEST_LOCATION);
  END_TEST;
}

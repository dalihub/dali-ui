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
#include <dali.h>
#include <type_traits>
#include <utility>

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

int UtcDaliGridLengthValueSemanticsP(void)
{
  GridLength copied;
  GridLength assigned;
  {
    const GridLength original = GridLength::Star(2.5f);
    GridLength       constructed(original);
    copied   = constructed;
    assigned = original;
  }
  DALI_TEST_CHECK(copied.GetType() == GridLengthType::STAR);
  DALI_TEST_EQUALS(copied.GetValue(), 2.5f, TEST_LOCATION);
  copied = GridLength::Absolute(40.0f);
  DALI_TEST_CHECK(assigned.GetType() == GridLengthType::STAR);
  DALI_TEST_EQUALS(assigned.GetValue(), 2.5f, TEST_LOCATION);
  const GridLength& self = assigned;
  assigned               = self;

  GridLength moved(std::move(copied));
  DALI_TEST_EQUALS(copied.GetValue(), 40.0f, TEST_LOCATION);
  DALI_TEST_CHECK(copied.GetType() == GridLengthType::ABSOLUTE);
  copied = GridLength::Auto();
  DALI_TEST_EQUALS(moved.GetValue(), 40.0f, TEST_LOCATION);
  DALI_TEST_CHECK(copied.GetType() == GridLengthType::AUTO);
  GridLength moveAssigned;
  moveAssigned = std::move(moved);
  moved        = assigned;
  DALI_TEST_CHECK(moveAssigned.GetType() == GridLengthType::ABSOLUTE);
  DALI_TEST_EQUALS(moveAssigned.GetValue(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetValue(), 2.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLengthVectorValueSemanticsP(void)
{
  static_assert(std::is_trivially_copyable<GridLength>::value, "Grid lengths must retain scalar copy semantics");
  static_assert(std::is_trivially_destructible<GridLength>::value, "Grid lengths must not own dynamic storage");
  DALI_TEST_CHECK(Dali::Vector<GridLength>::BaseType);
  Dali::Vector<GridLength> lengths;
  lengths.Reserve(1u);
  {
    const GridLength source = GridLength::Star(2.0f);
    lengths.PushBack(source);
  }
  lengths.PushBack(GridLength::Absolute(40.0f));
  lengths.PushBack(GridLength::Auto());
  lengths.Reserve(32u);

  Dali::Vector<GridLength> copied(lengths);
  Dali::Vector<GridLength> assigned;
  assigned   = lengths;
  lengths[0] = GridLength::Absolute(80.0f);
  lengths.Release();
  DALI_TEST_CHECK(copied[0].GetType() == GridLengthType::STAR);
  DALI_TEST_EQUALS(copied[0].GetValue(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copied[1].GetValue(), 40.0f, TEST_LOCATION);
  copied.Erase(copied.Begin() + 1);
  DALI_TEST_EQUALS(copied.Size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(copied[1].GetType() == GridLengthType::AUTO);
  DALI_TEST_EQUALS(assigned.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(assigned[1].GetValue(), 40.0f, TEST_LOCATION);

  // A temporary fill value must initialize every new slot.
  copied.Resize(4u, GridLength::Star(3.0f));
  DALI_TEST_EQUALS(copied[2].GetValue(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copied[3].GetValue(), 3.0f, TEST_LOCATION);
  copied[2] = GridLength::Auto();
  DALI_TEST_CHECK(copied[3].GetType() == GridLengthType::STAR);
  copied.Clear();
  DALI_TEST_EQUALS(assigned[0].GetValue(), 2.0f, TEST_LOCATION);
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
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::X_PROPORTIONAL), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::Y_PROPORTIONAL), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL), 12u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayoutFlags::ALL), 15u, TEST_LOCATION);

  AbsoluteLayoutFlags combined =
    AbsoluteLayoutFlags::POSITION_PROPORTIONAL | AbsoluteLayoutFlags::SIZE_PROPORTIONAL;
  DALI_TEST_EQUALS(static_cast<uint8_t>(combined), 15u, TEST_LOCATION);
  DALI_TEST_CHECK((combined & AbsoluteLayoutFlags::X_PROPORTIONAL) == AbsoluteLayoutFlags::X_PROPORTIONAL);
  DALI_TEST_CHECK((combined & AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL) == AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL);

  // Per-axis mix: X proportional + absolute Y/size
  AbsoluteLayoutFlags xOnly = AbsoluteLayoutFlags::X_PROPORTIONAL;
  DALI_TEST_CHECK((xOnly & AbsoluteLayoutFlags::X_PROPORTIONAL) == AbsoluteLayoutFlags::X_PROPORTIONAL);
  DALI_TEST_CHECK((xOnly & AbsoluteLayoutFlags::Y_PROPORTIONAL) == AbsoluteLayoutFlags::NONE);
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

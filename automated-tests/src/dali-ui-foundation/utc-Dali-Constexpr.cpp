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
#include <dali-test-suite-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/view-state.h>

namespace
{
constexpr bool CheckValues()
{
  using namespace Dali::Ui;
  MeasuredSize       empty;
  const MeasuredSize size(3.f, 4.f);
  if(size.GetWidth() != 3.f || size.GetHeight() != 4.f || size.ToVector2().x != 3.f || empty.width != 0.f)
  {
    return false;
  }
  LayoutRect       zero;
  const LayoutRect rectangle(5.f, 6.f, 7.f, 8.f);
  if(rectangle.GetX() != 5.f || rectangle.GetY() != 6.f || rectangle.GetWidth() != 7.f || rectangle.GetHeight() != 8.f ||
     rectangle.GetPosition().y != 6.f || rectangle.GetSize().x != 7.f || zero.x != 0.f)
  {
    return false;
  }
  const ViewState normal;
  const ViewState all = ~normal;
  if(!normal.IsNormal() || !static_cast<bool>(all) || all == normal || !(all != normal) ||
     !all.Contains(normal) || normal.HasIntersectionWith(all) || !all.HasIntersectionWith(all) ||
     (all + normal) != all || (all - all) != normal || (all | normal) != all ||
     (all & normal) != normal || (all ^ all) != normal ||
     !all.WasAdded(normal, all) || !all.WasRemoved(all, normal) || !all.WasChanged(normal, all) ||
     !all.AnyChanged(normal, all))
  {
    return false;
  }
  const auto flags = AbsoluteLayoutFlags::X_PROPORTIONAL | AbsoluteLayoutFlags::Y_PROPORTIONAL;
  if((flags & AbsoluteLayoutFlags::POSITION_PROPORTIONAL) != AbsoluteLayoutFlags::POSITION_PROPORTIONAL)
  {
    return false;
  }
  const GridLength emptyLength;
  const auto       absolute  = GridLength::Absolute(10.f);
  const auto       star      = GridLength::Star(2.f);
  const auto       automatic = GridLength::Auto();
  const auto       pixel     = LayoutBoundsLength::Pixel(-2.f);
  const auto       self      = LayoutBoundsLength::SelfFraction(0.5f);
  const auto       parent    = LayoutBoundsLength::ParentFraction(1.f);
  return emptyLength.GetValue() == 0.f && absolute.GetType() == GridLengthType::ABSOLUTE && absolute.GetValue() == 10.f &&
         star.GetType() == GridLengthType::STAR && star.GetValue() == 2.f && automatic.GetType() == GridLengthType::AUTO &&
         pixel.value == -2.f && pixel.unit == LayoutBoundsUnit::PIXEL && self.unit == LayoutBoundsUnit::SELF_FRACTION &&
         parent.unit == LayoutBoundsUnit::PARENT_FRACTION;
}
bool CheckMutableValues()
{
  using namespace Dali::Ui;
  MeasuredSize empty;
  MeasuredSize size(1.f, 2.f);
  size.SetWidth(3.f);
  size.SetHeight(4.f);
  if(size.GetWidth() != 3.f || size.GetHeight() != 4.f || size.ToVector2().x != 3.f || empty.width != 0.f)
  {
    return false;
  }
  LayoutRect zero;
  LayoutRect rectangle(1.f, 2.f, 3.f, 4.f);
  rectangle.SetX(5.f);
  rectangle.SetY(6.f);
  rectangle.SetWidth(7.f);
  rectangle.SetHeight(8.f);
  if(rectangle.GetX() != 5.f || rectangle.GetY() != 6.f || rectangle.GetWidth() != 7.f || rectangle.GetHeight() != 8.f ||
     rectangle.GetPosition().y != 6.f || rectangle.GetSize().x != 7.f || zero.x != 0.f)
  {
    return false;
  }
  return true;
}
static_assert(CheckValues(), "Value operations must support C++17 constant evaluation");
} //namespace

int UtcDaliUiConstexprValueTypes(void)
{
  DALI_TEST_CHECK(CheckValues());
  DALI_TEST_CHECK(CheckMutableValues());
  END_TEST;
}

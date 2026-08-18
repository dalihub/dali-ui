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

#include <stdlib.h>
#include <iostream>

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_view_order_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_order_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// The View sibling-order operations (Raise/Lower/RaiseToTop/LowerToBottom/
// RaiseAbove/LowerBelow with LayoutOrderPolicy, and Insert) affect TWO
// orders: the visual/actor z-order and the internal LOGICAL layout order
// (ViewImpl::mChildren). Now that the public View::IndexOfChild has been
// removed, the logical order is only observable internally, so these tests
// live in the internal suite and query it through GetImpl(view).IndexOfChildView().
// Visual order is still queried through the inherited Dali::Actor accessors.
namespace
{
struct OrderFixture
{
  View parent;
  View a;
  View b;
  View c;
};

OrderFixture MakeOrderFixture()
{
  OrderFixture f;
  f.parent = View::New();
  f.a      = View::New();
  f.b      = View::New();
  f.c      = View::New();
  f.parent.Add(f.a);
  f.parent.Add(f.b);
  f.parent.Add(f.c);
  return f;
}

// Actor sibling order (visual z-order).
uint32_t VisualIndexOf(View parent, View child)
{
  Dali::Actor parentActor = parent;
  for(uint32_t i = 0; i < parentActor.GetChildCount(); ++i)
  {
    if(parentActor.GetChildAt(i) == static_cast<Dali::Actor>(child))
    {
      return i;
    }
  }
  return static_cast<uint32_t>(-1);
}

// Logical (layout) sibling order, via the internal ViewImpl accessor that
// backs the public View::IndexOfChildView.
int32_t LogicalIndexOf(View parent, View child)
{
  return GetImpl(parent).IndexOfChildView(child);
}
} // namespace

int UtcDaliViewRaiseUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // Initial: [a, b, c] in both visual and logical order.
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 1u, TEST_LOCATION);

  // Default policy (UPDATE): both visual and logical order change.
  f.b.Raise(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaisePreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // PRESERVE: only visual order changes, logical order stays.
  f.b.Raise(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.b.Lower(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerPreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.b.Lower(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseToTopUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseToTop(): a moves to top -> [b, c, a]
  f.a.RaiseToTop(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseToTopPreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.a.RaiseToTop(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerToBottomUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerToBottom(): c moves to bottom -> [c, a, b]
  f.c.LowerToBottom(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerToBottomPreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.c.LowerToBottom(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseAboveUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseAbove(c): a moves above c -> visual [b, c, a]
  f.a.RaiseAbove(f.c, LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseAbovePreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.a.RaiseAbove(f.c, LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerBelowUpdateInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerBelow(a): c moves below a -> visual [c, a, b]
  f.c.LowerBelow(f.a, LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerBelowPreserveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.c.LowerBelow(f.a, LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertReordersMChildrenInternalP(void)
{
  // Insert of an existing child to a different index must reorder mChildren
  // so that order-sensitive layouts iterate children in the requested order.
  // Self().Add is a no-op in dali-core when the child is already under this
  // parent, so the reorder happens entirely inside Insert's reorder path.
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  View        c      = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 2, TEST_LOCATION);

  parent.Insert(0, c); // move c from index 2 to index 0

  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  END_TEST;
}

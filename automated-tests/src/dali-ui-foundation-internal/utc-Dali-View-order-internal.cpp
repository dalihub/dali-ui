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

// The sibling-order operations usable on a View (the inherited
// Dali::Actor::Raise/Lower/RaiseToTop/LowerToBottom/RaiseAbove/LowerBelow,
// and Dali::Actor::InsertAbove/InsertBelow) affect TWO orders: the
// visual/actor z-order and the internal LOGICAL layout order
// (ViewImpl::mChildren). Now that the public
// View::IndexOfChild has been removed, the logical order is only observable
// internally, so these tests live in the internal suite and query it through
// GetImpl(view).IndexOfChildView().
// Visual order is still queried through the inherited Dali::Actor accessors.
// Dali::Actor::SetDepthIndex is the draw-order-only counterpart: it must leave
// both the sibling order and the logical layout order untouched.
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

int UtcDaliViewRaiseInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // Initial: [a, b, c] in both visual and logical order.
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 1u, TEST_LOCATION);

  // The inherited Actor::Raise changes the visual order; the resulting
  // ChildOrderChangedSignal keeps the logical order in sync.
  f.b.Raise();

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.b.Lower();

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseToTopInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseToTop(): a moves to top -> [b, c, a]
  f.a.RaiseToTop();

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerToBottomInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerToBottom(): c moves to bottom -> [c, a, b]
  f.c.LowerToBottom();

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRaiseAboveInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseAbove(c): a moves above c -> visual [b, c, a]
  f.a.RaiseAbove(f.c);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLowerBelowInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerBelow(a): c moves below a -> visual [c, a, b]
  f.c.LowerBelow(f.a);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewDepthIndexDoesNotChangeChildOrderInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // Actor::Property::DEPTH_INDEX only changes the draw (and hit) order, so
  // neither the actor sibling order nor the logical layout order may move,
  // i.e. OnChildOrderChanged must not fire.
  f.b.SetDepthIndex(10);

  DALI_TEST_EQUALS(f.b.GetDepthIndex(), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowReordersMChildrenInternalP(void)
{
  // Moving an existing child to a different sibling index must reorder
  // mChildren so that order-sensitive layouts iterate children in the
  // requested order. The child is already under this parent, so dali-core
  // takes its sibling-reorder path and emits ChildOrderChangedSignal; that
  // signal is what resynchronises mChildren with the new actor order.
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

  parent.InsertBelow(c, a); // move c from index 2 to index 0

  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowFreshChildInternalP(void)
{
  // Inserting a FRESH child at a logical index needs the Add first. A bare
  // InsertBelow takes dali-core's "not yet our child" path, which puts the
  // child at the right ACTOR index but only fires OnChildAdd -- appending it
  // to mChildren -- and emits no ChildOrderChangedSignal, leaving the logical
  // order out of sync. The Add makes the child "already ours" so the
  // following InsertBelow is a sibling reorder and both orders agree.
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  parent.Add(a);
  parent.Add(b);

  View d = View::New();
  parent.Add(d);
  parent.InsertBelow(d, a);

  DALI_TEST_EQUALS(LogicalIndexOf(parent, d), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, d), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertAboveMovesForwardInternalP(void)
{
  // Moving an existing child TOWARDS THE BACK (current index j < target index
  // i) is the InsertAbove direction. dali-core erases the child before it
  // locates the anchor, so everything after j shifts left by one and the
  // anchor taken at i then sits at i-1; inserting ABOVE it lands the child on
  // i. Using InsertBelow here would be a silent off-by-one.
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  View        c      = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  // Move a from index 0 to index 2: j = 0 < i = 2.
  View anchor = parent.GetChildViewAt(2u);
  parent.InsertAbove(a, anchor);

  // Both orders must agree on the requested index.
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, c), 1u, TEST_LOCATION);
  END_TEST;
}

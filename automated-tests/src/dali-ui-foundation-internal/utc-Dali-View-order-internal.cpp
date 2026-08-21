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
#include <dali-ui-foundation/integration-api/view-integ.h>
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

// The sibling-order operations usable on a View (the View
// Raise/Lower/RaiseToTop/LowerToBottom/RaiseAbove/LowerBelow overloads taking
// LayoutOrderPolicy, the inherited Dali::Actor forms,
// and Dali::Actor::InsertAbove/InsertBelow) affect TWO orders: the
// visual/actor z-order and the internal LOGICAL layout order
// (ViewImpl::mChildren). Now that the public
// View::IndexOfChild has been removed, the logical order is only observable
// internally, so these tests live in the internal suite and query it through
// GetImpl(view).IndexOfChildView().
// Visual order is still queried through the inherited Dali::Actor accessors.
// Dali::Actor::SetDepthIndex is the draw-order-only counterpart: it must leave
// both the sibling order and the logical layout order untouched.
// Reordering an EXISTING child emits ChildOrderChangedSignal, which rebuilds
// the logical order from the actor order. Inserting a FRESH child emits no
// such signal (the child had no previous order), so OnChildAdd itself derives
// the logical index from the child's final actor position -- skipping non-View
// actor children and in-flight EXIT ghosts. These tests pin both orders for
// both paths.
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

int UtcDaliViewInsertBelowFreshChildAfterAddInternalP(void)
{
  // The two-step Add + InsertBelow form stays supported: the Add appends
  // (both orders end with the child at the tail) and the following InsertBelow
  // makes the child "already ours", so dali-core takes its sibling-reorder
  // path and emits ChildOrderChangedSignal, which rebuilds mChildren from the
  // actor order. Both orders agree, exactly as with the one-call form below.
  // The difference is that only this form tags the siblings as reordered.
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

int UtcDaliViewInsertBelowFreshChildDirectInternalP(void)
{
  // A FRESH child inserted with a single InsertBelow -- no Add first. Only
  // OnChildAdd fires (a child with no previous order emits no
  // ChildOrderChangedSignal), so OnChildAdd is what has to place the child at
  // the logical index matching its actor index.
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  parent.Add(a);
  parent.Add(b);

  View d = View::New();
  parent.InsertBelow(d, a);

  // Both orders must agree for every child.
  DALI_TEST_EQUALS(LogicalIndexOf(parent, d), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, d), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, a), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertAboveFreshChildDirectInternalP(void)
{
  // The InsertAbove direction of the one-call fresh insert, plus the tail
  // case served by the append shortcut: inserting above the LAST child must
  // land on the tail in both orders (both index branches agree on this value,
  // so this guards the result rather than which branch ran).
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  parent.Add(a);
  parent.Add(b);

  View d = View::New();
  parent.InsertAbove(d, a); // [a, d, b]

  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, d), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, a), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, d), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, b), 2u, TEST_LOCATION);

  // Fresh insert above the last child -> the tail (append-shortcut case).
  View e = View::New();
  parent.InsertAbove(e, b); // [a, d, b, e]

  DALI_TEST_EQUALS(LogicalIndexOf(parent, e), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(parent, e), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowFreshChildSkipsExitGhostInternalP(void)
{
  // An in-flight EXIT ghost is still an actor child but is no longer a
  // logical child, so it must not shift the logical index of a fresh insert.
  // The actor index and the logical index legitimately diverge here.
  UiTestApplication application;

  View parent = View::New();
  application.GetWindow().Add(parent);

  View a = View::New();
  View b = View::New();
  View c = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);
  application.SendNotification();
  application.Render(0);

  LayoutTransition  transition = LayoutTransition::New();
  ViewAnimationSpec exitSpec   = ViewAnimationSpec::New();
  exitSpec.Opacity(0.0f, Duration(0.5f));
  transition.SetExitVisualSpec(exitSpec);
  parent.SetLayoutTransition(transition);

  // a becomes a ghost: dropped from the logical children, kept under the
  // parent Actor until the EXIT animation finishes. The long duration and
  // Render(0) keep it in flight.
  parent.Remove(a, RemovePolicy::ANIMATE_EXIT);
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS(static_cast<Dali::Actor>(parent).GetChildCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 2u, TEST_LOCATION);

  // Actor order becomes [ghost a, d, b, c]; logical order must be [d, b, c].
  View d = View::New();
  parent.InsertBelow(d, b);

  DALI_TEST_EQUALS(LogicalIndexOf(parent, d), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 3u, TEST_LOCATION);

  // The ghost still occupies actor index 0, so the two orders differ by one.
  DALI_TEST_EQUALS(VisualIndexOf(parent, d), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::Actor>(parent).GetChildCount(), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowFreshChildSkipsRawActorInternalP(void)
{
  // A raw (non-View) actor child attached through the Integration helper is
  // never tracked as a logical child, so it must not shift the logical index
  // of a fresh insert either.
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::VERTICAL);
  View        a      = View::New();
  View        b      = View::New();
  parent.Add(a);
  Dali::Ui::Integration::View::AddActorChild(parent, Dali::Actor::New());
  parent.Add(b);

  DALI_TEST_EQUALS(static_cast<Dali::Actor>(parent).GetChildCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 2u, TEST_LOCATION);

  // Actor order becomes [a, raw, d, b]; logical order must be [a, d, b].
  View d = View::New();
  parent.InsertBelow(d, b);

  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, d), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 3u, TEST_LOCATION);

  // The raw actor still occupies actor index 1, so the two orders differ.
  DALI_TEST_EQUALS(VisualIndexOf(parent, d), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<Dali::Actor>(parent).GetChildCount(), 4u, TEST_LOCATION);
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

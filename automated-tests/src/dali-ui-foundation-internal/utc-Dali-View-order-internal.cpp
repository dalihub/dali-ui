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
#include <initializer_list>
#include <iostream>

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/type-registry.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;

void utc_dali_view_order_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_order_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// The sibling-order operations usable on a View -- the inherited Dali::Actor
// Raise/Lower/RaiseToTop/LowerToBottom/RaiseAbove/LowerBelow and
// InsertAbove/InsertBelow, the Extension::View ...PreservingLayout()
// functions, and the deprecated View overloads taking a LayoutOrderPolicy --
// affect TWO orders: the actor (sibling) order, read through
// Dali::Actor::GetChildAt, and the LOGICAL (layout) order, read through
// View::GetChildViewAt / View::IndexOfChildView (ViewDataImpl::mChildren).
// Dali::Actor::SetDepthIndex is the draw-order-only counterpart: it must leave
// both orders untouched.
// Reordering an EXISTING child emits ChildOrderChangedSignal, which rebuilds
// the logical order from the actor order. Inserting a FRESH child emits no
// such signal (the child had no previous order), so OnChildAdd itself places
// the child at the logical index matching its final actor position --
// skipping non-View actor children and in-flight EXIT ghosts -- and, unless
// the child ended up as the last actor child, then rebuilds the whole logical
// order from the actor order. The ...PreservingLayout() functions, which the
// deprecated PRESERVE overloads call, change the actor order only: the parent
// ignores the ChildOrderChangedSignal of that one call. These tests pin both
// orders for every path.
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

// Actor sibling order (the GetChildAt index). It equals the draw order only
// while every sibling has the same DEPTH_INDEX.
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

// True when the actor (sibling) order of @p parent is exactly @p expected.
bool ActorOrderIs(Dali::Actor parent, std::initializer_list<Dali::Actor> expected)
{
  if(parent.GetChildCount() != expected.size())
  {
    return false;
  }
  uint32_t index = 0u;
  for(const Dali::Actor& child : expected)
  {
    if(parent.GetChildAt(index++) != child)
    {
      return false;
    }
  }
  return true;
}

// True when the logical (layout) order of @p parent is exactly @p expected,
// read through both View::GetChildViewAt and View::IndexOfChildView.
bool LogicalOrderIs(View parent, std::initializer_list<View> expected)
{
  if(parent.GetChildViewCount() != expected.size())
  {
    return false;
  }
  uint32_t index = 0u;
  for(const View& child : expected)
  {
    if(parent.GetChildViewAt(index) != child || parent.IndexOfChildView(child) != static_cast<int32_t>(index))
    {
      return false;
    }
    ++index;
  }
  return true;
}

// Drives one full layout batch to completion.
void SettleLayout(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
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
  uint32_t          orderChangedCount = 0u; // Declared before the fixture so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  // Actor::Property::DEPTH_INDEX only changes the draw (and hit) order, so
  // neither the actor sibling order nor the logical layout order may move,
  // and no ChildOrderChangedSignal (hence no OnChildOrderChanged) may fire.
  f.b.SetDepthIndex(10);

  DALI_TEST_EQUALS(f.b.GetDepthIndex(), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 1, TEST_LOCATION);
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

  // The orders agreed apart from the ghost, so the insert tags nothing REORDERED.
  DALI_TEST_CHECK(ViewDataImpl::Get(GetImpl(parent)).TakePendingLayoutTransitionChanges().reorderedChildren.empty());
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

namespace
{
// The third-party mistake, reproduced exactly: OnInitialize overridden, base NOT called.
class NonChainingViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<NonChainingViewImpl> New()
  {
    return IntrusivePtr<NonChainingViewImpl>(new NonChainingViewImpl());
  }

  int measureCount{0};

protected:
  NonChainingViewImpl()
  : ViewImpl()
  {
  }

  void OnInitialize() override
  {
    // Deliberately empty, and deliberately does NOT call ViewImpl::OnInitialize().
  }

  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override
  {
    ++measureCount;
    return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
  }
};

Dali::TypeRegistration nonChainingViewTypeReg(typeid(NonChainingViewImpl), typeid(ViewImpl), nullptr);

// Mirrors View::New() exactly, including the explicit second-phase Initialize() that
// wraps the impl in a handle first -- that call is what a third-party factory makes.
// The hook under test is no longer connected there at all: it is made lazily, at the
// first tracked (View) child add, in Internal::ViewDataImpl::OnChildAdded. Either way
// the non-chaining OnInitialize() above must not be able to forfeit it, which is what
// the test below asserts.
View CreateNonChainingView()
{
  IntrusivePtr<NonChainingViewImpl> impl = NonChainingViewImpl::New();
  View                              handle(*impl);
  impl->Initialize();
  return handle;
}

NonChainingViewImpl& NonChainingImplOf(View view)
{
  return static_cast<NonChainingViewImpl&>(GetImpl(view));
}
} // namespace

int UtcDaliViewChildOrderHookSurvivesNonChainingOnInitializeInternalP(void)
{
  UiTestApplication application;

  View  parent     = CreateNonChainingView();
  auto* parentImpl = &NonChainingImplOf(parent);
  parent.SetRequestedWidth(WRAP_CONTENT);
  parent.SetRequestedHeight(WRAP_CONTENT);

  View a = View::New();
  View b = View::New();
  View c = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 2, TEST_LOCATION);

  // Settle the measure cache, then confirm it really is a cache.
  parent.Measure(1000.0f, 1000.0f);
  const int settledCount = parentImpl->measureCount;
  parent.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(parentImpl->measureCount, settledCount, TEST_LOCATION);

  // Reorder through the inherited Actor call; the logical resync relies
  // solely on the child-order-changed connection.
  Dali::Actor(a).RaiseToTop();

  // The logical order followed the actor order...
  DALI_TEST_EQUALS(LogicalIndexOf(parent, a), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, b), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(parent, c), 1, TEST_LOCATION);

  // ...and the measure cache was invalidated with it, so the next measure is real work
  // rather than a replay of the old order.
  parent.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(parentImpl->measureCount, settledCount + 1, TEST_LOCATION);

  END_TEST;
}

// The same guarantee for a plain View, so the test above is comparing against a known
// baseline rather than describing behaviour unique to the subclass.
int UtcDaliViewChildOrderHookOnPlainViewInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  Dali::Actor(f.a).RaiseToTop();

  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.a), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.b), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(LogicalIndexOf(f.parent, f.c), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaisePreservingLayoutInternalP(void)
{
  // The view moves one step up in the actor order only; the logical order
  // stays, and ChildOrderChangedSignal is still emitted for the move.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixture so they outlive the connected parent.
  Dali::Actor       movedChild;
  OrderFixture      f = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount, &movedChild](Dali::Actor, Dali::Actor child)
  {
    ++orderChangedCount;
    movedChild = child;
  });

  Ui::Extension::View::RaisePreservingLayout(f.a);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  DALI_TEST_EQUALS(orderChangedCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(movedChild == f.a);
  END_TEST;
}

int UtcDaliViewLowerPreservingLayoutInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c moves one step down in the actor order only.
  Ui::Extension::View::LowerPreservingLayout(f.c);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.a, f.c, f.b}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  END_TEST;
}

int UtcDaliViewRaiseToTopPreservingLayoutInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a moves to the top of the actor order only.
  Ui::Extension::View::RaiseToTopPreservingLayout(f.a);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  END_TEST;
}

int UtcDaliViewLowerToBottomPreservingLayoutInternalP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c moves to the bottom of the actor order only.
  Ui::Extension::View::LowerToBottomPreservingLayout(f.c);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.c, f.a, f.b}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  END_TEST;
}

int UtcDaliViewRaiseAbovePreservingLayoutInternalP(void)
{
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixtures so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();
  OrderFixture      g                 = MakeOrderFixture();

  // a moves above c in the actor order only.
  Ui::Extension::View::RaiseAbovePreservingLayout(f.a, f.c);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));

  // c is already the top-most child, so it stays where it is and dali-core
  // reports nothing.
  DevelActor::ChildOrderChangedSignal(g.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  Ui::Extension::View::RaiseAbovePreservingLayout(g.c, g.a);

  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.a, g.b, g.c}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b, g.c}));
  END_TEST;
}

int UtcDaliViewLowerBelowPreservingLayoutInternalP(void)
{
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixtures so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();
  OrderFixture      g                 = MakeOrderFixture();

  // c moves below a in the actor order only.
  Ui::Extension::View::LowerBelowPreservingLayout(f.c, f.a);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.c, f.a, f.b}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));

  // a is already the bottom-most child, so it stays where it is and dali-core
  // reports nothing.
  DevelActor::ChildOrderChangedSignal(g.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  Ui::Extension::View::LowerBelowPreservingLayout(g.a, g.c);

  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.a, g.b, g.c}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b, g.c}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutEmptyTargetInternalP(void)
{
  // An empty target is ignored, as the deprecated overloads do (the inherited
  // Actor calls assert on one): no reorder, no signal.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixture so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  Ui::Extension::View::RaiseAbovePreservingLayout(f.a, View());
  Ui::Extension::View::LowerBelowPreservingLayout(f.c, View());

  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.a, f.b, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutNonViewParentInternalP(void)
{
  // A parent that is not a View has no logical order to keep, so each
  // function is exactly the inherited Actor call.
  UiTestApplication application;

  Dali::Actor parent = Dali::Actor::New();
  View        a      = View::New();
  View        b      = View::New();
  View        c      = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  Ui::Extension::View::RaisePreservingLayout(a);
  DALI_TEST_CHECK(ActorOrderIs(parent, {b, a, c}));
  Ui::Extension::View::LowerPreservingLayout(a);
  DALI_TEST_CHECK(ActorOrderIs(parent, {a, b, c}));
  Ui::Extension::View::RaiseToTopPreservingLayout(a);
  DALI_TEST_CHECK(ActorOrderIs(parent, {b, c, a}));
  Ui::Extension::View::LowerToBottomPreservingLayout(a);
  DALI_TEST_CHECK(ActorOrderIs(parent, {a, b, c}));
  Ui::Extension::View::RaiseAbovePreservingLayout(a, c);
  DALI_TEST_CHECK(ActorOrderIs(parent, {b, c, a}));
  Ui::Extension::View::LowerBelowPreservingLayout(a, b);
  DALI_TEST_CHECK(ActorOrderIs(parent, {a, b, c}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutNoParentInternalP(void)
{
  // Without a parent each function is the inherited Actor call, which only
  // logs that the view has no parent: no assertion and no crash.
  UiTestApplication application;

  View view   = View::New();
  View target = View::New();

  Ui::Extension::View::RaisePreservingLayout(view);
  Ui::Extension::View::LowerPreservingLayout(view);
  Ui::Extension::View::RaiseToTopPreservingLayout(view);
  Ui::Extension::View::LowerToBottomPreservingLayout(view);
  Ui::Extension::View::RaiseAbovePreservingLayout(view, target);
  Ui::Extension::View::LowerBelowPreservingLayout(view, target);

  DALI_TEST_CHECK(!view.GetParent());
  DALI_TEST_CHECK(!target.GetParent());
  END_TEST;
}

int UtcDaliViewDeprecatedPreserveMatchesPreservingLayoutInternalP(void)
{
  // The deprecated LayoutOrderPolicy::PRESERVE overloads call the
  // ...PreservingLayout() functions, so the same sequence gives the same
  // actor order after every step and leaves both logical orders alone.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture(); // Deprecated overloads.
  OrderFixture      g = MakeOrderFixture(); // Extension::View functions.

  f.a.Raise(LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::RaisePreservingLayout(g.a);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a, f.c}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, g.a, g.c}));

  f.c.Lower(LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::LowerPreservingLayout(g.c);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, g.c, g.a}));

  // a is already the top-most child: nothing moves.
  f.a.RaiseToTop(LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::RaiseToTopPreservingLayout(g.a);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, g.c, g.a}));

  f.c.LowerToBottom(LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::LowerToBottomPreservingLayout(g.c);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.c, f.b, f.a}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.c, g.b, g.a}));

  f.b.RaiseAbove(f.a, LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::RaiseAbovePreservingLayout(g.b, g.a);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.c, f.a, f.b}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.c, g.a, g.b}));

  f.a.LowerBelow(f.c, LayoutOrderPolicy::PRESERVE);
  Ui::Extension::View::LowerBelowPreservingLayout(g.a, g.c);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.a, f.c, f.b}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.a, g.c, g.b}));

  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b, g.c}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutKeepsMeasureCleanInternalP(void)
{
  // A ...PreservingLayout() call leaves the parent's measure and arrange clean
  // (no layout pass is requested), while the inherited Actor reorder that later
  // makes the logical order follow the actor order invalidates the measure.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);
  root.Add(f.parent);
  SettleLayout(application);

  ViewDataImpl& data = ViewDataImpl::Get(GetImpl(f.parent));
  DALI_TEST_CHECK(!data.IsMeasureDirty());
  DALI_TEST_CHECK(!data.IsArrangeDirty());

  Ui::Extension::View::RaiseToTopPreservingLayout(f.a);

  DALI_TEST_CHECK(!data.IsMeasureDirty());
  DALI_TEST_CHECK(!data.IsArrangeDirty());
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));

  // Actor order [b, c, a] -> [c, b, a], and the logical order follows it.
  f.c.Lower();

  DALI_TEST_CHECK(data.IsMeasureDirty());
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.c, f.b, f.a}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutActorReorderSynchronisesInternalP(void)
{
  // A reorder that dali-core reports through ChildOrderChangedSignal makes the
  // logical order follow the actor order again, which undoes the earlier
  // ...PreservingLayout() call.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();
  OrderFixture      g = MakeOrderFixture();

  Ui::Extension::View::RaiseToTopPreservingLayout(f.a); // Actor order [b, c, a].
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));

  // The inherited Actor::Lower(): actor order [b, c, a] -> [c, b, a].
  f.c.Lower();

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.c, f.b, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.c, f.b, f.a}));

  Ui::Extension::View::RaisePreservingLayout(g.a); // Actor order [b, a, c].
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b, g.c}));

  // A SIBLING_ORDER write, which dali-core turns into LowerToBottom():
  // actor order [b, a, c] -> [c, b, a].
  g.c.SetProperty(DevelActor::Property::SIBLING_ORDER, 0);

  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.c, g.b, g.a}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.c, g.b, g.a}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutInsertExistingSynchronisesInternalP(void)
{
  // InsertAbove / InsertBelow of a child the parent already has always reports
  // the reorder, even when the child stays where it is, so the logical order
  // follows the actor order afterwards.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixtures so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();
  OrderFixture      g                 = MakeOrderFixture();

  // c moves above b: actor order [b, a, c] -> [b, c, a].
  Ui::Extension::View::RaisePreservingLayout(f.a);
  f.parent.InsertAbove(f.c, f.b);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, f.c, f.a}));

  // a is already directly above c, so the actor order stays [b, c, a].
  Ui::Extension::View::RaiseToTopPreservingLayout(g.a);
  DevelActor::ChildOrderChangedSignal(g.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });
  g.parent.InsertAbove(g.a, g.c);

  DALI_TEST_EQUALS(orderChangedCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, g.c, g.a}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.b, g.c, g.a}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutFreshMidInsertSynchronisesInternalP(void)
{
  // A fresh InsertAbove / InsertBelow that does not leave the child last makes
  // the WHOLE logical order follow the actor order, so it undoes an earlier
  // ...PreservingLayout() call. Placing only the newcomer would give
  // [a, d, b, c] for the first case.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();
  OrderFixture      g = MakeOrderFixture();

  Ui::Extension::View::RaisePreservingLayout(f.a); // Actor order [b, a, c].
  View d = View::New();
  f.parent.InsertAbove(d, f.b);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, d, f.a, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, d, f.a, f.c}));
  DALI_TEST_EQUALS(f.parent.GetChildViewCount(), 4u, TEST_LOCATION);

  Ui::Extension::View::RaiseToTopPreservingLayout(g.a); // Actor order [b, c, a].
  View e = View::New();
  g.parent.InsertBelow(e, g.c);

  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, e, g.c, g.a}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.b, e, g.c, g.a}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutMidInsertFromOtherParentInternalP(void)
{
  // A view taken from another parent is a child the parent does not have yet:
  // dali-core first removes it from its old parent, whose logical order drops
  // it, and a mid insert then makes the new parent's whole logical order follow
  // its actor order, as for a brand-new view.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();
  OrderFixture      g = MakeOrderFixture();

  Ui::Extension::View::RaisePreservingLayout(f.a); // f actor order [b, a, c].
  f.parent.InsertAbove(g.c, f.b);                  // f actor order [b, g.c, a, c].

  DALI_TEST_CHECK(g.c.GetParent() == f.parent);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, g.c, f.a, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, g.c, f.a, f.c}));
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.a, g.b}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutAppendDoesNotSynchroniseInternalP(void)
{
  // Actor::Add, and a fresh insert that leaves the child as the last actor
  // child (which cannot be told apart from Add), append the child to both
  // orders without synchronising them.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();
  OrderFixture      g = MakeOrderFixture();

  Ui::Extension::View::RaisePreservingLayout(f.a); // Actor order [b, a, c].
  View d = View::New();
  f.parent.Add(d);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a, f.c, d}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c, d}));

  Ui::Extension::View::RaiseToTopPreservingLayout(g.a); // Actor order [b, c, a].
  View e = View::New();
  g.parent.InsertAbove(e, g.a); // Above the top-most child: e ends up last.

  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.b, g.c, g.a, e}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.b, g.c, e}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutNoSignalAtEndInternalP(void)
{
  // dali-core reports nothing for Raise / RaiseToTop / RaiseAbove of the
  // top-most child or Lower / LowerToBottom / LowerBelow of the bottom-most
  // one, so such a call does not make the logical order follow the actor order.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixture so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();

  Ui::Extension::View::RaiseToTopPreservingLayout(f.a); // Actor order [b, c, a].

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  // a is the top-most child and b the bottom-most one.
  f.a.RaiseToTop();
  f.a.Raise();
  f.a.RaiseAbove(f.b);
  f.b.Lower();
  f.b.LowerToBottom();
  f.b.LowerBelow(f.c);

  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutReportWithoutMoveSynchronisesInternalP(void)
{
  // RaiseAbove / LowerBelow with a sibling target report the call even when
  // the child is already above / below the target and does not move, unless
  // the child is the top-most / bottom-most one, so the logical order follows
  // the actor order afterwards.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixtures so it outlives the connected parents.
  OrderFixture      f                 = MakeOrderFixture();
  OrderFixture      g                 = MakeOrderFixture();

  Ui::Extension::View::RaisePreservingLayout(f.a); // Actor order [b, a, c].

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  // a is already above b and is not the top-most child.
  f.a.RaiseAbove(f.b);

  DALI_TEST_EQUALS(orderChangedCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, f.a, f.c}));

  Ui::Extension::View::LowerPreservingLayout(g.c); // Actor order [a, c, b].

  DevelActor::ChildOrderChangedSignal(g.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  // c is already below b and is not the bottom-most child.
  g.c.LowerBelow(g.b);

  DALI_TEST_EQUALS(orderChangedCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(g.parent, {g.a, g.c, g.b}));
  DALI_TEST_CHECK(LogicalOrderIs(g.parent, {g.a, g.c, g.b}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutNonSiblingTargetInternalP(void)
{
  // With a target that is not a sibling (RaiseAbove / LowerBelow) or not a
  // child of the parent (InsertAbove / InsertBelow), dali-core changes and
  // reports nothing -- a fresh child is not even added -- so the logical order
  // stays as the earlier ...PreservingLayout() call left it.
  UiTestApplication application;
  uint32_t          orderChangedCount = 0u; // Declared before the fixture so it outlives the connected parent.
  OrderFixture      f                 = MakeOrderFixture();
  View              outsider          = View::New();
  View              e                 = View::New();

  Ui::Extension::View::RaiseToTopPreservingLayout(f.a); // Actor order [b, c, a].

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&orderChangedCount](Dali::Actor, Dali::Actor)
  { ++orderChangedCount; });

  f.b.RaiseAbove(outsider);
  f.c.LowerBelow(outsider);
  f.parent.InsertAbove(f.b, outsider);
  f.parent.InsertBelow(f.c, outsider);
  f.parent.InsertAbove(e, outsider);

  DALI_TEST_EQUALS(orderChangedCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c}));
  DALI_TEST_CHECK(!e.GetParent());
  DALI_TEST_EQUALS(f.parent.GetChildViewCount(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPreservingLayoutRemoveDoesNotSynchroniseInternalP(void)
{
  // Removing a child takes it out of both orders; the remaining children keep
  // their logical order.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  Ui::Extension::View::RaiseToTopPreservingLayout(f.a); // Actor order [b, c, a].
  f.parent.Remove(f.c);

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b}));
  DALI_TEST_EQUALS(f.parent.IndexOfChildView(f.c), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPreservingLayoutTracksChildAddedInCallbackInternalP(void)
{
  // The guard of a ...PreservingLayout() call suppresses the reorder handling
  // only: a child that a ChildOrderChangedSignal callback adds meanwhile is
  // still tracked as a logical child.
  UiTestApplication application;
  bool              added = false; // Declared before the fixture so they outlive the connected parent.
  View              d     = View::New();
  OrderFixture      f     = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&added, &d, &f](Dali::Actor, Dali::Actor)
  {
    if(!added)
    {
      added = true;
      f.parent.Add(d);
    }
  });

  // Actor order [b, c, a], then the callback appends d.
  Ui::Extension::View::RaiseToTopPreservingLayout(f.a);

  DALI_TEST_CHECK(added);
  DALI_TEST_EQUALS(f.parent.GetChildViewCount(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChildView(d), 3, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a, d}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b, f.c, d}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutTracksChildRemovedInCallbackInternalP(void)
{
  // ...and a child that such a callback removes leaves the logical order.
  UiTestApplication application;
  bool              removed = false; // Declared before the fixture so it outlives the connected parent.
  OrderFixture      f       = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&removed, &f](Dali::Actor, Dali::Actor)
  {
    if(!removed)
    {
      removed = true;
      f.parent.Remove(f.c);
    }
  });

  // Actor order [b, c, a], then the callback removes c.
  Ui::Extension::View::RaiseToTopPreservingLayout(f.a);

  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!f.c.GetParent());
  DALI_TEST_EQUALS(f.parent.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChildView(f.c), -1, TEST_LOCATION);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, f.b}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutFreshMidInsertInCallbackSynchronisesInternalP(void)
{
  // A fresh insert that does not leave the child last synchronises the whole
  // order even when a ChildOrderChangedSignal callback makes it while a
  // ...PreservingLayout() call is running, which undoes that call at once.
  UiTestApplication application;
  bool              inserted = false; // Declared before the fixture so they outlive the connected parent.
  View              d        = View::New();
  OrderFixture      f        = MakeOrderFixture();

  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&inserted, &d, &f](Dali::Actor, Dali::Actor)
  {
    if(!inserted)
    {
      inserted = true;
      f.parent.InsertBelow(d, f.b);
    }
  });

  // Actor order [b, c, a], then the callback's insert gives [d, b, c, a].
  Ui::Extension::View::RaiseToTopPreservingLayout(f.a);

  DALI_TEST_CHECK(inserted);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {d, f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {d, f.b, f.c, f.a}));
  END_TEST;
}

int UtcDaliViewPreservingLayoutNestedGuardInternalP(void)
{
  // A ...PreservingLayout() call made from a ChildOrderChangedSignal callback
  // while another one on a child of the same parent is running restores the
  // flag it found, so the parent's own handler, which runs after the callback,
  // still ignores the outer report. Once the outer call returns, a reported
  // reorder makes the logical order follow the actor order again.
  UiTestApplication application;
  bool              nested = false; // Declared before the views so it outlives the connected parent.
  View              parent = View::New();
  View              a      = View::New();
  View              b      = View::New();
  View              c      = View::New();

  // Connected before the first View child is added, which is when the parent
  // connects its own handler; handlers run in the order they were connected.
  DevelActor::ChildOrderChangedSignal(parent).Connect(&application, [&nested, &b](Dali::Actor, Dali::Actor)
  {
    if(!nested)
    {
      nested = true;
      // Actor order [b, c, a] -> [c, b, a]; DALi drops this nested report.
      Ui::Extension::View::RaisePreservingLayout(b);
    }
  });
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  Ui::Extension::View::RaiseToTopPreservingLayout(a); // Actor order [a, b, c] -> [b, c, a].

  DALI_TEST_CHECK(nested);
  DALI_TEST_CHECK(ActorOrderIs(parent, {c, b, a}));
  DALI_TEST_CHECK(LogicalOrderIs(parent, {a, b, c}));

  // Actor order [c, b, a] -> [c, a, b], reported to the parent's handler.
  a.Lower();

  DALI_TEST_CHECK(ActorOrderIs(parent, {c, a, b}));
  DALI_TEST_CHECK(LogicalOrderIs(parent, {c, a, b}));
  END_TEST;
}

int UtcDaliViewReorderInChildOrderChangedCallbackInternalP(void)
{
  // A reorder made from a ChildOrderChangedSignal callback of the same parent
  // is not reported: DALi drops an Emit() made while the same signal is being
  // emitted. Whether the logical order still picks the reorder up depends on
  // the order of the handlers. A callback connected after the parent's first
  // View child runs after the parent's own handler, so the logical order misses
  // the nested reorder; one connected before runs first, so the parent's
  // rebuild for the outer report already sees it.
  UiTestApplication application;
  bool              lateDone  = false; // Declared before the views so they outlive the connected parents.
  bool              earlyDone = false;
  OrderFixture      f         = MakeOrderFixture();
  View              parent    = View::New();
  View              a         = View::New();
  View              b         = View::New();
  View              c         = View::New();

  // Connected after the parent's own handler.
  DevelActor::ChildOrderChangedSignal(f.parent).Connect(&application, [&lateDone, &f](Dali::Actor, Dali::Actor)
  {
    if(!lateDone)
    {
      lateDone = true;
      f.c.Lower(); // Actor order [b, a, c] -> [b, c, a], not reported.
    }
  });

  // Actor order [a, b, c] -> [b, a, c]: the parent's handler rebuilds the
  // logical order to [b, a, c] before the callback moves c.
  f.a.Raise();

  DALI_TEST_CHECK(lateDone);
  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.b, f.c, f.a}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, f.a, f.c}));

  // Connected before the first View child is added, which is when the parent
  // connects its own handler.
  DevelActor::ChildOrderChangedSignal(parent).Connect(&application, [&earlyDone, &c](Dali::Actor, Dali::Actor)
  {
    if(!earlyDone)
    {
      earlyDone = true;
      c.Lower(); // Actor order [b, a, c] -> [b, c, a], not reported.
    }
  });
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  // Actor order [a, b, c] -> [b, a, c]; the callback runs first, and the
  // parent's handler then rebuilds from [b, c, a].
  a.Raise();

  DALI_TEST_CHECK(earlyDone);
  DALI_TEST_CHECK(ActorOrderIs(parent, {b, c, a}));
  DALI_TEST_CHECK(LogicalOrderIs(parent, {b, c, a}));
  END_TEST;
}

int UtcDaliViewFreshMidInsertSyncTagsReorderedInternalP(void)
{
  // When a fresh mid insert makes pre-existing children move in the logical
  // order, every logical child is tagged REORDERED for the attached
  // LayoutTransition, as for a reported reorder; the newcomer also keeps its
  // ENTER marker. The ...PreservingLayout() call before it tags nothing.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.parent.SetLayoutTransition(LayoutTransition::New());
  ViewDataImpl& data = ViewDataImpl::Get(GetImpl(f.parent));
  data.TakePendingLayoutTransitionChanges(); // Drops the ENTER markers the attach seeds for a, b and c.

  Ui::Extension::View::RaisePreservingLayout(f.a); // Actor order [b, a, c].
  auto afterPreserve = data.TakePendingLayoutTransitionChanges();

  DALI_TEST_CHECK(afterPreserve.reorderedChildren.empty());
  DALI_TEST_CHECK(afterPreserve.enterChildren.empty());

  View d = View::New();
  f.parent.InsertAbove(d, f.b); // Actor order [b, d, a, c].
  auto afterInsert = data.TakePendingLayoutTransitionChanges();

  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.b, d, f.a, f.c}));
  DALI_TEST_EQUALS(afterInsert.reorderedChildren.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.reorderedChildren.count(&GetImpl(f.a)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.reorderedChildren.count(&GetImpl(f.b)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.reorderedChildren.count(&GetImpl(f.c)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.reorderedChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterInsert.enterChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewFreshMidInsertInSyncDoesNotTagReorderedInternalP(void)
{
  // When the two orders agree, a fresh mid insert already lands where the sync
  // would put it, so nothing is tagged REORDERED: the result is the same as
  // placing the newcomer alone, with only its ENTER marker.
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.parent.SetLayoutTransition(LayoutTransition::New());
  ViewDataImpl& data = ViewDataImpl::Get(GetImpl(f.parent));
  data.TakePendingLayoutTransitionChanges(); // Drops the ENTER markers the attach seeds for a, b and c.

  View d = View::New();
  f.parent.InsertAbove(d, f.a);
  auto changes = data.TakePendingLayoutTransitionChanges();

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {f.a, d, f.b, f.c}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {f.a, d, f.b, f.c}));
  DALI_TEST_CHECK(changes.reorderedChildren.empty());
  DALI_TEST_EQUALS(changes.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.enterChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowFreshChildAtFrontRepeatedInternalP(void)
{
  // A fresh InsertBelow() in front of the first child, repeated. The newcomer is never the last
  // actor child, so OnChildAdded synchronises the whole order every time; as the two orders agree,
  // SyncChildrenWithActorOrder() finds nothing to change. Every newcomer lands at the front of
  // both orders, nothing is tagged REORDERED and each insert records only the newcomer's ENTER
  // marker. The raw actor child behind the views stays untracked.
  UiTestApplication application;
  OrderFixture      f   = MakeOrderFixture();
  Dali::Actor       raw = Dali::Actor::New();
  Dali::Ui::Integration::View::AddActorChild(f.parent, raw); // Actor order [a, b, c, raw].

  f.parent.SetLayoutTransition(LayoutTransition::New());
  ViewDataImpl& data = ViewDataImpl::Get(GetImpl(f.parent));
  data.TakePendingLayoutTransitionChanges(); // Drops the ENTER markers the attach seeds for a, b and c.

  View d = View::New();
  f.parent.InsertBelow(d, f.a); // Actor order [d, a, b, c, raw].
  auto afterD = data.TakePendingLayoutTransitionChanges();

  View e = View::New();
  f.parent.InsertBelow(e, d); // Actor order [e, d, a, b, c, raw].
  auto afterE = data.TakePendingLayoutTransitionChanges();

  View g = View::New();
  f.parent.InsertBelow(g, e); // Actor order [g, e, d, a, b, c, raw].
  auto afterG = data.TakePendingLayoutTransitionChanges();

  DALI_TEST_CHECK(ActorOrderIs(f.parent, {g, e, d, f.a, f.b, f.c, raw}));
  DALI_TEST_CHECK(LogicalOrderIs(f.parent, {g, e, d, f.a, f.b, f.c}));
  DALI_TEST_CHECK(afterD.reorderedChildren.empty());
  DALI_TEST_CHECK(afterE.reorderedChildren.empty());
  DALI_TEST_CHECK(afterG.reorderedChildren.empty());
  DALI_TEST_EQUALS(afterD.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterD.enterChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterE.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterE.enterChildren.count(&GetImpl(e)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterG.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(afterG.enterChildren.count(&GetImpl(g)), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertBelowFreshChildAtFrontResyncSkipsUntrackedInternalP(void)
{
  // A fresh InsertBelow() in front of the first child of a parent whose orders a
  // ...PreservingLayout() call left apart: the logical order is rebuilt from the actor order. The
  // rebuild keeps only logical children, so neither a raw actor child nor an in-flight EXIT ghost
  // gets in, and every logical child -- not the ghost -- is tagged REORDERED.
  UiTestApplication application;

  View parent = View::New();
  application.GetWindow().Add(parent);

  View        a   = View::New();
  View        b   = View::New();
  View        c   = View::New();
  Dali::Actor raw = Dali::Actor::New();
  parent.Add(a);
  Dali::Ui::Integration::View::AddActorChild(parent, raw);
  parent.Add(b);
  parent.Add(c);
  application.SendNotification();
  application.Render(0);

  LayoutTransition  transition = LayoutTransition::New();
  ViewAnimationSpec exitSpec   = ViewAnimationSpec::New();
  exitSpec.Opacity(0.0f, Duration(0.5f));
  transition.SetExitVisualSpec(exitSpec);
  parent.SetLayoutTransition(transition);

  // c becomes a ghost: dropped from the logical children, kept under the parent Actor until the
  // EXIT animation finishes. The long duration and Render(0) keep it in flight.
  parent.Remove(c, RemovePolicy::ANIMATE_EXIT);
  application.SendNotification();
  application.Render(0);

  Ui::Extension::View::RaiseToTopPreservingLayout(a); // Actor order [raw, b, ghost c, a]; logical order [a, b].
  ViewDataImpl& data = ViewDataImpl::Get(GetImpl(parent));
  data.TakePendingLayoutTransitionChanges(); // Drops the markers left by the attach and the removal.

  View d = View::New();
  parent.InsertBelow(d, raw); // Actor order [d, raw, b, ghost c, a].
  auto changes = data.TakePendingLayoutTransitionChanges();

  DALI_TEST_CHECK(ActorOrderIs(parent, {d, raw, b, c, a}));
  DALI_TEST_CHECK(LogicalOrderIs(parent, {d, b, a}));
  DALI_TEST_EQUALS(parent.IndexOfChildView(c), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.reorderedChildren.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.reorderedChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.reorderedChildren.count(&GetImpl(b)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.reorderedChildren.count(&GetImpl(a)), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.enterChildren.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes.enterChildren.count(&GetImpl(d)), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewChildRemovedDuringSetParentNotTrackedInternalP(void)
{
  // dali-core reports an add to OnChildAdd even when a callback run from
  // Actor::SetParent() has already removed the child again -- here the child's
  // SceneConnectedSignal, emitted while it is parented under an on-scene
  // parent. Such a child is no actor child any more, so the parent must not
  // track it: no logical child and no ENTER marker. A later add is tracked as
  // usual.
  UiTestApplication application;
  bool              removed = false; // Declared before the views so it outlives the connected child.
  View              parent  = View::New();
  application.GetWindow().Add(parent);
  parent.SetLayoutTransition(LayoutTransition::New());
  ViewDataImpl& parentData = ViewDataImpl::Get(GetImpl(parent));

  View child = View::New();
  child.SceneConnectedSignal().Connect(&application, [&removed, &parent](Dali::Actor actor)
  {
    if(!removed)
    {
      removed = true;
      parent.Remove(actor);
    }
  });

  parent.Add(child);

  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!child.GetParent());
  DALI_TEST_EQUALS(static_cast<Dali::Actor>(parent).GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChildView(child), -1, TEST_LOCATION);
  DALI_TEST_CHECK(parentData.TakePendingLayoutTransitionChanges().enterChildren.empty());

  // The callback has disarmed itself, so this add goes through.
  parent.Add(child);

  DALI_TEST_CHECK(child.GetParent() == parent);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChildView(child), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(parentData.TakePendingLayoutTransitionChanges().enterChildren.count(&GetImpl(child)), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewFreshInsertWhileChildInTransitDropsItInternalP(void)
{
  // dali-core takes a removed child out of the actor children BEFORE the callbacks of
  // Actor::SetParent(nullptr) run and reports the removal to OnChildRemove only after them, so
  // in such a callback the departing child is still a logical child but no actor child. A fresh
  // insert in front made there makes the logical order follow the actor order, which drops the
  // departing child at once (the known gap documented at SyncChildrenWithActorOrder()).
  UiTestApplication application;
  bool              inserted        = false; // Declared before the views so they outlive the connected child.
  uint32_t          countInCallback = 0u;
  int32_t           indexInCallback = 0;
  View              parent          = View::New();
  application.GetWindow().Add(parent);

  View a = View::New();
  View b = View::New();
  View c = View::New();
  View d = View::New();
  parent.Add(a);
  parent.Add(b);
  parent.Add(c);

  c.SceneDisconnectedSignal().Connect(&application, [&inserted, &countInCallback, &indexInCallback, &parent, &a, &d](Dali::Actor departing)
  {
    if(!inserted)
    {
      inserted = true;
      parent.InsertBelow(d, a); // Actor order [d, a, b]: c has left the actor children, not yet mChildren.
      countInCallback = parent.GetChildViewCount();
      indexInCallback = parent.IndexOfChildView(View::DownCast(departing));
    }
  });

  parent.Remove(c);

  DALI_TEST_CHECK(inserted);
  DALI_TEST_EQUALS(countInCallback, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(indexInCallback, -1, TEST_LOCATION);
  DALI_TEST_CHECK(!c.GetParent());
  DALI_TEST_CHECK(ActorOrderIs(parent, {d, a, b}));
  DALI_TEST_CHECK(LogicalOrderIs(parent, {d, a, b}));
  END_TEST;
}

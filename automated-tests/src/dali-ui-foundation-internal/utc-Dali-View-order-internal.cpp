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
#include <dali/devel-api/object/type-registry.h>

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

// INC-I / N5. ViewImpl::OnInitialize() is virtual, so a third-party subclass that
// overrides it without up-calling used to silently lose the child-order-changed hook:
// the connection was made from inside OnInitialize(). It now lives in the non-virtual
// Initialize(), alongside the layout-direction connection that was moved there earlier
// for exactly this reason.
//
// Losing the hook is worse than a stale mChildren order. OnChildOrderChanged also
// invalidates measure, so without it the measure and arrange caches stay valid and the
// settled subtree is replayed at the OLD order instead of being laid out again.
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
// wraps the impl in a handle first -- that call is what a third-party factory makes,
// and it is where the hook under test is now connected.
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

  // Reorder at the ACTOR level, bypassing the View sibling-order API. This is the only
  // path that depends on the child-order-changed connection.
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

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
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/integration-api/events/key-event-integ.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{
// Dummy trait implementation for testing
class DummyTraitImpl : public TraitImpl
{
public:
  DummyTraitImpl()
    : mAttachedCount(0),
      mDetachedCount(0),
      mBeforeAttachedCount(0),
      mViewDestroyingCount(0)
  {
  }

  void OnBeforeAttached(TraitId id, View& view) override
  {
    mBeforeAttachedCount++;
  }

  void OnAttached(TraitId id, View& view) override
  {
    mAttachedCount++;
  }

  void OnDetached(TraitId id, View& view) override
  {
    mDetachedCount++;
  }

  void OnViewDestroying(ViewImpl* viewImpl) override
  {
    mViewDestroyingCount++;
  }

  int GetAttachedCount() const
  {
    return mAttachedCount;
  }
  int GetDetachedCount() const
  {
    return mDetachedCount;
  }
  int GetBeforeAttachedCount() const
  {
    return mBeforeAttachedCount;
  }
  int GetViewDestroyingCount() const
  {
    return mViewDestroyingCount;
  }

private:
  int mAttachedCount;
  int mDetachedCount;
  int mBeforeAttachedCount;
  int mViewDestroyingCount;
};

class DummyTrait : public Trait
{
public:
  static DummyTrait New()
  {
    return DummyTrait(new DummyTraitImpl());
  }

  DummyTrait() = default;

  DummyTraitImpl& GetImpl()
  {
    return static_cast<DummyTraitImpl&>(Dali::Ui::GetImpl(*this));
  }

  const DummyTraitImpl& GetImpl() const
  {
    return static_cast<const DummyTraitImpl&>(Dali::Ui::GetImpl(*this));
  }

private:
  explicit DummyTrait(DummyTraitImpl* impl)
    : Trait(impl)
  {
  }
};

// Test-only trait IDs (allocated once, reused across tests)
static const TraitId TEST_TRAIT_ID_0 = TraitId::Alloc();
static const TraitId TEST_TRAIT_ID_1 = TraitId::Alloc();

} // namespace

void utc_dali_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewConstructorP(void)
{
  UiTestApplication application;
  View view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewNewP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewCopyConstructorP(void)
{
  UiTestApplication application;
  View view = View::New();
  View copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveConstructor(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  View moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  View view = View::New();
  View copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveAssignment(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  View moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewDownCastP(void)
{
  UiTestApplication application;
  View view = View::New();
  BaseHandle object(view);
  View view2 = View::DownCast(object);
  View view3 = DownCast<View>(object);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view3);
  END_TEST;
}

int UtcDaliViewDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  View view1 = View::DownCast(unInitializedObject);
  View view2 = DownCast<View>(unInitializedObject);
  DALI_TEST_CHECK(!view1);
  DALI_TEST_CHECK(!view2);
  END_TEST;
}

int UtcDaliViewGetSizeWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testWidth = 100.0f;

  view.SetRequestedWidth(testWidth);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetSizeHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testHeight = 200.0f;

  view.SetRequestedHeight(testHeight);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionXP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testX = 50.0f;

  view.SetPositionX(testX);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionYP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testY = 75.0f;

  view.SetPositionY(testY);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeWidthChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testWidth = 150.0f;

  View& result = view.SetRequestedWidth(testWidth);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeHeightChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testHeight = 250.0f;

  View& result = view.SetRequestedHeight(testHeight);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionXChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testX = 125.0f;

  View& result = view.SetPositionX(testX);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionYChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testY = 175.0f;

  View& result = view.SetPositionY(testY);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBackgroundColorChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const UiColor testColor(1.0f, 0.0f, 0.0f, 0.5f);

  View& result = view.SetBackgroundColor(testColor);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMultipleChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testWidth = 300.0f;
  const float testHeight = 200.0f;
  const float testX = 100.0f;
  const float testY = 50.0f;
  const UiColor testColor(0.0f, 1.0f, 0.0f, 0.8f);

  View& result = view.SetRequestedWidth(testWidth)
                     .SetRequestedHeight(testHeight)
                     .SetPositionX(testX)
                     .SetPositionY(testY)
                     .SetBackgroundColor(testColor);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAsP(void)
{
  UiTestApplication application;
  View view = View::New();
  View reference;

  View& result = view.As(reference);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(reference == view);
  END_TEST;
}

int UtcDaliViewWithP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testWidth = 400.0f;
  bool actionCalled = false;

  View& result = view.With(
      [&actionCalled, testWidth](View& v)
      {
        v.SetRequestedWidth(testWidth);
        actionCalled = true;
      });

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(actionCalled);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetParentOriginP(void)
{
  UiTestApplication application;
  View view = View::New();

  Vector3 parentOrigin = view.GetParentOrigin();
  DALI_TEST_EQUALS(parentOrigin.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetParentOriginP(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector3 testOrigin(0.0f, 1.0f, 0.5f);

  View& result = view.SetParentOrigin(testOrigin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector3 parentOrigin = view.GetParentOrigin();
  DALI_TEST_EQUALS(parentOrigin.x, testOrigin.x, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.y, testOrigin.y, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.z, testOrigin.z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewGetPivotP(void)
{
  UiTestApplication application;
  View view = View::New();

  Vector3 pivot = view.GetPivot();
  DALI_TEST_EQUALS(pivot.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetPivotP(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector3 testPivot(1.0f, 0.0f, 0.5f);

  View& result = view.SetPivot(testPivot);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector3 pivot = view.GetPivot();
  DALI_TEST_EQUALS(pivot.x, testPivot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.y, testPivot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.z, testPivot.z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewParentOriginChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector3 testOrigin(0.0f, 0.0f, 0.0f);

  View& result = view.SetParentOrigin(testOrigin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewPivotChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector3 testPivot(1.0f, 1.0f, 1.0f);

  View& result = view.SetPivot(testPivot);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetTraitP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);

  Dali::BaseHandle retrievedTrait = IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(retrievedTrait);
  DALI_TEST_CHECK(retrievedTrait == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetTraitP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);

  Dali::BaseHandle retrievedTrait = IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(retrievedTrait);

  Dali::BaseHandle nonExistentTrait = IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_1);
  DALI_TEST_CHECK(!nonExistentTrait);
  END_TEST;
}

int UtcDaliViewRemoveTraitP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);
  DALI_TEST_CHECK(IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0));

  bool removed = IntegrationView::RemoveTrait(viewImpl,TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0));
  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);

  removed = IntegrationView::RemoveTrait(viewImpl,TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(!removed);
  END_TEST;
}

int UtcDaliViewReplaceTraitP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait1 = DummyTrait::New();
  DummyTrait trait2 = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait1);
  DALI_TEST_CHECK(IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0) == trait1);

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait2);
  DALI_TEST_CHECK(IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0) == trait2);
  DALI_TEST_EQUALS(trait1.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait2.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetSameTraitP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);
  Dali::BaseHandle retrieved1 = IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0);

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);
  Dali::BaseHandle retrieved2 = IntegrationView::GetTrait(viewImpl,TEST_TRAIT_ID_0);

  DALI_TEST_CHECK(retrieved1 == retrieved2);
  DALI_TEST_CHECK(retrieved1 == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitLifecycleP(void)
{
  UiTestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 0, TEST_LOCATION);

  IntegrationView::SetTrait(viewImpl,TEST_TRAIT_ID_0, trait);

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);

  IntegrationView::RemoveTrait(viewImpl,TEST_TRAIT_ID_0);

  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float width = 200.0f;
  view.SetRequestedWidth(width);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), width, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetRequestedWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetRequestedWidth(), WRAP_CONTENT, TEST_LOCATION);
  view.SetRequestedWidth(300.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 300.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float height = 100.0f;
  view.SetRequestedHeight(height);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), height, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetRequestedHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetRequestedHeight(), WRAP_CONTENT, TEST_LOCATION);
  view.SetRequestedHeight(250.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 250.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMarginP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents margin(10, 20, 30, 40);
  view.SetMargin(margin);
  Extents got = view.GetMargin();
  DALI_TEST_EQUALS(got.start, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 30u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 40u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMarginP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents got = view.GetMargin();
  DALI_TEST_EQUALS(got.start, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetPaddingP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents padding(5, 15, 25, 35);
  view.SetPadding(padding);
  Extents got = view.GetPadding();
  DALI_TEST_EQUALS(got.start, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 15u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 25u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 35u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPaddingP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents got = view.GetPadding();
  DALI_TEST_EQUALS(got.start, 0u, TEST_LOCATION);
  END_TEST;
}


int UtcDaliViewLayoutWidthChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetRequestedWidth(150.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetRequestedHeight(80.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(50.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeP(void)
{
  UiTestApplication application;
  View view = View::New();
  LayoutRect bounds(10.0f, 20.0f, 100.0f, 80.0f);
  MeasuredSize size = view.Arrange(bounds);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMeasuredSizeP(void)
{
  UiTestApplication application;
  View view = View::New();
  MeasuredSize desired = view.GetMeasuredSize();
  DALI_TEST_EQUALS(desired.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(desired.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMinimumWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMinimumWidth(50.0f);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMinimumHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMinimumHeight(30.0f);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMaximumWidth(500.0f);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 500.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumWidth(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumHeight(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBackgroundColorP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view);
  UiColor color(0.2f, 0.4f, 0.6f, 1.0f);
  view.SetBackgroundColor(color);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewMarginChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents margin(1, 2, 3, 4);
  View& result = view.SetMargin(margin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMargin().start, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPaddingChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  Extents padding(5, 10, 15, 20);
  View& result = view.SetPadding(padding);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPadding().start, 5u, TEST_LOCATION);
  END_TEST;
}


int UtcDaliViewMinimumWidthChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetMinimumWidth(25.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 25.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMinimumHeightChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetMinimumHeight(35.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 35.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumWidthChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetMaximumWidth(300.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 300.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumHeightChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  View& result = view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureCacheHitP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  MeasuredSize s1 = view.Measure(200.0f, 200.0f);
  MeasuredSize s2 = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(s1.GetWidth(), s2.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(s1.GetHeight(), s2.GetHeight(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWithMarginP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMargin(Extents(10, 10, 10, 10));
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  MeasuredSize size = view.Measure(100.0f, 100.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureMatchParentP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);
  MeasuredSize size = view.Measure(200.0f, 150.0f);
  // MATCH_PARENT reports minimum desired size (0 by default); actual size
  // is determined by the parent during the Arrange phase.
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewApplyConstraintsMinMaxP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetMinimumWidth(80.0f);
  view.SetMaximumWidth(50.0f);
  view.SetMinimumHeight(60.0f);
  view.SetMaximumHeight(40.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutWidthFixedNoManagerP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(120.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightFixedNoManagerP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedHeight(80.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeWithLayoutP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View c1 = View::New();
  c1.SetRequestedWidth(100.0f);
  c1.SetRequestedHeight(50.0f);
  layout.Add(c1);
  View c2 = View::New();
  c2.SetRequestedWidth(100.0f);
  c2.SetRequestedHeight(50.0f);
  layout.Add(c2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(200.0f);
  MeasuredSize measured = layout.Measure(200.0f, 200.0f);
  MeasuredSize arranged = layout.Arrange(LayoutRect(0, 0, 200, 200));
  DALI_TEST_EQUALS(measured.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(arranged.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(arranged.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWithPaddingP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetPadding(Extents(5, 5, 5, 5));
  view.SetRequestedWidth(40.0f);
  view.SetRequestedHeight(30.0f);
  MeasuredSize size = view.Measure(100.0f, 100.0f);
  // Fixed size is total size; padding is inside, not added on top
  DALI_TEST_EQUALS(size.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWrapContentP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  MeasuredSize size = view.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutMatchParentWithManagerP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetRequestedWidth(MATCH_PARENT);
  layout.SetRequestedHeight(MATCH_PARENT);
  layout.Add(View::New().SetRequestedWidth(50.0f).SetRequestedHeight(50.0f));
  MeasuredSize size = layout.Measure(200.0f, 150.0f);
  // MATCH_PARENT layout reports minimum desired size (0 by default);
  // actual size is determined by its parent during the Arrange phase.
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeP2(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(10.0f, 20.0f, 100.0f, 100.0f));
  DALI_TEST_EQUALS(view.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSize().height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeMatchParentP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0, 0, 120.0f, 80.0f));
  DALI_TEST_EQUALS(view.GetSize().width, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSize().height, 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedWidthNoChangeP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedWidthZeroP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(0.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightZeroP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedHeight(0.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureZeroWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(0.0f);
  view.SetRequestedHeight(100.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureZeroHeightP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(0.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureZeroBothP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(0.0f);
  view.SetRequestedHeight(0.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

// Corner Radius API tests (lines 561-632)

int UtcDaliViewGetCornerRadiusP(void)
{
  UiTestApplication application;
  View view = View::New();
  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusUniformP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testRadius = 10.0f;

  View& result = view.SetCornerRadius(testRadius);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, testRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, testRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, testRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, testRadius, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusIndividualP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float topLeft = 5.0f;
  const float topRight = 10.0f;
  const float bottomRight = 15.0f;
  const float bottomLeft = 20.0f;

  View& result = view.SetCornerRadius(topLeft, topRight, bottomRight, bottomLeft);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, topLeft, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, topRight, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, bottomRight, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, bottomLeft, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusVector4P(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector4 testRadius(8.0f, 12.0f, 16.0f, 20.0f);

  View& result = view.SetCornerRadius(testRadius);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, testRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, testRadius.y, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, testRadius.z, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, testRadius.w, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetCornerRadiusPolicyP(void)
{
  UiTestApplication application;
  View view = View::New();
  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::ABSOLUTE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusPolicyP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::RELATIVE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusPolicyRelativeP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetCornerRadiusPolicyRelative();
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::RELATIVE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewIsCornerRadiusPolicyRelativeP(void)
{
  UiTestApplication application;
  View view = View::New();

  DALI_TEST_CHECK(!view.IsCornerRadiusPolicyRelative());

  view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  DALI_TEST_CHECK(view.IsCornerRadiusPolicyRelative());

  view.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  DALI_TEST_CHECK(!view.IsCornerRadiusPolicyRelative());
  END_TEST;
}

int UtcDaliViewCornerRadiusChainingP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetCornerRadius(10.0f)
                     .SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsCornerRadiusPolicyRelative());
  END_TEST;
}

// Corner Squareness API tests

int UtcDaliViewGetCornerSquarenessP(void)
{
  UiTestApplication application;
  View view = View::New();
  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerSquarenessUniformP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testSquareness = 0.5f;

  View& result = view.SetCornerSquareness(testSquareness);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, testSquareness, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, testSquareness, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, testSquareness, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, testSquareness, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerSquarenessIndividualP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float topLeft = 0.2f;
  const float topRight = 0.4f;
  const float bottomRight = 0.6f;
  const float bottomLeft = 0.8f;

  View& result = view.SetCornerSquareness(topLeft, topRight, bottomRight, bottomLeft);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, topLeft, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, topRight, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, bottomRight, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, bottomLeft, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerSquarenessVector4P(void)
{
  UiTestApplication application;
  View view = View::New();
  const Vector4 testSquareness(0.1f, 0.3f, 0.5f, 0.7f);

  View& result = view.SetCornerSquareness(testSquareness);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, testSquareness.x, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, testSquareness.y, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, testSquareness.z, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, testSquareness.w, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewCornerSquarenessChainingP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetCornerSquareness(0.5f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewCornerRadiusAndSquarenessCombinedP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetCornerRadius(15.0f, 20.0f, 25.0f, 30.0f)
                     .SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE)
                     .SetCornerSquareness(0.3f, 0.4f, 0.5f, 0.6f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, 30.0f, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, 0.4f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, 0.6f, TEST_LOCATION);

  DALI_TEST_CHECK(view.IsCornerRadiusPolicyRelative());
  END_TEST;
}

// Borderline Width API tests

int UtcDaliViewGetBorderlineWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  float width = view.GetBorderlineWidth();
  DALI_TEST_EQUALS(width, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineWidthP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testWidth = 5.0f;

  View& result = view.SetBorderlineWidth(testWidth);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  float width = view.GetBorderlineWidth();
  DALI_TEST_EQUALS(width, testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineWidthChainingP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetBorderlineWidth(10.0f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 10.0f, TEST_LOCATION);
  END_TEST;
}

// Borderline Color API tests

int UtcDaliViewGetBorderlineColorP(void)
{
  UiTestApplication application;
  View view = View::New();
  UiColor color = view.GetBorderlineColor();
  Vector4 resolved = color.Resolve();
  // Default color should be black/transparent
  DALI_TEST_EQUALS(resolved.r, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineColorP(void)
{
  UiTestApplication application;
  View view = View::New();
  const UiColor testColor(1.0f, 0.5f, 0.25f, 1.0f);

  View& result = view.SetBorderlineColor(testColor);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  UiColor color = view.GetBorderlineColor();
  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineColorChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  const UiColor testColor(0.2f, 0.4f, 0.6f, 0.8f);

  View& result = view.SetBorderlineColor(testColor);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  UiColor color = view.GetBorderlineColor();
  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 0.2f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.4f, TEST_LOCATION);
  END_TEST;
}

// Borderline Offset API tests

int UtcDaliViewGetBorderlineOffsetP(void)
{
  UiTestApplication application;
  View view = View::New();
  float offset = view.GetBorderlineOffset();
  DALI_TEST_EQUALS(offset, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineOffsetP(void)
{
  UiTestApplication application;
  View view = View::New();
  const float testOffset = 2.5f;

  View& result = view.SetBorderlineOffset(testOffset);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  float offset = view.GetBorderlineOffset();
  DALI_TEST_EQUALS(offset, testOffset, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineOffsetChainingP(void)
{
  UiTestApplication application;
  View view = View::New();

  View& result = view.SetBorderlineOffset(3.0f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 3.0f, TEST_LOCATION);
  END_TEST;
}

// Borderline Combined API tests

int UtcDaliViewBorderlineCombinedP(void)
{
  UiTestApplication application;
  View view = View::New();
  const UiColor testColor(1.0f, 0.0f, 0.0f, 1.0f);

  View& result = view.SetBorderlineWidth(4.0f)
                     .SetBorderlineColor(testColor)
                     .SetBorderlineOffset(1.5f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 1.5f, TEST_LOCATION);

  UiColor color = view.GetBorderlineColor();
  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineWithCornerRadiusCombinedP(void)
{
  UiTestApplication application;
  View view = View::New();
  const UiColor borderColor(0.0f, 1.0f, 0.0f, 1.0f);

  View& result = view.SetCornerRadius(10.0f)
                     .SetCornerSquareness(0.5f)
                     .SetBorderlineWidth(2.0f)
                     .SetBorderlineColor(borderColor)
                     .SetBorderlineOffset(0.5f);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  // Verify all values
  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, 10.0f, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.5f, TEST_LOCATION);

  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 0.5f, TEST_LOCATION);

  UiColor color = view.GetBorderlineColor();
  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.g, 1.0f, TEST_LOCATION);
  END_TEST;
}

// LayoutMode::STANDALONE tests

int UtcDaliViewSetLayoutModeP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::DEFAULT), TEST_LOCATION);
  View& result = view.SetLayoutMode(LayoutMode::STANDALONE);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::STANDALONE), TEST_LOCATION);
  view.SetLayoutMode(LayoutMode::DEFAULT);
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::DEFAULT), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewStandaloneIgnoresParentPaddingMatchParentP(void)
{
  UiTestApplication application;
  View parent = View::New();
  parent.SetPadding(Extents(10, 10, 10, 10));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);

  parent.Measure(200.0f, 150.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  // Standalone child ignores parent padding entirely:
  // size fills the parent edge to edge, position is at (0,0).
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewStandaloneAppliesOwnMarginP(void)
{
  UiTestApplication application;
  View parent = View::New();
  parent.SetPadding(Extents(10, 10, 10, 10));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetMargin(Extents(5, 5, 7, 7));
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);

  parent.Measure(200.0f, 150.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  // Parent padding is ignored; own margin shrinks the size and shifts the position.
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f - 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 150.0f - 14.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionX(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewStandaloneUsesPositionP(void)
{
  UiTestApplication application;
  View parent = View::New();
  parent.SetPadding(Extents(10, 10, 10, 10));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(30.0f);
  child.SetPositionX(50.0f);
  child.SetPositionY(60.0f);
  parent.Add(child);

  parent.Measure(200.0f, 150.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(child.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionX(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 60.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewStandaloneExcludedFromWrapContentP(void)
{
  UiTestApplication application;
  // WRAP_CONTENT parent should ignore the Standalone child when accumulating size.
  View parent = View::New();
  View normal = View::New();
  normal.SetRequestedWidth(40.0f);
  normal.SetRequestedHeight(30.0f);
  parent.Add(normal);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(500.0f);
  standalone.SetRequestedHeight(500.0f);
  standalone.SetPositionX(1000.0f);
  standalone.SetPositionY(1000.0f);
  parent.Add(standalone);

  MeasuredSize size = parent.Measure(800.0f, 800.0f);
  // Only the normal child contributes to WRAP_CONTENT accumulation.
  DALI_TEST_EQUALS(size.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// KeyEventSignal
// =============================================================================

namespace
{

struct KeyEventSignalData
{
  KeyEventSignalData()
  : called(false),
    consumed(false)
  {
  }

  void Reset()
  {
    called   = false;
    consumed = false;
    view     = View();
  }

  bool     called;
  bool     consumed;
  View     view;
  KeyEvent event;
};

struct KeyEventSignalFunctor
{
  KeyEventSignalFunctor(KeyEventSignalData& data, bool consume = true)
  : signalData(data),
    mConsume(consume)
  {
  }

  bool operator()(View view, const KeyEvent& event)
  {
    signalData.called = true;
    signalData.view   = view;
    signalData.event  = event;
    return mConsume;
  }

  KeyEventSignalData& signalData;
  bool                mConsume;
};

struct FocusChangedSignalData
{
  FocusChangedSignalData()
  : called(false),
    focused(false)
  {
  }

  void Reset()
  {
    called  = false;
    focused = false;
    view    = View();
  }

  bool called;
  bool focused;
  View view;
};

struct FocusChangedSignalFunctor
{
  FocusChangedSignalFunctor(FocusChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool focused)
  {
    signalData.called  = true;
    signalData.view    = view;
    signalData.focused = focused;
  }

  FocusChangedSignalData& signalData;
};

View CreateFocusableView(UiTestApplication& application)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetFocusable(true);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  return view;
}

} // namespace

int UtcDaliViewKeyEventSignalP(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  KeyEventSignalData    data;
  KeyEventSignalFunctor functor(data);
  view.KeyEventSignal().Connect(&application, functor);

  // Give focus to the view
  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  // Send key down event
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);

  END_TEST;
}

int UtcDaliViewKeyEventSignalConsumedP(void)
{
  UiTestApplication application;
  View              parent = CreateFocusableView(application);
  View              child  = CreateFocusableView(application);
  parent.Add(child);
  application.SendNotification();
  application.Render();

  // Child consumes the event
  KeyEventSignalData    childData;
  KeyEventSignalFunctor childFunctor(childData, true);
  child.KeyEventSignal().Connect(&application, childFunctor);

  // Parent should NOT receive it
  KeyEventSignalData    parentData;
  KeyEventSignalFunctor parentFunctor(parentData);
  parent.KeyEventSignal().Connect(&application, parentFunctor);

  FocusManager::Get().SetCurrentFocusView(child);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(childData.called);
  DALI_TEST_CHECK(!parentData.called);

  END_TEST;
}

int UtcDaliViewKeyEventSignalNotConsumedP(void)
{
  UiTestApplication application;
  View              parent = CreateFocusableView(application);
  View              child  = CreateFocusableView(application);
  parent.Add(child);
  application.SendNotification();
  application.Render();

  // Child does NOT consume the event
  KeyEventSignalData    childData;
  KeyEventSignalFunctor childFunctor(childData, false);
  child.KeyEventSignal().Connect(&application, childFunctor);

  // Parent should receive it
  KeyEventSignalData    parentData;
  KeyEventSignalFunctor parentFunctor(parentData);
  parent.KeyEventSignal().Connect(&application, parentFunctor);

  FocusManager::Get().SetCurrentFocusView(child);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(childData.called);
  DALI_TEST_CHECK(parentData.called);

  END_TEST;
}

int UtcDaliViewKeyEventSignalWithoutFocusN(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  KeyEventSignalData    data;
  KeyEventSignalFunctor functor(data);
  view.KeyEventSignal().Connect(&application, functor);

  // Do NOT set focus — just send key event directly
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  // Key event should NOT reach the view without focus
  DALI_TEST_CHECK(!data.called);

  END_TEST;
}

int UtcDaliViewKeyEventSignalNotFocusableN(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  // SetFocusable(true) is NOT called
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  KeyEventSignalData    data;
  KeyEventSignalFunctor functor(data);
  view.KeyEventSignal().Connect(&application, functor);

  // Attempt to set focus on a non-focusable view
  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  // Key event should NOT reach a non-focusable view
  DALI_TEST_CHECK(!data.called);

  END_TEST;
}

int UtcDaliViewKeyEventSignalNoConnectionN(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  // No signal connected — should not crash
  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  // Just verify no crash
  DALI_TEST_CHECK(true);

  END_TEST;
}

// =============================================================================
// FocusChangedSignal
// =============================================================================

int UtcDaliViewFocusChangedSignalGainedP(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  FocusChangedSignalData    data;
  FocusChangedSignalFunctor functor(data);
  view.FocusChangedSignal().Connect(&application, functor);

  FocusManager::Get().SetCurrentFocusView(view);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(data.focused == true);

  END_TEST;
}

int UtcDaliViewFocusChangedSignalLostP(void)
{
  UiTestApplication application;
  View              view1 = CreateFocusableView(application);
  View              view2 = CreateFocusableView(application);

  FocusChangedSignalData    data1;
  FocusChangedSignalFunctor functor1(data1);
  view1.FocusChangedSignal().Connect(&application, functor1);

  // Give focus to view1
  FocusManager::Get().SetCurrentFocusView(view1);
  DALI_TEST_CHECK(data1.called);
  DALI_TEST_CHECK(data1.focused == true);

  data1.Reset();

  // Move focus to view2 — view1 should lose focus
  FocusManager::Get().SetCurrentFocusView(view2);

  DALI_TEST_CHECK(data1.called);
  DALI_TEST_CHECK(data1.view == view1);
  DALI_TEST_CHECK(data1.focused == false);

  END_TEST;
}

int UtcDaliViewFocusChangedSignalBothViewsP(void)
{
  UiTestApplication application;
  View              view1 = CreateFocusableView(application);
  View              view2 = CreateFocusableView(application);

  FocusChangedSignalData    data1;
  FocusChangedSignalFunctor functor1(data1);
  view1.FocusChangedSignal().Connect(&application, functor1);

  FocusChangedSignalData    data2;
  FocusChangedSignalFunctor functor2(data2);
  view2.FocusChangedSignal().Connect(&application, functor2);

  // Focus view1
  FocusManager::Get().SetCurrentFocusView(view1);
  DALI_TEST_CHECK(data1.called);
  DALI_TEST_CHECK(data1.focused == true);
  DALI_TEST_CHECK(!data2.called);

  data1.Reset();
  data2.Reset();

  // Move focus to view2
  FocusManager::Get().SetCurrentFocusView(view2);

  // view1 should have lost focus
  DALI_TEST_CHECK(data1.called);
  DALI_TEST_CHECK(data1.focused == false);

  // view2 should have gained focus
  DALI_TEST_CHECK(data2.called);
  DALI_TEST_CHECK(data2.focused == true);

  END_TEST;
}

int UtcDaliViewFocusChangedSignalClearFocusP(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  FocusChangedSignalData    data;
  FocusChangedSignalFunctor functor(data);
  view.FocusChangedSignal().Connect(&application, functor);

  FocusManager::Get().SetCurrentFocusView(view);
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.focused == true);

  data.Reset();

  // Clear focus
  FocusManager::Get().ClearFocus();

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(data.focused == false);

  END_TEST;
}

int UtcDaliViewFocusChangedSignalNoConnectionN(void)
{
  UiTestApplication application;
  View              view = CreateFocusableView(application);

  // No signal connected — should not crash
  FocusManager::Get().SetCurrentFocusView(view);
  FocusManager::Get().ClearFocus();

  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliViewIsOnSceneP(void)
{
  UiTestApplication application;
  View view = View::New();

  DALI_TEST_CHECK(!view.IsOnScene());

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view.IsOnScene());

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!view.IsOnScene());

  END_TEST;
}

// =============================================================================
// SetLeftFocusableView: MoveFocus(LEFT) moves to the designated view
// =============================================================================

int UtcDaliViewSetLeftFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  View& ret = viewA.SetLeftFocusableView(viewB);
  DALI_TEST_CHECK(&ret == &viewA);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewA);

  mgr.MoveFocus(FocusDirection::LEFT);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// SetRightFocusableView: MoveFocus(RIGHT) moves to the designated view
// =============================================================================

int UtcDaliViewSetRightFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetRightFocusableView(viewB);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);

  mgr.MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// SetUpFocusableView: MoveFocus(UP) moves to the designated view
// =============================================================================

int UtcDaliViewSetUpFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetUpFocusableView(viewB);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);

  mgr.MoveFocus(FocusDirection::UP);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// SetDownFocusableView: MoveFocus(DOWN) moves to the designated view
// =============================================================================

int UtcDaliViewSetDownFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetDownFocusableView(viewB);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);

  mgr.MoveFocus(FocusDirection::DOWN);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// SetClockwiseFocusableView: MoveFocus(CLOCKWISE) moves to the designated view
// =============================================================================

int UtcDaliViewSetClockwiseFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetClockwiseFocusableView(viewB);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);

  mgr.MoveFocus(FocusDirection::CLOCKWISE);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// SetCounterClockwiseFocusableView: MoveFocus(COUNTER_CLOCKWISE) moves to the designated view
// =============================================================================

int UtcDaliViewSetCounterClockwiseFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetCounterClockwiseFocusableView(viewB);

  FocusManager mgr = FocusManager::Get();
  mgr.SetCurrentFocusView(viewA);

  mgr.MoveFocus(FocusDirection::COUNTER_CLOCKWISE);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == viewB);

  END_TEST;
}

// =============================================================================
// Chaining: set all four directions and verify focus movement for each
// =============================================================================

int UtcDaliViewSetFocusableViewChainingP(void)
{
  UiTestApplication application;
  View              center = CreateFocusableView(application);
  View              left   = CreateFocusableView(application);
  View              right  = CreateFocusableView(application);
  View              up     = CreateFocusableView(application);
  View              down   = CreateFocusableView(application);

  center.SetLeftFocusableView(left)
        .SetRightFocusableView(right)
        .SetUpFocusableView(up)
        .SetDownFocusableView(down);

  FocusManager mgr = FocusManager::Get();

  // LEFT
  mgr.SetCurrentFocusView(center);
  mgr.MoveFocus(FocusDirection::LEFT);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == left);

  // RIGHT
  mgr.SetCurrentFocusView(center);
  mgr.MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == right);

  // UP
  mgr.SetCurrentFocusView(center);
  mgr.MoveFocus(FocusDirection::UP);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == up);

  // DOWN
  mgr.SetCurrentFocusView(center);
  mgr.MoveFocus(FocusDirection::DOWN);
  DALI_TEST_CHECK(mgr.GetCurrentFocusView() == down);

  END_TEST;
}

namespace
{
// Helpers for sibling-order tests: build a parent with three named children
// and query visual (Actor) vs layout (View) sibling order.
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

// Actor sibling order (visual z-order)
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
} // namespace

int UtcDaliViewRaiseUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // Initial: [a, b, c] in both visual and layout order
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 1u, TEST_LOCATION);

  // Default policy (UPDATE): both visual and layout order change
  f.b.Raise(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.b), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaisePreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // PRESERVE: only visual order changes, layout order stays
  f.b.Raise(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.b), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.b.Lower(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.b), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerPreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.b.Lower(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.b), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.b), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaiseToTopUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseToTop(): a moves to top → [b, c, a]
  f.a.RaiseToTop(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.a), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaiseToTopPreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.a.RaiseToTop(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.a), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerToBottomUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerToBottom(): c moves to bottom → [c, a, b]
  f.c.LowerToBottom(LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.c), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerToBottomPreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.c.LowerToBottom(LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.c), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaiseAboveUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // a.RaiseAbove(c): a moves above c → visual [b, c, a]
  f.a.RaiseAbove(f.c, LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.a), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewRaiseAbovePreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.a.RaiseAbove(f.c, LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.a), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.a), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerBelowUpdateP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  // c.LowerBelow(a): c moves below a → visual [c, a, b]
  f.c.LowerBelow(f.a, LayoutOrderPolicy::UPDATE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.c), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLowerBelowPreserveP(void)
{
  UiTestApplication application;
  OrderFixture      f = MakeOrderFixture();

  f.c.LowerBelow(f.a, LayoutOrderPolicy::PRESERVE);

  DALI_TEST_EQUALS(VisualIndexOf(f.parent, f.c), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(f.parent.IndexOfChild(f.c), 2, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// PropertySetSignal: typed setters that delegate to SetProperty must fire the
// PropertySetSignal exactly like SetProperty does. This is the canonical
// guarantee of the property-system refactor.
// =============================================================================

namespace
{
struct PropertySetRecorder : public Dali::ConnectionTracker
{
  std::vector<Dali::Property::Index> indices;
  std::vector<Dali::Property::Value> values;

  void Connect(Ui::View view)
  {
    Dali::Handle handle = view;
    handle.PropertySetSignal().Connect(this, &PropertySetRecorder::OnSet);
  }

  void OnSet(Dali::Handle& /*handle*/, Dali::Property::Index index, const Dali::Property::Value& value)
  {
    indices.push_back(index);
    values.push_back(value);
  }

  bool Saw(Dali::Property::Index index) const
  {
    return std::find(indices.begin(), indices.end(), index) != indices.end();
  }
};
} // namespace

int UtcDaliViewSetMarginFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  view.SetMargin(Extents(1, 2, 3, 4));

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::MARGIN));
  END_TEST;
}

int UtcDaliViewSetPaddingFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetPadding(Extents(5, 6, 7, 8));

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::PADDING));
  END_TEST;
}

int UtcDaliViewSetRequestedWidthFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetRequestedWidth(120.0f);

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::REQUESTED_WIDTH));
  END_TEST;
}

int UtcDaliViewSetMinimumWidthFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetMinimumWidth(10.0f);

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::MINIMUM_WIDTH));
  END_TEST;
}

int UtcDaliViewSetLayoutModeFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetLayoutMode(Ui::LayoutMode::STANDALONE);

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::LAYOUT_MODE));
  END_TEST;
}

int UtcDaliViewSetKeyNavigationSupportFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetKeyNavigationSupport(true);

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::KEY_NAVIGATION_SUPPORT));
  END_TEST;
}

int UtcDaliViewTypedSetterAndSetPropertyConvergeP(void)
{
  // Both entry points must reach the same final state.
  UiTestApplication application;

  Ui::View viewA = Ui::View::New();
  viewA.SetMargin(Extents(7, 8, 9, 10));

  Ui::View viewB = Ui::View::New();
  Dali::Handle(viewB).SetProperty(Ui::View::Property::MARGIN, Extents(7, 8, 9, 10));

  DALI_TEST_EQUALS(viewA.GetMargin(), viewB.GetMargin(), TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Handle(viewA).GetProperty<Extents>(Ui::View::Property::MARGIN),
                   Dali::Handle(viewB).GetProperty<Extents>(Ui::View::Property::MARGIN),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInsertReordersMChildrenP(void)
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
  DALI_TEST_EQUALS(parent.IndexOfChild(a), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChild(b), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChild(c), 2, TEST_LOCATION);

  parent.Insert(0, c); // move c from index 2 to index 0

  DALI_TEST_EQUALS(parent.IndexOfChild(c), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChild(a), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.IndexOfChild(b), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetLayoutDirectionP(void)
{
  UiTestApplication application;
  View view = View::New();
  application.GetScene().Add(view);

  view.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(view.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsLayoutDirectionInherited(), false, TEST_LOCATION);

  view.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_EQUALS(view.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsLayoutDirectionInherited(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewClearLayoutDirectionP(void)
{
  UiTestApplication application;
  View view = View::New();
  application.GetScene().Add(view);

  view.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(view.IsLayoutDirectionInherited(), false, TEST_LOCATION);

  view.ClearLayoutDirection();
  DALI_TEST_EQUALS(view.IsLayoutDirectionInherited(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewIsLayoutDirectionInheritedDefaultP(void)
{
  UiTestApplication application;
  View view = View::New();

  DALI_TEST_EQUALS(view.IsLayoutDirectionInherited(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetEffectiveLayoutDirectionInheritedP(void)
{
  UiTestApplication application;
  View parent = View::New();
  View child  = View::New();
  application.GetScene().Add(parent);
  parent.Add(child);

  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(child.IsLayoutDirectionInherited(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionChainingP(void)
{
  UiTestApplication application;
  View view = View::New();
  application.GetScene().Add(view);

  View& ref = view.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(&ref, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  END_TEST;
}

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
#include <limits>
#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-test-suite-utils.h>
#include <dali/integration-api/events/key-event-integ.h>

using namespace Dali;
using namespace Dali::UI;
using namespace Dali::UI::Integration;

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
    return static_cast<DummyTraitImpl&>(Dali::UI::GetImpl(*this));
  }

  const DummyTraitImpl& GetImpl() const
  {
    return static_cast<const DummyTraitImpl&>(Dali::UI::GetImpl(*this));
  }

private:
  explicit DummyTrait(DummyTraitImpl* impl)
    : Trait(impl)
  {
  }
};

class KeyEventCallback : public Dali::ConnectionTracker
{
public:
  /**
   * Constructor
   * @param[in]  returnValue  Set return value of KeyEvent callback.
   * */
  KeyEventCallback(bool consumed)
    : mConsumed(consumed),
      mIsCalled(false)
  {
  }

  bool Callback(Toolkit::Control control, const KeyEvent& keyEvent)
  {
    mIsCalled = true;
    return mConsumed;
  }

  void Callback(const KeyEvent& keyEvent)
  {
    mIsCalled = true;
  }

  bool mConsumed;
  bool mIsCalled;
};

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
  TestApplication application;
  View view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewNewP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewCopyConstructorP(void)
{
  TestApplication application;
  View view = View::New();
  View copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveConstructor(void)
{
  TestApplication application;
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
  TestApplication application;
  View view = View::New();
  View copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveAssignment(void)
{
  TestApplication application;
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
  TestApplication application;
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
  TestApplication application;
  BaseHandle unInitializedObject;
  View view1 = View::DownCast(unInitializedObject);
  View view2 = DownCast<View>(unInitializedObject);
  DALI_TEST_CHECK(!view1);
  DALI_TEST_CHECK(!view2);
  END_TEST;
}

int UtcDaliViewGetSizeWidthP(void)
{
  TestApplication application;
  View view = View::New();
  const float testWidth = 100.0f;

  view.SetSizeWidth(testWidth);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetSizeHeightP(void)
{
  TestApplication application;
  View view = View::New();
  const float testHeight = 200.0f;

  view.SetSizeHeight(testHeight);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionXP(void)
{
  TestApplication application;
  View view = View::New();
  const float testX = 50.0f;

  view.SetPositionX(testX);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionYP(void)
{
  TestApplication application;
  View view = View::New();
  const float testY = 75.0f;

  view.SetPositionY(testY);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeWidthChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testWidth = 150.0f;

  View& result = view.SetSizeWidth(testWidth);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeHeightChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testHeight = 250.0f;

  View& result = view.SetSizeHeight(testHeight);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionXChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testX = 125.0f;

  View& result = view.SetPositionX(testX);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionYChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testY = 175.0f;

  View& result = view.SetPositionY(testY);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBackgroundColorChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const Vector4 testColor(1.0f, 0.0f, 0.0f, 0.5f);

  View& result = view.BackgroundColor(testColor);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMultipleChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testWidth = 300.0f;
  const float testHeight = 200.0f;
  const float testX = 100.0f;
  const float testY = 50.0f;
  const Vector4 testColor(0.0f, 1.0f, 0.0f, 0.8f);

  View& result = view.SetSizeWidth(testWidth)
                     .SetSizeHeight(testHeight)
                     .SetPositionX(testX)
                     .SetPositionY(testY)
                     .BackgroundColor(testColor);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAsP(void)
{
  TestApplication application;
  View view = View::New();
  View reference;

  View& result = view.As(reference);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(reference == view);
  END_TEST;
}

int UtcDaliViewWithP(void)
{
  TestApplication application;
  View view = View::New();
  const float testWidth = 400.0f;
  bool actionCalled = false;

  View& result = view.With(
      [&actionCalled, testWidth](View& v)
      {
        v.SetSizeWidth(testWidth);
        actionCalled = true;
      });

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(actionCalled);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetParentOriginP(void)
{
  TestApplication application;
  View view = View::New();

  Vector3 parentOrigin = view.GetParentOrigin();
  DALI_TEST_EQUALS(parentOrigin.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetParentOriginP(void)
{
  TestApplication application;
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

int UtcDaliViewGetPivotPointP(void)
{
  TestApplication application;
  View view = View::New();

  Vector3 pivotPoint = view.GetPivotPoint();
  DALI_TEST_EQUALS(pivotPoint.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivotPoint.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivotPoint.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetPivotPointP(void)
{
  TestApplication application;
  View view = View::New();
  const Vector3 testPivot(1.0f, 0.0f, 0.5f);

  View& result = view.SetPivotPoint(testPivot);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  Vector3 pivotPoint = view.GetPivotPoint();
  DALI_TEST_EQUALS(pivotPoint.x, testPivot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(pivotPoint.y, testPivot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(pivotPoint.z, testPivot.z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewParentOriginChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const Vector3 testOrigin(0.0f, 0.0f, 0.0f);

  View& result = view.SetParentOrigin(testOrigin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewPivotPointChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const Vector3 testPivot(1.0f, 1.0f, 1.0f);

  View& result = view.SetPivotPoint(testPivot);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetTraitP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  viewImpl.SetTrait(0, trait);

  Trait retrievedTrait = viewImpl.GetTrait(0);
  DALI_TEST_CHECK(retrievedTrait);
  DALI_TEST_CHECK(retrievedTrait == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetTraitP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  viewImpl.SetTrait(0, trait);

  Trait retrievedTrait = viewImpl.GetTrait(0);
  DALI_TEST_CHECK(retrievedTrait);

  Trait nonExistentTrait = viewImpl.GetTrait(1);
  DALI_TEST_CHECK(!nonExistentTrait);
  END_TEST;
}

int UtcDaliViewRemoveTraitP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  viewImpl.SetTrait(0, trait);
  DALI_TEST_CHECK(viewImpl.GetTrait(0));

  bool removed = viewImpl.RemoveTrait(0);
  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!viewImpl.GetTrait(0));
  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);

  removed = viewImpl.RemoveTrait(0);
  DALI_TEST_CHECK(!removed);
  END_TEST;
}

int UtcDaliViewReplaceTraitP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait1 = DummyTrait::New();
  DummyTrait trait2 = DummyTrait::New();

  viewImpl.SetTrait(0, trait1);
  DALI_TEST_CHECK(viewImpl.GetTrait(0) == trait1);

  viewImpl.SetTrait(0, trait2);
  DALI_TEST_CHECK(viewImpl.GetTrait(0) == trait2);
  DALI_TEST_EQUALS(trait1.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait2.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetSameTraitP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  viewImpl.SetTrait(0, trait);
  Trait retrieved1 = viewImpl.GetTrait(0);

  viewImpl.SetTrait(0, trait);
  Trait retrieved2 = viewImpl.GetTrait(0);

  DALI_TEST_CHECK(retrieved1 == retrieved2);
  DALI_TEST_CHECK(retrieved1 == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitLifecycleP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  DummyTrait trait = DummyTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 0, TEST_LOCATION);

  viewImpl.SetTrait(0, trait);

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);

  viewImpl.RemoveTrait(0);

  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetLayoutWidthP(void)
{
  TestApplication application;
  View view = View::New();
  const float width = 200.0f;
  view.SetLayoutWidth(width);
  DALI_TEST_EQUALS(view.GetLayoutWidth(), width, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetLayoutWidthP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetLayoutWidth(), LayoutDimension::WrapContent, TEST_LOCATION);
  view.SetLayoutWidth(LayoutDimension::MatchParent);
  DALI_TEST_EQUALS(view.GetLayoutWidth(), LayoutDimension::MatchParent, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetLayoutHeightP(void)
{
  TestApplication application;
  View view = View::New();
  const float height = 100.0f;
  view.SetLayoutHeight(height);
  DALI_TEST_EQUALS(view.GetLayoutHeight(), height, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetLayoutHeightP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetLayoutHeight(), LayoutDimension::WrapContent, TEST_LOCATION);
  view.SetLayoutHeight(LayoutDimension::MatchParent);
  DALI_TEST_EQUALS(view.GetLayoutHeight(), LayoutDimension::MatchParent, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetViewMarginP(void)
{
  TestApplication application;
  View view = View::New();
  Extents margin(10, 20, 30, 40);
  view.SetViewMargin(margin);
  Extents got = view.GetViewMargin();
  DALI_TEST_EQUALS(got.start, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 30u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 40u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetViewMarginP(void)
{
  TestApplication application;
  View view = View::New();
  Extents got = view.GetViewMargin();
  DALI_TEST_EQUALS(got.start, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetViewPaddingP(void)
{
  TestApplication application;
  View view = View::New();
  Extents padding(5, 15, 25, 35);
  view.SetViewPadding(padding);
  Extents got = view.GetViewPadding();
  DALI_TEST_EQUALS(got.start, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 15u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 25u, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 35u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetViewPaddingP(void)
{
  TestApplication application;
  View view = View::New();
  Extents got = view.GetViewPadding();
  DALI_TEST_EQUALS(got.start, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetHorizontalAlignmentP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetHorizontalAlignment(LayoutAlignment::CENTER);
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetHorizontalAlignment()), static_cast<uint8_t>(LayoutAlignment::CENTER),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetHorizontalAlignmentP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetHorizontalAlignment()), static_cast<uint8_t>(LayoutAlignment::START),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetVerticalAlignmentP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetVerticalAlignment(LayoutAlignment::END);
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetVerticalAlignment()), static_cast<uint8_t>(LayoutAlignment::END),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetVerticalAlignmentP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetVerticalAlignment()), static_cast<uint8_t>(LayoutAlignment::START),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutWidthChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetLayoutWidth(150.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLayoutWidth(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetLayoutHeight(80.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLayoutHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(100.0f);
  view.SetLayoutHeight(50.0f);
  MeasuredSize size = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeP(void)
{
  TestApplication application;
  View view = View::New();
  LayoutRect bounds(10.0f, 20.0f, 100.0f, 80.0f);
  MeasuredSize size = view.Arrange(bounds);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInvalidateMeasureP(void)
{
  TestApplication application;
  View view = View::New();
  view.Measure(100.0f, 100.0f);
  DALI_TEST_CHECK(view.IsMeasureValid());
  view.InvalidateMeasure();
  DALI_TEST_CHECK(!view.IsMeasureValid());
  END_TEST;
}

int UtcDaliViewInvalidateArrangeP(void)
{
  TestApplication application;
  View view = View::New();
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0, 0, 100, 100));
  DALI_TEST_CHECK(view.IsArrangeValid());
  view.InvalidateArrange();
  DALI_TEST_CHECK(!view.IsArrangeValid());
  END_TEST;
}

int UtcDaliViewGetDesiredSizeP(void)
{
  TestApplication application;
  View view = View::New();
  MeasuredSize desired = view.GetDesiredSize();
  DALI_TEST_EQUALS(desired.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(desired.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewIsMeasureValidP(void)
{
  TestApplication application;
  View view = View::New();
  view.Measure(100.0f, 100.0f);
  DALI_TEST_CHECK(view.IsMeasureValid());
  view.InvalidateMeasure();
  DALI_TEST_CHECK(!view.IsMeasureValid());
  END_TEST;
}

int UtcDaliViewIsArrangeValidP(void)
{
  TestApplication application;
  View view = View::New();
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0, 0, 100, 100));
  DALI_TEST_CHECK(view.IsArrangeValid());
  view.InvalidateArrange();
  DALI_TEST_CHECK(!view.IsArrangeValid());
  END_TEST;
}

int UtcDaliViewSetMinimumWidthP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetMinimumWidth(50.0f);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumWidthP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMinimumHeightP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetMinimumHeight(30.0f);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumHeightP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumWidthP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetMaximumWidth(500.0f);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 500.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumWidthP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumWidth(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumHeightP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumHeightP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumHeight(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBackgroundColorP(void)
{
  TestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view);
  Vector4 color(0.2f, 0.4f, 0.6f, 1.0f);
  view.SetBackgroundColor(color);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewViewMarginChainingP(void)
{
  TestApplication application;
  View view = View::New();
  Extents margin(1, 2, 3, 4);
  View& result = view.SetViewMargin(margin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetViewMargin().start, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewViewPaddingChainingP(void)
{
  TestApplication application;
  View view = View::New();
  Extents padding(5, 10, 15, 20);
  View& result = view.SetViewPadding(padding);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetViewPadding().start, 5u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewHorizontalAlignmentChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetHorizontalAlignment(LayoutAlignment::END);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetHorizontalAlignment()), static_cast<uint8_t>(LayoutAlignment::END),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewVerticalAlignmentChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetVerticalAlignment(LayoutAlignment::CENTER);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(view.GetVerticalAlignment()), static_cast<uint8_t>(LayoutAlignment::CENTER),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMinimumWidthChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetMinimumWidth(25.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 25.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMinimumHeightChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetMinimumHeight(35.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 35.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumWidthChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetMaximumWidth(300.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 300.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumHeightChainingP(void)
{
  TestApplication application;
  View view = View::New();
  View& result = view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureCacheHitP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  MeasuredSize s1 = view.Measure(200.0f, 200.0f);
  MeasuredSize s2 = view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(s1.GetWidth(), s2.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(s1.GetHeight(), s2.GetHeight(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWithMarginP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetViewMargin(Extents(10, 10, 10, 10));
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  MeasuredSize size = view.Measure(100.0f, 100.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureMatchParentP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(LayoutDimension::MatchParent);
  view.SetLayoutHeight(LayoutDimension::MatchParent);
  MeasuredSize size = view.Measure(200.0f, 150.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInvalidateMeasureWithParentP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View child = View::New();
  layout.AddView(child);
  child.InvalidateMeasure();
  DALI_TEST_CHECK(!child.IsMeasureValid());
  END_TEST;
}

int UtcDaliViewApplyConstraintsMinMaxP(void)
{
  TestApplication application;
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
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(120.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetLayoutWidth(), 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightFixedNoManagerP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutHeight(80.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetLayoutHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeWithLayoutP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View c1 = View::New();
  c1.SetLayoutWidth(100.0f);
  c1.SetLayoutHeight(50.0f);
  layout.AddView(c1);
  View c2 = View::New();
  c2.SetLayoutWidth(100.0f);
  c2.SetLayoutHeight(50.0f);
  layout.AddView(c2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(200.0f);
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
  TestApplication application;
  View view = View::New();
  view.SetViewPadding(Extents(5, 5, 5, 5));
  view.SetLayoutWidth(40.0f);
  view.SetLayoutHeight(30.0f);
  MeasuredSize size = view.Measure(100.0f, 100.0f);
  // Fixed size is total size; padding is inside, not added on top
  DALI_TEST_EQUALS(size.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWrapContentP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(LayoutDimension::WrapContent);
  view.SetLayoutHeight(LayoutDimension::WrapContent);
  MeasuredSize size = view.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutMatchParentWithManagerP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetLayoutWidth(LayoutDimension::MatchParent);
  layout.SetLayoutHeight(LayoutDimension::MatchParent);
  layout.AddView(View::New().SetSizeWidth(50.0f).SetSizeHeight(50.0f));
  MeasuredSize size = layout.Measure(200.0f, 150.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewInvalidateArrangeWithParentP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View child = View::New();
  layout.AddView(child);
  child.Measure(100.0f, 100.0f);
  child.Arrange(LayoutRect(0, 0, 100, 100));
  child.InvalidateArrange();
  DALI_TEST_CHECK(!child.IsArrangeValid());
  END_TEST;
}

int UtcDaliViewArrangeAlignmentCenterP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  view.SetHorizontalAlignment(LayoutAlignment::CENTER);
  view.SetVerticalAlignment(LayoutAlignment::CENTER);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(10.0f, 20.0f, 100.0f, 100.0f));
  DALI_TEST_EQUALS(view.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeAlignmentEndP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(40.0f);
  view.SetLayoutHeight(30.0f);
  view.SetHorizontalAlignment(LayoutAlignment::END);
  view.SetVerticalAlignment(LayoutAlignment::END);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0, 0, 100.0f, 100.0f));
  DALI_TEST_EQUALS(view.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeAlignmentFillP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  view.SetHorizontalAlignment(LayoutAlignment::FILL);
  view.SetVerticalAlignment(LayoutAlignment::FILL);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0, 0, 120.0f, 80.0f));
  DALI_TEST_EQUALS(view.GetSizeWidth(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetLayoutWidthNoChangeP(void)
{
  TestApplication application;
  View view = View::New();
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  view.SetLayoutWidth(50.0f);
  view.SetLayoutHeight(50.0f);
  DALI_TEST_EQUALS(view.GetLayoutWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLayoutHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

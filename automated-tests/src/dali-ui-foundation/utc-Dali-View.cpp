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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-test-suite-utils.h>

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

  view.SizeWidth(testWidth);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetSizeHeightP(void)
{
  TestApplication application;
  View view = View::New();
  const float testHeight = 200.0f;

  view.SizeHeight(testHeight);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionXP(void)
{
  TestApplication application;
  View view = View::New();
  const float testX = 50.0f;

  view.PositionX(testX);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionYP(void)
{
  TestApplication application;
  View view = View::New();
  const float testY = 75.0f;

  view.PositionY(testY);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeWidthChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testWidth = 150.0f;

  View& result = view.SizeWidth(testWidth);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeHeightChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testHeight = 250.0f;

  View& result = view.SizeHeight(testHeight);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionXChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testX = 125.0f;

  View& result = view.PositionX(testX);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionYChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const float testY = 175.0f;

  View& result = view.PositionY(testY);
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

  View& result =
      view.SizeWidth(testWidth).SizeHeight(testHeight).PositionX(testX).PositionY(testY).BackgroundColor(testColor);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewContentsP(void)
{
  TestApplication application;
  View parent = View::New();
  View child1 = View::New();
  View child2 = View::New();

  View& result = parent.Contents({child1, child2});
  DALI_TEST_EQUALS(&result, &parent, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);
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
        v.SizeWidth(testWidth);
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

  View& result = view.ParentOrigin(testOrigin);
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

  View& result = view.PivotPoint(testPivot);
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

  View& result = view.ParentOrigin(testOrigin);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewPivotPointChainingP(void)
{
  TestApplication application;
  View view = View::New();
  const Vector3 testPivot(1.0f, 1.0f, 1.0f);

  View& result = view.PivotPoint(testPivot);
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

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

#include <dali-test-suite-utils.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali/public-api/actors/actor.h>
#include <typeinfo>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{
// Test trait implementation for lifecycle testing
class TestTraitImpl : public TraitImpl
{
public:
  TestTraitImpl()
    : mBeforeAttachedCount(0),
      mAttachedCount(0),
      mDetachedCount(0),
      mViewDestroyingCount(0),
      mLastId(0)
  {
  }

  void OnBeforeAttached(TraitId id, View& view) override
  {
    mBeforeAttachedCount++;
    mLastId = id;
  }

  void OnAttached(TraitId id, View& view) override
  {
    mAttachedCount++;
    mLastId = id;
  }

  void OnDetached(TraitId id, View& view) override
  {
    mDetachedCount++;
    mLastId = id;
  }

  void OnViewDestroying(ViewImpl* viewImpl) override
  {
    mViewDestroyingCount++;
  }

  int GetBeforeAttachedCount() const
  {
    return mBeforeAttachedCount;
  }
  int GetAttachedCount() const
  {
    return mAttachedCount;
  }
  int GetDetachedCount() const
  {
    return mDetachedCount;
  }
  int GetViewDestroyingCount() const
  {
    return mViewDestroyingCount;
  }
  TraitId GetLastId() const
  {
    return mLastId;
  }

private:
  int mBeforeAttachedCount;
  int mAttachedCount;
  int mDetachedCount;
  int mViewDestroyingCount;
  TraitId mLastId;
};

class TestTrait : public Trait
{
public:
  static TestTrait New()
  {
    return TestTrait(new TestTraitImpl());
  }

  TestTrait() = default;

  TestTraitImpl& GetImpl()
  {
    return static_cast<TestTraitImpl&>(Dali::Ui::GetImpl(*this));
  }

  const TestTraitImpl& GetImpl() const
  {
    return static_cast<const TestTraitImpl&>(Dali::Ui::GetImpl(*this));
  }

private:
  explicit TestTrait(TestTraitImpl* impl)
    : Trait(impl)
  {
  }
};

} // namespace

void utc_dali_view_trait_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_trait_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTraitConstructorP(void)
{
  TestApplication application;
  Trait trait;
  DALI_TEST_CHECK(!trait);
  END_TEST;
}

int UtcDaliTraitNewP(void)
{
  TestApplication application;
  TestTrait trait = TestTrait::New();
  DALI_TEST_CHECK(trait);
  END_TEST;
}

int UtcDaliTraitCopyConstructorP(void)
{
  TestApplication application;
  TestTrait trait = TestTrait::New();
  TestTrait copy(trait);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(trait == copy);
  END_TEST;
}

int UtcDaliTraitMoveConstructor(void)
{
  TestApplication application;
  TestTrait trait = TestTrait::New();
  DALI_TEST_EQUALS(1, trait.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  TestTrait moved = std::move(trait);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!trait);
  END_TEST;
}

int UtcDaliTraitAssignmentOperatorP(void)
{
  TestApplication application;
  TestTrait trait = TestTrait::New();
  TestTrait copy;
  copy = trait;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(trait == copy);
  END_TEST;
}

int UtcDaliTraitMoveAssignment(void)
{
  TestApplication application;
  TestTrait trait = TestTrait::New();
  DALI_TEST_EQUALS(1, trait.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  TestTrait moved;
  moved = std::move(trait);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!trait);
  END_TEST;
}

int UtcDaliTraitOnBeforeAttachedP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 0, TEST_LOCATION);

  viewImpl.SetTrait(0, trait);

  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetLastId().value, 1001u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitOnAttachedP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 0, TEST_LOCATION);

  viewImpl.SetTrait(0, trait);

  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetLastId().value, 1001u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitOnDetachedP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 0, TEST_LOCATION);

  viewImpl.SetTrait(0, trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 0, TEST_LOCATION);

  viewImpl.RemoveTrait(0);

  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetLastId().value, 1001u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitOnViewDestroyingP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetViewDestroyingCount(), 0, TEST_LOCATION);

  viewImpl.SetTrait(0, trait);

  // View destruction will trigger OnViewDestroying
  view.Reset();

  DALI_TEST_EQUALS(trait.GetImpl().GetViewDestroyingCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitLifecycleOrderP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  viewImpl.SetTrait(0, trait);

  // OnBeforeAttached should be called before OnAttached
  DALI_TEST_EQUALS(trait.GetImpl().GetBeforeAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);

  viewImpl.RemoveTrait(0);

  // OnDetached should be called after removal
  DALI_TEST_EQUALS(trait.GetImpl().GetDetachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitReservedIdP(void)
{
  TestApplication application;
  View view = View::New();
  ViewImpl& viewImpl = GetImpl(view);
  TestTrait trait = TestTrait::New();

  viewImpl.SetTrait(ReservedTraitId::INTERACTION_EFFECT, trait);

  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait.GetImpl().GetLastId().value, 50u, TEST_LOCATION);
  END_TEST;
}

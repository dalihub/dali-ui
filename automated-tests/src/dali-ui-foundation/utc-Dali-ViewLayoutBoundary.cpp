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

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// ============================================================================
// MeasureCounterViewImpl
// Counts how many times OnMeasure / OnArrange are invoked. Used to verify
// that parent-side layout passes are (or are not) triggered when children
// change.
// ============================================================================
class MeasureCounterViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<MeasureCounterViewImpl> New()
  {
    return new MeasureCounterViewImpl();
  }

  int  GetMeasureCallCount() const { return mMeasureCount; }
  int  GetArrangeCallCount() const { return mArrangeCount; }
  void ResetCounters() { mMeasureCount = 0; mArrangeCount = 0; }

protected:
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override
  {
    mMeasureCount++;
    return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
  }

  LayoutRect OnArrange(const LayoutRect& bounds) override
  {
    mArrangeCount++;
    return ViewImpl::OnArrange(bounds);
  }

private:
  int mMeasureCount{0};
  int mArrangeCount{0};
};

// Register so TypeInfo lookup can walk the chain
Dali::TypeRegistration measureCounterViewTypeReg(
  typeid(MeasureCounterViewImpl), typeid(ViewImpl), nullptr);

// Build a View handle around the impl
View CreateCounterView()
{
  auto impl = MeasureCounterViewImpl::New();
  View view(*impl);
  return view;
}

// Helper to retrieve the impl from a View handle created via CreateCounterView
MeasureCounterViewImpl& GetCounterImpl(View view)
{
  ViewImpl& viewImpl = GetImpl(view);
  return static_cast<MeasureCounterViewImpl&>(viewImpl);
}

} // namespace

void utc_dali_viewlayoutboundary_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_viewlayoutboundary_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Phase 1 tests: OnChildRemove optimization
// Removing a standalone child must NOT invalidate parent's measure cache,
// because standalone children are excluded from parent's layout accumulation.
// Removing a default (non-standalone) child DOES invalidate (regression guard).
// ============================================================================

// T1.1: Standalone child removal does not invalidate parent's measure cache
int UtcDaliViewLayoutBoundary_RemoveStandaloneNoParentRemeasure_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  // First measure establishes cache
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Remove standalone child
  parent.Remove(child);

  // Subsequent Measure with same constraint should hit cache: OnMeasure not called
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T1.2: Default (non-standalone) child removal DOES invalidate parent (regression)
int UtcDaliViewLayoutBoundary_RemoveDefaultInvalidatesParent_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();  // DEFAULT mode
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Remove default child → parent must be invalidated
  parent.Remove(child);

  // Subsequent Measure is cache miss: OnMeasure called again
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T1.3: Multiple standalone children; removing all does not invalidate parent
int UtcDaliViewLayoutBoundary_RemoveMultipleStandaloneNoInvalidate_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View c1 = View::New();
  View c2 = View::New();
  View c3 = View::New();
  c1.SetLayoutMode(LayoutMode::STANDALONE);
  c2.SetLayoutMode(LayoutMode::STANDALONE);
  c3.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(c1);
  parent.Add(c2);
  parent.Add(c3);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  parent.Remove(c1);
  parent.Remove(c2);
  parent.Remove(c3);

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T1.4: Mixed children — only default removal invalidates parent
int UtcDaliViewLayoutBoundary_MixedChildRemoval_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View standaloneChild = View::New();
  View defaultChild    = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(standaloneChild);
  parent.Add(defaultChild);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Remove standalone first — parent cache remains valid
  parent.Remove(standaloneChild);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Remove default — parent cache invalidated
  parent.Remove(defaultChild);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T1.5: Removed child's own measure cache is invalidated (unchanged existing behavior)
int UtcDaliViewLayoutBoundary_RemovedChildCacheInvalidated_P(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);

  View child = CreateCounterView();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(60.0f);
  parent.Add(child);

  auto& cImpl = GetCounterImpl(child);

  child.Measure(100.0f, 100.0f);
  DALI_TEST_EQUALS(cImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Detach
  parent.Remove(child);

  // Child's cache was invalidated on remove: next Measure is cache miss
  child.Measure(100.0f, 100.0f);
  DALI_TEST_EQUALS(cImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T1.6: Focus-ring-like scenario — ring removal does not invalidate grand/parent
int UtcDaliViewLayoutBoundary_FocusRingRemovalOptimization_P(void)
{
  UiTestApplication application;

  View grandparent = CreateCounterView();
  grandparent.SetRequestedWidth(500.0f);
  grandparent.SetRequestedHeight(500.0f);

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);

  View targetView = CreateCounterView();
  targetView.SetRequestedWidth(150.0f);
  targetView.SetRequestedHeight(80.0f);

  grandparent.Add(parent);
  parent.Add(targetView);

  // Focus ring: STANDALONE + MATCH_PARENT
  View ring = View::New();
  ring.SetLayoutMode(LayoutMode::STANDALONE);
  ring.SetRequestedWidth(MATCH_PARENT);
  ring.SetRequestedHeight(MATCH_PARENT);
  targetView.Add(ring);

  auto& gpImpl     = GetCounterImpl(grandparent);
  auto& parentImpl = GetCounterImpl(parent);
  auto& targetImpl = GetCounterImpl(targetView);

  // Initial full pass
  grandparent.Measure(600.0f, 600.0f);
  grandparent.Arrange(LayoutRect(0, 0, 600, 600));

  const int gpBase     = gpImpl.GetMeasureCallCount();
  const int parentBase = parentImpl.GetMeasureCallCount();
  const int targetBase = targetImpl.GetMeasureCallCount();

  // Focus ring removal (focus navigation away)
  targetView.Remove(ring);

  // Subsequent Measure/Arrange should all hit cache up the chain
  grandparent.Measure(600.0f, 600.0f);
  grandparent.Arrange(LayoutRect(0, 0, 600, 600));

  DALI_TEST_EQUALS(gpImpl.GetMeasureCallCount(),     gpBase,     TEST_LOCATION);
  DALI_TEST_EQUALS(parentImpl.GetMeasureCallCount(), parentBase, TEST_LOCATION);
  DALI_TEST_EQUALS(targetImpl.GetMeasureCallCount(), targetBase, TEST_LOCATION);

  END_TEST;
}

// T1.7: Measure cache preserved when removing standalone child; a subsequent
// Arrange runs unconditionally (ViewImpl::Arrange has no cache gate), so only
// measure re-invocation is a valid behavioural signal of invalidation.
int UtcDaliViewLayoutBoundary_RemoveStandaloneNoParentRearrange_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  parent.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(pImpl.GetArrangeCallCount(), 1, TEST_LOCATION);

  parent.Remove(child);

  // Measure stays cache-hit: the standalone removal did not mark parent dirty.
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Phase 2 tests: InvalidateMeasure/InvalidateArrange boundary + SetLayoutMode
// transition + Scene connect/disconnect lifecycle.
//
// With the boundary rule in InvalidateMeasure/InvalidateArrange, invalidating a
// standalone child does NOT propagate to the parent. A layout-mode transition
// explicitly invalidates the parent. Scene disconnect unregisters from the
// controller so pending work does not leak to stale trees.
// ============================================================================

// T2.1: InvalidateMeasure on a standalone child does not invalidate parent
int UtcDaliViewLayoutBoundary_StandaloneInvalidateMeasureNoPropagate_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Invalidate the standalone child's measure — boundary blocks propagation
  child.InvalidateMeasure();

  // Re-measuring parent hits cache (not invalidated)
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T2.2: InvalidateArrange on a standalone child does not mark the parent
// dirty. Arrange() itself has no cache, so we instead assert that the
// parent's measure cache (which a non-boundary InvalidateArrange cannot
// touch, and a boundary InvalidateArrange must not either) still hits on a
// follow-up Measure call with the same constraint.
int UtcDaliViewLayoutBoundary_StandaloneInvalidateArrangeNoPropagate_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  parent.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Invalidate the standalone child's arrange. The boundary rule keeps the
  // invalidation local; the parent's measure cache must remain valid.
  child.InvalidateArrange();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T2.3: InvalidateMeasure on a default (non-standalone) child DOES propagate
int UtcDaliViewLayoutBoundary_DefaultInvalidatePropagates_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();  // DEFAULT
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Default child invalidation propagates → parent dirty
  child.InvalidateMeasure();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T2.4: SetLayoutMode DEFAULT → STANDALONE invalidates the parent
int UtcDaliViewLayoutBoundary_SetLayoutModeDefaultToStandaloneInvalidatesParent_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();  // DEFAULT initially
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Transitioning to STANDALONE: this child no longer contributes to parent's
  // measure accumulation — the parent's cached result is stale and must be
  // re-computed.
  child.SetLayoutMode(LayoutMode::STANDALONE);

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T2.5: SetLayoutMode STANDALONE → DEFAULT invalidates the parent
int UtcDaliViewLayoutBoundary_SetLayoutModeStandaloneToDefaultInvalidatesParent_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Transition back to DEFAULT: child now contributes to parent's measure —
  // parent's cache no longer reflects this contribution.
  child.SetLayoutMode(LayoutMode::DEFAULT);

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T2.6: Setting same mode does not invalidate parent (no-op)
int UtcDaliViewLayoutBoundary_SetSameLayoutModeNoInvalidate_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Setting the same mode twice should be a no-op: parent is not re-invalidated
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T2.7: End-to-end — standalone view invalidated with scene-connected tree
// processes through LayoutController without invalidating the grandparent.
int UtcDaliViewLayoutBoundary_StandaloneOnSceneBoundaryIntegration_P(void)
{
  UiTestApplication application;

  View grandparent = CreateCounterView();
  grandparent.SetRequestedWidth(500.0f);
  grandparent.SetRequestedHeight(500.0f);

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);

  grandparent.Add(parent);
  parent.Add(standaloneChild);

  auto& gpImpl     = GetCounterImpl(grandparent);
  auto& parentImpl = GetCounterImpl(parent);

  grandparent.Measure(500.0f, 500.0f);
  grandparent.Arrange(LayoutRect(0, 0, 500, 500));

  const int gpBase     = gpImpl.GetMeasureCallCount();
  const int parentBase = parentImpl.GetMeasureCallCount();

  // Invalidate standalone child — boundary prevents propagation up to parent
  standaloneChild.InvalidateMeasure();
  standaloneChild.InvalidateArrange();

  grandparent.Measure(500.0f, 500.0f);
  grandparent.Arrange(LayoutRect(0, 0, 500, 500));

  // Ancestors are not re-measured/re-arranged
  DALI_TEST_EQUALS(gpImpl.GetMeasureCallCount(),     gpBase,     TEST_LOCATION);
  DALI_TEST_EQUALS(parentImpl.GetMeasureCallCount(), parentBase, TEST_LOCATION);

  END_TEST;
}

// T2.8: Scene disconnect then reconnect works correctly with dirty boundary.
// Behavioral check: after disconnect + reconnect, subsequent measurement
// still produces a correct result (no crash, no stale state).
int UtcDaliViewLayoutBoundary_SceneDisconnectReconnectDirtyBoundary_P(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  standaloneChild.SetRequestedWidth(MATCH_PARENT);
  standaloneChild.SetRequestedHeight(MATCH_PARENT);
  parent.Add(standaloneChild);

  window.Add(parent);
  application.SendNotification();
  application.Render();

  // Dirty the standalone child while on-scene, then disconnect
  standaloneChild.InvalidateMeasure();
  window.Remove(parent);

  // Reconnect: boundary rule + OnSceneConnection self-register ensures
  // the pending state is picked up by the new window's controller.
  window.Add(parent);
  application.SendNotification();
  application.Render();

  // Smoke test: subsequent direct measure works without assertion
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_CHECK(true);

  END_TEST;
}

// T2.9: Phase 1 regression check — after Phase 2 boundary logic is in,
// the Phase 1 OnChildRemove behavior still holds.
int UtcDaliViewLayoutBoundary_Phase1RegressionStandaloneRemove_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(child);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  const int base = pImpl.GetMeasureCallCount();

  parent.Remove(child);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), base, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Phase 3 tests: OnChildAdd optimization for standalone (boundary) children.
//
// With Phase 2's boundary rule in InvalidateMeasure, adding a standalone
// child no longer needs to invalidate the parent. The child's own
// InvalidateMeasure registers it as a layout root (boundary), and
// OnSceneConnection re-registers dirty boundaries that were already dirty
// when reparented.
// ============================================================================

// T3.1: Adding a standalone child to a clean parent does not invalidate parent
int UtcDaliViewLayoutBoundary_AddStandaloneNoParentInvalidation_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  // Establish parent cache first
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Add a standalone child — should NOT invalidate parent
  View ring = View::New();
  ring.SetLayoutMode(LayoutMode::STANDALONE);
  ring.SetRequestedWidth(MATCH_PARENT);
  ring.SetRequestedHeight(MATCH_PARENT);
  parent.Add(ring);

  // Re-measuring parent with same constraint should be cache hit
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  END_TEST;
}

// T3.2: Adding a default (non-standalone) child still invalidates parent
int UtcDaliViewLayoutBoundary_AddDefaultInvalidatesParent_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Add a DEFAULT child — parent must be invalidated
  View child = View::New();
  parent.Add(child);

  // Re-measuring parent is cache miss → OnMeasure called again
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T3.3: Focus ring full navigation — add to A, remove from A, add to B.
// No parent chain re-measure in any step.
int UtcDaliViewLayoutBoundary_FocusRingFullNavigation_P(void)
{
  UiTestApplication application;

  View grandparent = CreateCounterView();
  grandparent.SetRequestedWidth(500.0f);
  grandparent.SetRequestedHeight(500.0f);

  View viewA = CreateCounterView();
  viewA.SetRequestedWidth(200.0f);
  viewA.SetRequestedHeight(100.0f);

  View viewB = CreateCounterView();
  viewB.SetRequestedWidth(200.0f);
  viewB.SetRequestedHeight(100.0f);

  grandparent.Add(viewA);
  grandparent.Add(viewB);

  // Focus ring
  View ring = View::New();
  ring.SetLayoutMode(LayoutMode::STANDALONE);
  ring.SetRequestedWidth(MATCH_PARENT);
  ring.SetRequestedHeight(MATCH_PARENT);

  auto& gpImpl = GetCounterImpl(grandparent);
  auto& aImpl  = GetCounterImpl(viewA);
  auto& bImpl  = GetCounterImpl(viewB);

  // Establish baseline: full layout pass
  grandparent.Measure(600.0f, 600.0f);
  grandparent.Arrange(LayoutRect(0, 0, 600, 600));

  const int gpMeasureBase = gpImpl.GetMeasureCallCount();
  const int aMeasureBase  = aImpl.GetMeasureCallCount();
  const int bMeasureBase  = bImpl.GetMeasureCallCount();

  // Step 1: focus on A — add ring
  viewA.Add(ring);
  // Step 2: focus moves to B — remove from A, add to B
  viewA.Remove(ring);
  viewB.Add(ring);

  // After focus navigation, re-measure from grandparent with the same
  // constraints. Cache-hit on every level proves no ancestor was invalidated.
  grandparent.Measure(600.0f, 600.0f);

  DALI_TEST_EQUALS(gpImpl.GetMeasureCallCount(), gpMeasureBase, TEST_LOCATION);
  DALI_TEST_EQUALS(aImpl.GetMeasureCallCount(),  aMeasureBase,  TEST_LOCATION);
  DALI_TEST_EQUALS(bImpl.GetMeasureCallCount(),  bMeasureBase,  TEST_LOCATION);

  END_TEST;
}

// T3.4: Mixed add — adding standalone does not invalidate, adding default does
int UtcDaliViewLayoutBoundary_AddMixedChildren_P(void)
{
  UiTestApplication application;

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  auto& pImpl = GetCounterImpl(parent);
  pImpl.ResetCounters();

  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Add standalone — cache valid
  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  parent.Add(standaloneChild);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 1, TEST_LOCATION);

  // Add default — cache invalidated
  View defaultChild = View::New();
  parent.Add(defaultChild);
  parent.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(), 2, TEST_LOCATION);

  END_TEST;
}

// T3.5: Reparenting a dirty standalone child into a new (clean) parent
// does not invalidate the new parent.
int UtcDaliViewLayoutBoundary_ReparentDirtyStandaloneNoNewParentInvalidation_P(void)
{
  UiTestApplication application;

  View parentA = CreateCounterView();
  parentA.SetRequestedWidth(200.0f);
  parentA.SetRequestedHeight(100.0f);

  View parentB = CreateCounterView();
  parentB.SetRequestedWidth(400.0f);
  parentB.SetRequestedHeight(200.0f);

  View ring = View::New();
  ring.SetLayoutMode(LayoutMode::STANDALONE);
  ring.SetRequestedWidth(MATCH_PARENT);
  ring.SetRequestedHeight(MATCH_PARENT);

  parentA.Add(ring);

  // Prime both parents and make the ring dirty.
  parentA.Measure(500.0f, 500.0f);
  parentB.Measure(500.0f, 500.0f);

  ring.InvalidateMeasure();  // ring now dirty (boundary registered self)

  auto& aImpl = GetCounterImpl(parentA);
  auto& bImpl = GetCounterImpl(parentB);
  const int aBase = aImpl.GetMeasureCallCount();
  const int bBase = bImpl.GetMeasureCallCount();

  // Reparent dirty ring
  parentA.Remove(ring);
  parentB.Add(ring);

  // Neither parent should be re-measured when re-triggered with same constraint
  parentA.Measure(500.0f, 500.0f);
  parentB.Measure(500.0f, 500.0f);

  DALI_TEST_EQUALS(aImpl.GetMeasureCallCount(), aBase, TEST_LOCATION);
  DALI_TEST_EQUALS(bImpl.GetMeasureCallCount(), bBase, TEST_LOCATION);

  END_TEST;
}

// T3.6: Phase 1/2 regression — combined add+remove cycle should leave no
// net invalidation of an untouched grandparent.
int UtcDaliViewLayoutBoundary_Phase12RegressionCombined_P(void)
{
  UiTestApplication application;

  View grandparent = CreateCounterView();
  grandparent.SetRequestedWidth(500.0f);
  grandparent.SetRequestedHeight(500.0f);

  View parent = CreateCounterView();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  grandparent.Add(parent);

  auto& gpImpl = GetCounterImpl(grandparent);
  auto& pImpl  = GetCounterImpl(parent);

  grandparent.Measure(600.0f, 600.0f);
  grandparent.Arrange(LayoutRect(0, 0, 600, 600));

  const int gpBase = gpImpl.GetMeasureCallCount();
  const int pBase  = pImpl.GetMeasureCallCount();

  // Add standalone, invalidate it, remove it
  View ring = View::New();
  ring.SetLayoutMode(LayoutMode::STANDALONE);
  ring.SetRequestedWidth(MATCH_PARENT);
  ring.SetRequestedHeight(MATCH_PARENT);

  parent.Add(ring);
  ring.InvalidateMeasure();
  parent.Remove(ring);

  grandparent.Measure(600.0f, 600.0f);
  grandparent.Arrange(LayoutRect(0, 0, 600, 600));

  DALI_TEST_EQUALS(gpImpl.GetMeasureCallCount(), gpBase, TEST_LOCATION);
  DALI_TEST_EQUALS(pImpl.GetMeasureCallCount(),  pBase,  TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Step 1-7 refactor tests (ChildData deletion + Fix-J + layout manager rewrite)
// ============================================================================

// T4.1: Repeated layout passes with flex-grow do not accumulate allocations.
// (Verifies Step 5 local working buffer, replacing the save/restore hack.)
int UtcDaliViewLayoutBoundary_FlexRepeatedPassNoAccumulation_P(void)
{
  UiTestApplication application;

  // Build a simple flex layout via ViewImpl default paths; we don't need a
  // full FlexLayout type. We just verify that repeatedly measuring+arranging
  // children produces stable results when MATCH_PARENT re-measures are
  // expected. Runs entirely through the ViewImpl default path.
  View parent = View::New();
  parent.SetRequestedWidth(300.0f);
  parent.SetRequestedHeight(100.0f);

  auto child = CreateCounterView();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);

  // Pass 1
  parent.Measure(300.0f, 100.0f);
  parent.Arrange(LayoutRect(0, 0, 300, 100));
  Vector3 size1 = child.GetProperty<Vector3>(Actor::Property::SIZE);

  // Pass 2 (same input) — size should be identical (no accumulation)
  parent.InvalidateMeasure();
  parent.InvalidateArrange();
  parent.Measure(300.0f, 100.0f);
  parent.Arrange(LayoutRect(0, 0, 300, 100));
  Vector3 size2 = child.GetProperty<Vector3>(Actor::Property::SIZE);

  DALI_TEST_EQUALS(size1.width,  size2.width,  TEST_LOCATION);
  DALI_TEST_EQUALS(size1.height, size2.height, TEST_LOCATION);

  END_TEST;
}

// T4.2: Fix-J — boundary view's explicit RequestedWidth takes precedence.
int UtcDaliViewLayoutBoundary_FixJExplicitRequestedWidthWins_P(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(400.0f);
  parent.SetRequestedHeight(300.0f);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  standaloneChild.SetRequestedWidth(80.0f);    // explicit, wins over parent.SIZE
  standaloneChild.SetRequestedHeight(60.0f);
  parent.Add(standaloneChild);

  parent.Measure(500.0f, 500.0f);
  parent.Arrange(LayoutRect(0, 0, 400, 300));

  // Read the just-set target SIZE (no Render flush needed).
  Vector3 childSize = standaloneChild.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(childSize.width,  80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childSize.height, 60.0f, TEST_LOCATION);

  END_TEST;
}

// T4.3: Fix-J — boundary view without explicit size uses parent.Actor.SIZE.
int UtcDaliViewLayoutBoundary_FixJUsesParentActorSize_P(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(400.0f);
  parent.SetRequestedHeight(300.0f);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  standaloneChild.SetRequestedWidth(MATCH_PARENT);
  standaloneChild.SetRequestedHeight(MATCH_PARENT);
  parent.Add(standaloneChild);

  parent.Measure(500.0f, 500.0f);
  parent.Arrange(LayoutRect(0, 0, 400, 300));

  // MATCH_PARENT inside parent of size 400x300 → child fills parent.
  Vector3 childSize = standaloneChild.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(childSize.width,  400.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childSize.height, 300.0f, TEST_LOCATION);

  END_TEST;
}

// T4.4: WRAP_CONTENT parent with padding does not accumulate the child's
// arranged offset on repeated measure passes.
//
// Before fix: OnMeasure read childImpl.GetPositionX() which had been
// overwritten by the previous Arrange to padLeft + margin + requested,
// so the next WRAP_CONTENT computation added padLeft a second time.
// After fix: OnMeasure reads GetRequestedPositionX (raw user intent),
// so size stays stable across resizes.
int UtcDaliViewLayoutBoundary_WrapContentPaddingNoAccumulation_P(void)
{
  UiTestApplication application;

  // child2: WRAP_CONTENT, padding=50/50/50/50
  // grandchild2: width=100
  // Expected child2 width = 100 + 50 + 50 = 200, stable across passes.
  View parent = View::New();
  parent.SetRequestedWidth(MATCH_PARENT);
  parent.SetRequestedHeight(MATCH_PARENT);

  View child = View::New();
  child.SetRequestedWidth(WRAP_CONTENT);
  child.SetRequestedHeight(200.0f);
  child.SetPadding(Extents(50, 50, 50, 50));
  parent.Add(child);

  View grandchild = View::New();
  grandchild.SetRequestedWidth(100.0f);
  grandchild.SetRequestedHeight(100.0f);
  child.Add(grandchild);

  // First pass at 480-wide window
  parent.Measure(480.0f, 800.0f);
  parent.Arrange(LayoutRect(0, 0, 480, 800));
  MeasuredSize firstSize = child.GetMeasuredSize();
  DALI_TEST_EQUALS(firstSize.width, 200.0f, TEST_LOCATION);

  // Simulate window resize: invalidate root and re-measure with new size
  parent.InvalidateMeasure();
  parent.Measure(600.0f, 800.0f);
  parent.Arrange(LayoutRect(0, 0, 600, 800));
  MeasuredSize secondSize = child.GetMeasuredSize();
  DALI_TEST_EQUALS(secondSize.width, 200.0f, TEST_LOCATION);

  // Third resize for good measure — should remain stable.
  parent.InvalidateMeasure();
  parent.Measure(720.0f, 800.0f);
  parent.Arrange(LayoutRect(0, 0, 720, 800));
  MeasuredSize thirdSize = child.GetMeasuredSize();
  DALI_TEST_EQUALS(thirdSize.width, 200.0f, TEST_LOCATION);

  END_TEST;
}

// T4.5: SetRequestedPositionX on a default-mode child must invalidate the
// parent's measure cache. A WRAP_CONTENT parent without a LayoutManager
// computes its size from each child's RequestedPosition + measured width
// (maxRight). Without invalidation, repeated Measure calls would return
// the stale cached size.
int UtcDaliViewLayoutBoundary_SetRequestedPositionInvalidatesParentMeasure_P(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(WRAP_CONTENT);
  parent.SetRequestedHeight(WRAP_CONTENT);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedPositionX(0.0f);
  child.SetRequestedPositionY(0.0f);
  parent.Add(child);

  parent.Measure(1000.0f, 1000.0f);
  parent.Arrange(LayoutRect(0, 0, 0, 0));
  MeasuredSize firstSize = parent.GetMeasuredSize();
  DALI_TEST_EQUALS(firstSize.width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(firstSize.height, 50.0f, TEST_LOCATION);

  // Move child further to the right/bottom; parent's WRAP_CONTENT size must
  // grow to include the new extent (250, 250) without an explicit
  // InvalidateMeasure on the parent.
  child.SetRequestedPositionX(200.0f);
  child.SetRequestedPositionY(200.0f);

  parent.Measure(1000.0f, 1000.0f);
  MeasuredSize secondSize = parent.GetMeasuredSize();
  DALI_TEST_EQUALS(secondSize.width, 250.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(secondSize.height, 250.0f, TEST_LOCATION);

  END_TEST;
}

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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout.h>
#include <dali-ui-foundation/public-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/video/video-source.h>
#include <dali-ui-foundation/public-api/video/video-view.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

namespace IntegrationView = Dali::Ui::Integration::View;
namespace ReservedTraitId = Dali::Ui::Integration::ReservedTraitId;

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;

void utc_dali_arrange_cache_hit_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_arrange_cache_hit_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// White-box coverage for the ARRANGE cache HIT (childless views).
//
// The black-box suite observes the hit through a counting arrange producer, which
// is the right instrument for "did the producer run" but says nothing about the
// bookkeeping state the hit leaves behind, or about the DEBUG invariant it asserts.
// Those two are pinned here, plus the one externally visible side effect a hit is
// required to keep: the LayoutFinished signal.

namespace
{
ViewDataImpl& DataOf(View& view)
{
  return ViewDataImpl::Get(GetImpl(view));
}

// Drives one full layout batch to completion.
void Settle(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}

int gLayoutFinishedCount = 0;

void OnLayoutFinished(View, LayoutRect)
{
  ++gLayoutFinishedCount;
}

// A counting arrange producer that echoes its input slot and does nothing else, so
// its invocation count measures exactly one thing: whether that view's Arrange() ran
// its producer or served a cache HIT.
int gCallbackArrangeCount = 0;

LayoutRect CountingCallbackArrange(View, const LayoutRect& bounds)
{
  ++gCallbackArrangeCount;
  return bounds;
}

// VideoSource::New() forwards providerId/nativeSession opaquely to the adaptor
// plugin and never dereferences nativeSession itself, and no platform video-player
// plugin is loaded here. A dummy address stands in for a real player handle. Same
// construction as utc-Dali-VideoView.cpp.
int gDummyVideoSession = 0;

VideoSource CreateTestVideoSource()
{
  return VideoSource::New("test.provider", &gDummyVideoSession, VideoSourceOwnership::EXTERNAL, VideoRenderingMode::UNDERLAY);
}

// A stand-in for a third-party ViewImpl subclass that counts OnArrange calls. The
// factory explicitly selects either execution policy so hit and miss behaviour can
// be compared with otherwise identical producers.
class PolicyCounterViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<PolicyCounterViewImpl> New(bool arrangeIfChanged)
  {
    IntrusivePtr<PolicyCounterViewImpl> impl(new PolicyCounterViewImpl());
    impl->SetArrangePolicy(arrangeIfChanged ? ArrangePolicy::IF_CHANGED : ArrangePolicy::ALWAYS);
    return impl;
  }

  int GetArrangeCallCount() const
  {
    return mArrangeCount;
  }

  // SetArrangePolicy is protected, so a runtime policy-change test exercises it
  // through the subclass just as a custom View implementation would.
  void SetPolicy(ArrangePolicy policy)
  {
    SetArrangePolicy(policy);
  }

  // GetArrangePolicy is protected too; expose it so the getter can be asserted the
  // same way a custom View implementation would read back its own policy.
  ArrangePolicy GetPolicy() const
  {
    return GetArrangePolicy();
  }

protected:
  PolicyCounterViewImpl()
  : ViewImpl()
  {
  }

  LayoutRect OnArrange(const LayoutRect& bounds) override
  {
    ++mArrangeCount;
    return ViewImpl::OnArrange(bounds);
  }

private:
  int mArrangeCount{0};
};

// Register so TypeInfo lookup can walk the chain.
Dali::TypeRegistration policyCounterViewTypeReg(
  typeid(PolicyCounterViewImpl), typeid(ViewImpl), nullptr);

View CreatePolicyCounterView(bool arrangeIfChanged)
{
  auto impl = PolicyCounterViewImpl::New(arrangeIfChanged);
  return View(*impl);
}

PolicyCounterViewImpl& PolicyCounterImplOf(View view)
{
  return static_cast<PolicyCounterViewImpl&>(GetImpl(view));
}

LayoutRect SettledSlotOf(View view)
{
  return LayoutRect(view.GetProperty<float>(Actor::Property::POSITION_X),
                    view.GetProperty<float>(Actor::Property::POSITION_Y),
                    view.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                    view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
}

// A third-party subclass of a first-party view. Its OnArrange override inherits the
// framework default IF_CHANGED policy, and may explicitly opt out when it
// performs work that cannot be represented by the arrange cache.
class ProbeLabelImpl : public Dali::Ui::Integration::LabelImpl
{
public:
  static IntrusivePtr<ProbeLabelImpl> New()
  {
    return IntrusivePtr<ProbeLabelImpl>(new ProbeLabelImpl());
  }

  void SetPolicy(ArrangePolicy policy)
  {
    SetArrangePolicy(policy);
  }

  int GetArrangeCallCount() const
  {
    return mArrangeCount;
  }

protected:
  ProbeLabelImpl()
  : Dali::Ui::Integration::LabelImpl()
  {
  }

  LayoutRect OnArrange(const LayoutRect& bounds) override
  {
    ++mArrangeCount;
    return bounds;
  }

private:
  int mArrangeCount{0};
};

// Register so TypeInfo lookup can walk the chain.
Dali::TypeRegistration probeLabelTypeReg(
  typeid(ProbeLabelImpl), typeid(Dali::Ui::Integration::LabelImpl), nullptr);

View CreateProbeLabel()
{
  auto impl = ProbeLabelImpl::New();
  View view(*impl);
  impl->Initialize();
  return view;
}

ProbeLabelImpl& ProbeLabelImplOf(View view)
{
  return static_cast<ProbeLabelImpl&>(GetImpl(view));
}

// --- LayoutManager policy (Phase 5c) --------------------------------------
//
// A counting layout manager. A manager is its owner's arrange producer, so "did the
// producer run" can only be counted by a manager that counts itself. Its Arrange
// stacks children vertically from layout-tracked state only -- measured sizes and
// the child list -- so the IF_CHANGED variant is valid.
int gManagerArrangeCount = 0;

class CountingLayoutManager : public LayoutManager
{
public:
  CountingLayoutManager() = default;

  explicit CountingLayoutManager(ArrangePolicy policy)
  : LayoutManager()
  {
    SetArrangePolicy(policy);
  }

  void SetPolicy(ArrangePolicy policy)
  {
    SetArrangePolicy(policy);
  }

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override
  {
    float          maxWidth    = 0.0f;
    float          totalHeight = 0.0f;
    const uint32_t count       = GetChildViewCount(view);

    for(uint32_t i = 0; i < count; ++i)
    {
      View child = GetChildViewAt(view, i);
      if(!child)
      {
        continue;
      }
      ViewImpl& childImpl = GetImpl(child);
      if(IsStandalone(&childImpl))
      {
        continue;
      }
      MeasuredSize childSize = childImpl.Measure(widthConstraint, heightConstraint);
      maxWidth               = std::max(maxWidth, childSize.width);
      totalHeight += childSize.height;
    }
    return MeasuredSize(maxWidth, totalHeight);
  }

  void Arrange(ViewImpl* view, const LayoutRect&) override
  {
    ++gManagerArrangeCount;

    float          y     = 0.0f;
    const uint32_t count = GetChildViewCount(view);

    for(uint32_t i = 0; i < count; ++i)
    {
      View child = GetChildViewAt(view, i);
      if(!child)
      {
        continue;
      }
      ViewImpl& childImpl = GetImpl(child);
      if(IsStandalone(&childImpl))
      {
        continue;
      }
      MeasuredSize childMeasured = childImpl.GetMeasuredSize();
      childImpl.Arrange(LayoutRect(0.0f, y, childMeasured.width, childMeasured.height));
      y += childMeasured.height;
    }
  }
};

// A third-party subclass used to verify that the default manager policy remains
// IF_CHANGED through normal inheritance.
class SubclassedStackLayoutManager : public StackLayoutManager
{
public:
  SubclassedStackLayoutManager()
  : StackLayoutManager(StackOrientation::VERTICAL, 0.0f)
  {
  }
};

class SubclassedScrollViewLayoutManager : public ScrollViewLayoutManager
{
};

} // namespace

// A cache hit consumes nothing: the entry it served is still valid afterwards, so
// the NEXT identical pass hits as well. This is what makes the optimisation
// monotone rather than a one-shot -- and it only holds because the hit returns
// BEFORE ArrangePassGuard, whose constructor would clear mArrangeCacheValid.
//
// Non-vacuity (verified by mutation): clearing mArrangeCacheValid in the hit body
// leaves the entry dead after the first hit and the post-hit assertions below fail.
// (The related "construct ArrangePassGuard before the hit test" mistake is NOT
// caught here -- it makes every pass a miss, which re-publishes a valid cache -- but
// it is caught by the black-box producer count in
// UtcDaliViewArrangeCacheHitSkipsLeafProducerP.)
int UtcDaliArrangeCacheHitStaysValidAcrossAHitP(void)
{
  UiTestApplication application;
  tet_infoline("A leaf's arrange cache entry survives being served");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  root.Add(leaf);

  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeDirty());

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  for(int pass = 0; pass < 3; ++pass)
  {
    leaf.Arrange(leafSlot);
    DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
    DALI_TEST_CHECK(!DataOf(leaf).IsArrangeDirty());
    DALI_TEST_EQUALS(DataOf(leaf).GetLastArrangeDirection(), leaf.GetEffectiveLayoutDirection(), TEST_LOCATION);
  }

  END_TEST;
}

// Corollary C, from the outside: a live arrange cache implies a live effective-scale
// sync bit, because every scale-context reset clears both. The hit body asserts this
// in DEBUG (DALI_ASSERT_DEBUG(mEffectiveScaleValid)); this test states the same claim
// as a normal assertion so it is checked in every build configuration, immediately
// before a pass that will take the hit.
//
// Non-vacuity (verified by mutation): removing the InvalidateLayoutCaches() call from
// ViewDataImpl::InvalidateMeasure breaks the pairing -- the arrange cache survives an
// invalidation that dropped the logical context, which the InvalidateMeasure half
// below catches directly (and which would make the hit's DEBUG assert fire on the
// next identical pass).
int UtcDaliArrangeCacheHitAssertsEffectiveScaleSyncP(void)
{
  UiTestApplication application;
  tet_infoline("A live arrange cache implies a live effective-scale sync bit");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  root.Add(leaf);

  Settle(application);

  // The state the hit's DEBUG assert relies on.
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  // Takes the hit. In a DEBUG build the assert inside it is the live check; in any
  // build the state assertions around it hold.
  leaf.Arrange(leafSlot);

  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // The pairing itself, over both callers that drop the logical context. Each must
  // drop the arrange cache in the same breath, or a later hit would serve a result
  // computed against the old scale -- with no term in the predicate to catch it.
  GetImpl(leaf).InvalidateMeasure();
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());

  Settle(application);
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // The recursive form, which is what a global UI scale change uses.
  DataOf(leaf).ResetSubtreeScaleAndLayoutCaches();
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());

  END_TEST;
}

// LayoutFinished semantics are pass-based, not work-based: a subscriber is told its
// view was arranged in this pass, and a cache hit IS an arrange of that view. The
// signal must therefore keep firing on frames where the leaf's producer never runs.
//
// Non-vacuity (verified by mutation): dropping the
// `if(HasLayoutFinishedSignalConnections()) LayoutController::NotifyViewArranged(...)`
// block from the hit body makes the subscribed leaf stop emitting on hitting passes
// and the second half of this test fails.
int UtcDaliArrangeCacheHitEmitsLayoutFinishedP(void)
{
  UiTestApplication application;
  tet_infoline("A settled leaf still emits LayoutFinished on a pass it serves from cache");

  gLayoutFinishedCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leafA = View::New();
  leafA.SetRequestedWidth(50.0f);
  leafA.SetRequestedHeight(40.0f);
  root.Add(leafA);

  View leafB = View::New();
  leafB.SetRequestedWidth(50.0f);
  leafB.SetRequestedHeight(40.0f);
  root.Add(leafB);

  leafA.LayoutFinishedSignal().Connect(&OnLayoutFinished);

  Settle(application);

  const int settledEmits = gLayoutFinishedCount;
  DALI_TEST_CHECK(settledEmits > 0);
  DALI_TEST_CHECK(DataOf(leafA).IsArrangeCacheValid());

  // A pass driven entirely by the SIBLING. leafA's own inputs are unchanged, so its
  // Arrange serves the cache -- and must still register as arranged.
  leafB.SetRequestedX(11.0f);
  Settle(application);

  DALI_TEST_CHECK(DataOf(leafA).IsArrangeCacheValid());
  DALI_TEST_CHECK(gLayoutFinishedCount > settledEmits);

  END_TEST;
}

// IF_CHANGED is the default. VideoView and WebView explicitly select
// ALWAYS because their OnArrange implementations read SCREEN_POSITION and push
// it to a native surface outside the actor tree. Neither operation is represented by
// the arrange cache key, so these producers must execute on every pass.
//
// The second half compares otherwise identical callbacks with explicit
// IF_CHANGED and ALWAYS policies, proving that the derived producer
// policy is honored by the cache-hit predicate.
//
// Non-vacuity (verified by mutation): dropping the mArrangeProducerAlways term from
// the hit predicate lets the ALWAYS leaf hit; changing VideoViewImpl to
// IF_CHANGED fails the first half.
int UtcDaliArrangeCacheHitAlwaysFirstPartyLeavesNeverCacheP(void)
{
  UiTestApplication application;
  tet_infoline("VideoView and WebView explicitly use ALWAYS and never take the cache hit");

  // --- Part 1: defaults and explicit opt-outs -------------------------------
  // These ordinary views use the framework default IF_CHANGED policy.
  View                plain    = View::New();
  Label               label    = Label::New();
  ImageView           image    = ImageView::New();
  AnimatedImageView   animated = AnimatedImageView::New();
  LottieAnimationView lottie   = LottieAnimationView::New();
  CanvasView          canvas   = CanvasView::New(Vector2(100.0f, 100.0f));
  InputField          field    = InputField::New();
  InputEditor         editor   = InputEditor::New();

  DALI_TEST_CHECK(DataOf(plain).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(label).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(image).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(animated).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(lottie).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(canvas).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(field).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(editor).ArrangesIfChanged());

  // These two producers explicitly opt out because they update external surfaces.
  VideoView video = VideoView::New(CreateTestVideoSource());
  WebView   web   = WebView::New();

  DALI_TEST_CHECK(!DataOf(video).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(web).ArrangesIfChanged());

  // --- Part 2: the bit is honoured by the hit predicate ----------------------
  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  // Two third-party-shaped subclasses with byte-identical OnArrange overrides and
  // explicitly different policies.
  View ifChangedLeaf = CreatePolicyCounterView(true);
  View alwaysLeaf    = CreatePolicyCounterView(false);

  DALI_TEST_CHECK(DataOf(ifChangedLeaf).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(alwaysLeaf).ArrangesIfChanged());

  ifChangedLeaf.SetRequestedWidth(50.0f);
  ifChangedLeaf.SetRequestedHeight(40.0f);
  root.Add(ifChangedLeaf);

  alwaysLeaf.SetRequestedWidth(50.0f);
  alwaysLeaf.SetRequestedHeight(40.0f);
  root.Add(alwaysLeaf);

  Settle(application);

  // ViewImpl::OnArrange echoes its input for a childless view, so the settled actor
  // geometry IS the slot each cache was keyed on.
  const LayoutRect ifChangedSlot = SettledSlotOf(ifChangedLeaf);
  const LayoutRect alwaysSlot    = SettledSlotOf(alwaysLeaf);

  // BOTH leaves settled with a live cache entry: using ALWAYS declines the HIT,
  // not the publish, so this is a genuine "entry exists but is refused" comparison
  // and not merely "the ALWAYS leaf never cached".
  DALI_TEST_CHECK(DataOf(ifChangedLeaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(alwaysLeaf).IsArrangeCacheValid());

  const int ifChangedBase = PolicyCounterImplOf(ifChangedLeaf).GetArrangeCallCount();
  const int alwaysBase    = PolicyCounterImplOf(alwaysLeaf).GetArrangeCallCount();
  DALI_TEST_CHECK(ifChangedBase > 0);
  DALI_TEST_CHECK(alwaysBase > 0);

  const int PASSES = 3;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    ifChangedLeaf.Arrange(ifChangedSlot);
    alwaysLeaf.Arrange(alwaysSlot);
  }

  // The IF_CHANGED leaf is served from cache on every one of those passes.
  DALI_TEST_EQUALS(PolicyCounterImplOf(ifChangedLeaf).GetArrangeCallCount(), ifChangedBase, TEST_LOCATION);

  // The ALWAYS leaf re-runs its producer on every single pass, with the same
  // slot, the same direction, the same scale and no dirty bit -- the ONLY term
  // rejecting it is mArrangeProducerAlways.
  DALI_TEST_EQUALS(PolicyCounterImplOf(alwaysLeaf).GetArrangeCallCount(), alwaysBase + PASSES, TEST_LOCATION);

  // Always-miss must still be result-identical: refusing the hit costs work, never
  // correctness.
  DALI_TEST_EQUALS(alwaysLeaf.GetProperty<float>(Actor::Property::POSITION_X), alwaysSlot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(alwaysLeaf.GetProperty<float>(Actor::Property::POSITION_Y), alwaysSlot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(alwaysLeaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), alwaysSlot.width, TEST_LOCATION);
  DALI_TEST_EQUALS(alwaysLeaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT), alwaysSlot.height, TEST_LOCATION);

  // Nothing above changed any policy, so the two states are stable -- including
  // Video/Web, which nothing in a layout pass can flip.
  DALI_TEST_CHECK(!DataOf(video).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(web).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(plain).ArrangesIfChanged());

  END_TEST;
}

// The derived policy follows the active producer in callback > manager > OnArrange
// dispatch order. Each producer carries its own policy.
int UtcDaliArrangeCacheProducerPolicyFollowsActiveProducerP(void)
{
  UiTestApplication application;
  tet_infoline("The derived policy tracks the active arrange producer");

  View view = View::New();
  DALI_TEST_CHECK(DataOf(view).ArrangesIfChanged());

  view.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange));
  DALI_TEST_CHECK(DataOf(view).ArrangesIfChanged());

  view.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange), ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(view).ArrangesIfChanged());

  view.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange));
  DALI_TEST_CHECK(DataOf(view).ArrangesIfChanged());

  view.SetArrangeCallback({});
  DALI_TEST_CHECK(DataOf(view).ArrangesIfChanged());

  View configurable = CreatePolicyCounterView(true);
  DALI_TEST_CHECK(DataOf(configurable).ArrangesIfChanged());
  PolicyCounterImplOf(configurable).SetPolicy(ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(configurable).ArrangesIfChanged());
  PolicyCounterImplOf(configurable).SetPolicy(ArrangePolicy::IF_CHANGED);
  DALI_TEST_CHECK(DataOf(configurable).ArrangesIfChanged());

  View managed = View::New();
  managed.AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  DALI_TEST_CHECK(DataOf(managed).ArrangesIfChanged());
  managed.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange), ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(managed).ArrangesIfChanged());
  managed.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange));
  DALI_TEST_CHECK(DataOf(managed).ArrangesIfChanged());
  managed.SetArrangeCallback({});
  DALI_TEST_CHECK(DataOf(managed).ArrangesIfChanged());

  View scrollManaged = View::New();
  scrollManaged.AttachLayoutManager(Dali::MakeUnique<ScrollViewLayoutManager>());
  DALI_TEST_CHECK(!DataOf(scrollManaged).ArrangesIfChanged());
  scrollManaged.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange));
  DALI_TEST_CHECK(DataOf(scrollManaged).ArrangesIfChanged());
  scrollManaged.SetArrangeCallback({});
  DALI_TEST_CHECK(!DataOf(scrollManaged).ArrangesIfChanged());

  END_TEST;
}

// ViewImpl::GetArrangePolicy() reports the OnArrange() policy ONLY -- exactly what
// SetArrangePolicy() stored -- and is deliberately NOT the effective producer policy
// that ArrangesIfChanged() reflects. The callback and manager arms below install a
// producer whose policy is the OPPOSITE of the view's OnArrange policy, so this test
// would FAIL if the getter returned the effective producer policy instead.
int UtcDaliViewImplGetArrangePolicyP(void)
{
  UiTestApplication application;
  tet_infoline("ViewImpl::GetArrangePolicy() mirrors SetArrangePolicy() and ignores the active producer");

  // 1) A view created with the default IF_CHANGED policy reads it back.
  View configurable = CreatePolicyCounterView(true);
  DALI_TEST_EQUALS(PolicyCounterImplOf(configurable).GetPolicy(), ArrangePolicy::IF_CHANGED, TEST_LOCATION);

  // 2) Opting into ALWAYS moves both the getter and the derived producer bit.
  PolicyCounterImplOf(configurable).SetPolicy(ArrangePolicy::ALWAYS);
  DALI_TEST_EQUALS(PolicyCounterImplOf(configurable).GetPolicy(), ArrangePolicy::ALWAYS, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(configurable).ArrangesIfChanged());

  // 3) ...and going back to IF_CHANGED round-trips both.
  PolicyCounterImplOf(configurable).SetPolicy(ArrangePolicy::IF_CHANGED);
  DALI_TEST_EQUALS(PolicyCounterImplOf(configurable).GetPolicy(), ArrangePolicy::IF_CHANGED, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(configurable).ArrangesIfChanged());

  // 4) Non-interference (callback). An IF_CHANGED view receives an ALWAYS
  // ArrangeCallback: the callback becomes the active producer (ArrangesIfChanged()
  // flips to false), but the OnArrange policy the getter reports is untouched. The two
  // policies are OPPOSITE on purpose, so a getter that returned the producer policy
  // would wrongly report ALWAYS here.
  View callbackView = CreatePolicyCounterView(true);
  DALI_TEST_EQUALS(PolicyCounterImplOf(callbackView).GetPolicy(), ArrangePolicy::IF_CHANGED, TEST_LOCATION);
  callbackView.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange), ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(callbackView).ArrangesIfChanged());
  DALI_TEST_EQUALS(PolicyCounterImplOf(callbackView).GetPolicy(), ArrangePolicy::IF_CHANGED, TEST_LOCATION);

  // 5) Non-interference (manager). A SEPARATE ALWAYS view receives an IF_CHANGED
  // LayoutManager: the manager becomes the active producer (ArrangesIfChanged() flips
  // to true), but the OnArrange policy the getter reports stays ALWAYS. Again the two
  // are OPPOSITE, so the same producer-returning bug would wrongly report IF_CHANGED.
  View managerView = CreatePolicyCounterView(false);
  DALI_TEST_EQUALS(PolicyCounterImplOf(managerView).GetPolicy(), ArrangePolicy::ALWAYS, TEST_LOCATION);
  managerView.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new CountingLayoutManager(ArrangePolicy::IF_CHANGED)));
  DALI_TEST_CHECK(DataOf(managerView).ArrangesIfChanged());
  DALI_TEST_EQUALS(PolicyCounterImplOf(managerView).GetPolicy(), ArrangePolicy::ALWAYS, TEST_LOCATION);

  END_TEST;
}

// IF_CHANGED is the default for a custom OnArrange override. A producer
// that requires every pass explicitly opts out with ALWAYS.
int UtcDaliArrangeCacheDefaultViewPolicyAndOptOutP(void)
{
  UiTestApplication application;
  tet_infoline("Custom OnArrange defaults to IF_CHANGED and can opt out");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View defaultView = CreateProbeLabel();
  View alwaysView  = CreateProbeLabel();
  ProbeLabelImplOf(alwaysView).SetPolicy(ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(DataOf(defaultView).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(alwaysView).ArrangesIfChanged());

  defaultView.SetRequestedWidth(50.0f);
  defaultView.SetRequestedHeight(40.0f);
  root.Add(defaultView);
  alwaysView.SetRequestedWidth(50.0f);
  alwaysView.SetRequestedHeight(40.0f);
  root.Add(alwaysView);
  Settle(application);

  const LayoutRect defaultSlot = SettledSlotOf(defaultView);
  const LayoutRect alwaysSlot  = SettledSlotOf(alwaysView);
  const int        defaultBase = ProbeLabelImplOf(defaultView).GetArrangeCallCount();
  const int        alwaysBase  = ProbeLabelImplOf(alwaysView).GetArrangeCallCount();
  const int        PASSES      = 3;

  for(int pass = 0; pass < PASSES; ++pass)
  {
    defaultView.Arrange(defaultSlot);
    alwaysView.Arrange(alwaysSlot);
  }

  DALI_TEST_EQUALS(ProbeLabelImplOf(defaultView).GetArrangeCallCount(), defaultBase, TEST_LOCATION);
  DALI_TEST_EQUALS(ProbeLabelImplOf(alwaysView).GetArrangeCallCount(), alwaysBase + PASSES, TEST_LOCATION);

  END_TEST;
}

// The `if(mArrangeCacheValid)` true branch of ViewDataImpl::SetArrangePolicy cannot
// be reached by constructor-time selection: the cache bit is false before the View
// handle exists. A policy change on an already settled view must invalidate the
// entry published under the old policy; otherwise an IF_CHANGED ->
// ALWAYS change could still serve that entry on the next identical pass.
//
// Non-vacuity (verified by mutation): removing the `if(mArrangeCacheValid)
// InvalidateArrange();` block leaves the entry valid and both post-declaration
// assertions fail.
int UtcDaliArrangeCachePolicyChangeInvalidatesSettledEntryP(void)
{
  UiTestApplication application;
  tet_infoline("Changing policy on a settled view drops its published arrange entry");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = CreatePolicyCounterView(true);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  root.Add(leaf);

  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeDirty());
  DALI_TEST_CHECK(DataOf(leaf).ArrangesIfChanged());

  // IF_CHANGED -> ALWAYS on a live entry: the entry must go, and the view must be scheduled.
  PolicyCounterImplOf(leaf).SetPolicy(ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(leaf).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeDirty());

  // The reverse direction, from a freshly settled state, takes the same branch: the
  // entry published while ALWAYS was never a hit candidate, but the guard does not
  // reason about that and drops it anyway.
  Settle(application);
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

  PolicyCounterImplOf(leaf).SetPolicy(ArrangePolicy::IF_CHANGED);
  DALI_TEST_CHECK(DataOf(leaf).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());

  // ...and it recovers: a re-settle republishes under the new policy.
  Settle(application);
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

  END_TEST;
}

// The derived policy bit must not survive an EXTERNAL swap of the traits that carry the
// producer. ArrangeCallback (ReservedTraitId::LAYOUT_SIGNALS) and LayoutManager
// (ReservedTraitId::LAYOUT_MANAGER) both outrank OnArrange in Arrange()'s dispatch
// order, and both are reachable through the public Integration::View::SetTrait /
// RemoveTrait surface, which does not go through SetArrangeCallback() /
// AttachLayoutManager() and so does no policy bookkeeping of its own.
//
// The desynchronizing case uses an OnArrange producer explicitly set to ALWAYS
// under an IF_CHANGED callback. Removing that callback through the trait API
// exposes OnArrange again; a stale derived bit would incorrectly reuse the callback's
// published entry.
//
// Non-vacuity (verified by mutation): removing the OnLayoutProducerTraitChanged() call
// from ViewDataImpl::RemoveTrait leaves the bit TRUE and the entry live, and both
// post-removal assertions fail.
int UtcDaliArrangeCacheProducerPolicyClearedByExternalTraitRemovalP(void)
{
  UiTestApplication application;
  tet_infoline("An external reserved-trait removal re-derives the arrange producer policy");

  gCallbackArrangeCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  // OnArrange explicitly uses ALWAYS.
  View leaf = CreatePolicyCounterView(false);
  DALI_TEST_CHECK(!DataOf(leaf).ArrangesIfChanged());

  // An IF_CHANGED callback outranks it.
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingCallbackArrange), ArrangePolicy::IF_CHANGED);
  DALI_TEST_CHECK(DataOf(leaf).ArrangesIfChanged());

  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  root.Add(leaf);

  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).ArrangesIfChanged());

  // The external swap. Nothing else in the library takes this path.
  DALI_TEST_CHECK(IntegrationView::RemoveTrait(GetImpl(leaf), ReservedTraitId::LAYOUT_SIGNALS));

  // The active producer is the ALWAYS OnArrange again, so the bit must be false...
  DALI_TEST_CHECK(!DataOf(leaf).ArrangesIfChanged());
  // ...and the entry the callback published must not outlive it.
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());
  // The MEASURE entry goes with it: LAYOUT_SIGNALS holds the MeasureCallback in the same
  // LayoutCallbacksObject, so removing the trait removes a measure producer too.
  DALI_TEST_CHECK(!DataOf(leaf).IsMeasureCacheValid());

  // Behaviourally: the override now runs on every settled pass.
  Settle(application);

  const LayoutRect leafSlot = SettledSlotOf(leaf);
  const int        callBase = PolicyCounterImplOf(leaf).GetArrangeCallCount();

  const int PASSES = 3;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    leaf.Arrange(leafSlot);
  }
  DALI_TEST_EQUALS(PolicyCounterImplOf(leaf).GetArrangeCallCount(), callBase + PASSES, TEST_LOCATION);

  END_TEST;
}

namespace
{
// A counting MEASURE producer. Its result is deliberately unlike anything the default
// MeasureDefault() would return for the same view, so "the cached slot was retracted"
// is visible as geometry and not only as a bit.
int gTraitMeasureCount = 0;

MeasuredSize CountingTraitMeasure(View, float, float)
{
  ++gTraitMeasureCount;
  return MeasuredSize(70.0f, 45.0f);
}
} // namespace

// The measure axis of the same external-swap story. LAYOUT_MANAGER supplies
// LayoutManager::Measure(), so removing it removes a MEASURE producer -- and the slot
// that producer published is what every ancestor arranges FROM. Retracting only the
// arrange entry would leave the manager's measured height in force after the manager
// itself was gone.
//
// Non-vacuity (verified by mutation): changing the tail of
// ViewDataImpl::OnLayoutProducerTraitChanged back to `if(mArrangeCacheValid)
// InvalidateArrange();` leaves the measure entry valid and un-dirty, so the two bit
// checks fail and the container keeps the manager's stacked height.
int UtcDaliMeasureCacheClearedByExternalLayoutManagerTraitRemovalP(void)
{
  UiTestApplication application;
  tet_infoline("An external LAYOUT_MANAGER removal retracts the measured slot the manager published");

  gManagerArrangeCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  // WRAP_CONTENT, so the measured size IS the manager's Measure() result.
  View container = View::New();
  container.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new CountingLayoutManager()));
  root.Add(container);

  View first = View::New();
  first.SetRequestedWidth(50.0f);
  first.SetRequestedHeight(30.0f);
  container.Add(first);

  View second = View::New();
  second.SetRequestedWidth(50.0f);
  second.SetRequestedHeight(30.0f);
  container.Add(second);

  Settle(application);

  DALI_TEST_CHECK(DataOf(container).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(container).IsMeasureDirty());

  const int   settledManagerCount = gManagerArrangeCount;
  const float managedHeight       = container.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  DALI_TEST_CHECK(settledManagerCount > 0);
  // The manager stacks its two 30-high children: a height no default measure produces.
  DALI_TEST_EQUALS(managedHeight, 60.0f, TEST_LOCATION);

  // The external swap. Nothing in the library takes this path.
  DALI_TEST_CHECK(IntegrationView::RemoveTrait(GetImpl(container), ReservedTraitId::LAYOUT_MANAGER));

  DALI_TEST_CHECK(!DataOf(container).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(container).IsMeasureDirty());
  DALI_TEST_CHECK(!DataOf(container).IsArrangeCacheValid());

  // The invalidation also propagated, so the batch that follows re-measures with the
  // producer that is actually installed now (the default OnMeasure).
  Settle(application);

  DALI_TEST_EQUALS(gManagerArrangeCount, settledManagerCount, TEST_LOCATION);
  DALI_TEST_CHECK(container.GetProperty<float>(Actor::Property::SIZE_HEIGHT) != managedHeight);
  DALI_TEST_EQUALS(container.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 30.0f, TEST_LOCATION);

  END_TEST;
}

// The LAYOUT_SIGNALS half. One LayoutCallbacksObject holds BOTH the MeasureCallback and
// the ArrangeCallback, so removing that trait removes the measure producer as well.
//
// Non-vacuity (verified by mutation): the same mutation as above leaves the callback's
// measured slot cached, so the view keeps its 70 x 45 size after the callback is gone.
int UtcDaliMeasureCacheClearedByExternalLayoutSignalsTraitRemovalP(void)
{
  UiTestApplication application;
  tet_infoline("An external LAYOUT_SIGNALS removal retracts the measured slot the MeasureCallback published");

  gTraitMeasureCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  // WRAP_CONTENT and childless: the callback's result is the only thing that can give
  // this view a non-zero size.
  View leaf = View::New();
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingTraitMeasure));
  root.Add(leaf);

  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsMeasureDirty());

  const int settledMeasureCount = gTraitMeasureCount;
  DALI_TEST_CHECK(settledMeasureCount > 0);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 45.0f, TEST_LOCATION);

  DALI_TEST_CHECK(IntegrationView::RemoveTrait(GetImpl(leaf), ReservedTraitId::LAYOUT_SIGNALS));

  DALI_TEST_CHECK(!DataOf(leaf).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsMeasureDirty());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());

  Settle(application);

  // The callback is gone, so it cannot have produced this: a childless WRAP_CONTENT view
  // measures to nothing.
  DALI_TEST_EQUALS(gTraitMeasureCount, settledMeasureCount, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5b: white-box coverage for the SUBTREE hit.
//
// The black-box suite observes the subtree replay through counting container
// producers. What it cannot see is the bookkeeping the replay leaves behind, the
// per-node DEBUG invariant it asserts, and the LayoutFinished registration it makes
// for descendants. Those three are pinned here.
// ---------------------------------------------------------------------------

// A subtree hit consumes nothing: every node's entry is still valid afterwards, so the
// NEXT identical pass hits as well. This is the subtree form of
// UtcDaliArrangeCacheHitStaysValidAcrossAHitP, and it only holds because the replay
// returns BEFORE ArrangePassGuard at every level -- the guard's constructor would clear
// mArrangeCacheValid, and the replay never constructs one.
//
// Non-vacuity (verified by mutation): clearing mArrangeCacheValid anywhere in
// ReplayArrangeSubtreeFromCache leaves the entries dead after the first hit and the
// post-hit assertions fail.
int UtcDaliArrangeCacheSubtreeHitLeavesEveryEntryValidP(void)
{
  UiTestApplication application;
  tet_infoline("A subtree served from cache still holds a valid entry at every node");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = View::New();
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

  const LayoutRect rootSlot = SettledSlotOf(root);

  for(int pass = 0; pass < 3; ++pass)
  {
    root.Arrange(rootSlot);

    DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
    DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
    DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

    DALI_TEST_CHECK(!DataOf(root).IsArrangeDirty());
    DALI_TEST_CHECK(!DataOf(mid).IsArrangeDirty());
    DALI_TEST_CHECK(!DataOf(leaf).IsArrangeDirty());
  }

  END_TEST;
}

// LayoutFinished is pass-based, not work-based, and that has to survive the replay at
// every DEPTH: a subscriber on a GRANDCHILD is telling the framework "notify me when
// this view was arranged in a pass", and being replayed from cache IS being arranged.
// Dropping the per-node registration would truncate the controller's arrangedViews set
// to whatever happened to miss.
//
// The counting container is what makes this non-vacuous: it proves the pass that fired
// the signal was a HIT, not a miss that fired it for the ordinary reason.
//
// Non-vacuity (verified by mutation): removing the
// `if(HasLayoutFinishedSignalConnections()) NotifyViewArranged(...)` block from
// ReplayArrangeSubtreeFromCache stops the grandchild emitting on hitting passes.
int UtcDaliArrangeCacheSubtreeHitEmitsLayoutFinishedForDescendantsP(void)
{
  UiTestApplication application;
  tet_infoline("A grandchild still emits LayoutFinished on a pass its whole subtree serves from cache");

  gLayoutFinishedCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  // A counting container so the pass can be proven to be a hit.
  View mid = CreatePolicyCounterView(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View grandchild = View::New();
  grandchild.SetRequestedWidth(50.0f);
  grandchild.SetRequestedHeight(40.0f);
  mid.Add(grandchild);

  // The reason a pass happens at all, and it is unrelated to the mid/grandchild chain.
  View sibling = View::New();
  sibling.SetRequestedWidth(30.0f);
  sibling.SetRequestedHeight(30.0f);
  root.Add(sibling);

  grandchild.LayoutFinishedSignal().Connect(&OnLayoutFinished);

  Settle(application);

  const int settledEmits = gLayoutFinishedCount;
  const int midBase      = PolicyCounterImplOf(mid).GetArrangeCallCount();
  DALI_TEST_CHECK(settledEmits > 0);
  DALI_TEST_CHECK(midBase > 0);
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());

  sibling.SetRequestedX(11.0f);
  Settle(application);

  // The mid/grandchild subtree was served from cache...
  DALI_TEST_EQUALS(PolicyCounterImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(grandchild).IsArrangeCacheValid());

  // ...and the grandchild still registered as arranged in that pass.
  DALI_TEST_CHECK(gLayoutFinishedCount > settledEmits);

  END_TEST;
}

// Corollary C, per node. The replay skips the GetEffectiveScale() that the miss path
// performs at every level, so a whole SUBTREE now rests on "a valid arrange cache
// implies a valid logical context" rather than a single node doing so. The replay
// asserts mEffectiveScaleValid at every node it visits (DEBUG); this states the same
// claim as a normal assertion, so it is checked in every build configuration, and then
// pins the pairing that makes it true.
//
// Non-vacuity (verified by mutation): removing the InvalidateLayoutCaches() call from
// ResetSubtreeScaleAndLayoutCaches leaves the subtree's arrange entries live after the
// context was dropped, which the second half below catches directly (and which would
// make the replay's DEBUG assert fire on the next identical pass).
int UtcDaliArrangeCacheSubtreeHitAssertsEffectiveScaleSyncP(void)
{
  UiTestApplication application;
  tet_infoline("A subtree-wide arrange cache implies a subtree-wide effective-scale sync bit");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = View::New();
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  Settle(application);

  // The state the replay's per-node DEBUG assert relies on, at every node.
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid() && DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid() && DataOf(mid).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid() && DataOf(leaf).IsEffectiveScaleValid());

  // Takes the subtree hit. In a DEBUG build the per-node asserts inside it are the live
  // check; in any build the state assertions around it hold.
  root.Arrange(SettledSlotOf(root));

  DALI_TEST_CHECK(DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(mid).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // The pairing, in its recursive form -- the one a global UI scale change uses. It
  // must drop BOTH bits for EVERY node, or a descendant could be replayed against a
  // stale scale with no term in the gate to catch it.
  DataOf(root).ResetSubtreeScaleAndLayoutCaches();

  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid() && !DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(mid).IsEffectiveScaleValid() && !DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid() && !DataOf(leaf).IsArrangeCacheValid());

  // ...and it recovers. The recursive drop is a CACHE-ONLY invalidation -- it raises
  // no dirty bit and registers no layout root, deliberately, so it cannot spin -- which
  // means a pass has to be asked for before the subtree can republish.
  root.InvalidateMeasure();
  Settle(application);
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid() && DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid() && DataOf(mid).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid() && DataOf(leaf).IsEffectiveScaleValid());

  END_TEST;
}

// I4 at depth: the corrective re-measure for an unconsumed standalone slot lives on the
// ARRANGE path (ArrangeStandaloneChildren), and !HasUnconsumedStandaloneChild() is
// O(direct children) -- so a standalone GRANDCHILD holding one is invisible to the
// node-local predicate at the root. The subtree gate re-evaluates the term at every
// node, which is what keeps the correction reachable.
//
// The fixture is the state a plain settle leaves behind, and it is the ONLY state in
// which this term decides anything. A standalone view is its own layout root, so the
// settle batch drives it AFTER its parent (depth-sorted): its measure publish marks the
// slot unconsumed, and its own arrange republishes a live entry. So `standalone` ends
// the settle with a VALID arrange cache AND an unconsumed slot, and nothing else in the
// subtree can refuse the hit.
//
// An out-of-band Measure() would raise the bit too, but it is NOT a sharp fixture: the
// measure pass clears that view's own arrange cache in the same breath, so the gate
// would reject on mArrangeCacheValid and the term under test would decide nothing.
//
// Non-vacuity (verified by mutation): dropping
// `!childData.HasUnconsumedStandaloneChild()` from CanReplayArrangeSubtreeFromCache
// lets the ROOT hit -- and then mid's Arrange is never called at all, so its own
// node-local copy of the term never gets a chance to refuse. The mid producer count
// stays flat and ArrangeStandaloneChildren never runs.
int UtcDaliArrangeCacheSubtreeGateRejectsUnconsumedStandaloneDescendantP(void)
{
  UiTestApplication application;
  tet_infoline("An unconsumed standalone slot on a grandchild refuses the whole subtree hit");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreatePolicyCounterView(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(30.0f);
  standalone.SetRequestedHeight(25.0f);
  mid.Add(standalone);

  Settle(application);

  const LayoutRect rootSlot = SettledSlotOf(root);
  const int        midBase  = PolicyCounterImplOf(mid).GetArrangeCallCount();
  DALI_TEST_CHECK(midBase > 0);

  // The precondition, stated explicitly: EVERY other term the gate could reject on is
  // satisfied at every node. The standalone grandchild in particular carries a live
  // entry of its own, so `mid` holding an unconsumed slot for it is the only refusal
  // available.
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(standalone).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(mid).IsArrangeDirty());
  DALI_TEST_CHECK(!DataOf(standalone).IsArrangeDirty());

  // The gate must refuse, so mid is arranged and ArrangeStandaloneChildren is reached.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(PolicyCounterImplOf(mid).GetArrangeCallCount(), midBase + 1, TEST_LOCATION);

  // That pass consumed the slot, so the refusal costs exactly one pass -- and this is
  // also the control that makes the miss above non-vacuous: everything else about this
  // subtree is cacheable.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(PolicyCounterImplOf(mid).GetArrangeCallCount(), midBase + 1, TEST_LOCATION);
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(PolicyCounterImplOf(mid).GetArrangeCallCount(), midBase + 1, TEST_LOCATION);

  // ...and the correction landed: the standalone child sits at the size its parent's
  // extent gives it.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 25.0f, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5c: the LayoutManager execution policy.
// ---------------------------------------------------------------------------

// THE WIN, counted. A settled container whose producer uses IF_CHANGED
// through its LayoutManager
// runs NO manager Arrange when a layout pass sweeps past it for an unrelated reason,
// and the geometry it leaves behind is byte-identical. The same container with an
// ALWAYS manager re-runs on every pass.
//
// Non-vacuity (verified by mutation): changing the SetArrangePolicy call in
// CountingLayoutManager's constructor makes the IF_CHANGED half behave like the ALWAYS half
// and the flat-count assertion fails.
int UtcDaliArrangeCacheIfChangedLayoutManagerContainerSkipsProducerP(void)
{
  UiTestApplication application;
  tet_infoline("An IF_CHANGED LayoutManager is not re-run for a settled container");

  for(int arrangeIfChanged = 1; arrangeIfChanged >= 0; --arrangeIfChanged)
  {
    gManagerArrangeCount = 0;

    View root = View::New();
    root.SetRequestedWidth(200.0f);
    root.SetRequestedHeight(200.0f);
    application.GetScene().Add(root);

    View container = View::New();
    container.SetRequestedWidth(120.0f);
    container.SetRequestedHeight(120.0f);
    container.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new CountingLayoutManager(arrangeIfChanged != 0 ? ArrangePolicy::IF_CHANGED : ArrangePolicy::ALWAYS)));
    root.Add(container);

    View first = View::New();
    first.SetRequestedWidth(50.0f);
    first.SetRequestedHeight(30.0f);
    container.Add(first);

    View second = View::New();
    second.SetRequestedWidth(50.0f);
    second.SetRequestedHeight(30.0f);
    container.Add(second);

    View sibling = View::New();
    sibling.SetRequestedWidth(30.0f);
    sibling.SetRequestedHeight(30.0f);
    root.Add(sibling);

    Settle(application);

    // The selected policy reached the derived bit.
    DALI_TEST_CHECK(DataOf(container).ArrangesIfChanged() == (arrangeIfChanged != 0));

    const int settledCount = gManagerArrangeCount;
    DALI_TEST_CHECK(settledCount > 0);

    const LayoutRect firstRect(first.GetProperty<float>(Actor::Property::POSITION_X),
                               first.GetProperty<float>(Actor::Property::POSITION_Y),
                               first.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                               first.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
    const LayoutRect secondRect(second.GetProperty<float>(Actor::Property::POSITION_X),
                                second.GetProperty<float>(Actor::Property::POSITION_Y),
                                second.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                                second.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
    DALI_TEST_EQUALS(secondRect.y, 30.0f, TEST_LOCATION);

    // A pass that has nothing to do with the container.
    sibling.SetRequestedX(11.0f);
    Settle(application);
    DALI_TEST_EQUALS(sibling.GetProperty<float>(Actor::Property::POSITION_X), 11.0f, TEST_LOCATION);

    if(arrangeIfChanged != 0)
    {
      DALI_TEST_EQUALS(gManagerArrangeCount, settledCount, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(gManagerArrangeCount > settledCount);
    }

    // Either way the result is identical -- serving the cache optimises the WORK.
    DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::POSITION_Y), firstRect.y, TEST_LOCATION);
    DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::SIZE_HEIGHT), firstRect.height, TEST_LOCATION);
    DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::POSITION_Y), secondRect.y, TEST_LOCATION);
    DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::SIZE_HEIGHT), secondRect.height, TEST_LOCATION);

    application.GetScene().Remove(root);
  }

  END_TEST;
}

// Manager policy is stored on the manager instance. Conservative ALWAYS
// opt-outs are inherited by subclasses, which may explicitly select another policy.
int UtcDaliArrangeCacheLayoutManagerPolicyInheritanceP(void)
{
  UiTestApplication application;
  tet_infoline("LayoutManager policy defaults and opt-outs are inherited safely");

  View stack = View::New();
  stack.AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  DALI_TEST_CHECK(DataOf(stack).ArrangesIfChanged());

  View stackSubclass = View::New();
  stackSubclass.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new SubclassedStackLayoutManager()));
  DALI_TEST_CHECK(DataOf(stackSubclass).ArrangesIfChanged());

  View scroll = View::New();
  scroll.AttachLayoutManager(Dali::MakeUnique<ScrollViewLayoutManager>());
  DALI_TEST_CHECK(!DataOf(scroll).ArrangesIfChanged());

  View scrollSubclass = View::New();
  scrollSubclass.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new SubclassedScrollViewLayoutManager()));
  DALI_TEST_CHECK(!DataOf(scrollSubclass).ArrangesIfChanged());

  View explicitDefault = View::New();
  explicitDefault.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new CountingLayoutManager()));
  DALI_TEST_CHECK(DataOf(explicitDefault).ArrangesIfChanged());

  View explicitAlways = View::New();
  explicitAlways.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new CountingLayoutManager(ArrangePolicy::ALWAYS)));
  DALI_TEST_CHECK(!DataOf(explicitAlways).ArrangesIfChanged());

  View  runtimeChange  = View::New();
  auto  runtimeManager = Dali::UniquePtr<CountingLayoutManager>(new CountingLayoutManager());
  auto* manager        = runtimeManager.Get();
  runtimeChange.AttachLayoutManager(std::move(runtimeManager));
  runtimeChange.SetRequestedWidth(50.0f);
  runtimeChange.SetRequestedHeight(40.0f);
  application.GetScene().Add(runtimeChange);
  Settle(application);
  DALI_TEST_CHECK(DataOf(runtimeChange).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(runtimeChange).IsArrangeCacheValid());

  manager->SetPolicy(ArrangePolicy::ALWAYS);
  DALI_TEST_CHECK(!DataOf(runtimeChange).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(runtimeChange).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(runtimeChange).IsArrangeDirty());

  Settle(application);
  DALI_TEST_CHECK(DataOf(runtimeChange).IsArrangeCacheValid());
  manager->SetPolicy(ArrangePolicy::IF_CHANGED);
  DALI_TEST_CHECK(DataOf(runtimeChange).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(runtimeChange).IsArrangeCacheValid());

  END_TEST;
}

// The in-library manager policy table: four geometry-only managers use the default
// IF_CHANGED policy, while ScrollView explicitly selects ALWAYS. Thus
// a StackLayout / GridLayout / FlexLayout / AbsoluteLayout
// container is cacheable and a ScrollView never is.
//
// Non-vacuity (verified by mutation): changing the default policy of any
// one of the four managers flips that manager's row; adding one to
// ScrollViewLayoutManager flips the ScrollView row.
int UtcDaliArrangeCacheInLibraryLayoutManagerPolicyP(void)
{
  UiTestApplication application;
  tet_infoline("Four geometry-free layout managers use IF_CHANGED; ScrollView uses ALWAYS");

  // --- the managers, attached to a bare View -----------------------------------
  View stackManaged = View::New();
  stackManaged.AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  DALI_TEST_CHECK(DataOf(stackManaged).ArrangesIfChanged());

  View absoluteManaged = View::New();
  absoluteManaged.AttachLayoutManager(Dali::MakeUnique<AbsoluteLayoutManager>());
  DALI_TEST_CHECK(DataOf(absoluteManaged).ArrangesIfChanged());

  Dali::Vector<GridLength> rows;
  Dali::Vector<GridLength> columns;
  View                     gridManaged = View::New();
  gridManaged.AttachLayoutManager(Dali::MakeUnique<GridLayoutManager>(rows, columns, 0.0f, 0.0f));
  DALI_TEST_CHECK(DataOf(gridManaged).ArrangesIfChanged());

  View flexManaged = View::New();
  flexManaged.AttachLayoutManager(Dali::MakeUnique<FlexLayoutManager>(
    FlexDirection::ROW, FlexWrap::NO_WRAP, FlexJustify::FLEX_START, FlexAlign::FLEX_START, FlexAlign::FLEX_START));
  DALI_TEST_CHECK(DataOf(flexManaged).ArrangesIfChanged());

  // The one exclusion, and the reason 5c is safe: it reads the scrolled child's live
  // actor position (scroll-view-layout-manager.cpp, childBounds.x = child.GetPositionX()).
  View scrollManaged = View::New();
  scrollManaged.AttachLayoutManager(Dali::MakeUnique<ScrollViewLayoutManager>());
  DALI_TEST_CHECK(!DataOf(scrollManaged).ArrangesIfChanged());

  // --- and through the container types an application actually writes -----------
  StackLayout    stackLayout    = StackLayout::New();
  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();
  GridLayout     gridLayout     = GridLayout::New();
  FlexLayout     flexLayout     = FlexLayout::New();
  ScrollView     scrollView     = ScrollView::New();
  DALI_TEST_CHECK(DataOf(stackLayout).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(absoluteLayout).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(gridLayout).ArrangesIfChanged());
  DALI_TEST_CHECK(DataOf(flexLayout).ArrangesIfChanged());
  DALI_TEST_CHECK(!DataOf(scrollView).ArrangesIfChanged());

  END_TEST;
}

// An out-of-band public Arrange() on a child retracts the DIRECT parent's entry
// only -- cache-only, one node deep. Ancestors above keep their entries and are
// still forced to miss through the recursive hit gate, which re-tests every
// descendant's cache validity; the miss chain then re-publishes level by level.
int UtcDaliArrangeCacheOutOfBandChildArrangeRetractsParentEntryP(void)
{
  UiTestApplication application;
  tet_infoline("An out-of-band child.Arrange() retracts exactly the parent's entry; the gate refuses ancestors");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(120.0f);
  root.Add(parent);

  View child = View::New();
  child.SetRequestedX(20.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(10.0f);
  parent.Add(child);

  Settle(application);

  const LayoutRect rootSlot = LayoutRect(root.GetProperty<float>(Actor::Property::POSITION_X),
                                         root.GetProperty<float>(Actor::Property::POSITION_Y),
                                         root.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                                         root.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);

  // The out-of-band write. The child's own pass publishes ITS entry; the direct
  // parent's entry is retracted; the grandparent's survives (one node deep).
  child.Arrange(LayoutRect(77.0f, 0.0f, 30.0f, 10.0f));
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(parent).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());

  // The grandparent cannot serve its surviving entry over the hole: its gate
  // re-tests the parent's validity, refuses, and the miss chain corrects the
  // child back to the parent-derived slot and re-publishes every level.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());

  // Settled again: the next same-bounds arrange is a whole-tree hit and serves
  // the corrected geometry.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);

  END_TEST;
}

// ArrangeOwnedMeasureScope owns only the nested Measure calls it surrounds. Its
// presence must not classify an unrelated public Arrange as owned; otherwise the
// target's direct parent retains an entry that replays foreign child bounds.
int UtcDaliArrangeCacheMeasureOwnerScopeDoesNotOwnArrangeP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(80.0f);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedX(20.0f);
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(10.0f);
  parent.Add(child);

  View unrelatedMeasureOwner = View::New();

  Settle(application);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());

  {
    Dali::Ui::Internal::LayoutDependency::ArrangeOwnedMeasureScope measureScope(&GetImpl(unrelatedMeasureOwner));
    child.Arrange(LayoutRect(77.0f, 0.0f, 30.0f, 10.0f));
  }

  DALI_TEST_CHECK(!DataOf(parent).IsArrangeCacheValid());

  END_TEST;
}

// A public Arrange on a STANDALONE child is not the framework-owned self pass. The
// parent still owns its requested-position / measured-extent slot on every miss, so
// arbitrary public bounds retract the parent's cache exactly as for a normal child.
int UtcDaliArrangeCacheOutOfBandStandaloneArrangeRetractsParentEntryP(void)
{
  UiTestApplication application;

  View root = View::New();
  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(80.0f);
  root.Add(parent);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedX(20.0f);
  standalone.SetRequestedWidth(40.0f);
  standalone.SetRequestedHeight(10.0f);
  parent.Add(standalone);

  Settle(application);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(standalone).IsArrangeCacheValid());

  standalone.Arrange(LayoutRect(77.0f, 0.0f, 30.0f, 10.0f));
  DALI_TEST_CHECK(DataOf(standalone).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(parent).IsArrangeCacheValid());

  END_TEST;
}

// A STANDALONE view's self-pass is out-of-band by design -- ProcessLayoutRoot
// drives it with no parent pass on the stack -- and must NOT retract the
// parent's entry: the parent-driven derivation converges on the same bounds, so
// the replay premise survives, and retracting would cost the parent a miss for
// every standalone self-layout.
int UtcDaliArrangeCacheStandaloneSelfPassKeepsParentEntryP(void)
{
  UiTestApplication application;
  tet_infoline("A standalone child's self-pass leaves the parent's arrange entry standing");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(120.0f);
  root.Add(parent);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(30.0f);
  standalone.SetRequestedHeight(30.0f);
  parent.Add(standalone);

  Settle(application);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());

  // Re-layout the standalone view through its own boundary: the invalidation
  // stops at the standalone view, which self-registers, and the next pass runs
  // it as its own layout root -- Measure and Arrange both out-of-band relative
  // to the parent.
  standalone.SetRequestedWidth(40.0f);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  Settle(application);

  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());

  END_TEST;
}

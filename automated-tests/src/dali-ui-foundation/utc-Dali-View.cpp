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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>

#include <dali-ui-foundation/extension-api/shadow.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

namespace IntegrationView = Dali::Ui::Integration::View;

using namespace Dali;
using namespace Dali::Ui;

namespace
{
namespace UiAccessibility = Dali::Ui::Accessibility;

// Dummy trait implementation for testing
class DummyTraitImpl : public TraitObject
{
public:
  DummyTraitImpl()
  : mAttachedCount(0),
    mDetachingCount(0),
    mViewDestroyingCount(0)
  {
  }

  void OnAttached(TraitId id, View& view) override
  {
    mAttachedCount++;
  }

  void OnDetaching(TraitId id, View& view) override
  {
    mDetachingCount++;
  }

  void OnViewDestroying(ViewImpl* viewImpl) override
  {
    mViewDestroyingCount++;
  }

  int GetAttachedCount() const
  {
    return mAttachedCount;
  }
  int GetDetachingCount() const
  {
    return mDetachingCount;
  }
  int GetViewDestroyingCount() const
  {
    return mViewDestroyingCount;
  }

private:
  int mAttachedCount;
  int mDetachingCount;
  int mViewDestroyingCount;
};

class DummyTrait : public BaseHandle
{
public:
  static DummyTrait New()
  {
    return DummyTrait(new DummyTraitImpl());
  }

  DummyTrait() = default;

  static DummyTrait DownCast(BaseHandle handle)
  {
    return DummyTrait(dynamic_cast<DummyTraitImpl*>(handle.GetObjectPtr()));
  }

  DummyTraitImpl& GetImpl()
  {
    return static_cast<DummyTraitImpl&>(GetBaseObject());
  }

  const DummyTraitImpl& GetImpl() const
  {
    return static_cast<const DummyTraitImpl&>(GetBaseObject());
  }

private:
  explicit DummyTrait(DummyTraitImpl* impl)
  : BaseHandle(impl)
  {
  }
};

// Test-only trait IDs (allocated once, reused across tests)
static const TraitId TEST_TRAIT_ID_0 = TraitId::Alloc();
static const TraitId TEST_TRAIT_ID_1 = TraitId::Alloc();

struct ViewMoveOnlyAttachment
{
  explicit ViewMoveOnlyAttachment(int v)
  : value(v)
  {
  }

  ViewMoveOnlyAttachment(ViewMoveOnlyAttachment&& rhs) noexcept
  : value(rhs.value)
  {
    rhs.value = 0;
  }

  ViewMoveOnlyAttachment& operator=(ViewMoveOnlyAttachment&& rhs) noexcept
  {
    value     = rhs.value;
    rhs.value = 0;
    return *this;
  }

  ViewMoveOnlyAttachment(const ViewMoveOnlyAttachment&)            = delete;
  ViewMoveOnlyAttachment& operator=(const ViewMoveOnlyAttachment&) = delete;

  int value;
};

IntrusivePtr<TraitObject> ToTraitObject(BaseHandle handle)
{
  return handle ? IntrusivePtr<TraitObject>(dynamic_cast<TraitObject*>(handle.GetObjectPtr())) : nullptr;
}

DummyTrait GetDummyTrait(ViewImpl& viewImpl, TraitId id)
{
  IntrusivePtr<TraitObject> object     = IntegrationView::GetTrait(viewImpl, id);
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? DummyTrait::DownCast(BaseHandle(baseObject)) : DummyTrait();
}

void ApplyViewExtension(View& view)
{
  view.SetName("ViewExtension");
}

void ApplyLabelExtension(Label& label)
{
  label.SetText("LabelExtension");
}

int ApplyLabelExtensionWithArgs(Label& label, int value, int offset)
{
  label.SetName("LabelExtensionWithArgs");
  return value + offset;
}

// --- Reentrant layout mutation helpers (child mutates parent's child list
// during the parent's Measure/Arrange snapshot loop). Callbacks cannot be
// capturing lambdas (Callback::New only supports free/member fns), so the
// parent + sibling handles are carried via file-static state. ---
Ui::View gReentrantParent;
Ui::View gSiblingToAdd;
Ui::View gSiblingToRemove;

MeasuredSize PlainMeasure(View, float, float)
{
  return MeasuredSize(40.0f, 30.0f);
}

LayoutRect PlainArrange(View, const LayoutRect& bounds)
{
  return bounds;
}

// Returns a self rect differing from the input slot on all four axes, used to
// verify the framework adopts the returned x/y/width/height as final geometry.
LayoutRect CustomBoundsArrange(View, const LayoutRect&)
{
  return LayoutRect(15.0f, 25.0f, 60.0f, 40.0f);
}

// During the child's own Measure, add a new sibling to the parent. This
// reaches ViewImpl::OnChildAdd -> mChildren.PushBack while the parent loop
// is mid-iteration, forcing a Dali::Vector reallocation.
MeasuredSize ReentrantAddMeasure(View, float, float)
{
  if(gSiblingToAdd && gSiblingToAdd.GetParent() != static_cast<Actor>(gReentrantParent))
  {
    gReentrantParent.Add(gSiblingToAdd);
  }
  return MeasuredSize(40.0f, 30.0f);
}

// During the child's own Measure, remove a sibling from the parent. This
// reaches ViewImpl::OnChildRemove -> mChildren.Erase mid-iteration.
MeasuredSize ReentrantRemoveMeasure(View, float, float)
{
  if(gSiblingToRemove && gSiblingToRemove.GetParent() == static_cast<Actor>(gReentrantParent))
  {
    gReentrantParent.Remove(gSiblingToRemove, RemovePolicy::IMMEDIATE);
  }
  return MeasuredSize(40.0f, 30.0f);
}

// During the child's own Arrange, remove a sibling from the parent. This
// reaches ViewImpl::OnChildRemove -> mChildren.Erase mid-iteration.
LayoutRect ReentrantRemoveArrange(View, const LayoutRect& bounds)
{
  if(gSiblingToRemove && gSiblingToRemove.GetParent() == static_cast<Actor>(gReentrantParent))
  {
    gReentrantParent.Remove(gSiblingToRemove, RemovePolicy::IMMEDIATE);
  }
  return bounds;
}

// --- Same-view re-entrancy helpers (a view's own Measure/Arrange producer calls
// Measure/Arrange on that same view). The engine must absorb this in RELEASE
// builds: return the last COMPLETED result instead of re-running the producer.
// The local depth guard below only bounds the damage if that engine guard ever
// regresses (the test then fails on the recorded value instead of overflowing
// the stack); it never re-enters more than once itself. ---
Ui::View     gSelfReentrantView;
int          gSelfReentrantDepth        = 0;
int          gSelfMeasureProducerCount  = 0;
int          gSelfArrangeProducerCount  = 0;
MeasuredSize gSelfReentrantMeasureInner = MeasuredSize(-1.0f, -1.0f);
LayoutRect   gSelfReentrantArrangeInner = LayoutRect(-1.0f, -1.0f, -1.0f, -1.0f);
bool         gSelfReentrantDidReenter   = false;

const LayoutRect SELF_REENTRANT_INNER_BOUNDS(5.0f, 7.0f, 50.0f, 60.0f);

MeasuredSize SelfReentrantMeasure(View, float, float)
{
  ++gSelfMeasureProducerCount;
  ++gSelfReentrantDepth;
  if(gSelfReentrantDepth == 1 && gSelfReentrantView)
  {
    gSelfReentrantDidReenter   = true;
    gSelfReentrantMeasureInner = gSelfReentrantView.Measure(200.0f, 100.0f);
  }
  --gSelfReentrantDepth;
  return MeasuredSize(40.0f, 30.0f);
}

LayoutRect SelfReentrantArrange(View, const LayoutRect& bounds)
{
  ++gSelfArrangeProducerCount;
  ++gSelfReentrantDepth;
  if(gSelfReentrantDepth == 1 && gSelfReentrantView)
  {
    gSelfReentrantDidReenter   = true;
    gSelfReentrantArrangeInner = gSelfReentrantView.Arrange(SELF_REENTRANT_INNER_BOUNDS);
  }
  --gSelfReentrantDepth;
  return bounds;
}

// --- "Ignoring parent" helpers: a custom parent whose Measure AND Arrange
// producers never touch their children. Such a parent never calls a child's
// Measure()/Arrange(), and a view's dirty flags are cleared only by its OWN
// pass -- so a child under this parent stays dirty forever. Used to prove that
// a later invalidation of that already-dirty child still reaches the layout
// root instead of being swallowed. ---
int gIgnoringMeasureProducerCount = 0;
int gIgnoringArrangeProducerCount = 0;

MeasuredSize IgnoringChildrenMeasure(View, float, float)
{
  ++gIgnoringMeasureProducerCount;
  return MeasuredSize(200.0f, 100.0f);
}

LayoutRect IgnoringChildrenArrange(View, const LayoutRect& bounds)
{
  ++gIgnoringArrangeProducerCount;
  return bounds;
}

// --- Mid-pass self-invalidation helpers: a producer that calls the PUBLIC
// View::InvalidateMeasure() / View::InvalidateArrange() on its OWN view while
// that view's pass is running.
//
// Such a call is inside the LAYOUT PROCESSING WINDOW, so it is logged once. The
// invalidation itself is complete: it drops caches, raises dirty, poisons the
// running pass, walks to the root and leaves that root pending. Only the idle
// wake is suppressed, so layout cannot perpetually wake ProcessEvents from work
// produced by its own callback.
//
// Dirty is still consumed at pass ENTRY (unchanged): the guards clear it when the
// next pass opens, so the standing bit means "a re-invalidation arrived after this
// pass started" at the publish point, and the NEXT pass recomputes the
// post-invalidation value instead of serving the pre-invalidation one.
//
// The difference the window makes is therefore about WAKING, not freshness:
// nothing stale is served and the work is retained, but the retained work does
// not schedule another idle ProcessEvents cycle by itself. ---
Ui::View gMidPassView;
int      gMidPassMeasureProducerCount = 0;
int      gMidPassArrangeProducerCount = 0;

const float MID_PASS_FIRST_WIDTH  = 40.0f;
const float MID_PASS_FIRST_HEIGHT = 30.0f;
const float MID_PASS_LATER_WIDTH  = 80.0f;
const float MID_PASS_LATER_HEIGHT = 60.0f;

MeasuredSize MidPassInvalidatingMeasure(View, float, float)
{
  ++gMidPassMeasureProducerCount;
  if(gMidPassMeasureProducerCount == 1 && gMidPassView)
  {
    // Re-invalidate this very view while its measure pass is running.
    gMidPassView.InvalidateMeasure();
    return MeasuredSize(MID_PASS_FIRST_WIDTH, MID_PASS_FIRST_HEIGHT);
  }
  return MeasuredSize(MID_PASS_LATER_WIDTH, MID_PASS_LATER_HEIGHT);
}

LayoutRect MidPassInvalidatingArrange(View, const LayoutRect& bounds)
{
  ++gMidPassArrangeProducerCount;
  if(gMidPassArrangeProducerCount == 1 && gMidPassView)
  {
    gMidPassView.InvalidateArrange();
  }
  return bounds;
}

// --- In-pass invalidation: the WINDOW itself ------------------------------
//
// The producers above disarm after their first call, so they cannot tell a
// "one follow-up, then settle" implementation apart from "no follow-up at all".
// The ones below invalidate on EVERY invocation, which is precisely the shape
// the layout processing window exists to stop: under the pre-window contract
// each pass re-registered its own view, the idle pump was re-armed every frame,
// the producer ran forever and the layout-finished signal NEVER fired.
//
// With the window in place the root remains pending and LayoutFinished remains
// deferred, but RequestProcessEventsOnIdle is false after every pass. An
// independent ProcessEvents trigger may service one parked pass without letting
// that pass arm the next one.
Ui::View gAlwaysInvalidatingView;
int      gAlwaysInvalidatingMeasureCount = 0;
int      gAlwaysInvalidatingArrangeCount = 0;

const float ALWAYS_INVALIDATING_MEASURED_WIDTH  = 70.0f;
const float ALWAYS_INVALIDATING_MEASURED_HEIGHT = 50.0f;
const LayoutRect ALWAYS_INVALIDATING_ARRANGE_RESULT(0.0f, 0.0f, 90.0f, 40.0f);

MeasuredSize AlwaysInvalidatingMeasure(View, float, float)
{
  ++gAlwaysInvalidatingMeasureCount;
  if(gAlwaysInvalidatingView)
  {
    gAlwaysInvalidatingView.InvalidateMeasure(); // inside the window: warned + parked
  }
  return MeasuredSize(ALWAYS_INVALIDATING_MEASURED_WIDTH, ALWAYS_INVALIDATING_MEASURED_HEIGHT);
}

LayoutRect AlwaysInvalidatingArrange(View, const LayoutRect&)
{
  ++gAlwaysInvalidatingArrangeCount;
  if(gAlwaysInvalidatingView)
  {
    gAlwaysInvalidatingView.InvalidateArrange(); // inside the window: warned + parked
  }
  return ALWAYS_INVALIDATING_ARRANGE_RESULT;
}

// A producer that invalidates a DIFFERENT view (the window's SCOPE is global: any
// view's pass closes the window for every view, not just the one being measured),
// plus a plain counting producer for the view on the receiving end.
Ui::View gCrossInvalidationTarget;
int      gCrossInvalidationSourceCount = 0;
int      gCrossInvalidationTargetCount = 0;

MeasuredSize CrossInvalidatingMeasure(View, float, float)
{
  ++gCrossInvalidationSourceCount;
  if(gCrossInvalidationTarget)
  {
    gCrossInvalidationTarget.InvalidateMeasure();
  }
  return MeasuredSize(30.0f, 20.0f);
}

MeasuredSize CrossInvalidationTargetMeasure(View, float, float)
{
  ++gCrossInvalidationTargetCount;
  return MeasuredSize(40.0f, 25.0f);
}

// Self-disarming variant of AlwaysInvalidatingMeasure that RECORDS the constraint
// it was handed. Recording it is what lets a test re-issue the framework's own
// constraint verbatim, so a later Measure() at that constraint tests the CACHE
// rather than accidentally testing a different key.
Ui::View gCacheDropView;
int      gCacheDropMeasureCount    = 0;
float    gCacheDropLastConstraintW = 0.0f;
float    gCacheDropLastConstraintH = 0.0f;

MeasuredSize CacheDropOnceMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gCacheDropMeasureCount;
  gCacheDropLastConstraintW = widthConstraint;
  gCacheDropLastConstraintH = heightConstraint;
  if(gCacheDropMeasureCount == 1 && gCacheDropView)
  {
    gCacheDropView.InvalidateMeasure();
  }
  return MeasuredSize(55.0f, 35.0f);
}

// A CONTAINER producer that adds a child View on its first arrange. Adding a child
// is a framework-internal invalidation (OnChildAdded): it fully retains a second
// pass under PARK, but does not wake ProcessEvents from inside the first. A ViewImpl
// subclass rather than an ArrangeCallback because a callback REPLACES OnArrange,
// and the child it adds would then never be arranged at all.
class ChildAddingContainerViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<ChildAddingContainerViewImpl> New()
  {
    return IntrusivePtr<ChildAddingContainerViewImpl>(new ChildAddingContainerViewImpl());
  }

  int GetArrangeCallCount() const
  {
    return mArrangeCount;
  }

  View GetAddedChild() const
  {
    return mAddedChild;
  }

protected:
  ChildAddingContainerViewImpl()
  : ViewImpl()
  {
  }

  LayoutRect OnArrange(const LayoutRect& bounds) override
  {
    ++mArrangeCount;
    if(mArrangeCount == 1)
    {
      mAddedChild = View::New();
      mAddedChild.SetRequestedWidth(20.0f);
      mAddedChild.SetRequestedHeight(10.0f);
      Ui::View::DownCast(Self()).Add(mAddedChild);
    }
    return ViewImpl::OnArrange(bounds);
  }

private:
  int  mArrangeCount{0};
  View mAddedChild;
};

// Register so TypeInfo lookup can walk the chain.
Dali::TypeRegistration childAddingContainerViewTypeReg(
  typeid(ChildAddingContainerViewImpl), typeid(ViewImpl), nullptr);

// A plain counting measure producer, for tests that need "did this view's measure
// producer run again?" without any invalidation behaviour of its own.
int gPlainMeasureProducerCount = 0;

MeasuredSize PlainCountingMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gPlainMeasureProducerCount;
  return MeasuredSize(widthConstraint >= 0.0f ? widthConstraint : 10.0f,
                      heightConstraint >= 0.0f ? heightConstraint : 10.0f);
}

// --- LayoutController::LayoutFinishedSignal observation --------------------
//
// "The main loop went idle" has no direct black-box probe, but the window-level
// layout-finished signal is its exact proxy: the controller emits it only when a
// pass ends with nothing left pending, and re-arms instead of emitting whenever
// anything re-scheduled work. A view stuck in a per-frame invalidation loop
// therefore emits ZERO times, and a settled one emits exactly once.
struct WindowLayoutFinishedCounter
{
  explicit WindowLayoutFinishedCounter(int& count)
  : count(count)
  {
  }
  void operator()(Dali::Window)
  {
    ++count;
  }
  int& count;
};

// --- "Poison once" helpers: a producer that re-enters its own view's
// Measure()/Arrange() on its FIRST invocation only. Re-entrancy poisons the
// running pass without going through Invalidate*(), so nothing propagates to a
// layout root: the pass itself must register exactly one follow-up layout, and
// the follow-up (which no longer re-enters) must complete and stop. ---
Ui::View gPoisonOnceView;
int      gPoisonOnceMeasureProducerCount = 0;
int      gPoisonOnceArrangeProducerCount = 0;
int      gPoisonOnceDepth                = 0;

MeasuredSize PoisonOnceMeasure(View, float, float)
{
  ++gPoisonOnceMeasureProducerCount;
  if(gPoisonOnceMeasureProducerCount == 1 && gPoisonOnceView && gPoisonOnceDepth == 0)
  {
    ++gPoisonOnceDepth;
    gPoisonOnceView.Measure(10.0f, 10.0f); // absorbed by the re-entrancy guard
    --gPoisonOnceDepth;
  }
  return MeasuredSize(40.0f, 30.0f);
}

LayoutRect PoisonOnceArrange(View, const LayoutRect& bounds)
{
  ++gPoisonOnceArrangeProducerCount;
  if(gPoisonOnceArrangeProducerCount == 1 && gPoisonOnceView && gPoisonOnceDepth == 0)
  {
    ++gPoisonOnceDepth;
    gPoisonOnceView.Arrange(LayoutRect(1.0f, 2.0f, 3.0f, 4.0f)); // absorbed by the guard
    --gPoisonOnceDepth;
  }
  return bounds;
}

// --- Ancestor-cache invalidation helpers (Phase 2a) -------------------------
//
// A completed Measure() rewrites the view's stored measured slot, and every
// ancestor arranges its children FROM that stored slot. So a Measure() issued
// out of band -- an external View::Measure(), or one issued from an unrelated
// view's producer -- leaves the ancestors' cached results describing a slot
// that no longer exists. If an ancestor may still serve a measure cache HIT it
// never re-measures the view, and then arranges it at the out-of-band size.
// A full measure miss must therefore drop the ancestor cache entries, up to the
// nearest layout boundary / the nearest ancestor that owns the measurement.
//
// A producer whose result DEPENDS on the constraint it is handed: measuring it
// at a small constraint yields a small size, so the corrupted slot is visible
// in the arranged geometry.
const float CLAMP_MEASURE_NATURAL_WIDTH  = 80.0f;
const float CLAMP_MEASURE_NATURAL_HEIGHT = 60.0f;

int gClampMeasureProducerCount = 0;

MeasuredSize ClampToConstraintMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gClampMeasureProducerCount;
  float width  = (widthConstraint >= 0.0f) ? std::min(widthConstraint, CLAMP_MEASURE_NATURAL_WIDTH) : CLAMP_MEASURE_NATURAL_WIDTH;
  float height = (heightConstraint >= 0.0f) ? std::min(heightConstraint, CLAMP_MEASURE_NATURAL_HEIGHT) : CLAMP_MEASURE_NATURAL_HEIGHT;
  return MeasuredSize(width, height);
}

// A counting producer that forwards the measurement to one child, standing in
// for a custom parent that participates in the normal top-down recursion. The
// count is the observable "did this view's cache hit or miss" signal.
Ui::View gCountingMeasureChild;
int      gCountingMeasureProducerCount = 0;

MeasuredSize CountingParentMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gCountingMeasureProducerCount;
  if(gCountingMeasureChild)
  {
    gCountingMeasureChild.Measure(widthConstraint, heightConstraint);
  }
  return MeasuredSize(200.0f, 100.0f);
}

// A counting producer that forwards the measurement to EVERY child, standing in
// for an observer parent that takes part in the normal top-down recursion. Its
// count is the observable "did this view's measure cache hit or miss" signal.
// Unlike CountingParentMeasure it needs no globally pinned child, so the same
// producer can sit above any subtree (a Layout, any of the layout managers, ...).
int gPassThroughMeasureProducerCount = 0;

MeasuredSize CountingPassThroughMeasure(View view, float widthConstraint, float heightConstraint)
{
  ++gPassThroughMeasureProducerCount;

  float          maxWidth   = 0.0f;
  float          maxHeight  = 0.0f;
  const uint32_t childCount = view.GetChildCount();
  for(uint32_t i = 0; i < childCount; ++i)
  {
    View child = View::DownCast(view.GetChildAt(i));
    if(child)
    {
      MeasuredSize childSize = child.Measure(widthConstraint, heightConstraint);
      maxWidth               = std::max(maxWidth, childSize.GetWidth());
      maxHeight              = std::max(maxHeight, childSize.GetHeight());
    }
  }
  return MeasuredSize(maxWidth, maxHeight);
}

// Two more pass-through measure producers, each with its OWN counter, so a test
// can tell WHICH of two parents re-measured. The body is CountingPassThroughMeasure's
// (measure every child, report the maximum extent), which is what makes the parent's
// measured size follow its child set.
int gFirstParentMeasureProducerCount  = 0;
int gSecondParentMeasureProducerCount = 0;

MeasuredSize AccumulateChildExtents(View view, float widthConstraint, float heightConstraint)
{
  float          maxWidth   = 0.0f;
  float          maxHeight  = 0.0f;
  const uint32_t childCount = view.GetChildCount();
  for(uint32_t i = 0; i < childCount; ++i)
  {
    View child = View::DownCast(view.GetChildAt(i));
    if(child)
    {
      MeasuredSize childSize = child.Measure(widthConstraint, heightConstraint);
      maxWidth               = std::max(maxWidth, childSize.GetWidth());
      maxHeight              = std::max(maxHeight, childSize.GetHeight());
    }
  }
  return MeasuredSize(maxWidth, maxHeight);
}

MeasuredSize FirstParentAccumulatingMeasure(View view, float widthConstraint, float heightConstraint)
{
  ++gFirstParentMeasureProducerCount;
  return AccumulateChildExtents(view, widthConstraint, heightConstraint);
}

MeasuredSize SecondParentAccumulatingMeasure(View view, float widthConstraint, float heightConstraint)
{
  ++gSecondParentMeasureProducerCount;
  return AccumulateChildExtents(view, widthConstraint, heightConstraint);
}

// --- Arrange cache-HIT observation (childless views) ---------------------
//
// A counting LEAF arrange producer. It echoes its input slot and does nothing
// else, so its invocation count measures exactly one thing: whether this view's
// Arrange() ran its producer or served an arrange cache HIT. Every test using it
// resets the counter at its own start (the counter is file-global and the suite
// shares a process).
int gCountingArrangeProducerCount = 0;

LayoutRect CountingLeafArrange(View, const LayoutRect& bounds)
{
  ++gCountingArrangeProducerCount;
  return bounds;
}

// The same counter, but the producer returns a rect that differs from its input
// on all four axes. Used to prove that a cache hit hands back (and re-applies)
// the PUBLISHED bounds the producer chose, not the input slot.
//
// Every component differs from the (0, 0, 50, 40) slot it is handed in
// UtcDaliViewArrangeCacheHitPreservesGeometryP, which is what makes "all four axes"
// literally true -- a hit that echoed its input would be caught on any of them.
const LayoutRect COUNTING_CUSTOM_ARRANGE_RESULT(15.0f, 25.0f, 60.0f, 45.0f);

LayoutRect CountingCustomBoundsArrange(View, const LayoutRect&)
{
  ++gCountingArrangeProducerCount;
  return COUNTING_CUSTOM_ARRANGE_RESULT;
}

// Drives one full layout batch to completion.
void SettleLayout(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}

bool WasProcessEventsOnIdleRequested(UiTestApplication& application)
{
  return application.GetRenderController().WasCalled(TestRenderController::RequestProcessEventsOnIdleFunc);
}

// Simulates delivery of the idle ProcessEvents request that mounted/event-time
// layout work armed. Reset first because the real adaptor consumes that wake as
// it enters ProcessEvents; any request observed afterwards was made by the pass
// itself and is therefore a distinct, testable wake.
void SendRequestedProcessEvents(UiTestApplication& application)
{
  DALI_TEST_CHECK(WasProcessEventsOnIdleRequested(application));
  application.GetRenderController().Initialize();
  application.SendNotification();
}

// Drives ProcessEvents for an unrelated external reason. PARK semantics require
// this to service retained layout work, while still forbidding that work from
// requesting the next idle ProcessEvents cycle.
void SendIndependentProcessEvents(UiTestApplication& application)
{
  application.GetRenderController().Initialize();
  application.SendNotification();
}

// --- Arrange cache-HIT observation (views WITH children) ------------------
//
// A counting producer for a CONTAINER. An ArrangeCallback cannot play this role: it
// REPLACES OnArrange, so a view carrying one never arranges its children at all and
// the subtree below it would simply never be laid out. This subclass counts and then
// delegates to ViewImpl::OnArrange (ArrangeDefault), so it is a genuine container
// producer whose invocation count measures exactly one thing -- whether this view's
// Arrange() ran its producer or was served from cache.
//
// The test factory selects the requested policy explicitly; production subclasses
// may do the same in a constructor.
class CountingContainerViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<CountingContainerViewImpl> New(bool arrangeIfChanged)
  {
    IntrusivePtr<CountingContainerViewImpl> impl(new CountingContainerViewImpl());
    impl->SetArrangePolicy(arrangeIfChanged ? ArrangePolicy::IF_CHANGED : ArrangePolicy::ALWAYS);
    return impl;
  }

  int GetArrangeCallCount() const
  {
    return mArrangeCount;
  }

  // ViewImpl::SetArrangePolicy is protected (it is a subclass's own declaration),
  // so a test that changes the policy AFTER construction needs this forwarder.
  void SelectArrangePolicy(ArrangePolicy policy)
  {
    SetArrangePolicy(policy);
  }

protected:
  CountingContainerViewImpl()
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
Dali::TypeRegistration countingContainerViewTypeReg(
  typeid(CountingContainerViewImpl), typeid(ViewImpl), nullptr);

View CreateCountingContainer(bool arrangeIfChanged)
{
  auto impl = CountingContainerViewImpl::New(arrangeIfChanged);
  return View(*impl);
}

CountingContainerViewImpl& CountingContainerImplOf(View view)
{
  return static_cast<CountingContainerViewImpl&>(GetImpl(view));
}

LayoutRect ActorRectOf(View view)
{
  return LayoutRect(view.GetProperty<float>(Actor::Property::POSITION_X),
                    view.GetProperty<float>(Actor::Property::POSITION_Y),
                    view.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                    view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
}

void CheckActorRect(View view, const LayoutRect& expected, const char* location)
{
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_X), expected.x, location);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Y), expected.y, location);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), expected.width, location);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), expected.height, location);
}

// An ArrangeCallback that measures a DESCENDANT (deliberately not a direct
// child, so no owner scope could ever legitimately cover it) at a constraint
// that changes on every invocation, so the measurement is always a genuine full
// miss. It never arranges its own children, and counts its own invocations so a
// test can detect layout passes it did not ask for.
Ui::View gArrangeMeasuredDescendant;
int      gDescendantMeasuringArrangeCount = 0;

LayoutRect DescendantMeasuringArrange(View, const LayoutRect& bounds)
{
  ++gDescendantMeasuringArrangeCount;
  if(gArrangeMeasuredDescendant)
  {
    // A fresh constraint every time: never a cache hit, so the ancestor walk
    // runs on every single arrange.
    const float extent = 10.0f + static_cast<float>(gDescendantMeasuringArrangeCount);
    gArrangeMeasuredDescendant.Measure(extent, extent);
  }
  return bounds;
}

// A producer that measures a view in a COMPLETELY DIFFERENT tree. The walk must
// follow the measured view's own ancestry, not the call stack: the unrelated
// tree's ancestors are invalidated, this producer's own ancestors are not.
Ui::View gUnrelatedMeasureTarget;
int      gUnrelatedOwnerProducerCount = 0;

MeasuredSize MeasureUnrelatedTreeMeasure(View, float, float)
{
  ++gUnrelatedOwnerProducerCount;
  if(gUnrelatedMeasureTarget)
  {
    gUnrelatedMeasureTarget.Measure(30.0f, 20.0f);
  }
  return MeasuredSize(100.0f, 50.0f);
}

// A second constraint-clamping producer whose natural size is deliberately LARGER
// than the arrange extent used in the standalone steady-state test, so the size it
// reports depends on WHICH constraint it was last measured against: the parent's
// measure constraint (the standalone slot's steady-state source) or the parent's
// smaller arrange extent.
const float WIDE_CLAMP_NATURAL_WIDTH  = 150.0f;
const float WIDE_CLAMP_NATURAL_HEIGHT = 90.0f;

int gWideClampMeasureProducerCount = 0;

MeasuredSize WideClampToConstraintMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gWideClampMeasureProducerCount;
  float width  = (widthConstraint >= 0.0f) ? std::min(widthConstraint, WIDE_CLAMP_NATURAL_WIDTH) : WIDE_CLAMP_NATURAL_WIDTH;
  float height = (heightConstraint >= 0.0f) ? std::min(heightConstraint, WIDE_CLAMP_NATURAL_HEIGHT) : WIDE_CLAMP_NATURAL_HEIGHT;
  return MeasuredSize(width, height);
}

// A parent that measures SMALLER than the constraint it was handed, so its arrange
// extent (its own final size, which is what its standalone children are placed
// against) and the constraint it forwards to MeasureStandaloneChildren are two
// different numbers -- the only way to tell those two constraints apart from the
// arranged geometry.
const float NARROW_PARENT_MEASURED_WIDTH  = 120.0f;
const float NARROW_PARENT_MEASURED_HEIGHT = 60.0f;

MeasuredSize NarrowParentMeasure(View, float, float)
{
  return MeasuredSize(NARROW_PARENT_MEASURED_WIDTH, NARROW_PARENT_MEASURED_HEIGHT);
}

Shadow GetShadowProperty(View view)
{
  Property::Value      shadowValue = view.GetProperty(View::Property::SHADOW);
  const Property::Map* shadowMap   = shadowValue.GetMap();
  DALI_TEST_CHECK(shadowMap);
  return shadowMap ? Extension::Shadow::CreateShadow(*shadowMap) : Shadow::None();
}

int GetVisualType(const Property::Map& map)
{
  Property::Value* typeValue = map.Find(Ui::VisualBasePropertyIndex::TYPE);
  DALI_TEST_CHECK(typeValue);

  int type = static_cast<int>(Ui::Integration::InternalVisualType::INVALID);
  DALI_TEST_CHECK(typeValue && typeValue->Get(type));
  return type;
}

uint32_t AccessibilityStateMask(UiAccessibility::State state)
{
  return 1u << static_cast<uint32_t>(state);
}

const char* const BACKGROUND_COLOR_TOKEN          = "UtcBackgroundColor";
const char* const BACKGROUND_GRADIENT_START_TOKEN = "UtcBackgroundGradientStart";
const char* const BACKGROUND_GRADIENT_END_TOKEN   = "UtcBackgroundGradientEnd";

const Vector4 BACKGROUND_COLOR_A(0.1f, 0.2f, 0.3f, 1.0f);
const Vector4 BACKGROUND_COLOR_B(0.7f, 0.6f, 0.5f, 1.0f);
const Vector4 BACKGROUND_GRADIENT_START_A(0.8f, 0.1f, 0.2f, 1.0f);
const Vector4 BACKGROUND_GRADIENT_START_B(0.2f, 0.8f, 0.1f, 1.0f);
const Vector4 BACKGROUND_GRADIENT_END_A(0.1f, 0.2f, 0.8f, 1.0f);
const Vector4 BACKGROUND_GRADIENT_END_B(0.9f, 0.8f, 0.2f, 1.0f);

bool gUseAlternateBackgroundColors = false;

bool OverrideBackgroundColors(StringView colorId, Vector4& outColor)
{
  if(colorId == BACKGROUND_COLOR_TOKEN)
  {
    outColor = gUseAlternateBackgroundColors ? BACKGROUND_COLOR_B : BACKGROUND_COLOR_A;
    return true;
  }

  if(colorId == BACKGROUND_GRADIENT_START_TOKEN)
  {
    outColor = gUseAlternateBackgroundColors ? BACKGROUND_GRADIENT_START_B : BACKGROUND_GRADIENT_START_A;
    return true;
  }

  if(colorId == BACKGROUND_GRADIENT_END_TOKEN)
  {
    outColor = gUseAlternateBackgroundColors ? BACKGROUND_GRADIENT_END_B : BACKGROUND_GRADIENT_END_A;
    return true;
  }

  return false;
}

Gradient::Linear CreateBackgroundTokenGradient()
{
  Gradient::Linear                 gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(BACKGROUND_GRADIENT_START_TOKEN)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(BACKGROUND_GRADIENT_END_TOKEN)));
  gradient.SetStopNodes(stopNodes);
  return gradient;
}

Property::Map GetBackgroundPropertyMap(View view)
{
  Property::Value      backgroundValue = view.GetProperty(Ui::View::Property::BACKGROUND);
  const Property::Map* backgroundMap   = backgroundValue.GetMap();
  DALI_TEST_CHECK(backgroundMap);
  return backgroundMap ? *backgroundMap : Property::Map();
}

Vector4 GetBackgroundMixColor(View view)
{
  Property::Map    backgroundMap = GetBackgroundPropertyMap(view);
  Property::Value* colorValue    = backgroundMap.Find(Ui::VisualBasePropertyIndex::MIX_COLOR);
  DALI_TEST_CHECK(colorValue);

  Vector4 color;
  DALI_TEST_CHECK(colorValue && colorValue->Get(color));
  return color;
}

Vector4 GetBackgroundGradientStopColor(View view, uint32_t index)
{
  Property::Map    backgroundMap  = GetBackgroundPropertyMap(view);
  Property::Value* stopColorValue = backgroundMap.Find(Ui::GradientVisualPropertyIndex::STOP_COLOR);
  DALI_TEST_CHECK(stopColorValue);

  const Property::Array* stopColors = stopColorValue ? stopColorValue->GetArray() : nullptr;
  DALI_TEST_CHECK(stopColors);
  DALI_TEST_CHECK(stopColors && index < stopColors->Count());

  Vector4 color;
  DALI_TEST_CHECK(stopColors && stopColors->GetElementAt(index).Get(color));
  return color;
}

bool HasBackgroundVisual(View view)
{
  Property::Map backgroundMap = GetBackgroundPropertyMap(view);
  return !backgroundMap.Empty() && backgroundMap.Find(Ui::VisualBasePropertyIndex::TYPE);
}

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
  View              view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewNewP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewLeaveRequiredDefaultP(void)
{
  UiTestApplication application;
  View              view = View::New();

  DALI_TEST_CHECK(view.GetLeaveRequired());

  view.SetLeaveRequired(false);
  DALI_TEST_CHECK(!view.GetLeaveRequired());
  END_TEST;
}

int UtcDaliViewWithExtensionHookP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.With(ApplyViewExtension);
  DALI_TEST_EQUALS(view.GetName(), Dali::String("ViewExtension"), TEST_LOCATION);

  Label label = Label::New();
  label.With(ApplyLabelExtension);
  DALI_TEST_EQUALS(label.GetText(), Dali::String("LabelExtension"), TEST_LOCATION);

  int result = label.With(ApplyLabelExtensionWithArgs, 20, 3);
  DALI_TEST_EQUALS(label.GetName(), Dali::String("LabelExtensionWithArgs"), TEST_LOCATION);
  DALI_TEST_EQUALS(result, 23, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewCopyConstructorP(void)
{
  UiTestApplication application;
  View              view = View::New();
  View              copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveConstructor(void)
{
  UiTestApplication application;
  View              view = View::New();
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
  View              view = View::New();
  View              copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveAssignment(void)
{
  UiTestApplication application;
  View              view = View::New();
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
  View              view = View::New();
  BaseHandle        object(view);
  View              view2 = View::DownCast(object);
  View              view3 = DownCast<View>(object);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view3);
  END_TEST;
}

int UtcDaliViewDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  View              view1 = View::DownCast(unInitializedObject);
  View              view2 = DownCast<View>(unInitializedObject);
  DALI_TEST_CHECK(!view1);
  DALI_TEST_CHECK(!view2);
  END_TEST;
}

int UtcDaliViewGetSizeWidthP(void)
{
  UiTestApplication application;
  View              view      = View::New();
  const float       testWidth = 100.0f;

  view.SetRequestedWidth(testWidth);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetSizeHeightP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  const float       testHeight = 200.0f;

  view.SetRequestedHeight(testHeight);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionXP(void)
{
  UiTestApplication application;
  View              view  = View::New();
  const float       testX = 50.0f;

  view.SetRequestedX(testX);
  DALI_TEST_EQUALS(view.GetRequestedX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionYP(void)
{
  UiTestApplication application;
  View              view  = View::New();
  const float       testY = 75.0f;

  view.SetRequestedY(testY);
  DALI_TEST_EQUALS(view.GetRequestedY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeWidthChainingP(void)
{
  UiTestApplication application;
  View              view      = View::New();
  const float       testWidth = 150.0f;
  view.SetRequestedWidth(testWidth);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeHeightChainingP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  const float       testHeight = 250.0f;
  view.SetRequestedHeight(testHeight);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionXChainingP(void)
{
  UiTestApplication application;
  View              view  = View::New();
  const float       testX = 125.0f;
  view.SetRequestedX(testX);
  DALI_TEST_EQUALS(view.GetRequestedX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionYChainingP(void)
{
  UiTestApplication application;
  View              view  = View::New();
  const float       testY = 175.0f;
  view.SetRequestedY(testY);
  DALI_TEST_EQUALS(view.GetRequestedY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBackgroundColorSetterP(void)
{
  UiTestApplication application;
  View              view            = View::New();
  const UiColor     backgroundColor = UiColor(0xFFFFFF);
  const UiColor     colorMultiplier = UiColor(0xFF0000);
  const UiColor     legacyColor     = UiColor(0x00FF00);
  const UiColor     implLegacyColor = UiColor(0x0000FF);

  application.GetScene().Add(view);
  view.SetBackgroundColor(backgroundColor);
  view.SetColorMultiplier(colorMultiplier);

  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), backgroundColor.GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetColorMultiplier().GetRgba(), colorMultiplier.GetRgba(), TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetCurrentColorMultiplier().GetRgba(), colorMultiplier.GetRgba(), TEST_LOCATION);

  // Verify that the legacy public API delegates to the color multiplier API.
  view.SetColor(legacyColor);
  DALI_TEST_EQUALS(view.GetColor().GetRgba(), legacyColor.GetRgba(), TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetCurrentColor().GetRgba(), legacyColor.GetRgba(), TEST_LOCATION);

  // Verify the same compatibility path on ViewImpl.
  ViewImpl& viewImpl = GetImpl(view);
  viewImpl.SetColor(implLegacyColor);
  DALI_TEST_EQUALS(viewImpl.GetColor().GetRgba(), implLegacyColor.GetRgba(), TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(viewImpl.GetCurrentColor().GetRgba(), implLegacyColor.GetRgba(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBackgroundImageP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  const char*       imageUrl = "background-image.png";

  view.SetBackgroundColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  view.SetBackgroundImage(Dali::String(imageUrl));

  Property::Map backgroundMap = view.GetProperty<Property::Map>(Ui::View::Property::BACKGROUND);
  DALI_TEST_EQUALS(GetVisualType(backgroundMap), static_cast<int>(Ui::Integration::InternalVisualType::IMAGE), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), UiColor().GetRgba(), TEST_LOCATION);

  Property::Value* urlValue = backgroundMap.Find(Ui::ImageVisualPropertyIndex::URL);
  DALI_TEST_CHECK(urlValue);

  Dali::String url;
  DALI_TEST_CHECK(urlValue && urlValue->Get(url));
  DALI_TEST_EQUALS(url, Dali::String(imageUrl), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBackgroundGradientP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Gradient::Linear                 gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(Color::RED)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(Color::BLUE)));
  gradient.SetStopNodes(stopNodes);
  gradient.SetUnits(Gradient::Units::USER_SPACE);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);

  view.SetBackgroundColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  view.SetBackgroundGradient(gradient);

  Property::Map backgroundMap = view.GetProperty<Property::Map>(Ui::View::Property::BACKGROUND);
  DALI_TEST_EQUALS(GetVisualType(backgroundMap), static_cast<int>(Ui::Integration::InternalVisualType::GRADIENT), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), UiColor().GetRgba(), TEST_LOCATION);

  Property::Value* startPositionValue = backgroundMap.Find(Ui::GradientVisualPropertyIndex::START_POSITION);
  DALI_TEST_CHECK(startPositionValue);
  Vector2 startPosition;
  DALI_TEST_CHECK(startPositionValue && startPositionValue->Get(startPosition));
  DALI_TEST_EQUALS(startPosition, Vector2(-0.5f, -0.5f), TEST_LOCATION);

  Property::Value* stopColorValue = backgroundMap.Find(Ui::GradientVisualPropertyIndex::STOP_COLOR);
  DALI_TEST_CHECK(stopColorValue);
  const Property::Array* stopColors = stopColorValue ? stopColorValue->GetArray() : nullptr;
  DALI_TEST_CHECK(stopColors);
  DALI_TEST_EQUALS(stopColors ? stopColors->Count() : 0u, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBackgroundGradientTokenBindingRefreshP(void)
{
  UiTestApplication application;
  UiColorManager    manager = UiColorManager::Get();
  View              view    = View::New();

  gUseAlternateBackgroundColors = false;
  manager.SetColorOverride(OverrideBackgroundColors);

  view.SetBackgroundColor(UiColor(BACKGROUND_COLOR_TOKEN));
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), BACKGROUND_COLOR_A, TEST_LOCATION);
  DALI_TEST_EQUALS(GetBackgroundMixColor(view), BACKGROUND_COLOR_A, TEST_LOCATION);

  view.SetBackgroundGradient(CreateBackgroundTokenGradient());
  DALI_TEST_EQUALS(GetVisualType(GetBackgroundPropertyMap(view)), static_cast<int>(Ui::Integration::InternalVisualType::GRADIENT), TEST_LOCATION);
  DALI_TEST_CHECK(view.GetBackgroundColor().IsNone());
  DALI_TEST_EQUALS(GetBackgroundGradientStopColor(view, 0u), BACKGROUND_GRADIENT_START_A, TEST_LOCATION);
  DALI_TEST_EQUALS(GetBackgroundGradientStopColor(view, 1u), BACKGROUND_GRADIENT_END_A, TEST_LOCATION);

  gUseAlternateBackgroundColors = true;
  manager.SetColorOverride(OverrideBackgroundColors);
  DALI_TEST_EQUALS(GetBackgroundGradientStopColor(view, 0u), BACKGROUND_GRADIENT_START_B, TEST_LOCATION);
  DALI_TEST_EQUALS(GetBackgroundGradientStopColor(view, 1u), BACKGROUND_GRADIENT_END_B, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetBackgroundColor().IsNone());

  view.SetBackgroundGradient(Gradient::Base::None());
  DALI_TEST_CHECK(!HasBackgroundVisual(view));

  gUseAlternateBackgroundColors = false;
  manager.SetColorOverride(OverrideBackgroundColors);
  DALI_TEST_CHECK(!HasBackgroundVisual(view));

  view.SetBackgroundColor(UiColor(BACKGROUND_COLOR_TOKEN));
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), BACKGROUND_COLOR_A, TEST_LOCATION);
  DALI_TEST_EQUALS(GetBackgroundMixColor(view), BACKGROUND_COLOR_A, TEST_LOCATION);

  gUseAlternateBackgroundColors = true;
  manager.SetColorOverride(OverrideBackgroundColors);
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), BACKGROUND_COLOR_B, TEST_LOCATION);
  DALI_TEST_EQUALS(GetBackgroundMixColor(view), BACKGROUND_COLOR_B, TEST_LOCATION);

  manager.ClearColorOverride();
  gUseAlternateBackgroundColors = false;

  END_TEST;
}

int UtcDaliViewShadowValueTypeP(void)
{
  UiTestApplication application;
  View              view = View::New();

  const UiColor originalColor(0.1f, 0.2f, 0.3f, 0.4f);
  const UiColor changedColor(0.8f, 0.7f, 0.6f, 0.5f);
  Shadow        shadow(12.0f, Vector2(3.0f, 4.0f), originalColor, Vector2(5.0f, 6.0f));

  view.SetShadow(shadow);
  shadow.SetColor(changedColor);
  shadow.SetBlurRadius(24.0f);

  Shadow appliedShadow = GetShadowProperty(view);
  DALI_TEST_EQUALS(appliedShadow.GetColor().GetRgba(), originalColor.GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(appliedShadow.GetBlurRadius(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(appliedShadow.GetOffset(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(appliedShadow.GetExtents(), Vector2(5.0f, 6.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewShadowStackReplaceAndClearP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Shadow shadow1(4.0f, Vector2(1.0f, 2.0f), UiColor(0.0f, 0.0f, 0.0f, 0.2f), Vector2(3.0f, 4.0f));
  Shadow shadow2(8.0f, Vector2(5.0f, 6.0f), UiColor(0.0f, 0.0f, 0.0f, 0.4f), Vector2(7.0f, 8.0f));
  Shadow shadow3(12.0f, Vector2(9.0f, 10.0f), UiColor(0.0f, 0.0f, 0.0f, 0.6f), Vector2(11.0f, 12.0f));

  view.SetShadow(shadow1);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), shadow1.GetBlurRadius(), TEST_LOCATION);

  ShadowStack stack{shadow2, shadow3};
  ShadowStack copiedStack(stack);
  DALI_TEST_EQUALS(stack.GetShadowCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetShadowAt(0u).GetBlurRadius(), shadow2.GetBlurRadius(), TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetShadowAt(1u).GetBlurRadius(), shadow3.GetBlurRadius(), TEST_LOCATION);
  stack.Clear();
  DALI_TEST_EQUALS(stack.GetShadowCount(), 0u, TEST_LOCATION);

  view.SetShadow(copiedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), shadow2.GetBlurRadius(), TEST_LOCATION);

  copiedStack.Add(shadow1);
  view.SetShadow(copiedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), shadow2.GetBlurRadius(), TEST_LOCATION);

  view.SetProperty(View::Property::SHADOW, Extension::Shadow::CreatePropertyMap(shadow1));
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), shadow1.GetBlurRadius(), TEST_LOCATION);

  view.SetShadow(copiedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 2u, TEST_LOCATION);
  view.SetProperty(View::Property::SHADOW, Property::Map());
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  Property::Value emptyShadowPropertyValue = view.GetProperty(View::Property::SHADOW);
  DALI_TEST_CHECK(emptyShadowPropertyValue.GetMap() && emptyShadowPropertyValue.GetMap()->Empty());

  ShadowStack deepCopiedStack{shadow2};
  shadow2.SetBlurRadius(99.0f);
  view.SetShadow(deepCopiedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), 8.0f, TEST_LOCATION);

  ShadowStack assignedStack;
  assignedStack = deepCopiedStack;
  deepCopiedStack.Clear();
  view.SetShadow(assignedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), 8.0f, TEST_LOCATION);

  view.SetShadow(stack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  Property::Value emptyShadowStackValue = view.GetProperty(View::Property::SHADOW);
  DALI_TEST_CHECK(emptyShadowStackValue.GetMap() && emptyShadowStackValue.GetMap()->Empty());

  view.SetShadow(shadow1);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetShadowProperty(view).GetBlurRadius(), shadow1.GetBlurRadius(), TEST_LOCATION);

  view.SetShadow(Shadow::None());
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 0u, TEST_LOCATION);
  Property::Value shadowValue = view.GetProperty(View::Property::SHADOW);
  DALI_TEST_CHECK(shadowValue.GetMap() && shadowValue.GetMap()->Empty());

  ColorVisual visual = ColorVisual::New();
  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), true, TEST_LOCATION);

  view.SetShadow(copiedStack);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 3u, TEST_LOCATION);

  view.SetShadow(Shadow::None());
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND, 0u), visual, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewShadowAnimationNoShadowP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Animation bridgeAnimation = Animation::New(0.0f);
  view.Animate(bridgeAnimation)
    .ShadowBlurRadius(8.0f, Duration(0.2f))
    .ShadowOpacity(0.25f, Duration(0.2f));
  DALI_TEST_EQUALS(bridgeAnimation.GetDuration(), 0.2f, TEST_LOCATION);

  ViewAnimationSpec spec = View::NewAnimationSpec();
  spec.ShadowBlurRadius(4.0f, Duration(0.1f))
    .ShadowOpacity(0.5f, Duration(0.1f))
    .Color(Vector4(0.8f, 0.7f, 0.6f, 0.5f), Duration(0.1f))
    .ColorBy(Vector4(0.1f, 0.1f, 0.1f, 0.1f), Duration(0.1f));

  Animation specAnimation = Animation::New(0.0f);
  spec.ApplyTo(specAnimation, view);
  DALI_TEST_EQUALS(specAnimation.GetDuration(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewShadowAnimationPrimaryShadowP(void)
{
  UiTestApplication application;
  View              view = View::New();

  view.SetShadow(Shadow(0.0f, Vector2::ZERO, UiColor(0.0f, 0.0f, 0.0f, 0.5f)));
  application.GetWindow().Add(view);
  application.SendNotification();
  application.Render();

  Dali::Property blurProperty    = IntegrationView::GetVisualProperty(view, View::Property::SHADOW, ColorVisualPropertyIndex::BLUR_RADIUS);
  Dali::Property opacityProperty = IntegrationView::GetVisualProperty(view, View::Property::SHADOW, VisualBasePropertyIndex::OPACITY);
  DALI_TEST_CHECK(blurProperty.propertyIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(opacityProperty.propertyIndex != Property::INVALID_INDEX);

  Animation bridgeAnimation = Animation::New(0.0f);
  view.Animate(bridgeAnimation)
    .ShadowBlurRadius(12.0f, Duration(0.1f))
    .ShadowOpacity(0.25f, Duration(0.1f));
  bridgeAnimation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(100);

  DALI_TEST_EQUALS(blurProperty.object.GetCurrentProperty<float>(blurProperty.propertyIndex), 12.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(opacityProperty.object.GetCurrentProperty<float>(opacityProperty.propertyIndex), 0.25f, 0.01f, TEST_LOCATION);

  ViewAnimationSpec spec = View::NewAnimationSpec();
  spec.ShadowBlurRadius(4.0f, Duration(0.1f))
    .ShadowOpacity(0.75f, Duration(0.1f));

  Animation specAnimation = Animation::New(0.0f);
  spec.ApplyTo(specAnimation, view);
  specAnimation.Play();
  application.SendNotification();
  application.Render(0);
  application.Render(100);

  DALI_TEST_EQUALS(blurProperty.object.GetCurrentProperty<float>(blurProperty.propertyIndex), 4.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(opacityProperty.object.GetCurrentProperty<float>(opacityProperty.propertyIndex), 0.75f, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewMultipleChainingP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  const float       testWidth  = 300.0f;
  const float       testHeight = 200.0f;
  const float       testX      = 100.0f;
  const float       testY      = 50.0f;
  const UiColor     testColor(0.0f, 1.0f, 0.0f, 0.8f);
  view.SetRequestedWidth(testWidth);
  view.SetRequestedHeight(testHeight);
  view.SetRequestedX(testX);
  view.SetRequestedY(testY);
  view.SetBackgroundColor(testColor);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), testHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedX(), testX, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetRequestedY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetParentOriginP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Vector3 parentOrigin = view.GetParentOrigin();
  DALI_TEST_EQUALS(parentOrigin.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetParentOriginP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const Vector3     testOrigin(0.0f, 1.0f, 0.5f);
  view.SetParentOrigin(testOrigin);

  Vector3 parentOrigin = view.GetParentOrigin();
  DALI_TEST_EQUALS(parentOrigin.x, testOrigin.x, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.y, testOrigin.y, TEST_LOCATION);
  DALI_TEST_EQUALS(parentOrigin.z, testOrigin.z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewGetPivotP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Vector3 pivot = view.GetPivot();
  DALI_TEST_EQUALS(pivot.x, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.y, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.z, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetPivotP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const Vector3     testPivot(1.0f, 0.0f, 0.5f);
  view.SetPivot(testPivot);

  Vector3 pivot = view.GetPivot();
  DALI_TEST_EQUALS(pivot.x, testPivot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.y, testPivot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(pivot.z, testPivot.z, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewParentOriginSetterP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const Vector3     testOrigin(0.0f, 0.0f, 0.0f);
  view.SetParentOrigin(testOrigin);
  DALI_TEST_EQUALS(view.GetParentOrigin(), testOrigin, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewPivotSetterP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const Vector3     testPivot(1.0f, 1.0f, 1.0f);
  view.SetPivot(testPivot);
  DALI_TEST_EQUALS(view.GetPivot(), testPivot, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewSetTraitP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait    = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));

  DummyTrait retrievedTrait = GetDummyTrait(viewImpl, TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(retrievedTrait);
  DALI_TEST_CHECK(retrievedTrait == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetTraitP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait    = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));

  DummyTrait retrievedTrait = GetDummyTrait(viewImpl, TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(retrievedTrait);

  DummyTrait nonExistentTrait = GetDummyTrait(viewImpl, TEST_TRAIT_ID_1);
  DALI_TEST_CHECK(!nonExistentTrait);
  END_TEST;
}

int UtcDaliViewRemoveTraitP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait    = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));
  DALI_TEST_CHECK(IntegrationView::GetTrait(viewImpl, TEST_TRAIT_ID_0));

  bool removed = IntegrationView::RemoveTrait(viewImpl, TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(removed);
  DALI_TEST_CHECK(!IntegrationView::GetTrait(viewImpl, TEST_TRAIT_ID_0));
  DALI_TEST_EQUALS(trait.GetImpl().GetDetachingCount(), 1, TEST_LOCATION);

  removed = IntegrationView::RemoveTrait(viewImpl, TEST_TRAIT_ID_0);
  DALI_TEST_CHECK(!removed);
  END_TEST;
}

int UtcDaliViewReplaceTraitP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait1   = DummyTrait::New();
  DummyTrait        trait2   = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait1));
  DALI_TEST_CHECK(GetDummyTrait(viewImpl, TEST_TRAIT_ID_0) == trait1);

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait2));
  DALI_TEST_CHECK(GetDummyTrait(viewImpl, TEST_TRAIT_ID_0) == trait2);
  DALI_TEST_EQUALS(trait1.GetImpl().GetDetachingCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(trait2.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetSameTraitP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait    = DummyTrait::New();

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));
  DummyTrait retrieved1 = GetDummyTrait(viewImpl, TEST_TRAIT_ID_0);

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));
  DummyTrait retrieved2 = GetDummyTrait(viewImpl, TEST_TRAIT_ID_0);

  DALI_TEST_CHECK(retrieved1 == retrieved2);
  DALI_TEST_CHECK(retrieved1 == trait);
  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTraitLifecycleP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  ViewImpl&         viewImpl = GetImpl(view);
  DummyTrait        trait    = DummyTrait::New();

  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 0, TEST_LOCATION);

  IntegrationView::SetTrait(viewImpl, TEST_TRAIT_ID_0, ToTraitObject(trait));

  DALI_TEST_EQUALS(trait.GetImpl().GetAttachedCount(), 1, TEST_LOCATION);

  IntegrationView::RemoveTrait(viewImpl, TEST_TRAIT_ID_0);

  DALI_TEST_EQUALS(trait.GetImpl().GetDetachingCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedWidthP(void)
{
  UiTestApplication application;
  View              view  = View::New();
  const float       width = 200.0f;
  view.SetRequestedWidth(width);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), width, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetRequestedWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetRequestedWidth(), WRAP_CONTENT, TEST_LOCATION);
  view.SetRequestedWidth(300.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 300.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightP(void)
{
  UiTestApplication application;
  View              view   = View::New();
  const float       height = 100.0f;
  view.SetRequestedHeight(height);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), height, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetRequestedHeightP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetRequestedHeight(), WRAP_CONTENT, TEST_LOCATION);
  view.SetRequestedHeight(250.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 250.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMarginP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            margin(10.0f, 20.0f, 30.0f, 40.0f);
  view.SetMargin(margin);
  Insets got = view.GetMargin();
  DALI_TEST_EQUALS(got.start, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMarginP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            got  = view.GetMargin();
  DALI_TEST_EQUALS(got.start, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetPaddingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            padding(5.0f, 15.0f, 25.0f, 35.0f);
  view.SetPadding(padding);
  Insets got = view.GetPadding();
  DALI_TEST_EQUALS(got.start, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.end, 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.top, 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.bottom, 35.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPaddingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            got  = view.GetPadding();
  DALI_TEST_EQUALS(got.start, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInsetsHorizontalVerticalConstructorP(void)
{
  UiTestApplication application;

  const Insets insets(12.5f, 7.5f);
  DALI_TEST_EQUALS(insets, Insets(12.5f, 12.5f, 7.5f, 7.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMarginHelpersP(void)
{
  UiTestApplication application;
  View              view = View::New();

  view.SetMargin(1, 2, 3, 4);
  DALI_TEST_EQUALS(view.GetMargin(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);

  view.SetMargin(5, 6);
  DALI_TEST_EQUALS(view.GetMargin(), Insets(5.0f, 5.0f, 6.0f, 6.0f), TEST_LOCATION);

  view.SetMargin(7);
  DALI_TEST_EQUALS(view.GetMargin(), Insets(7.0f, 7.0f, 7.0f, 7.0f), TEST_LOCATION);

  view.SetStartMargin(8);
  view.SetEndMargin(9);
  view.SetTopMargin(10);
  view.SetBottomMargin(11);
  DALI_TEST_EQUALS(view.GetMargin(), Insets(8.0f, 9.0f, 10.0f, 11.0f), TEST_LOCATION);

  view.SetMargin(0.5f, 1.5f, 2.5f, 3.5f);
  DALI_TEST_EQUALS(view.GetMargin(), Insets(0.5f, 1.5f, 2.5f, 3.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPaddingHelpersP(void)
{
  UiTestApplication application;
  View              view = View::New();

  view.SetPadding(1, 2, 3, 4);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);

  view.SetPadding(5, 6);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(5.0f, 5.0f, 6.0f, 6.0f), TEST_LOCATION);

  view.SetPadding(7);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(7.0f, 7.0f, 7.0f, 7.0f), TEST_LOCATION);

  view.SetStartPadding(8);
  view.SetEndPadding(9);
  view.SetTopPadding(10);
  view.SetBottomPadding(11);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(8.0f, 9.0f, 10.0f, 11.0f), TEST_LOCATION);

  view.SetPadding(0.5f, 1.5f, 2.5f, 3.5f);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(0.5f, 1.5f, 2.5f, 3.5f), TEST_LOCATION);

  float  assignedValues[] = {12.5f, 13.5f, 14.5f, 15.5f};
  Insets assignedInsets;
  assignedInsets = assignedValues;
  DALI_TEST_EQUALS(assignedInsets, Insets(12.5f, 13.5f, 14.5f, 15.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutWidthChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedWidth(150.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedHeight(80.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureP(void)
{
  UiTestApplication application;
  View              view = View::New();
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
  View              view = View::New();
  LayoutRect        bounds(10.0f, 20.0f, 100.0f, 80.0f);
  LayoutRect        size = view.Arrange(bounds);
  DALI_TEST_EQUALS(size.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeAdoptsReturnedBoundsP(void)
{
  UiTestApplication application;
  View              view = View::New();

  // A callback returning a self rect different from the input slot on all four
  // axes; the framework must adopt x/y/width/height as the final self geometry.
  view.SetArrangeCallback(ArrangeCallback::New(&CustomBoundsArrange));

  LayoutRect result = view.Arrange(LayoutRect(5.0f, 5.0f, 200.0f, 100.0f));

  // Arrange() returns the adopted final bounds (all four axes).
  DALI_TEST_EQUALS(result.x, 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.y, 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.width, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.height, 40.0f, TEST_LOCATION);

  // The self actor geometry reflects the returned rect on all four axes.
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_X), 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Y), 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewGetMeasuredSizeP(void)
{
  UiTestApplication application;
  View              view    = View::New();
  MeasuredSize      desired = view.GetMeasuredSize();
  DALI_TEST_EQUALS(desired.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(desired.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMinimumWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMinimumWidth(50.0f);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMinimumHeightP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMinimumHeight(30.0f);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMinimumHeightP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMaximumWidth(500.0f);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 500.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumWidth(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetMaximumHeightP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetMaximumHeightP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(view.GetMaximumHeight(), std::numeric_limits<float>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBackgroundColorP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_CHECK(view);
  UiColor color(0.2f, 0.4f, 0.6f, 1.0f);
  view.SetBackgroundColor(color);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewMarginChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            margin(1.0f, 2.0f, 3.0f, 4.0f);
  view.SetMargin(margin);
  DALI_TEST_EQUALS(view.GetMargin().start, 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPaddingChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Insets            padding(5.0f, 10.0f, 15.0f, 20.0f);
  view.SetPadding(padding);
  DALI_TEST_EQUALS(view.GetPadding().start, 5.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMinimumWidthChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMinimumWidth(25.0f);
  DALI_TEST_EQUALS(view.GetMinimumWidth(), 25.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMinimumHeightChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMinimumHeight(35.0f);
  DALI_TEST_EQUALS(view.GetMinimumHeight(), 35.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumWidthChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMaximumWidth(300.0f);
  DALI_TEST_EQUALS(view.GetMaximumWidth(), 300.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMaximumHeightChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetMaximumHeight(400.0f);
  DALI_TEST_EQUALS(view.GetMaximumHeight(), 400.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureCacheHitP(void)
{
  UiTestApplication application;
  View              view = View::New();
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
  View              view = View::New();
  view.SetMargin(Insets(10.0f, 10.0f, 10.0f, 10.0f));
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
  View              view = View::New();
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
  View              view = View::New();
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
  View              view = View::New();
  view.SetRequestedWidth(120.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutHeightFixedNoManagerP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedHeight(80.0f);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewArrangeWithLayoutP(void)
{
  UiTestApplication application;
  StackLayout       layout = StackLayout::New(StackOrientation::VERTICAL);
  View              c1     = View::New();
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
  LayoutRect   arranged = layout.Arrange(LayoutRect(0, 0, 200, 200));
  DALI_TEST_EQUALS(measured.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(arranged.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(arranged.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMeasureWithPaddingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetPadding(Insets(5.0f, 5.0f, 5.0f, 5.0f));
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
  View              view = View::New();
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
  StackLayout       layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetRequestedWidth(MATCH_PARENT);
  layout.SetRequestedHeight(MATCH_PARENT);
  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  layout.Add(child);
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
  View              view = View::New();
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
  View              view = View::New();
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
  View              view = View::New();
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
  View              view = View::New();
  view.SetRequestedWidth(0.0f);
  DALI_TEST_EQUALS(view.GetRequestedWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightZeroP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedHeight(0.0f);
  DALI_TEST_EQUALS(view.GetRequestedHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Setter / property equivalence for REQUESTED_WIDTH and REQUESTED_HEIGHT.
//
// The C++ setter no longer routes through Handle::SetProperty; it calls the same
// ViewDataImpl::ApplyRequested* the registered property callback calls, and then
// replays dali-core's observable tail. That is only sound while the two routes
// remain indistinguishable, so this pins every outcome the shared implementation
// decides: the stored value, the near-sentinel snap, the rejection of an invalid
// input, and the parentless-leaf immediate actor-size application. A future change
// that gives either route a private copy of the logic fails here.
// =============================================================================

int UtcDaliViewSetRequestedWidthMatchesPropertyRouteP(void)
{
  UiTestApplication application;
  tet_infoline("SetRequestedWidth and SetProperty(REQUESTED_WIDTH) agree on every outcome");

  View viaSetter   = View::New();
  View viaProperty = View::New();

  // Plain value. Both views are parentless, childless and have no layout
  // capability, so the write must also reach the actor immediately on both routes.
  Ui::GetImpl(viaSetter).SetRequestedWidth(120.0f);
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, 120.0f);
  DALI_TEST_EQUALS(viaSetter.GetRequestedWidth(), viaProperty.GetRequestedWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetRequestedWidth(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetSize().width, viaProperty.GetSize().width, TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetSize().width, 120.0f, TEST_LOCATION);

  // Near-sentinel snap. EXACT comparison on purpose: the point of the snap is that
  // downstream `== WRAP_CONTENT` tests hold, and an epsilon compare could not tell
  // a snapped value from a stored -1.0005f.
  Ui::GetImpl(viaSetter).SetRequestedWidth(-1.0005f);
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, -1.0005f);
  DALI_TEST_CHECK(viaSetter.GetRequestedWidth() == WRAP_CONTENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedWidth() == WRAP_CONTENT);

  Ui::GetImpl(viaSetter).SetRequestedWidth(-2.0005f);
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, -2.0005f);
  DALI_TEST_CHECK(viaSetter.GetRequestedWidth() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedWidth() == MATCH_PARENT);

  // Rejection. An invalid input leaves the previous value standing, identically.
  Ui::GetImpl(viaSetter).SetRequestedWidth(-3.0f);
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, -3.0f);
  DALI_TEST_CHECK(viaSetter.GetRequestedWidth() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedWidth() == MATCH_PARENT);

  Ui::GetImpl(viaSetter).SetRequestedWidth(std::numeric_limits<float>::quiet_NaN());
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_CHECK(viaSetter.GetRequestedWidth() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedWidth() == MATCH_PARENT);

  Ui::GetImpl(viaSetter).SetRequestedWidth(std::numeric_limits<float>::infinity());
  viaProperty.SetProperty(View::Property::REQUESTED_WIDTH, std::numeric_limits<float>::infinity());
  DALI_TEST_CHECK(viaSetter.GetRequestedWidth() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedWidth() == MATCH_PARENT);

  END_TEST;
}

int UtcDaliViewSetRequestedHeightMatchesPropertyRouteP(void)
{
  UiTestApplication application;
  tet_infoline("SetRequestedHeight and SetProperty(REQUESTED_HEIGHT) agree on every outcome");

  View viaSetter   = View::New();
  View viaProperty = View::New();

  Ui::GetImpl(viaSetter).SetRequestedHeight(90.0f);
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, 90.0f);
  DALI_TEST_EQUALS(viaSetter.GetRequestedHeight(), viaProperty.GetRequestedHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetRequestedHeight(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetSize().height, viaProperty.GetSize().height, TEST_LOCATION);
  DALI_TEST_EQUALS(viaSetter.GetSize().height, 90.0f, TEST_LOCATION);

  Ui::GetImpl(viaSetter).SetRequestedHeight(-1.0005f);
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, -1.0005f);
  DALI_TEST_CHECK(viaSetter.GetRequestedHeight() == WRAP_CONTENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedHeight() == WRAP_CONTENT);

  Ui::GetImpl(viaSetter).SetRequestedHeight(-2.0005f);
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, -2.0005f);
  DALI_TEST_CHECK(viaSetter.GetRequestedHeight() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedHeight() == MATCH_PARENT);

  Ui::GetImpl(viaSetter).SetRequestedHeight(-3.0f);
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, -3.0f);
  DALI_TEST_CHECK(viaSetter.GetRequestedHeight() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedHeight() == MATCH_PARENT);

  Ui::GetImpl(viaSetter).SetRequestedHeight(std::numeric_limits<float>::quiet_NaN());
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, std::numeric_limits<float>::quiet_NaN());
  DALI_TEST_CHECK(viaSetter.GetRequestedHeight() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedHeight() == MATCH_PARENT);

  Ui::GetImpl(viaSetter).SetRequestedHeight(std::numeric_limits<float>::infinity());
  viaProperty.SetProperty(View::Property::REQUESTED_HEIGHT, std::numeric_limits<float>::infinity());
  DALI_TEST_CHECK(viaSetter.GetRequestedHeight() == MATCH_PARENT);
  DALI_TEST_CHECK(viaProperty.GetRequestedHeight() == MATCH_PARENT);

  END_TEST;
}

int UtcDaliViewMeasureZeroWidthP(void)
{
  UiTestApplication application;
  View              view = View::New();
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
  View              view = View::New();
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
  View              view = View::New();
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
  View              view   = View::New();
  Vector4           radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusUniformP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  const float       testRadius = 10.0f;
  view.SetCornerRadius(testRadius);

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
  View              view        = View::New();
  const float       topLeft     = 5.0f;
  const float       topRight    = 10.0f;
  const float       bottomRight = 15.0f;
  const float       bottomLeft  = 20.0f;
  view.SetCornerRadius(topLeft, topRight, bottomRight, bottomLeft);

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
  View              view = View::New();
  const Vector4     testRadius(8.0f, 12.0f, 16.0f, 20.0f);
  view.SetCornerRadius(testRadius);

  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, testRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.y, testRadius.y, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.z, testRadius.z, TEST_LOCATION);
  DALI_TEST_EQUALS(radius.w, testRadius.w, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetCornerRadiusPolicyP(void)
{
  UiTestApplication  application;
  View               view   = View::New();
  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::ABSOLUTE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusPolicyP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::RELATIVE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerRadiusPolicyRelativeP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetCornerRadiusPolicyRelative();

  CornerRadiusPolicy policy = view.GetCornerRadiusPolicy();
  DALI_TEST_EQUALS(static_cast<int>(policy), static_cast<int>(CornerRadiusPolicy::RELATIVE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewIsCornerRadiusPolicyRelativeP(void)
{
  UiTestApplication application;
  View              view = View::New();

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
  View              view = View::New();
  view.SetCornerRadius(10.0f);
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  DALI_TEST_CHECK(view.IsCornerRadiusPolicyRelative());
  END_TEST;
}

// Corner Squareness API tests

int UtcDaliViewGetCornerSquarenessP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  Vector4           squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.z, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(squareness.w, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetCornerSquarenessUniformP(void)
{
  UiTestApplication application;
  View              view           = View::New();
  const float       testSquareness = 0.5f;
  view.SetCornerSquareness(testSquareness);

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
  View              view        = View::New();
  const float       topLeft     = 0.2f;
  const float       topRight    = 0.4f;
  const float       bottomRight = 0.6f;
  const float       bottomLeft  = 0.8f;
  view.SetCornerSquareness(topLeft, topRight, bottomRight, bottomLeft);

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
  View              view = View::New();
  const Vector4     testSquareness(0.1f, 0.3f, 0.5f, 0.7f);
  view.SetCornerSquareness(testSquareness);

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
  View              view = View::New();
  view.SetCornerSquareness(0.5f);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewCornerRadiusAndSquarenessCombinedP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetCornerRadius(15.0f, 20.0f, 25.0f, 30.0f);
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  view.SetCornerSquareness(0.3f, 0.4f, 0.5f, 0.6f);

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
  View              view  = View::New();
  float             width = view.GetBorderlineWidth();
  DALI_TEST_EQUALS(width, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineWidthP(void)
{
  UiTestApplication application;
  View              view      = View::New();
  const float       testWidth = 5.0f;
  view.SetBorderlineWidth(testWidth);

  float width = view.GetBorderlineWidth();
  DALI_TEST_EQUALS(width, testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineWidthChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetBorderlineWidth(10.0f);
  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 10.0f, TEST_LOCATION);
  END_TEST;
}

// Borderline Color API tests

int UtcDaliViewGetBorderlineColorP(void)
{
  UiTestApplication application;
  View              view     = View::New();
  UiColor           color    = view.GetBorderlineColor();
  Vector4           resolved = color.GetRgba();
  // Default color should be black/transparent
  DALI_TEST_EQUALS(resolved.r, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineColorP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const UiColor     testColor(1.0f, 0.5f, 0.25f, 1.0f);
  view.SetBorderlineColor(testColor);

  UiColor color    = view.GetBorderlineColor();
  Vector4 resolved = color.GetRgba();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineColorChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const UiColor     testColor(0.2f, 0.4f, 0.6f, 0.8f);
  view.SetBorderlineColor(testColor);

  UiColor color    = view.GetBorderlineColor();
  Vector4 resolved = color.GetRgba();
  DALI_TEST_EQUALS(resolved.r, 0.2f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.4f, TEST_LOCATION);
  END_TEST;
}

// Borderline Offset API tests

int UtcDaliViewGetBorderlineOffsetP(void)
{
  UiTestApplication application;
  View              view   = View::New();
  float             offset = view.GetBorderlineOffset();
  DALI_TEST_EQUALS(offset, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetBorderlineOffsetP(void)
{
  UiTestApplication application;
  View              view       = View::New();
  const float       testOffset = 2.5f;
  view.SetBorderlineOffset(testOffset);

  float offset = view.GetBorderlineOffset();
  DALI_TEST_EQUALS(offset, testOffset, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineOffsetChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetBorderlineOffset(3.0f);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 3.0f, TEST_LOCATION);
  END_TEST;
}

// Borderline Combined API tests

int UtcDaliViewBorderlineCombinedP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const UiColor     testColor(1.0f, 0.0f, 0.0f, 1.0f);
  view.SetBorderlineWidth(4.0f);
  view.SetBorderlineColor(testColor);
  view.SetBorderlineOffset(1.5f);

  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 1.5f, TEST_LOCATION);

  UiColor color    = view.GetBorderlineColor();
  Vector4 resolved = color.GetRgba();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBorderlineWithCornerRadiusCombinedP(void)
{
  UiTestApplication application;
  View              view = View::New();
  const UiColor     borderColor(0.0f, 1.0f, 0.0f, 1.0f);
  view.SetCornerRadius(10.0f);
  view.SetCornerSquareness(0.5f);
  view.SetBorderlineWidth(2.0f);
  view.SetBorderlineColor(borderColor);
  view.SetBorderlineOffset(0.5f);

  // Verify all values
  Vector4 radius = view.GetCornerRadius();
  DALI_TEST_EQUALS(radius.x, 10.0f, TEST_LOCATION);

  Vector4 squareness = view.GetCornerSquareness();
  DALI_TEST_EQUALS(squareness.x, 0.5f, TEST_LOCATION);

  DALI_TEST_EQUALS(view.GetBorderlineWidth(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetBorderlineOffset(), 0.5f, TEST_LOCATION);

  UiColor color    = view.GetBorderlineColor();
  Vector4 resolved = color.GetRgba();
  DALI_TEST_EQUALS(resolved.g, 1.0f, TEST_LOCATION);
  END_TEST;
}

// LayoutMode::STANDALONE tests

int UtcDaliViewSetLayoutModeP(void)
{
  UiTestApplication application;
  View              view = View::New();
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::DEFAULT), TEST_LOCATION);
  view.SetLayoutMode(LayoutMode::STANDALONE);
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::STANDALONE), TEST_LOCATION);
  view.SetLayoutMode(LayoutMode::DEFAULT);
  DALI_TEST_EQUALS(static_cast<int>(view.GetLayoutMode()), static_cast<int>(LayoutMode::DEFAULT), TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewStandaloneIgnoresParentPaddingMatchParentP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
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
  View              parent = View::New();
  parent.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetMargin(Insets(5.0f, 5.0f, 7.0f, 7.0f));
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
  View              parent = View::New();
  parent.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(30.0f);
  child.SetRequestedX(50.0f);
  child.SetRequestedY(60.0f);
  parent.Add(child);

  parent.Measure(200.0f, 150.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(child.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionX(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 60.0f, TEST_LOCATION);
  END_TEST;
}

// A Standalone child has no parent layout to clamp it, so its own min/max is applied
// to the slot the parent places it in. For a MATCH_PARENT axis that slot derivation is
// the only place the clamp can reach, because the measured value is discarded there.
//
// The second settle is what makes this test about the PARENT's derivation: a boundary
// view is also a layout root of its own, and its own root pass is the last to arrange
// it in the first batch. Invalidating only the parent's arrange makes the parent
// re-derive the slot and be the last writer.
int UtcDaliViewStandaloneMaximumClampAppliesFromParentPassP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(150.0f);
  application.GetScene().Add(parent);

  // Clamped DOWN by its own maximum on both axes.
  View capped = View::New();
  capped.SetLayoutMode(LayoutMode::STANDALONE);
  capped.SetRequestedWidth(MATCH_PARENT);
  capped.SetRequestedHeight(MATCH_PARENT);
  capped.SetMaximumWidth(60.0f);
  capped.SetMaximumHeight(45.0f);
  parent.Add(capped);

  // Clamped UP by its own minimum, above the parent extent.
  View wide = View::New();
  wide.SetLayoutMode(LayoutMode::STANDALONE);
  wide.SetRequestedWidth(MATCH_PARENT);
  wide.SetRequestedHeight(MATCH_PARENT);
  wide.SetMinimumWidth(300.0f);
  parent.Add(wide);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(capped.GetProperty<float>(Actor::Property::SIZE_WIDTH), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(capped.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(wide.GetProperty<float>(Actor::Property::SIZE_WIDTH), 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(wide.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 150.0f, TEST_LOCATION);

  parent.InvalidateArrange();
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(capped.GetProperty<float>(Actor::Property::SIZE_WIDTH), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(capped.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(wide.GetProperty<float>(Actor::Property::SIZE_WIDTH), 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(wide.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 150.0f, TEST_LOCATION);

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
  standalone.SetRequestedX(1000.0f);
  standalone.SetRequestedY(1000.0f);
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

  bool operator()(View view, KeyEvent event)
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
  View              view = View::New();
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

// =============================================================================
// SetLeftFocusableView: MoveFocus(LEFT) moves to the designated view
// =============================================================================

int UtcDaliViewSetLeftFocusableViewP(void)
{
  UiTestApplication application;
  View              viewA = CreateFocusableView(application);
  View              viewB = CreateFocusableView(application);

  viewA.SetLeftFocusableView(viewB);

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

  center.SetLeftFocusableView(left);
  center.SetRightFocusableView(right);
  center.SetUpFocusableView(up);
  center.SetDownFocusableView(down);

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

  void OnSet(Dali::Handle /*handle*/, Dali::Property::Index index, const Dali::Property::Value& value)
  {
    indices.push_back(index);
    values.push_back(value);
  }

  bool Saw(Dali::Property::Index index) const
  {
    return std::find(indices.begin(), indices.end(), index) != indices.end();
  }

  // Per-INDEX count, not the total: a setter's own work can emit unrelated properties
  // (the parentless-leaf immediate apply writes Actor SIZE_WIDTH, for one), so only a
  // per-index count can express "exactly once".
  uint32_t CountOf(Dali::Property::Index index) const
  {
    return static_cast<uint32_t>(std::count(indices.begin(), indices.end(), index));
  }

  float FirstFloatOf(Dali::Property::Index index) const
  {
    for(size_t i = 0u; i < indices.size(); ++i)
    {
      if(indices[i] == index)
      {
        float value = 0.0f;
        values[i].Get(value);
        return value;
      }
    }
    return std::numeric_limits<float>::quiet_NaN();
  }
};
} // namespace

int UtcDaliViewSetMarginFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  view.SetMargin(Insets(1.0f, 2.0f, 3.0f, 4.0f));

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::MARGIN));
  END_TEST;
}

int UtcDaliViewSetPaddingFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetPadding(Insets(5.0f, 6.0f, 7.0f, 8.0f));

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

int UtcDaliViewSetRequestedHeightFiresPropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetRequestedHeight(80.0f);

  DALI_TEST_CHECK(recorder.Saw(Ui::View::Property::REQUESTED_HEIGHT));
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

int UtcDaliViewBackgroundTypedSettersDoNotFirePropertySetSignalP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Gradient::Linear                 gradient(Vector2::ZERO, Vector2::ONE);
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(Color::RED)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(Color::BLUE)));
  gradient.SetStopNodes(stopNodes);

  view.SetBackgroundColor(UiColor(Color::GREEN));
  view.SetBackgroundImage(Dali::String("background-image.png"));
  view.SetBackgroundGradient(gradient);

  DALI_TEST_CHECK(!recorder.Saw(Ui::View::Property::BACKGROUND));
  END_TEST;
}

int UtcDaliViewTypedSetterAndSetPropertyConvergeP(void)
{
  // Both entry points must reach the same final state.
  UiTestApplication application;

  Ui::View viewA = Ui::View::New();
  viewA.SetMargin(Insets(7.0f, 8.0f, 9.0f, 10.0f));

  Ui::View viewB = Ui::View::New();
  Dali::Handle(viewB).SetProperty(Ui::View::Property::MARGIN, Vector4(7.0f, 8.0f, 9.0f, 10.0f));

  DALI_TEST_EQUALS(viewA.GetMargin(), viewB.GetMargin(), TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Handle(viewA).GetProperty<Vector4>(Ui::View::Property::MARGIN),
                   Dali::Handle(viewB).GetProperty<Vector4>(Ui::View::Property::MARGIN),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetPropertyAcceptsExtentsForSpacingP(void)
{
  UiTestApplication application;

  Ui::View view = Ui::View::New();
  Dali::Handle(view).SetProperty(Ui::View::Property::MARGIN, Extents(1, 2, 3, 4));
  Dali::Handle(view).SetProperty(Ui::View::Property::PADDING, Extents(5, 6, 7, 8));

  DALI_TEST_EQUALS(view.GetMargin(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPadding(), Insets(5.0f, 6.0f, 7.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Handle(view).GetProperty<Vector4>(Ui::View::Property::MARGIN),
                   Vector4(1.0f, 2.0f, 3.0f, 4.0f),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Handle(view).GetProperty<Vector4>(Ui::View::Property::PADDING),
                   Vector4(5.0f, 6.0f, 7.0f, 8.0f),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAttachmentSetGetP(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  view.SetAttachment(id, Dali::MakeUnique<int>(13));

  int* value = view.GetAttachment<int>(id);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(*value, 13, TEST_LOCATION);
  DALI_TEST_CHECK(!view.GetAttachment<float>(id));

  const View constView(view);
  const int* constValue = constView.GetAttachment<int>(id);
  DALI_TEST_CHECK(constValue);
  DALI_TEST_EQUALS(*constValue, 13, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAttachmentReplaceP(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  view.SetAttachment(id, Dali::MakeUnique<int>(13));
  view.SetAttachment(id, Dali::MakeUnique<int>(27));

  int* value = view.GetAttachment<int>(id);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(*value, 27, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAttachmentRemoveP(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  view.SetAttachment(id, Dali::MakeUnique<int>(13));

  DALI_TEST_CHECK(view.RemoveAttachment(id));
  DALI_TEST_CHECK(!view.GetAttachment<int>(id));
  DALI_TEST_CHECK(!view.RemoveAttachment(id));
  END_TEST;
}

int UtcDaliViewAttachmentDetachP(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  view.SetAttachment(id, Dali::MakeUnique<int>(43));

  Dali::UniquePtr<float> mismatch = view.DetachAttachment<float>(id);
  DALI_TEST_CHECK(!mismatch.Get());
  DALI_TEST_CHECK(view.GetAttachment<int>(id));

  Dali::UniquePtr<int> value = view.DetachAttachment<int>(id);
  DALI_TEST_CHECK(value.Get());
  DALI_TEST_EQUALS(*value, 43, TEST_LOCATION);
  DALI_TEST_CHECK(!view.GetAttachment<int>(id));
  DALI_TEST_CHECK(!view.DetachAttachment<int>(id).Get());
  END_TEST;
}

int UtcDaliViewAttachmentMoveOnlyP(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  view.SetAttachment(id, Dali::MakeUnique<ViewMoveOnlyAttachment>(31));

  ViewMoveOnlyAttachment* value = view.GetAttachment<ViewMoveOnlyAttachment>(id);
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->value, 31, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAttachmentSetNullN(void)
{
  UiTestApplication application;
  View              view = View::New();
  AttachmentId      id   = AttachmentId::Alloc();

  DALI_TEST_ASSERTION(view.SetAttachment(id, Dali::UniquePtr<int>()), "SetAttachment requires non-null data");
  END_TEST;
}

int UtcDaliViewGetEffectiveLayoutDirectionInheritedP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  View              child  = View::New();
  application.GetScene().Add(parent);
  parent.Add(child);

  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(child.GetLayoutDirection(), LayoutDirection::INHERIT, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  application.GetScene().Add(view);
  view.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(view.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionRtlMirrorsChildP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  parent.Measure(200.0f, 100.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  // newX = parentWidth(200) - oldX(20) - childWidth(50) = 130
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  // Size is unchanged by RTL.
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionLtrLeavesChildP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  // Default direction is LEFT_TO_RIGHT.
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  parent.Measure(200.0f, 100.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionRtlSkipsStandaloneChildP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(50.0f);
  standalone.SetRequestedHeight(30.0f);
  standalone.SetRequestedX(10.0f);
  standalone.SetRequestedY(15.0f);
  parent.Add(standalone);

  parent.Measure(200.0f, 100.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  // Standalone child is excluded from mirroring; stays at requested position.
  DALI_TEST_EQUALS(standalone.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionRtlMirrorsRecursivelyP(void)
{
  UiTestApplication application;
  View              grandParent = View::New();
  grandParent.SetRequestedWidth(300.0f);
  grandParent.SetRequestedHeight(200.0f);
  grandParent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(grandParent);

  View parent = View::New();
  parent.SetRequestedWidth(150.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetRequestedX(0.0f);
  grandParent.Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  child.SetRequestedX(10.0f);
  parent.Add(child);

  grandParent.Measure(300.0f, 200.0f);
  grandParent.Arrange(LayoutRect(0.0f, 0.0f, 300.0f, 200.0f));

  // Parent mirrored at grandParent level: 300 - 0 - 150 = 150.
  DALI_TEST_EQUALS(parent.GetPositionX(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetSize().width, 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetSize().height, 100.0f, TEST_LOCATION);
  // Child mirrored at parent level (parent inherits RTL): 150 - 10 - 50 = 90.
  DALI_TEST_EQUALS(child.GetPositionX(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutDirectionSetReArrangesP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  parent.Measure(200.0f, 100.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));
  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);

  // SetLayoutDirection now invalidates arrange; this test drives the pass
  // explicitly (see UtcDaliViewLayoutDirectionChangeRelayoutsSubtreeP for the
  // scheduled-pass version) and only checks that the mirror is applied.
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewLayoutDirectionRtlStableAcrossRepeatedArrangeP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  const LayoutRect slot(0.0f, 0.0f, 200.0f, 100.0f);
  parent.Measure(200.0f, 100.0f);
  parent.Arrange(slot);
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  // The mirror is idempotent: re-arranging the same tree into the same slot puts
  // the child at the same mirrored x every time. A mirror that folded the
  // already-mirrored actor position back in would only look stable while every
  // pass happens to rewrite the child's logical x first; the moment one does not,
  // it oscillates (130 -> 20 -> 130 ...). Mirroring the child's logical arranged
  // bounds removes that hidden precondition.
  for(int pass = 0; pass < 3; ++pass)
  {
    parent.Arrange(slot);
    DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliViewRtlMirrorIgnoresExternalActorPositionWriteP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  const LayoutRect slot(0.0f, 0.0f, 200.0f, 100.0f);
  parent.Measure(200.0f, 100.0f);
  parent.Arrange(slot);
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  // A producer that leaves its children alone, so nothing rewrites the child's
  // actor POSITION_X back to its logical value before the mirror runs. The
  // child keeps the arranged bounds it already has.
  parent.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));

  // The sanctioned escape hatch for driving rendered geometry directly.
  Dali::Ui::Extension::View::SetPositionX(child, 7.0f);
  DALI_TEST_EQUALS(child.GetPositionX(), 7.0f, TEST_LOCATION);

  parent.Arrange(slot);

  // Mirrored from the child's LOGICAL arranged x (20), not from the actor:
  // 200 - 20 - 50 = 130. Reading the actor back would yield 200 - 7 - 50 = 143.
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  END_TEST;
}

// THE LIVE BUG this pins: a layout-direction change once reached NO layout
// invalidation at all. Actor::SetLayoutDirection emits Core's
// LayoutDirectionChangedSignal and issues a legacy RelayoutRequest (size
// negotiation), and neither of those is the dali-ui layout pass, so flipping a
// SETTLED tree to RTL and driving frames did nothing: no mirror was ever applied.
// Every other RTL test hides this by calling Arrange() explicitly.
//
// What this test pins today is that SOMETHING turns the change into an
// invalidation for a settled tree driven only by real passes. Here the flip is on
// the layout root, so both live mechanisms would cover it -- the root's lazy
// signal hook (which is what actually runs; the hook is checked first and
// OnPropertySet defers to it) and, if that hook were absent, the LAYOUT_DIRECTION
// case of ViewDataImpl::OnPropertySet. The mid-tree case, where only the property
// interception can see the change, is
// UtcDaliViewLayoutDirectionChangeOnMidTreeViewRelayoutsSubtreeP.
//
// This test therefore uses the REAL pass only -- no explicit Measure/Arrange.
int UtcDaliViewLayoutDirectionChangeRelayoutsSubtreeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  window.Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  // Settle: the default LEFT_TO_RIGHT arrangement puts the child at its
  // requested (logical) x. A non-zero x here also proves a real pass ran.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);

  // The direction change alone must schedule the re-arrange.
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  application.SendNotification();
  application.SendNotification();

  // newX = parentWidth(200) - logicalX(20) - childWidth(50) = 130.
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  END_TEST;
}

// The change is set on the GRANDPARENT only; the two descendants merely INHERIT
// it. Core resolves inheritance by walking the actor tree, and dali-ui mirrors
// that walk from the one view that observed the change: the grandparent is the
// layout root here, its lazy signal hook fires, and its handler recurses down the
// inherit chain (InvalidateSubtreeLayoutForDirectionChange). Neither descendant
// holds a hook or sees a property write of its own, so this is exactly the case
// the recursive walk exists for -- a walk that stopped at the changed node would
// leave both descendants unmirrored.
int UtcDaliViewLayoutDirectionChangeOnAncestorRelayoutsDescendantP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View grandParent = View::New();
  grandParent.SetRequestedWidth(300.0f);
  grandParent.SetRequestedHeight(200.0f);
  window.Add(grandParent);

  View parent = View::New();
  parent.SetRequestedWidth(150.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetRequestedX(0.0f);
  grandParent.Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  child.SetRequestedX(10.0f);
  parent.Add(child);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(parent.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionX(), 10.0f, TEST_LOCATION);

  grandParent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(parent.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  application.SendNotification();
  application.SendNotification();

  // Parent mirrored by the grandparent: 300 - 0 - 150 = 150.
  DALI_TEST_EQUALS(parent.GetPositionX(), 150.0f, TEST_LOCATION);
  // Child mirrored by the parent, which only inherited the change: 150 - 10 - 50 = 90.
  DALI_TEST_EQUALS(child.GetPositionX(), 90.0f, TEST_LOCATION);

  END_TEST;
}

// The invalidation is symmetric: reverting to LEFT_TO_RIGHT must un-mirror
// through the real pass too. This also pins that the revert restores the LOGICAL
// x exactly (a mirror folded back over the already-mirrored actor position would
// land somewhere else).
int UtcDaliViewLayoutDirectionRevertRelayoutsSubtreeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  window.Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  parent.Add(child);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  parent.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);

  application.SendNotification();
  application.SendNotification();

  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);

  END_TEST;
}

// The change is set on a MID-TREE View: not the layout root, so it holds no
// signal hook of its own, and its subtree has to be reached from the property
// write instead (the LAYOUT_DIRECTION case of ViewDataImpl::OnPropertySet, which
// raises the same subtree walk the hook does).
//
// Real pass only -- no explicit Measure/Arrange -- so this pins the scheduling as
// well as the mirror. The root stays LEFT_TO_RIGHT throughout, which is what makes
// `mid` a mid-tree case rather than a whole-tree flip.
//
// The views are PARENTED before they are configured. That ordering is not
// load-bearing -- the hook is only made by a view that registers with a LIVE
// WINDOW, so an off-scene parentless invalidation connects nothing either way -- and
// it is kept as documentation of the shape under test: `mid` is a plain child, so
// the property path is the only mechanism that can observe the write.
int UtcDaliViewLayoutDirectionChangeOnMidTreeViewRelayoutsSubtreeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View root = View::New();
  View mid  = View::New();
  View leaf = View::New();

  root.Add(mid);
  mid.Add(leaf);
  window.Add(root);

  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);

  mid.SetRequestedWidth(150.0f);
  mid.SetRequestedHeight(100.0f);

  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetRequestedX(10.0f);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(mid.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetPositionX(), 10.0f, TEST_LOCATION);

  mid.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(leaf.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  application.SendNotification();
  application.SendNotification();

  // The leaf is mirrored inside `mid`: 150 - 10 - 50 = 90. `mid` itself is not
  // mirrored, because the root that arranges it is still LEFT_TO_RIGHT.
  DALI_TEST_EQUALS(leaf.GetPositionX(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(mid.GetPositionX(), 0.0f, TEST_LOCATION);

  END_TEST;
}

// The change is set on a non-View actor ABOVE the root View -- an intermediate
// Layer. No View is written to at all, so no property interception can see it;
// the only thing that can is the layout root's actor signal hook, which dali-core
// fires while resolving the inherit chain down through the layer.
//
// This is the case that keeps the hook necessary rather than merely convenient.
//
// Non-vacuity (verified by mutation): removing the lazy Connect from
// ViewDataImpl::RegisterWithLayoutController leaves the flip unobserved, no pass
// is scheduled, and the final check fails at the unmirrored x.
int UtcDaliViewLayoutDirectionChangeOnIntermediateLayerRelayoutsSubtreeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  Layer layer = Layer::New();
  window.Add(layer);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  layer.Add(root);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  root.Add(child);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);

  // Written on the LAYER. Layer inherits Actor, so this is the same property the
  // View API writes -- just on an actor dali-ui does not own.
  layer.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(root.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  application.SendNotification();
  application.SendNotification();

  // 200 - 20 - 50 = 130.
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  END_TEST;
}

// A MATCH_PARENT child inside a fixed-size DEFAULT parent must fill
// the parent's content area. OnArrange re-measures MATCH_PARENT children but
// must discard the Measure return (which is the child's GetMinimum == 0);
// adopting it would collapse the child to 0.
int UtcDaliViewDefaultModeMatchParentChildFillsParentP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);

  parent.Measure(200.0f, 200.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 200.0f));

  // Child fills the parent (no padding, no margin). Buggy code collapses to 0.
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 200.0f, TEST_LOCATION);
  END_TEST;
}

// A leaf WRAP view with padding and no visual must measure to its
// padding (pw, ph), not collapse to 0. GetBackgroundVisualNaturalSize returns
// ZERO with no visual, so the result must re-add padding.
int UtcDaliViewWrapNoVisualPaddingMeasuresPaddingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetPadding(Insets(5.0f, 15.0f, 25.0f, 35.0f)); // pw = 20, ph = 60

  MeasuredSize size = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 20.0f, TEST_LOCATION);  // buggy: 0
  DALI_TEST_EQUALS(size.GetHeight(), 60.0f, TEST_LOCATION); // buggy: 0
  END_TEST;
}

// Guard: with a background color visual (natural size 0) plus padding,
// the WRAP measure must still be exactly pw, ph (no double-count of padding).
int UtcDaliViewWrapBackgroundPaddingNoDoubleCountP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetBackgroundColor(UiColor(1.0f, 0.0f, 0.0f, 1.0f));
  view.SetPadding(Insets(5.0f, 15.0f, 25.0f, 35.0f)); // pw = 20, ph = 60

  MeasuredSize size = view.Measure(1000.0f, 1000.0f);
  // Color visual has natural size 0, so result == padding only, not 2x padding.
  DALI_TEST_EQUALS(size.GetWidth(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.GetHeight(), 60.0f, TEST_LOCATION);
  END_TEST;
}

// A leaf WRAP view whose background visual reports a non-zero natural size must
// measure to that natural size plus padding. The measure path must resolve the
// natural size from this view's own visual data; resolving it through the actor
// handle instead yields ZERO and collapses the result to the padding alone.
int UtcDaliViewWrapBackgroundNaturalSizePlusPaddingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetPadding(Insets(5.0f, 15.0f, 25.0f, 35.0f)); // pw = 20, ph = 60

  // An image visual with an explicit desired size reports that size as its
  // natural size, so the expected measure result is deterministic without
  // needing the image itself to be decodable.
  Property::Map backgroundMap;
  backgroundMap.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  backgroundMap.Insert(Ui::ImageVisualPropertyIndex::URL, Dali::String("background-image.png"));
  backgroundMap.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, 120);
  backgroundMap.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, 90);
  view.SetProperty(Ui::View::Property::BACKGROUND, backgroundMap);

  MeasuredSize size = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 140.0f, TEST_LOCATION);  // 120 + pw, buggy: 20
  DALI_TEST_EQUALS(size.GetHeight(), 150.0f, TEST_LOCATION); // 90 + ph, buggy: 60
  END_TEST;
}

// F2-docs anchor: min/max conflict resolution is "max wins" (floor to min, then
// ceil to max). Min 100 > Max 30 with a requested 50 yields 30.
int UtcDaliViewMinMaxConflictMaxWinsP(void)
{
  UiTestApplication application;
  View              view = View::New();
  view.SetRequestedWidth(50.0f);
  view.SetRequestedHeight(50.0f);
  view.SetMinimumWidth(100.0f);
  view.SetMaximumWidth(30.0f);

  MeasuredSize size = view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(size.GetWidth(), 30.0f, TEST_LOCATION);
  END_TEST;
}

// A child whose Measure() adds a sibling to the parent must not corrupt the
// parent's default OnMeasure iteration. OnMeasure snapshots mChildren into a
// std::vector before iterating (view-impl.cpp:969-970); without that snapshot
// the PushBack inside OnChildAdd reallocates the live Dali::Vector mid-loop.
int UtcDaliViewReentrantChildAddDuringMeasureP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  // Parent has no MeasureCallback / LayoutManager -> default OnMeasure runs the
  // snapshot loop.
  application.GetScene().Add(parent);
  gReentrantParent = parent;

  for(int i = 0; i < 4; ++i)
  {
    View c = View::New();
    c.SetRequestedWidth(10.0f);
    c.SetRequestedHeight(10.0f);
    // First child mutates the parent during its own Measure; the rest are plain.
    if(i == 0)
    {
      c.SetMeasureCallback(MeasureCallback::New(&ReentrantAddMeasure));
    }
    else
    {
      c.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
    }
    parent.Add(c);
  }

  gSiblingToAdd = View::New();
  gSiblingToAdd.SetRequestedWidth(10.0f);
  gSiblingToAdd.SetRequestedHeight(10.0f);
  gSiblingToAdd.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));

  DALI_TEST_EQUALS(parent.GetChildCount(), 4u, TEST_LOCATION);

  // Drives the default OnMeasure snapshot loop; the first child's Measure adds
  // a sibling mid-iteration. Must complete without crashing.
  parent.Measure(200.0f, 100.0f);

  DALI_TEST_EQUALS(parent.GetChildCount(), 5u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToAdd.Reset();
  END_TEST;
}

// A child whose Measure() removes a sibling from the parent must not corrupt
// the parent's default OnMeasure iteration. The Erase inside OnChildRemove
// shifts/shrinks the live Dali::Vector mid-loop; the snapshot makes the
// iteration target immune.
int UtcDaliViewReentrantChildRemoveDuringMeasureP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);
  gReentrantParent = parent;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetMeasureCallback(MeasureCallback::New(&ReentrantRemoveMeasure));
  parent.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  parent.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  parent.Add(third);

  DALI_TEST_EQUALS(parent.GetChildCount(), 3u, TEST_LOCATION);

  // First child's Measure removes the sibling mid-loop. Must complete cleanly.
  parent.Measure(200.0f, 100.0f);

  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}

// A child whose Arrange() removes a sibling must not corrupt the parent's default
// OnArrange iteration; the Erase inside OnChildRemove shifts the live Dali::Vector
// mid-loop, and the snapshot at view-impl.cpp:1153-1154 makes the loop immune.
int UtcDaliViewReentrantChildRemoveDuringArrangeP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);
  gReentrantParent = parent;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  first.SetArrangeCallback(ArrangeCallback::New(&ReentrantRemoveArrange));
  parent.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  gSiblingToRemove.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  parent.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  third.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  parent.Add(third);

  DALI_TEST_EQUALS(parent.GetChildCount(), 3u, TEST_LOCATION);

  parent.Measure(200.0f, 100.0f);
  // First child's Arrange removes the sibling mid-loop. Must complete cleanly.
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}

// A STANDALONE child whose Measure() removes a STANDALONE sibling must not corrupt
// MeasureStandaloneChildren's iteration. Standalone children are skipped by the
// default OnMeasure loop (view-impl.cpp:976-979) and iterated by the snapshot at
// view-impl.cpp:1211-1212; the Erase shifts the live vector mid-loop.
int UtcDaliViewReentrantStandaloneChildRemoveDuringMeasureP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);
  gReentrantParent = parent;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetLayoutMode(LayoutMode::STANDALONE);
  first.SetMeasureCallback(MeasureCallback::New(&ReentrantRemoveMeasure));
  parent.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetLayoutMode(LayoutMode::STANDALONE);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  parent.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetLayoutMode(LayoutMode::STANDALONE);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  parent.Add(third);

  DALI_TEST_EQUALS(parent.GetChildCount(), 3u, TEST_LOCATION);

  // Drives MeasureStandaloneChildren (called at view-impl.cpp:940); the first
  // standalone child's Measure removes a standalone sibling mid-loop.
  parent.Measure(200.0f, 100.0f);

  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}

// A STANDALONE child whose Arrange() removes a STANDALONE sibling must not corrupt
// ArrangeStandaloneChildren's iteration. Standalone children are arranged only by
// the snapshot loop at view-impl.cpp:1232-1233 (via ArrangeStandaloneChild ->
// childImpl.Arrange()); the Erase shifts the live vector mid-loop.
int UtcDaliViewReentrantStandaloneChildRemoveDuringArrangeP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);
  gReentrantParent = parent;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetLayoutMode(LayoutMode::STANDALONE);
  first.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  first.SetArrangeCallback(ArrangeCallback::New(&ReentrantRemoveArrange));
  parent.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetLayoutMode(LayoutMode::STANDALONE);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  gSiblingToRemove.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  parent.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetLayoutMode(LayoutMode::STANDALONE);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  third.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  parent.Add(third);

  DALI_TEST_EQUALS(parent.GetChildCount(), 3u, TEST_LOCATION);

  parent.Measure(200.0f, 100.0f);
  // Drives ArrangeStandaloneChildren (called at view-impl.cpp:1113); the first
  // standalone child's Arrange removes a standalone sibling mid-loop.
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}

int UtcDaliViewAccessibilityExportedPropertiesP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetAccessibilityName("Accessible view");
  view.SetAccessibilityDescription("Accessible description");
  view.SetAccessibilityRole(UiAccessibility::Role::CHECK_BOX);
  view.SetAccessibilityHighlightable(true);
  view.SetAccessibilityHidden(true);
  view.SetAutomationId("view-automation-id");
  view.SetAccessibilityValue("60%");
  view.SetAccessibilityScrollable(true);
  view.SetAccessibilityModal(true);
  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::SELECTED));
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::CHECKED));

  view.AddAccessibilityState(UiAccessibility::State::SELECTED);
  view.AddAccessibilityState(UiAccessibility::State::CHECKED);
  view.AddAccessibilityState(UiAccessibility::State::BUSY);
  view.AddAccessibilityState(UiAccessibility::State::EXPANDED);

  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::SELECTED));
  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::BUSY));
  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::EXPANDED));

  view.RemoveAccessibilityState(UiAccessibility::State::SELECTED);
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::SELECTED));
  view.AddAccessibilityState(UiAccessibility::State::SELECTED);
  view.RemoveAccessibilityState(UiAccessibility::State::BUSY);
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::BUSY));
  view.AddAccessibilityState(UiAccessibility::State::BUSY);

  view.AppendAccessibilityAttribute("resID", "test-resource");

  auto accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetName(), "Accessible view", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Accessible description", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), "60%", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::CHECK_BOX, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->IsHidden(), true, TEST_LOCATION);

  auto states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::ENABLED]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::SELECTED]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::CHECKED]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::BUSY]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::EXPANDED]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::HIGHLIGHTABLE]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::MODAL]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(dynamic_cast<ViewAccessible*>(accessible)->IsScrollable(), true, TEST_LOCATION);

  view.RemoveAccessibilityState(UiAccessibility::State::SELECTED);
  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::SELECTED]), false, TEST_LOCATION);

  view.AddAccessibilityState(UiAccessibility::State::SELECTED);
  states = accessible->GetStates();
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::SELECTED]), true, TEST_LOCATION);

  auto exportedAttributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(exportedAttributes["resID"], "test-resource", TEST_LOCATION);
  DALI_TEST_EQUALS(exportedAttributes["automationId"], "view-automation-id", TEST_LOCATION);
  DALI_TEST_CHECK(exportedAttributes.find("class") != exportedAttributes.end());

  view.ClearAccessibilityStates();
  states = accessible->GetStates();
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::ENABLED]), false, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::SELECTED]), false, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::CHECKED]), false, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::BUSY]), false, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(states[Dali::Integration::Accessibility::State::EXPANDED]), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewAccessibilityRoleConversionP(void)
{
  UiTestApplication application;

  View view       = View::New();
  auto accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);

  view.SetAccessibilityRole(UiAccessibility::Role::BUTTON);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PUSH_BUTTON, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TEXT);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::TEXT, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::LABEL);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::LABEL, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::SCROLL_PANE);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::SCROLL_PANE, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TAB);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PAGE_TAB, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::SWITCH);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::SWITCH, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::SCENE_3D);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::FILLER, TEST_LOCATION);
  DALI_TEST_EQUALS(ViewAccessible::IsScene3D(view), true, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::DIALOG);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::DIALOG, TEST_LOCATION);
  DALI_TEST_EQUALS(ViewAccessible::IsModal(view), true, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TABLE);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::TABLE, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TABLE_CELL);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::TABLE_CELL, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TABLE_COLUMN_HEADER);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::TABLE_COLUMN_HEADER, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TABLE_ROW_HEADER);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::TABLE_ROW_HEADER, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::EMBEDDED);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::EMBEDDED, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::NONE);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::UNKNOWN, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(accessible->GetStates()[Dali::Integration::Accessibility::State::HIGHLIGHTABLE]), false, TEST_LOCATION);

  view.SetAccessibilityRole(static_cast<UiAccessibility::Role>(-1));
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::UNKNOWN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewAccessibilityCallbacksP(void)
{
  UiTestApplication application;

  View parent = View::New();
  View child  = View::New();
  parent.SetAccessibilityScrollable(true);
  parent.Add(child);

  auto accessible = dynamic_cast<ViewAccessible*>(Dali::Accessibility::Accessible::Get(parent));
  DALI_TEST_CHECK(accessible);

  Dali::Devel::Accessibility::GestureInfo gestureInfo{
    Dali::Devel::Accessibility::Gesture::ONE_FINGER_SINGLE_TAP,
    0,
    1,
    0,
    1,
    Dali::Devel::Accessibility::GestureState::ENDED,
    1u};
  DALI_TEST_EQUALS(accessible->DoGesture(gestureInfo), false, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetRelationSet().empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->ScrollToChild(child), false, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GrabHighlight(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->ClearHighlight(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(Extension::View::GrabAccessibilityHighlight(parent), false, TEST_LOCATION);
  DALI_TEST_EQUALS(Extension::View::ClearAccessibilityHighlight(parent), false, TEST_LOCATION);

  parent.SetAccessibilityRole(UiAccessibility::Role::CHECK_BOX);
  accessible->OnStatePropertySet(AccessibilityStateMask(UiAccessibility::State::CHECKED));

  parent.SetAccessibilityRole(UiAccessibility::Role::BUTTON);
  accessible->OnStatePropertySet(AccessibilityStateMask(UiAccessibility::State::SELECTED));

  END_TEST;
}

int UtcDaliViewExtensionGeometrySettersP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliViewExtensionGeometrySettersP - extension-api free functions drive the Actor render geometry of a View handle");

  View view = View::New();

  // The raw Actor geometry setters are deleted on the public View handle;
  // custom-view authors use the Dali::Ui::Extension free functions instead.
  Extension::View::SetPositionX(view, 12.0f);
  Extension::View::SetPositionY(view, 34.0f);
  Extension::View::SetPositionZ(view, 45.0f);
  Extension::View::SetSizeWidth(view, 56.0f);
  Extension::View::SetSizeHeight(view, 78.0f);
  Extension::View::SetSizeDepth(view, 90.0f);

  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_X), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Y), 34.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Z), 45.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 78.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_DEPTH), 90.0f, TEST_LOCATION);

  END_TEST;
}

// A MeasureCallback that calls Measure() on its OWN view must not re-run the
// producer. Measure() is public and LayoutController::ProcessLayouts() is
// nestable, so this is reachable from application code and must be safe in
// RELEASE, not just guarded by a debug assertion. Contract:
//   - the re-entrant call returns immediately with the last COMPLETED result
//     ({0,0} while no pass has completed), so there is no unbounded recursion;
//   - the outer pass is poisoned, so it cannot serve a cache hit afterwards;
//   - poison is per-pass: once a pass runs without re-entrancy, the next
//     same-constraint Measure hits the cache again.
int UtcDaliViewReentrantSameViewMeasureReturnsLastCompletedP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetMeasureCallback(MeasureCallback::New(&SelfReentrantMeasure));

  gSelfReentrantView         = view;
  gSelfReentrantDepth        = 0;
  gSelfMeasureProducerCount  = 0;
  gSelfReentrantDidReenter   = false;
  gSelfReentrantMeasureInner = MeasuredSize(-1.0f, -1.0f);

  // Pass 1: nothing has completed yet, so the re-entrant call must hand back
  // the "never measured" result rather than recursing into the producer.
  MeasuredSize outer1 = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gSelfReentrantDidReenter, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfMeasureProducerCount, 1, TEST_LOCATION); // producer ran once, not twice
  DALI_TEST_EQUALS(gSelfReentrantMeasureInner.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantMeasureInner.GetHeight(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer1.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer1.GetHeight(), 30.0f, TEST_LOCATION);

  // Pass 2: same constraint. Pass 1 was poisoned by the re-entrancy, so this
  // must MISS (producer runs again) and the inner call now sees pass 1's
  // completed result.
  MeasuredSize outer2 = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gSelfMeasureProducerCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantMeasureInner.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantMeasureInner.GetHeight(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer2.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer2.GetHeight(), 30.0f, TEST_LOCATION);

  // Stop re-entering: pass 3 still misses (pass 2 was poisoned) but completes
  // cleanly, which clears the poison for good.
  gSelfReentrantView.Reset();
  view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gSelfMeasureProducerCount, 3, TEST_LOCATION);

  // Pass 4: clean cache entry, same constraint -> hit, producer not re-run.
  MeasuredSize outer4 = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gSelfMeasureProducerCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(outer4.GetWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer4.GetHeight(), 30.0f, TEST_LOCATION);

  gSelfReentrantView.Reset();
  END_TEST;
}

// An ArrangeCallback that calls Arrange() on its OWN view. This used to be a
// DEBUG-only assertion (undefined behaviour in release); it is now absorbed in
// release too. Contract:
//   - the re-entrant call returns its own input bounds while no arrange pass
//     has completed, and the last COMPLETED bounds afterwards;
//   - the producer is not re-run, so the outer pass's self geometry is the
//     rect the outer pass resolved, not the re-entrant one.
int UtcDaliViewReentrantSameViewArrangeDoesNotAssertOrCorruptP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetArrangeCallback(ArrangeCallback::New(&SelfReentrantArrange));

  gSelfReentrantView         = view;
  gSelfReentrantDepth        = 0;
  gSelfArrangeProducerCount  = 0;
  gSelfReentrantDidReenter   = false;
  gSelfReentrantArrangeInner = LayoutRect(-1.0f, -1.0f, -1.0f, -1.0f);

  const LayoutRect outerBounds(0.0f, 0.0f, 200.0f, 100.0f);

  // Pass 1: no completed arrange yet -> the re-entrant call falls back to the
  // bounds it was handed, and does not re-run the producer.
  LayoutRect outer1 = view.Arrange(outerBounds);
  DALI_TEST_EQUALS(gSelfReentrantDidReenter, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfArrangeProducerCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.x, SELF_REENTRANT_INNER_BOUNDS.x, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.y, SELF_REENTRANT_INNER_BOUNDS.y, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.width, SELF_REENTRANT_INNER_BOUNDS.width, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.height, SELF_REENTRANT_INNER_BOUNDS.height, TEST_LOCATION);

  // The outer pass still publishes its own resolved rect; the re-entrant call
  // must not have overwritten the self geometry.
  DALI_TEST_EQUALS(outer1.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer1.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer1.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer1.height, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  // Pass 2: a completed arrange now exists, so the re-entrant call returns it.
  LayoutRect outer2 = view.Arrange(outerBounds);
  DALI_TEST_EQUALS(gSelfArrangeProducerCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.x, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.y, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gSelfReentrantArrangeInner.height, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer2.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(outer2.height, 100.0f, TEST_LOCATION);

  gSelfReentrantView.Reset();
  END_TEST;
}

// InvalidateMeasure() must NOT short-circuit on "this view is already dirty".
// A view's dirty flags are consumed only by its OWN Measure()/Arrange(), so a
// custom parent that never measures/arranges its children leaves those children
// dirty indefinitely. With an "already dirty -> return" guard the child's next
// invalidation never reaches the layout root, the root is never re-registered,
// and the change is lost for good. Invalidation must therefore always propagate
// and register; the controller's pending set coalesces the duplicates.
int UtcDaliViewInvalidateMeasureNotSwallowedWhenAlreadyDirtyP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gIgnoringMeasureProducerCount = 0;
  gIgnoringArrangeProducerCount = 0;

  // Parent is the layout root; both producers ignore children entirely.
  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetMeasureCallback(MeasureCallback::New(&IgnoringChildrenMeasure));
  parent.SetArrangeCallback(ArrangeCallback::New(&IgnoringChildrenArrange));

  // DEFAULT layout mode: the child contributes to the parent, so its
  // invalidation must propagate to the parent (it is not a layout boundary).
  View child = View::New();
  child.SetRequestedWidth(10.0f);
  child.SetRequestedHeight(10.0f);
  parent.Add(child);
  window.Add(parent);

  // Drain the initial layout. The parent runs one measure pass; the child is
  // never measured (the producer ignores it), so the child's measure-dirty
  // raised by Add() is still standing after this pass.
  application.SendNotification();
  const int measureAfterFirstPass = gIgnoringMeasureProducerCount;
  DALI_TEST_CHECK(measureAfterFirstPass > 0);

  // Nothing pending: an idle cycle must not run another pass. This pins the
  // baseline so the assertion below can only be satisfied by a NEW pass.
  application.SendNotification();
  DALI_TEST_EQUALS(gIgnoringMeasureProducerCount, measureAfterFirstPass, TEST_LOCATION);

  // Change the still-dirty child. This must reach the layout root and
  // re-register it, even though the child was already dirty.
  child.SetRequestedWidth(80.0f);
  DALI_TEST_EQUALS(child.GetRequestedWidth(), 80.0f, TEST_LOCATION);

  application.SendNotification();
  DALI_TEST_CHECK(gIgnoringMeasureProducerCount > measureAfterFirstPass);

  // A second change on the (still unconsumed) child is likewise not swallowed.
  const int measureAfterSecondPass = gIgnoringMeasureProducerCount;
  child.SetRequestedWidth(120.0f);
  application.SendNotification();
  DALI_TEST_CHECK(gIgnoringMeasureProducerCount > measureAfterSecondPass);
  DALI_TEST_EQUALS(child.GetRequestedWidth(), 120.0f, TEST_LOCATION);

  END_TEST;
}

// Arrange-axis mirror of the case above: InvalidateArrange() must not
// short-circuit on mArrangeDirty either. mArrangeDirty is cleared only by the
// view's own Arrange(), so under a parent that never arranges its children the
// flag is pinned true and every later InvalidateArrange() would be dropped.
int UtcDaliViewInvalidateArrangeNotSwallowedWhenAlreadyDirtyP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gIgnoringMeasureProducerCount = 0;
  gIgnoringArrangeProducerCount = 0;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetMeasureCallback(MeasureCallback::New(&IgnoringChildrenMeasure));
  parent.SetArrangeCallback(ArrangeCallback::New(&IgnoringChildrenArrange));

  View child = View::New();
  child.SetRequestedWidth(10.0f);
  child.SetRequestedHeight(10.0f);
  parent.Add(child);
  window.Add(parent);

  // Initial drain. The parent's arrange producer runs; the child is never
  // arranged, so the child's arrange-dirty raised by Add() is still standing.
  application.SendNotification();
  const int arrangeAfterFirstPass = gIgnoringArrangeProducerCount;
  DALI_TEST_CHECK(arrangeAfterFirstPass > 0);

  application.SendNotification();
  DALI_TEST_EQUALS(gIgnoringArrangeProducerCount, arrangeAfterFirstPass, TEST_LOCATION);

  // Invalidate the already-arrange-dirty child: must reach the root.
  const int measureBefore = gIgnoringMeasureProducerCount;
  child.InvalidateArrange();

  application.SendNotification();
  DALI_TEST_CHECK(gIgnoringArrangeProducerCount > arrangeAfterFirstPass);

  // Arrange-only invalidation: the parent's measure cache is untouched, so the
  // measure producer must NOT be re-run by this pass.
  DALI_TEST_EQUALS(gIgnoringMeasureProducerCount, measureBefore, TEST_LOCATION);

  // Repeat: still not swallowed.
  const int arrangeAfterSecondPass = gIgnoringArrangeProducerCount;
  child.InvalidateArrange();
  application.SendNotification();
  DALI_TEST_CHECK(gIgnoringArrangeProducerCount > arrangeAfterSecondPass);

  END_TEST;
}

// A Measure producer that re-invalidates its OWN view mid-pass must not leave the
// pre-invalidation result pinned in the cache (plan33 3.1, "loss A").
//
// The mid-pass View::InvalidateMeasure() is a public-API call from inside the
// layout processing window. It is warned, fully propagated and left pending,
// while the controller suppresses only a self-generated idle wake. Dirty is
// consumed at pass ENTRY, so that raise is still standing when the pass reaches
// its publish gate; the publish is declined, and the next Measure() with the SAME
// constraint misses and recomputes the post-invalidation value.
//
// This test drives view.Measure() directly rather than through an on-scene
// controller pass, so it observes cache freshness only. See
// UtcDaliViewInvalidateMeasureDuringMeasurePassParkedAndIdleN for
// pending-versus-wake behaviour on scene.
int UtcDaliViewMidPassSelfInvalidationBlocksMeasurePublishAndRecomputesP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetMeasureCallback(MeasureCallback::New(&MidPassInvalidatingMeasure));

  gMidPassView                 = view;
  gMidPassMeasureProducerCount = 0;

  // Pass 1: the producer invalidates its own view while the pass is running,
  // then returns the "stale" size.
  MeasuredSize first = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gMidPassMeasureProducerCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetWidth(), MID_PASS_FIRST_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetHeight(), MID_PASS_FIRST_HEIGHT, TEST_LOCATION);

  // Pass 2, SAME constraint: must MISS (the mid-pass invalidation blocked the
  // publish) and hand back the recomputed value.
  MeasuredSize second = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gMidPassMeasureProducerCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetWidth(), MID_PASS_LATER_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetHeight(), MID_PASS_LATER_HEIGHT, TEST_LOCATION);

  // Pass 3: pass 2 completed cleanly, so this is a genuine cache hit. This
  // pins that the miss above came from the invalidation, not from a cache
  // that the mid-pass invalidation disabled for good.
  MeasuredSize third = view.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(gMidPassMeasureProducerCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetWidth(), MID_PASS_LATER_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(third.GetHeight(), MID_PASS_LATER_HEIGHT, TEST_LOCATION);

  gMidPassView.Reset();
  END_TEST;
}

// Arrange-axis twin, on scene: an ArrangeCallback that calls the public
// View::InvalidateArrange() on its own view mid-pass.
//
// The call is inside the layout processing window, so it is warned, propagated
// and parked. The pass declines its cache publish and the root remains pending,
// but no idle ProcessEvents wake is requested. An independent ProcessEvents call
// services that retained pass and lets this one-shot producer settle.
int UtcDaliViewMidPassSelfInvalidationBlocksArrangePublishP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetArrangeCallback(ArrangeCallback::New(&MidPassInvalidatingArrange));

  gMidPassView                 = view;
  gMidPassArrangeProducerCount = 0;

  window.Add(view);

  // The mount armed one normal event-time wake. Its arrange pass invalidates the
  // view and parks a follow-up without arming another wake.
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // An unrelated ProcessEvents trigger services the parked pass. The callback
  // disarmed after its first run, so this pass completes without another wake.
  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 2, TEST_LOCATION);

  gMidPassView.Reset();
  END_TEST;
}

// A measure pass poisoned by same-view re-entrancy declines to publish, but the
// re-entrancy never went through InvalidateMeasure(), so nothing propagated to a
// layout root and nothing registered a follow-up. Without an explicit follow-up
// the view would sit with an invalid cache and no pass scheduled to refill it.
// The pass must therefore retain exactly ONE follow-up layout without waking
// itself, and an independently triggered follow-up that completes cleanly must
// not register another (no spin).
int UtcDaliViewPoisonedMeasurePassSchedulesOneFollowUpLayoutP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&PoisonOnceMeasure));

  gPoisonOnceView                 = view;
  gPoisonOnceMeasureProducerCount = 0;
  gPoisonOnceDepth                = 0;

  window.Add(view);

  // Pass 1 is poisoned by the re-entrant Measure() and publishes nothing; the
  // producer ran exactly once (the re-entrant call is absorbed). Its recovery
  // work is parked, not converted into another idle wake.
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceMeasureProducerCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // An unrelated ProcessEvents trigger services the retained recovery pass.
  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceMeasureProducerCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceMeasureProducerCount, 2, TEST_LOCATION);

  gPoisonOnceView.Reset();
  END_TEST;
}

// Arrange-axis twin of the case above.
int UtcDaliViewPoisonedArrangePassSchedulesOneFollowUpLayoutP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetArrangeCallback(ArrangeCallback::New(&PoisonOnceArrange));

  gPoisonOnceView                 = view;
  gPoisonOnceArrangeProducerCount = 0;
  gPoisonOnceDepth                = 0;

  window.Add(view);

  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceArrangeProducerCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceArrangeProducerCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gPoisonOnceArrangeProducerCount, 2, TEST_LOCATION);

  gPoisonOnceView.Reset();
  END_TEST;
}

// --- Phase 2a: ancestor layout-cache invalidation on a full Measure miss -----

// THE LIVE BUG. root -> A -> B, all measured and cached. An external
// View::Measure() on B publishes B's measured slot unconditionally, so B's slot
// now describes the external constraint. Without ancestor invalidation the next
// pass goes: root misses (dirty) -> measures A at an UNCHANGED constraint -> A's
// measure cache HITS -> A never re-measures B -> A's arrange reads B's
// overwritten slot -> B is arranged at the external size and stays there.
// A full measure miss must therefore drop the ancestor measure caches, so A
// misses, re-measures B against the real constraint, and arranges the result.
int UtcDaliViewDirectChildMeasureInvalidatesAncestorMeasureCacheP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View a = View::New();
  a.SetRequestedWidth(200.0f);
  a.SetRequestedHeight(100.0f);

  // B wraps: its measured size depends on the constraint it is handed, so a
  // measurement taken against the wrong constraint is visible in its geometry.
  View b = View::New();
  b.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  a.Add(b);
  root.Add(a);
  window.Add(root);

  // Baseline: B measured under A at the real constraint (200 x 100), so the
  // clamp producer returns its natural size and B is arranged at it.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);
  const int producerAfterLayout = gClampMeasureProducerCount;
  DALI_TEST_CHECK(producerAfterLayout > 0);

  // Out-of-band measurement at a much smaller constraint. This is a legitimate
  // public API call; it overwrites B's stored slot with 30 x 20.
  MeasuredSize external = b.Measure(30.0f, 20.0f);
  DALI_TEST_EQUALS(external.GetWidth(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(external.GetHeight(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gClampMeasureProducerCount, producerAfterLayout + 1, TEST_LOCATION);

  // Next pass, driven from the root. A's constraint has not changed, so the
  // only thing that can make A re-measure B is the ancestor invalidation.
  root.InvalidateMeasure();
  application.SendNotification();

  // A re-measured B (producer ran again) ...
  DALI_TEST_EQUALS(gClampMeasureProducerCount, producerAfterLayout + 2, TEST_LOCATION);
  // ... and B ends the pass at its natural size, not at the external one.
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);

  // And it settles: no further pass, no re-measure, no spin.
  const int producerAfterFix = gClampMeasureProducerCount;
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(gClampMeasureProducerCount, producerAfterFix, TEST_LOCATION);

  gCountingMeasureChild.Reset();
  END_TEST;
}

// The counterpart: measurements a view issues as part of the normal recursion
// must NOT invalidate its ancestors, or every frame would re-measure the whole
// chain. Two stop conditions are exercised at once by A -> B:
//   (a) B is measured from inside A's MEASURE pass  -> A is measure-in-progress;
//   (c) B is MATCH_PARENT, so A's default ARRANGE re-measures it at the resolved
//       width -> A is arrange-in-progress. This one is the dangerous one: A's
//       measure pass has already published by then, so clearing A's cache here
//       would stick and cost a full re-measure of A on every frame.
// Pinned by producer counts across two identical passes: A must hit its cache
// on the second pass, and B must hit the arrange-phase entry it published on
// the first.
int UtcDaliViewRecursiveChildMeasureDoesNotInvalidateAncestorCacheP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount    = 0;
  gCountingMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View a = View::New();
  a.SetMeasureCallback(MeasureCallback::New(&CountingParentMeasure));

  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  gCountingMeasureChild = b;

  a.Add(b);
  root.Add(a);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  const int parentCountAfterLayout = gCountingMeasureProducerCount;
  const int childCountAfterLayout  = gClampMeasureProducerCount;
  DALI_TEST_CHECK(parentCountAfterLayout > 0);
  // B was measured twice: once by A's measure producer, once by A's arrange
  // resolving MATCH_PARENT against a different constraint.
  DALI_TEST_CHECK(childCountAfterLayout >= 2);

  // Identical second pass. Nothing about A's or B's inputs changed, so both
  // must serve cache hits: no producer may run again.
  root.InvalidateMeasure();
  application.SendNotification();

  DALI_TEST_EQUALS(gCountingMeasureProducerCount, parentCountAfterLayout, TEST_LOCATION);
  DALI_TEST_EQUALS(gClampMeasureProducerCount, childCountAfterLayout, TEST_LOCATION);

  // A third identical pass, to catch a slow leak rather than a one-off.
  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gCountingMeasureProducerCount, parentCountAfterLayout, TEST_LOCATION);
  DALI_TEST_EQUALS(gClampMeasureProducerCount, childCountAfterLayout, TEST_LOCATION);

  gCountingMeasureChild.Reset();
  END_TEST;
}

// The walk follows the MEASURED view's ancestry, not the call stack. A producer
// in one tree measures a view in a completely different tree: the measured
// view's ancestors are invalidated all the way to its own root, while the
// measuring view's own chain is untouched (it owns nothing of the other tree).
int UtcDaliViewUnrelatedTreeMeasureFromCallbackWalksToRootP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount   = 0;
  gUnrelatedOwnerProducerCount = 0;
  gUnrelatedMeasureTarget.Reset();

  // Tree 1: the target of the out-of-band measurement.
  View targetRoot = View::New();
  targetRoot.SetRequestedWidth(200.0f);
  targetRoot.SetRequestedHeight(100.0f);

  View targetMid = View::New();
  targetMid.SetRequestedWidth(200.0f);
  targetMid.SetRequestedHeight(100.0f);

  View target = View::New();
  target.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  targetMid.Add(target);
  targetRoot.Add(targetMid);
  window.Add(targetRoot);

  // Tree 2: the owner whose producer reaches into tree 1.
  View ownerRoot = View::New();
  ownerRoot.SetRequestedWidth(100.0f);
  ownerRoot.SetRequestedHeight(50.0f);

  View owner = View::New();
  owner.SetMeasureCallback(MeasureCallback::New(&MeasureUnrelatedTreeMeasure));

  ownerRoot.Add(owner);
  window.Add(ownerRoot);

  // Settle both trees with the cross-tree measurement disarmed.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(target.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  const int targetCountAfterLayout = gClampMeasureProducerCount;
  const int ownerCountAfterLayout  = gUnrelatedOwnerProducerCount;
  DALI_TEST_CHECK(ownerCountAfterLayout > 0);

  // Arm it and run a pass in the OWNER tree only. The owner itself is
  // invalidated (invalidating only ownerRoot would leave the owner's own cache
  // valid, so its producer would not run at all).
  gUnrelatedMeasureTarget = target;
  owner.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gUnrelatedOwnerProducerCount, ownerCountAfterLayout + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gClampMeasureProducerCount, targetCountAfterLayout + 1, TEST_LOCATION);

  // Disarm, then drive a pass in the TARGET tree. targetMid's constraint is
  // unchanged, so it can only re-measure the target if the cross-tree measure
  // invalidated it (case b: the walk climbed the target's own ancestry).
  gUnrelatedMeasureTarget.Reset();
  const int targetCountBeforeFix = gClampMeasureProducerCount;
  targetRoot.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gClampMeasureProducerCount, targetCountBeforeFix + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);

  // The owner's own chain was never touched by that walk: an identical pass in
  // the owner tree still hits its cache.
  const int ownerCountBefore = gUnrelatedOwnerProducerCount;
  ownerRoot.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gUnrelatedOwnerProducerCount, ownerCountBefore, TEST_LOCATION);

  gUnrelatedMeasureTarget.Reset();
  END_TEST;
}

// The walk stops at a standalone boundary: an external Measure() on a standalone
// view invalidates NO ancestor cache, so the parent still serves its measure
// cache hit and its producer does not re-run. That boundary stop is what this
// test pins (the producer counts below), and it prevents every focus ring,
// scroll bar and other standalone decoration from dragging its whole ancestor
// chain into a re-measure.
//
// The child itself is nevertheless CORRECTED, without any ancestor cache being
// touched: the measure publish leaves the slot marked unconsumed, and
// ArrangeStandaloneChild re-measures the child against the parent's own extent
// before placing it. The correction sits on the ARRANGE side, which is why it is
// reached on this very pass even though the parent served a measure cache hit and
// MeasureStandaloneChildren never ran. It costs exactly ONE producer run, and the
// second pass below pins that it is not paid again (the slot is consumed, so a
// steady state cannot spin).
int UtcDaliViewStandaloneDirectMeasureStopsAtBoundaryP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount    = 0;
  gCountingMeasureProducerCount = 0;
  gCountingMeasureChild.Reset(); // the counting parent measures no child here

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View parent = View::New();
  parent.SetMeasureCallback(MeasureCallback::New(&CountingParentMeasure));

  // Standalone child: measured by MeasureStandaloneChildren, not by the
  // parent's producer.
  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  parent.Add(standalone);
  root.Add(parent);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  const int parentCountAfterLayout = gCountingMeasureProducerCount;
  DALI_TEST_CHECK(parentCountAfterLayout > 0);
  DALI_TEST_CHECK(gClampMeasureProducerCount > 0);

  // External measure on the boundary view.
  standalone.Measure(30.0f, 20.0f);
  const int standaloneCountAfterExternal = gClampMeasureProducerCount;

  // The parent's cache must have survived: an identical pass hits it, so its
  // producer does not re-run. THIS is the boundary stop, and it is unchanged.
  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gCountingMeasureProducerCount, parentCountAfterLayout, TEST_LOCATION);

  // The standalone child, however, was corrected on the arrange side: exactly one
  // re-measure, against the parent's extent. It is a genuine miss because the
  // child's measure cache holds the out-of-band 30x20 constraint.
  DALI_TEST_EQUALS(gClampMeasureProducerCount, standaloneCountAfterExternal + 1, TEST_LOCATION);

  // ... so it is placed at the size the parent's extent produces, not at the
  // out-of-band size the external Measure() left in the slot.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);

  // Anti-spin: the correction CONSUMED the slot, so an identical second pass pays
  // for nothing -- neither producer runs again, and the geometry stays put. (A
  // correction that re-armed itself would show up here as a per-pass re-measure.)
  const int standaloneCountAfterCorrection = gClampMeasureProducerCount;
  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gCountingMeasureProducerCount, parentCountAfterLayout, TEST_LOCATION);
  DALI_TEST_EQUALS(gClampMeasureProducerCount, standaloneCountAfterCorrection, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);

  END_TEST;
}

// The first-party standalone shape -- MATCH_PARENT on BOTH axes, which is what the
// ScrollBar and the focus indicator use -- must be a strict no-op under the slot
// correction. Its measured value is discarded on both axes, and the placement
// re-measure runs at exactly the extent the correction would have used, so the
// correction is skipped: the pass costs ONE producer run, not two, and an
// out-of-band Measure() cannot move the geometry at all.
int UtcDaliViewStandaloneMatchParentSlotUnaffectedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(MATCH_PARENT);
  standalone.SetRequestedHeight(MATCH_PARENT);
  standalone.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  root.Add(standalone);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  // MATCH_PARENT fills the parent's extent whatever the producer reported.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  // An out-of-band measure marks the slot unconsumed, which for this shape must
  // change nothing.
  standalone.Measure(30.0f, 20.0f);
  const int countAfterExternal = gClampMeasureProducerCount;

  root.InvalidateArrange();
  application.SendNotification();

  // Exactly one producer run in the pass: the MATCH_PARENT placement re-measure.
  DALI_TEST_EQUALS(gClampMeasureProducerCount, countAfterExternal + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  END_TEST;
}

// The option discriminator. The correction lives on the ARRANGE side, so it is
// reached on a pass that only re-arranges: InvalidateArrange leaves every measure
// cache valid, so nothing above re-measures anything and any fix hung off the
// measure path (an ancestor cache clear, or a re-measure on the measure cache-hit
// path) would never run here. The standalone child is corrected anyway.
int UtcDaliViewStandaloneSlotCorrectedOnArrangeOnlyPassP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount    = 0;
  gCountingMeasureProducerCount = 0;
  gCountingMeasureChild.Reset(); // the counting parent measures no child here

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View parent = View::New();
  parent.SetMeasureCallback(MeasureCallback::New(&CountingParentMeasure));

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  parent.Add(standalone);
  root.Add(parent);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  const int parentCountAfterLayout = gCountingMeasureProducerCount;
  DALI_TEST_CHECK(parentCountAfterLayout > 0);
  DALI_TEST_CHECK(gClampMeasureProducerCount > 0);

  standalone.Measure(30.0f, 20.0f);
  const int standaloneCountAfterExternal = gClampMeasureProducerCount;

  // Arrange only: no measure cache anywhere is invalidated.
  root.InvalidateArrange();
  application.SendNotification();

  // Nothing was re-measured from the measure side ...
  DALI_TEST_EQUALS(gCountingMeasureProducerCount, parentCountAfterLayout, TEST_LOCATION);
  // ... yet the standalone child was corrected exactly once, and placed at the
  // size its parent's extent yields rather than the out-of-band 30x20.
  DALI_TEST_EQUALS(gClampMeasureProducerCount, standaloneCountAfterExternal + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), CLAMP_MEASURE_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), CLAMP_MEASURE_NATURAL_HEIGHT, TEST_LOCATION);

  END_TEST;
}

// The steady state must keep using the parent's MEASURE constraint, not its
// (generally smaller) arrange extent: MeasureStandaloneChildren is the parent's
// consumption of the slot, so the arrange-time correction must not fire behind it.
// The two constraints are told apart by a producer whose natural size falls
// between them -- 150x90 fits the 200x100 measure constraint but not the 120x60
// arrange extent -- so a correction that ran unconditionally would visibly shrink
// the child.
int UtcDaliViewStandaloneSteadyStateUsesMeasureConstraintP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gWideClampMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  // WRAP_CONTENT, but its producer reports 120x60: the constraint it forwards to
  // its standalone children (200x100) and the extent they are arranged in
  // (120x60) are therefore different numbers.
  View parent = View::New();
  parent.SetMeasureCallback(MeasureCallback::New(&NarrowParentMeasure));

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetMeasureCallback(MeasureCallback::New(&WideClampToConstraintMeasure));

  parent.Add(standalone);
  root.Add(parent);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  DALI_TEST_CHECK(gWideClampMeasureProducerCount > 0);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_WIDTH), NARROW_PARENT_MEASURED_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT), NARROW_PARENT_MEASURED_HEIGHT, TEST_LOCATION);

  // Drive a pass through the PARENT (a standalone view self-registers as its own
  // layout root when it is invalidated, and LayoutController::ProcessLayoutRoot
  // then measures it against the parent's SIZE -- the initial mount above goes
  // through that path, which is not what this test is about). Invalidating the
  // parent instead re-runs MeasureStandaloneChildren, which is the consumption
  // point under test.
  const int countBeforePass = gWideClampMeasureProducerCount;
  parent.InvalidateMeasure();
  application.SendNotification();

  // MeasureStandaloneChildren re-measured the child at the parent's own 200x100
  // measure constraint and marked the slot consumed, so the arrange placed it from
  // that slot: 150x90, overflowing the parent's 120x60 extent. A correction that
  // fired here would re-measure at 120x60 and shrink it.
  DALI_TEST_EQUALS(gWideClampMeasureProducerCount, countBeforePass + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), WIDE_CLAMP_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), WIDE_CLAMP_NATURAL_HEIGHT, TEST_LOCATION);

  // Steady state, and free: an identical pass re-issues the SAME 200x100
  // constraint, so the child's measure cache hits and the arrange again finds the
  // slot consumed. Nothing runs, nothing moves.
  const int countAfterPass = gWideClampMeasureProducerCount;
  parent.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gWideClampMeasureProducerCount, countAfterPass, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), WIDE_CLAMP_NATURAL_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), WIDE_CLAMP_NATURAL_HEIGHT, TEST_LOCATION);

  END_TEST;
}

// Anti-spin pin. The ancestor invalidation is CACHE-ONLY: it clears cache-valid
// bits and nothing else -- no dirty bit, no LayoutController registration. So
// repeated out-of-band measurements must schedule exactly ZERO layout passes,
// however many of them there are. (If it ever started raising dirty instead,
// every external Measure() would cost a frame of layout.)
// The root's arrange producer is the pass detector: the arrange cache HIT is
// restricted to CHILDLESS views and the root has children, so its producer runs
// on every pass the controller schedules.
int UtcDaliViewAncestorCacheOnlyInvalidationSchedulesNoLayoutP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount    = 0;
  gIgnoringArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  root.SetArrangeCallback(ArrangeCallback::New(&IgnoringChildrenArrange));

  View a = View::New();
  a.SetRequestedWidth(200.0f);
  a.SetRequestedHeight(100.0f);

  View b = View::New();
  b.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  a.Add(b);
  root.Add(a);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  const int passesAfterLayout = gIgnoringArrangeProducerCount;
  DALI_TEST_CHECK(passesAfterLayout > 0);

  // Five external measurements, each at a different constraint so each one is
  // a genuine full miss that runs the walk.
  for(int i = 0; i < 5; ++i)
  {
    b.Measure(30.0f + i, 20.0f + i);
    application.SendNotification();
    DALI_TEST_EQUALS(gIgnoringArrangeProducerCount, passesAfterLayout, TEST_LOCATION);
  }

  // Idle frames afterwards are quiet too.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(gIgnoringArrangeProducerCount, passesAfterLayout, TEST_LOCATION);
  // The walk really did run five times (five misses on B's producer).
  DALI_TEST_EQUALS(gClampMeasureProducerCount, 6, TEST_LOCATION);

  // Sanity: the detector is not simply stuck -- a real invalidation still
  // schedules a pass.
  root.InvalidateArrange();
  application.SendNotification();
  DALI_TEST_EQUALS(gIgnoringArrangeProducerCount, passesAfterLayout + 1, TEST_LOCATION);

  END_TEST;
}

// --- Phase 2b: the ancestor walk stops at the layout-dependency owner --------

// T6. An arrange-time re-measure is OWNED by the arranging view: the producer
// pushed an ArrangeOwnedMeasureScope around it, so the ancestor walk stops at
// that view and everything above it keeps its measure cache.
//
// Tree: root(fixed 200x100) -> OBS(pass-through counting producer, fixed) ->
// M(the arranging container under test, WRAP_CONTENT) -> C(MATCH_PARENT width,
// clamp producer).
//
// M is deliberately WRAP_CONTENT: its arrange bounds are then its own measured
// size (80x60), which differs from the 200x100 constraint its measure pass handed
// C, so C's arrange-phase re-measure is a genuine full MISS and really does run
// the walk. Sizing M to 200x100 instead makes the two constraints equal, the
// re-measure a cache hit, and the whole test vacuous (verified for Grid and Flex).
// The non-vacuity guard below pins that the re-measure ran.
//
// From C the walk would reach M (arrange-in-progress) and then OBS; the owner
// scope stops it at M, so OBS's producer must never run again on an otherwise
// identical pass.
//
// Without the scope the walk clears M's and OBS's caches, and because C's two
// constraints alternate for ever, the clearing repeats on every pass: OBS's count
// then grows once per pass. That divergence is what makes this test non-vacuous.
void CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(UiTestApplication& application, View container)
{
  Window window = application.GetWindow();

  gClampMeasureProducerCount       = 0;
  gPassThroughMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View observer = View::New();
  observer.SetRequestedWidth(200.0f);
  observer.SetRequestedHeight(100.0f);
  observer.SetMeasureCallback(MeasureCallback::New(&CountingPassThroughMeasure));

  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  // ScrollView owns its content through SetContent (it also keeps a scroll bar
  // child); every other container takes an ordinary child.
  ScrollView scrollView = ScrollView::DownCast(container);
  if(scrollView)
  {
    scrollView.SetContent(child);
  }
  else
  {
    container.Add(child);
  }
  observer.Add(container);
  root.Add(observer);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  // Non-vacuity guard: C was measured at least twice -- once by M's measure
  // pass, once by M's arrange re-measuring the MATCH_PARENT slot. Without that
  // second measurement there is no arrange-owned measure to attribute and the
  // assertions below would hold trivially.
  DALI_TEST_CHECK(gClampMeasureProducerCount >= 2);
  const int observerCountAfterLayout = gPassThroughMeasureProducerCount;
  DALI_TEST_CHECK(observerCountAfterLayout > 0);

  // Two further identical passes. Nothing about OBS's inputs changed, and the
  // arrange-owned re-measure below it is not allowed to invalidate its cache.
  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gPassThroughMeasureProducerCount, observerCountAfterLayout, TEST_LOCATION);

  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_EQUALS(gPassThroughMeasureProducerCount, observerCountAfterLayout, TEST_LOCATION);
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsP(void)
{
  UiTestApplication application;
  // Plain View: the ArrangeDefault re-measure site.
  View container = View::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsStackP(void)
{
  UiTestApplication application;
  StackLayout       container = StackLayout::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsGridP(void)
{
  UiTestApplication application;
  GridLayout        container = GridLayout::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsFlexP(void)
{
  UiTestApplication application;
  FlexLayout        container = FlexLayout::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsAbsoluteP(void)
{
  UiTestApplication application;
  AbsoluteLayout    container = AbsoluteLayout::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

int UtcDaliViewArrangeOwnedChildMeasureDoesNotInvalidateAncestorsScrollViewP(void)
{
  UiTestApplication application;
  ScrollView        container = ScrollView::New();
  CheckArrangeOwnedChildMeasureDoesNotInvalidateAncestors(application, container);
  END_TEST;
}

// T7. The other side of the owner boundary: an arrange-in-progress ancestor that
// NO scope claims is not a boundary at all. The walk passes straight through it,
// clearing its caches on the way, and keeps climbing.
//
// Tree: root(fixed) -> OBS(pass-through counting producer) -> X(ArrangeCallback
// that measures D at a fresh constraint every time) -> A(fixed) -> D(clamp).
// D is a GRANDCHILD of X on purpose: no owner scope could ever legitimately
// cover it, so this is the genuine out-of-band case rather than a missing scope.
//
// (i) Every driven pass must run OBS's producer exactly once: X is
//     arrange-in-progress and unowned when D is measured, so the walk clears
//     A, X and OBS instead of stopping.
// (ii) And it must schedule NOTHING: the walk is cache-only, so the blocked
//     arrange publish on X (and on the arranging ancestors above it) must not
//     register a follow-up layout. Idle frames leave X's arrange count flat.
int UtcDaliViewUnownedArrangeInProgressAncestorLosesCacheP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount       = 0;
  gPassThroughMeasureProducerCount = 0;
  gDescendantMeasuringArrangeCount = 0;
  gArrangeMeasuredDescendant.Reset();

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View observer = View::New();
  observer.SetRequestedWidth(200.0f);
  observer.SetRequestedHeight(100.0f);
  observer.SetMeasureCallback(MeasureCallback::New(&CountingPassThroughMeasure));

  View x = View::New();
  x.SetRequestedWidth(200.0f);
  x.SetRequestedHeight(100.0f);
  // ALWAYS, and honestly so: the callback reads the file-static arming
  // state, which no layout invalidation tracks. It is also what this test NEEDS --
  // the armed out-of-band measure below must fire on every driven pass, and under
  // the IF_CHANGED default a settled X would be served from cache and
  // never measure D at all.
  x.SetArrangeCallback(ArrangeCallback::New(&DescendantMeasuringArrange), ArrangePolicy::ALWAYS);

  View a = View::New();
  a.SetRequestedWidth(200.0f);
  a.SetRequestedHeight(100.0f);

  View d = View::New();
  d.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  a.Add(d);
  x.Add(a);
  observer.Add(x);
  root.Add(observer);
  window.Add(root);

  // Settle with the out-of-band measurement disarmed.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_CHECK(gPassThroughMeasureProducerCount > 0);
  DALI_TEST_CHECK(gDescendantMeasuringArrangeCount > 0);

  // Arm it, then drive three identical passes. Each one arranges X, which
  // measures D out-of-band at a brand new constraint -- a guaranteed full miss,
  // so the walk runs, and it must reach OBS.
  gArrangeMeasuredDescendant = d;

  // One warm-up pass: the out-of-band measure happens while this pass ARRANGES,
  // i.e. after OBS was already measured in it, so the cache clear it performs is
  // consumed by the NEXT pass.
  root.InvalidateMeasure();
  application.SendNotification();

  int observerCount = gPassThroughMeasureProducerCount;
  for(int i = 0; i < 3; ++i)
  {
    root.InvalidateMeasure();
    application.SendNotification();
    DALI_TEST_EQUALS(gPassThroughMeasureProducerCount, observerCount + 1, TEST_LOCATION);
    observerCount = gPassThroughMeasureProducerCount;
  }

  // Cache-only: the blocked arrange publish registered no follow-up, so idle
  // frames run no further pass at all.
  const int arrangeCountBeforeIdle = gDescendantMeasuringArrangeCount;
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(gDescendantMeasuringArrangeCount, arrangeCountBeforeIdle, TEST_LOCATION);
  DALI_TEST_EQUALS(gPassThroughMeasureProducerCount, observerCount, TEST_LOCATION);

  // Sanity: the layout machinery is not simply wedged -- a real invalidation still
  // runs a pass, and X's producer with it.
  root.InvalidateArrange();
  application.SendNotification();
  DALI_TEST_EQUALS(gDescendantMeasuringArrangeCount, arrangeCountBeforeIdle + 1, TEST_LOCATION);

  gArrangeMeasuredDescendant.Reset();
  END_TEST;
}

// Third-party protection. A View / LayoutManager subclass that re-measures its OWN
// DIRECT child while arranging is protected WITHOUT pushing an owner scope: the
// safety-net stop treats an arrange-in-progress DIRECT parent as the owner. This is
// the case that matters for third-party code, which cannot push an
// ArrangeOwnedMeasureScope (the header is internal), so the framework must protect it
// automatically. Contrast with T7 above: there the measured view is a GRANDCHILD
// (genuinely out-of-band, so Δ1 clears the chain); here it is a DIRECT child owned by
// V's arrange, so OBS's cache must SURVIVE.
//
// Tree: root(fixed) -> OBS(pass-through counting producer) -> V(ArrangeCallback that
// measures its direct child C at a fresh constraint every arrange, no scope) -> C(clamp).
// Removing the safety-net stop makes the walk clear V and OBS and this test fail.
int UtcDaliViewUnscopedArrangeReMeasureOfDirectChildIsProtectedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gClampMeasureProducerCount       = 0;
  gPassThroughMeasureProducerCount = 0;
  gDescendantMeasuringArrangeCount = 0;
  gArrangeMeasuredDescendant.Reset();

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View observer = View::New();
  observer.SetRequestedWidth(200.0f);
  observer.SetRequestedHeight(100.0f);
  observer.SetMeasureCallback(MeasureCallback::New(&CountingPassThroughMeasure));

  // V stands in for a third-party View / LayoutManager: it re-measures its own child
  // from its arrange callback, but pushes no ArrangeOwnedMeasureScope.
  View v = View::New();
  v.SetRequestedWidth(200.0f);
  v.SetRequestedHeight(100.0f);
  // ALWAYS for the same two reasons as the sibling test above: the callback
  // reads untracked arming state, and the armed re-measure must fire on every pass.
  v.SetArrangeCallback(ArrangeCallback::New(&DescendantMeasuringArrange), ArrangePolicy::ALWAYS);

  View c = View::New();
  c.SetMeasureCallback(MeasureCallback::New(&ClampToConstraintMeasure));

  v.Add(c);
  observer.Add(v);
  root.Add(observer);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_CHECK(gPassThroughMeasureProducerCount > 0);
  DALI_TEST_CHECK(gDescendantMeasuringArrangeCount > 0);

  // Arm: V now re-measures its DIRECT child C at a fresh constraint every arrange.
  gArrangeMeasuredDescendant = c;

  // Non-vacuity: prove the arrange-time re-measure of C actually runs (a genuine miss
  // on every arrange), so there is a real re-measure for the safety net to protect.
  const int clampBeforeArm = gClampMeasureProducerCount;
  root.InvalidateMeasure();
  application.SendNotification();
  DALI_TEST_CHECK(gClampMeasureProducerCount > clampBeforeArm);

  // OBS's cache must SURVIVE across further identical passes: the safety-net stop
  // treats V (arrange-in-progress, the direct parent of C) as the owner and does not
  // climb to OBS.
  const int observerCount = gPassThroughMeasureProducerCount;
  for(int i = 0; i < 3; ++i)
  {
    root.InvalidateMeasure();
    application.SendNotification();
    DALI_TEST_EQUALS(gPassThroughMeasureProducerCount, observerCount, TEST_LOCATION);
  }

  gArrangeMeasuredDescendant.Reset();
  END_TEST;
}

// Phase 3c. Removing a subtree re-roots its effective-scale (INHERIT) chain, so
// the removed subtree's cached effective scale must be invalidated on EVERY node,
// not just the direct child. OnChildRemoved's InvalidateMeasure drops it on the
// child only and propagates upward -- the mirror of OnChildAdded's recursive reset
// was missing, so descendants kept a stale scale.
//
// Tree: global scale 2.0; root(INHERIT, on window)=2.0 -> P(scale DISABLED)=1.0
// -> C(INHERIT)=1.0 -> D(INHERIT, fixed 50)=1.0. Remove C from the DISABLED P and
// the whole C subtree re-roots at the global 2.0.
int UtcDaliViewRemoveInvalidatesSubtreeEffectiveScaleP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(2.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View p = View::New();
  p.SetUiScalePolicy(UiScalePolicy::DISABLED);
  p.SetRequestedWidth(200.0f);
  p.SetRequestedHeight(100.0f);

  View c = View::New();

  View d = View::New();
  d.SetRequestedWidth(50.0f);
  d.SetRequestedHeight(50.0f);

  c.Add(d);
  p.Add(c);
  root.Add(p);
  window.Add(root);

  // Warm every node's cached effective scale.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(GetImpl(d).GetEffectiveScale(), 1.0f, TEST_LOCATION);

  // Sever C from the DISABLED parent. C, now parentless, re-roots at the global
  // scale (2.0); D, still C's child, inherits it.
  p.Remove(c);

  DALI_TEST_EQUALS(GetImpl(c).GetEffectiveScale(), 2.0f, TEST_LOCATION);
  // The fix: D's cached scale was invalidated too, so it recomputes to 2.0.
  // Without the recursive invalidation D would report the stale 1.0.
  DALI_TEST_EQUALS(GetImpl(d).GetEffectiveScale(), 2.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// The geometry consequence of the above, through the path that has no
// ViewDataImpl::OnChildAdded to save it: remove a subtree from a scale-DISABLED
// parent and re-add it directly to the Window. A torn subtree would keep arranging
// D at the old scale.
int UtcDaliViewRemoveThenWindowAddRelayoutsSubtreeAtNewScaleP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(2.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View p = View::New();
  p.SetUiScalePolicy(UiScalePolicy::DISABLED);
  p.SetRequestedWidth(200.0f);
  p.SetRequestedHeight(100.0f);

  View c = View::New();

  View d = View::New();
  d.SetRequestedWidth(50.0f);
  d.SetRequestedHeight(50.0f);

  c.Add(d);
  p.Add(c);
  root.Add(p);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();
  // Baseline: D under the DISABLED parent is 50 wide (fixed 50 x scale 1.0).
  DALI_TEST_EQUALS(d.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  // Re-root C directly under the Window (fires no ViewDataImpl::OnChildAdded).
  p.Remove(c);
  window.Add(c);

  application.SendNotification();
  application.SendNotification();

  // D re-measures at the new effective scale 2.0: fixed 50 x 2.0 = 100. A torn
  // subtree (stale scale 1.0) would leave it at 50.
  DALI_TEST_EQUALS(d.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// Phase 3d (off-scene root gap). A layout ROOT removed from the Window -- not from
// a View parent, so no ViewDataImpl::OnChildRemoved fires -- is no longer tracked
// by UiScaleManager, so a global SetScale() made while it is detached never reaches
// it. On re-connection the root must re-derive its (stale) effective scale, which
// OnViewSceneConnection now does. Without it the root arranges at the scale in force
// before it was detached.
int UtcDaliViewOffSceneRootReconnectAfterScaleChangeRelayoutsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View root = View::New(); // INHERIT -> global scale
  root.SetRequestedWidth(50.0f);
  root.SetRequestedHeight(50.0f);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  // Detach, change the global scale while detached (the root, untracked, misses
  // it), then re-attach.
  window.Remove(root);
  UiScaleManager::Get().SetScale(2.0f);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  // Re-connection re-derived the scale (2.0) and re-measured: 50 x 2.0 = 100.
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveScale(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// The global master switch, observed through geometry. With a non-unit global
// scale in force, turning scaling off must re-arrange the whole tree at 1.0, and
// turning it back on must restore the stored scale. Both flips restore/re-apply
// the same stored value (SetScale is never called while off), so the switch, not
// the scale, is what moves the geometry here.
int UtcDaliViewUnscaledWhenNotScalableP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale    = UiScaleManager::Get().GetScale();
  const bool  originalScalable = UiScaleManager::Get().IsScalable();
  UiScaleManager::Get().SetScalable(true);
  UiScaleManager::Get().SetScale(2.0f);

  View root = View::New(); // INHERIT -> global scale
  root.SetRequestedWidth(50.0f);
  root.SetRequestedHeight(50.0f);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  // Scaling on, global 2.0: 50 x 2.0 = 100.
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  // Master switch off: the tree re-arranges unscaled despite the stored 2.0.
  UiScaleManager::Get().SetScalable(false);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 50.0f, TEST_LOCATION);

  // Master switch back on: the preserved 2.0 is re-applied.
  UiScaleManager::Get().SetScalable(true);
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
// Multiple callbacks connected to one View's KeyEventSignal.
//
// View combines the return values of its connected key callbacks with OR, so a
// single consuming callback consumes the event for all of them. Unlike the
// equivalent signals in dali-core and dali-adaptor this is NOT gated on geometry
// hittest: dali-ui always uses the combined behaviour.
///////////////////////////////////////////////////////////////////////////////

int UtcDaliViewKeyEventSignalMultipleCallbacksConsumedP(void)
{
  UiTestApplication application;
  View              parent = CreateFocusableView(application);
  View              child  = CreateFocusableView(application);
  parent.Add(child);
  application.SendNotification();
  application.Render();

  // Two callbacks on the SAME child view. The first consumes, the second one,
  // connected last, does not.
  KeyEventSignalData    consumingData;
  KeyEventSignalFunctor consumingFunctor(consumingData, true);
  child.KeyEventSignal().Connect(&application, consumingFunctor);

  KeyEventSignalData    passiveData;
  KeyEventSignalFunctor passiveFunctor(passiveData, false);
  child.KeyEventSignal().Connect(&application, passiveFunctor);

  // The parent only receives the event if the child did not consume it, so it is
  // the probe for whether the two return values were combined with OR.
  KeyEventSignalData    parentData;
  KeyEventSignalFunctor parentFunctor(parentData, false);
  parent.KeyEventSignal().Connect(&application, parentFunctor);

  FocusManager::Get().SetCurrentFocusView(child);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  // Every connected callback runs; combining the results does not short-circuit.
  DALI_TEST_CHECK(consumingData.called);
  DALI_TEST_CHECK(passiveData.called);

  // The consuming callback wins, so the event never reached the parent.
  DALI_TEST_CHECK(!parentData.called);
  END_TEST;
}

// Phase 4c (C4-B1 forward pin). ApplySelfBoundsIfChanged is an UNCONDITIONAL
// per-pass reconciliation, not a one-time apply: a re-Arrange restores a child
// whose actor geometry was clobbered externally -- e.g. via the sanctioned
// Extension::SetPositionX escape hatch that ScrollView / RecyclerView use to
// bypass layout. This passes today; it must keep passing after Phase 5, which
// means a Phase-5 arrange cache HIT has to return AFTER this reconciliation,
// never before it. Fails the day a hit is placed above ApplySelfBoundsIfChanged.
int UtcDaliViewArrangeRestoresExternallyMovedSelfGeometryP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedX(20.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  parent.Add(child);

  const LayoutRect slot(0.0f, 0.0f, 200.0f, 100.0f);
  parent.Measure(200.0f, 100.0f);
  parent.Arrange(slot);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  // Clobber the child's actor geometry directly (bypasses layout invalidation).
  Dali::Ui::Extension::View::SetPositionX(child, 999.0f);
  Dali::Ui::Extension::View::SetSizeWidth(child, 7.0f);

  // Re-arrange the parent into the same slot: the child's own Arrange reconciles
  // its actor geometry back to the arranged values.
  parent.Arrange(slot);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  END_TEST;
}

// Phase 4c (C4-B2 forward pin). Same under RTL: the child's Arrange re-applies its
// LOGICAL bounds and the parent's ApplyLayoutDirection mirrors from those logical
// bounds (never from the clobbered actor value, per 4a), so the child returns to
// its mirrored x -- proving the logical-apply + parent-mirror composition survives
// an external write and that a Phase-5 hit must re-apply logical bounds, not a
// mirrored value.
int UtcDaliViewArrangeRestoresExternallyMovedSelfGeometryRtlP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View child = View::New();
  child.SetRequestedX(20.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  parent.Add(child);

  const LayoutRect slot(0.0f, 0.0f, 200.0f, 100.0f);
  parent.Measure(200.0f, 100.0f);
  parent.Arrange(slot);
  // Mirror of logical x 20 about parent width 200 = 130.
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 130.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  Dali::Ui::Extension::View::SetPositionX(child, 999.0f);
  Dali::Ui::Extension::View::SetSizeWidth(child, 7.0f);

  parent.Arrange(slot);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 130.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  END_TEST;
}

// A STANDALONE child added to an already SETTLED parent is placed by the parent's
// next arrange pass.
//
// The placement of a standalone child lives on the ARRANGE path only
// (ArrangeStandaloneChildren), below the point where an arrange cache HIT returns.
// The parent's entry was published for a child set that did not contain this child,
// so it must not survive the add -- ViewDataImpl::OnChildAdded issues an
// InvalidateArrange() on the standalone-child path for exactly that reason. Adding a
// standalone child deliberately does NOT invalidate the parent's MEASURE (a standalone
// child contributes nothing to the parent's measured size), which is what makes the
// arrange-side signal the only one there is.
//
// The child is MATCH_PARENT on both axes -- the shape of the first-party standalone
// views (ScrollBar, focus indicator) -- so ArrangeStandaloneChild measures it against
// the parent's extent as part of placing it, and the expected geometry does not depend
// on a measure pass having run beforehand.
//
// Non-vacuity (verified by mutation): removing the InvalidateArrange() from
// OnChildAdded's standalone branch leaves the parent's entry live, the re-Arrange below
// serves it, and the standalone child is never placed. (While the hit is childless-only
// this is masked -- the parent stops being childless at the add and misses anyway --
// so this behavioural test is the pin for that bookkeeping.)
int UtcDaliViewArrangeStandaloneChildAddedAfterSettleIsPlacedP(void)
{
  UiTestApplication application;
  tet_infoline("A standalone child added after the parent settled is still placed by its next arrange");

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);

  View regular = View::New();
  regular.SetRequestedWidth(50.0f);
  regular.SetRequestedHeight(40.0f);
  parent.Add(regular);

  SettleLayout(application);

  // The slot the parent settled into IS the key its arrange entry was published under,
  // so the re-Arrange below is a hit candidate on every term but the one under test.
  const LayoutRect parentSlot(parent.GetProperty<float>(Actor::Property::POSITION_X),
                              parent.GetProperty<float>(Actor::Property::POSITION_Y),
                              parent.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                              parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  DALI_TEST_EQUALS(parentSlot.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parentSlot.height, 100.0f, TEST_LOCATION);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedX(10.0f);
  standalone.SetRequestedY(20.0f);
  standalone.SetRequestedWidth(MATCH_PARENT);
  standalone.SetRequestedHeight(MATCH_PARENT);
  parent.Add(standalone);

  // Nothing has placed it yet.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);

  // One arrange of the parent into the SAME slot. ArrangeStandaloneChild computes
  // (requestedX, requestedY) at the parent's full extent for a MATCH_PARENT child.
  parent.Arrange(parentSlot);

  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_Y), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, TEST_LOCATION);

  // The regular child is untouched by any of this.
  DALI_TEST_EQUALS(regular.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5a: the arrange cache HIT, for CHILDLESS (leaf) views.
//
// The whole increment is observed through one signal: whether a leaf's arrange
// producer RAN. Geometry is deliberately asserted alongside every count, because
// the contract is not "fewer producer runs" but "fewer producer runs AND a
// byte-identical result".
// ---------------------------------------------------------------------------

// THE WIN. A settled leaf does not re-run its arrange producer when a layout pass
// sweeps past it for a reason that has nothing to do with it: a SIBLING was
// invalidated, the root re-arranges, and the settled leaf is handed the same slot
// it already holds a cached result for.
//
// Non-vacuity (verified by mutation): disabling the hit predicate
// (`if(false && mArrangeCacheValid && ...)`) makes the leaf's producer run again
// and the count assertion below fails.
int UtcDaliViewArrangeCacheHitSkipsLeafProducerP(void)
{
  UiTestApplication application;
  tet_infoline("A settled leaf serves its arrange from cache when a sibling forces a pass");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leafA = View::New();
  leafA.SetRequestedWidth(50.0f);
  leafA.SetRequestedHeight(50.0f);
  leafA.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leafA);

  View leafB = View::New();
  leafB.SetRequestedWidth(50.0f);
  leafB.SetRequestedHeight(50.0f);
  root.Add(leafB);

  SettleLayout(application);

  const int settledCount = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(settledCount > 0);
  const float leafAx = leafA.GetProperty<float>(Actor::Property::POSITION_X);
  const float leafAw = leafA.GetProperty<float>(Actor::Property::SIZE_WIDTH);

  // Invalidate the SIBLING. This propagates up to the root and schedules a real
  // layout pass, which re-arranges every child -- including leafA, whose own
  // inputs (its measured slot, its position in the parent, the layout direction,
  // the effective scale) are all unchanged.
  leafB.SetRequestedX(11.0f);
  SettleLayout(application);

  // The pass really happened...
  DALI_TEST_EQUALS(leafB.GetProperty<float>(Actor::Property::POSITION_X), 11.0f, TEST_LOCATION);

  // ...and leafA was arranged by it without its producer running again.
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  // The result is unchanged, which is the other half of the contract.
  DALI_TEST_EQUALS(leafA.GetProperty<float>(Actor::Property::POSITION_X), leafAx, TEST_LOCATION);
  DALI_TEST_EQUALS(leafA.GetProperty<float>(Actor::Property::SIZE_WIDTH), leafAw, TEST_LOCATION);

  END_TEST;
}

// A hit is result-identical to the miss it replaces, on all four axes and on the
// value Arrange() hands back.
//
// Part 2 is what pins "return mArrangedBounds, not bounds": the producer returns a
// rect that differs from its input slot on every axis, so a hit that echoed its
// input would be visible in the return value even though the actor geometry
// (re-applied from mArrangedBounds either way) would look right.
//
// Non-vacuity (verified by mutation): `return bounds;` in place of
// `return mArrangedBounds;` in the hit body fails part 2; disabling the hit
// predicate leaves part 1 passing but breaks its producer-count assertions.
int UtcDaliViewArrangeCacheHitPreservesGeometryP(void)
{
  UiTestApplication application;
  tet_infoline("An arrange cache hit reproduces the missing pass's geometry and return value");

  gCountingArrangeProducerCount = 0;

  // --- Part 1: five repeat passes leave all four axes untouched.
  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedY(10.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const LayoutRect rootSlot(root.GetProperty<float>(Actor::Property::POSITION_X),
                            root.GetProperty<float>(Actor::Property::POSITION_Y),
                            root.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            root.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  const int settledCount = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(settledCount > 0);

  for(int pass = 0; pass < 5; ++pass)
  {
    root.Arrange(rootSlot);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_Y), 10.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
    // Every one of those passes served the leaf from cache.
    DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);
  }

  // --- Part 2: the hit returns the PUBLISHED bounds, not the input slot.
  gCountingArrangeProducerCount = 0;

  View standalone = View::New();
  standalone.SetRequestedWidth(50.0f);
  standalone.SetRequestedHeight(40.0f);
  standalone.SetArrangeCallback(ArrangeCallback::New(&CountingCustomBoundsArrange), ArrangePolicy::IF_CHANGED);
  application.GetScene().Add(standalone);

  const LayoutRect slot(0.0f, 0.0f, 50.0f, 40.0f);
  standalone.Measure(50.0f, 40.0f);

  const LayoutRect produced = standalone.Arrange(slot); // MISS: the producer runs.
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(produced.x, COUNTING_CUSTOM_ARRANGE_RESULT.x, TEST_LOCATION);
  DALI_TEST_EQUALS(produced.y, COUNTING_CUSTOM_ARRANGE_RESULT.y, TEST_LOCATION);
  DALI_TEST_EQUALS(produced.width, COUNTING_CUSTOM_ARRANGE_RESULT.width, TEST_LOCATION);
  DALI_TEST_EQUALS(produced.height, COUNTING_CUSTOM_ARRANGE_RESULT.height, TEST_LOCATION);

  const LayoutRect served = standalone.Arrange(slot); // HIT: no producer run.
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(served.x, produced.x, TEST_LOCATION);
  DALI_TEST_EQUALS(served.y, produced.y, TEST_LOCATION);
  DALI_TEST_EQUALS(served.width, produced.width, TEST_LOCATION);
  DALI_TEST_EQUALS(served.height, produced.height, TEST_LOCATION);

  // ...and the actor still carries the produced geometry, not the input slot, on every
  // one of the four axes.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), COUNTING_CUSTOM_ARRANGE_RESULT.x, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_Y), COUNTING_CUSTOM_ARRANGE_RESULT.y, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), COUNTING_CUSTOM_ARRANGE_RESULT.width, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), COUNTING_CUSTOM_ARRANGE_RESULT.height, TEST_LOCATION);

  END_TEST;
}

// The cache KEY. A leaf handed a DIFFERENT slot must miss and re-run its producer.
//
// The slot is changed by arranging the leaf directly rather than by moving it
// through its parent: a slot change routed through the parent (padding, requested
// position) also changes the constraint the leaf is measured against, so the leaf's
// arrange cache would be dropped by MeasurePassGuard and the KEY comparison would
// never be the thing under test.
//
// Non-vacuity (verified by mutation): making SameLayoutRect return true
// unconditionally makes the second Arrange hit, and both the producer count and the
// leaf's position assertion below fail.
int UtcDaliViewArrangeCacheMissOnDifferentSlotP(void)
{
  UiTestApplication application;
  tet_infoline("A leaf handed a different arrange slot misses and re-runs its producer");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  // The producer echoes its input, so the settled actor geometry IS the slot the
  // cache was keyed on.
  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  // Control: the SAME slot hits (this is also what makes the miss below meaningful).
  const int settledCount = gCountingArrangeProducerCount;
  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  // A different slot: one axis is enough.
  const LayoutRect movedSlot(leafSlot.x + 5.0f, leafSlot.y, leafSlot.width, leafSlot.height);
  leaf.Arrange(movedSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), leafSlot.x + 5.0f, TEST_LOCATION);

  END_TEST;
}

// A layout-direction change re-arranges the leaf rather than serving it from cache,
// and the mirror lands. Note the direction term of the hit predicate is belt and
// braces: the invalidating subtree walk already clears the cache, so a missed hook
// degrades to a MISS (slower) and never to a wrongly mirrored arrangement.
//
// Non-vacuity (verified by mutation): dropping the
// `mLastArrangeDirection == GetEffectiveLayoutDirection()` term from the predicate
// AND emptying ViewDataImpl::OnLayoutDirectionChanged leaves the leaf's cache live
// and unkeyed, so no pass is scheduled and both assertions below fail. The second
// half of that mutation is still sufficient on its own here: the flip is set on
// `root`, which is the layout root and therefore the hooked view, and a hooked
// view's OnPropertySet defers to the hook rather than raising a second walk.
int UtcDaliViewArrangeCacheMissOnDirectionChangeP(void)
{
  UiTestApplication application;
  tet_infoline("A layout-direction change costs the leaf its arrange cache entry");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const int settledCount = gCountingArrangeProducerCount;
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);

  // The producer echoes its input, so the settled actor geometry IS the slot the
  // cache was keyed on.
  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  // Control: the SAME slot under the UNCHANGED direction hits (this is also what
  // makes the miss below meaningful -- without it the miss could be vacuous).
  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount + 1, TEST_LOCATION);
  // Mirror of logical x 0 about parent width 200, child width 50 => 150.
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 150.0f, TEST_LOCATION);

  END_TEST;
}

// Corollary C. The hit predicate carries NO effective-scale term; it relies on every
// scale-context reset ALSO dropping the layout caches. This pins that pairing from
// the outside: after a global UI scale change the leaf must miss even though its
// slot, its direction and its dirty state are all untouched.
//
// The leaf is arranged directly, without an intervening Measure, so the only thing
// that can have invalidated its cache is the scale reset itself (a re-Measure would
// clear the arrange cache through MeasurePassGuard and hide the mechanism). It is a
// CHILD rather than the layout root, so UiScaleManager's InvalidateMeasure() lands on
// the root and never raises the leaf's own dirty bit.
//
// Non-vacuity (verified by mutation): removing `mArrangeCacheValid = false` from
// ViewDataImpl::InvalidateLayoutCaches leaves the entry live and the Arrange below
// hits, failing the count assertion.
int UtcDaliViewArrangeCacheMissOnScaleChangeP(void)
{
  UiTestApplication application;
  tet_infoline("A UI scale change drops the leaf's arrange cache entry");

  gCountingArrangeProducerCount = 0;

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  // Control: the same slot hits while the scale is unchanged.
  const int settledCount = gCountingArrangeProducerCount;
  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  UiScaleManager::Get().SetScale(2.0f);

  // Same slot, same direction, no dirty bit on the leaf -- and it must still miss.
  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount + 1, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// C4-B1. The hit is NOT a plain early return: it still reconciles the actor's
// geometry against the cached arranged bounds, so a write that bypassed layout (the
// sanctioned Extension::SetPositionX / SetSizeWidth escape hatch used by ScrollView
// and RecyclerView, or a transition frame) is repaired exactly as a miss would repair
// it. The flat producer count is what proves the repair came from a HIT.
//
// Non-vacuity (verified by mutation): moving ApplySelfBoundsIfChanged out of the hit
// body (an early `return mArrangedBounds;` above it) leaves the clobbered 999 / 7 in
// place and this test fails.
int UtcDaliViewArrangeCacheHitStillReconcilesSelfGeometryP(void)
{
  UiTestApplication application;
  tet_infoline("An arrange cache hit still restores externally clobbered self geometry");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  DALI_TEST_EQUALS(leafSlot.x, 20.0f, TEST_LOCATION);

  const int settledCount = gCountingArrangeProducerCount;

  // Clobber the leaf's actor geometry directly; this bypasses layout entirely and
  // deliberately does NOT invalidate the arrange cache.
  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);
  Dali::Ui::Extension::View::SetSizeWidth(leaf, 7.0f);

  leaf.Arrange(leafSlot);

  // Served from cache...
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);
  // ...and still repaired.
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  END_TEST;
}

// C4-B2 / C4-A2. Under RTL the hit re-applies the leaf's LOGICAL bounds; mirroring
// stays the parent's job (ApplyLayoutDirection reads the child's logical
// mArrangedBounds, never the actor). Composing the two is idempotent, so repeated
// passes over a clobbered child converge on the same mirrored x instead of
// oscillating.
//
// The leaf is arranged DIRECTLY first, on purpose: driven through the parent, the
// parent's ApplyLayoutDirection overwrites the child's POSITION_X from the logical
// bounds afterwards and would mask whatever the hit applied. The direct call is the
// only place the hit's own write is observable, so that is where C4-B2 is asserted.
//
// Non-vacuity (verified by mutation): re-applying an already-mirrored rect in the hit
// body (mirroring mArrangedBounds.x about the parent width before applying it) makes
// the direct call land on 130 instead of the logical 20.
int UtcDaliViewArrangeCacheHitReAppliesLogicalBoundsUnderRtlP(void)
{
  UiTestApplication application;
  tet_infoline("An RTL arrange cache hit re-applies logical bounds, leaving the mirror to the parent");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  // Mirror of logical x 20 about parent width 200, child width 50 => 130.
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 130.0f, TEST_LOCATION);

  const LayoutRect rootSlot(root.GetProperty<float>(Actor::Property::POSITION_X),
                            root.GetProperty<float>(Actor::Property::POSITION_Y),
                            root.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            root.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  const int settledCount = gCountingArrangeProducerCount;

  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);

  // The leaf's LOGICAL slot, as ArrangeDefault computes it: padding 0 + margin 0 +
  // requested x 20, at the measured 50 x 40. The producer echoes its input, so this
  // is also the key the cache entry was published under -- and the flat producer
  // count below is what proves the slot is right (a wrong slot would MISS).
  const LayoutRect leafLogicalSlot(20.0f, 0.0f, 50.0f, 40.0f);

  leaf.Arrange(leafLogicalSlot);

  // Served from cache, and the value it re-applied is the LOGICAL x, not a mirrored
  // one: mirroring is the parent's job and is not folded in here.
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);

  // Composed with the parent's mirror the pair is idempotent: repeated passes over
  // the clobbered child converge on 130 instead of oscillating 130 -> 20 -> 130.
  for(int pass = 0; pass < 3; ++pass)
  {
    root.Arrange(rootSlot);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 130.0f, TEST_LOCATION);
    // The leaf was served from cache on every one of those passes.
    DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);
  }

  END_TEST;
}

// A leaf's own full Measure invalidates its arrange cache: the measured size is an
// input to its arrangement, so a result produced against the previous measurement
// must not survive it. MeasurePassGuard is what clears it.
//
// Non-vacuity (verified by mutation): removing `mArrangeCacheValid = false` from
// MeasurePassGuard's constructor makes the Arrange below hit and the count assertion
// fails.
int UtcDaliViewArrangeCacheMissAfterOwnMeasureP(void)
{
  UiTestApplication application;
  tet_infoline("An out-of-band Measure on a leaf costs it its arrange cache entry");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  // Control: the same slot hits before the out-of-band measure.
  const int settledCount = gCountingArrangeProducerCount;
  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  // A genuine measure MISS (a constraint the leaf was never measured against).
  leaf.Measure(31.0f, 29.0f);

  leaf.Arrange(leafSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount + 1, TEST_LOCATION);

  END_TEST;
}

// Anti-spin. A hit writes nothing that feeds back into layout: no dirty bit, no
// LayoutController registration, and its actor writes (POSITION / SIZE) are not
// layout-invalidating properties. An idle application must therefore reach a fixed
// point and stay there.
//
// Non-vacuity (verified by mutation): adding an InvalidateArrange() call to the hit
// body turns every hit into a scheduled follow-up pass and the producer count keeps
// climbing frame after frame.
int UtcDaliViewArrangeCacheHitDoesNotScheduleFurtherLayoutP(void)
{
  UiTestApplication application;
  tet_infoline("Serving a leaf from the arrange cache schedules no further layout work");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(leaf);

  SettleLayout(application);

  const int settledCount = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(settledCount > 0);

  // Idle frames, plus repeated explicit passes: neither may raise new layout work.
  for(int frame = 0; frame < 5; ++frame)
  {
    SettleLayout(application);
    DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);
  }

  const LayoutRect rootSlot(root.GetProperty<float>(Actor::Property::POSITION_X),
                            root.GetProperty<float>(Actor::Property::POSITION_Y),
                            root.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            root.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  for(int pass = 0; pass < 5; ++pass)
  {
    root.Arrange(rootSlot);
  }
  SettleLayout(application);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount, TEST_LOCATION);

  END_TEST;
}

// ALWAYS is the explicit opt-out for a callback that must execute on every
// arrange pass, even when its layout inputs are unchanged.
int UtcDaliViewArrangeAlwaysCallbackRunsProducerP(void)
{
  UiTestApplication application;
  tet_infoline("An ALWAYS callback runs on every arrange pass");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::ALWAYS);
  root.Add(leaf);

  View sibling = View::New();
  sibling.SetRequestedWidth(50.0f);
  sibling.SetRequestedHeight(40.0f);
  root.Add(sibling);

  SettleLayout(application);

  const int settledCount = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(settledCount > 0);

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  const int PASSES = 4;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    leaf.Arrange(leafSlot);
    DALI_TEST_EQUALS(gCountingArrangeProducerCount, settledCount + pass + 1, TEST_LOCATION);
  }

  const int afterDirect = gCountingArrangeProducerCount;
  sibling.SetRequestedX(11.0f);
  SettleLayout(application);
  DALI_TEST_CHECK(gCountingArrangeProducerCount > afterDirect);

  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), leafSlot.x, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_Y), leafSlot.y, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), leafSlot.width, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT), leafSlot.height, TEST_LOCATION);

  END_TEST;
}

// The one-argument callback API uses IF_CHANGED. Explicit ALWAYS may
// opt out, and reinstalling through the one-argument API restores the default.
int UtcDaliViewDefaultArrangeCallbackSkipsUnchangedP(void)
{
  UiTestApplication application;
  tet_infoline("The one-argument callback uses IF_CHANGED by default");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange));
  root.Add(leaf);

  View sibling = View::New();
  sibling.SetRequestedWidth(50.0f);
  sibling.SetRequestedHeight(40.0f);
  root.Add(sibling);

  SettleLayout(application);

  const LayoutRect leafSlot(leaf.GetProperty<float>(Actor::Property::POSITION_X),
                            leaf.GetProperty<float>(Actor::Property::POSITION_Y),
                            leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                            leaf.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
  const int        PASSES       = 4;
  const int        defaultCount = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(defaultCount > 0);

  for(int pass = 0; pass < PASSES; ++pass)
  {
    leaf.Arrange(leafSlot);
  }
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, defaultCount, TEST_LOCATION);

  sibling.SetRequestedX(11.0f);
  SettleLayout(application);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, defaultCount, TEST_LOCATION);

  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::ALWAYS);
  SettleLayout(application);
  const int alwaysCount = gCountingArrangeProducerCount;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    leaf.Arrange(leafSlot);
    DALI_TEST_EQUALS(gCountingArrangeProducerCount, alwaysCount + pass + 1, TEST_LOCATION);
  }

  leaf.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange));
  SettleLayout(application);
  const int restoredDefaultCount = gCountingArrangeProducerCount;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    leaf.Arrange(leafSlot);
  }
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, restoredDefaultCount, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5b: the arrange cache HIT for views WITH children.
//
// A hit on a parent does not PRUNE its subtree: it replays it from cache, so every
// descendant ends the pass at exactly the geometry a re-run would have left it at.
// Every test below therefore asserts geometry alongside counts -- the contract is
// "fewer producer runs AND a byte-identical result", now stated at depth.
// ---------------------------------------------------------------------------

// THE WIN, at depth. A settled root → mid → leaf chain runs NO producer when a layout
// pass sweeps past it for a reason that has nothing to do with it. Under the
// childless-only hit, `mid` had a child and therefore always re-ran.
//
// Non-vacuity (verified by mutation): restoring `mChildren.Empty() &&` in the hit
// predicate makes `mid` miss on every pass and its counter climbs.
int UtcDaliViewArrangeCacheHitSkipsSubtreeProducersP(void)
{
  UiTestApplication application;
  tet_infoline("A settled subtree runs no arrange producer when a sibling forces a pass");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedX(10.0f);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  // The reason a pass happens at all. It is a sibling of `mid`, so its invalidation
  // walks up to the root and never touches the mid/leaf chain.
  View sibling = View::New();
  sibling.SetRequestedWidth(30.0f);
  sibling.SetRequestedHeight(30.0f);
  root.Add(sibling);

  SettleLayout(application);

  const int midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();
  DALI_TEST_CHECK(midBase > 0);
  DALI_TEST_CHECK(leafBase > 0);

  const LayoutRect midRect  = ActorRectOf(mid);
  const LayoutRect leafRect = ActorRectOf(leaf);
  DALI_TEST_EQUALS(midRect.x, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leafRect.x, 20.0f, TEST_LOCATION);

  sibling.SetRequestedX(11.0f);
  SettleLayout(application);

  // The pass really happened...
  DALI_TEST_EQUALS(sibling.GetProperty<float>(Actor::Property::POSITION_X), 11.0f, TEST_LOCATION);

  // ...and NEITHER producer in the settled subtree ran again.
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);

  // The other half of the contract: the result is byte-identical at every level.
  CheckActorRect(mid, midRect, TEST_LOCATION);
  CheckActorRect(leaf, leafRect, TEST_LOCATION);

  END_TEST;
}

// A subtree hit is result-identical to the miss it replaces, on all four axes at
// every node, and it is IDEMPOTENT: repeated passes converge instead of drifting.
//
// The clobber before the first pass is what makes the sweep's per-node
// ApplySelfBoundsIfChanged load-bearing here rather than merely redundant -- without
// an external write, an omitted re-apply would be invisible in a clean loop.
//
// Non-vacuity (verified by mutation): replacing the sweep's ApplySelfBoundsIfChanged
// with a no-op leaves the clobbered values in place and the first pass's assertions
// fail.
int UtcDaliViewArrangeCacheHitPreservesSubtreeGeometryP(void)
{
  UiTestApplication application;
  tet_infoline("Repeated subtree hits reproduce all four axes at every node");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedX(10.0f);
  mid.SetRequestedY(5.0f);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedY(15.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);
  const LayoutRect midRect  = ActorRectOf(mid);
  const LayoutRect leafRect = ActorRectOf(leaf);

  const int midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();

  // Drive every axis of both descendants off their arranged values, bypassing layout.
  Dali::Ui::Extension::View::SetPositionX(mid, 901.0f);
  Dali::Ui::Extension::View::SetPositionY(mid, 902.0f);
  Dali::Ui::Extension::View::SetSizeWidth(mid, 903.0f);
  Dali::Ui::Extension::View::SetSizeHeight(mid, 904.0f);
  Dali::Ui::Extension::View::SetPositionX(leaf, 905.0f);
  Dali::Ui::Extension::View::SetPositionY(leaf, 906.0f);
  Dali::Ui::Extension::View::SetSizeWidth(leaf, 907.0f);
  Dali::Ui::Extension::View::SetSizeHeight(leaf, 908.0f);

  for(int pass = 0; pass < 5; ++pass)
  {
    root.Arrange(rootSlot);

    CheckActorRect(root, rootSlot, TEST_LOCATION);
    CheckActorRect(mid, midRect, TEST_LOCATION);
    CheckActorRect(leaf, leafRect, TEST_LOCATION);

    // Every one of those passes was served from cache, top to bottom.
    DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
    DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);
  }

  END_TEST;
}

// The external-clobber repair, at depth >= 2. This is the invariant the true-prune
// design could not keep: a GRANDCHILD moved behind layout's back (the sanctioned
// Extension:: escape hatch that ScrollView / RecyclerView use, or a transition frame)
// is restored by the parent's next arrange -- and it is restored on a HIT, with no
// producer in the chain running.
//
// Non-vacuity (verified by mutation): removing the child recursion (step 2) from
// ReplayArrangeSubtreeFromCache leaves the grandchild at 999 / 7 and this fails.
int UtcDaliViewArrangeCacheHitRestoresClobberedDescendantP(void)
{
  UiTestApplication application;
  tet_infoline("A subtree hit restores a grandchild whose geometry was clobbered outside layout");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedX(10.0f);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);
  const LayoutRect leafRect = ActorRectOf(leaf);
  DALI_TEST_EQUALS(leafRect.x, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leafRect.width, 50.0f, TEST_LOCATION);

  const int midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();

  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);
  Dali::Ui::Extension::View::SetSizeWidth(leaf, 7.0f);

  root.Arrange(rootSlot);

  // Repaired...
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  // ...by a HIT: nothing in the chain re-ran its producer.
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);

  END_TEST;
}

// The same at depth >= 2 under RTL, which is where the composition is delicate: the
// replay re-applies each node's LOGICAL bounds and leaves the mirror to that node's
// PARENT, exactly as a miss does. Getting the order wrong (mirroring in the self-apply,
// or mirroring before the children are visited) shows up either as the wrong value or
// as an oscillation across repeated passes, so both are asserted.
//
// Non-vacuity (verified by mutation): dropping the ApplyLayoutDirection call (step 3)
// from ReplayArrangeSubtreeFromCache leaves the grandchild at its logical x and the
// first assertion fails.
int UtcDaliViewArrangeCacheHitRestoresClobberedDescendantRtlP(void)
{
  UiTestApplication application;
  tet_infoline("An RTL subtree hit restores the mirrored geometry of a clobbered grandchild");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  // mid: logical x 0, mirrored about the root's 200 with width 120 => 80.
  // leaf: logical x 20, mirrored about mid's 120 with width 50 => 50.
  DALI_TEST_EQUALS(mid.GetProperty<float>(Actor::Property::POSITION_X), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 50.0f, TEST_LOCATION);

  const LayoutRect rootSlot = ActorRectOf(root);
  const int        midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int        leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();

  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);
  Dali::Ui::Extension::View::SetPositionX(mid, 998.0f);

  // Three consecutive hits: the first repairs, the rest must not move anything.
  for(int pass = 0; pass < 3; ++pass)
  {
    root.Arrange(rootSlot);

    DALI_TEST_EQUALS(mid.GetProperty<float>(Actor::Property::POSITION_X), 80.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 50.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
    DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);
  }

  END_TEST;
}

namespace
{
// --- Physical-write and re-entry observation for the arrange cache-HIT replay -------
//
// Every actor write the layout system performs goes through Object::SetProperty, which
// emits PropertySetSignal synchronously. That makes the signal two things at once: an
// exact counter of the PHYSICAL writes a pass performed, and the very channel through
// which a replay's writes can reach application code.

// Counts notifications for ONE property index on ONE view.
struct SinglePropertyWriteCounter : public Dali::ConnectionTracker
{
  explicit SinglePropertyWriteCounter(Dali::Property::Index watched)
  : watched(watched)
  {
  }

  void Connect(Ui::View view)
  {
    Dali::Handle handle = view;
    handle.PropertySetSignal().Connect(this, &SinglePropertyWriteCounter::OnSet);
  }

  void OnSet(Dali::Handle, Dali::Property::Index index, const Dali::Property::Value&)
  {
    if(index == watched)
    {
      ++count;
    }
  }

  Dali::Property::Index watched;
  int                   count{0};
};

// Runs `action` from inside the FIRST notification of `watched` received while ARMED,
// then disarms. Arming is explicit so that neither the settle-time writes nor the
// deliberate clobber that makes the replay write at all consumes the shot.
//
// SetTriggerValue narrows the shot further, to the first notification carrying a given
// value. A single pass can write the same axis more than once -- a MISS applies the
// child's LOGICAL x from the producer recursion and its MIRRORED x afterwards -- and a
// test that means "when the mirror runs" must say so rather than count writes.
struct OneShotPropertySetAction : public Dali::ConnectionTracker
{
  OneShotPropertySetAction(Dali::Property::Index watched, std::function<void()> action)
  : watched(watched),
    action(std::move(action))
  {
  }

  void Connect(Ui::View view)
  {
    Dali::Handle handle = view;
    handle.PropertySetSignal().Connect(this, &OneShotPropertySetAction::OnSet);
  }

  void SetTriggerValue(float value)
  {
    triggerValue    = value;
    hasTriggerValue = true;
  }

  void OnSet(Dali::Handle, Dali::Property::Index index, const Dali::Property::Value& value)
  {
    if(!armed || index != watched)
    {
      return;
    }
    if(hasTriggerValue && value.Get<float>() != triggerValue)
    {
      return;
    }
    armed = false;
    ++fireCount;
    action();
  }

  Dali::Property::Index watched;
  std::function<void()> action;
  bool                  armed{false};
  int                   fireCount{0};
  bool                  hasTriggerValue{false};
  float                 triggerValue{0.0f};
};
} // namespace

// Items 1+2. The right-to-left mirror is FOLDED into each node's own single self apply
// instead of being re-applied afterwards by the parent, so a replayed child's POSITION_X
// is written at most ONCE per pass -- the physical value straight away, never the logical
// value followed by its mirror. A settled right-to-left subtree therefore performs no
// write at all, which is what a cache hit is supposed to cost.
//
// The write count is the assertion, not the final value: the old two-write shape ended on
// the same number, but each write emitted a synchronous PropertySetSignal and so ran
// application code from inside the hit.
//
// Non-vacuity (verified by mutation): restoring the parent-side mirror
// (ApplyLayoutDirection at the end of the replay, self apply left logical) makes the
// settled phase count 2 and the repair phase 2.
int UtcDaliViewArrangeCacheHitReplayWritesMirroredPositionOnceP(void)
{
  UiTestApplication application;
  tet_infoline("A right-to-left cache-hit replay writes each child's mirrored position exactly once");

  View root = CreateCountingContainer(true);
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  // The settled mirrored geometry, the same values
  // UtcDaliViewArrangeCacheHitRestoresClobberedDescendantRtlP pins: mid's logical 0
  // mirrored about the root's 200 at width 120 => 80; leaf's logical 20 mirrored about
  // mid's 120 at width 50 => 50.
  DALI_TEST_EQUALS(mid.GetProperty<float>(Actor::Property::POSITION_X), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 50.0f, TEST_LOCATION);

  const LayoutRect rootSlot = ActorRectOf(root);
  const int        midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int        leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();

  SinglePropertyWriteCounter writes(Actor::Property::POSITION_X);
  writes.Connect(leaf);

  // Settled: the replay computes the value the actor already holds, so nothing is
  // written and no application code is reached.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(writes.count, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 50.0f, TEST_LOCATION);

  // Clobbered outside layout: the hit still repairs it, in ONE write.
  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);
  writes.count = 0;

  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(writes.count, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 50.0f, TEST_LOCATION);

  // Both phases were genuine hits: no producer in the chain re-ran.
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);

  END_TEST;
}

// Item 1. A replay WRITES actor properties and therefore runs application code, so an
// invalidation raised from that code is an IN-PASS invalidation and must be PARKED:
// retained as pending work, never allowed to arm an idle ProcessEvents wake.
// ReplayPassScope is what puts the hit inside the layout processing window that
// LayoutController::RequestIdleWakeIfAllowed reads.
//
// Non-vacuity (verified by mutation): without ReplayPassScope the hit runs at layout-pass
// depth 0, the invalidation arms a wake, and the NO-WAKE assertion fails.
int UtcDaliViewArrangeCacheHitReplayParksInPassInvalidationN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("An invalidation raised from a cache-hit replay's actor write is parked, not woken");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  View root = CreateCountingContainer(true);
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  const LayoutRect rootSlot = ActorRectOf(root);
  const int        rootBase = CountingContainerImplOf(root).GetArrangeCallCount();
  const int        midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();

  // A settled hit is write-free, so the observer would never run without this: the
  // clobber is what gives the replay something to repair.
  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);

  // The observer resets the render controller immediately BEFORE it invalidates. That is
  // the attribution trick SendRequestedProcessEvents documents, and it is REQUIRED here:
  // the replay's repair write queues a scene-graph message, and dali-core asks for an idle
  // ProcessEvents cycle for any message queued outside Core::ProcessEvents
  // (MessageQueue::ReserveMessageSlot), so a request observed across the whole call would
  // belong to the write rather than to layout. After this reset the only remaining writer
  // is the rest of that same reconciliation, whose other three axes are already correct --
  // so any request seen below came from the invalidation.
  OneShotPropertySetAction invalidator(Actor::Property::POSITION_X,
                                       [&application, &root]() {
                                         application.GetRenderController().Initialize();
                                         root.InvalidateMeasure();
                                       });
  invalidator.Connect(leaf);
  invalidator.armed = true;

  root.Arrange(rootSlot);

  DALI_TEST_EQUALS(invalidator.fireCount, 1, TEST_LOCATION);

  // NO WAKE: the replay held the layout-pass depth open, so the invalidation raised from
  // inside it could not request another idle ProcessEvents cycle.
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The hit itself still elided every producer and repaired the clobber.
  DALI_TEST_EQUALS(CountingContainerImplOf(root).GetArrangeCallCount(), rootBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  // RETENTION: the parked work is still there. An independently triggered ProcessEvents
  // services it -- the root's producer re-runs and the batch settles into a fresh emit.
  SendIndependentProcessEvents(application);
  DALI_TEST_CHECK(CountingContainerImplOf(root).GetArrangeCallCount() > rootBase);
  DALI_TEST_EQUALS(emitCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  END_TEST;
}

// Item 1. ReplayNodeScope raises mArrangeInProgress for every node a replay visits, so a
// property-set observer woken by the replay's own write meets the same release-mode
// re-entrancy guard a producer pass would give it. Without it the observer's Arrange()
// re-runs the producer of the view being replayed and rewrites its records underneath the
// walk that is serving them.
//
// The absorbed call still POISONS that view's pass, which the second half asserts: poison
// is what the guard trades the re-entrant work for, and both the node-local predicate and
// the recursive subtree gate must honour it.
//
// Non-vacuity (verified by mutation): without ReplayNodeScope the re-entrant Arrange()
// runs the leaf's producer (the flat-count assertion fails) and leaves POSITION_X at the
// re-entrant rect's x rather than the cached one.
int UtcDaliViewArrangeCacheHitReplayGuardsReentrantArrangeN(void)
{
  UiTestApplication application;
  tet_infoline("A cache-hit replay's actor write cannot re-enter Arrange() on the view being replayed");

  View root = CreateCountingContainer(true);
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedX(20.0f);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);
  const LayoutRect leafRect = ActorRectOf(leaf);
  DALI_TEST_EQUALS(leafRect.x, 20.0f, TEST_LOCATION);

  const int midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();

  Dali::Ui::Extension::View::SetPositionX(leaf, 999.0f);

  OneShotPropertySetAction reenter(Actor::Property::POSITION_X,
                                   [&leaf]() { leaf.Arrange(LayoutRect(1.0f, 2.0f, 3.0f, 4.0f)); });
  reenter.Connect(leaf);
  reenter.armed = true;

  root.Arrange(rootSlot);

  DALI_TEST_EQUALS(reenter.fireCount, 1, TEST_LOCATION);

  // Absorbed: no producer ran for the re-entrant call, and the geometry the replay is
  // applying stands rather than the rect the observer asked for.
  DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);
  CheckActorRect(leaf, leafRect, TEST_LOCATION);

  // ...and the poison the guard left behind is honoured: the next identical pass cannot
  // be served from cache at any level of the chain.
  root.Arrange(rootSlot);
  DALI_TEST_CHECK(CountingContainerImplOf(mid).GetArrangeCallCount() > midBase);
  DALI_TEST_CHECK(CountingContainerImplOf(leaf).GetArrangeCallCount() > leafBase);

  END_TEST;
}

// Item 3, replay half. Every child traversal that WRITES must be a snapshot, not an index
// loop over a live container. dali-core erases a child from its container BEFORE notifying
// (ActorParentImpl::Remove), so an unparent performed from a property-set observer shifts
// every following sibling down one position: an index loop that re-reads Count() stays in
// bounds, but silently skips the sibling that moved into the index it has already passed.
//
// Non-vacuity (verified by mutation): with the replay's child loop written as
// `for(i = 0; i < mChildren.Count(); ++i)`, removing `a` from index 0 leaves `b` at index 0
// while `i` has advanced to 1, so `b` is never visited and keeps its clobbered position.
int UtcDaliViewArrangeCacheHitReplayVisitsEverySiblingWhenChildRemovedP(void)
{
  UiTestApplication application;
  tet_infoline("A child unparented during a cache-hit replay does not cost its siblings their reconciliation");

  View parent = CreateCountingContainer(true);
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  application.GetScene().Add(parent);

  View a = CreateCountingContainer(true);
  a.SetRequestedX(0.0f);
  a.SetRequestedWidth(30.0f);
  a.SetRequestedHeight(20.0f);
  parent.Add(a);

  View b = CreateCountingContainer(true);
  b.SetRequestedX(40.0f);
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(20.0f);
  parent.Add(b);

  View c = CreateCountingContainer(true);
  c.SetRequestedX(100.0f);
  c.SetRequestedWidth(20.0f);
  c.SetRequestedHeight(20.0f);
  parent.Add(c);

  SettleLayout(application);

  DALI_TEST_EQUALS(a.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::POSITION_X), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetProperty<float>(Actor::Property::POSITION_X), 100.0f, TEST_LOCATION);

  const LayoutRect parentSlot = ActorRectOf(parent);
  const int        parentBase = CountingContainerImplOf(parent).GetArrangeCallCount();
  const int        bBase      = CountingContainerImplOf(b).GetArrangeCallCount();

  // All three need repairing, so the replay writes at every one of them -- which is what
  // makes "was this sibling visited?" observable at all.
  Dali::Ui::Extension::View::SetPositionX(a, 999.0f);
  Dali::Ui::Extension::View::SetPositionX(b, 999.0f);
  Dali::Ui::Extension::View::SetPositionX(c, 999.0f);

  OneShotPropertySetAction remover(Actor::Property::POSITION_X,
                                   [&parent, &a]() { parent.Remove(a); });
  remover.Connect(a);
  remover.armed = true;

  parent.Arrange(parentSlot);

  DALI_TEST_EQUALS(remover.fireCount, 1, TEST_LOCATION);

  // Both surviving siblings were visited and repaired, in spite of the container having
  // shifted underneath the walk.
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::POSITION_X), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetProperty<float>(Actor::Property::POSITION_X), 100.0f, TEST_LOCATION);

  // ...and it really was a hit: no producer ran.
  DALI_TEST_EQUALS(CountingContainerImplOf(parent).GetArrangeCallCount(), parentBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(b).GetArrangeCallCount(), bBase, TEST_LOCATION);

  END_TEST;
}

// Item 3, MISS half. The same defect, in the traversal a MISS uses: ApplyLayoutDirection
// mirrors the direct children after the producer recursion, and its SetPositionX is an
// actor write with the same synchronous PropertySetSignal reach.
//
// Driven through a forced MISS so the mirror under test is the parent-side one; the hit
// path folds its mirror into each child's own self apply and never reaches here.
//
// The observer is armed on the MIRRORED value, not on "the first write": a MISS writes each
// child's LOGICAL x from the producer recursion first, and the removal has to land inside
// the mirror loop to exercise it.
//
// Non-vacuity (verified by mutation): with ApplyLayoutDirection's index loop, `b` is never
// mirrored and keeps the logical x the producer left it at.
int UtcDaliViewRtlMirrorVisitsEverySiblingWhenChildRemovedP(void)
{
  UiTestApplication application;
  tet_infoline("A child unparented during the right-to-left mirror does not cost its siblings their mirror");

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  View a = View::New();
  a.SetRequestedX(0.0f);
  a.SetRequestedWidth(30.0f);
  a.SetRequestedHeight(20.0f);
  parent.Add(a);

  View b = View::New();
  b.SetRequestedX(40.0f);
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(20.0f);
  parent.Add(b);

  View c = View::New();
  c.SetRequestedX(100.0f);
  c.SetRequestedWidth(20.0f);
  c.SetRequestedHeight(20.0f);
  parent.Add(c);

  SettleLayout(application);

  // Mirrored about the parent's 200: a 200-0-30 = 170, b 200-40-50 = 110, c 200-100-20 = 80.
  DALI_TEST_EQUALS(a.GetProperty<float>(Actor::Property::POSITION_X), 170.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::POSITION_X), 110.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetProperty<float>(Actor::Property::POSITION_X), 80.0f, TEST_LOCATION);

  const LayoutRect parentSlot = ActorRectOf(parent);

  OneShotPropertySetAction remover(Actor::Property::POSITION_X,
                                   [&parent, &a]() { parent.Remove(a); });
  remover.SetTriggerValue(170.0f);
  remover.Connect(a);
  remover.armed = true;

  // Force the MISS: identical geometry, so the only thing that changes is which traversal
  // performs the mirror.
  parent.InvalidateArrange();
  parent.Arrange(parentSlot);

  DALI_TEST_EQUALS(remover.fireCount, 1, TEST_LOCATION);

  // The producer recursion left every child at its LOGICAL x; both survivors were still
  // reached by the mirror afterwards.
  DALI_TEST_EQUALS(b.GetProperty<float>(Actor::Property::POSITION_X), 110.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetProperty<float>(Actor::Property::POSITION_X), 80.0f, TEST_LOCATION);

  END_TEST;
}

// The recursive gate, invalidation half. A STANDALONE grandchild is a layout BOUNDARY:
// its InvalidateArrange stops at itself and self-registers, so it never reaches `mid`
// or the root and their cache entries stay live. Only a PER-NODE test in the subtree
// gate can see it -- and it must refuse the whole hit, because the invalidated node is
// one the replay would otherwise have written cached bounds over.
//
// Non-vacuity (verified by mutation): dropping BOTH `childData.mArrangeCacheValid` and
// `!childData.mArrangeDirty` from CanReplayArrangeSubtreeFromCache lets the root hit and
// the producer count stays flat. Dropping either one ALONE does not break it, and is not
// expected to: InvalidateArrange raises the dirty bit and clears the cache-valid bit in
// the same breath, so on this path the two are redundant with each other. That
// redundancy is the point -- the dirty/poison/blocked terms are defence in depth against
// the pairing being broken later, exactly as they are in the node-local predicate.
int UtcDaliViewArrangeCacheMissWhenDescendantIsDirtyP(void)
{
  UiTestApplication application;
  tet_infoline("A dirty standalone grandchild refuses the whole subtree hit");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(30.0f);
  standalone.SetRequestedHeight(25.0f);
  mid.Add(standalone);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);

  // Warm-up, and NOT part of what is under test. A standalone view is its own layout
  // root, so the settle batch drives it after its parent and its measure publish
  // leaves the slot marked unconsumed -- which the node-local predicate rejects on
  // `mid` in its own right. One pass consumes it (ArrangeStandaloneChildren clears the
  // bit), and only after that is a hit reachable at all.
  root.Arrange(rootSlot);

  const int midBase = CountingContainerImplOf(mid).GetArrangeCallCount();
  DALI_TEST_CHECK(midBase > 0);

  // Control: with nothing dirty, the same slot HITS. Without this the miss below
  // could be vacuous.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);

  // The boundary stop: this invalidation never reaches `mid` or `root`.
  standalone.InvalidateArrange();

  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase + 1, TEST_LOCATION);

  // ...and the refusal is one pass long: the miss consumed the dirty bit, so the next
  // identical pass hits again.
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase + 1, TEST_LOCATION);

  END_TEST;
}

// The recursive gate: an ALWAYS producer anywhere in a subtree makes the
// whole subtree re-run. The gate consults mArrangeProducerAlways at every node it
// would elide,
// where the childless-only hit consulted it only at the node being served.
//
// The two chains are identical except for the policy on their deepest node.
//
// Non-vacuity (verified by mutation): dropping `childData.mArrangeProducerAlways` from
// CanReplayArrangeSubtreeFromCache lets the ALWAYS chain hit and its counters go flat.
int UtcDaliViewArrangeCacheMissWhenDescendantIsAlwaysP(void)
{
  UiTestApplication application;
  tet_infoline("An ALWAYS producer at any depth makes the whole subtree re-run");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  // Chain A: every node uses IF_CHANGED.
  View ifChangedMid = CreateCountingContainer(true);
  ifChangedMid.SetRequestedWidth(120.0f);
  ifChangedMid.SetRequestedHeight(60.0f);
  root.Add(ifChangedMid);

  View ifChangedLeaf = CreateCountingContainer(true);
  ifChangedLeaf.SetRequestedWidth(50.0f);
  ifChangedLeaf.SetRequestedHeight(40.0f);
  ifChangedMid.Add(ifChangedLeaf);

  // Chain B: identical, except the deepest node explicitly uses ALWAYS.
  View alwaysMid = CreateCountingContainer(true);
  alwaysMid.SetRequestedWidth(120.0f);
  alwaysMid.SetRequestedHeight(60.0f);
  root.Add(alwaysMid);

  View alwaysLeaf = CreateCountingContainer(false);
  alwaysLeaf.SetRequestedWidth(50.0f);
  alwaysLeaf.SetRequestedHeight(40.0f);
  alwaysMid.Add(alwaysLeaf);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);

  const int ifChangedMidBase  = CountingContainerImplOf(ifChangedMid).GetArrangeCallCount();
  const int ifChangedLeafBase = CountingContainerImplOf(ifChangedLeaf).GetArrangeCallCount();
  const int alwaysMidBase     = CountingContainerImplOf(alwaysMid).GetArrangeCallCount();
  const int alwaysLeafBase    = CountingContainerImplOf(alwaysLeaf).GetArrangeCallCount();
  DALI_TEST_CHECK(alwaysLeafBase > 0);

  const LayoutRect alwaysLeafRect = ActorRectOf(alwaysLeaf);

  const int PASSES = 3;
  for(int pass = 0; pass < PASSES; ++pass)
  {
    root.Arrange(rootSlot);
  }

  // The IF_CHANGED chain is served on every pass...
  DALI_TEST_EQUALS(CountingContainerImplOf(ifChangedMid).GetArrangeCallCount(), ifChangedMidBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(ifChangedLeaf).GetArrangeCallCount(), ifChangedLeafBase, TEST_LOCATION);

  // ...while one ALWAYS node re-runs its own producer AND its ancestor's, on every
  // pass. (The root misses too, which is why the pure chain above is reached at all:
  // it is served by its own node-local hit, not by the root's.)
  DALI_TEST_EQUALS(CountingContainerImplOf(alwaysMid).GetArrangeCallCount(), alwaysMidBase + PASSES, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(alwaysLeaf).GetArrangeCallCount(), alwaysLeafBase + PASSES, TEST_LOCATION);

  // Always-miss must still be result-identical.
  CheckActorRect(alwaysLeaf, alwaysLeafRect, TEST_LOCATION);

  END_TEST;
}

// The replay visits exactly the nodes the producer would have arranged, and no others.
// LabelImpl::OnArrange returns its input bounds and never touches children, so a View
// child of a Label holds no arrange result and a MISS leaves its actor geometry alone.
// The sweep must do the same -- writing a never-arranged child's (default) bounds over
// whatever is there would be a geometry change invented by the optimisation.
//
// Non-vacuity (verified by mutation): removing the `mArrangeResultAvailable` filter
// from step 2 of ReplayArrangeSubtreeFromCache makes the sweep descend into the
// unarranged child, overwriting the values below (and tripping its own
// DALI_ASSERT_DEBUG in a debug build).
int UtcDaliViewArrangeCacheHitSkipsUnarrangedChildrenP(void)
{
  UiTestApplication application;
  tet_infoline("A subtree hit does not write geometry for a child the producer never arranges");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  Label label = Label::New();
  label.SetProperty(Label::Property::TEXT, "hello");
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(60.0f);
  root.Add(label);

  // A View child of a Label: in mChildren, never arranged by LabelImpl::OnArrange.
  View orphan = View::New();
  orphan.SetRequestedWidth(40.0f);
  orphan.SetRequestedHeight(30.0f);
  label.Add(orphan);

  SettleLayout(application);

  const LayoutRect rootSlot = ActorRectOf(root);

  // Park the never-arranged child somewhere layout would never put it.
  Dali::Ui::Extension::View::SetPositionX(orphan, 42.0f);
  Dali::Ui::Extension::View::SetSizeWidth(orphan, 7.0f);

  // The reference behaviour: a MISS leaves it exactly there.
  root.InvalidateArrange();
  SettleLayout(application);
  DALI_TEST_EQUALS(orphan.GetProperty<float>(Actor::Property::POSITION_X), 42.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(orphan.GetProperty<float>(Actor::Property::SIZE_WIDTH), 7.0f, TEST_LOCATION);

  // ...and so must a HIT, on every one of these passes.
  for(int pass = 0; pass < 3; ++pass)
  {
    root.Arrange(rootSlot);
    DALI_TEST_EQUALS(orphan.GetProperty<float>(Actor::Property::POSITION_X), 42.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(orphan.GetProperty<float>(Actor::Property::SIZE_WIDTH), 7.0f, TEST_LOCATION);
  }

  // The Label itself is still reconciled by those passes: the child is skipped, not
  // the node.
  const LayoutRect labelRect = ActorRectOf(label);
  Dali::Ui::Extension::View::SetPositionX(label, 555.0f);
  root.Arrange(rootSlot);
  DALI_TEST_EQUALS(label.GetProperty<float>(Actor::Property::POSITION_X), labelRect.x, TEST_LOCATION);

  END_TEST;
}

// A non-standalone child with no arrange result has no parent-owned logical X to
// mirror. Treating its current actor X as logical input makes the transform an
// involution (x -> mirrored x -> x), so repeated identical passes oscillate. Both a
// miss and every cache hit must leave a child ignored by the producer untouched.
int UtcDaliViewRtlLeavesUnarrangedChildUntouchedP(void)
{
  UiTestApplication application;

  Label parent = Label::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(60.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  // Label's pure OnArrange echoes its own bounds and never arranges children.
  View ignored = View::New();
  ignored.SetRequestedWidth(40.0f);
  ignored.SetRequestedHeight(30.0f);
  parent.Add(ignored);

  const LayoutRect slot(0.0f, 0.0f, 120.0f, 60.0f);
  parent.Measure(slot.width, slot.height);
  parent.Arrange(slot);

  Dali::Ui::Extension::View::SetPositionX(ignored, 42.0f);
  Dali::Ui::Extension::View::SetSizeWidth(ignored, 7.0f);

  // Forced miss: the producer ignores the child, and so must the RTL resolver.
  parent.InvalidateArrange();
  parent.Arrange(slot);
  DALI_TEST_EQUALS(ignored.GetProperty<float>(Actor::Property::POSITION_X), 42.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(ignored.GetProperty<float>(Actor::Property::SIZE_WIDTH), 7.0f, TEST_LOCATION);

  // Settled hits are identical and idempotent. The old actor read-back alternated
  // 42 -> 71 -> 42 for parentWidth=120 and childWidth=7.
  for(int pass = 0; pass < 3; ++pass)
  {
    parent.Arrange(slot);
    DALI_TEST_EQUALS(ignored.GetProperty<float>(Actor::Property::POSITION_X), 42.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(ignored.GetProperty<float>(Actor::Property::SIZE_WIDTH), 7.0f, TEST_LOCATION);
  }

  END_TEST;
}

// Anti-spin at depth. The replay writes only actor geometry and mInitialLayoutDone; it
// calls no Invalidate* and registers nothing with the LayoutController, so an idle
// application over a settled subtree reaches a fixed point and stays there.
//
// Non-vacuity (verified by mutation): adding an InvalidateArrange() call to
// ReplayArrangeSubtreeFromCache turns every hit into a scheduled follow-up pass and
// the counters climb frame after frame.
int UtcDaliViewArrangeCacheHitDoesNotScheduleFurtherLayoutSubtreeP(void)
{
  UiTestApplication application;
  tet_infoline("Serving a settled subtree from cache schedules no further layout work");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View mid = CreateCountingContainer(true);
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(60.0f);
  root.Add(mid);

  View leaf = CreateCountingContainer(true);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  SettleLayout(application);

  const int        midBase  = CountingContainerImplOf(mid).GetArrangeCallCount();
  const int        leafBase = CountingContainerImplOf(leaf).GetArrangeCallCount();
  const LayoutRect rootSlot = ActorRectOf(root);
  const LayoutRect midRect  = ActorRectOf(mid);
  const LayoutRect leafRect = ActorRectOf(leaf);
  DALI_TEST_CHECK(midBase > 0);

  // Idle frames: no producer may run.
  for(int frame = 0; frame < 5; ++frame)
  {
    SettleLayout(application);
    DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
    DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);
  }

  // Explicit passes, then idle frames again: the hits raised no new work.
  for(int pass = 0; pass < 5; ++pass)
  {
    root.Arrange(rootSlot);
  }
  SettleLayout(application);
  SettleLayout(application);

  DALI_TEST_EQUALS(CountingContainerImplOf(mid).GetArrangeCallCount(), midBase, TEST_LOCATION);
  DALI_TEST_EQUALS(CountingContainerImplOf(leaf).GetArrangeCallCount(), leafBase, TEST_LOCATION);
  CheckActorRect(mid, midRect, TEST_LOCATION);
  CheckActorRect(leaf, leafRect, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5c: LayoutManager execution policy.
//
// Manager-bearing views use IF_CHANGED by default, enabling unchanged-result
// reuse for the four geometry-only in-library managers. ScrollView explicitly uses
// ALWAYS, and utc-Dali-ScrollView.cpp pins that exception behaviourally.
//
// IF_CHANGED is valid only because a manager's own state is layout-tracked:
// every built-in setter -- StackLayoutManager::SetSpacing and friends -- pairs its
// write with an owner invalidation (LayoutManager::InvalidateOwnerMeasure), so a
// state change always retracts the cached result it would falsify. That closes the
// probe these tests once used (a direct manager write that nothing invalidated), and
// with it the last black-box observable of the skip itself: an IF_CHANGED producer re-run on
// unchanged inputs is result-identical to a served cache, by definition. The skip is
// therefore pinned white-box, with a counting manager, in the internal suite --
// UtcDaliArrangeCacheIfChangedLayoutManagerContainerSkipsProducerP -- and the per-manager
// policies by UtcDaliArrangeCacheInLibraryLayoutManagerPolicyP. What remains
// OBSERVABLE here, and what these two tests pin, is the setter contract itself: a
// direct manager write alone reaches the screen (it schedules the pass and the
// manager honours the new value), and a same-value write moves nothing.
// ---------------------------------------------------------------------------

int UtcDaliViewStackLayoutManagerSetterInvalidatesOwnerP(void)
{
  UiTestApplication application;
  tet_infoline("A StackLayoutManager setter alone re-lays-out its owner; a same-value write moves nothing");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  Dali::UniquePtr<StackLayoutManager> owned(new StackLayoutManager(StackOrientation::VERTICAL, 0.0f));
  StackLayoutManager*                 manager = owned.Get();

  View stack = View::New();
  stack.SetRequestedWidth(120.0f);
  stack.SetRequestedHeight(120.0f);
  stack.AttachLayoutManager(std::move(owned));
  root.Add(stack);

  View first = View::New();
  first.SetRequestedWidth(50.0f);
  first.SetRequestedHeight(30.0f);
  stack.Add(first);

  View second = View::New();
  second.SetRequestedWidth(50.0f);
  second.SetRequestedHeight(30.0f);
  stack.Add(second);

  SettleLayout(application);

  const LayoutRect stackRect = ActorRectOf(stack);

  // The manager really did stack them, so the geometry below is its output.
  DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::POSITION_Y), 30.0f, TEST_LOCATION);

  // The setter ALONE: nothing else invalidates, nothing else is touched. The write
  // must invalidate its owner and schedule the pass by itself; before the owner
  // back-pointer existed this exact sequence left the tree settled on the old
  // spacing indefinitely (and, with the manager declared IF_CHANGED, the arrange cache
  // would have kept serving that stale placement forever).
  manager->SetSpacing(20.0f);
  SettleLayout(application);

  DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::POSITION_Y), 50.0f, TEST_LOCATION);

  // The container's own slot is a function of its FIXED requested size, not of the
  // spacing, so the setter re-laid-out the children without moving the container.
  CheckActorRect(stack, stackRect, TEST_LOCATION);

  // Same-value write: the setter's equality guard makes it a no-op, so the settled
  // geometry is byte-identical.
  const LayoutRect firstRect  = ActorRectOf(first);
  const LayoutRect secondRect = ActorRectOf(second);
  manager->SetSpacing(20.0f);
  SettleLayout(application);

  CheckActorRect(stack, stackRect, TEST_LOCATION);
  CheckActorRect(first, firstRect, TEST_LOCATION);
  CheckActorRect(second, secondRect, TEST_LOCATION);

  END_TEST;
}

// The same statement for a second geometry-free manager, so the setter contract is
// not pinned on StackLayoutManager alone.
int UtcDaliViewFlexLayoutManagerSetterInvalidatesOwnerP(void)
{
  UiTestApplication application;
  tet_infoline("A FlexLayoutManager setter alone re-lays-out its owner; a same-value write moves nothing");

  View root = View::New();
  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  Dali::UniquePtr<FlexLayoutManager> owned(new FlexLayoutManager(
    FlexDirection::ROW, FlexWrap::NO_WRAP, FlexJustify::FLEX_START, FlexAlign::FLEX_START, FlexAlign::FLEX_START));
  FlexLayoutManager*                 manager = owned.Get();

  View flex = View::New();
  flex.SetRequestedWidth(200.0f);
  flex.SetRequestedHeight(100.0f);
  flex.AttachLayoutManager(std::move(owned));
  root.Add(flex);

  View first = View::New();
  first.SetRequestedWidth(40.0f);
  first.SetRequestedHeight(20.0f);
  flex.Add(first);

  View second = View::New();
  second.SetRequestedWidth(40.0f);
  second.SetRequestedHeight(20.0f);
  flex.Add(second);

  SettleLayout(application);

  const LayoutRect flexRect = ActorRectOf(flex);

  // FLEX_START packs them at the start of the 200-wide main axis.
  DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::POSITION_X), 40.0f, TEST_LOCATION);

  // The setter ALONE must invalidate its owner and schedule the pass by itself; the
  // manager then honours the new justification: FLEX_END moves both children to the
  // far end of the main axis.
  manager->SetJustifyContent(FlexJustify::FLEX_END);
  SettleLayout(application);

  DALI_TEST_EQUALS(first.GetProperty<float>(Actor::Property::POSITION_X), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetProperty<float>(Actor::Property::POSITION_X), 160.0f, TEST_LOCATION);
  CheckActorRect(flex, flexRect, TEST_LOCATION);

  // Same-value write: the equality guard makes it a no-op.
  const LayoutRect firstRect  = ActorRectOf(first);
  const LayoutRect secondRect = ActorRectOf(second);
  manager->SetJustifyContent(FlexJustify::FLEX_END);
  SettleLayout(application);

  CheckActorRect(flex, flexRect, TEST_LOCATION);
  CheckActorRect(first, firstRect, TEST_LOCATION);
  CheckActorRect(second, secondRect, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Out-of-band Arrange: the hit/miss equivalence survives a DIRECT public
// Arrange() on a descendant.
//
// An out-of-band child.Arrange() rewrites the very records a parent's cache HIT
// replays the child from, so the parent's entry is retracted at that moment
// (cache-only) and the next parent Arrange MISSES and re-hands the
// parent-derived slot -- after which the hit is live again and byte-identical
// to a forced miss. Without the retraction the hit would keep serving the
// foreign geometry that a forced miss corrects.
// ---------------------------------------------------------------------------

namespace
{
int  gOutOfBandParentArrangeCount = 0;
View gOutOfBandChild;

// A IF_CHANGED producer: arranges its one child at a fixed logical slot derived from
// nothing but constants, then echoes its bounds.
LayoutRect OutOfBandParentArrange(View, const LayoutRect& bounds)
{
  ++gOutOfBandParentArrangeCount;
  if(gOutOfBandChild)
  {
    gOutOfBandChild.Arrange(LayoutRect(20.0f, 0.0f, 50.0f, 10.0f));
  }
  return bounds;
}

} // namespace

int UtcDaliViewOutOfBandChildArrangeMatchesForcedMissP(void)
{
  UiTestApplication application;
  tet_infoline("An out-of-band child.Arrange() retracts the parent entry, so the next hit equals a forced miss");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(120.0f);
  root.Add(parent);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(10.0f);
  parent.Add(child);

  gOutOfBandChild              = child;
  gOutOfBandParentArrangeCount = 0;
  parent.SetArrangeCallback(ArrangeCallback::New(&OutOfBandParentArrange), ArrangePolicy::IF_CHANGED);

  SettleLayout(application);

  const LayoutRect pSlot = ActorRectOf(parent);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);
  const int settledCount = gOutOfBandParentArrangeCount;
  DALI_TEST_CHECK(settledCount > 0);

  // The out-of-band write: a public Arrange straight onto the child.
  child.Arrange(LayoutRect(77.0f, 0.0f, 30.0f, 10.0f));
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 77.0f, TEST_LOCATION);

  // The parent's next same-bounds Arrange must NOT serve the retracted entry: the
  // producer re-runs and restores the parent-derived slot.
  parent.Arrange(pSlot);
  DALI_TEST_EQUALS(gOutOfBandParentArrangeCount, settledCount + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  // The retraction is one miss deep, not a permanent demotion: the entry is live
  // again and the hit reproduces the producer's geometry.
  parent.Arrange(pSlot);
  DALI_TEST_EQUALS(gOutOfBandParentArrangeCount, settledCount + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);

  // And the equivalence the retraction exists to keep: a forced miss lands on
  // exactly the geometry the hit just served.
  parent.InvalidateArrange();
  parent.Arrange(pSlot);
  DALI_TEST_EQUALS(gOutOfBandParentArrangeCount, settledCount + 2, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 50.0f, TEST_LOCATION);

  gOutOfBandChild.Reset();

  END_TEST;
}

// STANDALONE is an accumulation/placement-algorithm boundary, not a promise that a
// parent never places the child. A parent miss still derives the child's slot from
// SetRequestedX/Y and its measured/available extent in ArrangeStandaloneChildren.
// Therefore an arbitrary public child.Arrange() must retract the parent entry just
// like it does for a normal child, or a hit replays the foreign slot while a miss
// restores the requested slot.
int UtcDaliViewOutOfBandStandaloneArrangeMatchesForcedMissP(void)
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
  standalone.SetRequestedY(5.0f);
  standalone.SetRequestedWidth(40.0f);
  standalone.SetRequestedHeight(10.0f);
  parent.Add(standalone);

  SettleLayout(application);

  const LayoutRect parentSlot = ActorRectOf(parent);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);

  // Public, arbitrary bounds: this is not LayoutController's framework-owned
  // standalone self pass.
  standalone.Arrange(LayoutRect(77.0f, 9.0f, 30.0f, 12.0f));
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), 77.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 30.0f, TEST_LOCATION);

  // The retracted parent entry forces one miss, restoring the requested standalone
  // slot that ArrangeStandaloneChildren would always produce on a forced miss.
  parent.Arrange(parentSlot);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_X), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::POSITION_Y), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);

  // Settled hit and explicit miss agree.
  parent.Arrange(parentSlot);
  const LayoutRect hitRect = ActorRectOf(standalone);
  parent.InvalidateArrange();
  parent.Arrange(parentSlot);
  CheckActorRect(standalone, hitRect, TEST_LOCATION);

  END_TEST;
}

// A child that was arranged under one parent and then reparented under a
// producer that never arranges its children must not hold that producer's
// cache hostage. The move retracts the child's arrange RESULT record, so the
// hit gate treats it as never-arranged -- skipped, exactly as the replay and a
// forced miss would leave it -- instead of demanding a cache validity it can
// never regain.
int UtcDaliViewReparentedChildDoesNotBlockIgnoringIfChangedParentCacheP(void)
{
  UiTestApplication application;
  tet_infoline("A reparented child under an ignoring IF_CHANGED producer leaves the parent's cache reachable");

  View root = View::New();
  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(300.0f);
  application.GetScene().Add(root);

  View oldParent = View::New();
  oldParent.SetRequestedWidth(120.0f);
  oldParent.SetRequestedHeight(120.0f);
  root.Add(oldParent);

  View child = View::New();
  child.SetRequestedWidth(20.0f);
  child.SetRequestedHeight(10.0f);
  oldParent.Add(child);

  gCountingArrangeProducerCount = 0;

  View ifChangedParent = View::New();
  ifChangedParent.SetRequestedWidth(100.0f);
  ifChangedParent.SetRequestedHeight(50.0f);
  ifChangedParent.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange), ArrangePolicy::IF_CHANGED);
  root.Add(ifChangedParent);

  SettleLayout(application);
  const LayoutRect bSlot = ActorRectOf(ifChangedParent);

  // Control: settled and childless, the IF_CHANGED producer is served from cache.
  const int c0 = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(c0 > 0);
  ifChangedParent.Arrange(bSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, c0, TEST_LOCATION);

  // Move the once-arranged child under the ignoring producer and settle the
  // add-invalidation away.
  ifChangedParent.Add(child);
  SettleLayout(application);
  const int c1 = gCountingArrangeProducerCount;

  const LayoutRect childRect = ActorRectOf(child);

  // Three same-bounds arranges: every one must be a HIT. Before the result
  // record was retracted on reparent, the gate saw a result-holding child whose
  // cache could never revalidate and refused the hit forever.
  ifChangedParent.Arrange(bSlot);
  ifChangedParent.Arrange(bSlot);
  ifChangedParent.Arrange(bSlot);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, c1, TEST_LOCATION);

  // The hits leave the ignored child exactly where a miss would: untouched.
  CheckActorRect(child, childRect, TEST_LOCATION);

  END_TEST;
}

// INC-A. The BACKGROUND visual is the one ViewDataImpl::GetNaturalSize() reads, so
// registering, replacing or removing it changes this view's natural size -- a measure
// input. Before the fix these paths only asked dali-core for a legacy relayout, which
// does not touch the dali-ui measure cache, so the cache kept serving the size it had
// computed for the previous background forever.
//
// Deterministic without any image I/O: an image visual reports its DESIRED_WIDTH /
// DESIRED_HEIGHT as its natural size while it has no texture, and the view is kept
// off-scene so nothing ever loads.
int UtcDaliViewBackgroundChangeInvalidatesMeasureP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  auto backgroundMap = [](int width, int height)
  {
    Property::Map map;
    map.Insert(Ui::VisualBasePropertyIndex::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::IMAGE));
    map.Insert(Ui::ImageVisualPropertyIndex::URL, "background-image.png");
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, width);
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, height);
    return map;
  };

  // No background at all: GetNaturalSize() is ZERO. This populates the measure cache.
  MeasuredSize bare = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(bare.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(bare.GetHeight(), 0.0f, TEST_LOCATION);

  // Register. Re-measured with the SAME constraints, so a surviving cache entry would
  // still answer 0x0.
  view.SetProperty(Ui::View::Property::BACKGROUND, backgroundMap(120, 60));
  MeasuredSize registered = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(registered.GetWidth(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(registered.GetHeight(), 60.0f, TEST_LOCATION);

  // Replace.
  view.SetProperty(Ui::View::Property::BACKGROUND, backgroundMap(200, 90));
  MeasuredSize replaced = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(replaced.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(replaced.GetHeight(), 90.0f, TEST_LOCATION);

  // Remove: back to ZERO.
  view.ClearBackground();
  MeasuredSize cleared = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(cleared.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cleared.GetHeight(), 0.0f, TEST_LOCATION);

  // The cache is still a cache: re-measuring with nothing changed serves the same
  // answer, so the assertions above are not passing merely because caching is off.
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  END_TEST;
}

// INC-A, arranged-geometry half of the same contract: a background registered after
// the tree has settled must move the view's ARRANGED size too, not just the value
// Measure() returns. Exercised through the real layout pass on-scene.
int UtcDaliViewBackgroundChangeAfterSettleUpdatesArrangedSizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  window.Add(view);

  SettleLayout(application);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);

  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::IMAGE));
  map.Insert(Ui::ImageVisualPropertyIndex::URL, "background-image.png");
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, 140);
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, 70);
  view.SetProperty(Ui::View::Property::BACKGROUND, map);

  SettleLayout(application);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 140.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 70.0f, TEST_LOCATION);

  END_TEST;
}

// ---------------------------------------------------------------------------
// LAYOUT PROCESSING WINDOW / PARK: a public or framework-internal invalidation
// raised from inside Measure/Arrange (or a LayoutFinished emit) performs the full
// dirty/cache/ancestor/root-registration transaction. Pending work and an idle
// wake are deliberately separate states:
//
//   FRESHNESS -- dirty propagation prevents an ancestor cache hit from hiding the
//                invalidated descendant;
//   RETENTION -- the root remains pending until a later ProcessEvents services it;
//   NO WAKE   -- processing-created work never calls RequestProcessEventsOnIdle,
//                so an unconditional callback cannot keep the main loop busy.
//
// LayoutFinished stays deferred while parked work exists. Tests therefore inspect
// TestRenderController directly, then use an explicit independent SendNotification
// to prove that parked work remains serviceable. The warning text itself is not
// asserted (DALI_LOG_ERROR goes to stderr, which this harness does not capture).
// ---------------------------------------------------------------------------

int UtcDaliViewInvalidateMeasureDuringMeasurePassParkedAndIdleN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A measure producer's in-pass invalidation remains pending without requesting another idle ProcessEvents cycle");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gAlwaysInvalidatingMeasureCount = 0;

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&AlwaysInvalidatingMeasure));
  gAlwaysInvalidatingView = view;

  window.Add(view);

  // The normal event-time mount request wakes the first pass. Its callback
  // retains another dirty pass but must not wake ProcessEvents again.
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gAlwaysInvalidatingMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // A separately triggered ProcessEvents services exactly one parked pass. The
  // unconditional producer parks itself again, still without arming an idle wake.
  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gAlwaysInvalidatingMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The pass still did its work: the measured RESULT is published
  // unconditionally (only the cache KEY is withheld), so the view carries the
  // producer's size rather than some pre-pass value.
  DALI_TEST_EQUALS(view.GetMeasuredSize().GetWidth(), ALWAYS_INVALIDATING_MEASURED_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetMeasuredSize().GetHeight(), ALWAYS_INVALIDATING_MEASURED_HEIGHT, TEST_LOCATION);

  gAlwaysInvalidatingView.Reset();
  END_TEST;
}

int UtcDaliViewInvalidateArrangeDuringArrangePassParkedAndIdleN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("An arrange producer's in-pass invalidation remains pending without requesting another idle ProcessEvents cycle");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gAlwaysInvalidatingArrangeCount = 0;

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetArrangeCallback(ArrangeCallback::New(&AlwaysInvalidatingArrange));
  gAlwaysInvalidatingView = view;

  window.Add(view);

  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gAlwaysInvalidatingArrangeCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gAlwaysInvalidatingArrangeCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The arranged RESULT is published unconditionally too, so the producer's
  // returned rect reached the actor even though the cache entry was withheld.
  CheckActorRect(view, ALWAYS_INVALIDATING_ARRANGE_RESULT, TEST_LOCATION);

  gAlwaysInvalidatingView.Reset();
  END_TEST;
}

// Batch-local duplicate regression. A and B are independent layout roots already
// present in the controller's local batch, ordered by depth so A runs first. A's
// producer invalidates B before B's turn. That call must fully invalidate B, but
// B's existing local-batch turn services it: the duplicate member-pending entry
// must be erased immediately before B runs. Otherwise the controller falsely
// remains parked after processing fresh B geometry.
int UtcDaliViewInvalidateOtherViewDuringPassConsumedByItsTurnN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A root invalidated before its current-batch turn is consumed by that turn without a false parked duplicate");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gCrossInvalidationSourceCount = 0;
  gCrossInvalidationTargetCount = 0;

  // A is directly under the Window (depth 1).
  View source = View::New();
  source.SetRequestedWidth(200.0f);
  source.SetRequestedHeight(100.0f);
  source.SetMeasureCallback(MeasureCallback::New(&CrossInvalidatingMeasure));

  // B is under a non-View actor (depth 2), so it is still an independent View
  // layout root but sorting guarantees A precedes it in the same local batch.
  Actor targetHost = Actor::New();
  View target = View::New();
  target.SetRequestedWidth(100.0f);
  target.SetRequestedHeight(50.0f);
  target.SetMeasureCallback(MeasureCallback::New(&CrossInvalidationTargetMeasure));

  gCrossInvalidationTarget = target;

  window.Add(source);
  window.Add(targetHost);
  targetHost.Add(target);

  SendRequestedProcessEvents(application);

  // A invalidated B before B's turn; B's one local turn consumed the dirty state.
  DALI_TEST_EQUALS(gCrossInvalidationSourceCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gCrossInvalidationTargetCount, 1, TEST_LOCATION);

  // No duplicate remains parked and no self-wake was armed: the same batch is
  // genuinely settled and may emit LayoutFinished.
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gCrossInvalidationSourceCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gCrossInvalidationTargetCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  gCrossInvalidationTarget.Reset();
  END_TEST;
}

// A one-shot in-pass invalidation is PARKED, not ignored: it both drops the cache and
// leaves the root pending. The explicitly triggered second pass must recompute, publish
// and settle; a later direct same-constraint Measure then proves that the second result
// was cached.
int UtcDaliViewParkedInPassInvalidationStillDropsCacheP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A parked in-pass invalidation drops the cache and is recomputed by the next independently triggered ProcessEvents");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gCacheDropMeasureCount    = 0;
  gCacheDropLastConstraintW = 0.0f;
  gCacheDropLastConstraintH = 0.0f;

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&CacheDropOnceMeasure));
  gCacheDropView = view;

  window.Add(view);
  SendRequestedProcessEvents(application);

  // The first pass was invalidated after entry, so it cannot publish or settle.
  DALI_TEST_EQUALS(gCacheDropMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // PARK retains the root. An independent ProcessEvents services it; this
  // producer disarms after its first call, so the second pass publishes/settles.
  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gCacheDropMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // Re-issue the framework's OWN constraint, captured by the producer, so what
  // follows is a genuine cache probe and not an accidental change of key.
  const float constraintW = gCacheDropLastConstraintW;
  const float constraintH = gCacheDropLastConstraintH;

  // HIT: the independently triggered recovery pass already recomputed and
  // published this exact key.
  view.Measure(constraintW, constraintH);
  DALI_TEST_EQUALS(gCacheDropMeasureCount, 2, TEST_LOCATION);

  // A second direct probe remains a hit.
  view.Measure(constraintW, constraintH);
  DALI_TEST_EQUALS(gCacheDropMeasureCount, 2, TEST_LOCATION);

  SendIndependentProcessEvents(application);
  DALI_TEST_EQUALS(gCacheDropMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  gCacheDropView.Reset();
  END_TEST;
}

// Framework-internal invalidation follows the same PARK rule. A child added from
// OnArrange fully invalidates and retains its root, but the follow-up is serviced
// only when another ProcessEvents cycle is independently triggered.
int UtcDaliViewInternalInvalidationDuringPassStillSchedulesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("Adding a child from OnArrange parks a complete follow-up without self-waking ProcessEvents");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  IntrusivePtr<ChildAddingContainerViewImpl> impl = ChildAddingContainerViewImpl::New();
  View                                       container(*impl);
  container.SetRequestedWidth(200.0f);
  container.SetRequestedHeight(100.0f);

  window.Add(container);

  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(impl->GetArrangeCallCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  SendIndependentProcessEvents(application);

  // Exactly one retained follow-up: the add invalidated through OnChildAdded,
  // and the explicit external trigger drained it. The second pass adds nothing.
  DALI_TEST_EQUALS(impl->GetArrangeCallCount(), 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The child was measured and arranged by that follow-up.
  View child = impl->GetAddedChild();
  DALI_TEST_CHECK(child);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 10.0f, TEST_LOCATION);

  // ...and then the layout settled: one emit, once the follow-up had drained.
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  END_TEST;
}

// The user-visible shape of PARK, driven through a real component path: a sibling's
// arrange rewrites a Label's TEXT. SetText() re-invalidates the label's measure through
// the framework-internal text path (InvalidateTextMeasure), from INSIDE the arrange
// pass, so the work is parked -- retained as dirty + pending, but with no idle wake.
// The label keeps its stale geometry until the next EXTERNAL event drains the park.
namespace
{
int   gLabelArrangeMutationCount = 0;
Label gLabelArrangeMutationTarget;

LayoutRect LabelMutatingArrange(View, const LayoutRect& bounds)
{
  ++gLabelArrangeMutationCount;
  if(gLabelArrangeMutationCount == 1 && gLabelArrangeMutationTarget)
  {
    gLabelArrangeMutationTarget.SetText("WWWWWWWWWWWWWWWW");
  }
  return bounds;
}
} // namespace

int UtcDaliViewLabelTextChangeDuringArrangeParkedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("Changing a Label's text from a sibling's arrange parks the re-measure without an idle wake");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(400.0f);

  Label label = Label::New("W");
  label.SetRequestedWidth(WRAP_CONTENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  root.Add(label);

  gLabelArrangeMutationCount  = 0;
  gLabelArrangeMutationTarget = label;

  View box = View::New();
  box.SetRequestedWidth(50.0f);
  box.SetRequestedHeight(20.0f);
  box.SetArrangeCallback(ArrangeCallback::New(&LabelMutatingArrange));
  root.Add(box);

  window.Add(root);
  SendRequestedProcessEvents(application);

  // Pass 1 measured the label for "W"; the box's arrange then rewrote the text.
  // That in-pass invalidation is PARKED: the label keeps the short-text geometry,
  // no idle wake was requested, and LayoutFinished stays deferred.
  DALI_TEST_EQUALS(gLabelArrangeMutationCount, 1, TEST_LOCATION);
  const float staleWidth = label.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  DALI_TEST_CHECK(staleWidth > 0.0f);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);

  // The next EXTERNAL event drains the parked work: the label is re-measured for
  // the long text (strictly wider), the layout settles and emits exactly once,
  // and processing still never woke itself. (The box may legitimately serve its
  // unchanged slot from the arrange cache, so its callback count is not pinned.)
  SendIndependentProcessEvents(application);
  const float grownWidth = label.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  DALI_TEST_CHECK(grownWidth > staleWidth);
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  gLabelArrangeMutationTarget.Reset();
  END_TEST;
}


// Value guard, child-reorder half. OnChildOrderChanged rebuilds the View-ONLY child
// sequence out of the actor order, so a reorder among the NON-View actor children
// really does fire the signal while leaving that sequence identical -- dali-core
// suppresses only reorders that move no actor at all. The guard must drop such an
// event without invalidating, and must NOT take a real View reorder with it.
int UtcDaliViewChildOrderUnchangedDoesNotInvalidateP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A reorder that leaves the View-only child order unchanged invalidates nothing");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));

  View a = View::New();
  a.SetRequestedWidth(50.0f);
  a.SetRequestedHeight(40.0f);

  // A plain Actor between the two Views: the piece dali-core can reorder without
  // moving any View. It has to go in through the Integration helper -- View::Add
  // asserts on a non-View child otherwise -- and it is deliberately NOT tracked in
  // the View child container, which is exactly the asymmetry the guard covers.
  Dali::Actor spacer = Dali::Actor::New();

  View b = View::New();
  b.SetRequestedWidth(60.0f);
  b.SetRequestedHeight(30.0f);

  parent.Add(a);
  IntegrationView::AddActorChild(parent, spacer);
  parent.Add(b);
  window.Add(parent);

  SettleLayout(application);
  const int settledMeasures = gPlainMeasureProducerCount;
  const int settledEmits    = emitCount;
  DALI_TEST_CHECK(settledMeasures > 0);
  DALI_TEST_CHECK(settledEmits > 0);

  // Actor order [a, spacer, b] -> [a, b, spacer]. An actor moved, so dali-core
  // emits; the View subsequence is still [a, b], so the guard returns early.
  spacer.RaiseToTop();
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, settledMeasures, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, settledEmits, TEST_LOCATION);

  // Sanity, and the half the guard must not eat: a REAL View reorder
  // ([a, b] -> [b, a]) still invalidates and still re-lays-out.
  Dali::Actor(a).RaiseToTop();
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > settledMeasures);
  DALI_TEST_CHECK(emitCount > settledEmits);

  END_TEST;
}

// Lazy child-order connection. The actor child-order signal is connected at the FIRST
// tracked (View) child add, not in ViewImpl::Initialize, so a leaf that never gains a
// child pays no callback, no connection-pool block and no ConnectionTracker entry.
// Two halves of that have to hold.
//
// (1) A reorder among NON-View actor children while the parent has no View child at
//     all is delivered to nothing. It must be a harmless no-op -- which is the same
//     fact that makes deferring the connection sound: the handler rebuilds the View
//     sequence out of views ALREADY in mChildren, so with none there it compares equal
//     and returns.
// (2) The connection must be live by the time a real View reorder is possible, so the
//     View child order still resyncs to the actor order afterwards. GetChildViewAt is
//     the public read of exactly that sequence.
int UtcDaliViewReorderBeforeFirstViewChildIsHarmlessP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A reorder with no View child yet is a harmless no-op; a later View reorder still resyncs");

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  window.Add(parent);

  // Two plain Actors and not one View: nothing is tracked, so nothing is connected.
  // They have to go in through the Integration helper -- View::Add asserts on a
  // non-View child otherwise.
  Dali::Actor spacerA = Dali::Actor::New();
  Dali::Actor spacerB = Dali::Actor::New();
  IntegrationView::AddActorChild(parent, spacerA);
  IntegrationView::AddActorChild(parent, spacerB);

  SettleLayout(application);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 0u, TEST_LOCATION);

  // The reorder that used to be delivered to a connected-but-idle handler.
  spacerA.RaiseToTop();
  SettleLayout(application);
  DALI_TEST_EQUALS(parent.GetChildViewCount(), 0u, TEST_LOCATION);

  // The first tracked children arrive; this is where the connection is made.
  View a = View::New();
  a.SetRequestedWidth(50.0f);
  a.SetRequestedHeight(40.0f);
  View b = View::New();
  b.SetRequestedWidth(60.0f);
  b.SetRequestedHeight(30.0f);
  parent.Add(a);
  parent.Add(b);
  SettleLayout(application);

  DALI_TEST_EQUALS(parent.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(parent.GetChildViewAt(0) == a);
  DALI_TEST_CHECK(parent.GetChildViewAt(1) == b);

  // A real View reorder must still be observed: [a, b] -> [b, a].
  Dali::Actor(a).RaiseToTop();
  SettleLayout(application);

  DALI_TEST_EQUALS(parent.GetChildViewCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(parent.GetChildViewAt(0) == b);
  DALI_TEST_CHECK(parent.GetChildViewAt(1) == a);

  END_TEST;
}

// Value guard, layout-params half. SetLayoutParams re-writing the params already in
// place has nothing to retract and nothing to schedule. The motivating recurrence is
// ScrollBarImpl::SetVBarBounds, which re-writes its bar's AbsoluteLayoutParams on
// every bar update and needed a hand-rolled epsilon test of its own to keep that
// write from re-arming layout every pass.
int UtcDaliViewSetSameLayoutParamsDoesNotInvalidateP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("Re-writing identical layout params invalidates nothing; a real change still does");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  Dali::UniquePtr<AbsoluteLayoutManager> owned(new AbsoluteLayoutManager());
  root.AttachLayoutManager(std::move(owned));

  View child = View::New();
  child.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));
  root.Add(child);

  window.Add(root);
  SettleLayout(application);

  const LayoutRect firstBounds(10.0f, 20.0f, 60.0f, 40.0f);

  // A real change: it invalidates, schedules, and the child is re-measured at the
  // constraint the manager derives from those bounds.
  int measuresBefore = gPlainMeasureProducerCount;
  int emitsBefore    = emitCount;
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(firstBounds));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  // The SAME value again, as a distinct object, so this tests field-wise equality
  // and not pointer identity. Nothing may move.
  measuresBefore = gPlainMeasureProducerCount;
  emitsBefore    = emitCount;
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(firstBounds));
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, measuresBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, emitsBefore, TEST_LOCATION);

  // A different value must still go through, so the guard is an equality test and
  // not a "the second write is always dropped" latch.
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10.0f, 20.0f, 80.0f, 50.0f)));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  END_TEST;
}

// --- Layout-params identity is EXACT, not tolerant ------------------------
//
// The three tests below drive a change that is smaller than the 0.001f epsilon the
// property setters use, on each of the three params types that carry a float. A
// tolerance in the params comparator would swallow every one of them: the write would
// return early, so the stored value would stay behind AND no re-measure would be
// scheduled. Both halves are asserted, because either one alone could pass by accident
// (an unrelated invalidation can re-run a producer; a getter can read a value that was
// never acted on).

// StackLayoutParams::SetWeight documents 0 as a distinct MODE -- "measured normally"
// rather than "sized from the weight proportion" -- so 0 -> 0.0005 is a mode switch,
// not a rounding difference.
int UtcDaliViewSetStackLayoutParamsSubEpsilonWeightAppliesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A sub-epsilon stack weight change is stored exactly and re-measures the child");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.0f));
  root.Add(child);

  window.Add(root);
  SettleLayout(application);

  StackLayoutParams settled;
  DALI_TEST_CHECK(child.TryGetLayoutParams(settled));
  DALI_TEST_EQUALS(settled.GetWeight(), 0.0f, TEST_LOCATION);

  const int measuresBefore = gPlainMeasureProducerCount;
  const int emitsBefore    = emitCount;

  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.0005f));

  // Stored EXACTLY: not rounded back to 0, not left at the old value.
  StackLayoutParams changed;
  DALI_TEST_CHECK(child.TryGetLayoutParams(changed));
  DALI_TEST_CHECK(changed.GetWeight() == 0.0005f);

  // ...and acted on: the write retracted the measure cache and scheduled a pass.
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  END_TEST;
}

// FlexLayoutParams::SetFlexGrow carries the same 0-is-a-mode contract.
int UtcDaliViewSetFlexLayoutParamsSubEpsilonGrowAppliesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A sub-epsilon flex grow change is stored exactly and re-measures the child");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  FlexLayout root = FlexLayout::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(0.0f));
  root.Add(child);

  window.Add(root);
  SettleLayout(application);

  FlexLayoutParams settled;
  DALI_TEST_CHECK(child.TryGetLayoutParams(settled));
  DALI_TEST_EQUALS(settled.GetFlexGrow(), 0.0f, TEST_LOCATION);

  const int measuresBefore = gPlainMeasureProducerCount;
  const int emitsBefore    = emitCount;

  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(0.0005f));

  FlexLayoutParams changed;
  DALI_TEST_CHECK(child.TryGetLayoutParams(changed));
  DALI_TEST_CHECK(changed.GetFlexGrow() == 0.0005f);

  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  END_TEST;
}

// Under the *_PROPORTIONAL flags an AbsoluteLayoutParams bound is a FRACTION of the
// parent, so its whole meaningful range is 0..1 and 0.0005 of a 200-wide parent is a
// tenth of a pixel -- a real difference the epsilon would have discarded.
int UtcDaliViewSetAbsoluteLayoutParamsSubEpsilonBoundsAppliesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A sub-epsilon proportional bounds change is stored exactly and re-measures the child");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  Dali::UniquePtr<AbsoluteLayoutManager> owned(new AbsoluteLayoutManager());
  root.AttachLayoutManager(std::move(owned));

  View child = View::New();
  child.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.10f, 0.0f, 0.5f, 0.5f))
                          .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  root.Add(child);

  window.Add(root);
  SettleLayout(application);

  AbsoluteLayoutParams settled;
  DALI_TEST_CHECK(child.TryGetLayoutParams(settled));
  DALI_TEST_EQUALS(settled.GetBounds().x, 0.10f, TEST_LOCATION);

  const int measuresBefore = gPlainMeasureProducerCount;
  const int emitsBefore    = emitCount;

  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.1005f, 0.0f, 0.5f, 0.5f))
                          .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));

  AbsoluteLayoutParams changed;
  DALI_TEST_CHECK(child.TryGetLayoutParams(changed));
  DALI_TEST_CHECK(changed.GetBounds().x == 0.1005f);

  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  END_TEST;
}

// The other half of the same contract, for all four params types at once: exact
// equality still means "no-op". Each value is written twice as a DISTINCT object, so
// this tests field-wise equality and not pointer identity, and each type then takes a
// real change so the guard is proven to be an equality test rather than a "the second
// write is always dropped" latch.
int UtcDaliViewSetSameLayoutParamsAllTypesDoNotInvalidateP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("Re-writing identical params of any of the four types invalidates nothing");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  gPlainMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  Dali::UniquePtr<AbsoluteLayoutManager> owned(new AbsoluteLayoutManager());
  root.AttachLayoutManager(std::move(owned));

  View child = View::New();
  child.SetMeasureCallback(MeasureCallback::New(&PlainCountingMeasure));
  root.Add(child);

  window.Add(root);
  SettleLayout(application);

  // --- AbsoluteLayoutParams ---
  const LayoutRect absBounds(10.0f, 20.0f, 60.0f, 40.0f);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(absBounds));
  SettleLayout(application);

  int measuresBefore = gPlainMeasureProducerCount;
  int emitsBefore    = emitCount;
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(absBounds));
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, measuresBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, emitsBefore, TEST_LOCATION);

  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10.0f, 20.0f, 80.0f, 50.0f)));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  // --- FlexLayoutParams ---
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f).SetFlexBasis(30.0f).SetAlignSelf(FlexAlign::CENTER));
  SettleLayout(application);

  measuresBefore = gPlainMeasureProducerCount;
  emitsBefore    = emitCount;
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f).SetFlexBasis(30.0f).SetAlignSelf(FlexAlign::CENTER));
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, measuresBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, emitsBefore, TEST_LOCATION);

  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(2.0f).SetFlexShrink(0.0f).SetFlexBasis(30.0f).SetAlignSelf(FlexAlign::CENTER));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  // --- GridLayoutParams ---
  child.SetLayoutParams(GridLayoutParams::New().SetRow(1u).SetColumn(2u).SetRowSpan(1u).SetColumnSpan(3u));
  SettleLayout(application);

  measuresBefore = gPlainMeasureProducerCount;
  emitsBefore    = emitCount;
  child.SetLayoutParams(GridLayoutParams::New().SetRow(1u).SetColumn(2u).SetRowSpan(1u).SetColumnSpan(3u));
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, measuresBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, emitsBefore, TEST_LOCATION);

  child.SetLayoutParams(GridLayoutParams::New().SetRow(2u).SetColumn(2u).SetRowSpan(1u).SetColumnSpan(3u));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  // --- StackLayoutParams ---
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::CENTER));
  SettleLayout(application);

  measuresBefore = gPlainMeasureProducerCount;
  emitsBefore    = emitCount;
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::CENTER));
  SettleLayout(application);
  DALI_TEST_EQUALS(gPlainMeasureProducerCount, measuresBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(emitCount, emitsBefore, TEST_LOCATION);

  child.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f).SetAlignment(LayoutAlignment::CENTER));
  SettleLayout(application);
  DALI_TEST_CHECK(gPlainMeasureProducerCount > measuresBefore);
  DALI_TEST_CHECK(emitCount > emitsBefore);

  END_TEST;
}

// --- MeasureDefault branches on CONTRIBUTING children, not on any child ----
//
// A standalone child is excluded from its parent's measurement by contract, and the
// accumulation loop skips it. A view whose only children are standalone therefore
// accumulates nothing, so it must measure the same as the same view with no children
// at all -- its background's natural size. Keying the branch on "has any child" sent it
// down the accumulation path instead and published 0 x 0.
int UtcDaliViewMeasureDefaultOnlyStandaloneChildrenUsesBackgroundNaturalSizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A view whose only children are standalone measures its background, like a childless one");

  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::IMAGE));
  map.Insert(Ui::ImageVisualPropertyIndex::URL, "background-image.png");
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, 140);
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, 70);

  // The reference: no children at all.
  View childless = View::New();
  childless.SetRequestedWidth(WRAP_CONTENT);
  childless.SetRequestedHeight(WRAP_CONTENT);
  childless.SetProperty(Ui::View::Property::BACKGROUND, map);
  window.Add(childless);

  // The case under test: the same background, one STANDALONE child.
  View withStandalone = View::New();
  withStandalone.SetRequestedWidth(WRAP_CONTENT);
  withStandalone.SetRequestedHeight(WRAP_CONTENT);
  withStandalone.SetProperty(Ui::View::Property::BACKGROUND, map);
  window.Add(withStandalone);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  standaloneChild.SetRequestedWidth(30.0f);
  standaloneChild.SetRequestedHeight(20.0f);
  withStandalone.Add(standaloneChild);

  SettleLayout(application);

  DALI_TEST_EQUALS(childless.GetProperty<float>(Actor::Property::SIZE_WIDTH), 140.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childless.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 70.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(withStandalone.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                   childless.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(withStandalone.GetProperty<float>(Actor::Property::SIZE_HEIGHT),
                   childless.GetProperty<float>(Actor::Property::SIZE_HEIGHT),
                   TEST_LOCATION);

  END_TEST;
}

// The other side of the same branch: ONE contributing child is enough to select the
// accumulation formula, standalone siblings notwithstanding. Without this the fix
// above could have collapsed the branch to "always use the background".
int UtcDaliViewMeasureDefaultContributingChildUsesAccumulationP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("One contributing child selects the accumulation formula, not the background natural size");

  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::IMAGE));
  map.Insert(Ui::ImageVisualPropertyIndex::URL, "background-image.png");
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, 140);
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, 70);

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetProperty(Ui::View::Property::BACKGROUND, map);
  window.Add(view);

  View standaloneChild = View::New();
  standaloneChild.SetLayoutMode(LayoutMode::STANDALONE);
  standaloneChild.SetRequestedWidth(30.0f);
  standaloneChild.SetRequestedHeight(20.0f);
  view.Add(standaloneChild);

  // The contributing child: x = 10, width = 200, so maxRight = 210.
  View child = View::New();
  child.SetRequestedX(10.0f);
  child.SetRequestedWidth(200.0f);
  child.SetRequestedHeight(40.0f);
  view.Add(child);

  SettleLayout(application);

  // maxRight + pw = 210, clamped by the 480-wide window constraint, so the accumulation
  // formula governs and the background's 140 does NOT.
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 210.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);

  END_TEST;
}

// ─── OnChildAdded: the child's own invalidation is the whole signal ────────
//
// OnChildAdded used to invalidate THIS view's measure a second time, on top of the
// InvalidateMeasure() it issues on the freshly added child. That call was removed,
// and these two tests pin the guarantee it stood in for: the child's walk always
// reaches the new parent, because ResetSubtreeScaleAndLayoutCaches() zeroes the added
// subtree's propagation generation and 0 never equals a live generation.
//
// Non-vacuity note: BOTH of these pass before AND after the removal. That is their
// design intent -- they pin the surviving behaviour, not the removed line.

// The child is fully configured while OFF-SCENE, so every requested-size write it
// receives happens before it has a parent and cannot invalidate anything. The add
// itself is therefore the only event that can re-measure the parent.
int UtcDaliViewAddConfiguredOffSceneChildReMeasuresParentP(void)
{
  UiTestApplication application;
  tet_infoline("Adding an off-scene, pre-configured child re-measures the parent");

  gFirstParentMeasureProducerCount = 0;

  View parent = View::New();
  parent.SetRequestedWidth(WRAP_CONTENT);
  parent.SetRequestedHeight(WRAP_CONTENT);
  parent.SetMeasureCallback(MeasureCallback::New(&FirstParentAccumulatingMeasure));
  application.GetWindow().Add(parent);

  SettleLayout(application);

  const int baseCount = gFirstParentMeasureProducerCount;
  DALI_TEST_CHECK(baseCount > 0);

  // Built and configured with no parent at all: nothing here can reach the parent.
  View child = View::New();
  child.SetRequestedWidth(200.0f);
  child.SetRequestedHeight(80.0f);

  SettleLayout(application);
  DALI_TEST_EQUALS(gFirstParentMeasureProducerCount, baseCount, TEST_LOCATION);

  parent.Add(child);
  SettleLayout(application);

  // The parent's producer ran again...
  DALI_TEST_CHECK(gFirstParentMeasureProducerCount > baseCount);

  // ...and the parent's geometry now follows the child it was given.
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 80.0f, TEST_LOCATION);

  END_TEST;
}

// A reparent inside ONE event batch: the child leaves p1 and joins p2 with no layout
// pass in between, so both chains must be invalidated by the two hooks alone
// (OnChildRemoved on p1, the added child's own walk on p2).
int UtcDaliViewReparentChildInSameBatchReMeasuresNewParentChainP(void)
{
  UiTestApplication application;
  tet_infoline("A same-batch reparent re-measures both the old and the new parent chain");

  gFirstParentMeasureProducerCount  = 0;
  gSecondParentMeasureProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(400.0f);
  root.SetRequestedHeight(300.0f);
  application.GetWindow().Add(root);

  View p1 = View::New();
  p1.SetRequestedWidth(WRAP_CONTENT);
  p1.SetRequestedHeight(WRAP_CONTENT);
  p1.SetMeasureCallback(MeasureCallback::New(&FirstParentAccumulatingMeasure));
  root.Add(p1);

  View p2 = View::New();
  p2.SetRequestedWidth(WRAP_CONTENT);
  p2.SetRequestedHeight(WRAP_CONTENT);
  p2.SetMeasureCallback(MeasureCallback::New(&SecondParentAccumulatingMeasure));
  root.Add(p2);

  View child = View::New();
  child.SetRequestedWidth(150.0f);
  child.SetRequestedHeight(90.0f);
  p1.Add(child);

  SettleLayout(application);

  DALI_TEST_EQUALS(p1.GetProperty<float>(Actor::Property::SIZE_WIDTH), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);

  const int p1Base = gFirstParentMeasureProducerCount;
  const int p2Base = gSecondParentMeasureProducerCount;
  DALI_TEST_CHECK(p1Base > 0);
  DALI_TEST_CHECK(p2Base > 0);

  // One event-time batch: Actor::Add reparents, so p1 sees OnChildRemoved and p2 sees
  // OnChildAdded with no pass in between.
  p2.Add(child);
  SettleLayout(application);

  DALI_TEST_CHECK(gSecondParentMeasureProducerCount > p2Base);
  DALI_TEST_CHECK(gFirstParentMeasureProducerCount > p1Base);

  DALI_TEST_EQUALS(p2.GetProperty<float>(Actor::Property::SIZE_WIDTH), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(p2.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(p1.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);

  END_TEST;
}

// ─── SetArrangePolicy value guard ─────────────────────────────────────────
//
// Re-selecting the policy a view already has retracts nothing and schedules nothing.
// Observed behaviourally: with the guard, a same-value write leaves the tree settled
// so the following SettleLayout runs no producer at all. Without it the setter
// invalidated the arrange cache and the counter climbs.
int UtcDaliViewSetSameArrangePolicyKeepsArrangeCacheP(void)
{
  UiTestApplication application;
  tet_infoline("Re-selecting the current arrange policy schedules nothing");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetWindow().Add(root);

  // No callback and no manager, so this view's OWN OnArrange is the active producer
  // and the override policy is the effective one.
  View view = CreateCountingContainer(true);
  view.SetRequestedWidth(120.0f);
  view.SetRequestedHeight(60.0f);
  root.Add(view);

  View sibling = View::New();
  sibling.SetRequestedWidth(30.0f);
  sibling.SetRequestedHeight(30.0f);
  root.Add(sibling);

  SettleLayout(application);

  const int settledCount = CountingContainerImplOf(view).GetArrangeCallCount();
  DALI_TEST_CHECK(settledCount > 0);

  // Same value: nothing is retracted, so nothing is pending and the pass is a no-op.
  CountingContainerImplOf(view).SelectArrangePolicy(ArrangePolicy::IF_CHANGED);
  SettleLayout(application);
  DALI_TEST_EQUALS(CountingContainerImplOf(view).GetArrangeCallCount(), settledCount, TEST_LOCATION);

  // The guard is an equality test, not a latch: a real move still invalidates.
  CountingContainerImplOf(view).SelectArrangePolicy(ArrangePolicy::ALWAYS);
  SettleLayout(application);
  const int afterChange = CountingContainerImplOf(view).GetArrangeCallCount();
  DALI_TEST_CHECK(afterChange > settledCount);

  // ...and under ALWAYS the producer runs on every pass that reaches it.
  sibling.SetRequestedX(11.0f);
  SettleLayout(application);
  DALI_TEST_CHECK(CountingContainerImplOf(view).GetArrangeCallCount() > afterChange);

  END_TEST;
}

// The override is the THIRD-ranked input to the effective producer policy
// (callback > LayoutManager > OnArrange). While a callback is installed, moving the
// override moves nothing the cache predicate can see, so it must retract nothing.
int UtcDaliViewInactiveArrangePolicyChangeKeepsArrangeCacheP(void)
{
  UiTestApplication application;
  tet_infoline("A policy change on an inactive producer keeps the arrange cache entry");

  gCountingArrangeProducerCount = 0;

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetWindow().Add(root);

  View view = CreateCountingContainer(true);
  view.SetRequestedWidth(120.0f);
  view.SetRequestedHeight(60.0f);
  // The callback REPLACES OnArrange and carries its own policy (IF_CHANGED here).
  view.SetArrangeCallback(ArrangeCallback::New(&CountingLeafArrange));
  root.Add(view);

  View sibling = View::New();
  sibling.SetRequestedWidth(30.0f);
  sibling.SetRequestedHeight(30.0f);
  root.Add(sibling);

  SettleLayout(application);

  const int callbackBase = gCountingArrangeProducerCount;
  DALI_TEST_CHECK(callbackBase > 0);

  // ALWAYS is a real move of the OVERRIDE, but the callback outranks it, so the
  // effective producer policy is unchanged and nothing may be retracted.
  CountingContainerImplOf(view).SelectArrangePolicy(ArrangePolicy::ALWAYS);
  SettleLayout(application);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, callbackBase, TEST_LOCATION);

  // A pass that sweeps past for an unrelated reason still serves the entry.
  sibling.SetRequestedX(11.0f);
  SettleLayout(application);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, callbackBase, TEST_LOCATION);

  // Clearing the callback hands the view back to OnArrange, and the override that
  // was inactive above now governs: ALWAYS means every pass runs the producer.
  view.SetArrangeCallback({});
  SettleLayout(application);
  const int ownBase = CountingContainerImplOf(view).GetArrangeCallCount();

  sibling.SetRequestedX(12.0f);
  SettleLayout(application);
  DALI_TEST_CHECK(CountingContainerImplOf(view).GetArrangeCallCount() > ownBase);
  DALI_TEST_EQUALS(gCountingArrangeProducerCount, callbackBase, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// The C++ requested-size setters write the member directly rather than going
// through Handle::SetProperty, and replay dali-core's two OBSERVABLE tails
// themselves: the OnPropertySet virtual, then the guarded PropertySetSignal
// emit. These tests pin BOTH tails, their ORDER, and the pin that keeps the
// view alive across them.
// =============================================================================

namespace
{
// Ordered trace of the two OBSERVABLE tails: 1 = OnPropertySet virtual, 2 = signal emit.
std::vector<int>             gTailOrder;
std::vector<Property::Index> gRecordedIndices;
std::vector<float>           gRecordedValues;

class PropertySetRecordingViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<PropertySetRecordingViewImpl> New()
  {
    return IntrusivePtr<PropertySetRecordingViewImpl>(new PropertySetRecordingViewImpl());
  }

protected:
  PropertySetRecordingViewImpl()
  : ViewImpl()
  {
  }

  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override
  {
    if(index == Ui::View::Property::REQUESTED_WIDTH || index == Ui::View::Property::REQUESTED_HEIGHT)
    {
      float value = 0.0f;
      propertyValue.Get(value);
      gTailOrder.push_back(1);
      gRecordedIndices.push_back(index);
      gRecordedValues.push_back(value);
    }
    // Chain: the LAYOUT_DIRECTION / CLIPPING_MODE cases live in the base.
    ViewImpl::OnPropertySet(index, propertyValue);
  }
};

// Register so TypeInfo lookup can walk the chain.
Dali::TypeRegistration propertySetRecordingViewTypeReg(
  typeid(PropertySetRecordingViewImpl), typeid(ViewImpl), nullptr);

struct TailOrderWitness : public Dali::ConnectionTracker
{
  void OnSet(Dali::Handle /*handle*/, Property::Index index, const Property::Value& /*value*/)
  {
    if(index == Ui::View::Property::REQUESTED_WIDTH || index == Ui::View::Property::REQUESTED_HEIGHT)
    {
      gTailOrder.push_back(2);
    }
  }
};

// Mirrors View::New(), including the explicit second-phase Initialize().
View CreatePropertySetRecordingView()
{
  IntrusivePtr<PropertySetRecordingViewImpl> impl = PropertySetRecordingViewImpl::New();
  View                                       handle(*impl);
  impl->Initialize();
  return handle;
}

void ResetTailCaptures()
{
  gTailOrder.clear();
  gRecordedIndices.clear();
  gRecordedValues.clear();
}
} // namespace

int UtcDaliViewSetRequestedWidthCallsOnPropertySetBeforeSignalP(void)
{
  UiTestApplication application;
  tet_infoline("The C++ requested-size setter replays BOTH core tails: the OnPropertySet "
               "virtual once with the original value, and then the signal emit");

  ResetTailCaptures();

  View             view = CreatePropertySetRecordingView();
  TailOrderWitness witness;
  Dali::Handle     handle = view;
  handle.PropertySetSignal().Connect(&witness, &TailOrderWitness::OnSet);

  Ui::GetImpl(view).SetRequestedWidth(120.0f);

  DALI_TEST_EQUALS(gRecordedIndices.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedIndices[0], static_cast<Property::Index>(Ui::View::Property::REQUESTED_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedValues[0], 120.0f, 0.0001f, TEST_LOCATION);
  // OnPropertySet FIRST, emit SECOND -- the order dali-core's Object::SetProperty uses.
  DALI_TEST_EQUALS(gTailOrder.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[1], 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightCallsOnPropertySetBeforeSignalP(void)
{
  UiTestApplication application;
  tet_infoline("The height setter replays the same two tails in the same order");

  ResetTailCaptures();

  View             view = CreatePropertySetRecordingView();
  TailOrderWitness witness;
  Dali::Handle     handle = view;
  handle.PropertySetSignal().Connect(&witness, &TailOrderWitness::OnSet);

  Ui::GetImpl(view).SetRequestedHeight(80.0f);

  DALI_TEST_EQUALS(gRecordedIndices.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedIndices[0], static_cast<Property::Index>(Ui::View::Property::REQUESTED_HEIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedValues[0], 80.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[1], 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRequestedWidthPropertyCallsOnPropertySetBeforeSignalP(void)
{
  UiTestApplication application;
  tet_infoline("The property route produces the SAME two tails in the same order, which "
               "is what makes the C++ setter's replay equivalent");

  ResetTailCaptures();

  View             view = CreatePropertySetRecordingView();
  TailOrderWitness witness;
  Dali::Handle     handle = view;
  handle.PropertySetSignal().Connect(&witness, &TailOrderWitness::OnSet);

  view.SetProperty(Ui::View::Property::REQUESTED_WIDTH, 120.0f);

  DALI_TEST_EQUALS(gRecordedIndices.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedIndices[0], static_cast<Property::Index>(Ui::View::Property::REQUESTED_WIDTH), TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedValues[0], 120.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[0], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gTailOrder[1], 2, TEST_LOCATION);
  END_TEST;
}

namespace
{
bool gDropReached           = false; // the override ran and released the handle
bool gEmitAfterDrop         = false; // the signal fired AFTER that release
bool gDroppingViewDestroyed = false; // the release really was the last one

// THE only application handle to the view while the setter runs. File-static, so the
// assertions below never touch the (by then destroyed) view.
View gDropHolder;

class HandleDroppingViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<HandleDroppingViewImpl> New()
  {
    return IntrusivePtr<HandleDroppingViewImpl>(new HandleDroppingViewImpl());
  }

protected:
  HandleDroppingViewImpl()
  : ViewImpl()
  {
  }

  ~HandleDroppingViewImpl() override
  {
    gDroppingViewDestroyed = true;
  }

  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override
  {
    if(index == Ui::View::Property::REQUESTED_WIDTH && gDropHolder)
    {
      gDropHolder.Reset(); // last APPLICATION reference gone, mid-write
      gDropReached = true;
    }
    ViewImpl::OnPropertySet(index, propertyValue);
  }
};

Dali::TypeRegistration handleDroppingViewTypeReg(
  typeid(HandleDroppingViewImpl), typeid(ViewImpl), nullptr);

struct DropWitness : public Dali::ConnectionTracker
{
  void OnSet(Dali::Handle /*handle*/, Property::Index index, const Property::Value& /*value*/)
  {
    if(index == Ui::View::Property::REQUESTED_WIDTH && gDropReached)
    {
      gEmitAfterDrop = true;
    }
  }
};
} // namespace

int UtcDaliViewSetRequestedWidthSurvivesHandleDropInOnPropertySetP(void)
{
  UiTestApplication application;
  tet_infoline("The C++ requested-size setter pins the view for the whole write, so an "
               "OnPropertySet override may release the last application handle");

  gDropReached = gEmitAfterDrop = gDroppingViewDestroyed = false;
  DropWitness witness;

  {
    IntrusivePtr<HandleDroppingViewImpl> impl = HandleDroppingViewImpl::New();
    gDropHolder = View(*impl);
    impl->Initialize();
  } // `impl` released: the CustomActor owns the impl, gDropHolder owns the CustomActor

  {
    Dali::Handle probe = gDropHolder;
    probe.PropertySetSignal().Connect(&witness, &DropWitness::OnSet);
  } // a signal connection holds no reference -- gDropHolder is again the ONLY handle

  // Raw-impl entry: no handle of the caller's own is alive for the duration. This is
  // exactly the call shape the pin protects. The view is deliberately OFF-SCENE, so
  // no window holds a reference either.
  Ui::GetImpl(gDropHolder).SetRequestedWidth(120.0f);

  // The view is GONE from here on. Read only the witnesses -- never gDropHolder's impl.
  DALI_TEST_EQUALS(gDropReached, true, TEST_LOCATION);
  DALI_TEST_EQUALS(gEmitAfterDrop, true, TEST_LOCATION);         // it emitted => it was alive
  DALI_TEST_EQUALS(gDroppingViewDestroyed, true, TEST_LOCATION); // the drop WAS the last
  DALI_TEST_CHECK(!gDropHolder);
  END_TEST;
}

// =============================================================================
// Exactly-once emission, and the emitted VALUE. Guaranteed by construction: the
// property route reaches only ApplyRequested*, with dali-core supplying the single
// emit, while the C++ route calls ApplyRequested* plus one NotifyPropertySet.
// =============================================================================

int UtcDaliViewSetRequestedWidthEmitsPropertySetSignalExactlyOnceP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetRequestedWidth(120.0f); // C++ route

  DALI_TEST_EQUALS(recorder.CountOf(Ui::View::Property::REQUESTED_WIDTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.FirstFloatOf(Ui::View::Property::REQUESTED_WIDTH), 120.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSetRequestedHeightEmitsPropertySetSignalExactlyOnceP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  Ui::GetImpl(view).SetRequestedHeight(80.0f); // C++ route

  DALI_TEST_EQUALS(recorder.CountOf(Ui::View::Property::REQUESTED_HEIGHT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.FirstFloatOf(Ui::View::Property::REQUESTED_HEIGHT), 80.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRequestedWidthPropertyEmitsPropertySetSignalExactlyOnceP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  view.SetProperty(Ui::View::Property::REQUESTED_WIDTH, 120.0f); // property route

  DALI_TEST_EQUALS(recorder.CountOf(Ui::View::Property::REQUESTED_WIDTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.FirstFloatOf(Ui::View::Property::REQUESTED_WIDTH), 120.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewRequestedHeightPropertyEmitsPropertySetSignalExactlyOnceP(void)
{
  UiTestApplication   application;
  Ui::View            view = Ui::View::New();
  PropertySetRecorder recorder;
  recorder.Connect(view);

  view.SetProperty(Ui::View::Property::REQUESTED_HEIGHT, 80.0f); // property route

  DALI_TEST_EQUALS(recorder.CountOf(Ui::View::Property::REQUESTED_HEIGHT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.FirstFloatOf(Ui::View::Property::REQUESTED_HEIGHT), 80.0f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

// ─── Remove with no transition alive: the gate skips only dead work ────────
//
// The remove-side zero-transition gate in Internal::ViewDataImpl::Remove skips the
// window lookup, the mChildren find and the ancestor EXIT resolver when no
// LayoutTransition exists anywhere in the process. Everything the caller can observe
// must be unchanged: the child is unparented on the spot (never deferred as an EXIT
// ghost), and the parent re-measures so its WRAP_CONTENT size follows the child set
// it no longer has.
int UtcDaliViewRemoveNoTransitionUnparentsAndReMeasuresParentP(void)
{
  UiTestApplication application;
  tet_infoline("Remove with no LayoutTransition alive unparents at once and re-measures the parent");

  gFirstParentMeasureProducerCount = 0;

  View parent = View::New();
  parent.SetRequestedWidth(WRAP_CONTENT);
  parent.SetRequestedHeight(WRAP_CONTENT);
  parent.SetMeasureCallback(MeasureCallback::New(&FirstParentAccumulatingMeasure));
  application.GetWindow().Add(parent);

  View child = View::New();
  child.SetRequestedWidth(200.0f);
  child.SetRequestedHeight(80.0f);
  parent.Add(child);

  SettleLayout(application);

  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 80.0f, TEST_LOCATION);

  const int baseCount = gFirstParentMeasureProducerCount;
  DALI_TEST_CHECK(baseCount > 0);

  // ANIMATE_EXIT, but with no transition in existence there is no EXIT to animate, so
  // the unparent is synchronous -- asserted BEFORE any frame is pumped.
  parent.Remove(child, RemovePolicy::ANIMATE_EXIT);
  DALI_TEST_EQUALS(parent.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!child.GetParent());

  SettleLayout(application);

  DALI_TEST_CHECK(gFirstParentMeasureProducerCount > baseCount);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);

  END_TEST;
}

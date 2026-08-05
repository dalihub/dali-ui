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
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <stdlib.h>
#include <algorithm>
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

// --- Mid-pass self-invalidation helpers: a producer that invalidates its OWN
// view while that view's pass is running. Dirty is consumed at pass ENTRY, so
// such a re-invalidation is still standing when the pass reaches its publish
// point; the publish is therefore declined and the next call recomputes the
// post-invalidation value instead of serving the pre-invalidation one. ---
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
  View              view = View::New();
  const UiColor     testColor(1.0f, 0.0f, 0.0f, 0.5f);
  view.SetBackgroundColor(testColor);
  DALI_TEST_EQUALS(view.GetBackgroundColor().GetRgba(), testColor.GetRgba(), TEST_LOCATION);
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
    .ShadowOpacity(0.5f, Duration(0.1f));

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
  Extents           margin(10, 20, 30, 40);
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
  Extents           padding(5, 15, 25, 35);
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
  Extents           margin(1, 2, 3, 4);
  view.SetMargin(margin);
  DALI_TEST_EQUALS(view.GetMargin().start, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPaddingChainingP(void)
{
  UiTestApplication application;
  View              view = View::New();
  Extents           padding(5, 10, 15, 20);
  view.SetPadding(padding);
  DALI_TEST_EQUALS(view.GetPadding().start, 5u, TEST_LOCATION);
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
  View              parent = View::New();
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
  View              parent = View::New();
  parent.SetPadding(Extents(10, 10, 10, 10));
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
  viewA.SetMargin(Extents(7, 8, 9, 10));

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

  // SetLayoutDirection invalidates arrange so the next pass mirrors.
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));
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
  view.SetPadding(Extents(5, 15, 25, 35)); // pw = 20, ph = 60

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
  view.SetPadding(Extents(5, 15, 25, 35)); // pw = 20, ph = 60

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
  view.SetPadding(Extents(5, 15, 25, 35)); // pw = 20, ph = 60

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
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::LABEL, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::TAB);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PAGE_TAB, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::SCENE_3D);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::FILLER, TEST_LOCATION);
  DALI_TEST_EQUALS(ViewAccessible::IsScene3D(view), true, TEST_LOCATION);

  view.SetAccessibilityRole(UiAccessibility::Role::DIALOG);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::DIALOG, TEST_LOCATION);
  DALI_TEST_EQUALS(ViewAccessible::IsModal(view), true, TEST_LOCATION);

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
  Extension::View::SetSizeWidth(view, 56.0f);
  Extension::View::SetSizeHeight(view, 78.0f);

  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_X), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::POSITION_Y), 34.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 78.0f, TEST_LOCATION);

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

// A Measure producer that re-invalidates its OWN view mid-pass must not have
// that invalidation wiped by its own pass's cache publish (plan33 3.1, "loss A").
// Dirty is consumed at pass ENTRY, so the mid-pass invalidation is still
// standing at publish time; the publish is declined and the next call with the
// SAME constraint misses and recomputes the post-invalidation value. Without
// this, the pre-invalidation result would be pinned in the cache until some
// unrelated invalidation happened to arrive.
//
// Note that the same Invalidate*() call also poisons the running pass, and the
// poison is an independent second guard on the same outcome. This test pins the
// OUTCOME (recompute, then settle), so it holds whichever guard fires first.
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

// Arrange-axis twin: an ArrangeCallback that calls InvalidateArrange() on its
// own view mid-pass. mArrangeDirty is consumed at pass ENTRY and is no longer
// cleared where the arranged bounds are published, so the invalidation survives
// the pass and the follow-up layout it registered runs the producer again.
// It must also SETTLE: exactly one follow-up, no per-frame re-arrange spin.
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

  // Frame 1: the first arrange pass invalidates its own view mid-pass.
  application.SendNotification();
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 1, TEST_LOCATION);

  // Frame 2: the invalidation was not swallowed -- a follow-up pass runs.
  application.SendNotification();
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 2, TEST_LOCATION);

  // Frames 3+: the producer no longer invalidates, so the layout settles.
  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(gMidPassArrangeProducerCount, 2, TEST_LOCATION);

  gMidPassView.Reset();
  END_TEST;
}

// A measure pass poisoned by same-view re-entrancy declines to publish, but the
// re-entrancy never went through InvalidateMeasure(), so nothing propagated to a
// layout root and nothing registered a follow-up. Without an explicit follow-up
// the view would sit with an invalid cache and no pass scheduled to refill it.
// The pass must therefore register exactly ONE follow-up layout, and that
// follow-up -- which completes cleanly -- must not register another (no spin).
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

  // Frame 1: the pass is poisoned by the re-entrant Measure() and publishes
  // nothing; the producer ran exactly once (the re-entrant call is absorbed).
  application.SendNotification();
  DALI_TEST_EQUALS(gPoisonOnceMeasureProducerCount, 1, TEST_LOCATION);

  // Frame 2: the follow-up layout registered by the poisoned pass runs.
  application.SendNotification();
  DALI_TEST_EQUALS(gPoisonOnceMeasureProducerCount, 2, TEST_LOCATION);

  // Frames 3+: the follow-up completed cleanly, so it published and stopped.
  application.SendNotification();
  application.SendNotification();
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

  application.SendNotification();
  DALI_TEST_EQUALS(gPoisonOnceArrangeProducerCount, 1, TEST_LOCATION);

  application.SendNotification();
  DALI_TEST_EQUALS(gPoisonOnceArrangeProducerCount, 2, TEST_LOCATION);

  application.SendNotification();
  application.SendNotification();
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
// The root's arrange producer is the pass detector: arrange has no cache-hit
// path, so it runs on every pass the controller schedules.
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
  x.SetArrangeCallback(ArrangeCallback::New(&DescendantMeasuringArrange));

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
  v.SetArrangeCallback(ArrangeCallback::New(&DescendantMeasuringArrange));

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

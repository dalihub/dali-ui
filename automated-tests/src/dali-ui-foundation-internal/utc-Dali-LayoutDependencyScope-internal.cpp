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
#include <vector>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace LD = Dali::Ui::Internal::LayoutDependency;

void utc_dali_layout_dependency_scope_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layout_dependency_scope_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// White-box coverage for the layout-dependency owner scopes installed around every
// nested Measure() a producer issues while it is arranging (ArrangeOwnedMeasureScope)
// or while a RecyclerView drives its ItemsLayouter (RecyclerLayoutOwnerScope).
//
// The one consumer, the ancestor-invalidation walk, reads Top() only to choose where
// to stop, so a black-box test can observe THAT a scope exists but never WHICH view it
// names: these tests read Top() directly from inside a MeasureCallback, which is
// exactly the position the walk reads it from.

namespace
{
// ---------------------------------------------------------------------------
// Recording harness
// ---------------------------------------------------------------------------

struct OwnerRecord
{
  bool          hasFrame{false};
  LD::OwnerKind kind{LD::OwnerKind::ARRANGE};
  const void*   owner{nullptr};
  const void*   secondary{nullptr};
  float         widthConstraint{0.0f};
  float         heightConstraint{0.0f};
};

std::vector<OwnerRecord> gRecords;

// Reads the owner stack from exactly the position the ancestor-invalidation walk
// reads it from: inside the measurement that a producer's scope is supposed to claim.
OwnerRecord SnapshotTop(float widthConstraint, float heightConstraint)
{
  OwnerRecord            record;
  const LD::Frame* const frame = LD::Top();
  record.hasFrame              = (frame != nullptr);
  if(frame)
  {
    record.kind      = frame->kind;
    record.owner     = frame->owner;
    record.secondary = frame->secondary;
  }
  record.widthConstraint  = widthConstraint;
  record.heightConstraint = heightConstraint;
  return record;
}

void RecordTop(float widthConstraint, float heightConstraint)
{
  gRecords.push_back(SnapshotTop(widthConstraint, heightConstraint));
}

const float RECORDING_NATURAL_WIDTH  = 80.0f;
const float RECORDING_NATURAL_HEIGHT = 60.0f;

MeasuredSize RecordingClampMeasure(View, float widthConstraint, float heightConstraint)
{
  RecordTop(widthConstraint, heightConstraint);
  const float width  = (widthConstraint >= 0.0f) ? std::min(widthConstraint, RECORDING_NATURAL_WIDTH)
                                                 : RECORDING_NATURAL_WIDTH;
  const float height = (heightConstraint >= 0.0f) ? std::min(heightConstraint, RECORDING_NATURAL_HEIGHT)
                                                  : RECORDING_NATURAL_HEIGHT;
  return MeasuredSize(width, height);
}

size_t CountOwned()
{
  size_t count = 0u;
  for(const auto& record : gRecords)
  {
    if(record.hasFrame)
    {
      ++count;
    }
  }
  return count;
}

void DumpRecords(const char* label)
{
  tet_printf("[%s] %zu recording(s), %zu owned\n", label, gRecords.size(), CountOwned());
  for(size_t i = 0u; i < gRecords.size(); ++i)
  {
    const OwnerRecord& record = gRecords[i];
    tet_printf("  #%zu constraint(%.1f, %.1f) frame=%s kind=%s owner=%p secondary=%p\n",
               i,
               record.widthConstraint,
               record.heightConstraint,
               record.hasFrame ? "yes" : "no",
               record.hasFrame ? (record.kind == LD::OwnerKind::ARRANGE ? "ARRANGE" : "RECYCLER") : "-",
               record.owner,
               record.secondary);
  }
}

// Every frame observed must belong to the expected producer. A wrong owner is as bad
// as a missing scope, so this is checked over ALL owned recordings, not just the last.
bool AllOwnedFramesMatch(LD::OwnerKind kind, const void* owner, const void* secondary)
{
  for(const auto& record : gRecords)
  {
    if(!record.hasFrame)
    {
      continue;
    }
    if(record.kind != kind || record.owner != owner || record.secondary != secondary)
    {
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
// T8 helpers: one owner case is a root(200x100) -> M(fixed 200x100) -> C tree.
// M is deliberately NOT MATCH_PARENT so the enclosing ArrangeDefault of root never
// re-measures it: every owned frame in the recording must then belong to M itself.
// ---------------------------------------------------------------------------

const float OWNER_CASE_ROOT_WIDTH  = 200.0f;
const float OWNER_CASE_ROOT_HEIGHT = 100.0f;

View MakeRecordingChild(bool matchParentWidth)
{
  View child = View::New();
  if(matchParentWidth)
  {
    child.SetRequestedWidth(MATCH_PARENT);
  }
  child.SetMeasureCallback(MeasureCallback::New(&RecordingClampMeasure));
  return child;
}

// Drives two settle passes over root(fixed) -> m -> (already-attached child) and
// asserts that the arrange-phase re-measure was attributed to m.
void CheckArrangeOwner(UiTestApplication& application, Window window, const char* label, View m)
{
  View root = View::New();
  root.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
  root.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
  root.Add(m);
  window.Add(root);

  gRecords.clear();
  application.SendNotification();
  application.SendNotification();

  DumpRecords(label);

  const void* const expectedOwner = &GetImpl(m);

  // The scope was reached at least once: without it Top() would be nullptr here.
  DALI_TEST_CHECK(CountOwned() >= 1u);
  // No stray owner: every frame seen belongs to m, and it is an ARRANGE frame.
  DALI_TEST_CHECK(AllOwnedFramesMatch(LD::OwnerKind::ARRANGE, expectedOwner, nullptr));
  // The arrange phase runs after the measure phase, so the final recording of the
  // pass is the arrange-owned one.
  DALI_TEST_CHECK(!gRecords.empty());
  DALI_TEST_CHECK(gRecords.back().hasFrame);
  DALI_TEST_EQUALS(gRecords.back().owner, expectedOwner, TEST_LOCATION);

  window.Remove(root);
  application.SendNotification();
}

// ---------------------------------------------------------------------------
// T9 helpers: a real RecyclerView + LinearItemsLayouter + adapter.
// ---------------------------------------------------------------------------

const float RECYCLER_ITEM_HEIGHT = 50.0f;
const float RECYCLER_WIDTH       = 200.0f;
const float RECYCLER_HEIGHT      = 400.0f;

uint32_t gAdapterItemCount   = 40u;
int      gCreatedItemViews   = 0;

MeasuredSize RecyclerItemMeasure(View, float widthConstraint, float heightConstraint)
{
  RecordTop(widthConstraint, heightConstraint);
  const float width = (widthConstraint >= 0.0f) ? std::min(widthConstraint, RECYCLER_WIDTH) : RECYCLER_WIDTH;
  return MeasuredSize(width, RECYCLER_ITEM_HEIGHT);
}

uint32_t AdapterItemCount()
{
  return gAdapterItemCount;
}

View AdapterCreateItemView(uint32_t)
{
  ++gCreatedItemViews;
  View item = View::New();
  item.SetRequestedWidth(RECYCLER_WIDTH);
  item.SetRequestedHeight(RECYCLER_ITEM_HEIGHT);
  item.SetMeasureCallback(MeasureCallback::New(&RecyclerItemMeasure));
  return item;
}

void AdapterBindItemView(View, uint32_t)
{
}

// ---------------------------------------------------------------------------
// Narrow-extent probe: root(fixed) -> P(fixed) -> C(MATCH_PARENT) -> G(recording).
//
// P's ArrangeDefault re-measures C inside the site #1 scope and then arranges C
// OUTSIDE it. C's ArrangeCallback issues an out-of-band Measure() on G, so G sees
// the owner stack from two positions: nested inside the scope (during C's measure)
// and after it has closed (during C's arrange).
// ---------------------------------------------------------------------------

std::vector<OwnerRecord> gSpanProbeArrangeRecords; ///< Recorded only from inside C's arrange.
bool                     gSpanProbeInArrange    = false;
int                      gSpanProbeArrangeCount = 0;
View                     gSpanProbeGrandchild;

MeasuredSize SpanProbeGrandchildMeasure(View, float widthConstraint, float heightConstraint)
{
  OwnerRecord record = SnapshotTop(widthConstraint, heightConstraint);
  if(gSpanProbeInArrange)
  {
    gSpanProbeArrangeRecords.push_back(record);
  }
  else
  {
    gRecords.push_back(record);
  }
  const float width  = (widthConstraint >= 0.0f) ? std::min(widthConstraint, RECORDING_NATURAL_WIDTH)
                                                 : RECORDING_NATURAL_WIDTH;
  const float height = (heightConstraint >= 0.0f) ? std::min(heightConstraint, RECORDING_NATURAL_HEIGHT)
                                                  : RECORDING_NATURAL_HEIGHT;
  return MeasuredSize(width, height);
}

LayoutRect SpanProbeArrangeCallback(View, const LayoutRect& bounds)
{
  ++gSpanProbeArrangeCount;
  if(gSpanProbeGrandchild)
  {
    // A constraint that changes every invocation, and that no layout pass ever uses,
    // so this Measure() is always a genuine miss and always reaches the callback.
    const float probeWidth = 17.0f + static_cast<float>(gSpanProbeArrangeCount);
    gSpanProbeInArrange    = true;
    GetImpl(gSpanProbeGrandchild).Measure(probeWidth, 11.0f);
    gSpanProbeInArrange = false;
  }
  return bounds;
}

} // namespace

// The arrange-owned scopes span 15 Measure() statements at 14 scope objects (the grid
// auto-fill scope spans two of the statements). Twelve statements at eleven scope objects
// live in this suite's reach; the cases below drive nine of them -- ViewDataImpl::ArrangeDefault
// and the five LayoutManagers -- reading the owner frame from inside the re-measured child.
// The two remaining in-reach scopes are ArrangeStandaloneChild's pair (the slot-correcting
// re-measure and the MATCH_PARENT placement re-measure), driven by
// UtcDaliLayoutDependencyStandaloneArrangeOwnerIdentityP below. The remaining three
// (CheckBox x2, TextButton x1) live in dali-ui-components and are out of this suite's reach.
int UtcDaliLayoutDependencyArrangeOwnerIdentityP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  // Site #1 — ViewDataImpl::ArrangeDefault.
  {
    View m = View::New();
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.Add(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "ArrangeDefault", m);
  }

  // Site #2 — AbsoluteLayoutManager::Arrange.
  {
    AbsoluteLayout m = AbsoluteLayout::New();
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.Add(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "AbsoluteLayoutManager", m);
  }

  // Site #3 — FlexLayoutManager::Arrange via the threaded ArrangeOneFlexLine owner.
  // FLEX_START keeps the cross extent at the child's own size so the arrange-phase
  // constraint genuinely differs from the measure-phase one.
  {
    FlexLayout m = FlexLayout::New();
    m.SetAlignItems(FlexAlign::FLEX_START);
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.Add(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "FlexLayoutManager", m);
  }

  // Sites #4/#5 — the single scope around MeasureGridChildrenAndFillAuto in
  // GridLayoutManager::Arrange. The grid is left WRAP_CONTENT so its arrange bounds
  // differ from its measure constraint; the child is NOT MATCH_PARENT, so the cells
  // path below cannot be the one that produced the recording.
  {
    GridLayout m = GridLayout::New();
    m.Add(MakeRecordingChild(false));
    CheckArrangeOwner(application, window, "GridLayoutManager (auto-fill helper)", m);
  }

  // Site #6 — ArrangeGridChildrenToCells via its threaded owner. Two star columns make
  // the cell narrower than the measure-phase constraint, so the MATCH_PARENT child is
  // genuinely re-measured while being placed into its cell.
  {
    GridLayout m = GridLayout::New();
    m.AddColumnDefinition(GridLength::Star(1.0f));
    m.AddColumnDefinition(GridLength::Star(1.0f));
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.Add(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "GridLayoutManager (cells)", m);
  }

  // Site #7 — ScrollViewLayoutManager::Arrange.
  {
    ScrollView m = ScrollView::New();
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.SetContent(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "ScrollViewLayoutManager", m);
  }

  // Site #8 — StackLayoutManager::Arrange, weight redistribution (the re-measure
  // guarded by totalWeight > 0). The child carries a StackLayoutParams weight, which
  // is what puts totalWeight above zero. It is deliberately NOT MATCH_PARENT on either
  // axis, so the placement loop below (sites #9/#10) skips its own re-measure and the
  // ONLY owned recording this case can produce is site #8's: drop that scope and
  // CountOwned() falls to zero. The stack itself is left WRAP_CONTENT so its arrange
  // bounds (its own measured size) differ from its measure constraint, making the
  // weight re-measure a genuine cache miss that reaches the MeasureCallback.
  {
    StackLayout m     = StackLayout::New(StackOrientation::VERTICAL);
    View        child = View::New();
    child.SetMeasureCallback(MeasureCallback::New(&RecordingClampMeasure));
    child.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    m.Add(child);
    CheckArrangeOwner(application, window, "StackLayoutManager (weight redistribution)", m);
  }

  // Site #9 — StackLayoutManager::Arrange, vertical branch.
  {
    StackLayout m = StackLayout::New(StackOrientation::VERTICAL);
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    m.Add(MakeRecordingChild(true));
    CheckArrangeOwner(application, window, "StackLayoutManager (vertical)", m);
  }

  // Site #10 — StackLayoutManager::Arrange, horizontal branch.
  {
    StackLayout m = StackLayout::New(StackOrientation::HORIZONTAL);
    m.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    m.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);
    View child = View::New();
    child.SetRequestedHeight(MATCH_PARENT);
    child.SetMeasureCallback(MeasureCallback::New(&RecordingClampMeasure));
    m.Add(child);
    CheckArrangeOwner(application, window, "StackLayoutManager (horizontal)", m);
  }

  gRecords.clear();
  END_TEST;
}

// ArrangeStandaloneChild's two re-measures, which a black-box test cannot attribute:
// the ancestor-invalidation walk returns early for standalone views, so it never reads
// the owner stack on this path and no walk behaviour can distinguish these frames from
// no frame at all. What is pinned here is therefore SCOPE IDENTITY, not walk behaviour --
// that both re-measures are attributed to the arranging PARENT, so that a future consumer
// of the owner stack (or a third-party stop) sees a uniform, correctly-named frame.
//
// Case A drives the MATCH_PARENT placement re-measure, case B the slot-correcting one
// (only reachable with an unconsumed slot, i.e. after an out-of-band Measure()).
int UtcDaliLayoutDependencyStandaloneArrangeOwnerIdentityP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  // Case A -- MATCH_PARENT standalone child, placement re-measure.
  {
    View root = View::New();
    root.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    root.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);

    // WRAP_CONTENT, sized by an ordinary fixed child: the parent's arrange extent
    // (100x40) differs from the constraint it forwards to MeasureStandaloneChildren
    // (200x100), so the placement re-measure is a genuine cache miss and reaches the
    // recording callback.
    View parent = View::New();

    View sizer = View::New();
    sizer.SetRequestedWidth(100.0f);
    sizer.SetRequestedHeight(40.0f);
    parent.Add(sizer);

    View standalone = View::New();
    standalone.SetLayoutMode(LayoutMode::STANDALONE);
    standalone.SetRequestedWidth(MATCH_PARENT);
    standalone.SetRequestedHeight(MATCH_PARENT);
    standalone.SetMeasureCallback(MeasureCallback::New(&RecordingClampMeasure));
    parent.Add(standalone);

    root.Add(parent);
    window.Add(root);

    gRecords.clear();
    application.SendNotification();
    application.SendNotification();

    DumpRecords("ArrangeStandaloneChild (MATCH_PARENT placement)");

    const void* const expectedOwner = &GetImpl(parent);

    DALI_TEST_CHECK(CountOwned() >= 1u);
    DALI_TEST_CHECK(AllOwnedFramesMatch(LD::OwnerKind::ARRANGE, expectedOwner, nullptr));
    // The arrange phase runs last, so the final recording is the arrange-owned one.
    DALI_TEST_CHECK(!gRecords.empty());
    DALI_TEST_CHECK(gRecords.back().hasFrame);
    DALI_TEST_CHECK(gRecords.back().kind == LD::OwnerKind::ARRANGE);
    DALI_TEST_EQUALS(gRecords.back().owner, expectedOwner, TEST_LOCATION);

    window.Remove(root);
    application.SendNotification();
  }

  // Case B -- non-MATCH_PARENT standalone child whose slot was overwritten
  // out-of-band, i.e. the slot-correcting re-measure.
  {
    View root = View::New();
    root.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    root.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);

    View parent = View::New();
    parent.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
    parent.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);

    View standalone = View::New();
    standalone.SetLayoutMode(LayoutMode::STANDALONE);
    standalone.SetMeasureCallback(MeasureCallback::New(&RecordingClampMeasure));
    parent.Add(standalone);

    root.Add(parent);
    window.Add(root);

    application.SendNotification();
    application.SendNotification();

    // Overwrite the slot from outside any pass, then re-arrange only: the sole
    // Measure() the pass can issue on this child is the corrective one.
    standalone.Measure(30.0f, 20.0f);

    gRecords.clear();
    root.InvalidateArrange();
    application.SendNotification();

    DumpRecords("ArrangeStandaloneChild (slot correction)");

    const void* const expectedOwner = &GetImpl(parent);

    DALI_TEST_CHECK(CountOwned() >= 1u);
    DALI_TEST_CHECK(AllOwnedFramesMatch(LD::OwnerKind::ARRANGE, expectedOwner, nullptr));
    DALI_TEST_CHECK(!gRecords.empty());
    DALI_TEST_CHECK(gRecords.back().hasFrame);
    DALI_TEST_CHECK(gRecords.back().kind == LD::OwnerKind::ARRANGE);
    DALI_TEST_EQUALS(gRecords.back().owner, expectedOwner, TEST_LOCATION);

    window.Remove(root);
    application.SendNotification();
  }

  gRecords.clear();
  END_TEST;
}

// The 18 layouter-boundary calls in RecyclerViewImpl are funnelled through
// LayoutChildrenScoped()/ScrollByScoped(), which push a RECYCLER frame naming both the
// recycler and the layouter identity. Every entry family is exercised here.
int UtcDaliLayoutDependencyRecyclerOwnerIdentityP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gAdapterItemCount = 40u;
  gCreatedItemViews = 0;

  ItemAdapter adapter;
  adapter.GetItemCountSignal().Connect(&AdapterItemCount);
  adapter.CreateItemViewSignal().Connect(&AdapterCreateItemView);
  adapter.BindItemViewSignal().Connect(&AdapterBindItemView);

  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);

  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(RECYCLER_WIDTH);
  recycler.SetRequestedHeight(RECYCLER_HEIGHT);
  window.Add(recycler);

  const void* const expectedOwner     = &GetImpl(recycler);
  const void* const expectedSecondary = &layouter.GetImpl();

  auto checkFamily = [&](const char* label) {
    DumpRecords(label);
    DALI_TEST_CHECK(CountOwned() >= 1u);
    DALI_TEST_CHECK(AllOwnedFramesMatch(LD::OwnerKind::RECYCLER, expectedOwner, expectedSecondary));
  };

  // Family 1 — the initial layout driven by SetItemsLayouter/SetAdapter.
  gRecords.clear();
  recycler.SetItemsLayouter(layouter);
  recycler.SetAdapter(adapter);
  application.SendNotification();
  application.SendNotification();
  checkFamily("recycler initial layout");

  // Family 2 — ScrollBy on the non-animated path.
  gRecords.clear();
  recycler.ScrollBy(300.0f, false);
  application.SendNotification();
  checkFamily("recycler ScrollBy");

  // Family 3 — SetScrollOffset.
  gRecords.clear();
  recycler.SetScrollOffset(700.0f);
  application.SendNotification();
  checkFamily("recycler SetScrollOffset");

  // Family 4 — a full data-set change.
  gRecords.clear();
  adapter.NotifyDataSetChanged();
  application.SendNotification();
  checkFamily("recycler NotifyDataSetChanged");

  // Family 5 — a viewport resize, which relayouts from RecyclerViewImpl::OnArrange.
  gRecords.clear();
  recycler.SetRequestedHeight(RECYCLER_HEIGHT * 2.0f);
  application.SendNotification();
  application.SendNotification();
  checkFamily("recycler viewport resize");

  gRecords.clear();
  END_TEST;
}

// The two producer scopes nest, and popping one must restore the ENCLOSING frame
// rather than emptying the stack. The tests above are depth-1 only, so nothing there
// distinguishes "restore previous" from "clear"; this one does, and it also pins the
// inert (owner == nullptr) form, which must leave the stack completely untouched.
int UtcDaliLayoutDependencyScopeNestingAndInertP(void)
{
  UiTestApplication application;

  View recyclerOwner = View::New();
  View arrangeOwner  = View::New();

  // The layouter identity is opaque to the scope: any stable address will do.
  int               layouterIdentity  = 0;
  const void* const expectedSecondary = &layouterIdentity;

  DALI_TEST_CHECK(LD::Top() == nullptr);

  {
    LD::RecyclerLayoutOwnerScope recyclerScope(&GetImpl(recyclerOwner), expectedSecondary);

    const LD::Frame* const recyclerFrame = LD::Top();
    DALI_TEST_CHECK(recyclerFrame != nullptr);
    DALI_TEST_CHECK(recyclerFrame->kind == LD::OwnerKind::RECYCLER);
    DALI_TEST_CHECK(recyclerFrame->owner == &GetImpl(recyclerOwner));
    DALI_TEST_CHECK(recyclerFrame->secondary == expectedSecondary);
    DALI_TEST_CHECK(recyclerFrame->previous == nullptr);

    {
      LD::ArrangeOwnedMeasureScope arrangeScope(&GetImpl(arrangeOwner));

      const LD::Frame* const arrangeFrame = LD::Top();
      DALI_TEST_CHECK(arrangeFrame != nullptr);
      DALI_TEST_CHECK(arrangeFrame != recyclerFrame);
      DALI_TEST_CHECK(arrangeFrame->kind == LD::OwnerKind::ARRANGE);
      DALI_TEST_CHECK(arrangeFrame->owner == &GetImpl(arrangeOwner));
      DALI_TEST_CHECK(arrangeFrame->secondary == nullptr);
      // The inner frame links to the outer one, which is how the dtor can restore it.
      DALI_TEST_CHECK(arrangeFrame->previous == recyclerFrame);

      {
        // Inert: a null owner pushes nothing, so the innermost frame is unchanged for
        // the whole lifetime of the scope object.
        LD::ArrangeOwnedMeasureScope inertScope(nullptr);
        DALI_TEST_CHECK(LD::Top() == arrangeFrame);
      }
      DALI_TEST_CHECK(LD::Top() == arrangeFrame);
    }

    // The mutation target: a dtor that cleared the stack instead of restoring
    // mFrame.previous would leave Top() == nullptr here.
    DALI_TEST_CHECK(LD::Top() == recyclerFrame);
  }

  DALI_TEST_CHECK(LD::Top() == nullptr);

  END_TEST;
}

// A scope must span the owned Measure() and nothing else. The extent is only visible
// from a Measure() issued out-of-band from the following child Arrange(): if the scope
// leaked over that Arrange(), the arranging parent would be blamed for a measurement it
// does not account for. ViewDataImpl::ArrangeDefault is the site under test.
//
// NOTE this is the one case in the suite that deliberately performs an UNSCOPED
// arrange-time Measure() on a direct child (C measures G from C's own arrange
// callback), so in a DEBUG_ENABLED build it trips the walk's scope-completeness
// detector ("no ArrangeOwnedMeasureScope"). That single log line is this test's
// construction, not a missing production scope.
int UtcDaliLayoutDependencyScopeDoesNotSpanChildArrangeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gRecords.clear();
  gSpanProbeArrangeRecords.clear();
  gSpanProbeArrangeCount = 0;
  gSpanProbeInArrange    = false;

  View root = View::New();
  root.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
  root.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);

  // P arranges with ArrangeDefault: it re-measures the MATCH_PARENT child C inside the
  // site #1 scope, then calls C.Arrange() after the scope has closed.
  View parent = View::New();
  parent.SetRequestedWidth(OWNER_CASE_ROOT_WIDTH);
  parent.SetRequestedHeight(OWNER_CASE_ROOT_HEIGHT);

  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetArrangeCallback(ArrangeCallback::New(&SpanProbeArrangeCallback));

  View grandchild = View::New();
  grandchild.SetMeasureCallback(MeasureCallback::New(&SpanProbeGrandchildMeasure));
  gSpanProbeGrandchild = grandchild;

  child.Add(grandchild);
  parent.Add(child);
  root.Add(parent);
  window.Add(root);

  application.SendNotification();
  application.SendNotification();

  DumpRecords("span probe (outside C's arrange)");
  tet_printf("[span probe] C arranged %d time(s), %zu recording(s) taken during C's arrange\n",
             gSpanProbeArrangeCount,
             gSpanProbeArrangeRecords.size());

  // Non-vacuity, part 1: the probe really ran.
  DALI_TEST_CHECK(gSpanProbeArrangeCount >= 1);
  DALI_TEST_CHECK(!gSpanProbeArrangeRecords.empty());

  // Non-vacuity, part 2: the site #1 scope exists and DOES claim the measurements
  // nested under it -- G is measured through C while P holds the scope open. Without
  // this the test would also pass with the scope deleted outright.
  DALI_TEST_CHECK(CountOwned() >= 1u);
  DALI_TEST_CHECK(AllOwnedFramesMatch(LD::OwnerKind::ARRANGE, &GetImpl(parent), nullptr));

  // The assertion under test: once P has moved on to C.Arrange(), the scope is closed,
  // so a Measure() issued from C's arrange callback is unowned. Widening the site #1
  // scope to cover childImpl.Arrange() makes every one of these report P as the owner.
  for(size_t i = 0u; i < gSpanProbeArrangeRecords.size(); ++i)
  {
    const OwnerRecord& record = gSpanProbeArrangeRecords[i];
    tet_printf("  arrange-time #%zu constraint(%.1f, %.1f) frame=%s owner=%p\n",
               i,
               record.widthConstraint,
               record.heightConstraint,
               record.hasFrame ? "yes" : "no",
               record.owner);
    DALI_TEST_CHECK(!record.hasFrame);
  }

  window.Remove(root);
  application.SendNotification();

  gSpanProbeGrandchild.Reset();
  gSpanProbeArrangeRecords.clear();
  gRecords.clear();
  END_TEST;
}

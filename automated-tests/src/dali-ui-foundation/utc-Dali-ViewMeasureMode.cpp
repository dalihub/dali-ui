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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <algorithm>
#include <limits>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

// Measure modes, seen from the public API.
//
// A parent's layout measures a child with one scalar per axis, and pairs it with the meaning of
// that scalar: an upper bound the child may size within (the default), or the size the parent has
// already decided the child will be laid out at. A MATCH_PARENT child, a stretched child (FILL /
// STRETCH) and an explicitly or proportionally sized child are measured at that decided size --
// clamped by their own minimum/maximum, which is the size they are laid out at -- and report it;
// and when a WRAP_CONTENT container's own size is the aggregate of its children, it
// first resolves that size exactly as before and then measures the children whose size it decides
// at their final slot. A child whose WRAP_CONTENT height follows its width -- a multi-line Label,
// a container holding one -- therefore gets the height it needs at the width it is laid out at.
//
// Unless a case says otherwise, every case runs in the default 480 x 800 test window at UI scale 1,
// with no padding or margin. A view added straight to the window is a layout root measured at the
// root layer's 480 x 800.
// H(w) denotes the text height of the wrapping Label at width w, read through
// Label::GetHeightForWidth() after the first settle, so the expectations hold whatever the harness
// font metrics are. Each Label case first checks the precondition that makes it non-vacuous (the
// text wraps differently at the widths involved).

namespace
{
// 78 uppercase letters, wrapped by character: several lines at narrow widths.
const char* const WRAPPING_TEXT = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr float   MODE_EPSILON  = 0.01f;

// Drives one full layout batch to completion.
void SettleLayout(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}

// A multi-line, character-wrapped Label; its height keeps the default WRAP_CONTENT.
Label CreateWrappingLabel(float requestedWidth)
{
  Label label = Label::New(WRAPPING_TEXT);
  label.SetMultiLine(true);
  label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  label.SetRequestedWidth(requestedWidth);
  return label;
}

// The sibling that sets a wrapping parent's width: fixed 100 x 0.
View CreateSeed()
{
  View seed = View::New();
  seed.SetRequestedWidth(100.0f);
  seed.SetRequestedHeight(0.0f);
  return seed;
}

float WidthOf(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

float HeightOf(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

float PositionXOf(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_X);
}

float PositionYOf(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_Y);
}

// A 480-wide, WRAP_CONTENT-height grid with two ABSOLUTE columns and one AUTO row.
GridLayout CreateTwoColumnGrid(float firstColumnWidth, float secondColumnWidth)
{
  GridLayout grid = GridLayout::New();
  grid.SetRequestedWidth(480.0f);
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Absolute(firstColumnWidth));
  columns.PushBack(GridLength::Absolute(secondColumnWidth));
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  return grid;
}

// A custom parent: arranges every child at the origin at its measured size.
LayoutRect ArrangeChildrenAtMeasuredSize(View view, const LayoutRect& bounds)
{
  for(uint32_t i = 0; i < view.GetChildViewCount(); ++i)
  {
    View               child    = view.GetChildViewAt(i);
    const MeasuredSize measured = child.GetMeasuredSize();
    child.Arrange(LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  }
  return bounds;
}

// A custom parent's measure: every child measured with the parent's own constraint, mode-less.
MeasuredSize MeasureChildrenAtConstraint(View view, float widthConstraint, float heightConstraint)
{
  for(uint32_t i = 0; i < view.GetChildViewCount(); ++i)
  {
    view.GetChildViewAt(i).Measure(widthConstraint, heightConstraint);
  }
  return MeasuredSize(widthConstraint, heightConstraint);
}

// A fixed-size leaf producer whose every call is counted. Not a test case: no parameter list.
int gCountingLeafMeasureCount = 0;

MeasuredSize CountingFixedLeafMeasure(View, float, float)
{
  ++gCountingLeafMeasureCount;
  return MeasuredSize(60.0f, 40.0f);
}

// A counted producer that wants 60 wide (less when the constraint is smaller) and 40 high.
MeasuredSize CountingNarrowLeafMeasure(View, float widthConstraint, float)
{
  ++gCountingLeafMeasureCount;
  return MeasuredSize(std::min(widthConstraint, 60.0f), 40.0f);
}

// A counted producer for a 100 x 50 leaf.
MeasuredSize CountingBoxMeasure(View, float, float)
{
  ++gCountingLeafMeasureCount;
  return MeasuredSize(100.0f, 50.0f);
}

// A producer that does NOT follow the MATCH_PARENT contract: it reports up to 200 wide.
MeasuredSize ClampTo200Measure(View, float widthConstraint, float)
{
  return MeasuredSize(std::min(widthConstraint, 200.0f), 10.0f);
}

// A custom WRAP_CONTENT parent's measure: every child measured with the parent's constraint,
// mode-less, and a fixed 100 x 100 for itself.
MeasuredSize MeasureChildrenReport100(View view, float widthConstraint, float heightConstraint)
{
  for(uint32_t i = 0; i < view.GetChildViewCount(); ++i)
  {
    view.GetChildViewAt(i).Measure(widthConstraint, heightConstraint);
  }
  return MeasuredSize(100.0f, 100.0f);
}

// A counted producer for a 50 x 50 leaf.
MeasuredSize CountingSquareMeasure(View, float, float)
{
  ++gCountingLeafMeasureCount;
  return MeasuredSize(50.0f, 50.0f);
}

// A custom parent's arrange: every child at the origin, the parent's full width, its own
// measured height.
LayoutRect ArrangeChildrenAtFullWidth(View view, const LayoutRect& bounds)
{
  for(uint32_t i = 0; i < view.GetChildViewCount(); ++i)
  {
    View child = view.GetChildViewAt(i);
    child.Arrange(LayoutRect(0.0f, 0.0f, bounds.width, child.GetMeasuredSize().height));
  }
  return bounds;
}

// Records which view was measured, in order, and at which width constraint.
struct MeasureRecord
{
  int   id;
  float widthConstraint;
};
std::vector<MeasureRecord> gMeasureRecords;

MeasuredSize RecordingMeasureA(View, float widthConstraint, float)
{
  gMeasureRecords.push_back(MeasureRecord{0, widthConstraint});
  return MeasuredSize(std::min(widthConstraint, 30.0f), 10.0f);
}

MeasuredSize RecordingMeasureB(View, float widthConstraint, float)
{
  gMeasureRecords.push_back(MeasureRecord{1, widthConstraint});
  return MeasuredSize(100.0f, 10.0f);
}

// A plain View with the given explicit size.
View CreateSizedView(float width, float height)
{
  View view = View::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  return view;
}

// A plain View with MATCH_PARENT on both axes.
View CreateMatchParentView()
{
  return CreateSizedView(MATCH_PARENT, MATCH_PARENT);
}

// An echoing arrange callback that counts its calls and records the bounds it received.
int        gRecordingArrangeCount = 0;
LayoutRect gRecordedArrangeBounds;

LayoutRect RecordingEchoArrange(View, const LayoutRect& bounds)
{
  ++gRecordingArrangeCount;
  gRecordedArrangeBounds = bounds;
  return bounds;
}

// An arrange callback that records the width it received and returns a 250-wide rect.
float gWideningReceivedWidth = 0.0f;

LayoutRect WideningArrange(View, const LayoutRect& bounds)
{
  gWideningReceivedWidth = bounds.width;
  return LayoutRect(bounds.x, bounds.y, 250.0f, bounds.height);
}

// A layout animator that counts the CHANGE slots it is given and records their target bounds.
uint32_t   gChangeTargetCount = 0u;
LayoutRect gChangeTarget;

void CaptureChangeTargetAnimator(const LayoutAnimatorContext& context)
{
  if(context.GetSlot() == LayoutTransitionSlot::CHANGE)
  {
    ++gChangeTargetCount;
    gChangeTarget = context.GetToBounds();
  }
}

// A counted producer that does NOT follow the MATCH_PARENT contract: it reports up to 200 wide.
MeasuredSize CountingClampTo200Measure(View, float widthConstraint, float)
{
  ++gCountingLeafMeasureCount;
  return MeasuredSize(std::min(widthConstraint, 200.0f), 10.0f);
}

// A custom parent's arrange: every child at the origin, gArrangeWidth wide, its own measured height.
float gArrangeWidth = 0.0f;

LayoutRect ArrangeChildrenAtGlobalWidth(View view, const LayoutRect& bounds)
{
  for(uint32_t i = 0; i < view.GetChildViewCount(); ++i)
  {
    View child = view.GetChildViewAt(i);
    child.Arrange(LayoutRect(0.0f, 0.0f, gArrangeWidth, child.GetMeasuredSize().height));
  }
  return bounds;
}

// The constraints a recording producer was called with, by producer id, in call order.
struct SizeCall
{
  int   id;
  float width;
  float height;
};
std::vector<SizeCall> gSizeCalls;

// Recording producers that follow the MATCH_PARENT contract (they report 0 x 0).
MeasuredSize RecordingZeroMeasure0(View, float widthConstraint, float heightConstraint)
{
  gSizeCalls.push_back(SizeCall{0, widthConstraint, heightConstraint});
  return MeasuredSize(0.0f, 0.0f);
}

MeasuredSize RecordingZeroMeasure1(View, float widthConstraint, float heightConstraint)
{
  gSizeCalls.push_back(SizeCall{1, widthConstraint, heightConstraint});
  return MeasuredSize(0.0f, 0.0f);
}

// The recorded calls of one producer.
std::vector<SizeCall> CallsOf(int id)
{
  std::vector<SizeCall> calls;
  for(const SizeCall& call : gSizeCalls)
  {
    if(call.id == id)
    {
      calls.push_back(call);
    }
  }
  return calls;
}

// A custom parent: every child measured mode-less at its constraint, and laid out at its own
// width with the child's measured height.
View CreateCustomParent(float width, float height)
{
  View parent = CreateSizedView(width, height);
  parent.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  parent.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  return parent;
}

// RecyclerView items: a vertical StackLayout holding a gItemIconWidth x 64 icon above a
// MATCH_PARENT-width wrapping Label, recorded in creation order.
float              gItemIconWidth = 64.0f;
std::vector<View>  gItemViews;
std::vector<Label> gItemTitles;

uint32_t TwoItems()
{
  return 2u;
}

void CreateIconTitleItem(ItemViewHolder& holder)
{
  StackLayout item  = StackLayout::New(StackOrientation::VERTICAL);
  Label       title = CreateWrappingLabel(MATCH_PARENT);
  item.Add(CreateSizedView(gItemIconWidth, 64.0f));
  item.Add(title);
  gItemViews.push_back(item);
  gItemTitles.push_back(title);
  holder.view = item;
}

void BindNothing(ItemViewHolder&)
{
}

// Adds a 480 x 800 vertical RecyclerView of two icon-and-title items to the window.
void AddIconTitleList(Window window)
{
  gItemViews.clear();
  gItemTitles.clear();

  ItemAdapter adapter = ItemAdapter::New();
  adapter.GetItemCountSignal().Connect(&TwoItems);
  adapter.CreateViewHolderSignal().Connect(&CreateIconTitleItem);
  adapter.BindViewHolderSignal().Connect(&BindNothing);

  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(480.0f);
  recycler.SetRequestedHeight(800.0f);
  window.Add(recycler);
  recycler.SetItemsLayouter(LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL));
  recycler.SetAdapter(adapter);
}

// Checks a rect component by component.
void CheckRect(const LayoutRect& rect, float x, float y, float width, float height, const char* location)
{
  DALI_TEST_EQUALS(rect.x, x, MODE_EPSILON, location);
  DALI_TEST_EQUALS(rect.y, y, MODE_EPSILON, location);
  DALI_TEST_EQUALS(rect.width, width, MODE_EPSILON, location);
  DALI_TEST_EQUALS(rect.height, height, MODE_EPSILON, location);
}

} // namespace

void utc_dali_view_measure_mode_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_measure_mode_cleanup(void)
{
  test_return_value = TET_PASS;
}

// MM01. A WRAP_CONTENT x WRAP_CONTENT vertical StackLayout root holding a 100-wide seed and a
// MATCH_PARENT-width Label. The stack resolves its width from the seed (100) and then measures the
// Label exactly at 100: the stack and the Label get H(100), and the Label reports its slot.
int UtcDaliViewMeasureModeWrapStackMatchParentLabelP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  View        seed  = CreateSeed();
  Label       label = CreateWrappingLabel(MATCH_PARENT);
  stack.Add(seed);
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  const float h480 = label.GetHeightForWidth(480.0f);
  DALI_TEST_CHECK(h100 > h480);

  DALI_TEST_EQUALS(WidthOf(stack), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(stack), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(label), 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM02. A stretch chain: the outer WRAP_CONTENT stack resolves 100 from the seed; the inner
// WRAP_CONTENT stack (FILL) holding a MATCH_PARENT Label is narrower than that slot, so it is
// re-measured exactly at 100, which measures its Label exactly at 100.
int UtcDaliViewMeasureModeNestedStretchChainP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
  StackLayout inner = StackLayout::New(StackOrientation::VERTICAL);
  inner.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  Label label = CreateWrappingLabel(MATCH_PARENT);
  inner.Add(label);
  outer.Add(CreateSeed());
  outer.Add(inner);
  window.Add(outer);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(outer), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(outer), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(inner), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(inner), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM03. Plain Views: P (480 wide, WRAP_CONTENT height) <- R (WRAP_CONTENT x WRAP_CONTENT) <-
// [seed, MATCH_PARENT-width Label C]. R resolves 100 from the seed and measures C exactly at 100,
// so R and P are H(100) tall.
int UtcDaliViewMeasureModeDefaultContainersP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = View::New();
  p.SetRequestedWidth(480.0f);
  View  r = View::New();
  Label c = CreateWrappingLabel(MATCH_PARENT);
  r.Add(CreateSeed());
  r.Add(c);
  p.Add(r);
  window.Add(p);

  SettleLayout(application);

  const float h100 = c.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > c.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(p), 480.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(p), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(r), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(r), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM04. A grid with ABSOLUTE columns knows its cells before measuring: MATCH_PARENT Labels are
// measured exactly at their cell (100 and 380), so the AUTO row is H(100) tall.
int UtcDaliViewMeasureModeGridAbsoluteColumnsMatchParentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = CreateTwoColumnGrid(100.0f, 380.0f);
  Label      a    = CreateWrappingLabel(MATCH_PARENT);
  a.SetLayoutParams(GridLayoutParams::New().SetColumn(0).SetHorizontalAlignment(LayoutAlignment::START));
  Label b = CreateWrappingLabel(MATCH_PARENT);
  b.SetLayoutParams(GridLayoutParams::New().SetColumn(1).SetHorizontalAlignment(LayoutAlignment::FILL));
  grid.Add(a);
  grid.Add(b);
  window.Add(grid);

  SettleLayout(application);

  const float h100 = a.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > a.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(HeightOf(grid), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 380.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(b), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM05. A narrow ABSOLUTE cell bounds a WRAP_CONTENT Label at the cell width (it is not
// stretched, START, so it is an upper bound): the Label wraps at 100 and the row is H(100) tall.
int UtcDaliViewMeasureModeGridNarrowCellWrapContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = CreateTwoColumnGrid(100.0f, 380.0f);
  Label      a    = CreateWrappingLabel(WRAP_CONTENT);
  a.SetLayoutParams(GridLayoutParams::New().SetColumn(0).SetHorizontalAlignment(LayoutAlignment::START));
  Label b = CreateWrappingLabel(200.0f);
  b.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
  grid.Add(a);
  grid.Add(b);
  window.Add(grid);

  SettleLayout(application);

  const float h100 = a.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > a.GetHeightForWidth(468.0f));
  DALI_TEST_CHECK(h100 > a.GetHeightForWidth(200.0f));

  DALI_TEST_EQUALS(HeightOf(grid), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM06. A single-line row FlexLayout with a determined width distributes shrink in the measure: the
// Label is shrunk to 200 next to a 200-wide icon that does not shrink, is measured exactly at 200,
// and the flex is H(200) tall.
int UtcDaliViewMeasureModeFlexRowShrinkP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(400.0f);
  View icon = View::New();
  icon.SetRequestedWidth(200.0f);
  icon.SetRequestedHeight(20.0f);
  icon.SetLayoutParams(FlexLayoutParams::New().SetFlexShrink(0.0f));
  Label label = CreateWrappingLabel(WRAP_CONTENT);
  flex.Add(icon);
  flex.Add(label);
  window.Add(flex);

  SettleLayout(application);

  const float h200 = label.GetHeightForWidth(200.0f);
  DALI_TEST_CHECK(h200 > label.GetHeightForWidth(400.0f));

  DALI_TEST_EQUALS(HeightOf(flex), h200, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(label), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h200, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h200, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM07. A WRAP_CONTENT column FlexLayout resolves its cross width from the seed (100) and then
// measures the MATCH_PARENT-width Label exactly at 100; its main extent takes the new height.
int UtcDaliViewMeasureModeFlexColumnMatchParentCrossP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetDirection(FlexDirection::COLUMN);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  flex.Add(CreateSeed());
  flex.Add(label);
  window.Add(flex);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(flex), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(flex), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM08. A stretched (STRETCH by default) WRAP_CONTENT StackLayout in a WRAP_CONTENT column flex
// is re-measured exactly at the resolved cross width, which measures its MATCH_PARENT Label there.
int UtcDaliViewMeasureModeFlexColumnStretchedContainerP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetDirection(FlexDirection::COLUMN);
  StackLayout inner = StackLayout::New(StackOrientation::VERTICAL);
  Label       label = CreateWrappingLabel(MATCH_PARENT);
  inner.Add(label);
  flex.Add(CreateSeed());
  flex.Add(inner);
  window.Add(flex);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(flex), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(flex), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(inner), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(inner), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM09. A WIDTH_PROPORTIONAL child of a WRAP_CONTENT AbsoluteLayout is measured after the width
// is resolved from the other children (100), at half of it, so the layout is H(50) tall.
int UtcDaliViewMeasureModeAbsoluteProportionalWidthP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  AbsoluteLayout layout = AbsoluteLayout::New();
  Label          label  = CreateWrappingLabel(WRAP_CONTENT);
  label.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.0f, 0.0f, 0.5f, -1.0f))
                          .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  layout.Add(CreateSeed());
  layout.Add(label);
  window.Add(layout);

  SettleLayout(application);

  const float h50 = label.GetHeightForWidth(50.0f);
  DALI_TEST_CHECK(h50 > label.GetHeightForWidth(240.0f));

  DALI_TEST_EQUALS(WidthOf(layout), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(layout), h50, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h50, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM10. ScrollView content on its scrolled axes binds nothing; a WRAP_CONTENT stack as content
// still resolves 100 from its seed and measures its MATCH_PARENT Label exactly at 100.
int UtcDaliViewMeasureModeScrollViewContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetRequestedWidth(480.0f);
  scrollView.SetRequestedHeight(800.0f);
  StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
  Label       label   = CreateWrappingLabel(MATCH_PARENT);
  content.Add(CreateSeed());
  content.Add(label);
  scrollView.SetContent(content);
  window.Add(scrollView);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(content), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(content), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM11. A WRAP_CONTENT STANDALONE stack is measured at the same constraint and modes by its
// parent's measure, its parent's placement and its own pass, so all three agree on
// (100, H(100) + 40), and a later invalidation of the parent re-measures nothing below.
int UtcDaliViewMeasureModeStandaloneWrapStackP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetLayoutMode(LayoutMode::STANDALONE);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  View  leaf  = View::New();
  leaf.SetRequestedWidth(60.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingFixedLeafMeasure));
  stack.Add(CreateSeed());
  stack.Add(label);
  stack.Add(leaf);
  parent.Add(stack);
  window.Add(parent);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));
  DALI_TEST_CHECK(gCountingLeafMeasureCount > 0);

  DALI_TEST_EQUALS(WidthOf(stack), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(stack), h100 + 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetMeasuredSize().height, h100 + 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);

  const int leafCount = gCountingLeafMeasureCount;
  parent.InvalidateMeasure();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(stack), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(stack), h100 + 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, leafCount, TEST_LOCATION);

  END_TEST;
}

// MM23. A custom parent (a measure callback) with a MATCH_PARENT size, laid out as a root at the
// window size: its own size is determined, so its MATCH_PARENT child is exact at the constraint the
// parent passes it, after the pass and in an application's own call alike.
int UtcDaliViewMeasureModeCustomMatchParentParentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(MATCH_PARENT);
  parent.SetRequestedHeight(MATCH_PARENT);
  parent.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);
  window.Add(parent);

  SettleLayout(application);

  DALI_TEST_EQUALS(child.GetMeasuredSize().width, 480.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetMeasuredSize().height, 800.0f, MODE_EPSILON, TEST_LOCATION);

  const MeasuredSize again = child.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(again.width, 1000.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 1000.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM24. A custom WRAP_CONTENT parent measured with a budget: its own size is not determined, so
// its MATCH_PARENT child keeps reporting its minimum, as before.
int UtcDaliViewMeasureModeCustomWrapParentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenReport100));
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);
  window.Add(parent);

  SettleLayout(application);

  DALI_TEST_EQUALS(child.GetMeasuredSize().width, 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetMeasuredSize().height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  const MeasuredSize again = child.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(again.width, 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM25. A custom parent with an explicit size: its MATCH_PARENT-width Label is exact at the
// parent's width, and gets the height of that width.
int UtcDaliViewMeasureModeCustomFixedParentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(300.0f);
  parent.SetRequestedHeight(200.0f);
  parent.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  Label label = CreateWrappingLabel(MATCH_PARENT);
  parent.Add(label);
  window.Add(parent);

  SettleLayout(application);

  const float h300 = label.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(h300 > 0.0f);
  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);

  const MeasuredSize again = label.Measure(300.0f, 200.0f);
  DALI_TEST_EQUALS(again.width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, h300, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM13. Reporting. (a) A MATCH_PARENT Label in a 300-wide stack reports its slot (300) as its
// measured width. (b)(c) A view without a View parent is exact on its MATCH_PARENT axes: the
// caller's constraint is its size.
int UtcDaliViewMeasureModeReportingRuleP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  Label inStack = CreateWrappingLabel(MATCH_PARENT);
  stack.Add(inStack);
  window.Add(stack);

  SettleLayout(application);

  const float h300 = inStack.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(h300 > 0.0f);
  DALI_TEST_EQUALS(inStack.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inStack.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);

  View detached = View::New();
  detached.SetRequestedWidth(MATCH_PARENT);
  detached.SetRequestedHeight(MATCH_PARENT);
  const MeasuredSize detachedSize = detached.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(detachedSize.width, 1000.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(detachedSize.height, 1000.0f, MODE_EPSILON, TEST_LOCATION);

  Label              detachedLabel     = CreateWrappingLabel(MATCH_PARENT);
  const MeasuredSize detachedLabelSize = detachedLabel.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(detachedLabelSize.width, 1000.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(detachedLabelSize.height, detachedLabel.GetHeightForWidth(1000.0f), MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM14. An application's own Measure() after a pass derives the modes the parent's layout used,
// so it is served from the pass's cache entry: same size, no producer run.
int UtcDaliViewMeasureModeApplicationCallAfterPassP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  View leaf = View::New();
  leaf.SetRequestedWidth(MATCH_PARENT);
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingNarrowLeafMeasure));
  stack.Add(leaf);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingLeafMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetMeasuredSize().height, 40.0f, MODE_EPSILON, TEST_LOCATION);

  const MeasuredSize again = leaf.Measure(300.0f, 400.0f);
  DALI_TEST_EQUALS(again.width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, 1, TEST_LOCATION);

  END_TEST;
}

// MM15. An explicitly sized axis means the same whatever the parent decides, so its mode is
// canonical: the pass's entry serves the application's call, and FILL does not stretch it.
int UtcDaliViewMeasureModeFixedAxisCanonicalP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  View leaf = View::New();
  leaf.SetRequestedWidth(100.0f);
  leaf.SetRequestedHeight(50.0f);
  leaf.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingBoxMeasure));
  stack.Add(leaf);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingLeafMeasureCount, 1, TEST_LOCATION);
  const MeasuredSize again = leaf.Measure(300.0f, 400.0f);
  DALI_TEST_EQUALS(again.width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(leaf), 100.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM16. The view's own maximum bounds the size its parent assigns: the Label is measured at,
// reports, and is laid out at 120 -- not the 300 slot -- with the height of 120, from the slot's
// start edge.
int UtcDaliViewMeasureModeOwnMaximumBoundsExactSizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  label.SetMaximumWidth(120.0f);
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);

  const float h120 = label.GetHeightForWidth(120.0f);
  DALI_TEST_CHECK(h120 > label.GetHeightForWidth(300.0f));

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h120, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h120, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(label), 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM17. On a WRAP_CONTENT axis a proportional child can only be measured once the axis is
// resolved, so it is measured after the others, at its proportion of the resolved width.
int UtcDaliViewMeasureModeProportionalMeasuredLastP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gMeasureRecords.clear();

  AbsoluteLayout layout = AbsoluteLayout::New();
  View           a      = View::New();
  a.SetRequestedHeight(10.0f);
  a.SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetBounds(LayoutRect(0.0f, 0.0f, 0.5f, -1.0f))
                      .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  a.SetMeasureCallback(MeasureCallback::New(&RecordingMeasureA));
  View b = View::New();
  b.SetRequestedWidth(100.0f);
  b.SetRequestedHeight(10.0f);
  b.SetMeasureCallback(MeasureCallback::New(&RecordingMeasureB));
  layout.Add(a);
  layout.Add(b);
  window.Add(layout);

  SettleLayout(application);

  DALI_TEST_EQUALS(static_cast<uint32_t>(gMeasureRecords.size()), 2u, TEST_LOCATION);
  if(gMeasureRecords.size() == 2u)
  {
    DALI_TEST_EQUALS(gMeasureRecords[0].id, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(gMeasureRecords[1].id, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(gMeasureRecords[1].widthConstraint, 50.0f, MODE_EPSILON, TEST_LOCATION);
  }

  END_TEST;
}

// MM18. An ArrangeCallback replaces the stack's own placement, so the stack is a custom parent
// from then on, and installing it re-measures. The WRAP_CONTENT stack was measured with a budget,
// so its MATCH_PARENT Label goes from its slot (100, the seed's width) back to the minimum width.
int UtcDaliViewMeasureModeArrangeCallbackMakesParentLegacyP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  Label       label = CreateWrappingLabel(MATCH_PARENT);
  stack.Add(CreateSeed());
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);
  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));
  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);

  stack.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtMeasuredSize));
  SettleLayout(application);
  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM19. Stretching is a measure input of a single-line FlexLayout: a STRETCH child is measured at
// the cross extent (its MATCH_PARENT grandchild is 100 high); after FLEX_START it is measured at
// its content, where the grandchild has no height to match.
int UtcDaliViewMeasureModeFlexAlignItemsRemeasuresP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(300.0f);
  flex.SetRequestedHeight(100.0f);
  View child = View::New();
  View g     = View::New();
  g.SetRequestedWidth(10.0f);
  g.SetRequestedHeight(MATCH_PARENT);
  child.Add(g);
  flex.Add(child);
  window.Add(flex);

  SettleLayout(application);
  DALI_TEST_EQUALS(g.GetMeasuredSize().height, 100.0f, MODE_EPSILON, TEST_LOCATION);

  flex.SetAlignItems(FlexAlign::FLEX_START);
  SettleLayout(application);
  DALI_TEST_EQUALS(g.GetMeasuredSize().height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM20. A MATCH_PARENT main axis without weight stays an upper bound (the stack sums its main
// extents), so the weight child still gets the whole height, exactly as before.
int UtcDaliViewMeasureModeStackMatchParentMainUnchangedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(200.0f);
  stack.SetRequestedHeight(300.0f);
  View a = View::New();
  a.SetRequestedWidth(MATCH_PARENT);
  a.SetRequestedHeight(MATCH_PARENT);
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  stack.Add(a);
  stack.Add(b);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionYOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(b), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(b), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM21. Grow is distributed in the measure of a single-line flex with a determined width; the
// MATCH_PARENT grow child keeps its share in Arrange, which gives the rest to the other grow child:
// the same widths as before (180 and 120), also on an arrange-only pass.
int UtcDaliViewMeasureModeFlexGrowAssignedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(300.0f);
  flex.SetRequestedHeight(50.0f);
  View a = View::New();
  a.SetRequestedWidth(60.0f);
  a.SetRequestedHeight(20.0f);
  a.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  flex.Add(a);
  flex.Add(b);
  window.Add(flex);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);

  flex.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM22. A MATCH_PARENT producer that reports more than its minimum still widens its WRAP_CONTENT
// parent exactly as before (200), and is then measured exactly at that width.
int UtcDaliViewMeasureModeNonCompliantMatchParentKeepsWidthP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  View        a     = View::New();
  a.SetRequestedWidth(MATCH_PARENT);
  a.SetRequestedHeight(10.0f);
  a.SetMeasureCallback(MeasureCallback::New(&ClampTo200Measure));
  stack.Add(CreateSeed());
  stack.Add(a);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(stack), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 200.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM26. An unbounded budget is not a size: an exact constraint at the float maximum is demoted to
// at-most, so a MATCH_PARENT view reports its minimum, never the budget. (a) Under the custom
// MATCH_PARENT parent of MM23; (b) detached.
int UtcDaliViewMeasureModeUnboundedExactBudgetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(MATCH_PARENT);
  parent.SetRequestedHeight(MATCH_PARENT);
  parent.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  parent.Add(child);
  window.Add(parent);

  SettleLayout(application);

  const float        unbounded   = std::numeric_limits<float>::max();
  const MeasuredSize underCustom = child.Measure(unbounded, unbounded);
  DALI_TEST_EQUALS(underCustom.width, 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(underCustom.height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  View detached = View::New();
  detached.SetRequestedWidth(MATCH_PARENT);
  detached.SetRequestedHeight(MATCH_PARENT);
  const MeasuredSize detachedSize = detached.Measure(unbounded, unbounded);
  DALI_TEST_EQUALS(detachedSize.width, 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(detachedSize.height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM27. An application's call on a MATCH_PARENT layout root with the pass's numbers returns the
// size the pass produced, from the pass's cache entry: no child is measured again.
int UtcDaliViewMeasureModeRootApplicationCallP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(MATCH_PARENT);
  stack.SetRequestedHeight(MATCH_PARENT);
  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(50.0f);
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingSquareMeasure));
  stack.Add(leaf);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(stack.GetMeasuredSize().width, 480.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetMeasuredSize().height, 800.0f, MODE_EPSILON, TEST_LOCATION);
  const int leafCount = gCountingLeafMeasureCount;
  DALI_TEST_CHECK(leafCount > 0);

  const MeasuredSize again = stack.Measure(480.0f, 800.0f);
  DALI_TEST_EQUALS(again.width, 480.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 800.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, leafCount, TEST_LOCATION);

  END_TEST;
}

// MM28. A detached MATCH_PARENT view: the caller's constraint is its size; a negative constraint
// is no size, so it reports its minimum.
int UtcDaliViewMeasureModeDetachedRootP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);

  const MeasuredSize exact = view.Measure(200.0f, 150.0f);
  DALI_TEST_EQUALS(exact.width, 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(exact.height, 150.0f, MODE_EPSILON, TEST_LOCATION);

  const MeasuredSize negative = view.Measure(-1.0f, -1.0f);
  DALI_TEST_EQUALS(negative.width, 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(negative.height, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM29. STAR columns of a grid with an explicit width are cells determined before the children are
// measured: a WRAP_CONTENT Label in the first third is measured at most at its cell (100).
int UtcDaliViewMeasureModeGridStarDeterminedCellP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = GridLayout::New();
  grid.SetRequestedWidth(300.0f);
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Star(1.0f));
  columns.PushBack(GridLength::Star(2.0f));
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  Label label = CreateWrappingLabel(WRAP_CONTENT);
  label.SetLayoutParams(GridLayoutParams::New().SetColumn(0).SetHorizontalAlignment(LayoutAlignment::START));
  grid.Add(label);
  window.Add(grid);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(300.0f));

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(grid), h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM30. STAR columns of a WRAP_CONTENT grid are not determined when the grid measures, so the Label
// is measured at the budget; laid out in a 100-wide cell, it is re-measured at that cell, so its
// measured size is the size it is laid out at (its height bounded by the row).
int UtcDaliViewMeasureModeGridStarUndeterminedCellP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View parent = View::New();
  parent.SetRequestedWidth(300.0f);
  parent.SetRequestedHeight(800.0f);
  parent.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  GridLayout               grid = GridLayout::New();
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Star(1.0f));
  columns.PushBack(GridLength::Star(2.0f));
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  Label label = CreateWrappingLabel(WRAP_CONTENT);
  label.SetLayoutParams(GridLayoutParams::New().SetColumn(0).SetHorizontalAlignment(LayoutAlignment::START));
  grid.Add(label);
  parent.Add(grid);
  window.Add(parent);

  SettleLayout(application);

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, HeightOf(label), MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM31. A stretched wrapping Label never grows past its own maximum: its FILL slot is 300 wide, it
// is measured at, reports and is laid out at 120 with the height of 120.
int UtcDaliViewMeasureModeLabelStretchedOverMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  Label label = CreateWrappingLabel(WRAP_CONTENT);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  label.SetMaximumWidth(120.0f);
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);

  const float h120 = label.GetHeightForWidth(120.0f);
  DALI_TEST_CHECK(h120 > label.GetHeightForWidth(300.0f));

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h120, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h120, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(label), 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM32. A stretched wrapping Label never shrinks below its own minimum: its FILL slot is 100 wide,
// it is measured at, reports and is laid out at 300 -- overflowing the slot from its start edge --
// with the height of 300.
int UtcDaliViewMeasureModeLabelStretchedUnderMinimumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(100.0f);
  stack.SetRequestedHeight(400.0f);
  Label label = CreateWrappingLabel(WRAP_CONTENT);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  label.SetMinimumWidth(300.0f);
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);

  const float h300 = label.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(label.GetHeightForWidth(100.0f) > h300);

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(label), 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM33. A short stretched Label: measured exactly at its 300-wide slot, its height is the height of
// its text at its natural width -- the same value as before.
int UtcDaliViewMeasureModeShortStretchedLabelP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  Label label = Label::New("Short");
  label.SetMultiLine(true);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  stack.Add(label);
  window.Add(stack);

  SettleLayout(application);

  const float naturalWidth = label.GetNaturalSize().width;
  DALI_TEST_CHECK(naturalWidth > 0.0f && naturalWidth < 300.0f);

  DALI_TEST_EQUALS(label.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, label.GetHeightForWidth(naturalWidth), MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetMeasuredSize().height, label.GetHeightForWidth(300.0f), MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM34. A MATCH_PARENT view whose own maximum is smaller than its slot is laid out at its maximum
// from the slot's start edge, its margin kept: the measured size, the arranged bounds and the actor
// geometry agree.
int UtcDaliViewMeasureModeMaximumKeepsStartEdgeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 200.0f);
  View c = CreateMatchParentView();
  c.SetMargin(Insets(10.0f, 0.0f, 20.0f, 0.0f));
  c.SetMaximumWidth(120.0f);
  c.SetMaximumHeight(80.0f);
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(c), 10.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(c), 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 80.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().height, 80.0f, MODE_EPSILON, TEST_LOCATION);
  CheckRect(Ui::GetImpl(c).GetArrangedBounds(), 10.0f, 20.0f, 120.0f, 80.0f, TEST_LOCATION);

  END_TEST;
}

// MM35. Under a RIGHT_TO_LEFT parent the clamped view keeps the logical x of its slot, so the
// mirror keeps the slot's right edge: the parent's width less the start margin.
int UtcDaliViewMeasureModeMaximumRtlKeepsRightEdgeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 200.0f);
  p.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  View c = CreateMatchParentView();
  c.SetMargin(Insets(10.0f, 0.0f, 20.0f, 0.0f));
  c.SetMaximumWidth(120.0f);
  c.SetMaximumHeight(80.0f);
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(c), 170.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(c) + WidthOf(c), 290.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(c), 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 80.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::GetImpl(c).GetArrangedBounds().x, 10.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM36. A minimum larger than the slot makes the view overflow its slot from the start edge, at the
// size it is measured at; the parent keeps its own size.
int UtcDaliViewMeasureModeMinimumOverflowsSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(100.0f, 50.0f);
  View c = CreateMatchParentView();
  c.SetMinimumWidth(300.0f);
  c.SetMinimumHeight(80.0f);
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(c), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(c), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 80.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().height, 80.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(p), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(p), 50.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM37. A container clamped by its own maximum lays its children out inside the clamped bounds: its
// MATCH_PARENT child takes the clamped width and its END child aligns against the clamped edge.
int UtcDaliViewMeasureModeClampedContainerArrangesInsideP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View        p = CreateSizedView(300.0f, 200.0f);
  StackLayout s = StackLayout::New(StackOrientation::VERTICAL);
  s.SetRequestedWidth(MATCH_PARENT);
  s.SetRequestedHeight(MATCH_PARENT);
  s.SetMaximumWidth(120.0f);
  View g = View::New();
  g.SetRequestedWidth(MATCH_PARENT);
  g.SetRequestedHeight(30.0f);
  View h = CreateSizedView(50.0f, 40.0f);
  h.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
  s.Add(g);
  s.Add(h);
  p.Add(s);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(s), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(s), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().height, 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(g), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(g.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(h), 70.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(h), 30.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM38. A stretched (FILL) child of a vertical StackLayout is laid out at its own maximum width from
// its slot's start edge; the stack still advances its cursor by the child's slot.
int UtcDaliViewMeasureModeStackFillMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(300.0f);
  stack.SetRequestedHeight(400.0f);
  View a = View::New();
  a.SetRequestedHeight(20.0f);
  a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  a.SetMaximumWidth(120.0f);
  View b = CreateSizedView(50.0f, 10.0f);
  stack.Add(a);
  stack.Add(b);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().height, 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(b), 20.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM39. A stretched (FILL, the default) child of an ABSOLUTE grid cell is laid out at its own
// maximum width from the cell's start edge; the next column is unchanged.
int UtcDaliViewMeasureModeGridFillCellMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = CreateTwoColumnGrid(100.0f, 380.0f);
  View       a    = View::New();
  a.SetRequestedHeight(20.0f);
  a.SetLayoutParams(GridLayoutParams::New().SetColumn(0));
  a.SetMaximumWidth(60.0f);
  View b = View::New();
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
  grid.Add(a);
  grid.Add(b);
  window.Add(grid);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 380.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(grid), 20.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM40. A STRETCH child of a FlexLayout row is laid out at its own maximum height from the top edge
// of its stretched slot; its sibling is unaffected.
int UtcDaliViewMeasureModeFlexStretchMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(300.0f);
  flex.SetRequestedHeight(100.0f);
  View a = View::New();
  a.SetRequestedWidth(60.0f);
  a.SetMaximumHeight(40.0f);
  View b = CreateSizedView(60.0f, 20.0f);
  flex.Add(a);
  flex.Add(b);
  window.Add(flex);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().height, 40.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM41. A flex-grow child clamped by its own maximum keeps the share the grow distribution gave it as
// its slot: the rest of that slot stays empty and its sibling keeps its own share, on the first pass
// and on an arrange-only pass alike (its clamped report is never read back as its share).
int UtcDaliViewMeasureModeFlexGrowMaximumKeepsSiblingsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(300.0f);
  flex.SetRequestedHeight(50.0f);
  View a = CreateSizedView(60.0f, 20.0f);
  a.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  b.SetMaximumWidth(50.0f);
  flex.Add(a);
  flex.Add(b);
  window.Add(flex);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 50.0f, MODE_EPSILON, TEST_LOCATION);

  flex.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 50.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM42. The same grow child under a flex measured with a budget: there the arrange assigns the
// share, and keeps it for the next arrange-only pass, which places the children identically.
int UtcDaliViewMeasureModeFlexGrowMaximumArrangeOnlyStableP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 50.0f);
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  FlexLayout f = FlexLayout::New();
  View       a = CreateSizedView(60.0f, 20.0f);
  a.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  b.SetMaximumWidth(50.0f);
  f.Add(a);
  f.Add(b);
  p.Add(f);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 50.0f, MODE_EPSILON, TEST_LOCATION);

  f.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 50.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 50.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM43. AbsoluteLayout proportional widths are clamped by the child's own maximum; a proportional
// position, computed from the unclamped width, keeps the start edge of that slot.
int UtcDaliViewMeasureModeAbsoluteProportionalMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(300.0f);
  layout.SetRequestedHeight(200.0f);
  View a = View::New();
  a.SetRequestedHeight(20.0f);
  a.SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetBounds(LayoutRect(0.0f, 0.0f, 0.5f, -1.0f))
                      .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  a.SetMaximumWidth(100.0f);
  View b = View::New();
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetBounds(LayoutRect(1.0f, 30.0f, 0.5f, -1.0f))
                      .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL | AbsoluteLayoutFlags::X_PROPORTIONAL));
  b.SetMaximumWidth(100.0f);
  layout.Add(a);
  layout.Add(b);
  window.Add(layout);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(b), 30.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 100.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM44. A STANDALONE MATCH_PARENT view's slot already carries its own maximum, so the arrange clamp
// changes nothing there: measured and arranged agree, and keep agreeing after the parent re-arranges.
int UtcDaliViewMeasureModeStandaloneMaximumMeasuredEqualsArrangedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(200.0f, 150.0f);
  View s = CreateMatchParentView();
  s.SetLayoutMode(LayoutMode::STANDALONE);
  s.SetMargin(Insets(5.0f, 0.0f, 7.0f, 0.0f));
  s.SetMaximumWidth(60.0f);
  s.SetMaximumHeight(45.0f);
  p.Add(s);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(s), 5.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(s), 7.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(s), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(s), 45.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().width, 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().height, 45.0f, MODE_EPSILON, TEST_LOCATION);

  p.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(s), 5.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(s), 7.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(s), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(s), 45.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().width, 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(s.GetMeasuredSize().height, 45.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM45. A MATCH_PARENT layout root with its own maximum is measured exactly at the window extent
// clamped by it and laid out at that size; an application's call with the window's numbers keys on
// the same clamped constraint, so it is served from the pass's cache entry.
int UtcDaliViewMeasureModeWindowRootMaximumP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  StackLayout r = StackLayout::New(StackOrientation::VERTICAL);
  r.SetRequestedWidth(MATCH_PARENT);
  r.SetRequestedHeight(MATCH_PARENT);
  r.SetMaximumWidth(300.0f);
  r.SetMaximumHeight(500.0f);
  View leaf = CreateSizedView(50.0f, 50.0f);
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingSquareMeasure));
  r.Add(leaf);
  window.Add(r);

  SettleLayout(application);

  DALI_TEST_EQUALS(PositionXOf(r), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(r), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(r), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(r), 500.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(r.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(r.GetMeasuredSize().height, 500.0f, MODE_EPSILON, TEST_LOCATION);
  const int leafCount = gCountingLeafMeasureCount;
  DALI_TEST_CHECK(leafCount > 0);

  const MeasuredSize again = r.Measure(480.0f, 800.0f);
  DALI_TEST_EQUALS(again.width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(again.height, 500.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, leafCount, TEST_LOCATION);

  END_TEST;
}

// MM46. The arrange implementation receives the clamped bounds. The arrange cache keys on the raw
// slot, so the same slot again is a cache hit that returns and re-applies the clamped rect -- also
// over an external size write -- without running the implementation or the producer.
int UtcDaliViewMeasureModeArrangeHitKeepsClampP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;
  gRecordingArrangeCount    = 0;
  gRecordedArrangeBounds    = LayoutRect();

  View p = CreateSizedView(300.0f, 200.0f);
  View c = CreateMatchParentView();
  c.SetMaximumWidth(120.0f);
  c.SetMaximumHeight(80.0f);
  c.SetMeasureCallback(MeasureCallback::New(&CountingFixedLeafMeasure));
  c.SetArrangeCallback(ArrangeCallback::New(&RecordingEchoArrange));
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_CHECK(gRecordingArrangeCount > 0);
  CheckRect(gRecordedArrangeBounds, 0.0f, 0.0f, 120.0f, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 80.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().height, 80.0f, MODE_EPSILON, TEST_LOCATION);

  const int arrangeCount = gRecordingArrangeCount;
  const int measureCount = gCountingLeafMeasureCount;

  Dali::Ui::Extension::View::SetSizeWidth(c, 290.0f);
  const LayoutRect hit = c.Arrange(LayoutRect(0.0f, 0.0f, 300.0f, 200.0f));

  DALI_TEST_EQUALS(gRecordingArrangeCount, arrangeCount, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  CheckRect(hit, 0.0f, 0.0f, 120.0f, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, measureCount, TEST_LOCATION);

  p.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(gRecordingArrangeCount, arrangeCount, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 80.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM47. A rect an arrange implementation returns is applied as is, even outside the view's own
// maximum: the implementation received the clamped bounds, and the measured size stays clamped.
int UtcDaliViewMeasureModeCustomArrangeReturnHonouredP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gWideningReceivedWidth = 0.0f;

  View p = CreateSizedView(300.0f, 200.0f);
  View c = CreateMatchParentView();
  c.SetMaximumWidth(120.0f);
  c.SetArrangeCallback(ArrangeCallback::New(&WideningArrange));
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(gWideningReceivedWidth, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 250.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM48. A minimum above the maximum: the maximum wins, for the measured and the laid-out size alike,
// even where that overflows the slot.
int UtcDaliViewMeasureModeMinimumAboveMaximumMaxWinsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(100.0f, 50.0f);
  View c = CreateMatchParentView();
  c.SetMinimumWidth(200.0f);
  c.SetMaximumWidth(120.0f);
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(c), 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 120.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(c), 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM49. A layout transition animates a clamped view to its clamped rect, not to its slot.
int UtcDaliViewMeasureModeChangeTransitionTargetsClampedRectP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View                 p = CreateSizedView(300.0f, 200.0f);
  LayoutAnimatorTiming timing{Duration(0.2f), AlphaFunction(AlphaFunction::LINEAR), Duration()};
  LayoutTransition     transition = LayoutTransition::New();
  transition.SetChangeAnimator(LayoutAnimatorCallback::New(&CaptureChangeTargetAnimator), timing);
  p.SetLayoutTransition(transition);
  View c = CreateMatchParentView();
  c.SetMaximumWidth(120.0f);
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  gChangeTargetCount = 0u;
  gChangeTarget      = LayoutRect();
  c.SetMaximumWidth(150.0f);
  for(int i = 0; i < 3; ++i)
  {
    application.SendNotification();
    application.Render(16);
  }

  DALI_TEST_CHECK(gChangeTargetCount > 0u);
  DALI_TEST_EQUALS(gChangeTarget.width, 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gChangeTarget.x, 0.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM50. The scroll range follows the content's arranged size: a MATCH_PARENT content whose own
// minimum height is above the viewport is laid out at that minimum and scrolls over all of it.
int UtcDaliViewMeasureModeScrollContentMinimumScrollableP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetRequestedWidth(480.0f);
  scrollView.SetRequestedHeight(400.0f);
  View content = CreateMatchParentView();
  content.SetMinimumHeight(1000.0f);
  scrollView.SetContent(content);
  window.Add(scrollView);

  SettleLayout(application);

  DALI_TEST_EQUALS(HeightOf(content), 1000.0f, MODE_EPSILON, TEST_LOCATION);

  scrollView.ScrollTo(Vector2(0.0f, 10000.0f), false);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 600.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM51. A single-line FlexLayout whose own minimum width is above its explicit width measures and lays
// out its line at the minimum: the grow child takes the free space at that width, also on an
// arrange-only pass.
int UtcDaliViewMeasureModeFlexMinimumAboveRequestP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedWidth(480.0f);
  stack.SetRequestedHeight(800.0f);
  FlexLayout f = FlexLayout::New();
  f.SetRequestedWidth(200.0f);
  f.SetRequestedHeight(50.0f);
  f.SetMinimumWidth(300.0f);
  View a = CreateSizedView(60.0f, 20.0f);
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  f.Add(a);
  f.Add(b);
  stack.Add(f);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(f), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 240.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 240.0f, MODE_EPSILON, TEST_LOCATION);

  f.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(f), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 240.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 240.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM52. An explicitly sized FlexLayout stretched by a wider FILL grid cell distributes its line again
// at the cell's width: the grow child takes the whole free space, also on an arrange-only pass.
int UtcDaliViewMeasureModeFlexInWiderFillCellP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = CreateTwoColumnGrid(400.0f, 80.0f);
  FlexLayout f    = FlexLayout::New();
  f.SetRequestedWidth(300.0f);
  f.SetRequestedHeight(50.0f);
  f.SetLayoutParams(GridLayoutParams::New().SetColumn(0));
  View a = CreateSizedView(60.0f, 20.0f);
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  f.Add(a);
  f.Add(b);
  grid.Add(f);
  window.Add(grid);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(f), 400.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 340.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 340.0f, MODE_EPSILON, TEST_LOCATION);

  f.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(f), 400.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 340.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 340.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM53. The same FlexLayout in a narrower FILL cell distributes its line at the cell's width: the
// fixed child keeps its width and the grow child takes what is left, instead of overflowing.
int UtcDaliViewMeasureModeFlexInNarrowerCellP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = CreateTwoColumnGrid(200.0f, 280.0f);
  FlexLayout f    = FlexLayout::New();
  f.SetRequestedWidth(300.0f);
  f.SetRequestedHeight(50.0f);
  f.SetLayoutParams(GridLayoutParams::New().SetColumn(0));
  View a = CreateSizedView(60.0f, 20.0f);
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  f.Add(a);
  f.Add(b);
  grid.Add(f);
  window.Add(grid);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(f), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(a), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 60.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 140.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 140.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM54. An explicitly sized column FlexLayout stretched taller by a row FlexLayout distributes its line
// again at the stretched height: the grow child fills the rest of the column.
int UtcDaliViewMeasureModeColumnFlexStretchedByRowFlexP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout row = FlexLayout::New();
  row.SetRequestedWidth(480.0f);
  row.SetRequestedHeight(200.0f);
  FlexLayout c = FlexLayout::New();
  c.SetDirection(FlexDirection::COLUMN);
  c.SetRequestedWidth(100.0f);
  c.SetRequestedHeight(100.0f);
  View a = CreateSizedView(50.0f, 20.0f);
  View b = View::New();
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(MATCH_PARENT);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  c.Add(a);
  c.Add(b);
  row.Add(c);
  window.Add(row);

  SettleLayout(application);

  DALI_TEST_EQUALS(HeightOf(c), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(a), 0.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(a), 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionYOf(b), 20.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().height, 180.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM55. A FlexLayout arranged by its parent at a new width on arrange-only passes distributes its line
// at each width as a fresh pass would: the result depends on the width, not on the previous pass.
int UtcDaliViewMeasureModeFlexArrangeOnlyAtNewWidthP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gArrangeWidth = 300.0f;

  View p = CreateSizedView(400.0f, 50.0f);
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtGlobalWidth));
  FlexLayout f = FlexLayout::New();
  View       a = CreateSizedView(60.0f, 20.0f);
  a.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  View b = View::New();
  b.SetRequestedWidth(MATCH_PARENT);
  b.SetRequestedHeight(20.0f);
  b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  f.Add(a);
  f.Add(b);
  p.Add(f);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 120.0f, MODE_EPSILON, TEST_LOCATION);

  gArrangeWidth = 400.0f;
  p.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(a), 230.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 230.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 170.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetMeasuredSize().width, 170.0f, MODE_EPSILON, TEST_LOCATION);

  gArrangeWidth = 300.0f;
  p.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(a), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionXOf(b), 180.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(b), 120.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM56. A stretched container whose content keeps its own size on the stretched axis is measured once
// at its natural size and laid out stretched; re-measuring its flex through a sibling serves it from
// its cache.
int UtcDaliViewMeasureModeStretchedContainerWithoutSizeDependentContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  FlexLayout  flex = FlexLayout::New();
  View        x    = CreateSizedView(50.0f, 100.0f);
  StackLayout c    = StackLayout::New(StackOrientation::VERTICAL);
  View        leaf = View::New();
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingNarrowLeafMeasure));
  c.Add(leaf);
  flex.Add(x);
  flex.Add(c);
  window.Add(flex);

  SettleLayout(application);

  const int n0 = gCountingLeafMeasureCount;
  DALI_TEST_CHECK(n0 > 0);
  DALI_TEST_EQUALS(HeightOf(c), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().height, 40.0f, MODE_EPSILON, TEST_LOCATION);

  x.InvalidateMeasure();
  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingLeafMeasureCount, n0, TEST_LOCATION);

  END_TEST;
}

// MM57. The same for a FILL container in a StackLayout whose width follows a sibling: the container is
// laid out at the stack's width, measured at its natural width, and served from its cache when the
// stack is re-measured.
int UtcDaliViewMeasureModeStackFillContainerWithoutSizeDependentContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  View        seed  = CreateSeed();
  StackLayout c     = StackLayout::New(StackOrientation::HORIZONTAL);
  c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  View leaf = View::New();
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingNarrowLeafMeasure));
  c.Add(leaf);
  stack.Add(seed);
  stack.Add(c);
  window.Add(stack);

  SettleLayout(application);

  const int n0 = gCountingLeafMeasureCount;
  DALI_TEST_CHECK(n0 > 0);
  DALI_TEST_EQUALS(WidthOf(c), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 60.0f, MODE_EPSILON, TEST_LOCATION);

  seed.InvalidateMeasure();
  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingLeafMeasureCount, n0, TEST_LOCATION);

  END_TEST;
}

// MM58. A stretched container whose content follows its width (a MATCH_PARENT Label) is still measured
// exactly at the stretched width, on the first pass and after its flex is re-measured.
int UtcDaliViewMeasureModeStretchedContainerWithSizeDependentContentStaysExactP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetDirection(FlexDirection::COLUMN);
  View        seed  = CreateSeed();
  StackLayout inner = StackLayout::New(StackOrientation::VERTICAL);
  Label       label = CreateWrappingLabel(MATCH_PARENT);
  inner.Add(label);
  flex.Add(seed);
  flex.Add(inner);
  window.Add(flex);

  SettleLayout(application);

  const float h100 = label.GetHeightForWidth(100.0f);
  DALI_TEST_CHECK(h100 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(WidthOf(inner), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(inner), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inner.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inner.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);

  seed.InvalidateMeasure();
  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(inner), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(inner), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h100, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inner.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inner.GetMeasuredSize().height, h100, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM59. A stretched child measured at the flex's own height is re-measured when a sibling's minimum
// makes the line taller: it reports, and is laid out at, the taller line's height.
int UtcDaliViewMeasureModeFlexStretchTallerLineP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  FlexLayout flex = FlexLayout::New();
  flex.SetRequestedWidth(300.0f);
  flex.SetRequestedHeight(100.0f);
  View x = View::New();
  x.SetRequestedWidth(50.0f);
  x.SetMinimumHeight(150.0f);
  Label y = CreateWrappingLabel(150.0f);
  flex.Add(x);
  flex.Add(y);
  window.Add(flex);

  SettleLayout(application);

  DALI_TEST_EQUALS(HeightOf(x), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(y), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(y.GetMeasuredSize().height, 150.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM60. A WRAP_CONTENT child in a STAR column of a grid whose width is not determined keeps the
// measurement of its cell on an arrange-only pass: its producer does not run again.
int UtcDaliViewMeasureModeGridStarUndeterminedArrangeOnlyP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  View parent = CreateSizedView(300.0f, 800.0f);
  parent.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  GridLayout               grid = GridLayout::New();
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Star(1.0f));
  columns.PushBack(GridLength::Star(2.0f));
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  View leaf = View::New();
  leaf.SetLayoutParams(GridLayoutParams::New().SetColumn(0).SetHorizontalAlignment(LayoutAlignment::START));
  leaf.SetMeasureCallback(MeasureCallback::New(&CountingClampTo200Measure));
  grid.Add(leaf);
  parent.Add(grid);
  window.Add(parent);

  SettleLayout(application);

  DALI_TEST_EQUALS(leaf.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(leaf), 100.0f, MODE_EPSILON, TEST_LOCATION);
  const float height = HeightOf(leaf);
  const int   n0     = gCountingLeafMeasureCount;
  DALI_TEST_CHECK(n0 > 0);

  grid.InvalidateArrange();
  SettleLayout(application);

  DALI_TEST_EQUALS(gCountingLeafMeasureCount, n0, TEST_LOCATION);
  DALI_TEST_EQUALS(leaf.GetMeasuredSize().width, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(leaf), 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(leaf), height, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM61. A MATCH_PARENT-width weight child of a StackLayout whose width is not determined is probed at
// the stack's budget, so what it reports widens the stack as before modes existed.
int UtcDaliViewMeasureModeStackWeightMatchCrossProbedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedHeight(400.0f);
  View s = CreateSizedView(30.0f, 10.0f);
  View w = View::New();
  w.SetRequestedWidth(MATCH_PARENT);
  w.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  w.SetMeasureCallback(MeasureCallback::New(&ClampTo200Measure));
  stack.Add(s);
  stack.Add(w);
  window.Add(stack);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(stack), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(w.GetMeasuredSize().width, 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(w), 200.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM62. An explicitly sized view whose own minimum is above its explicit size measures its content
// inside the size it is laid out at: its MATCH_PARENT child is measured once, at that size.
int UtcDaliViewMeasureModeExplicitContainerMeasuresContentAtClampedSizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gCountingLeafMeasureCount = 0;

  View p = CreateSizedView(200.0f, 100.0f);
  p.SetMinimumWidth(300.0f);
  View c = CreateMatchParentView();
  c.SetMeasureCallback(MeasureCallback::New(&CountingFixedLeafMeasure));
  p.Add(c);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(WidthOf(p), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gCountingLeafMeasureCount, 1, TEST_LOCATION);

  END_TEST;
}

// MM63. Weighted cells of a stretched row hold only MATCH_PARENT Labels: the cells are not measured at
// a zero width (they keep their at-most report), the row is measured exactly at the stack's width and
// distributes its weights over it, so every Label gets the height of its 150-wide share.
int UtcDaliViewMeasureModeWeightedCellsOfStretchedRowP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  View        seed  = CreateSizedView(300.0f, 0.0f);
  StackLayout row   = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  StackLayout c1 = StackLayout::New(StackOrientation::VERTICAL);
  c1.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  Label l1 = CreateWrappingLabel(MATCH_PARENT);
  c1.Add(l1);
  StackLayout c2 = StackLayout::New(StackOrientation::VERTICAL);
  c2.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  Label l2 = CreateWrappingLabel(MATCH_PARENT);
  c2.Add(l2);
  row.Add(c1);
  row.Add(c2);
  stack.Add(seed);
  stack.Add(row);
  window.Add(stack);

  SettleLayout(application);

  const float h150 = l1.GetHeightForWidth(150.0f);
  DALI_TEST_CHECK(h150 > 0.0f);
  DALI_TEST_CHECK(h150 > l1.GetHeightForWidth(300.0f));

  DALI_TEST_EQUALS(WidthOf(c1), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(row), h150, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(l1), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(l1), h150, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(l2), 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(l2), h150, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c1.GetMeasuredSize().width, 150.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(c1.GetMeasuredSize().height, h150, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM64. A grid cell whose only content is a MATCH_PARENT Label keeps its at-most report in an AUTO
// column (the Label's height at the grid's width), so its row does not collapse; the Label is laid out
// in the 200-wide column at that height.
int UtcDaliViewMeasureModeMatchContentCellInAutoColumnP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = GridLayout::New();
  grid.SetRequestedWidth(480.0f);
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Auto());
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  Label l0 = CreateWrappingLabel(200.0f);
  l0.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  StackLayout c = StackLayout::New(StackOrientation::VERTICAL);
  c.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(0));
  Label label = CreateWrappingLabel(MATCH_PARENT);
  c.Add(label);
  grid.Add(l0);
  grid.Add(c);
  window.Add(grid);

  SettleLayout(application);

  const float h480 = label.GetHeightForWidth(480.0f);
  DALI_TEST_CHECK(h480 > 0.0f);

  DALI_TEST_EQUALS(c.GetMeasuredSize().height, h480, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), h480, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 200.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h480, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM65. An item whose only content is a MATCH_PARENT Label, under a custom parent that lays it out at
// the parent's width without measuring it exactly: the item keeps its at-most report (the Label's
// height at the parent's width), and its Label is laid out at that width and height.
int UtcDaliViewMeasureModeMatchContentItemUnderCustomParentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 800.0f);
  p.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  View  item  = View::New();
  Label label = CreateWrappingLabel(MATCH_PARENT);
  item.Add(label);
  p.Add(item);
  window.Add(p);

  SettleLayout(application);

  const float h300 = label.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(h300 > 0.0f);

  DALI_TEST_EQUALS(item.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(item), h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h300, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM66. A column FlexLayout whose only child is a MATCH_PARENT-width Label keeps the Label's at-most
// report under a custom parent: the flex is as tall as the Label at the parent's width, not 0.
int UtcDaliViewMeasureModeFlexColumnOnlyMatchContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 800.0f);
  p.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  FlexLayout f = FlexLayout::New();
  f.SetDirection(FlexDirection::COLUMN);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  f.Add(label);
  p.Add(f);
  window.Add(p);

  SettleLayout(application);

  const float h300 = label.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(h300 > 0.0f);

  DALI_TEST_EQUALS(f.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM67. AbsoluteLayouts whose only child is a MATCH_PARENT Label, or a width-proportional one, keep the
// Label's at-most report under a custom parent: the first is as tall as the Label at the parent's
// width, the second as tall as the Label at its proportion of that width.
int UtcDaliViewMeasureModeAbsoluteOnlyMatchOrProportionalContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(300.0f, 800.0f);
  p.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtFullWidth));
  AbsoluteLayout a1 = AbsoluteLayout::New();
  Label          l1 = CreateWrappingLabel(MATCH_PARENT);
  a1.Add(l1);
  AbsoluteLayout a2 = AbsoluteLayout::New();
  Label          l2 = CreateWrappingLabel(WRAP_CONTENT);
  l2.SetLayoutParams(AbsoluteLayoutParams::New()
                       .SetBounds(LayoutRect(0.0f, 0.0f, 0.5f, -1.0f))
                       .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  a2.Add(l2);
  p.Add(a1);
  p.Add(a2);
  window.Add(p);

  SettleLayout(application);

  const float h300 = l1.GetHeightForWidth(300.0f);
  const float h150 = l2.GetHeightForWidth(150.0f);
  DALI_TEST_CHECK(h300 > 0.0f);
  DALI_TEST_CHECK(h150 > 0.0f);

  DALI_TEST_EQUALS(a1.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(a2.GetMeasuredSize().height, h150, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM68. At a UI scale that is not a power-of-two fraction, taking an item's padding off its size
// leaves a float residue where its MATCH_PARENT Label's slot is empty: the slot still counts as
// empty, so the item keeps the Label's height at the parent's width.
int UtcDaliViewMeasureModeMatchContentItemAtFractionalScaleP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.1f);
  const float s = 1.1f;

  View p    = CreateCustomParent(300.0f, 600.0f);
  View item = View::New();
  item.SetPadding(16.0f, 8.0f, 0.0f, 0.0f);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  item.Add(label);
  p.Add(item);
  window.Add(p);

  SettleLayout(application);

  const float h = label.GetHeightForWidth(WidthOf(p) - 24.0f * s);
  DALI_TEST_CHECK(h > 0.0f);

  DALI_TEST_EQUALS(item.GetMeasuredSize().height, h, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(item), h, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h, MODE_EPSILON, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// MM69. The same residue in a grid cell: a padded StackLayout whose only child is a MATCH_PARENT
// Label with a margin. The Label's slot still counts as empty, so the cell keeps the Label's height
// at the grid's width.
int UtcDaliViewMeasureModeMatchContentCellAtFractionalScaleP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.1f);
  const float s = 1.1f;

  GridLayout grid = GridLayout::New();
  grid.SetRequestedWidth(400.0f);
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Auto());
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  Label l0 = CreateWrappingLabel(200.0f);
  l0.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  StackLayout c = StackLayout::New(StackOrientation::VERTICAL);
  c.SetPadding(8.0f);
  c.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(0));
  Label label = CreateWrappingLabel(MATCH_PARENT);
  label.SetMargin(4.0f);
  c.Add(label);
  grid.Add(l0);
  grid.Add(c);
  window.Add(grid);

  SettleLayout(application);

  const float h = label.GetHeightForWidth(WidthOf(grid) - 24.0f * s);
  DALI_TEST_CHECK(h > 0.0f);

  DALI_TEST_EQUALS(c.GetMeasuredSize().height, h + 24.0f * s, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), h + 24.0f * s, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h, MODE_EPSILON, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// MM70. A plain View whose width only a MATCH_PARENT child would give: the child's empty width slot
// keeps the probe's width number in the height step, and the child is measured at its real slot when
// the View is laid out.
int UtcDaliViewMeasureModeDefaultZeroWidthSlotKeepsProbeWidthP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gSizeCalls.clear();

  View p    = CreateCustomParent(300.0f, 800.0f);
  View item = View::New();
  View c    = CreateMatchParentView();
  c.SetMeasureCallback(MeasureCallback::New(&RecordingZeroMeasure0));
  item.Add(CreateSizedView(0.0f, 100.0f));
  item.Add(c);
  p.Add(item);
  window.Add(p);

  SettleLayout(application);

  const std::vector<SizeCall> calls = CallsOf(0);
  DALI_TEST_CHECK(calls.size() >= 2u);
  if(calls.size() >= 2u)
  {
    DALI_TEST_EQUALS(calls[0].width, 300.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[0].height, 800.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[1].width, 300.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[1].height, 100.0f, MODE_EPSILON, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(item.GetMeasuredSize().height, 100.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(c), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(c), 100.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM71. A grow child of a FlexLayout row that is MATCH_PARENT on its empty cross axis: its main share
// is measured once, with the cross axis at the step-1 number, never at the empty cross slot.
int UtcDaliViewMeasureModeFlexAssignedChildAtZeroCrossSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gSizeCalls.clear();

  View       p = CreateCustomParent(300.0f, 800.0f);
  FlexLayout f = FlexLayout::New();
  f.SetRequestedWidth(300.0f);
  View c = CreateMatchParentView();
  c.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  c.SetMeasureCallback(MeasureCallback::New(&RecordingZeroMeasure0));
  f.Add(c);
  p.Add(f);
  window.Add(p);

  SettleLayout(application);

  const std::vector<SizeCall> calls = CallsOf(0);
  DALI_TEST_CHECK(calls.size() >= 2u);
  if(calls.size() >= 2u)
  {
    DALI_TEST_EQUALS(calls[0].width, 300.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[0].height, 800.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[1].width, 300.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls[1].height, 800.0f, MODE_EPSILON, TEST_LOCATION);
  }

  END_TEST;
}

// MM72. AbsoluteLayouts with empty slots: a MATCH_PARENT child keeps the budget width in the height
// step, and a child proportional on both axes is measured once at its proportions of the budget.
int UtcDaliViewMeasureModeAbsoluteZeroSlotFallbacksP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gSizeCalls.clear();

  View           p  = CreateCustomParent(300.0f, 800.0f);
  AbsoluteLayout a1 = AbsoluteLayout::New();
  View           c1 = CreateMatchParentView();
  c1.SetMeasureCallback(MeasureCallback::New(&RecordingZeroMeasure0));
  a1.Add(CreateSizedView(0.0f, 100.0f));
  a1.Add(c1);
  AbsoluteLayout a2 = AbsoluteLayout::New();
  View           c2 = View::New();
  c2.SetLayoutParams(AbsoluteLayoutParams::New()
                       .SetBounds(LayoutRect(0.0f, 0.0f, 0.5f, 0.25f))
                       .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
  c2.SetMeasureCallback(MeasureCallback::New(&RecordingZeroMeasure1));
  a2.Add(c2);
  p.Add(a1);
  p.Add(a2);
  window.Add(p);

  SettleLayout(application);

  const std::vector<SizeCall> calls0 = CallsOf(0);
  DALI_TEST_CHECK(calls0.size() >= 2u);
  if(calls0.size() >= 2u)
  {
    DALI_TEST_EQUALS(calls0[1].width, 300.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls0[1].height, 100.0f, MODE_EPSILON, TEST_LOCATION);
  }
  const std::vector<SizeCall> calls1 = CallsOf(1);
  DALI_TEST_CHECK(!calls1.empty());
  if(!calls1.empty())
  {
    DALI_TEST_EQUALS(calls1[0].width, 150.0f, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(calls1[0].height, 200.0f, MODE_EPSILON, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(a1.GetMeasuredSize().height, 100.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM73. A width-proportional, MATCH_PARENT-height child of an AbsoluteLayout counts towards the
// layout's height with its offset, its margins and the height it reports, as before modes existed.
int UtcDaliViewMeasureModeAbsoluteProportionalMatchHeightBottomP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View           p = CreateCustomParent(300.0f, 800.0f);
  AbsoluteLayout a = AbsoluteLayout::New();
  View           c = View::New();
  c.SetRequestedHeight(MATCH_PARENT);
  c.SetMinimumHeight(40.0f);
  c.SetMargin(0.0f, 0.0f, 5.0f, 5.0f);
  c.SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetBounds(LayoutRect(0.0f, 30.0f, 0.5f, -1.0f))
                      .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  a.Add(c);
  p.Add(a);
  window.Add(p);

  SettleLayout(application);

  DALI_TEST_EQUALS(a.GetMeasuredSize().height, 80.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM74. An AUTO grid column that only a MATCH_PARENT Label spans has no width of its own: the Label
// keeps its height at the grid's width, so its row keeps that height and the row below starts there.
int UtcDaliViewMeasureModeGridAutoColumnOfMatchChildrenKeepsRowP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  GridLayout grid = GridLayout::New();
  grid.SetRequestedWidth(480.0f);
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Auto());
  columns.PushBack(GridLength::Absolute(200.0f));
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Auto());
  rows.PushBack(GridLength::Auto());
  grid.SetColumnDefinitions(columns);
  grid.SetRowDefinitions(rows);
  Label l = CreateWrappingLabel(MATCH_PARENT);
  l.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  View seed = CreateSizedView(100.0f, 20.0f);
  seed.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
  grid.Add(l);
  grid.Add(seed);
  window.Add(grid);

  SettleLayout(application);

  const float h = l.GetHeightForWidth(480.0f);
  DALI_TEST_CHECK(h > 0.0f);

  DALI_TEST_EQUALS(PositionYOf(seed), h, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(grid.GetMeasuredSize().height, h + 20.0f, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM75. A MATCH_PARENT-width weight child of a StackLayout whose width only it would give: its empty
// cross slot is never used, so every measurement keeps a real width.
int UtcDaliViewMeasureModeStackWeightChildAtZeroCrossSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gSizeCalls.clear();

  View        p     = CreateCustomParent(300.0f, 800.0f);
  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  stack.SetRequestedHeight(200.0f);
  View w = View::New();
  w.SetRequestedWidth(MATCH_PARENT);
  w.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  w.SetMeasureCallback(MeasureCallback::New(&RecordingZeroMeasure0));
  stack.Add(w);
  p.Add(stack);
  window.Add(p);

  SettleLayout(application);

  const std::vector<SizeCall> calls = CallsOf(0);
  DALI_TEST_CHECK(calls.size() >= 2u);
  for(const SizeCall& call : calls)
  {
    DALI_TEST_EQUALS(call.width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  }

  END_TEST;
}

// MM76. A RecyclerView lays every item out at the list's width, so it measures a WRAP_CONTENT item
// exactly there: the item's MATCH_PARENT-width Label gets the height of the list's width, not the
// height of the item's 64-wide icon.
int UtcDaliViewMeasureModeRecyclerWrapItemMatchContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gItemIconWidth = 64.0f;
  AddIconTitleList(window);

  SettleLayout(application);

  DALI_TEST_CHECK(!gItemViews.empty());
  if(!gItemViews.empty())
  {
    View        item  = gItemViews[0];
    Label       title = gItemTitles[0];
    const float h     = title.GetHeightForWidth(480.0f);
    DALI_TEST_CHECK(h > 0.0f);
    DALI_TEST_CHECK(title.GetHeightForWidth(64.0f) > h);

    DALI_TEST_EQUALS(HeightOf(item), 64.0f + h, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(HeightOf(title), h, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(WidthOf(title), 480.0f, MODE_EPSILON, TEST_LOCATION);
  }

  END_TEST;
}

// MM77. The same list with a 4-wide stripe for an icon: the item's Label still gets the height of
// the list's width, not the height at the stripe's width.
int UtcDaliViewMeasureModeRecyclerWrapItemNarrowSiblingP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  gItemIconWidth = 4.0f;
  AddIconTitleList(window);

  SettleLayout(application);

  DALI_TEST_CHECK(!gItemViews.empty());
  if(!gItemViews.empty())
  {
    View        item  = gItemViews[0];
    Label       title = gItemTitles[0];
    const float h     = title.GetHeightForWidth(480.0f);
    DALI_TEST_CHECK(h > 0.0f);

    DALI_TEST_EQUALS(HeightOf(title), h, MODE_EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(HeightOf(item), 64.0f + h, MODE_EPSILON, TEST_LOCATION);
  }

  END_TEST;
}

// MM78. Two MATCH_PARENT-width Labels, the second with a 16 start margin, in a WRAP_CONTENT
// StackLayout under a custom parent that lays it out at its full width. Only MATCH_PARENT children
// would give the stack a width, so neither Label is measured at a slot: each keeps its height at its
// budget (480 and 464), and the second Label's margin does not make a 16-wide slot for the first.
int UtcDaliViewMeasureModeMatchSiblingsWithUnequalMarginsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View        p     = CreateCustomParent(480.0f, 800.0f);
  StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
  Label       l1    = CreateWrappingLabel(MATCH_PARENT);
  Label       l2    = CreateWrappingLabel(MATCH_PARENT);
  l2.SetMargin(16.0f, 0.0f, 0.0f, 0.0f);
  stack.Add(l1);
  stack.Add(l2);
  p.Add(stack);
  window.Add(p);

  SettleLayout(application);

  const float h1 = l1.GetHeightForWidth(480.0f);
  const float h2 = l2.GetHeightForWidth(464.0f);
  DALI_TEST_CHECK(l1.GetHeightForWidth(16.0f) > h1);

  DALI_TEST_EQUALS(HeightOf(l1), h1, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(l2), h2, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stack.GetMeasuredSize().height, h1 + h2, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

// MM79. A grid without column definitions whose width is not determined: its own minimum gives its
// single column a width of its own, so the MATCH_PARENT Label there is measured exactly at 300 and
// the grid measures the Label's height at 300.
int UtcDaliViewMeasureModeGridOwnMinimumColumnP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View p = CreateSizedView(480.0f, 800.0f);
  p.SetMeasureCallback(MeasureCallback::New(&MeasureChildrenAtConstraint));
  p.SetArrangeCallback(ArrangeCallback::New(&ArrangeChildrenAtMeasuredSize));
  GridLayout grid = GridLayout::New();
  grid.SetMinimumWidth(300.0f);
  Label label = CreateWrappingLabel(MATCH_PARENT);
  grid.Add(label);
  p.Add(grid);
  window.Add(p);

  SettleLayout(application);

  const float h300 = label.GetHeightForWidth(300.0f);
  DALI_TEST_CHECK(h300 > label.GetHeightForWidth(480.0f));

  DALI_TEST_EQUALS(grid.GetMeasuredSize().width, 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(grid.GetMeasuredSize().height, h300, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(WidthOf(label), 300.0f, MODE_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(HeightOf(label), h300, MODE_EPSILON, TEST_LOCATION);

  END_TEST;
}

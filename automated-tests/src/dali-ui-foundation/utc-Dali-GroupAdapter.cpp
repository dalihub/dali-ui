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
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/integration-api/recycler/group-adapter.h>
#include <dali-ui-foundation/integration-api/recycler/group-body-decoration.h>
#include <dali-ui-foundation/integration-api/recycler/group-data-source.h>
#include <dali-ui-foundation/integration-api/recycler/group-linear-items-layouter.h>
#include <dali-ui-foundation/integration-api/recycler/item-adapter.h>
#include <dali-ui-foundation/integration-api/recycler/item-view-holder.h>
#include <dali-ui-foundation/integration-api/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/integration-api/recycler/recycler-view.h>
#include <dali-ui-test-suite-utils.h>
#include <array>

using Dali::Ui::Integration::GroupAdapter;
using Dali::Ui::Integration::GroupBodyDecoration;
using Dali::Ui::Integration::GroupDataSource;
using Dali::Ui::Integration::GroupLinearItemsLayouter;
using Dali::Ui::Integration::GroupRowType;
using Dali::Ui::Integration::ItemAdapter;
using Dali::Ui::Integration::ItemViewHolder;
using Dali::Ui::Integration::LinearItemsLayouter;
using Dali::Ui::Integration::RecyclerView;


using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Extension::LayoutScaleManager;

namespace
{
class TestGroupDataSource : public GroupDataSource
{
public:
  uint32_t GetGroupCount() const override
  {
    return 2u;
  }

  uint32_t GetGroupItemCount(uint32_t groupIndex) const override
  {
    return groupIndex == 0u ? 2u : 1u;
  }

  bool HasGroupHeader(uint32_t groupIndex) const override
  {
    return groupIndex == 0u;
  }
};

class TestInnerAdapterCallbacks : public ConnectionTracker
{
public:
  void OnCreateViewHolder(ItemViewHolder& holder)
  {
    createdPosition = holder.position;
  }

  void OnBindViewHolder(ItemViewHolder& holder)
  {
    boundPosition = holder.position;
  }

  void OnRecycleViewHolder(ItemViewHolder& holder)
  {
    recycledPosition = holder.position;
  }

  uint32_t createdPosition{0u};
  uint32_t boundPosition{0u};
  uint32_t recycledPosition{0u};
};

// Sets the system UI scale for the scope of a test and restores 1.0 when the scope ends, also when
// a DALI_TEST_* check throws. Declare it after the UiTestApplication so that it is destroyed first.
class ScopedUiScale
{
public:
  explicit ScopedUiScale(float scale)
  {
    LayoutScaleManager::Get().SetUiScale(scale);
  }

  ~ScopedUiScale()
  {
    LayoutScaleManager::Get().SetUiScale(1.0f);
  }

  ScopedUiScale(const ScopedUiScale&)            = delete;
  ScopedUiScale& operator=(const ScopedUiScale&) = delete;
};
} // unnamed namespace

void utc_dali_group_adapter_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_group_adapter_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGroupAdapterFlattensHeadersBodiesAndGapsP(void)
{
  auto         source  = std::make_shared<TestGroupDataSource>();
  GroupAdapter adapter = GroupAdapter::New();
  adapter.SetDataSource(source);

  ItemAdapter flatAdapter = adapter.GetAdapter();
  DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 5u, TEST_LOCATION);

  DALI_TEST_EQUALS(flatAdapter.GetItemViewType(0u), GroupAdapter::RESERVED_VIEW_TYPE_HEADER, TEST_LOCATION);
  DALI_TEST_EQUALS(flatAdapter.GetItemViewType(3u), GroupAdapter::RESERVED_VIEW_TYPE_GAP, TEST_LOCATION);

  adapter.ClearDataSource();
  DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupAdapterRecyclesInnerAdapterWithInnerPositionP(void)
{
  auto         source = std::make_shared<TestGroupDataSource>();
  GroupAdapter groupAdapter = GroupAdapter::New();
  ItemAdapter        innerAdapter = ItemAdapter::New();
  TestInnerAdapterCallbacks callbacks;

  innerAdapter.CreateViewHolderSignal().Connect(&callbacks, &TestInnerAdapterCallbacks::OnCreateViewHolder);
  innerAdapter.BindViewHolderSignal().Connect(&callbacks, &TestInnerAdapterCallbacks::OnBindViewHolder);
  innerAdapter.RecycleViewHolderSignal().Connect(&callbacks, &TestInnerAdapterCallbacks::OnRecycleViewHolder);

  groupAdapter.SetDataSource(source);
  groupAdapter.SetInnerAdapter(innerAdapter);

  ItemAdapter    flatAdapter = groupAdapter.GetAdapter();
  ItemViewHolder holder;
  holder.position = 4u; // Second group's sole body item: flat=4, inner=2.
  flatAdapter.CreateViewHolder(holder);
  DALI_TEST_EQUALS(callbacks.createdPosition, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(holder.position, 4u, TEST_LOCATION);

  flatAdapter.BindViewHolder(holder);
  DALI_TEST_EQUALS(callbacks.boundPosition, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(holder.position, 4u, TEST_LOCATION);

  flatAdapter.RecycleViewHolder(holder);

  DALI_TEST_EQUALS(callbacks.recycledPosition, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(holder.position, 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupAdapterRetainsDataSourceP(void)
{
  GroupAdapter adapter = GroupAdapter::New();
  ItemAdapter  flatAdapter;

  {
    auto source = std::make_shared<TestGroupDataSource>();
    adapter.SetDataSource(source);
    flatAdapter = adapter.GetAdapter();
    DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 5u, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 5u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupAdapterRetainsBridgeForFlatAdapterP(void)
{
  ItemAdapter flatAdapter;

  {
    auto         source  = std::make_shared<TestGroupDataSource>();
    GroupAdapter groupAdapter = GroupAdapter::New();
    groupAdapter.SetDataSource(source);
    flatAdapter = groupAdapter.GetAdapter();
    DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 5u, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(flatAdapter.GetItemCount(), 5u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupBodyDecorationLifecycleP(void)
{
  UiTestApplication application;
  View scroller = View::New();
  GroupBodyDecoration decoration;
  int invalidated = 0;
  decoration.LayoutInvalidatedSignal().Connect(&application, [&invalidated]() { ++invalidated; });
  decoration.SetBodyHorizontalMargin(-5.0f, 12.0f);
  decoration.SetBodyColor(UiColor(0xFF0000));
  decoration.SetBodyCornerRadius(8.0f);
  DALI_TEST_EQUALS(invalidated, 1, TEST_LOCATION);

  ItemViewHolder header;
  header.rowType = GroupRowType::HEADER;
  decoration.OnItemActivated(header, scroller);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(header).start, 0.0f, 0.001f, TEST_LOCATION);

  ItemViewHolder top;
  top.rowType = GroupRowType::BODY_TOP;
  top.groupIndex = 3u;
  top.view = View::New();
  ItemViewHolder bottom = top;
  bottom.rowType = GroupRowType::BODY_BOTTOM;
  decoration.OnItemActivated(top, scroller);
  decoration.OnItemActivated(bottom, scroller);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).end, 12.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).start, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).top, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).bottom, 0.0f, 0.001f, TEST_LOCATION);

  decoration.OnLayoutStart();
  decoration.OnItemBoundsUpdated(header, LayoutRect(0.0f, 0.0f, 100.0f, 20.0f));
  decoration.OnItemBoundsUpdated(top, LayoutRect(0.0f, 20.0f, 100.0f, 30.0f));
  decoration.OnItemBoundsUpdated(bottom, LayoutRect(0.0f, 50.0f, 100.0f, 40.0f));
  View background = View::DownCast(scroller.GetChildAt(0u));
  DALI_TEST_EQUALS(background.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(88.0f, 70.0f, 1.0f), TEST_LOCATION);

  decoration.SetBodyColor(UiColor(0x00FF00));
  decoration.SetBodyCornerRadius(4.0f);
  decoration.OnItemRecycled(header);
  ItemViewHolder unknown = top;
  unknown.groupIndex = 99u;
  decoration.OnItemRecycled(unknown);
  decoration.OnItemRecycled(top);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 1u, TEST_LOCATION);
  decoration.OnItemRecycled(bottom);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupLinearItemsLayouterP(void)
{
  UiTestApplication application;
  GroupLinearItemsLayouter horizontal = GroupLinearItemsLayouter::New(LinearItemsLayouter::Orientation::HORIZONTAL);
  DALI_TEST_CHECK(horizontal.CanScrollVertically());

  GroupAdapter adapter = GroupAdapter::New();
  adapter.SetDataSource(std::make_shared<TestGroupDataSource>());
  GroupLinearItemsLayouter layouter = GroupLinearItemsLayouter::New();
  layouter.SetGroupAdapter(adapter);
  layouter.SetBodyHorizontalMargin(-4.0f, 15.0f);
  DALI_TEST_EQUALS(layouter.GetBodyMarginLeft(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.GetBodyMarginRight(), 15.0f, 0.001f, TEST_LOCATION);

  LayoutRect header = layouter.GetItemBounds(0u, 100.0f);
  LayoutRect body = layouter.GetItemBounds(1u, 100.0f);
  DALI_TEST_EQUALS(header.x, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(header.width, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(body.x, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(body.width, 85.0f, 0.001f, TEST_LOCATION);

  layouter.SetBodyHorizontalMargin(80.0f, 80.0f);
  body = layouter.GetItemBounds(1u, 100.0f);
  DALI_TEST_EQUALS(body.x, 80.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(body.width, 0.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupLinearItemsLayouterAppliesBodyMarginsDuringLayoutP(void)
{
  UiTestApplication application;
  View header;
  std::array<View, 3> bodyViews;
  ItemAdapter inner = ItemAdapter::New();
  inner.CreateViewHolderSignal().Connect(&application, [](ItemViewHolder& holder)
  {
    holder.view = View::New();
    holder.view.SetRequestedHeight(40.0f);
  });
  inner.BindViewHolderSignal().Connect(&application, [&](ItemViewHolder& holder)
  {
    bodyViews.at(holder.position) = holder.view;
  });

  GroupAdapter group = GroupAdapter::New();
  group.SetDataSource(std::make_shared<TestGroupDataSource>());
  group.SetInnerAdapter(inner);
  group.SetGapHeight(12.0f);
  group.CreateHeaderViewHolderSignal().Connect(&application, [&](ItemViewHolder& holder)
  {
    header = View::New();
    header.SetRequestedHeight(30.0f);
    holder.view = header;
  });

  GroupLinearItemsLayouter layouter = GroupLinearItemsLayouter::New();
  layouter.SetGroupAdapter(group);
  layouter.SetBodyHorizontalMargin(12.0f, 23.0f);

  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(200.0f);
  recycler.SetRequestedHeight(240.0f);
  recycler.SetItemsLayouter(layouter);
  recycler.SetAdapter(group.GetAdapter());
  application.GetWindow().Add(recycler);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 4u, TEST_LOCATION);
  DALI_TEST_CHECK(header);
  DALI_TEST_EQUALS(header.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(header.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, 0.001f, TEST_LOCATION);
  // The first group's top/bottom rows and the second group's single row all use body margins.
  for(const View& body : bodyViews)
  {
    DALI_TEST_CHECK(body);
    DALI_TEST_EQUALS(body.GetProperty<float>(Actor::Property::POSITION_X), 12.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(body.GetProperty<float>(Actor::Property::SIZE_WIDTH), 165.0f, 0.001f, TEST_LOCATION);
  }

  layouter.SetBodyHorizontalMargin(7.0f, 9.0f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(header.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(header.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, 0.001f, TEST_LOCATION);
  for(const View& body : bodyViews)
  {
    DALI_TEST_EQUALS(body.GetProperty<float>(Actor::Property::POSITION_X), 7.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(body.GetProperty<float>(Actor::Property::SIZE_WIDTH), 184.0f, 0.001f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliGroupAdapterHeaderAndGapHolderLifecycleP(void)
{
  UiTestApplication application;
  GroupAdapter group = GroupAdapter::New();
  group.SetDataSource(std::make_shared<TestGroupDataSource>());
  group.SetGapHeight(14.0f);
  DALI_TEST_EQUALS(group.GetGapHeight(), 14.0f, TEST_LOCATION);
  ItemAdapter flat = group.GetAdapter();

  ItemViewHolder fallbackHeader;
  fallbackHeader.position = 0u;
  flat.CreateViewHolder(fallbackHeader);
  DALI_TEST_CHECK(fallbackHeader.view);
  DALI_TEST_EQUALS(fallbackHeader.rowType, GroupRowType::HEADER, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackHeader.view.GetRequestedHeight(), 0.0f, TEST_LOCATION);

  int createCount = 0;
  int bindCount = 0;
  int recycleCount = 0;
  group.CreateHeaderViewHolderSignal().Connect(&application, [&](ItemViewHolder& holder)
  {
    ++createCount;
    holder.view = View::New();
  });
  group.BindHeaderViewHolderSignal().Connect(&application, [&](ItemViewHolder&)
  {
    ++bindCount;
  });
  group.RecycleHeaderViewHolderSignal().Connect(&application, [&](ItemViewHolder&)
  {
    ++recycleCount;
  });

  ItemViewHolder header;
  header.position = 0u;
  flat.CreateViewHolder(header);
  flat.BindViewHolder(header);
  flat.RecycleViewHolder(header);
  DALI_TEST_EQUALS(createCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(bindCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycleCount, 1, TEST_LOCATION);

  ItemViewHolder gap;
  gap.position = 3u;
  flat.CreateViewHolder(gap);
  DALI_TEST_EQUALS(gap.rowType, GroupRowType::GAP, TEST_LOCATION);
  DALI_TEST_EQUALS(gap.view.GetRequestedHeight(), 14.0f, TEST_LOCATION);
  group.SetGapHeight(22.0f);
  flat.BindViewHolder(gap);
  DALI_TEST_EQUALS(gap.view.GetRequestedHeight(), 22.0f, TEST_LOCATION);
  flat.RecycleViewHolder(gap);

  group.NotifyDataSetChanged();
  DALI_TEST_EQUALS(flat.GetItemCount(), 5u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGroupAdapterPreventsReservedInnerViewTypesP(void)
{
  UiTestApplication application;
  GroupAdapter group = GroupAdapter::New();
  group.SetDataSource(std::make_shared<TestGroupDataSource>());
  ItemAdapter inner = ItemAdapter::New();
  inner.GetItemViewTypeSignal().Connect(&application, [](uint32_t)
  {
    return GroupAdapter::RESERVED_VIEW_TYPE_HEADER;
  });
  group.SetInnerAdapter(inner);

  ItemAdapter flat = group.GetAdapter();
  DALI_TEST_EQUALS(flat.GetItemViewType(0u), GroupAdapter::RESERVED_VIEW_TYPE_HEADER, TEST_LOCATION);
  DALI_TEST_EQUALS(flat.GetItemViewType(3u), GroupAdapter::RESERVED_VIEW_TYPE_GAP, TEST_LOCATION);
  DALI_TEST_EQUALS(flat.GetItemViewType(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(flat.GetItemViewType(4u), 0u, TEST_LOCATION);
  END_TEST;
}

// GroupBodyDecoration returns its margins in natural units (RecyclerView multiplies them) and insets
// the card by the same scaled margins. At a UI scale of 2 margins of (5, 12) in a 100 wide slot put
// the card at x 10, 100 - 10 - 24 = 66 wide. Fails before this change: the card was at x 5, 83 wide.
int UtcDaliGroupBodyDecorationMarginsFollowScaleP(void)
{
  UiTestApplication   application;
  ScopedUiScale       uiScale(2.0f);
  View                scroller = View::New();
  GroupBodyDecoration decoration;
  decoration.SetBodyHorizontalMargin(5.0f, 12.0f);

  ItemViewHolder top;
  top.rowType    = GroupRowType::BODY_TOP;
  top.groupIndex = 3u;
  top.view       = View::New();
  ItemViewHolder bottom = top;
  bottom.rowType        = GroupRowType::BODY_BOTTOM;
  decoration.OnItemActivated(top, scroller);
  decoration.OnItemActivated(bottom, scroller);

  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).start, 5.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(decoration.GetItemOffsets(top).end, 12.0f, 0.001f, TEST_LOCATION);

  decoration.OnLayoutStart();
  decoration.OnItemBoundsUpdated(top, LayoutRect(0.0f, 20.0f, 100.0f, 30.0f));
  decoration.OnItemBoundsUpdated(bottom, LayoutRect(0.0f, 50.0f, 100.0f, 40.0f));
  View background = View::DownCast(scroller.GetChildAt(0u));
  DALI_TEST_CHECK(background);
  DALI_TEST_EQUALS(background.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(10.0f, 20.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(background.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(66.0f, 70.0f, 1.0f), TEST_LOCATION);

  decoration.OnItemRecycled(top);
  decoration.OnItemRecycled(bottom);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

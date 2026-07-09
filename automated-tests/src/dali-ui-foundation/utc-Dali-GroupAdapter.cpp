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
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

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

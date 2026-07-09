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

#include <dali-ui-foundation/integration-api/group-adapter-impl.h>
#include <dali-ui-foundation/integration-api/item-adapter-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <limits>

namespace Dali
{
namespace Ui
{

constexpr uint32_t GAP_VIEW_TYPE    = GroupAdapter::RESERVED_VIEW_TYPE_GAP;
constexpr uint32_t HEADER_VIEW_TYPE = GroupAdapter::RESERVED_VIEW_TYPE_HEADER;

const GroupAdapterImpl::FlatRecord GroupAdapterImpl::sInvalidRecord{
  GroupRowType::NONE,
  ItemViewHolder::INVALID_GROUP_INDEX,
  std::numeric_limits<uint32_t>::max()};

GroupAdapterImplPtr GroupAdapterImpl::New()
{
  return new GroupAdapterImpl();
}

GroupAdapterImpl::GroupAdapterImpl()
{
}

void GroupAdapterImpl::SetDataSource(std::shared_ptr<GroupDataSource> source)
{
  std::vector<FlatRecord> oldRecords = std::move(mRecords);
  ItemAdapter             oldInner   = mInner;
  mSource                            = std::move(source);
  Rebuild();
  NotifyFlatAdapterForReplacement(std::move(oldRecords), std::move(oldInner));
}

void GroupAdapterImpl::ClearDataSource()
{
  std::vector<FlatRecord> oldRecords = std::move(mRecords);
  ItemAdapter             oldInner   = mInner;
  mSource.reset();
  ReleaseRecords();
  NotifyFlatAdapterForReplacement(std::move(oldRecords), std::move(oldInner));
}

void GroupAdapterImpl::SetInnerAdapter(ItemAdapter inner)
{
  std::vector<FlatRecord> oldRecords = std::move(mRecords);
  ItemAdapter             oldInner   = mInner;
  mInner                             = std::move(inner);
  Rebuild();
  NotifyFlatAdapterForReplacement(std::move(oldRecords), std::move(oldInner));
}

void GroupAdapterImpl::SetGapHeight(float height)
{
  mGapHeight = std::max(0.0f, height);
  NotifyFlatAdapter();
}

float GroupAdapterImpl::GetGapHeight() const
{
  return mGapHeight;
}

Signal<void(ItemViewHolder&)>& GroupAdapterImpl::CreateHeaderViewHolderSignal()
{
  return mCreateHeaderSignal;
}

Signal<void(ItemViewHolder&)>& GroupAdapterImpl::BindHeaderViewHolderSignal()
{
  return mBindHeaderSignal;
}

Signal<void(ItemViewHolder&)>& GroupAdapterImpl::RecycleHeaderViewHolderSignal()
{
  return mRecycleHeaderSignal;
}

void GroupAdapterImpl::NotifyDataSetChanged()
{
  std::vector<FlatRecord> oldRecords = std::move(mRecords);
  ItemAdapter             oldInner   = mInner;
  Rebuild();
  NotifyFlatAdapterForReplacement(std::move(oldRecords), std::move(oldInner));
}

ItemAdapter GroupAdapterImpl::GetFlatAdapter() const
{
  return const_cast<GroupAdapterImpl*>(this)->EnsureFlatAdapter();
}

const GroupAdapterImpl::FlatRecord& GroupAdapterImpl::GetRecord(uint32_t flatPosition) const
{
  if(flatPosition >= mRecords.size())
  {
    return sInvalidRecord;
  }
  return mRecords[flatPosition];
}

const GroupAdapterImpl::FlatRecord& GroupAdapterImpl::GetRecycleRecord(uint32_t flatPosition) const
{
  const auto& records = mReplacementSnapshots.back().records;
  if(flatPosition >= records.size())
  {
    return sInvalidRecord;
  }
  return records[flatPosition];
}

GroupRowType GroupAdapterImpl::GetRowType(uint32_t flatPosition) const
{
  return GetRecord(flatPosition).rowType;
}

// ---------------------------------------------------------------------------

void GroupAdapterImpl::Rebuild()
{
  mRecords.clear();
  if(!mSource)
  {
    return;
  }

  const uint32_t groupCount = mSource->GetGroupCount();
  uint32_t       innerPos   = 0u;

  for(uint32_t g = 0u; g < groupCount; ++g)
  {
    if(g > 0u)
    {
      mRecords.push_back({GroupRowType::GAP, g, std::numeric_limits<uint32_t>::max()});
    }

    if(mSource->HasGroupHeader(g))
    {
      mRecords.push_back({GroupRowType::HEADER, g, std::numeric_limits<uint32_t>::max()});
    }

    const uint32_t bodyCount = mSource->GetGroupItemCount(g);
    for(uint32_t b = 0u; b < bodyCount; ++b)
    {
      GroupRowType rt;
      if(bodyCount == 1u)
      {
        rt = GroupRowType::BODY_SINGLE;
      }
      else if(b == 0u)
      {
        rt = GroupRowType::BODY_TOP;
      }
      else if(b == bodyCount - 1u)
      {
        rt = GroupRowType::BODY_BOTTOM;
      }
      else
      {
        rt = GroupRowType::BODY_MIDDLE;
      }
      mRecords.push_back({rt, g, innerPos++});
    }
  }
}

void GroupAdapterImpl::ReleaseRecords()
{
  std::vector<FlatRecord>().swap(mRecords);
}

void GroupAdapterImpl::NotifyFlatAdapter()
{
  ItemAdapter flatAdapter = mFlatAdapter.GetHandle();
  if(flatAdapter)
  {
    flatAdapter.NotifyDataSetChanged();
  }
}

void GroupAdapterImpl::NotifyFlatAdapterForReplacement(std::vector<FlatRecord> oldRecords, ItemAdapter oldInner)
{
  mReplacementSnapshots.push_back({std::move(oldRecords), std::move(oldInner)});
  ItemAdapter flatAdapter = mFlatAdapter.GetHandle();
  if(flatAdapter)
  {
    flatAdapter.GetImpl().NotifyAdapterReplaced();
  }
  mReplacementSnapshots.pop_back();
}

ItemAdapter GroupAdapterImpl::EnsureFlatAdapter()
{
  ItemAdapter flatAdapter = mFlatAdapter.GetHandle();
  if(flatAdapter)
  {
    return flatAdapter;
  }

  flatAdapter = ItemAdapter::New();
  flatAdapter.GetImpl().SetInternalKeepAlive(Dali::BaseHandle(this));
  flatAdapter.GetItemCountSignal().Connect(mSlotDelegate, &GroupAdapterImpl::OnGetItemCount);
  flatAdapter.GetItemViewTypeSignal().Connect(mSlotDelegate, &GroupAdapterImpl::OnGetItemViewType);
  flatAdapter.CreateViewHolderSignal().Connect(mSlotDelegate, &GroupAdapterImpl::OnCreateViewHolder);
  flatAdapter.BindViewHolderSignal().Connect(mSlotDelegate, &GroupAdapterImpl::OnBindViewHolder);
  flatAdapter.RecycleViewHolderSignal().Connect(mSlotDelegate, &GroupAdapterImpl::OnRecycleViewHolder);
  mFlatAdapter = Dali::WeakHandle<ItemAdapter>(flatAdapter);
  return flatAdapter;
}

void GroupAdapterImpl::FillHolderGroupInfo(ItemViewHolder& holder) const
{
  const FlatRecord& rec = GetRecord(holder.position);
  holder.rowType        = rec.rowType;
  holder.groupIndex     = rec.groupIndex;
}

uint32_t GroupAdapterImpl::OnGetItemCount()
{
  return static_cast<uint32_t>(mRecords.size());
}

uint32_t GroupAdapterImpl::OnGetItemViewType(uint32_t flatPosition)
{
  const FlatRecord& rec = GetRecord(flatPosition);
  if(rec.rowType == GroupRowType::GAP)
  {
    return GAP_VIEW_TYPE;
  }
  if(rec.rowType == GroupRowType::HEADER)
  {
    return HEADER_VIEW_TYPE;
  }
  if(!mInner)
  {
    return 0u;
  }
  const uint32_t queryPos = (rec.innerPosition != std::numeric_limits<uint32_t>::max())
                              ? rec.innerPosition
                              : flatPosition;
  const uint32_t type     = mInner.GetItemViewType(queryPos);
  if(type == GAP_VIEW_TYPE || type == HEADER_VIEW_TYPE)
  {
    DALI_LOG_ERROR(
      "GroupAdapter: inner adapter view type %u collides with a reserved type "
      "(RESERVED_VIEW_TYPE_GAP or RESERVED_VIEW_TYPE_HEADER). "
      "Clamping to 0 to prevent recycle pool corruption.\n",
      type);
    return 0u;
  }
  return type;
}

void GroupAdapterImpl::OnCreateViewHolder(ItemViewHolder& holder)
{
  FillHolderGroupInfo(holder);

  if(holder.rowType == GroupRowType::GAP)
  {
    View spacer = View::New();
    spacer.SetRequestedHeight(mGapHeight);
    holder.view = spacer;
    return;
  }

  if(holder.rowType == GroupRowType::HEADER)
  {
    if(mCreateHeaderSignal.GetConnectionCount() > 0u)
    {
      mCreateHeaderSignal.Emit(holder);
    }
    if(!holder.view)
    {
      DALI_LOG_ERROR(
        "GroupAdapter: CreateHeaderViewHolderSignal did not set holder.view "
        "for groupIndex=%u. Using invisible spacer.\n",
        holder.groupIndex);
      View spacer = View::New();
      spacer.SetRequestedHeight(0.0f);
      holder.view = spacer;
    }
    return;
  }

  if(!mInner)
  {
    return;
  }

  const FlatRecord& rec     = GetRecord(holder.position);
  const uint32_t    origPos = holder.position;

  if(rec.innerPosition != std::numeric_limits<uint32_t>::max())
  {
    holder.position = rec.innerPosition;
  }

  mInner.CreateViewHolder(holder);
  holder.position   = origPos;
  holder.rowType    = rec.rowType;
  holder.groupIndex = rec.groupIndex;
}

void GroupAdapterImpl::OnBindViewHolder(ItemViewHolder& holder)
{
  FillHolderGroupInfo(holder);

  if(holder.rowType == GroupRowType::GAP)
  {
    holder.view.SetRequestedHeight(mGapHeight);
    return;
  }

  if(holder.rowType == GroupRowType::HEADER)
  {
    if(mBindHeaderSignal.GetConnectionCount() > 0u)
    {
      mBindHeaderSignal.Emit(holder);
    }
    return;
  }

  if(!mInner)
  {
    return;
  }

  const FlatRecord& rec     = GetRecord(holder.position);
  const uint32_t    origPos = holder.position;

  if(rec.innerPosition != std::numeric_limits<uint32_t>::max())
  {
    holder.position = rec.innerPosition;
  }

  mInner.BindViewHolder(holder);
  holder.position   = origPos;
  holder.rowType    = rec.rowType;
  holder.groupIndex = rec.groupIndex;
}

void GroupAdapterImpl::OnRecycleViewHolder(ItemViewHolder& holder)
{
  const bool        replacing = !mReplacementSnapshots.empty();
  const FlatRecord& rec       = replacing ? GetRecycleRecord(holder.position) : GetRecord(holder.position);
  const uint32_t    origPos   = holder.position;

  if(rec.rowType == GroupRowType::GAP)
  {
    return;
  }

  if(rec.rowType == GroupRowType::HEADER)
  {
    if(mRecycleHeaderSignal.GetConnectionCount() > 0u)
    {
      mRecycleHeaderSignal.Emit(holder);
    }
    return;
  }

  ItemAdapter& innerAdapter = replacing ? mReplacementSnapshots.back().inner : mInner;
  if(innerAdapter)
  {
    if(rec.innerPosition != std::numeric_limits<uint32_t>::max())
    {
      holder.position = rec.innerPosition;
    }

    innerAdapter.RecycleViewHolder(holder);
    holder.position   = origPos;
    holder.rowType    = rec.rowType;
    holder.groupIndex = rec.groupIndex;
  }
}

} // namespace Ui
} // namespace Dali

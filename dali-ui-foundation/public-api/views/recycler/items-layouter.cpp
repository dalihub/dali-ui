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

#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{

// ---------------------------------------------------------------------------
// Handle constructor / GetImpl
// ---------------------------------------------------------------------------

ItemsLayouter::ItemsLayouter(ItemsLayouterImpl* impl)
: BaseHandle(impl)
{
}

ItemsLayouterImpl& ItemsLayouter::GetImpl()
{
  DALI_ASSERT_ALWAYS(*this && "ItemsLayouter handle is empty");
  return static_cast<ItemsLayouterImpl&>(GetBaseObject());
}

const ItemsLayouterImpl& ItemsLayouter::GetImpl() const
{
  DALI_ASSERT_ALWAYS(*this && "ItemsLayouter handle is empty");
  return static_cast<const ItemsLayouterImpl&>(GetBaseObject());
}

// ---------------------------------------------------------------------------
// App-facing query / configuration API
// ---------------------------------------------------------------------------

bool ItemsLayouter::CanScrollVertically() const
{
  return GetImpl().CanScrollVertically();
}

bool ItemsLayouter::CanScrollHorizontally() const
{
  return GetImpl().CanScrollHorizontally();
}

float ItemsLayouter::ComputeScrollOffset() const
{
  return GetImpl().ComputeScrollOffset();
}

float ItemsLayouter::ComputeScrollExtent() const
{
  return GetImpl().ComputeScrollExtent();
}

float ItemsLayouter::ComputeScrollRange() const
{
  return GetImpl().ComputeScrollRange();
}

uint32_t ItemsLayouter::GetFirstVisiblePosition() const
{
  return GetImpl().GetFirstVisiblePosition();
}

uint32_t ItemsLayouter::GetLastVisiblePosition() const
{
  return GetImpl().GetLastVisiblePosition();
}

LayoutRect ItemsLayouter::GetItemBounds(uint32_t position, float crossExtent) const
{
  return GetImpl().GetItemBounds(position, crossExtent);
}

ItemsLayouter::Orientation ItemsLayouter::GetOrientation() const
{
  return GetImpl().GetOrientation();
}

void ItemsLayouter::SetOrientation(Orientation orientation)
{
  GetImpl().SetOrientation(orientation);
}

void ItemsLayouter::SetItemExtent(float extent)
{
  GetImpl().SetItemExtent(extent);
}

float ItemsLayouter::GetItemExtent() const
{
  return GetImpl().GetItemExtent();
}

void ItemsLayouter::SetItemSpacing(float spacing)
{
  GetImpl().SetItemSpacing(spacing);
}

float ItemsLayouter::GetItemSpacing() const
{
  return GetImpl().GetItemSpacing();
}

} // namespace Ui
} // namespace Dali

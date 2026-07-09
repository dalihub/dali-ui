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

#include <dali-ui-foundation/public-api/views/recycler/group-body-decoration.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <limits>

namespace Dali
{
namespace Ui
{

GroupBodyDecoration::GroupBodyDecoration() = default;

GroupBodyDecoration::~GroupBodyDecoration()
{
  // Remove all background views from the scroller. This runs before
  // ItemDecoration::~ItemDecoration emits DestroyedSignal, so bgViews are
  // cleaned up even when the decoration is destroyed without a prior
  // RemoveItemDecoration() call (auto-remove path).
  for(auto& [gi, gs] : mGroupStates)
  {
    if(gs.bgView)
    {
      Dali::Actor parent = gs.bgView.GetParent();
      if(parent)
      {
        parent.Remove(gs.bgView);
      }
    }
  }
}

void GroupBodyDecoration::SetBodyColor(UiColor color)
{
  mBodyColor = color;
  for(auto& [gi, gs] : mGroupStates)
  {
    if(gs.bgView)
    {
      gs.bgView.SetBackgroundColor(mBodyColor);
    }
  }
}

void GroupBodyDecoration::SetBodyHorizontalMargin(float left, float right)
{
  mMarginLeft  = std::max(0.0f, left);
  mMarginRight = std::max(0.0f, right);
  mLayoutInvalidatedSignal.Emit();
}

void GroupBodyDecoration::SetBodyCornerRadius(float radius)
{
  mCornerRadius = std::max(0.0f, radius);
  for(auto& [gi, gs] : mGroupStates)
  {
    if(gs.bgView)
    {
      gs.bgView.SetCornerRadius(mCornerRadius);
    }
  }
}

Dali::Signal<void()>& GroupBodyDecoration::LayoutInvalidatedSignal()
{
  return mLayoutInvalidatedSignal;
}

ItemOffsets GroupBodyDecoration::GetItemOffsets(const ItemViewHolder& holder) const
{
  if(IsBodyType(holder.rowType))
  {
    return {mMarginLeft, 0.0f, mMarginRight, 0.0f};
  }
  return {};
}

bool GroupBodyDecoration::IsBodyType(GroupRowType rt) const
{
  return rt == GroupRowType::BODY_SINGLE ||
         rt == GroupRowType::BODY_TOP ||
         rt == GroupRowType::BODY_MIDDLE ||
         rt == GroupRowType::BODY_BOTTOM;
}

void GroupBodyDecoration::OnLayoutStart()
{
  for(auto& [gi, gs] : mGroupStates)
  {
    gs.topY    = std::numeric_limits<float>::max();
    gs.bottomY = std::numeric_limits<float>::lowest();
  }
}

void GroupBodyDecoration::OnItemActivated(ItemViewHolder& holder, View scroller)
{
  if(!IsBodyType(holder.rowType))
  {
    return;
  }

  const uint32_t gi = holder.groupIndex;
  GroupState&    gs = mGroupStates[gi];
  gs.activeBodyCount++;

  if(!gs.bgView)
  {
    gs.bgView = View::New();
    gs.bgView.SetBackgroundColor(mBodyColor);
    if(mCornerRadius > 0.0f)
    {
      gs.bgView.SetCornerRadius(mCornerRadius);
    }
    scroller.Add(gs.bgView);
    gs.bgView.LowerToBottom();
  }
}

void GroupBodyDecoration::OnItemRecycled(const ItemViewHolder& holder)
{
  if(!IsBodyType(holder.rowType))
  {
    return;
  }

  const uint32_t gi = holder.groupIndex;
  auto           it = mGroupStates.find(gi);
  if(it == mGroupStates.end())
  {
    return;
  }

  GroupState& gs = it->second;
  gs.activeBodyCount--;

  if(gs.activeBodyCount <= 0)
  {
    if(gs.bgView)
    {
      Dali::Actor parent = gs.bgView.GetParent();
      if(parent)
      {
        parent.Remove(gs.bgView);
      }
      gs.bgView.Reset();
    }
    mGroupStates.erase(it);
  }
}

void GroupBodyDecoration::OnItemBoundsUpdated(const ItemViewHolder& holder, const LayoutRect& bounds)
{
  if(!IsBodyType(holder.rowType))
  {
    return;
  }

  const uint32_t gi = holder.groupIndex;
  auto           it = mGroupStates.find(gi);
  if(it == mGroupStates.end() || !it->second.bgView)
  {
    return;
  }

  GroupState& gs = it->second;

  // bounds is slot-space: for a VERTICAL list, x=0 and width=full cross extent.
  // The item view is placed inside at x=mMarginLeft, width=bounds.width-mMarginLeft-mMarginRight
  // (via GetItemOffsets). Apply the same margins here so the card aligns with the items.
  const float cardX = bounds.x + mMarginLeft;
  const float cardW = std::max(0.0f, bounds.width - mMarginLeft - mMarginRight);

  // Track the vertical extent of ALL visible body items this pass.
  gs.topY    = std::min(gs.topY, bounds.y);
  gs.bottomY = std::max(gs.bottomY, bounds.y + bounds.height);

  const float bgH = gs.bottomY - gs.topY;
  if(bgH <= 0.0f || gs.topY == std::numeric_limits<float>::max())
  {
    return;
  }

  gs.bgView.SetProperty(Actor::Property::POSITION, Vector3(cardX, gs.topY, 0.0f));
  gs.bgView.SetProperty(Actor::Property::SIZE, Vector3(cardW, bgH, 1.0f));
}

} // namespace Ui
} // namespace Dali

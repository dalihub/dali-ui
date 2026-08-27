#pragma once

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

#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/recycler/item-decoration.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <limits>
#include <unordered_map>

namespace Dali
{
namespace Ui
{

/**
 * @brief ItemDecoration that draws a card background behind each group's body items.
 *
 * GroupBodyDecoration manages one background View per visible group. It tracks
 * active body items via OnItemActivated/OnItemRecycled and updates background
 * position/size via OnItemBoundsUpdated after each layout pass.
 *
 * Background views are added to the scroller below all item views (LowerToBottom).
 * The background spans from the top of BODY_TOP/BODY_SINGLE to the bottom of
 * BODY_BOTTOM/BODY_SINGLE, at the body horizontal margins set on the layouter.
 *
 * Usage:
 * @code
 *   GroupBodyDecoration deco;
 *   deco.SetBodyColor(UiColor(0xFFFFFF));
 *   deco.SetBodyHorizontalMargin(16.0f, 16.0f);
 *   deco.SetBodyCornerRadius(12.0f);
 *   recyclerView.AddItemDecoration(deco);
 * @endcode
 */
class DALI_UI_API GroupBodyDecoration : public ItemDecoration,
                                        public ItemDecorationInvalidationSupport
{
public:
  GroupBodyDecoration();
  ~GroupBodyDecoration() override;

  void SetBodyColor(UiColor color);
  // Sets the horizontal margins reserved around body items.
  // This value is returned from GetItemOffsets() and applied by the layouter.
  // Do NOT combine with GroupLinearItemsLayouter::SetBodyHorizontalMargin() on the
  // same RecyclerView — the two margins would be additive.
  void SetBodyHorizontalMargin(float left, float right);
  void SetBodyCornerRadius(float radius);

  ItemOffsets           GetItemOffsets(const ItemViewHolder& holder) const override;
  void                  OnLayoutStart() override;
  void                  OnItemActivated(ItemViewHolder& holder, View scroller) override;
  void                  OnItemRecycled(const ItemViewHolder& holder) override;
  void                  OnItemBoundsUpdated(const ItemViewHolder& holder, const LayoutRect& bounds) override;
  Dali::Signal<void()>& LayoutInvalidatedSignal() override;

private:
  bool IsBodyType(GroupRowType rt) const;

  struct GroupState
  {
    View  bgView;
    float topY{std::numeric_limits<float>::max()};
    float bottomY{std::numeric_limits<float>::lowest()};
    int   activeBodyCount{0};
  };

  UiColor mBodyColor{UiColor(0xFFFFFF)};
  float   mMarginLeft{0.0f};
  float   mMarginRight{0.0f};
  float   mCornerRadius{0.0f};

  std::unordered_map<uint32_t, GroupState> mGroupStates;
  Dali::Signal<void()>                     mLayoutInvalidatedSignal;
};

} // namespace Ui
} // namespace Dali

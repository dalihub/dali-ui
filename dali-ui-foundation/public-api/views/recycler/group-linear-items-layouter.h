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

#include <dali-ui-foundation/public-api/views/recycler/group-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/linear-items-layouter.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class GroupLinearItemsLayouterImpl;
}

/**
 * @brief LinearItemsLayouter handle that supports grouped item layout.
 *
 * Attach a GroupAdapter to enable horizontal body margins so that body items
 * appear inset within their group card. GAP, HEADER and non-body items are
 * laid out at full width.
 *
 * @note Currently only VERTICAL orientation is supported for group layout.
 *       Horizontal orientation compiles but GroupBodyDecoration and
 *       GetItemCrossInset use axis-specific assumptions that are not yet
 *       generalised for horizontal scrolling.
 *
 * @code
 *   GroupLinearItemsLayouter layouter = GroupLinearItemsLayouter::New(Orientation::VERTICAL);
 *   layouter.SetGroupAdapter(groupAdapter);
 *   layouter.SetBodyHorizontalMargin(16.0f, 16.0f);
 *   recyclerView.SetItemsLayouter(layouter);
 * @endcode
 */
class DALI_UI_API GroupLinearItemsLayouter : public LinearItemsLayouter
{
public:
  GroupLinearItemsLayouter()  = default;
  ~GroupLinearItemsLayouter() = default;

  GroupLinearItemsLayouter(const GroupLinearItemsLayouter&)            = default;
  GroupLinearItemsLayouter(GroupLinearItemsLayouter&&)                 = default;
  GroupLinearItemsLayouter& operator=(const GroupLinearItemsLayouter&) = default;
  GroupLinearItemsLayouter& operator=(GroupLinearItemsLayouter&&)      = default;

  static GroupLinearItemsLayouter New(Orientation orientation = Orientation::VERTICAL);

  // Must be called before attaching to RecyclerView. GroupAdapter provides the
  // row-type information needed by GetItemBounds.
  void SetGroupAdapter(GroupAdapter adapter);

  // Horizontal inset applied to BODY_* items. Decorations should match these margins.
  void  SetBodyHorizontalMargin(float left, float right);
  float GetBodyMarginLeft() const;
  float GetBodyMarginRight() const;

public: // Not intended for application developers
  /// @cond internal
  explicit GroupLinearItemsLayouter(Integration::ItemsLayouterImpl* impl);
  Internal::GroupLinearItemsLayouterImpl& GetGroupImpl();
  /// @endcond
};

} // namespace Ui
} // namespace Dali

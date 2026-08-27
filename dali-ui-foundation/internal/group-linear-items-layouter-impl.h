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

#include <dali-ui-foundation/internal/linear-items-layouter-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/group-adapter.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{

class GroupLinearItemsLayouterImpl;
using GroupLinearItemsLayouterImplPtr = IntrusivePtr<GroupLinearItemsLayouterImpl>;

/**
 * @brief LinearItemsLayouter subclass that applies horizontal body margins for group items.
 *
 * When a GroupAdapter is attached, GetItemBounds narrows the bounding rect for
 * BODY_* rows by the configured horizontal margins. This makes body items appear
 * inset within the group card while GAP and HEADER items span the full width.
 */
class DALI_UI_API GroupLinearItemsLayouterImpl : public LinearItemsLayouterImpl
{
public:
  static GroupLinearItemsLayouterImplPtr New(Orientation orientation);

  explicit GroupLinearItemsLayouterImpl(Orientation orientation);
  ~GroupLinearItemsLayouterImpl() override;

  void  SetGroupAdapter(GroupAdapter adapter);
  void  SetBodyHorizontalMargin(float left, float right);
  float GetBodyMarginLeft() const;
  float GetBodyMarginRight() const;

  // LinearItemsLayouterImpl overrides — apply body inset for grouped rows.
  LayoutRect GetItemBounds(uint32_t position, float crossExtent) const override;
  ItemInset  GetItemCrossInset(uint32_t position) const override;

private:
  GroupAdapter mGroupAdapter;
  float        mBodyMarginLeft{0.0f};
  float        mBodyMarginRight{0.0f};
};

} // namespace Internal
} // namespace Ui
} // namespace Dali

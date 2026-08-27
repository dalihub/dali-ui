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

#include <dali-ui-foundation/internal/group-linear-items-layouter-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/group-linear-items-layouter.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{

GroupLinearItemsLayouter GroupLinearItemsLayouter::New(Orientation orientation)
{
  if(orientation != Orientation::VERTICAL)
  {
    DALI_LOG_ERROR(
      "GroupLinearItemsLayouter: only VERTICAL orientation is supported. "
      "Forcing VERTICAL.\n");
    orientation = Orientation::VERTICAL;
  }
  Internal::GroupLinearItemsLayouterImplPtr impl = Internal::GroupLinearItemsLayouterImpl::New(orientation);
  return GroupLinearItemsLayouter(impl.Get());
}

GroupLinearItemsLayouter::GroupLinearItemsLayouter(Integration::ItemsLayouterImpl* impl)
: LinearItemsLayouter(impl)
{
}

Internal::GroupLinearItemsLayouterImpl& GroupLinearItemsLayouter::GetGroupImpl()
{
  return static_cast<Internal::GroupLinearItemsLayouterImpl&>(GetImpl());
}

void GroupLinearItemsLayouter::SetGroupAdapter(GroupAdapter adapter)
{
  GetGroupImpl().SetGroupAdapter(std::move(adapter));
}

void GroupLinearItemsLayouter::SetBodyHorizontalMargin(float left, float right)
{
  GetGroupImpl().SetBodyHorizontalMargin(left, right);
}

float GroupLinearItemsLayouter::GetBodyMarginLeft() const
{
  return static_cast<const Internal::GroupLinearItemsLayouterImpl&>(GetImpl()).GetBodyMarginLeft();
}

float GroupLinearItemsLayouter::GetBodyMarginRight() const
{
  return static_cast<const Internal::GroupLinearItemsLayouterImpl&>(GetImpl()).GetBodyMarginRight();
}

} // namespace Ui
} // namespace Dali

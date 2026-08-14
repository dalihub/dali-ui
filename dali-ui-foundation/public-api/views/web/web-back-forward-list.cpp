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

#include <dali-ui-foundation/integration-api/web-back-forward-list-impl.h>
#include <dali-ui-foundation/public-api/views/web/web-back-forward-list.h>

namespace Dali
{
namespace Ui
{

WebBackForwardList::WebBackForwardList()  = default;
WebBackForwardList::~WebBackForwardList() = default;

WebBackForwardList::WebBackForwardList(const WebBackForwardList& list)                = default;
WebBackForwardList::WebBackForwardList(WebBackForwardList&& list) noexcept            = default;
WebBackForwardList& WebBackForwardList::operator=(const WebBackForwardList& list)     = default;
WebBackForwardList& WebBackForwardList::operator=(WebBackForwardList&& list) noexcept = default;

WebBackForwardList WebBackForwardList::DownCast(BaseHandle handle)
{
  auto* impl = dynamic_cast<Integration::WebBackForwardListImpl*>(handle.GetObjectPtr());
  return impl ? WebBackForwardList(*impl) : WebBackForwardList();
}

WebBackForwardListItem WebBackForwardList::GetCurrentItem() const
{
  return Integration::GetImpl(*this).GetCurrentItem();
}

WebBackForwardListItem WebBackForwardList::GetPreviousItem() const
{
  return Integration::GetImpl(*this).GetPreviousItem();
}

WebBackForwardListItem WebBackForwardList::GetNextItem() const
{
  return Integration::GetImpl(*this).GetNextItem();
}

WebBackForwardListItem WebBackForwardList::GetItemAtIndex(uint32_t index) const
{
  return Integration::GetImpl(*this).GetItemAtIndex(index);
}

uint32_t WebBackForwardList::GetItemCount() const
{
  return Integration::GetImpl(*this).GetItemCount();
}

Dali::Vector<WebBackForwardListItem> WebBackForwardList::GetBackwardItems(int32_t limit) const
{
  return Integration::GetImpl(*this).GetBackwardItems(limit);
}

Dali::Vector<WebBackForwardListItem> WebBackForwardList::GetForwardItems(int32_t limit) const
{
  return Integration::GetImpl(*this).GetForwardItems(limit);
}

WebBackForwardList::WebBackForwardList(Integration::WebBackForwardListImpl& implementation)
: BaseHandle(&implementation)
{
}

} // namespace Ui
} // namespace Dali

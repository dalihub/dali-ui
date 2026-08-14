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
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list-item.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list.h>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Integration
{

WebBackForwardListImplPtr WebBackForwardListImpl::New(Dali::WebEngine webEngine)
{
  return new WebBackForwardListImpl(std::move(webEngine));
}

WebBackForwardListImpl::WebBackForwardListImpl(Dali::WebEngine webEngine)
: mWebEngine(webEngine)
{
}

WebBackForwardListImpl::~WebBackForwardListImpl() = default;

WebBackForwardListItem WebBackForwardListImpl::CreateItem(std::unique_ptr<Dali::WebEngineBackForwardListItem> item) const
{
  if(!item)
  {
    return {};
  }

  const std::string url         = item->GetUrl();
  const std::string title       = item->GetTitle();
  const std::string originalUrl = item->GetOriginalUrl();
  return WebBackForwardListItem(Dali::String(url.c_str()), Dali::String(title.c_str()), Dali::String(originalUrl.c_str()));
}

WebBackForwardListItem WebBackForwardListImpl::GetCurrentItem() const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? CreateItem(webEngine.GetBackForwardList().GetCurrentItem()) : WebBackForwardListItem();
}

WebBackForwardListItem WebBackForwardListImpl::GetPreviousItem() const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? CreateItem(webEngine.GetBackForwardList().GetPreviousItem()) : WebBackForwardListItem();
}

WebBackForwardListItem WebBackForwardListImpl::GetNextItem() const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? CreateItem(webEngine.GetBackForwardList().GetNextItem()) : WebBackForwardListItem();
}

WebBackForwardListItem WebBackForwardListImpl::GetItemAtIndex(uint32_t index) const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? CreateItem(webEngine.GetBackForwardList().GetItemAtIndex(index)) : WebBackForwardListItem();
}

uint32_t WebBackForwardListImpl::GetItemCount() const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? webEngine.GetBackForwardList().GetItemCount() : 0u;
}

Dali::Vector<WebBackForwardListItem> WebBackForwardListImpl::GetBackwardItems(int32_t limit) const
{
  Dali::Vector<WebBackForwardListItem> result;
  Dali::WebEngine                      webEngine = mWebEngine.GetHandle();
  if(!webEngine || !webEngine.GetPlugin())
  {
    return result;
  }

  auto items = webEngine.GetBackForwardList().GetBackwardItems(limit);
  result.Reserve(items.size());
  for(auto& item : items)
  {
    WebBackForwardListItem converted = CreateItem(std::move(item));
    if(converted.IsValid())
    {
      result.PushBack(std::move(converted));
    }
  }
  return result;
}

Dali::Vector<WebBackForwardListItem> WebBackForwardListImpl::GetForwardItems(int32_t limit) const
{
  Dali::Vector<WebBackForwardListItem> result;
  Dali::WebEngine                      webEngine = mWebEngine.GetHandle();
  if(!webEngine || !webEngine.GetPlugin())
  {
    return result;
  }

  auto items = webEngine.GetBackForwardList().GetForwardItems(limit);
  result.Reserve(items.size());
  for(auto& item : items)
  {
    WebBackForwardListItem converted = CreateItem(std::move(item));
    if(converted.IsValid())
    {
      result.PushBack(std::move(converted));
    }
  }
  return result;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

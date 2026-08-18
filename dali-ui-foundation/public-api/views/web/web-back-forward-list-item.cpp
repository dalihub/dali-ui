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

#include <dali-ui-foundation/public-api/views/web/web-back-forward-list-item.h>
#include <utility>

namespace Dali
{
namespace Ui
{

struct WebBackForwardListItem::Impl
{
  Impl() = default;

  Impl(Dali::String itemUrl, Dali::String itemTitle, Dali::String itemOriginalUrl)
  : valid(true),
    url(std::move(itemUrl)),
    title(std::move(itemTitle)),
    originalUrl(std::move(itemOriginalUrl))
  {
  }

  bool         valid{false};
  Dali::String url;
  Dali::String title;
  Dali::String originalUrl;
};

WebBackForwardListItem::WebBackForwardListItem()
: mImpl(new Impl())
{
}

WebBackForwardListItem::~WebBackForwardListItem() = default;

WebBackForwardListItem::WebBackForwardListItem(const WebBackForwardListItem& item)
: mImpl(item.mImpl ? new Impl(*item.mImpl) : new Impl())
{
}

WebBackForwardListItem::WebBackForwardListItem(WebBackForwardListItem&& item) noexcept = default;

WebBackForwardListItem& WebBackForwardListItem::operator=(const WebBackForwardListItem& item)
{
  if(this != &item)
  {
    mImpl.Reset(item.mImpl ? new Impl(*item.mImpl) : new Impl());
  }
  return *this;
}

WebBackForwardListItem& WebBackForwardListItem::operator=(WebBackForwardListItem&& item) noexcept = default;

bool WebBackForwardListItem::IsValid() const
{
  return mImpl && mImpl->valid;
}

Dali::String WebBackForwardListItem::GetUrl() const
{
  return mImpl ? mImpl->url : Dali::String();
}

Dali::String WebBackForwardListItem::GetTitle() const
{
  return mImpl ? mImpl->title : Dali::String();
}

Dali::String WebBackForwardListItem::GetOriginalUrl() const
{
  return mImpl ? mImpl->originalUrl : Dali::String();
}

WebBackForwardListItem::WebBackForwardListItem(Dali::String url, Dali::String title, Dali::String originalUrl)
: mImpl(new Impl(std::move(url), std::move(title), std::move(originalUrl)))
{
}

} // namespace Ui
} // namespace Dali

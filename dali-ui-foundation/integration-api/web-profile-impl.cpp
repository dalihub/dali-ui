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

#include <dali-ui-foundation/integration-api/web-cookie-manager-impl.h>
#include <dali-ui-foundation/integration-api/web-profile-impl.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

WebProfileImplPtr WebProfileImpl::New(Dali::WebEngineContext* context, Dali::WebEngineCookieManager* cookieManager)
{
  return new WebProfileImpl(context, cookieManager);
}

WebProfileImpl::WebProfileImpl(Dali::WebEngineContext* context, Dali::WebEngineCookieManager* cookieManager)
: mContext(context),
  mCookieManager(cookieManager)
{
}

WebProfileImpl::~WebProfileImpl() = default;

void WebProfileImpl::ClearCache()
{
  if(mContext)
  {
    mContext->ClearCache();
  }
}

WebCookieManager WebProfileImpl::GetCookieManager() const
{
  if(!mWebCookieManager)
  {
    WebCookieManagerImplPtr impl = WebCookieManagerImpl::New(mCookieManager);
    mWebCookieManager            = WebCookieManager(*impl);
  }
  return mWebCookieManager;
}

bool WebProfileImpl::IsEqual(const WebProfileImpl& rhs) const
{
  return mContext == rhs.mContext;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

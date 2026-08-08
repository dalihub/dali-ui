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
#include <dali-ui-foundation/public-api/views/web/web-cookie-manager.h>

namespace Dali
{
namespace Ui
{

WebCookieManager::WebCookieManager()  = default;
WebCookieManager::~WebCookieManager() = default;

WebCookieManager::WebCookieManager(const WebCookieManager& manager)                = default;
WebCookieManager::WebCookieManager(WebCookieManager&& manager) noexcept            = default;
WebCookieManager& WebCookieManager::operator=(const WebCookieManager& manager)     = default;
WebCookieManager& WebCookieManager::operator=(WebCookieManager&& manager) noexcept = default;

bool WebCookieManager::operator==(const WebCookieManager& rhs) const
{
  if(!*this || !rhs)
  {
    return BaseHandle::operator==(rhs);
  }
  return Integration::GetImpl(*this).IsEqual(Integration::GetImpl(rhs));
}

bool WebCookieManager::operator!=(const WebCookieManager& rhs) const
{
  return !(*this == rhs);
}

WebCookieManager WebCookieManager::DownCast(BaseHandle handle)
{
  auto* impl = dynamic_cast<Integration::WebCookieManagerImpl*>(handle.GetObjectPtr());
  return impl ? WebCookieManager(*impl) : WebCookieManager();
}

void WebCookieManager::ClearAllCookies()
{
  Integration::GetImpl(*this).ClearAllCookies();
}

WebCookieManager::WebCookieManager(Integration::WebCookieManagerImpl& implementation)
: BaseHandle(&implementation)
{
}

} // namespace Ui
} // namespace Dali

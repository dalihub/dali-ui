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

#include <dali-ui-foundation/integration-api/web-profile-impl.h>
#include <dali-ui-foundation/public-api/views/web/web-profile.h>

namespace Dali
{
namespace Ui
{

WebProfile::WebProfile()  = default;
WebProfile::~WebProfile() = default;

WebProfile::WebProfile(const WebProfile& profile)                = default;
WebProfile::WebProfile(WebProfile&& profile) noexcept            = default;
WebProfile& WebProfile::operator=(const WebProfile& profile)     = default;
WebProfile& WebProfile::operator=(WebProfile&& profile) noexcept = default;

bool WebProfile::operator==(const WebProfile& rhs) const
{
  if(!*this || !rhs)
  {
    return BaseHandle::operator==(rhs);
  }
  return Integration::GetImpl(*this).IsEqual(Integration::GetImpl(rhs));
}

bool WebProfile::operator!=(const WebProfile& rhs) const
{
  return !(*this == rhs);
}

WebProfile WebProfile::DownCast(BaseHandle handle)
{
  auto* impl = dynamic_cast<Integration::WebProfileImpl*>(handle.GetObjectPtr());
  return impl ? WebProfile(*impl) : WebProfile();
}

void WebProfile::ClearCache()
{
  Integration::GetImpl(*this).ClearCache();
}

WebCookieManager WebProfile::GetCookieManager() const
{
  return Integration::GetImpl(*this).GetCookieManager();
}

WebProfile::WebProfile(Integration::WebProfileImpl& implementation)
: BaseHandle(&implementation)
{
}

} // namespace Ui
} // namespace Dali

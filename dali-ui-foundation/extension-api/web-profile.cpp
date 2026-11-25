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

#include <dali-ui-foundation/extension-api/web-profile.h>
#include <dali-ui-foundation/integration-api/web-profile-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
namespace WebProfile
{

Ui::WebProfile New(const Dali::String& name, bool incognito, const Dali::String& injectedBundlePath)
{
  return Integration::WebProfileImpl::New(name, incognito, injectedBundlePath);
}

void SetAppId(Ui::WebProfile profile, const Dali::String& appId)
{
  Integration::GetImpl(profile).SetAppId(appId);
}

bool SetAppVersion(Ui::WebProfile profile, const Dali::String& version)
{
  return Integration::GetImpl(profile).SetAppVersion(version);
}

void SetTimeOffset(Ui::WebProfile profile, double offset)
{
  Integration::GetImpl(profile).SetTimeOffset(offset);
}

void SetTimeZoneOffset(Ui::WebProfile profile, double zoneOffset, double daylightSavingTime)
{
  Integration::GetImpl(profile).SetTimeZoneOffset(zoneOffset, daylightSavingTime);
}

bool SetCacheEnabled(Ui::WebProfile profile, bool enabled)
{
  return Integration::GetImpl(profile).SetCacheEnabled(enabled);
}

bool SetCertificateFilePath(Ui::WebProfile profile, const Dali::String& path)
{
  return Integration::GetImpl(profile).SetCertificateFilePath(path);
}

bool DeleteAllWebStorage(Ui::WebProfile profile)
{
  return Integration::GetImpl(profile).DeleteAllWebStorage();
}

bool DeleteAllWebIndexedDatabase(Ui::WebProfile profile)
{
  return Integration::GetImpl(profile).DeleteAllWebIndexedDatabase();
}

void SetMaxRefreshRate(Ui::WebProfile profile, int32_t rate)
{
  Integration::GetImpl(profile).SetMaxRefreshRate(rate);
}

void RegisterUrlSchemesAsCorsEnabled(Ui::WebProfile profile, const Dali::Vector<Dali::String>& schemes)
{
  Integration::GetImpl(profile).RegisterUrlSchemesAsCorsEnabled(schemes);
}

} // namespace WebProfile
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE

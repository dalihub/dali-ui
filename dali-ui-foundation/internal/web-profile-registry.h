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

#pragma once

#include <dali-ui-foundation/public-api/views/web/web-profile.h>
#include <dali/public-api/object/weak-handle.h>
#include <map>
#include <string>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
using WebProfileRegistry = std::map<std::string, Dali::WeakHandle<WebProfile>>;

inline void RegisterWebProfile(WebProfileRegistry& profiles, WebProfile profile)
{
  // Keep only weak references: views and cookie managers own live profiles.
  for(auto entry = profiles.begin(); entry != profiles.end();)
  {
    if(!entry->second.GetHandle())
    {
      entry = profiles.erase(entry);
    }
    else
    {
      ++entry;
    }
  }
  profiles[profile.GetName().CStr()] = Dali::WeakHandle<WebProfile>(profile);
}
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE

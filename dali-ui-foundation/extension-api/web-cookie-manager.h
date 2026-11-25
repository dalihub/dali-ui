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

#include <dali-ui-foundation/public-api/views/web/web-cookie-manager.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
namespace WebCookieManager
{

/**
 * @brief Signal emitted without arguments when this profile's cookies change.
 * @SINCE_3_0.3
 */
using CookieChangedSignalType = Dali::Signal<void()>;

/**
 * @brief Sets the directory for persistent storage of non-session cookies in SQLite.
 *
 * Call on the DALi event thread. Incognito profiles ignore the request.
 * Logs an error and does nothing when the profile has been destroyed or
 * the cookie store is unavailable.
 * @SINCE_3_0.3
 * @param[in] manager An initialized cookie manager
 * @param[in] directory Directory used for cookie storage
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void SetPersistentStorage(Ui::WebCookieManager manager, const Dali::String& directory);

/**
 * @brief Returns the signal emitted when this profile's cookies change.
 *
 * Access the signal and manage its connections on the DALi event thread.
 * The signal is emitted asynchronously on that thread. Multiple pending
 * changes may be delivered as one notification. The signal carries no
 * cookie identity or change type.
 *
 * Accessing the signal starts a watch and keeps its connections until the
 * profile is destroyed, even if all cookie manager handles are released.
 * Use the returned signal reference only while the profile is alive.
 * If the profile has been destroyed or the cookie store is unavailable,
 * this function logs an error and returns an inactive signal owned by
 * the supplied cookie manager.
 * @SINCE_3_0.3
 * @param[in] manager An initialized cookie manager
 * @return The cookie change signal
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API CookieChangedSignalType& CookieChangedSignal(Ui::WebCookieManager manager);

} // namespace WebCookieManager
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE

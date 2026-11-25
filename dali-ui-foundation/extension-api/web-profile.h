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

#include <dali-ui-foundation/public-api/views/web/web-profile.h>
#include <dali/public-api/common/dali-vector.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
namespace WebProfile
{

/**
 * @brief Creates a named profile with a native injected bundle.
 *
 * Call on the DALi event thread after application initialization and UiConfig::Apply().
 * Set the Tizen application ID with SetAppId() before loading content to start
 * the injected-bundle session. This extension depends on backend support.
 *
 * @SINCE_3_0.3
 * @param[in] name The case-sensitive profile lookup key
 * @param[in] incognito Whether browsing data is private
 * @param[in] injectedBundlePath Path to the shared library file, or an empty string for no bundle
 * @return The new profile, or an empty handle on failure or lack of support
 * @see Dali::Ui::WebProfile::New(const Dali::String&, bool)
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API Ui::WebProfile New(const Dali::String& name, bool incognito, const Dali::String& injectedBundlePath);

/**
 * @brief Sets the Tizen application ID before loading content.
 *
 * Required for injected-bundle session startup. Native application setup can
 * also affect process-wide state. Call on the DALi event thread. This operation
 * has no effect on unsupported backends or after context termination.
 *
 * @SINCE_3_0.3
 * @param[in] profile The profile to configure
 * @param[in] appId Application identifier used to initialize the context
 * @pre profile is an initialized handle
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void SetAppId(Ui::WebProfile profile, const Dali::String& appId);

/**
 * @brief Sets the Tizen application version associated with a profile.
 *
 * Call on the DALi event thread after setting the application ID with SetAppId().
 *
 * @SINCE_3_0.3
 * @param[in] profile The profile to configure
 * @param[in] version Application version associated with the previously set application ID
 * @pre profile is an initialized handle
 * @return True if the backend accepts the version; false on rejection, lack of support, or termination
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API bool SetAppVersion(Ui::WebProfile profile, const Dali::String& version);

/**
 * @brief Adjusts the engine clock for the selected profile.
 * Chromium applies the offset to the browser process and forwards it to all
 * existing renderers. New renderers read their own profile's value.
 * Call on the DALi event thread. Unsupported or terminated backends ignore the request.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] offset Offset added to the current system time, in milliseconds
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void SetTimeOffset(Ui::WebProfile profile, double offset);

/**
 * @brief Sets the time-zone and daylight-saving offsets through the selected profile.
 * The change can affect the entire process. The backend may skip repeated values for this
 * profile and may ignore a request with both offsets zero. Call on the DALi event thread.
 * Unsupported or terminated backends ignore the request.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] zoneOffset Offset from UTC in milliseconds, positive east of UTC
 * @param[in] daylightSavingTime Daylight-saving adjustment in milliseconds added to zoneOffset
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void SetTimeZoneOffset(Ui::WebProfile profile, double zoneOffset, double daylightSavingTime);

/**
 * @brief Enables or disables resource caching without clearing existing data.
 *
 * Returns false if the backend rejects the request or lacks result support.
 * Call on the DALi event thread. After context termination, this operation
 * returns false.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] enabled True to enable caching, false to disable it
 * @return True if the backend accepts the request; false on rejection, lack of support, or termination
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API bool SetCacheEnabled(Ui::WebProfile profile, bool enabled);

/**
 * @brief Sets CA certificate files, directories or a colon-separated list.
 *
 * This configures CA trust, independently of TLS client certificates.
 * Chromium imports into a shared certificate database, so trust is not isolated
 * by profile. True reports request acceptance, not successful certificate import.
 * Returns false on rejection or unsupported operation.
 * Call on the DALi event thread. After context termination, this operation
 * returns false.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] path CA certificate file, directory, or colon-separated list of paths
 * @return True if the import request is accepted, not necessarily completed; false on rejection, lack of support, or termination
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API bool SetCertificateFilePath(Ui::WebProfile profile, const Dali::String& path);

/**
 * @brief Requests deletion of all web storage belonging to this profile.
 *
 * True means request acceptance, not deletion completion. Returns false on rejection or unsupported operation.
 * Call on the DALi event thread. After context termination, this operation
 * returns false.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @return True if the deletion request is accepted, not necessarily completed; false on rejection, lack of support, or termination
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API bool DeleteAllWebStorage(Ui::WebProfile profile);

/**
 * @brief Requests deletion of all IndexedDB data belonging to this profile.
 *
 * True means request acceptance, not deletion completion. Returns false on rejection or unsupported operation.
 * Call on the DALi event thread. After context termination, this operation
 * returns false.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @return True if the deletion request is accepted, not necessarily completed; false on rejection, lack of support, or termination
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API bool DeleteAllWebIndexedDatabase(Ui::WebProfile profile);

/**
 * @brief Requests the maximum refresh rate through this profile's context.
 *
 * Call on the DALi event thread, before creating views that should use the cap.
 * The web engine implementation may apply this setting across contexts.
 * Rates outside 1 to 60 FPS are ignored, as are requests on unsupported
 * backends or after context termination.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] rate Maximum frame rate from 1 to 60 FPS
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void SetMaxRefreshRate(Ui::WebProfile profile, int32_t rate);

/**
 * @brief Registers CORS-enabled URL schemes through this profile's context.
 *
 * Call on the DALi event thread. Does nothing after context termination.
 * The Chromium implementation stores the list process-wide; profile isolation
 * and immediate propagation to existing renderers are not guaranteed.
 * CORS registration does not register a request-interception callback.
 * @SINCE_3_0.3
 * @param[in] profile An initialized profile
 * @param[in] schemes URL schemes to register
 * @supported TIZEN_CHROMIUM
 */
DALI_UI_API void RegisterUrlSchemesAsCorsEnabled(Ui::WebProfile profile, const Dali::Vector<Dali::String>& schemes);

} // namespace WebProfile
} // namespace Extension
} // namespace Ui
} // namespace DALI_NAMESPACE

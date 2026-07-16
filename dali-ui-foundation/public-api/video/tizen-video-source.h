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

// EXTERNAL INCLUDES
#include <esplusplayer_capi/esplusplayer_capi.h>
#include <player.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/video/video-source.h>

namespace Dali
{
namespace Ui
{
namespace Tizen
{
static constexpr const char* MMPLAYER_PROVIDER_ID = "tizen.mmplayer";
static constexpr const char* ESPLAYER_PROVIDER_ID = "tizen.esplayer";

/**
 * @brief Wraps an externally created Tizen MMPlayer handle as a VideoSource for underlay (hole-punch)
 * rendering.
 *
 * The caller owns the native player lifecycle (create / prepare / destroy). The
 * returned VideoSource only describes the session; VideoView attaches it for display.
 *
 * @note Header-only so that the dali-ui library itself does not build-depend on the
 *       Tizen player headers; only the consuming application pulls them in. The
 *       VideoSource::New entry point it calls is exported (DALI_UI_API), so this links
 *       from an external app regardless of the library's symbol-visibility build.
 */
inline VideoSource CreateVideoSourceFromMMPlayerUnderlay(player_h player, const VideoSourceOptions& options = {})
{
  VideoSourceCapabilities capabilities;
  capabilities.flags = VideoSourceCapabilities::SupportsUnderlay |
                       VideoSourceCapabilities::SupportsSeek |
                       VideoSourceCapabilities::SupportsVolume;
  return VideoSource::New(MMPLAYER_PROVIDER_ID, static_cast<void*>(player), options, capabilities);
}

/**
 * @brief Wraps an externally created Tizen MMPlayer handle as a VideoSource for NativeImage rendering.
 *
 * Uses GPU texture rendering: decoded frames are pushed into a NativeImageSourceQueue and
 * rendered as a regular textured quad. Use this when underlay compositing is not available or
 * when a textured rendering pipeline is preferred.
 */
inline VideoSource CreateVideoSourceFromMMPlayerNativeImage(player_h player, const VideoSourceOptions& options = {})
{
  VideoSourceCapabilities capabilities;
  capabilities.flags = VideoSourceCapabilities::SupportsNativeImage |
                       VideoSourceCapabilities::SupportsSeek |
                       VideoSourceCapabilities::SupportsVolume;
  return VideoSource::New(MMPLAYER_PROVIDER_ID, static_cast<void*>(player), options, capabilities);
}

/**
 * @brief Wraps an externally created Tizen ESPlayer handle as a VideoSource for NativeImage rendering.
 *
 * Uses GPU texture rendering: decoded frames are pushed into a NativeImageSourceQueue and
 * rendered as a regular textured quad.
 */
inline VideoSource CreateVideoSourceFromESPlayerNativeImage(esplusplayer_handle player, const VideoSourceOptions& options = {})
{
  VideoSourceCapabilities capabilities;
  capabilities.flags = VideoSourceCapabilities::SupportsNativeImage;
  return VideoSource::New(ESPLAYER_PROVIDER_ID, static_cast<void*>(player), options, capabilities);
}

/**
 * @brief Wraps an externally created Tizen ESPlayer handle as a VideoSource for underlay (hole-punch)
 * rendering.
 *
 * The tcore/ecore backend distinction is a compile-time choice for dali-extension (a given build
 * supports exactly one), so the app does not need to select between them: EsVideoPlayer binds the
 * window via esplusplayer_set_display_window() (tcore) or esplusplayer_set_ecore_display() (ecore)
 * internally, whichever the linked dali-extension was built for.
 */
inline VideoSource CreateVideoSourceFromESPlayerUnderlay(esplusplayer_handle player, const VideoSourceOptions& options = {})
{
  VideoSourceCapabilities capabilities;
  capabilities.flags = VideoSourceCapabilities::SupportsUnderlay;
  return VideoSource::New(ESPLAYER_PROVIDER_ID, static_cast<void*>(player), options, capabilities);
}

} // namespace Tizen
} // namespace Ui
} // namespace Dali

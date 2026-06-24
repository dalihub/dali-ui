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
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class VideoSource;
}

enum class VideoSourceOwnership : uint32_t
{
  External = 0,
  Shared   = 1,
  Transfer = 2,
};

enum class VideoControlPolicy : uint32_t
{
  ViewControlsPlayback = 0,
  DisplayOnly          = 1,
};

struct VideoSourceCapabilities
{
  enum Flag : uint32_t
  {
    SupportsUnderlay    = 1u << 0,
    SupportsNativeImage = 1u << 1,
    SupportsSeek        = 1u << 2,
    SupportsVolume      = 1u << 3,
  };

  uint32_t flags{0u};
};

struct VideoSourceOptions
{
  VideoSourceOwnership ownership{VideoSourceOwnership::External};
  VideoControlPolicy   controlPolicy{VideoControlPolicy::ViewControlsPlayback};
};

class DALI_UI_API VideoSource : public BaseHandle
{
public:
  VideoSource();
  ~VideoSource();

  VideoSource(const VideoSource& rhs);
  VideoSource& operator=(const VideoSource& rhs);

  VideoSource(VideoSource&& rhs) noexcept;
  VideoSource& operator=(VideoSource&& rhs) noexcept;

  static VideoSource DownCast(BaseHandle handle);

  bool                    IsValid() const;
  VideoSourceCapabilities GetCapabilities() const;
  VideoSourceOwnership    GetOwnership() const;
  VideoControlPolicy      GetControlPolicy() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL VideoSource(Internal::VideoSource* internal);

  /**
   * @brief Creation entry point used by the platform VideoSource helpers.
   *
   * Exported (DALI_UI_API, not DALI_INTERNAL) so the header-only Tizen helpers in
   * devel-api can link against it even on symbol-visibility (release / GBS) builds.
   * Applications should use the platform helpers (e.g. Dali::Ui::Tizen::
   * CreateVideoSourceFromMMPlayer), not this directly.
   */
  static DALI_UI_API VideoSource New(const char*               providerId,
                                     void*                     nativeSession,
                                     const VideoSourceOptions& options,
                                     VideoSourceCapabilities   capabilities);
  /// @endcond
};

} // namespace Ui
} // namespace Dali

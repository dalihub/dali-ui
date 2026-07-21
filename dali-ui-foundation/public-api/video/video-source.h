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
#include <dali/public-api/object/any.h>
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

enum class VideoRenderingMode : uint32_t
{
  Underlay    = 0, ///< Platform-composited hole-punch; renders beneath the UI.
  NativeImage = 1, ///< Decoded frames become a GPU texture; supports UI render effects.
};

struct VideoSourceOptions
{
  VideoSourceOwnership ownership{VideoSourceOwnership::External};
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

  bool                 IsValid() const;
  VideoRenderingMode   GetRenderingMode() const;
  VideoSourceOwnership GetOwnership() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL VideoSource(Internal::VideoSource* internal);

  /**
   * @brief Creation entry point used by CreateVideoSource().
   *
   * Exported (DALI_UI_API, not DALI_INTERNAL) so the header-only CreateVideoSource()
   * bridge can link against it even on symbol-visibility (release / GBS) builds.
   * Applications should use CreateVideoSource() with a descriptor built by a
   * dali-adaptor platform helper, not this directly.
   */
  static DALI_UI_API VideoSource New(const char*               providerId,
                                     void*                     nativeSession,
                                     const VideoSourceOptions& options,
                                     VideoRenderingMode        renderingMode);
  /// @endcond
};

/**
 * @brief Creates a VideoSource from a platform video source descriptor.
 *
 * The application fills a Dali::VideoSourceDescriptor (declared in
 * <dali/public-api/adaptor-framework/video-source-descriptor.h>) with the provider
 * id, native session handle and capability flags, then passes it here:
 *
 * @code
 * Dali::VideoSourceDescriptor descriptor;
 * descriptor.SetProviderId("tizen.mmplayer");
 * descriptor.SetNativeSession(Dali::Any(static_cast<void*>(player)));
 * descriptor.SetRenderingMode(Dali::VideoRenderingMode::UNDERLAY);
 * VideoSource source = Dali::Ui::CreateVideoSource(descriptor);
 * @endcode
 *
 * The provider id, native session, and rendering mode are taken from the
 * descriptor as-is; ownership and control policy come from the caller-supplied
 * options (the descriptor's own ownership/control-policy fields are not read here).
 *
 * This is a template so that this header depends on neither the descriptor type
 * (Dali::VideoSourceDescriptor) nor any platform CAPI header; only the consuming
 * application pulls those in. The descriptor's rendering mode enum shares the
 * same underlying values as VideoRenderingMode, so it is mapped by value.
 *
 * @param[in] descriptor A platform video source descriptor
 * @param[in] options Ownership and control-policy options for the source
 * @return A VideoSource describing the platform player session
 */
template<typename SourceDescriptor>
inline VideoSource CreateVideoSource(const SourceDescriptor& descriptor, const VideoSourceOptions& options = {})
{
  const auto renderingMode = static_cast<VideoRenderingMode>(static_cast<uint32_t>(descriptor.GetRenderingMode()));
  return VideoSource::New(descriptor.GetProviderId().CStr(), AnyCast<void*>(descriptor.GetNativeSession()), options, renderingMode);
}

} // namespace Ui
} // namespace Dali

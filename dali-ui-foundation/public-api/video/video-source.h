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
#include <dali/public-api/adaptor-framework/video-source-descriptor.h>
#include <dali/public-api/common/dali-string-view.h>
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

// VideoSourceOwnership and VideoRenderingMode are Dali::VideoSourceOwnership and
// Dali::VideoRenderingMode (declared in video-source-descriptor.h above), found
// here through ordinary namespace lookup, not declared in Dali::Ui.

/**
 * @brief Describes an externally created native player session, used as the
 * source for a VideoView.
 *
 * A VideoSource is created with CreateVideoSource() and attached to a
 * VideoView with VideoView::New(VideoSource) or VideoView::SetSource().
 * VideoSource only carries the provider id, native session handle, and
 * rendering mode; VideoView does not read or write these once attached.
 */
class DALI_UI_API VideoSource : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized VideoSource handle.
   */
  VideoSource();

  /**
   * @brief Destructor.
   */
  ~VideoSource();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  VideoSource(const VideoSource& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  VideoSource& operator=(const VideoSource& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  VideoSource(VideoSource&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  VideoSource& operator=(VideoSource&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to a VideoSource handle.
   *
   * @param[in] handle The handle to downcast
   * @return A VideoSource handle, or an uninitialized handle if the cast fails
   */
  static VideoSource DownCast(BaseHandle handle);

  /**
   * @brief Retrieves whether this source has both a provider id and a native session.
   *
   * VideoView::SetSource() attaches the source only if this is true and the
   * handle itself is initialized.
   *
   * @return True if the source has a provider id and a native session
   */
  bool IsValid() const;

  /**
   * @brief Retrieves how this source is rendered into the scene.
   *
   * @return The rendering mode
   */
  VideoRenderingMode GetRenderingMode() const;

  /**
   * @brief Retrieves the native session ownership policy.
   *
   * @return The ownership policy
   */
  VideoSourceOwnership GetOwnership() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL VideoSource(Internal::VideoSource* internal);

  /**
   * @brief Creation entry point used by CreateVideoSource().
   *
   * Exported through the VideoSource DLL interface so the header-only
   * CreateVideoSource() bridge can link against it even on symbol-visibility
   * (release / GBS) builds.
   * Applications should use CreateVideoSource() with a descriptor built by a
   * dali-adaptor platform helper, not this directly.
   *
   * providerId is only read here; the source stores its own copy.
   */
  static VideoSource New(Dali::StringView     providerId,
                         void*                nativeSession,
                         VideoSourceOwnership ownership,
                         VideoRenderingMode   renderingMode);
  /// @endcond
};

/**
 * @brief Creates a VideoSource from a platform video source descriptor.
 *
 * The application fills a Dali::VideoSourceDescriptor (declared in
 * <dali/public-api/adaptor-framework/video-source-descriptor.h>) with the provider
 * id, native session handle, and rendering mode, then passes it here:
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
 * descriptor as-is; ownership comes from the caller-supplied argument (the
 * descriptor's own ownership field, if it has one, is not read here).
 *
 * This is a template so callers are not required to use Dali::VideoSourceDescriptor
 * specifically; any duck-typed descriptor with matching accessors works. The
 * descriptor's rendering mode is mapped by value in case a caller's descriptor
 * type uses a different (but underlying-value-compatible) rendering mode enum.
 *
 * @param[in] descriptor A platform video source descriptor
 * @param[in] ownership The native session ownership policy
 * @return A VideoSource describing the platform player session
 */
template<typename SourceDescriptor>
inline VideoSource CreateVideoSource(const SourceDescriptor& descriptor, VideoSourceOwnership ownership = VideoSourceOwnership::EXTERNAL)
{
  const auto renderingMode = static_cast<VideoRenderingMode>(static_cast<uint32_t>(descriptor.GetRenderingMode()));
  return VideoSource::New(descriptor.GetProviderId(), AnyCast<void*>(descriptor.GetNativeSession()), ownership, renderingMode);
}

} // namespace Ui
} // namespace Dali

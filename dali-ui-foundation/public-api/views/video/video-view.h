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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/video/video-source.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class VideoViewImpl;
}

/**
 * @brief Enumeration for whether the underlay video display is resynchronized with the
 * UI's transparent hole on every UI frame, instead of only when the video player commits
 * a newly decoded frame.
 *
 * With ENABLED, position/size changes (e.g. from an Animation) stay visually in sync with
 * the underlay video even while the source is paused or stopped, at the cost of extra
 * synchronization overhead. Only affects underlay-mode sources; NativeImage sources are
 * unaffected either way.
 * @SINCE_3_0.0
 */
enum class VideoSyncMode
{
  DISABLED = 0, ///< Default. The underlay video geometry updates only when the player commits a new frame. @SINCE_3_0.0
  ENABLED       ///< The underlay video geometry stays synchronized with the UI on every UI frame. @SINCE_3_0.0
};

/**
 * @brief View that displays video from a VideoSource.
 *
 * The VideoSource owns the provider selection details. VideoView only attaches
 * it to the adaptor video player and forwards playback commands.
 * @SINCE_3_0.0
 */
class DALI_UI_API VideoView : public View
{
public:
  /**
   * @brief Creates an uninitialized VideoView handle.
   * @SINCE_3_0.0
   */
  VideoView();

  /**
   * @brief Creates an initialized VideoView without a source.
   *
   * @SINCE_3_0.0
   * @return A handle to a newly allocated VideoView
   */
  static VideoView New();

  /**
   * @brief Creates an initialized VideoView with the given source.
   *
   * @SINCE_3_0.0
   * @param[in] source The source to display
   * @return A handle to a newly allocated VideoView
   */
  static VideoView New(VideoSource source);

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~VideoView();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  VideoView(const VideoView& videoView);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  VideoView(VideoView&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  VideoView& operator=(const VideoView& videoView);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  VideoView& operator=(VideoView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(VideoView)

public:
  /**
   * @brief Downcasts a handle to VideoView handle.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a VideoView or an uninitialized handle
   */
  static VideoView DownCast(BaseHandle handle);

  /**
   * @brief Sets the source to display.
   *
   * @SINCE_3_0.0
   * @param[in] source The source to attach
   * @return True if the source was valid and attached
   */
  bool SetSource(VideoSource source);

  /**
   * @brief Gets the currently attached source.
   *
   * @SINCE_3_0.0
   * @return The current source or an uninitialized handle
   */
  VideoSource GetSource() const;

  /**
   * @brief Clears the current source.
   * @SINCE_3_0.0
   */
  void ClearSource();

  /**
   * @brief Starts playback when a source is attached.
   * @SINCE_3_0.0
   */
  void Play();

  /**
   * @brief Pauses playback when a source is attached.
   * @SINCE_3_0.0
   */
  void Pause();

  /**
   * @brief Stops playback when a source is attached.
   * @SINCE_3_0.0
   */
  void Stop();

  /**
   * @brief Sets the synchronization mode between the UI and the underlay video display.
   *
   * Prefer calling this before SetSource()/New(source) attaches a source. If a source is
   * already attached, the change still takes effect immediately, but the video player is
   * recreated against the same source to do so.
   *
   * @SINCE_3_0.0
   * @param[in] syncMode The synchronization mode. Default is DISABLED.
   */
  void SetSyncMode(VideoSyncMode syncMode);

  /**
   * @brief Gets the current synchronization mode.
   *
   * @SINCE_3_0.0
   * @return The synchronization mode
   */
  VideoSyncMode GetSyncMode() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL VideoView(Integration::VideoViewImpl& implementation);

  explicit DALI_INTERNAL VideoView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE

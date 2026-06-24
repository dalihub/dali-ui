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
#include <dali-ui-foundation/public-api/video/video-source.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
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
 */
enum class VideoSyncMode
{
  DISABLED = 0, ///< Default. The underlay video geometry updates only when the player commits a new frame.
  ENABLED       ///< The underlay video geometry stays synchronized with the UI on every UI frame.
};

/**
 * @brief View that displays video from a VideoSource.
 *
 * The VideoSource owns the provider selection details. VideoView only attaches
 * it to the adaptor video player and forwards playback commands.
 */
class DALI_UI_API VideoView : public View
{
public:
  /**
   * @brief Creates an uninitialized VideoView handle.
   */
  VideoView();

  /**
   * @brief Creates an initialized VideoView without a source.
   *
   * @return A handle to a newly allocated VideoView
   */
  static VideoView New();

  /**
   * @brief Creates an initialized VideoView with the given source.
   *
   * @param[in] source The source to display
   * @return A handle to a newly allocated VideoView
   */
  static VideoView New(VideoSource source);

  /**
   * @brief Destructor.
   */
  ~VideoView();

  /**
   * @brief Copy constructor.
   */
  VideoView(const VideoView& videoView);

  /**
   * @brief Move constructor.
   */
  VideoView(VideoView&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  VideoView& operator=(const VideoView& videoView);

  /**
   * @brief Move assignment operator.
   */
  VideoView& operator=(VideoView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(VideoView)

public:
  /**
   * @brief Downcasts a handle to VideoView handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a VideoView or an uninitialized handle
   */
  static VideoView DownCast(BaseHandle handle);

  /**
   * @brief Sets the source to display.
   *
   * @param[in] source The source to attach
   * @return True if the source was valid and attached
   */
  bool SetSource(VideoSource source);

  /**
   * @brief Gets the currently attached source.
   *
   * @return The current source or an uninitialized handle
   */
  VideoSource GetSource() const;

  /**
   * @brief Clears the current source.
   */
  void ClearSource();

  /**
   * @brief Starts playback when a source is attached.
   */
  void Play();

  /**
   * @brief Pauses playback when a source is attached.
   */
  void Pause();

  /**
   * @brief Stops playback when a source is attached.
   */
  void Stop();

  /**
   * @brief Sets the synchronization mode between the UI and the underlay video display.
   *
   * Prefer calling this before SetSource()/New(source) attaches a source. If a source is
   * already attached, the change still takes effect immediately, but the video player is
   * recreated against the same source to do so.
   *
   * @param[in] syncMode The synchronization mode. Default is DISABLED.
   */
  void SetSyncMode(VideoSyncMode syncMode);

  /**
   * @brief Gets the current synchronization mode.
   *
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
} // namespace Dali

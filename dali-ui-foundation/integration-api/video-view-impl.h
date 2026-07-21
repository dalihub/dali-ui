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
#include <dali/devel-api/adaptor-framework/video-player.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property-notification.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/video/video-source.h>
#include <dali-ui-foundation/public-api/video/video-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class VideoViewImpl;
using VideoViewImplPtr = IntrusivePtr<VideoViewImpl>;

/**
 * @brief Internal implementation class for VideoView.
 */
class DALI_UI_API VideoViewImpl : public ViewImpl
{
protected:
  VideoViewImpl();
  ~VideoViewImpl() override;

public:
  static VideoViewImplPtr New();

public:
  bool        SetSource(VideoSource source);
  VideoSource GetSource() const;
  void        ClearSource();

  void Play();
  void Pause();
  void Stop();

  void              SetSyncMode(Ui::VideoSyncMode syncMode);
  Ui::VideoSyncMode GetSyncMode() const;

private:
  void       OnInitialize() override;
  LayoutRect OnArrange(const LayoutRect& bounds) override;
  void       OnSceneConnection(int depth) override;
  void       OnSceneDisconnection() override;

private:
  void CreateVideoPlayer();
  void AttachToScene();
  void DetachFromScene();
  void UpdateDisplayArea();

  /**
   * @brief Ensures the transparent underlay visual exists and is registered on the view.
   *
   * Underlay video is drawn by the platform player on the window surface below the
   * UI scene. A color visual with hole-punch blending punches a transparent hole over
   * the view bounds so the video becomes visible through the UI.
   */
  void EnsureUnderlayVisual();

  /**
   * @brief Unregisters the transparent underlay visual from the view.
   */
  void RemoveUnderlayVisual();

  /**
   * @brief Creates a NativeImage and an IMAGE visual that displays decoded video frames,
   * used when the video source's rendering mode is NativeImage (e.g. ESPlayer).
   */
  void EnsureNativeImageVisual();

  /**
   * @brief Unregisters the NativeImage visual and releases the NativeImage.
   */
  void RemoveNativeImageVisual();

  /**
   * @brief Connects property notifications and window signals that keep the display
   * area synchronized with the actor geometry.
   */
  void ConnectGeometrySync();

  /**
   * @brief Disconnects the geometry synchronization notifications and signals.
   */
  void DisconnectGeometrySync();

  /**
   * @brief Called when the actor world position/size/scale changes.
   */
  void OnGeometryChanged(Dali::PropertyNotification source);

  /**
   * @brief Called when the window is resized or rotated.
   */
  void OnWindowResized(Dali::Window window, Dali::Window::WindowSize size);

private:
  VideoSource mSource;
  VideoPlayer mVideoPlayer;

  Dali::Ui::Integration::Visual::Base mUnderlayVisual;

  Dali::NativeImagePtr                mNativeImagePtr;
  Dali::Ui::Integration::Visual::Base mNativeImageVisual;

  Dali::Window               mWindow;
  Dali::PropertyNotification mPositionNotification;
  Dali::PropertyNotification mSizeNotification;
  Dali::PropertyNotification mScaleNotification;

  Dali::VideoSyncMode mSyncMode;
  bool                mAttachedToScene;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali

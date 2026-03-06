/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/devel-api/controls/video-view/video-view-devel.h>
#include <dali-ui-foundation/internal/controls/video-view/video-view-impl.h>

namespace Dali
{
namespace Ui
{
namespace DevelVideoView
{
Any GetMediaPlayer(VideoView videoView)
{
  return Dali::Ui::GetImpl(videoView).GetMediaPlayer();
}

VideoView New(VideoSyncMode syncMode)
{
  VideoView videoView = Internal::VideoView::New(syncMode);
  return videoView;
}

void PlayAnimation(VideoView videoView, Animation animation)
{
  Dali::Ui::GetImpl(videoView).PlayAnimation(animation);
}

void SetAutoRotationEnabled(VideoView videoView, bool enable)
{
  Dali::Ui::GetImpl(videoView).SetAutoRotationEnabled(enable);
}

bool IsAutoRotationEnabled(VideoView videoView)
{
  return Dali::Ui::GetImpl(videoView).IsAutoRotationEnabled();
}

void SetLetterBoxEnabled(VideoView videoView, bool enable)
{
  Dali::Ui::GetImpl(videoView).SetLetterBoxEnabled(enable);
}

bool IsLetterBoxEnabled(VideoView videoView)
{
  return Dali::Ui::GetImpl(videoView).IsLetterBoxEnabled();
}

void SetFrameInterpolationInterval(VideoView videoView, float intervalSeconds)
{
  Dali::Ui::GetImpl(videoView).SetFrameInterpolationInterval(intervalSeconds);
}

float GetFrameInterpolationInterval(VideoView videoView)
{
  return Dali::Ui::GetImpl(videoView).GetFrameInterpolationInterval();
}

void EnableOffscreenFrameRendering(VideoView videoView, bool useCoverFrame)
{
  Dali::Ui::GetImpl(videoView).EnableOffscreenFrameRendering(useCoverFrame);
}

void SetVideoFrameBuffer(VideoView videoView, Dali::NativeImagePtr source)
{
  Dali::Ui::GetImpl(videoView).SetVideoFrameBuffer(source);
}

} // namespace DevelVideoView

} // namespace Ui

} // namespace Dali

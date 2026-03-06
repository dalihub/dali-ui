/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-ui-foundation/public-api/controls/video-view/video-view.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/video-view/video-view-impl.h>

namespace Dali
{
namespace Ui
{
VideoView::VideoView()
{
}

VideoView::VideoView(const VideoView& videoView) = default;

VideoView::VideoView(VideoView&& rhs) noexcept = default;

VideoView& VideoView::operator=(const VideoView& videoView) = default;

VideoView& VideoView::operator=(VideoView&& rhs) noexcept = default;

VideoView::~VideoView()
{
}

VideoView VideoView::New()
{
  return Internal::VideoView::New(Dali::VideoSyncMode::DISABLED);
}

VideoView VideoView::New(const std::string& url)
{
  VideoView videoView = Internal::VideoView::New(Dali::VideoSyncMode::DISABLED);
  Dali::Ui::GetImpl(videoView).SetUrl(url);
  return videoView;
}

VideoView VideoView::New(bool swCodec)
{
  VideoView videoView = Internal::VideoView::New(Dali::VideoSyncMode::DISABLED);
  Dali::Ui::GetImpl(videoView).SetSWCodec(swCodec);
  return videoView;
}

VideoView VideoView::New(const std::string& url, bool swCodec)
{
  VideoView videoView = Internal::VideoView::New(Dali::VideoSyncMode::DISABLED);
  Dali::Ui::GetImpl(videoView).SetUrl(url);
  Dali::Ui::GetImpl(videoView).SetSWCodec(swCodec);
  return videoView;
}

VideoView VideoView::DownCast(BaseHandle handle)
{
  return Control::DownCast<VideoView, Internal::VideoView>(handle);
}

void VideoView::Play()
{
  Dali::Ui::GetImpl(*this).Play();
}

void VideoView::Pause()
{
  Dali::Ui::GetImpl(*this).Pause();
}

void VideoView::Stop()
{
  Dali::Ui::GetImpl(*this).Stop();
}

void VideoView::Forward(int millisecond)
{
  Dali::Ui::GetImpl(*this).Forward(millisecond);
}

void VideoView::Backward(int millisecond)
{
  Dali::Ui::GetImpl(*this).Backward(millisecond);
}

VideoView::VideoViewSignalType& VideoView::FinishedSignal()
{
  return Dali::Ui::GetImpl(*this).FinishedSignal();
}

VideoView::VideoView(Internal::VideoView& implementation)
  : Control(implementation)
{
}

VideoView::VideoView(Dali::Internal::CustomActor* internal)
  : Control(internal)
{
  VerifyCustomActorPointer<Internal::VideoView>(internal);
}

} // namespace Ui

} // namespace Dali

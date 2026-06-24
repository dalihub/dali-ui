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

// CLASS HEADER
#include <dali-ui-foundation/public-api/video/video-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/video-view-impl.h>

namespace Dali
{
namespace Ui
{
namespace
{
inline Integration::VideoViewImpl& GetImpl(VideoView& videoView)
{
  DALI_ASSERT_ALWAYS(videoView);
  return static_cast<Integration::VideoViewImpl&>(videoView.GetImplementation());
}

inline const Integration::VideoViewImpl& GetImpl(const VideoView& videoView)
{
  DALI_ASSERT_ALWAYS(videoView);
  return static_cast<const Integration::VideoViewImpl&>(videoView.GetImplementation());
}
} // namespace

VideoView::VideoView()                                      = default;
VideoView::~VideoView()                                     = default;
VideoView::VideoView(const VideoView& videoView)            = default;
VideoView::VideoView(VideoView&& rhs) noexcept              = default;
VideoView& VideoView::operator=(const VideoView& videoView) = default;
VideoView& VideoView::operator=(VideoView&& rhs) noexcept   = default;

VideoView VideoView::New()
{
  Integration::VideoViewImplPtr impl = Integration::VideoViewImpl::New();
  VideoView                     view(*impl);
  impl->Initialize();
  return view;
}

VideoView VideoView::New(VideoSource source)
{
  VideoView view = New();
  view.SetSource(source);
  return view;
}

VideoView VideoView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<VideoView, Ui::Integration::VideoViewImpl>(handle);
}

bool VideoView::SetSource(VideoSource source)
{
  return GetImpl(*this).SetSource(source);
}

VideoSource VideoView::GetSource() const
{
  return GetImpl(*this).GetSource();
}

void VideoView::ClearSource()
{
  GetImpl(*this).ClearSource();
}

void VideoView::Play()
{
  GetImpl(*this).Play();
}

void VideoView::Pause()
{
  GetImpl(*this).Pause();
}

void VideoView::Stop()
{
  GetImpl(*this).Stop();
}

void VideoView::SetSyncMode(VideoSyncMode syncMode)
{
  GetImpl(*this).SetSyncMode(syncMode);
}

VideoSyncMode VideoView::GetSyncMode() const
{
  return GetImpl(*this).GetSyncMode();
}

VideoView::VideoView(Integration::VideoViewImpl& implementation)
: View(implementation)
{
}

VideoView::VideoView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::VideoViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali

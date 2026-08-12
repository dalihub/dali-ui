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
#include <dali-ui-foundation/public-api/video/video-source.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/video/video-source-impl.h>

namespace Dali
{
namespace Ui
{
VideoSource::VideoSource()
{
}

VideoSource::~VideoSource()
{
}

VideoSource::VideoSource(const VideoSource& rhs) = default;

VideoSource& VideoSource::operator=(const VideoSource& rhs) = default;

VideoSource::VideoSource(VideoSource&& rhs) noexcept = default;

VideoSource& VideoSource::operator=(VideoSource&& rhs) noexcept = default;

VideoSource VideoSource::DownCast(BaseHandle handle)
{
  return VideoSource(dynamic_cast<Ui::Internal::VideoSource*>(handle.GetObjectPtr()));
}

bool VideoSource::IsValid() const
{
  return *this && GetImpl(*this).IsValid();
}

VideoRenderingMode VideoSource::GetRenderingMode() const
{
  return GetImpl(*this).GetRenderingMode();
}

VideoSourceOwnership VideoSource::GetOwnership() const
{
  return GetImpl(*this).GetOwnership();
}

VideoSource::VideoSource(Internal::VideoSource* internal)
: BaseHandle(internal)
{
}

VideoSource VideoSource::New(const char*          providerId,
                             void*                nativeSession,
                             VideoSourceOwnership ownership,
                             VideoRenderingMode   renderingMode)
{
  if(providerId == nullptr || nativeSession == nullptr)
  {
    return VideoSource();
  }

  Ui::Internal::VideoSourcePtr internal = Ui::Internal::VideoSource::New(providerId, nativeSession, ownership, renderingMode);
  return VideoSource(internal.Get());
}

} // namespace Ui
} // namespace Dali

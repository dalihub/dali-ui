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
#include <dali-ui-foundation/internal/video/video-source-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
VideoSourcePtr VideoSource::New(const char*          providerId,
                                void*                nativeSession,
                                VideoSourceOwnership ownership,
                                VideoRenderingMode   renderingMode)
{
  return new VideoSource(providerId, nativeSession, ownership, renderingMode);
}

VideoSource::VideoSource(const char*          providerId,
                         void*                nativeSession,
                         VideoSourceOwnership ownership,
                         VideoRenderingMode   renderingMode)
: mProviderId(providerId),
  mNativeSession(nativeSession),
  mOwnership(ownership),
  mRenderingMode(renderingMode)
{
}

VideoSource::~VideoSource()
{
}

bool VideoSource::IsValid() const
{
  return mProviderId != nullptr && !mNativeSession.Empty();
}

VideoRenderingMode VideoSource::GetRenderingMode() const
{
  return mRenderingMode;
}

VideoSourceOwnership VideoSource::GetOwnership() const
{
  return mOwnership;
}

Dali::VideoPlayerPlugin::VideoSourceDescriptor VideoSource::ToAdaptorDescriptor() const
{
  Dali::VideoPlayerPlugin::VideoSourceDescriptor descriptor;
  descriptor.SetProviderId(mProviderId);
  descriptor.SetNativeSession(mNativeSession);
  descriptor.SetOwnership(mOwnership);
  descriptor.SetRenderingMode(mRenderingMode);
  return descriptor;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

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
#include <dali/devel-api/adaptor-framework/video-player-plugin.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/video/video-source.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class VideoSource;
using VideoSourcePtr = IntrusivePtr<VideoSource>;

class VideoSource : public BaseObject
{
public:
  static VideoSourcePtr New(const char*               providerId,
                            void*                     nativeSession,
                            const VideoSourceOptions& options,
                            VideoRenderingMode        renderingMode);

  bool                 IsValid() const;
  VideoRenderingMode   GetRenderingMode() const;
  VideoSourceOwnership GetOwnership() const;

  Dali::VideoPlayerPlugin::VideoSourceDescriptor ToAdaptorDescriptor() const;

protected:
  virtual ~VideoSource();

private:
  VideoSource(const char*               providerId,
              void*                     nativeSession,
              const VideoSourceOptions& options,
              VideoRenderingMode        renderingMode);

  VideoSource(const VideoSource&)            = delete;
  VideoSource& operator=(const VideoSource&) = delete;

private:
  const char*          mProviderId;
  Any                  mNativeSession;
  VideoSourceOwnership mOwnership;
  VideoRenderingMode   mRenderingMode;
};

} // namespace Internal
} // namespace Ui

inline Ui::Internal::VideoSource& GetImpl(Dali::Ui::VideoSource& source)
{
  DALI_ASSERT_ALWAYS(source && "VideoSource handle is empty");

  BaseObject& handle = source.GetBaseObject();
  return static_cast<Ui::Internal::VideoSource&>(handle);
}

inline const Ui::Internal::VideoSource& GetImpl(const Dali::Ui::VideoSource& source)
{
  DALI_ASSERT_ALWAYS(source && "VideoSource handle is empty");

  const BaseObject& handle = source.GetBaseObject();
  return static_cast<const Ui::Internal::VideoSource&>(handle);
}

} // namespace Dali

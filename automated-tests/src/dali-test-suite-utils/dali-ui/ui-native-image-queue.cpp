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

#include <dali/devel-api/adaptor-framework/native-image-queue.h>

#include <dali/integration-api/gl-defines.h>
#include <cstring>
#include <string>
#include <vector>

#include <dali-test-suite-utils.h>

namespace DALI_NAMESPACE
{
/********************************************************************************
 * Stub for Dali::Internal::Adaptor::NativeImageQueue
 *
 * The real one reaches into the adaptor for its EGL graphics, which the harness has
 * replaced with a stub of an entirely different shape. This stands in for it with a
 * plain CPU buffer ring, so the code under test gets a queue that behaves.
 ********************************************************************************/
namespace Internal
{
namespace Adaptor
{
class NativeImageQueue
{
public:
  NativeImageQueue(uint32_t queueCount, uint32_t width, uint32_t height, Dali::NativeImageQueue::ColorFormat colorFormat)
  : mQueueCount(queueCount),
    mWidth(width),
    mHeight(height),
    mColorFormat(colorFormat)
  {
    Resize();
  }

  void Resize()
  {
    mBuffers.resize(mQueueCount);
    for(auto& buffer : mBuffers)
    {
      buffer.assign(static_cast<size_t>(mWidth) * mHeight * BYTES_PER_PIXEL, 0u);
    }
  }

  static constexpr uint32_t BYTES_PER_PIXEL = 4u;
  static constexpr uint32_t DEFAULT_COUNT   = 3u;

  uint32_t                           mQueueCount;
  uint32_t                           mWidth;
  uint32_t                           mHeight;
  Dali::NativeImageQueue::ColorFormat mColorFormat;

  std::vector<std::vector<uint8_t>> mBuffers;
  uint32_t                          mNextBuffer{0u};
  uint8_t*                          mDequeued{nullptr};
};

} // namespace Adaptor
} // namespace Internal

/********************************************************************************
 * Stub for Dali::NativeImageQueue
 ********************************************************************************/

NativeImageQueuePtr NativeImageQueue::New(uint32_t width, uint32_t height, ColorFormat colorFormat)
{
  return New(Internal::Adaptor::NativeImageQueue::DEFAULT_COUNT, width, height, colorFormat);
}

NativeImageQueuePtr NativeImageQueue::New(uint32_t queueCount, uint32_t width, uint32_t height, ColorFormat colorFormat)
{
  return NativeImageQueuePtr(new NativeImageQueue(queueCount, width, height, colorFormat, Any()));
}

NativeImageQueuePtr NativeImageQueue::New(Any nativeImageQueue)
{
  // There is no platform queue type to adopt here.
  return NativeImageQueuePtr();
}

NativeImageQueue::NativeImageQueue(uint32_t queueCount, uint32_t width, uint32_t height, ColorFormat colorFormat, Any nativeImageQueue)
: mImpl(new Internal::Adaptor::NativeImageQueue(queueCount, width, height, colorFormat))
{
}

NativeImageQueue::~NativeImageQueue()
{
  delete mImpl;
}

Any NativeImageQueue::GetNativeImageQueue()
{
  return Any();
}

void NativeImageQueue::SetSize(uint32_t width, uint32_t height)
{
  if(mImpl->mWidth != width || mImpl->mHeight != height)
  {
    mImpl->mWidth  = width;
    mImpl->mHeight = height;
    mImpl->Resize();
  }
}

void NativeImageQueue::IgnoreSourceImage()
{
}

bool NativeImageQueue::CanDequeueBuffer()
{
  return mImpl->mDequeued == nullptr;
}

uint8_t* NativeImageQueue::DequeueBuffer(uint32_t& width, uint32_t& height, uint32_t& stride)
{
  return DequeueBuffer(width, height, stride, BufferAccessType::WRITE);
}

uint8_t* NativeImageQueue::DequeueBuffer(uint32_t& width, uint32_t& height, uint32_t& stride, BufferAccessType type)
{
  if(mImpl->mDequeued || mImpl->mBuffers.empty())
  {
    return nullptr;
  }

  auto& buffer     = mImpl->mBuffers[mImpl->mNextBuffer];
  mImpl->mNextBuffer = (mImpl->mNextBuffer + 1u) % static_cast<uint32_t>(mImpl->mBuffers.size());

  width  = mImpl->mWidth;
  height = mImpl->mHeight;
  stride = mImpl->mWidth * Internal::Adaptor::NativeImageQueue::BYTES_PER_PIXEL;

  mImpl->mDequeued = buffer.data();
  return mImpl->mDequeued;
}

bool NativeImageQueue::EnqueueBuffer(uint8_t* buffer)
{
  if(mImpl->mDequeued != buffer)
  {
    return false;
  }
  mImpl->mDequeued = nullptr;
  return true;
}

void NativeImageQueue::CancelDequeuedBuffer(uint8_t* buffer)
{
  if(mImpl->mDequeued == buffer)
  {
    mImpl->mDequeued = nullptr;
  }
}

void NativeImageQueue::FreeReleasedBuffers()
{
}

void NativeImageQueue::SetQueueUsageHint(QueueUsageType type)
{
}

uint32_t NativeImageQueue::GetQueueCount() const
{
  return mImpl->mQueueCount;
}

bool NativeImageQueue::CreateResource()
{
  return true;
}

void NativeImageQueue::DestroyResource()
{
}

uint32_t NativeImageQueue::TargetTexture()
{
  return 0u;
}

Dali::NativeImageInterface::PrepareTextureResult NativeImageQueue::PrepareTexture()
{
  return Dali::NativeImageInterface::PrepareTextureResult::IMAGE_CHANGED;
}

uint32_t NativeImageQueue::GetWidth() const
{
  return mImpl->mWidth;
}

uint32_t NativeImageQueue::GetHeight() const
{
  return mImpl->mHeight;
}

bool NativeImageQueue::RequiresBlending() const
{
  return mImpl->mColorFormat == ColorFormat::RGBA8888 || mImpl->mColorFormat == ColorFormat::BGRA8888;
}

int NativeImageQueue::GetTextureTarget() const
{
  return GL_TEXTURE_EXTERNAL_OES;
}

bool NativeImageQueue::ApplyNativeFragmentShader(Dali::String& shaderStr, int mask)
{
  std::string shader = Integration::ToStdString(shaderStr);

  shader = "#extension GL_OES_EGL_image_external:require\n" + shader;

  const char* customSamplerTypename = GetCustomSamplerTypename();
  if(customSamplerTypename)
  {
    size_t pos = 0;
    while(mask && (pos = shader.find("sampler2D", pos)) != std::string::npos)
    {
      if(mask & 1)
      {
        shader.replace(pos, strlen("sampler2D"), customSamplerTypename);
        pos += strlen(customSamplerTypename);
      }
      else
      {
        pos += strlen("sampler2D");
      }
      mask >>= 1;
    }
  }

  shaderStr = Integration::ToDaliString(shader);
  return true;
}

const char* NativeImageQueue::GetCustomSamplerTypename() const
{
  return "samplerExternalOES";
}

Any NativeImageQueue::GetNativeImageHandle() const
{
  return nullptr;
}

bool NativeImageQueue::SourceChanged() const
{
  return true;
}

Rect<uint32_t> NativeImageQueue::GetUpdatedArea()
{
  return Rect<uint32_t>(0u, 0u, mImpl->mWidth, mImpl->mHeight);
}

void NativeImageQueue::PostRender()
{
}

NativeImageInterface::Extension* NativeImageQueue::GetExtension()
{
  return nullptr;
}

} // namespace Dali

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

#include <dali/integration-api/adaptor-framework/native-image-surface.h>

#include <dali/devel-api/adaptor-framework/native-image-queue.h>

namespace DALI_NAMESPACE
{
/********************************************************************************
 * Stub for Dali::Internal::Adaptor::NativeImageSurface
 *
 * Holds no graphics resources: it reports that it can render, and hands the queue
 * back a buffer for every frame, which is all the code under test asks of it.
 ********************************************************************************/
namespace Internal
{
namespace Adaptor
{
class NativeImageSurface
{
public:
  NativeImageSurface(Dali::NativeImageQueuePtr queue)
  : mQueue(queue)
  {
  }

  Dali::NativeImageQueuePtr mQueue;
  uint8_t*                  mBuffer{nullptr};
  bool                      mGraphicsInitialized{false};
};

} // namespace Adaptor
} // namespace Internal

/********************************************************************************
 * Stub for Dali::NativeImageSurface
 ********************************************************************************/

NativeImageSurfacePtr NativeImageSurface::New(Dali::NativeImageQueuePtr queue)
{
  return NativeImageSurfacePtr(new NativeImageSurface(queue));
}

NativeImageSurface::NativeImageSurface(Dali::NativeImageQueuePtr queue)
: mImpl(new Internal::Adaptor::NativeImageSurface(queue))
{
}

NativeImageSurface::~NativeImageSurface() = default;

Any NativeImageSurface::GetNativeRenderable()
{
  return Any();
}

void NativeImageSurface::InitializeGraphics()
{
  mImpl->mGraphicsInitialized = true;
}

void NativeImageSurface::TerminateGraphics()
{
  mImpl->mGraphicsInitialized = false;
}

void NativeImageSurface::PreRender()
{
  if(mImpl->mQueue && !mImpl->mBuffer)
  {
    uint32_t width = 0u, height = 0u, stride = 0u;
    mImpl->mBuffer = mImpl->mQueue->DequeueBuffer(width, height, stride);
  }
}

void NativeImageSurface::PostRender()
{
  if(mImpl->mQueue && mImpl->mBuffer)
  {
    mImpl->mQueue->EnqueueBuffer(mImpl->mBuffer);
    mImpl->mBuffer = nullptr;
  }
}

bool NativeImageSurface::CanRender()
{
  return mImpl->mGraphicsInitialized && mImpl->mBuffer != nullptr;
}

bool NativeImageSurface::SetGraphicsConfig(bool depth, bool stencil, int msaa, int version)
{
  return true;
}

} // namespace Dali

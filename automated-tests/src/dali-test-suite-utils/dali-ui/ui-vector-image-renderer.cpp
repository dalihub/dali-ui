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

// Mock implementation of Dali::VectorImageRenderer (SVG rasterization) used by the dali-ui
// automated-tests. See ui-canvas-renderer.cpp / ui-vector-animation-renderer.cpp for why this
// mock exists: the real implementation is backed by thorvg, which may not be installed on the
// test machine. Mirrors dali-toolkit-test-utils/toolkit-vector-image-renderer.cpp.

#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class VectorImageRenderer : public Dali::BaseObject
{
public:
  bool Load(const Vector<uint8_t>& data)
  {
    mLoadSuccess = !data.Empty();
    return mLoadSuccess;
  }

  bool IsLoaded() const
  {
    return mLoadSuccess;
  }

  Dali::Devel::PixelBuffer Rasterize(uint32_t width, uint32_t height)
  {
    if(!mLoadSuccess)
    {
      return Dali::Devel::PixelBuffer();
    }
    return Devel::PixelBuffer::New(width, height, Dali::Pixel::RGBA8888);
  }

  void GetDefaultSize(uint32_t& width, uint32_t& height) const
  {
    width  = 100;
    height = 100;
  }

public:
  bool mLoadSuccess{false};
};

inline VectorImageRenderer& GetImplementation(Dali::VectorImageRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorImageRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::VectorImageRenderer&>(handle);
}

inline const VectorImageRenderer& GetImplementation(const Dali::VectorImageRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "VectorImageRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::VectorImageRenderer&>(handle);
}

} // namespace Adaptor
} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

VectorImageRenderer VectorImageRenderer::New()
{
  Internal::Adaptor::VectorImageRenderer* imageRenderer = new Internal::Adaptor::VectorImageRenderer();
  return VectorImageRenderer(imageRenderer);
}

VectorImageRenderer::VectorImageRenderer()
{
}

VectorImageRenderer::~VectorImageRenderer()
{
}

VectorImageRenderer::VectorImageRenderer(Internal::Adaptor::VectorImageRenderer* internal)
: BaseHandle(internal)
{
}

bool VectorImageRenderer::Load(const Vector<uint8_t>& data)
{
  return Internal::Adaptor::GetImplementation(*this).Load(data);
}

bool VectorImageRenderer::IsLoaded() const
{
  return Internal::Adaptor::GetImplementation(*this).IsLoaded();
}

Dali::Devel::PixelBuffer VectorImageRenderer::Rasterize(uint32_t width, uint32_t height)
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize(width, height);
}

void VectorImageRenderer::GetDefaultSize(uint32_t& width, uint32_t& height) const
{
  Internal::Adaptor::GetImplementation(*this).GetDefaultSize(width, height);
}

} // namespace Dali

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

// Mock implementation of Dali::CanvasRenderer used by the dali-ui automated-tests.
//
// The real implementation (dali-adaptor's Internal::Adaptor::CanvasRenderer) is backed by
// thorvg, which may not be installed on the test machine. Symbol interposition (this file is
// compiled directly into the test binary, ahead of libdali2-adaptor.so on the link line) lets
// the test suite exercise the CanvasRenderer/CanvasView API surface without depending on thorvg
// being present at all, matching the approach already used by dali-toolkit's
// dali-toolkit-test-utils/toolkit-canvas-renderer.cpp.

#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class CanvasRenderer : public Dali::BaseObject
{
public:
  CanvasRenderer(const Vector2& size)
  : mTexture(Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, size.width, size.height)),
    mSize(size),
    mViewBox(size)
  {
  }

  bool Commit()
  {
    return true;
  }

  bool IsCanvasChanged() const
  {
    return true;
  }

  Dali::Texture GetRasterizedTexture()
  {
    return mTexture;
  }

  bool Rasterize()
  {
    return true;
  }

  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    // Matches the real implementation (canvas-renderer-impl.cpp), which asserts via the
    // Drawable's own GetImplementation() before checking thorvg support.
    DALI_ASSERT_ALWAYS(drawable && "Drawable handle is empty.");
    mDrawable = &drawable;
    return true;
  }

  bool RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
  {
    if(!drawable || mDrawable != &drawable)
    {
      return false;
    }
    mDrawable = nullptr;
    return true;
  }

  bool RemoveAllDrawables()
  {
    if(!mDrawable)
    {
      return false;
    }
    mDrawable = nullptr;
    return true;
  }

  bool SetSize(Vector2 size)
  {
    mSize = size;
    return true;
  }

  Vector2 GetSize() const
  {
    return mSize;
  }

  bool SetViewBox(const Vector2& viewBox)
  {
    mViewBox = viewBox;
    return true;
  }

  const Vector2& GetViewBox()
  {
    return mViewBox;
  }

  bool SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
  {
    mHasDropShadow  = true;
    mHasGaussianBlur = false;
    return true;
  }

  bool ClearDropShadow()
  {
    mHasDropShadow = false;
    return true;
  }

  bool HasDropShadow() const
  {
    return mHasDropShadow;
  }

  bool SetGaussianBlur(float blurRadius)
  {
    mHasGaussianBlur = true;
    mHasDropShadow   = false;
    return true;
  }

  bool ClearGaussianBlur()
  {
    mHasGaussianBlur = false;
    return true;
  }

  bool HasGaussianBlur() const
  {
    return mHasGaussianBlur;
  }

  void SetEffectAutoPaddingEnabled(bool enable)
  {
    mEffectAutoPaddingEnabled = enable;
  }

  bool IsEffectAutoPaddingEnabled() const
  {
    return mEffectAutoPaddingEnabled;
  }

public:
  Dali::CanvasRenderer::Drawable* mDrawable{nullptr};
  Dali::Texture                   mTexture;
  Vector2                         mSize;
  Vector2                         mViewBox;
  bool                            mHasDropShadow{false};
  bool                            mHasGaussianBlur{false};
  bool                            mEffectAutoPaddingEnabled{true};
};

inline CanvasRenderer& GetImplementation(Dali::CanvasRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "CanvasRenderer handle is empty.");
  BaseObject& handle = renderer.GetBaseObject();
  return static_cast<Internal::Adaptor::CanvasRenderer&>(handle);
}

inline const CanvasRenderer& GetImplementation(const Dali::CanvasRenderer& renderer)
{
  DALI_ASSERT_ALWAYS(renderer && "CanvasRenderer handle is empty.");
  const BaseObject& handle = renderer.GetBaseObject();
  return static_cast<const Internal::Adaptor::CanvasRenderer&>(handle);
}

} // namespace Adaptor
} // namespace Internal

/********************************************************************************/
/*********************************  PUBLIC CLASS  *******************************/
/********************************************************************************/

CanvasRenderer CanvasRenderer::New(const Vector2& size)
{
  Internal::Adaptor::CanvasRenderer* renderer = new Internal::Adaptor::CanvasRenderer(size);
  return CanvasRenderer(renderer);
}

CanvasRenderer::CanvasRenderer()
{
}

CanvasRenderer::~CanvasRenderer()
{
}

CanvasRenderer::CanvasRenderer(Internal::Adaptor::CanvasRenderer* internal)
: BaseHandle(internal)
{
}

bool CanvasRenderer::Commit()
{
  return Internal::Adaptor::GetImplementation(*this).Commit();
}

bool CanvasRenderer::IsCanvasChanged() const
{
  return Internal::Adaptor::GetImplementation(*this).IsCanvasChanged();
}

bool CanvasRenderer::Rasterize()
{
  return Internal::Adaptor::GetImplementation(*this).Rasterize();
}

Dali::Texture CanvasRenderer::GetRasterizedTexture()
{
  return Internal::Adaptor::GetImplementation(*this).GetRasterizedTexture();
}

bool CanvasRenderer::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  return Internal::Adaptor::GetImplementation(*this).AddDrawable(drawable);
}

bool CanvasRenderer::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  return Internal::Adaptor::GetImplementation(*this).RemoveDrawable(drawable);
}

bool CanvasRenderer::RemoveAllDrawables()
{
  return Internal::Adaptor::GetImplementation(*this).RemoveAllDrawables();
}

bool CanvasRenderer::SetSize(Vector2 size)
{
  return Internal::Adaptor::GetImplementation(*this).SetSize(size);
}

Vector2 CanvasRenderer::GetSize() const
{
  return Internal::Adaptor::GetImplementation(*this).GetSize();
}

bool CanvasRenderer::SetViewBox(const Vector2& viewBox)
{
  return Internal::Adaptor::GetImplementation(*this).SetViewBox(viewBox);
}

const Vector2& CanvasRenderer::GetViewBox()
{
  return Internal::Adaptor::GetImplementation(*this).GetViewBox();
}

bool CanvasRenderer::SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
{
  return Internal::Adaptor::GetImplementation(*this).SetDropShadow(color, offsetX, offsetY, blurRadius);
}

bool CanvasRenderer::ClearDropShadow()
{
  return Internal::Adaptor::GetImplementation(*this).ClearDropShadow();
}

bool CanvasRenderer::HasDropShadow() const
{
  return Internal::Adaptor::GetImplementation(*this).HasDropShadow();
}

bool CanvasRenderer::SetGaussianBlur(float blurRadius)
{
  return Internal::Adaptor::GetImplementation(*this).SetGaussianBlur(blurRadius);
}

bool CanvasRenderer::ClearGaussianBlur()
{
  return Internal::Adaptor::GetImplementation(*this).ClearGaussianBlur();
}

bool CanvasRenderer::HasGaussianBlur() const
{
  return Internal::Adaptor::GetImplementation(*this).HasGaussianBlur();
}

void CanvasRenderer::SetEffectAutoPaddingEnabled(bool enable)
{
  Internal::Adaptor::GetImplementation(*this).SetEffectAutoPaddingEnabled(enable);
}

bool CanvasRenderer::IsEffectAutoPaddingEnabled() const
{
  return Internal::Adaptor::GetImplementation(*this).IsEffectAutoPaddingEnabled();
}

} // namespace Dali

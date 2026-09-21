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
#include <dali-ui-foundation/public-api/views/gl/gl-view-render-info.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-render-info-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace
{
/// An offscreen backend has no bound textures to report.
const Dali::Vector<uint32_t> EMPTY_TEXTURE_BINDINGS{};
} // namespace

GlViewRenderInfo::GlViewRenderInfo()
: mImpl(MakeUnique<Internal::GlViewRenderInfoImpl>())
{
}

GlViewRenderInfo::~GlViewRenderInfo() = default;

Internal::GlViewRenderInfoImpl& GlViewRenderInfo::GetImplementation()
{
  return *mImpl;
}

const Matrix& GlViewRenderInfo::GetMvpMatrix() const
{
  return mImpl->input ? mImpl->input->mvp : mImpl->mvp;
}

const Matrix& GlViewRenderInfo::GetViewMatrix() const
{
  // An offscreen backend has no render task, so there is no view transform to report.
  return mImpl->input ? mImpl->input->view : Matrix::IDENTITY;
}

const Matrix& GlViewRenderInfo::GetProjectionMatrix() const
{
  return mImpl->input ? mImpl->input->projection : mImpl->projection;
}

const Size& GlViewRenderInfo::GetSize() const
{
  return mImpl->input ? mImpl->input->size : mImpl->size;
}

const BoundsInteger& GlViewRenderInfo::GetClippingBox() const
{
  return mImpl->input ? mImpl->input->clippingBox : mImpl->clippingBox;
}

const Vector4& GlViewRenderInfo::GetWorldColorMultiplier() const
{
  // An offscreen backend's output is composited by DALi, whose shader applies the
  // inherited colour - so the application must not apply it a second time.
  return mImpl->input ? mImpl->input->worldColorMultiplier : Vector4::ONE;
}

const Vector4& GlViewRenderInfo::GetWorldColor() const
{
  return GetWorldColorMultiplier();
}

const Dali::Vector<uint32_t>& GlViewRenderInfo::GetBoundTextureNativeHandles() const
{
  return mImpl->input ? mImpl->input->textureBindings : EMPTY_TEXTURE_BINDINGS;
}

} // namespace Ui
} //namespace DALI_NAMESPACE

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

namespace Dali
{
namespace Ui
{
namespace
{
/// Returned while there is no input to read through.
const Size                   EMPTY_SIZE{};
const BoundsInteger          EMPTY_CLIPPING_BOX{};
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
  return mImpl->input ? mImpl->input->mvp : Matrix::IDENTITY;
}

const Matrix& GlViewRenderInfo::GetViewMatrix() const
{
  return mImpl->input ? mImpl->input->view : Matrix::IDENTITY;
}

const Matrix& GlViewRenderInfo::GetProjectionMatrix() const
{
  return mImpl->input ? mImpl->input->projection : Matrix::IDENTITY;
}

const Size& GlViewRenderInfo::GetSize() const
{
  return mImpl->input ? mImpl->input->size : EMPTY_SIZE;
}

const BoundsInteger& GlViewRenderInfo::GetClippingBox() const
{
  return mImpl->input ? mImpl->input->clippingBox : EMPTY_CLIPPING_BOX;
}

const Vector4& GlViewRenderInfo::GetWorldColorMultiplier() const
{
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
} // namespace Dali

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
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-impl.h>

namespace Dali
{
namespace Ui
{
namespace
{
inline Internal::GlViewImpl& GetImpl(GlView& glView)
{
  DALI_ASSERT_ALWAYS(glView);
  return static_cast<Internal::GlViewImpl&>(glView.GetImplementation());
}

inline const Internal::GlViewImpl& GetImpl(const GlView& glView)
{
  DALI_ASSERT_ALWAYS(glView);
  return static_cast<const Internal::GlViewImpl&>(glView.GetImplementation());
}
} // namespace

GlView::GlView()                                 = default;
GlView::~GlView()                                = default;
GlView::GlView(const GlView& glView)             = default;
GlView::GlView(GlView&& rhs) noexcept            = default;
GlView& GlView::operator=(const GlView& glView)  = default;
GlView& GlView::operator=(GlView&& rhs) noexcept = default;

GlView GlView::New(GlView::BackendMode backendMode)
{
  Internal::GlViewImplPtr impl = Internal::GlViewImpl::New(backendMode);
  GlView                  view(*impl);
  impl->Initialize();
  return view;
}

GlView GlView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<GlView, Ui::Internal::GlViewImpl>(handle);
}

void GlView::RegisterGlCallbacks(Callback<void(const GlViewRenderInfo&)> initCallback,
                                 Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                                 Callback<void()>                        terminateCallback)
{
  GetImpl(*this).RegisterGlCallbacks(std::move(initCallback),
                                     std::move(renderFrameCallback),
                                     std::move(terminateCallback));
}

void GlView::BindTextureResources(Dali::Vector<Dali::Texture> textures)
{
  GetImpl(*this).BindTextureResources(std::move(textures));
}

void GlView::SetRenderingMode(GlView::RenderingMode mode)
{
  GetImpl(*this).SetRenderingMode(mode);
}

GlView::RenderingMode GlView::GetRenderingMode() const
{
  return GetImpl(*this).GetRenderingMode();
}

void GlView::RenderOnce()
{
  GetImpl(*this).RenderOnce();
}

GlView::BackendMode GlView::GetBackendMode() const
{
  return GetImpl(*this).GetBackendMode();
}

void GlView::Terminate(Callback<void()> onTerminated)
{
  GetImpl(*this).Terminate(std::move(onTerminated));
}

GlView::GlView(Internal::GlViewImpl& implementation)
: View(implementation)
{
}

GlView::GlView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::GlViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali

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
#include <dali-ui-foundation/integration-api/canvas-view-impl.h>
#include <dali-ui-foundation/public-api/canvas-view.h>

namespace Dali
{
namespace Ui
{

namespace
{
inline Integration::CanvasViewImpl& GetImpl(CanvasView& canvasView)
{
  DALI_ASSERT_ALWAYS(canvasView);
  return static_cast<Integration::CanvasViewImpl&>(canvasView.GetImplementation());
}

inline const Integration::CanvasViewImpl& GetImpl(const CanvasView& canvasView)
{
  DALI_ASSERT_ALWAYS(canvasView);
  return static_cast<const Integration::CanvasViewImpl&>(canvasView.GetImplementation());
}
} // namespace

CanvasView::CanvasView()                                        = default;
CanvasView::~CanvasView()                                       = default;
CanvasView::CanvasView(const CanvasView& canvasView)            = default;
CanvasView::CanvasView(CanvasView&& rhs) noexcept               = default;
CanvasView& CanvasView::operator=(const CanvasView& canvasView) = default;
CanvasView& CanvasView::operator=(CanvasView&& rhs) noexcept    = default;

CanvasView CanvasView::New()
{
  Integration::CanvasViewImplPtr impl = Integration::CanvasViewImpl::New(Vector2::ZERO);
  CanvasView                     view(*impl);
  impl->Initialize();
  return view;
}

CanvasView CanvasView::New(const Vector2& viewBox)
{
  Integration::CanvasViewImplPtr impl = Integration::CanvasViewImpl::New(viewBox);
  CanvasView                     view(*impl);
  impl->Initialize();
  return view;
}

CanvasView CanvasView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<CanvasView, Ui::Integration::CanvasViewImpl>(handle);
}

bool CanvasView::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  return GetImpl(*this).AddDrawable(drawable);
}

bool CanvasView::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  return GetImpl(*this).RemoveDrawable(drawable);
}

bool CanvasView::RemoveAllDrawables()
{
  return GetImpl(*this).RemoveAllDrawables();
}

void CanvasView::SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
{
  GetImpl(*this).SetDropShadow(color, offsetX, offsetY, blurRadius);
}

void CanvasView::ClearDropShadow()
{
  GetImpl(*this).ClearDropShadow();
}

bool CanvasView::HasDropShadow() const
{
  return GetImpl(*this).HasDropShadow();
}

void CanvasView::SetGaussianBlur(float blurRadius)
{
  GetImpl(*this).SetGaussianBlur(blurRadius);
}

void CanvasView::ClearGaussianBlur()
{
  GetImpl(*this).ClearGaussianBlur();
}

bool CanvasView::HasGaussianBlur() const
{
  return GetImpl(*this).HasGaussianBlur();
}

void CanvasView::SetEffectAutoPaddingEnable(bool enable)
{
  GetImpl(*this).SetEffectAutoPaddingEnable(enable);
}

bool CanvasView::IsEffectAutoPaddingEnabled() const
{
  return GetImpl(*this).IsEffectAutoPaddingEnabled();
}

void CanvasView::SetViewBox(const Vector2& viewBox)
{
  GetImpl(*this).SetViewBox(viewBox);
}

Vector2 CanvasView::GetViewBox()
{
  return GetImpl(*this).GetViewBox();
}

void CanvasView::SetSynchronousLoading(bool synchronous)
{
  GetImpl(*this).SetSynchronousLoading(synchronous);
}

bool CanvasView::IsSynchronousLoading() const
{
  return GetImpl(*this).IsSynchronousLoading();
}

void CanvasView::SetRasterizationRequestManually(bool manually)
{
  GetImpl(*this).SetRasterizationRequestManually(manually);
}

bool CanvasView::IsRasterizationRequestManually() const
{
  return GetImpl(*this).IsRasterizationRequestManually();
}

void CanvasView::RequestRasterization()
{
  GetImpl(*this).RequestRasterization();
}

CanvasView::CanvasView(Integration::CanvasViewImpl& implementation)
: View(implementation)
{
}

CanvasView::CanvasView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::CanvasViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali

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
#include <dali-ui-foundation/public-api/controls/gl-view/gl-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/gl-view/gl-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/addon-binder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/graphics-backend.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dlfcn.h>

namespace Dali::UI
{
namespace
{
const char* const DALI_UI_GLES_SO("libdali2-ui-foundation-gles.so");
const char* const DALI_UI_GLES_ADDON_NAME("UIGlesAddOn");

struct UIGlesAddOn : public Dali::AddOn::AddOnBinder
{
  UIGlesAddOn()
    : Dali::AddOn::AddOnBinder(DALI_UI_GLES_ADDON_NAME, DALI_UI_GLES_SO)
  {
  }
  ~UIGlesAddOn() = default;

  ADDON_BIND_FUNCTION(GlViewNew, GlView(GlView::BackendMode, GlView::ColorFormat));
  ADDON_BIND_FUNCTION(GlViewRegisterGlCallbacks,
                      void(Internal::GlViewImpl&, CallbackBase*, CallbackBase*, CallbackBase*));
  ADDON_BIND_FUNCTION(GlViewSetResizeCallback, void(Internal::GlViewImpl&, CallbackBase*));
  ADDON_BIND_FUNCTION(GlViewSetGraphicsConfig,
                      bool(Internal::GlViewImpl&, bool, bool, int, GlView::GraphicsApiVersion));
  ADDON_BIND_FUNCTION(GlViewSetRenderingMode, void(Internal::GlViewImpl&, GlView::RenderingMode));
  ADDON_BIND_FUNCTION(GlViewGetRenderingMode, GlView::RenderingMode(const Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewGetBackendMode, GlView::BackendMode(const Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewRenderOnce, void(Internal::GlViewImpl&));
  ADDON_BIND_FUNCTION(GlViewBindTextureResources, void(Internal::GlViewImpl&, Dali::Vector<Dali::Texture>));
  ADDON_BIND_FUNCTION(GlViewTerminate, void(Internal::GlViewImpl&));
};

UniquePtr<UIGlesAddOn> gUIGlesAddon;
} // namespace

GlView::GlView() = default;

GlView::GlView(const GlView& GlView) = default;

GlView::GlView(GlView&& rhs) noexcept = default;

GlView& GlView::operator=(const GlView& GlView) = default;

GlView& GlView::operator=(GlView&& rhs) noexcept = default;

GlView::~GlView() = default;

GlView GlView::New(ColorFormat colorFormat)
{
  return New(BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING, colorFormat);
}

GlView GlView::New(BackendMode backendMode, ColorFormat colorFormat)
{
  if (Graphics::GetCurrentGraphicsBackend() == Graphics::Backend::GLES)
  {
    if (!gUIGlesAddon)
    {
      gUIGlesAddon.Reset(new UIGlesAddOn);
    }
    DALI_ASSERT_ALWAYS(gUIGlesAddon && "Cannot load the GlView Addon\n");
    return gUIGlesAddon->GlViewNew(backendMode, colorFormat);
  }
  DALI_ABORT("Current Graphics Backend does not support GlView\n");
}

GlView GlView::DownCast(BaseHandle handle)
{
  return Control::DownCast<GlView, Internal::GlViewImpl>(handle);
}

void GlView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback,
                                 CallbackBase* terminateCallback)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewRegisterGlCallbacks(impl, initCallback, renderFrameCallback, terminateCallback);
  }
}

void GlView::SetResizeCallback(CallbackBase* resizeCallback)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewSetResizeCallback(impl, resizeCallback);
  }
}

bool GlView::SetGraphicsConfig(bool depth, bool stencil, int msaa, GraphicsApiVersion version)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gUIGlesAddon ? gUIGlesAddon->GlViewSetGraphicsConfig(impl, depth, stencil, msaa, version) : false;
}

void GlView::SetRenderingMode(RenderingMode mode)
{
  Internal::GlViewImpl& impl = GetImpl(*this);
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewSetRenderingMode(impl, mode);
  }
}

Dali::UI::GlView::RenderingMode GlView::GetRenderingMode() const
{
  const Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gUIGlesAddon ? gUIGlesAddon->GlViewGetRenderingMode(impl) : RenderingMode::CONTINUOUS;
}

Dali::UI::GlView::BackendMode GlView::GetBackendMode() const
{
  const Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  return gUIGlesAddon ? gUIGlesAddon->GlViewGetBackendMode(impl) : BackendMode::DEFAULT;
}

void GlView::RenderOnce()
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewRenderOnce(impl);
  }
}

void GlView::BindTextureResources(Dali::Vector<Dali::Texture> textures)
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewBindTextureResources(impl, std::move(textures));
  }
}

void GlView::Terminate()
{
  Internal::GlViewImpl& impl = GetImpl(*this); // Get Impl here to catch uninitialized usage
  if (gUIGlesAddon)
  {
    gUIGlesAddon->GlViewTerminate(impl);
  }
}

GlView::GlView(Internal::GlViewImpl& implementation)
  : Control(implementation)
{
}

GlView::GlView(Dali::Internal::CustomActor* internal)
  : Control(internal)
{
  VerifyCustomActorPointer<Internal::GlViewImpl>(internal);
}

} // namespace Dali::UI

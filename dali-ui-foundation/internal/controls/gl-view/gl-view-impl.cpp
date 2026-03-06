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
#include <dali-ui-foundation/internal/controls/gl-view/gl-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gles-addon/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
Dali::Ui::GlView GlView::New(Dali::Ui::GlView::ColorFormat colorFormat)
{
  auto* impl = new Dali::Ui::Internal::GlView(colorFormat);
  Dali::Ui::GlView handle = Dali::Ui::GlView(*impl);
  impl->Initialize();
  return handle;
}

GlView::GlView(Dali::Ui::GlView::ColorFormat colorFormat)
  : Dali::Ui::Internal::GlViewImpl(Ui::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING),
    mColorFormat(colorFormat)
{
}

GlView::~GlView()
{
  Terminate();
}

void GlView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback,
                                 CallbackBase* terminateCallback)
{
  if (mRenderThread)
  {
    mRenderThread->RegisterGlCallbacks(initCallback, renderFrameCallback, terminateCallback);
  }
}

void GlView::SetResizeCallback(CallbackBase* resizeCallback)
{
  if (mRenderThread)
  {
    mRenderThread->SetResizeCallback(resizeCallback);
  }
}

bool GlView::SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Ui::GlView::GraphicsApiVersion version)
{
  // Init Graphics
  mDepth = depth;
  mStencil = stencil;
  mMSAA = msaa;

  int rVersion;

  if (version == Dali::Ui::GlView::GraphicsApiVersion::GLES_VERSION_2_0)
  {
    rVersion = 20;
  }
  else
  {
    rVersion = 30;
  }

  if (mRenderThread)
  {
    return mRenderThread->SetGraphicsConfig(depth, stencil, msaa, rVersion);
  }

  return false;
}

void GlView::SetRenderingMode(Dali::Ui::GlView::RenderingMode mode)
{
  mRenderingMode = mode;
  Renderer renderer = Self().GetRendererAt(0);

  if (mRenderingMode == Dali::Ui::GlView::RenderingMode::ON_DEMAND)
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);

    if (mRenderThread)
    {
      mRenderThread->SetOnDemandRenderMode(true);
    }
  }
  else
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);

    if (mRenderThread)
    {
      mRenderThread->SetOnDemandRenderMode(false);
    }
  }
}

void GlView::RenderOnce()
{
  if (mRenderThread)
  {
    mRenderThread->RenderOnce();
  }
}

void GlView::BindTextureResources(Dali::Vector<Dali::Texture> textures)
{
  // Not supported in the indirect mode
}

void GlView::Terminate()
{
  if (mRenderThread)
  {
    mRenderThread->Stop();
    mRenderThread->Join();

    mRenderThread.reset();
  }
}

void GlView::OnInitialize()
{
  // Create NativeImageQueuewith the size of 1,1
  mNativeImageQueue = Dali::NativeImageQueue::New(1, 1, GetColorFormat(mColorFormat));

  if (!mNativeImageQueue)
  {
    DALI_LOG_ERROR("NativeImageQueueis NULL");
    return;
  }

  AddRenderer();

  Actor self = Self();

  // Create a RenderThread
  mRenderThread = std::unique_ptr<GlViewRenderThread>(new GlViewRenderThread(mNativeImageQueue));
  if (!mRenderThread)
  {
    DALI_LOG_ERROR("Fail to create GlView Render Thread!!!!\n");
    return;
  }

  // Adding VisibilityChange Signal.
  self.InheritedVisibilityChangedSignal().Connect(this, &GlView::OnControlInheritedVisibilityChanged);
}

void GlView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet(targetSize);

  if (mRenderThread)
  {
    if (mNativeImageQueue)
    {
      mRenderThread->AcquireSurface();
      mNativeImageQueue->SetSize(static_cast<uint32_t>(targetSize.x), static_cast<uint32_t>(targetSize.y));
      mRenderThread->SetSurfaceSize(Vector2(targetSize.x, targetSize.y));
      mRenderThread->ReleaseSurface();
    }
  }
}

Shader GlView::CreateShader()
{
  std::string fragmentShader = std::string(SHADER_GL_VIEW_FRAG);

  if (mNativeImageQueue)
  {
    mNativeImageQueue->ApplyNativeFragmentShader(fragmentShader, 1);
  }

  return Shader::New(SHADER_GL_VIEW_VERT, fragmentShader,
                     static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                     "GL_VIEW");
}

void GlView::OnControlInheritedVisibilityChanged(Dali::Actor actor, bool visible)
{
  Actor self = Self();
  if (mRenderThread)
  {
    auto window = DevelWindow::Get(self);
    if (visible && window && window.IsVisible())
    {
      mRenderThread->Resume();
    }
    else
    {
      mRenderThread->Pause();
    }
  }
}

void GlView::OnSceneConnection(int depth)
{
  Control::OnSceneConnection(depth);
}

void GlView::OnSceneDisconnection()
{
  Control::OnSceneDisconnection();
}

Dali::Geometry GlView::CreateTexturedQuad()
{
  struct Vertex
  {
    Dali::Vector2 position;
  };

  static const Vertex data[] = {{Dali::Vector2(-0.5f, -0.5f)},
                                {Dali::Vector2(0.5f, -0.5f)},
                                {Dali::Vector2(-0.5f, 0.5f)},
                                {Dali::Vector2(0.5f, 0.5f)}};

  uint32_t numberOfVertices = sizeof(data) / sizeof(Vertex);

  Dali::VertexBuffer vertexBuffer;
  Dali::Property::Map vertexFormat;
  vertexFormat["aPosition"] = Dali::Property::VECTOR2;

  // Create a vertex buffer for vertex positions and texture coordinates
  vertexBuffer = Dali::VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(data, numberOfVertices);

  // Create the geometry
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Dali::Geometry::TRIANGLE_STRIP);

  return geometry;
}

void GlView::AddRenderer()
{
  if (!mNativeImageQueue)
  {
    DALI_LOG_ERROR("Target Surface is NULL");
    return;
  }

  Actor self = Self();
  Geometry geometry = CreateTexturedQuad();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Texture nativeTexture = Texture::New(*mNativeImageQueue);
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, nativeTexture);

  renderer.SetTextures(textureSet);

  self.AddRenderer(renderer);
}

Dali::NativeImageQueue::ColorFormat GlView::GetColorFormat(Dali::Ui::GlView::ColorFormat format)
{
  switch (format)
  {
    case Ui::GlView::ColorFormat::RGBA8888:
    {
      // TODO : Shouldn't it be RGBA8888?
      return Dali::NativeImageQueue::ColorFormat::BGRA8888;
    }

    case Ui::GlView::ColorFormat::RGB888:
    default:
    {
      // TODO : Shouldn't it be RGBX8888?
      return Dali::NativeImageQueue::ColorFormat::BGRX8888;
    }
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali

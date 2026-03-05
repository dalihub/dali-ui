/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/internal/text/rendering/text-backend-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/text/rendering-backend.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.h>
#ifdef ENABLE_VECTOR_BASED_TEXT_RENDERING
#include <dali-ui-foundation/internal/text/rendering/vector-based/vector-based-renderer.h>
#endif

namespace Dali
{
namespace UI
{
namespace Text
{
namespace Internal
{
struct Backend::Impl
{
  int temp; // placeholder for future backend implemenations
};

Backend::Backend()
  : mImpl(NULL)
{
  mImpl = new Impl();
}

Backend::~Backend()
{
  delete mImpl;
}

Dali::UI::Text::Backend Backend::Get()
{
  Dali::UI::Text::Backend backendHandle;

  Dali::SingletonService service(SingletonService::Get());
  if (service)
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton(typeid(Dali::UI::Text::Backend));
    if (handle)
    {
      // If so, downcast the handle
      Backend* impl = dynamic_cast<Dali::UI::Text::Internal::Backend*>(handle.GetObjectPtr());
      backendHandle = Dali::UI::Text::Backend(impl);
    }
    else // create and register the object
    {
      backendHandle = Dali::UI::Text::Backend(new Backend);
      service.Register(typeid(backendHandle), backendHandle);
    }
  }

  return backendHandle;
}

RendererPtr Backend::NewRenderer(unsigned int renderingType)
{
  RendererPtr renderer;

  switch (renderingType)
  {
    case Dali::UI::DevelText::RENDERING_SHARED_ATLAS:
    {
      renderer = Dali::UI::Text::AtlasRenderer::New();
    }
    break;

    case Dali::UI::DevelText::RENDERING_VECTOR_BASED:
    {
#ifdef ENABLE_VECTOR_BASED_TEXT_RENDERING
      renderer = Dali::UI::Text::VectorBasedRenderer::New();
#else
      renderer = Dali::UI::Text::AtlasRenderer::New(); // Fallback to bitmap-based rendering
#endif
    }
    break;

    default:
    {
      DALI_LOG_ERROR("Unknown renderer type: %d\n", renderingType);
      break;
    }
  }

  return renderer;
}

} // namespace Internal

} // namespace Text

} // namespace UI

} // namespace Dali

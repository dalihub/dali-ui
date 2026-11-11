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
#include <dali-ui-foundation/extension-api/gl-view-direct-rendering.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-impl.h>
#include <dali-ui-foundation/internal/gl-view/gl-view-render-info-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
namespace GlViewDirectRendering
{
namespace
{
/// A view on the public backend has no bound textures to report.
const Dali::Vector<uint32_t> EMPTY_TEXTURE_BINDINGS{};
} // namespace

void BindTextureResources(GlView glView, Dali::Vector<Dali::Texture> textures)
{
  DALI_ASSERT_ALWAYS(glView);
  static_cast<Internal::GlViewImpl&>(glView.GetImplementation()).BindTextureResources(std::move(textures));
}

const Dali::Vector<uint32_t>& GetBoundTextureNativeHandles(const GlViewRenderInfo& renderInfo)
{
  const Dali::RenderCallbackInput* input = renderInfo.GetImplementation().input;
  return input ? input->textureBindings : EMPTY_TEXTURE_BINDINGS;
}

} // namespace GlViewDirectRendering
} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE

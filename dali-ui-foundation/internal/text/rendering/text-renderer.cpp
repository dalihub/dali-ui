/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/internal/text/rendering/text-renderer.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

bool Renderer::SetAtlasGradientState(const Internal::Gradient::AtlasRendererState&)
{
  return false;
}

void Renderer::UpdateAtlasGradient(const Vector2& coordinateSize, const Vector4& bounds)
{
}

void Renderer::SetAtlasGradientAnimProperties(Actor sourceActor, Property::Index startOffsetPropertyIndex)
{
}

void Renderer::SetAtlasGradientAnimApplyAlways(bool applyAlways, bool notifyToConstraint)
{
}

} // namespace Text

} // namespace Ui

} // namespace Dali

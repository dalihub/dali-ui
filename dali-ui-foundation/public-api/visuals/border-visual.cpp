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
#include <dali-ui-foundation/public-api/visuals/border-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
BorderVisual BorderVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::BORDER);
  return BorderVisual(internal.Get());
}

BorderVisual BorderVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::BORDER)
  {
    return BorderVisual(visualBaseImpl);
  }
  return BorderVisual();
}

// =============================================================================
// Properties
// =============================================================================

float BorderVisual::GetBorderSize() const
{
  return VisualBase::GetProperty<float>(BorderVisual::Property::BORDER_SIZE);
}

void BorderVisual::SetBorderSize(float borderSize)
{
  VisualBase::SetProperty(BorderVisual::Property::BORDER_SIZE, borderSize);
}

bool BorderVisual::IsAntiAliasingEnabled() const
{
  return VisualBase::GetProperty<bool>(BorderVisual::Property::ANTI_ALIASING);
}

void BorderVisual::SetAntiAliasingEnabled(bool enable)
{
  VisualBase::SetProperty(BorderVisual::Property::ANTI_ALIASING, enable);
}

// =============================================================================
// Internal API
// =============================================================================

BorderVisual::BorderVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali

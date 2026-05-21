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
#include <dali-ui-foundation/public-api/visuals/color-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
ColorVisual ColorVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::COLOR);
  return ColorVisual(internal.Get());
}

ColorVisual ColorVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::COLOR)
  {
    return ColorVisual(visualBaseImpl);
  }
  return ColorVisual();
}

// =============================================================================
// Properties
// =============================================================================

float ColorVisual::GetBlurRadius() const
{
  return VisualBase::GetProperty<float>(ColorVisual::Property::BLUR_RADIUS);
}

ColorVisual& ColorVisual::SetBlurRadius(float blurRadius)
{
  VisualBase::SetProperty(ColorVisual::Property::BLUR_RADIUS, blurRadius);
  return *this;
}

CutoutPolicy ColorVisual::GetCutoutPolicy() const
{
  return VisualBase::GetProperty<CutoutPolicy>(ColorVisual::Property::CUTOUT_POLICY);
}

ColorVisual& ColorVisual::SetCutoutPolicy(CutoutPolicy cutoutPolicy)
{
  VisualBase::SetProperty(ColorVisual::Property::CUTOUT_POLICY, cutoutPolicy);
  return *this;
}

// =============================================================================
// Internal API
// =============================================================================

ColorVisual::ColorVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali

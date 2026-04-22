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
#include <dali-ui-foundation/devel-api/visuals/visuals-container.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/visuals/visuals-container-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{
VisualsContainer::VisualsContainer(Dali::Ui::Internal::VisualsContainer* object)
: BaseHandle(object)
{
}

VisualsContainer VisualsContainer::New(Dali::Ui::View view, Dali::Ui::DevelVisual::InternalContainerRangeType rangeType)
{
  Internal::VisualsContainerPtr internal = Internal::VisualsContainer::New(view, rangeType);
  return VisualsContainer(internal.Get());
}

VisualsContainer VisualsContainer::DownCast(BaseHandle handle)
{
  return VisualsContainer(dynamic_cast<Dali::Ui::Internal::VisualsContainer*>(handle.GetObjectPtr()));
}

///< Public API

Dali::Ui::View VisualsContainer::GetOwner() const
{
  return GetImplementation(*this).GetOwner();
}

Dali::Ui::DevelVisual::InternalContainerRangeType VisualsContainer::GetContainerRangeType() const
{
  return GetImplementation(*this).GetContainerRangeType();
}

uint32_t VisualsContainer::GetVisualBasesCount() const
{
  return GetImplementation(*this).GetVisualBasesCount();
}

Dali::Ui::VisualBase VisualsContainer::GetVisualBaseAt(uint32_t index) const
{
  return GetImplementation(*this).GetVisualBaseAt(index);
}

bool VisualsContainer::AddVisualBase(Dali::Ui::VisualBase visualObject)
{
  return GetImplementation(*this).AddVisualBase(visualObject);
}

bool VisualsContainer::AddShadowVisualBase(Dali::Ui::VisualBase visualObject, ShadowType shadowType)
{
  return GetImplementation(*this).AddVisualBase(visualObject, shadowType);
}

void VisualsContainer::RemoveVisualBase(Dali::Ui::VisualBase visualObject)
{
  GetImplementation(*this).RemoveVisualBase(visualObject);
}
} // namespace Ui
} // namespace Dali
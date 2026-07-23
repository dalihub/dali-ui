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
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/selectable-image-interface-impl.h>

namespace Dali
{
namespace Ui
{

SelectableImageInterface SelectableImageInterface::DownCast(BaseHandle handle)
{
  return SelectableImageInterface(dynamic_cast<Integration::SelectableImageInterfaceImpl*>(handle.GetObjectPtr()));
}

Ui::View SelectableImageInterface::GetView() const
{
  return GetImpl(*this).GetView();
}

void SelectableImageInterface::SetSelected(bool selected)
{
  GetImpl(*this).SetSelected(selected);
}

void SelectableImageInterface::SetSelected(bool selected, bool animated)
{
  GetImpl(*this).SetSelected(selected, animated);
}

void SelectableImageInterface::SetStateColors(const Vector4& deselected, const Vector4& selected)
{
  GetImpl(*this).SetStateColors(deselected, selected);
}

bool SelectableImageInterface::IsTransitioning() const
{
  return GetImpl(*this).IsTransitioning();
}

SelectableImageInterface::TransitionFinishedSignalType& SelectableImageInterface::TransitionFinishedSignal()
{
  return GetImpl(*this).TransitionFinishedSignal();
}

SelectableImageInterface::SelectableImageInterface(Integration::SelectableImageInterfaceImpl* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali

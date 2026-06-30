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
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/ui-scale-manager-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{

UiScaleManager::UiScaleManager() = default;

UiScaleManager::UiScaleManager(Internal::UiScaleManagerImpl* impl)
: BaseHandle(impl)
{
}

UiScaleManager UiScaleManager::Get()
{
  return Internal::UiScaleManagerImpl::Get();
}

UiScaleManager UiScaleManager::DownCast(BaseHandle handle)
{
  return UiScaleManager(dynamic_cast<Internal::UiScaleManagerImpl*>(handle.GetObjectPtr()));
}

float UiScaleManager::GetScale() const
{
  return GetImpl(*this).GetScale();
}

void UiScaleManager::SetScale(float scale)
{
  GetImpl(*this).SetScale(scale);
}

void UiScaleManager::RegisterLayoutRoot(View root)
{
  GetImpl(*this).RegisterLayoutRoot(root);
}

void UiScaleManager::UnregisterLayoutRoot(View root)
{
  GetImpl(*this).UnregisterLayoutRoot(root);
}

} // namespace Ui
} // namespace Dali

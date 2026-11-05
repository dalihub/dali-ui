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
#include <dali-ui-foundation/public-api/configuration/layout-scale-manager.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layout-scale-manager-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

LayoutScaleManager::LayoutScaleManager() = default;

LayoutScaleManager::LayoutScaleManager(Internal::LayoutScaleManagerImpl* impl)
: BaseHandle(impl)
{
}

LayoutScaleManager LayoutScaleManager::Get()
{
  return Internal::LayoutScaleManagerImpl::Get();
}

LayoutScaleManager LayoutScaleManager::DownCast(BaseHandle handle)
{
  return LayoutScaleManager(dynamic_cast<Internal::LayoutScaleManagerImpl*>(handle.GetObjectPtr()));
}

float LayoutScaleManager::GetUiScale() const
{
  return GetImpl(*this).GetUiScale();
}

void LayoutScaleManager::SetUiScale(float scale)
{
  GetImpl(*this).SetUiScale(scale);
}

bool LayoutScaleManager::IsScalable() const
{
  return GetImpl(*this).IsScalable();
}

void LayoutScaleManager::SetScalable(bool enable)
{
  GetImpl(*this).SetScalable(enable);
}

float LayoutScaleManager::GetWindowScale(Window window) const
{
  return GetImpl(*this).GetWindowScale(window);
}

void LayoutScaleManager::SetWindowScale(Window window, float scale)
{
  GetImpl(*this).SetWindowScale(window, scale);
}

} // namespace Ui
} //namespace DALI_NAMESPACE

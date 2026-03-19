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
#include <dali-ui-foundation/public-api/ui-color-manager.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>

namespace Dali
{
namespace Ui
{

UiColorManager::UiColorManager() = default;

UiColorManager::UiColorManager(Internal::UiColorManagerImpl* impl)
: BaseHandle(impl)
{
}

UiColorManager UiColorManager::Get()
{
  return Internal::UiColorManagerImpl::Get();
}

UiColorManager UiColorManager::DownCast(BaseHandle handle)
{
  return UiColorManager(dynamic_cast<Internal::UiColorManagerImpl*>(handle.GetObjectPtr()));
}

Vector4 UiColorManager::GetColor(StringView colorId) const
{
  return GetImpl(*this).GetColor(colorId);
}

bool UiColorManager::GetColor(StringView colorId, Vector4& outColor) const
{
  return GetImpl(*this).GetColor(colorId, outColor);
}

void UiColorManager::UpdateBinding(const UiColor& color, View view, CallbackBase* applyFunc)
{
  GetImpl(*this).UpdateBinding(color, view, applyFunc);
}

bool UiColorManager::GetBindingColor(View view, CallbackBase* applyFunc, UiColor& outColor) const
{
  return GetImpl(*this).GetBindingColor(view, applyFunc, outColor);
}

void UiColorManager::RemoveBinding(View view, CallbackBase* applyFunc)
{
  GetImpl(*this).RemoveBinding(view, applyFunc);
}

void UiColorManager::RemoveBindings(View view)
{
  GetImpl(*this).RemoveBindings(view);
}

void UiColorManager::SetColorOverride(ColorOverrideFunc func)
{
  GetImpl(*this).SetColorOverride(func);
}

void UiColorManager::ClearColorOverride()
{
  GetImpl(*this).ClearColorOverride();
}

} // namespace Ui
} // namespace Dali

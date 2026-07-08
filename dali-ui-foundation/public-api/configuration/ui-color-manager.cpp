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
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>

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

void UiColorManager::RegisterBinding(BaseHandle view, StringView bindingId, ColorCallback callback)
{
  GetImpl(*this).RegisterBinding(view, bindingId, std::move(callback));
}

bool UiColorManager::GetBindingColor(BaseHandle view, StringView bindingId, UiColor& outColor) const
{
  return GetImpl(*this).GetBindingColor(view, bindingId, outColor);
}

bool UiColorManager::HasBinding(BaseHandle view, StringView bindingId) const
{
  return GetImpl(*this).HasBinding(view, bindingId);
}

void UiColorManager::SetBindingColor(BaseHandle view, StringView bindingId, const UiColor& color)
{
  GetImpl(*this).SetBindingColor(view, bindingId, color);
}

void UiColorManager::ClearBinding(BaseHandle view, StringView bindingId)
{
  GetImpl(*this).ClearBinding(view, bindingId);
}

void UiColorManager::ClearBindings(BaseHandle view)
{
  GetImpl(*this).ClearBindings(view);
}

void UiColorManager::SetColorOverride(ColorOverrideFunc func)
{
  GetImpl(*this).SetColorOverride(func);
}

void UiColorManager::ClearColorOverride()
{
  GetImpl(*this).ClearColorOverride();
}

void UiColorManager::InvalidateCache()
{
  GetImpl(*this).InvalidateCache();
}

void UiColorManager::InvalidateCache(const UiColor& color)
{
  GetImpl(*this).InvalidateCache(color);
}

UiColorManager::ColorTableChangedSignalType& UiColorManager::ColorTableChangedSignal()
{
  return GetImpl(*this).ColorTableChangedSignal();
}

} // namespace Ui
} // namespace Dali

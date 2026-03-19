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
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>
#include <dali-ui-foundation/public-api/ui-theme-manager.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{

struct ApplyingGuard
{
  bool& mFlag;
  explicit ApplyingGuard(bool& flag)
  : mFlag(flag)
  {
    mFlag = true;
  }
  ~ApplyingGuard()
  {
    mFlag = false;
  }
};

} // namespace

UiColorManagerImpl::UiColorManagerImpl() = default;

UiColorManagerImpl::~UiColorManagerImpl() = default;

UiColorManager UiColorManagerImpl::Get()
{
  static IntrusivePtr<UiColorManagerImpl> impl;

  if(!impl)
  {
    impl = new UiColorManagerImpl();
  }

  return UiColorManager(impl.Get());
}

Vector4 UiColorManagerImpl::GetColor(StringView colorId) const
{
  Vector4 color;
  return GetColor(colorId, color) ? color : Vector4::ZERO;
}

bool UiColorManagerImpl::GetColor(StringView colorId, Vector4& outColor) const
{
  if(mColorOverride)
  {
    if(mColorOverride(colorId, outColor))
    {
      return true;
    }
  }

  UiThemeManager themeManager = UiThemeManager::Get();
  return GetImpl(themeManager).GetLoader().GetColor(colorId, outColor);
}

void UiColorManagerImpl::UpdateBinding(const UiColor& color, View view, CallbackBase* applyFunc)
{
  std::unique_ptr<CallbackBase> callback(applyFunc);

  if(mIsApplying)
  {
    return;
  }

  if(!mConnected)
  {
    UiThemeManager::Get().ThemeChangedSignal().Connect(mSlotDelegate, &UiColorManagerImpl::OnThemeChanged);
    mConnected = true;
  }

  void* viewPtr = static_cast<void*>(view.GetObjectPtr());

  if(color.HasColorId())
  {
    // Register the binding even if the color was not found in the current theme.
    // When the theme changes later, RefreshBindings() will resolve the color again.
    auto& viewBinding = mBindings[viewPtr];
    if(!viewBinding.weakView.GetBaseHandle())
    {
      viewBinding.weakView = WeakHandle<View>(view);
    }

    auto& bindings = viewBinding.bindings;
    for(auto& info : bindings)
    {
      if(*info.applyFunc == *callback)
      {
        info.color = color;
        return;
      }
    }
    bindings.push_back({std::move(callback), color});
  }
  else
  {
    EraseBinding(view, *callback);
  }
}

bool UiColorManagerImpl::GetBindingColor(View view, CallbackBase* applyFunc, UiColor& outColor) const
{
  std::unique_ptr<CallbackBase> callback(applyFunc);

  const BindingInfo* info = FindBinding(view, *callback);
  if(info)
  {
    outColor = info->color;
    return true;
  }
  return false;
}

void UiColorManagerImpl::RemoveBinding(View view, CallbackBase* applyFunc)
{
  std::unique_ptr<CallbackBase> callback(applyFunc);
  EraseBinding(view, *callback);
}

void UiColorManagerImpl::RemoveBindings(View view)
{
  void* viewPtr = static_cast<void*>(view.GetObjectPtr());
  mBindings.erase(viewPtr);
}

const UiColorManagerImpl::BindingInfo* UiColorManagerImpl::FindBinding(View view, const CallbackBase& callback) const
{
  void* viewPtr = static_cast<void*>(view.GetObjectPtr());

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return nullptr;
  }

  for(const auto& info : it->second.bindings)
  {
    if(*info.applyFunc == callback)
    {
      return &info;
    }
  }
  return nullptr;
}

void UiColorManagerImpl::EraseBinding(View view, const CallbackBase& callback)
{
  void* viewPtr = static_cast<void*>(view.GetObjectPtr());

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return;
  }

  auto& bindings = it->second.bindings;
  bindings.erase(std::remove_if(bindings.begin(), bindings.end(),
                                [&callback](const BindingInfo& info)
  { return *info.applyFunc == callback; }),
                 bindings.end());

  if(bindings.empty())
  {
    mBindings.erase(it);
  }
}

void UiColorManagerImpl::SetColorOverride(ColorOverrideFunc func)
{
  mColorOverride = func;
  RefreshBindings();
}

void UiColorManagerImpl::ClearColorOverride()
{
  mColorOverride = nullptr;
  RefreshBindings();
}

void UiColorManagerImpl::OnThemeChanged()
{
  RefreshBindings();
}

void UiColorManagerImpl::RefreshBindings()
{
  if(mIsApplying)
  {
    return;
  }

  ApplyingGuard guard(mIsApplying);

  UiThemeManager themeManager = UiThemeManager::Get();

  auto it = mBindings.begin();
  while(it != mBindings.end())
  {
    auto&      viewBinding = it->second;
    BaseHandle handle      = viewBinding.weakView.GetBaseHandle();
    if(!handle)
    {
      it = mBindings.erase(it);
      continue;
    }

    View view = View::DownCast(handle);
    for(auto& info : viewBinding.bindings)
    {
      CallbackBase::Execute<const Vector4&>(*info.applyFunc, info.color.Resolve());
    }

    ++it;
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

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

void UiColorManagerImpl::RegisterBinding(BaseHandle view, StringView bindingId, ColorCallback callback)
{
  if(mIsApplying)
  {
    return;
  }

  if(!mConnected)
  {
    UiThemeManager::Get().ThemeChangedSignal().Connect(mSlotDelegate, &UiColorManagerImpl::OnThemeChanged);
    mConnected = true;
  }

  RefObject* viewPtr = view.GetObjectPtr();

  auto& viewBinding = mBindings[viewPtr];
  if(!viewBinding.weakView.GetBaseHandle())
  {
    viewBinding.weakView = WeakHandle<BaseHandle>(view);
  }

  auto& bindings = viewBinding.bindings;
  for(auto& [key, info] : bindings)
  {
    if(key.size() == bindingId.Size() && key.compare(0, key.size(), bindingId.Data(), bindingId.Size()) == 0)
    {
      info.applyFunc = std::move(callback);
      return;
    }
  }
  bindings.emplace_back(std::string(bindingId.Data(), bindingId.Size()), BindingInfo{std::move(callback), UiColor{}});
}

bool UiColorManagerImpl::GetBindingColor(BaseHandle view, StringView bindingId, UiColor& outColor) const
{
  RefObject* viewPtr = view.GetObjectPtr();

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return false;
  }

  for(const auto& [key, info] : it->second.bindings)
  {
    if(key.size() == bindingId.Size() && key.compare(0, key.size(), bindingId.Data(), bindingId.Size()) == 0)
    {
      outColor = info.color;
      return true;
    }
  }
  return false;
}

bool UiColorManagerImpl::HasBinding(BaseHandle view, StringView bindingId) const
{
  RefObject* viewPtr = view.GetObjectPtr();

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return false;
  }

  for(const auto& [key, info] : it->second.bindings)
  {
    if(key.size() == bindingId.Size() && key.compare(0, key.size(), bindingId.Data(), bindingId.Size()) == 0)
    {
      return true;
    }
  }
  return false;
}

void UiColorManagerImpl::SetBindingColor(BaseHandle view, StringView bindingId, const UiColor& color)
{
  if(mIsApplying)
  {
    return;
  }

  RefObject* viewPtr = view.GetObjectPtr();

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return;
  }

  for(auto& [key, info] : it->second.bindings)
  {
    if(key.size() == bindingId.Size() && key.compare(0, key.size(), bindingId.Data(), bindingId.Size()) == 0)
    {
      info.color = color;
      return;
    }
  }
}

void UiColorManagerImpl::ClearBinding(BaseHandle view, StringView bindingId)
{
  RefObject* viewPtr = view.GetObjectPtr();

  auto it = mBindings.find(viewPtr);
  if(it == mBindings.end())
  {
    return;
  }

  auto& bindings = it->second.bindings;
  bindings.erase(std::remove_if(bindings.begin(), bindings.end(),
                                [&bindingId](const auto& entry)
  {
    const auto& key = entry.first;
    return key.size() == bindingId.Size() &&
           key.compare(0, key.size(), bindingId.Data(), bindingId.Size()) == 0;
  }),
                 bindings.end());

  if(bindings.empty())
  {
    mBindings.erase(it);
  }
}

void UiColorManagerImpl::ClearBindings(BaseHandle view)
{
  RefObject* viewPtr = view.GetObjectPtr();
  mBindings.erase(viewPtr);
}

void UiColorManagerImpl::ClearBindings(void* objectPtr)
{
  mBindings.erase(static_cast<RefObject*>(objectPtr));
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

    for(auto& [id, info] : viewBinding.bindings)
    {
      if(info.color.HasColorId())
      {
        info.applyFunc.Invoke(info.color.Resolve());
      }
    }

    ++it;
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

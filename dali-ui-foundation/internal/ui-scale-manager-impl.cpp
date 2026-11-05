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
#include <dali-ui-foundation/internal/ui-scale-manager-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

UiScaleManagerImpl::UiScaleManagerImpl()
{
  // The one counted reference this object holds. Taking it here, rather than calling
  // LayoutScaleManagerImpl::Get() on every forward, keeps the shared state alive for as
  // long as this object lives, in whatever order the two function-local statics are
  // destroyed at exit.
  LayoutScaleManager layoutScaleManager = LayoutScaleManagerImpl::Get();
  mLayoutScaleManager                   = &GetImpl(layoutScaleManager);
}

UiScaleManagerImpl::~UiScaleManagerImpl() = default;

UiScaleManager UiScaleManagerImpl::Get()
{
  static IntrusivePtr<UiScaleManagerImpl> impl;

  if(!impl)
  {
    impl = new UiScaleManagerImpl();
  }

  return UiScaleManager(impl.Get());
}

float UiScaleManagerImpl::GetScale() const
{
  return mLayoutScaleManager->GetUiScale();
}

void UiScaleManagerImpl::SetScale(float scale)
{
  mLayoutScaleManager->SetUiScale(scale);
}

bool UiScaleManagerImpl::IsScalable() const
{
  return mLayoutScaleManager->IsScalable();
}

void UiScaleManagerImpl::SetScalable(bool enable)
{
  mLayoutScaleManager->SetScalable(enable);
}

void UiScaleManagerImpl::RegisterLayoutRoot(Ui::View root)
{
  mLayoutScaleManager->RegisterLayoutRoot(root);
}

void UiScaleManagerImpl::UnregisterLayoutRoot(Ui::View root)
{
  mLayoutScaleManager->UnregisterLayoutRoot(root);
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

UiScaleManagerImpl::UiScaleManagerImpl() = default;

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
  return mScale;
}

void UiScaleManagerImpl::SetScale(float scale)
{
  if(std::isnan(scale) || scale <= 0.0f)
  {
    DALI_LOG_ERROR("UiScaleManagerImpl::SetScale: invalid scale value (%f). Scale must be a positive finite number.\n", scale);
    return;
  }

  if(mScale == scale)
  {
    return;
  }

  mScale = scale;

  // While scaling is disabled the stored scale has no effect on any view's
  // effective scale (ComputeEffectiveScale collapses to 1.0f), so applying it
  // now would relayout the whole tree for no visible change. Store it and defer
  // the relayout to SetScalable(true), which re-applies the stored scale.
  if(mIsScalable)
  {
    InvalidateAllLayoutRoots();
  }
}

bool UiScaleManagerImpl::IsScalable() const
{
  return mIsScalable;
}

void UiScaleManagerImpl::SetScalable(bool enable)
{
  if(mIsScalable == enable)
  {
    return;
  }

  mIsScalable = enable;

  // Both directions move every scaled view's effective scale (enable: 1.0 ->
  // mScale, disable: mScale -> 1.0), so a full subtree reset plus a re-layout is
  // required either way.
  InvalidateAllLayoutRoots();
}

void UiScaleManagerImpl::InvalidateAllLayoutRoots()
{
  // Invalidate and re-trigger layout for all registered roots
  auto it = mLayoutRoots.begin();
  while(it != mLayoutRoots.end())
  {
    BaseHandle handle = it->GetBaseHandle();
    if(!handle)
    {
      it = mLayoutRoots.erase(it);
      continue;
    }

    Ui::View rootView = Ui::View::DownCast(handle);
    if(rootView)
    {
      ViewImpl& viewImpl = GetImpl(rootView);
      // Drop the cached scale (and the layout caches derived from it) for the
      // entire subtree first, so every view re-evaluates its effective scale on
      // the next Measure pass.
      ViewDataImpl::Get(viewImpl).ResetSubtreeScaleAndLayoutCaches();
      // Then invalidate measure at the root to schedule a re-layout pass.
      viewImpl.InvalidateMeasure();
    }

    ++it;
  }
}

void UiScaleManagerImpl::RegisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  // Check if already registered (avoid duplicates)
  for(auto& weakRoot : mLayoutRoots)
  {
    BaseHandle handle = weakRoot.GetBaseHandle();
    if(handle && handle == root)
    {
      return;
    }
  }

  mLayoutRoots.emplace_back(WeakHandle<BaseHandle>(root));
}

void UiScaleManagerImpl::UnregisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  mLayoutRoots.erase(
    std::remove_if(mLayoutRoots.begin(), mLayoutRoots.end(),
                   [&root](const WeakHandle<BaseHandle>& weakRoot)
  {
    BaseHandle handle = weakRoot.GetBaseHandle();
    return !handle || handle == root;
  }),
    mLayoutRoots.end());
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

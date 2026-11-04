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
#include <dali-ui-foundation/internal/layout-scale-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

LayoutScaleManagerImpl::LayoutScaleManagerImpl() = default;

LayoutScaleManagerImpl::~LayoutScaleManagerImpl() = default;

LayoutScaleManager LayoutScaleManagerImpl::Get()
{
  static IntrusivePtr<LayoutScaleManagerImpl> impl;

  if(!impl)
  {
    impl = new LayoutScaleManagerImpl();
  }

  return LayoutScaleManager(impl.Get());
}

float LayoutScaleManagerImpl::GetUiScale() const
{
  return mUiScale;
}

void LayoutScaleManagerImpl::SetUiScale(float scale)
{
  // !isfinite rather than isnan: the documented contract is a positive FINITE
  // scale, and an infinite scale multiplies straight into every view's effective
  // scale, so every measured size and arranged rect in the tree becomes infinite.
  // isnan alone let +/-Inf through (Inf > 0 and is not NaN).
  //
  // The lower bound is FLT_MIN, not 0: the layout divides by the effective scale in a
  // dozen places, and a subnormal scale makes 1 / scale overflow to infinity long
  // before the scale itself is zero. A subnormal is not a usable display scale, so it
  // is rejected with every other invalid value rather than clamped.
  if(!std::isfinite(scale) || scale < std::numeric_limits<float>::min())
  {
    DALI_LOG_ERROR("LayoutScaleManagerImpl::SetUiScale: invalid scale value (%f). Scale must be a positive, finite, normal number (>= FLT_MIN).\n", scale);
    return;
  }

  if(mUiScale == scale)
  {
    return;
  }

  mUiScale = scale;

  // While scaling is disabled the stored scale has no effect on any view's
  // effective scale (ComputeEffectiveScale collapses to 1.0f), so applying it
  // now would relayout the whole tree for no visible change. Store it and defer
  // the relayout to SetScalable(true), which re-applies the stored scale.
  if(mIsScalable)
  {
    InvalidateAllLayoutRoots();
  }
}

bool LayoutScaleManagerImpl::IsScalable() const
{
  return mIsScalable;
}

void LayoutScaleManagerImpl::SetScalable(bool enable)
{
  if(mIsScalable == enable)
  {
    return;
  }

  mIsScalable = enable;

  // Both directions move every scaled view's effective scale (enable: 1.0 ->
  // mUiScale, disable: mUiScale -> 1.0), so a full subtree reset plus a re-layout is
  // required either way.
  InvalidateAllLayoutRoots();
}

void LayoutScaleManagerImpl::InvalidateAllLayoutRoots()
{
  // Invalidate and re-trigger layout for all registered roots
  auto it = mLayoutRoots.begin();
  while(it != mLayoutRoots.end())
  {
    Ui::View rootView = it->second.GetHandle();
    if(!rootView)
    {
      it = mLayoutRoots.erase(it);
      continue;
    }

    ViewImpl& viewImpl = GetImpl(rootView);
    // Drop the cached scale (and the layout caches derived from it) for the
    // entire subtree first, so every view re-evaluates its effective scale on
    // the next Measure pass.
    ViewDataImpl::Get(viewImpl).ResetSubtreeScaleAndLayoutCaches();
    // Then invalidate measure at the root to schedule a re-layout pass.
    // Through the internal primitive, not ViewImpl::InvalidateMeasure(): a
    // global scale change is a framework-internal event, not an application call.
    ViewDataImpl::Get(viewImpl).InvalidateMeasure();

    ++it;
  }
}

void LayoutScaleManagerImpl::RegisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  const auto key      = root.GetObjectPtr();
  auto [it, inserted] = mLayoutRoots.try_emplace(key, root);
  if(!inserted && !it->second.GetHandle())
  {
    // Replace a stale weak entry when a newly allocated view reuses an old
    // root's address. A live root is already registered, so leave its
    // WeakHandle intact and avoid allocation on duplicate registration.
    it->second = WeakHandle<Ui::View>(root);
  }
}

void LayoutScaleManagerImpl::UnregisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  mLayoutRoots.erase(root.GetObjectPtr());
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

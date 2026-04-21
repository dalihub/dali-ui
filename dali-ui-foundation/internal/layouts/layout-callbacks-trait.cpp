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
#include <dali-ui-foundation/internal/layouts/layout-callbacks-trait.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

// ============================================================================
// LayoutCallbacksTrait — handle
// ============================================================================

LayoutCallbacksTrait LayoutCallbacksTrait::New(LayoutCallbacksTraitImpl* impl)
{
  return LayoutCallbacksTrait(impl);
}

LayoutCallbacksTrait::LayoutCallbacksTrait(LayoutCallbacksTraitImpl* impl)
: Trait(impl)
{
}

// ============================================================================
// LayoutCallbacksTraitImpl — implementation
// ============================================================================

LayoutCallbacksTraitImpl::LayoutCallbacksTraitImpl()
: TraitImpl()
{
}

LayoutCallbacksTraitImpl::~LayoutCallbacksTraitImpl()
{
}

void LayoutCallbacksTraitImpl::SetMeasureCallback(MeasureCallback callback)
{
  mOnMeasure = std::move(callback);
}

void LayoutCallbacksTraitImpl::SetArrangeCallback(ArrangeCallback callback)
{
  mOnArrange = std::move(callback);
}

MeasureCallback* LayoutCallbacksTraitImpl::GetMeasureCallback()
{
  return mOnMeasure ? &mOnMeasure : nullptr;
}

ArrangeCallback* LayoutCallbacksTraitImpl::GetArrangeCallback()
{
  return mOnArrange ? &mOnArrange : nullptr;
}

void LayoutCallbacksTraitImpl::OnBeforeAttached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksTraitImpl::OnAttached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksTraitImpl::OnDetached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksTraitImpl::OnViewDestroying(Integration::ViewImpl* viewImpl)
{
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

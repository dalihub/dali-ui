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
#include <dali-ui-foundation/internal/layouts/layout-callbacks-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

// ============================================================================
// LayoutCallbacksTrait — handle
// ============================================================================

LayoutCallbacksTrait LayoutCallbacksTrait::New(LayoutCallbacksImpl* impl)
{
  return LayoutCallbacksTrait(impl);
}

LayoutCallbacksTrait::LayoutCallbacksTrait(LayoutCallbacksImpl* impl)
: Trait(impl)
{
}

// ============================================================================
// LayoutCallbacksImpl — implementation
// ============================================================================

LayoutCallbacksImpl::LayoutCallbacksImpl()
: TraitImpl()
{
}

LayoutCallbacksImpl::~LayoutCallbacksImpl()
{
}

void LayoutCallbacksImpl::SetMeasureCallback(CallbackBase* callback)
{
  mOnMeasure.Reset(callback);
}

void LayoutCallbacksImpl::SetArrangeCallback(CallbackBase* callback)
{
  mOnArrange.Reset(callback);
}

bool LayoutCallbacksImpl::HasMeasureCallback() const
{
  return static_cast<bool>(mOnMeasure);
}

bool LayoutCallbacksImpl::HasArrangeCallback() const
{
  return static_cast<bool>(mOnArrange);
}

MeasuredSize LayoutCallbacksImpl::InvokeMeasure(View view, float widthConstraint, float heightConstraint)
{
  return CallbackBase::ExecuteReturn<MeasuredSize>(*mOnMeasure, view, widthConstraint, heightConstraint);
}

MeasuredSize LayoutCallbacksImpl::InvokeArrange(View view, const LayoutRect& bounds)
{
  return CallbackBase::ExecuteReturn<MeasuredSize>(*mOnArrange, view, bounds);
}

void LayoutCallbacksImpl::OnBeforeAttached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksImpl::OnAttached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksImpl::OnDetached(Integration::TraitId id, View& view)
{
}

void LayoutCallbacksImpl::OnViewDestroying(Integration::ViewImpl* viewImpl)
{
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

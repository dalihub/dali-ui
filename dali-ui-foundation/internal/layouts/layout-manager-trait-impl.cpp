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
#include <dali-ui-foundation/internal/layouts/layout-manager-trait-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-manager.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

// ============================================================================
// LayoutManagerTrait — handle
// ============================================================================

LayoutManagerTrait LayoutManagerTrait::New(LayoutManager* layoutManager)
{
  IntrusivePtr<LayoutManagerTraitImpl> impl(new LayoutManagerTraitImpl(layoutManager));
  return LayoutManagerTrait(impl.Get());
}

LayoutManagerTrait::LayoutManagerTrait(LayoutManagerTraitImpl* impl)
: Trait(impl)
{
}

LayoutManagerTraitImpl& LayoutManagerTrait::GetImpl()
{
  return static_cast<LayoutManagerTraitImpl&>(GetBaseObject());
}

// ============================================================================
// LayoutManagerTraitImpl — implementation
// ============================================================================

LayoutManagerTraitImpl::LayoutManagerTraitImpl(LayoutManager* layoutManager)
: TraitImpl(),
  mLayoutManager(layoutManager)
{
}

LayoutManagerTraitImpl::~LayoutManagerTraitImpl()
{
}

LayoutManager* LayoutManagerTraitImpl::GetLayoutManager() const
{
  return mLayoutManager.get();
}

void LayoutManagerTraitImpl::OnBeforeAttached(TraitId id, View& view)
{
}

void LayoutManagerTraitImpl::OnAttached(TraitId id, View& view)
{
}

void LayoutManagerTraitImpl::OnDetached(TraitId id, View& view)
{
}

void LayoutManagerTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

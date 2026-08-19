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
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Ui
{

LayoutManager::LayoutManager()
: LayoutManager(new Impl())
{
}

LayoutManager::LayoutManager(Impl* impl)
: mImpl(impl)
{
  DALI_ASSERT_ALWAYS(mImpl && "LayoutManager implementation storage must not be null.");
}

LayoutManager::~LayoutManager()
{
  delete mImpl;
}

ArrangePolicy LayoutManager::GetArrangePolicy() const
{
  return mImpl ? mImpl->GetArrangePolicy() : ArrangePolicy::IF_CHANGED;
}

void LayoutManager::SetArrangePolicy(ArrangePolicy policy)
{
  if(mImpl && mImpl->SetArrangePolicy(policy))
  {
    if(ViewImpl* owner = mImpl->GetOwner())
    {
      Internal::ViewDataImpl::Get(*owner).OnLayoutManagerArrangePolicyChanged();
    }
  }
}

void LayoutManager::SetOwnerView(ViewImpl* owner)
{
  if(mImpl)
  {
    mImpl->SetOwner(owner);
  }
}

void LayoutManager::InvalidateOwnerMeasure()
{
  // Null owner == not attached yet (a manager is normally configured before
  // View::AttachLayoutManager runs) -- a no-op then, because there is no cached
  // result to retract and nothing to schedule.
  if(ViewImpl* owner = (mImpl ? mImpl->GetOwner() : nullptr))
  {
    owner->InvalidateMeasure();
  }
}

void LayoutManager::InvalidateOwnerArrange()
{
  if(ViewImpl* owner = (mImpl ? mImpl->GetOwner() : nullptr))
  {
    owner->InvalidateArrange();
  }
}

uint32_t LayoutManager::GetChildViewCount(ViewImpl* view) const
{
  return view ? view->GetChildViewCount() : 0u;
}

View LayoutManager::GetChildViewAt(ViewImpl* view, uint32_t index) const
{
  return view ? view->GetChildViewAt(index) : View();
}

bool LayoutManager::IsStandalone(ViewImpl* child) const
{
  return child && child->GetLayoutMode() == LayoutMode::STANDALONE;
}

} // namespace Ui
} // namespace Dali

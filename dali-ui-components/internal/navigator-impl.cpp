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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/navigator-impl.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

UI::Navigator NavigatorImpl::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Internal::NavigatorImpl> impl = new Internal::NavigatorImpl();

  // Pass ownership to CustomActor handle
  UI::Navigator handle = UI::Navigator(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

NavigatorImpl::NavigatorImpl()
  : ViewImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

void NavigatorImpl::Push(UI::Page page)
{
  if (!page)
  {
    return;
  }

  if (!mPages.empty())
  {
    // Hide or process transition for the top page if necessary
    auto topPage = mPages.back();
    // For simple stacking, we might just hide the old top or put the new one over it
  }

  mPages.push_back(page);

  // Add child to the Navigator View
  Self().Add(page);
}

UI::Page NavigatorImpl::Pop()
{
  if (mPages.empty())
  {
    return UI::Page();
  }

  UI::Page topPage = mPages.back();
  mPages.pop_back();

  // Remove from Navigator View
  Self().Remove(topPage);

  return topPage;
}

void NavigatorImpl::Insert(UI::Page page, uint32_t index)
{
  if (!page)
  {
    return;
  }
  if (index > mPages.size())
  {
    return;
  }

  mPages.insert(mPages.begin() + index, page);
  // Add child to Navigator view
  Self().Add(page);
  // We may need to reorder the Dali::Actor hierarchy, e.g. using LowerToBottom / RaiseToTop
  // but for now simple Addition is done.
}

void NavigatorImpl::Remove(UI::Page page)
{
  if (!page)
  {
    return;
  }

  auto it = std::find(mPages.begin(), mPages.end(), page);
  if (it != mPages.end())
  {
    mPages.erase(it);
    Self().Remove(page);
  }
}

UI::Page NavigatorImpl::GetPage(uint32_t index) const
{
  if (index < mPages.size())
  {
    return mPages[index];
  }
  return UI::Page();
}

uint32_t NavigatorImpl::GetPageCount() const
{
  return static_cast<uint32_t>(mPages.size());
}

} // namespace Internal

} // namespace UI

} // namespace Dali

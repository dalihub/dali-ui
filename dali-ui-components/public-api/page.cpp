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
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/page.h>
#include <dali-ui-components/internal/page-impl.h>

namespace Dali
{

namespace UI
{

Page::Page()
{
}

Page Page::New()
{
  return Internal::PageImpl::New();
}

Page::Page(const Page& page)
  : View(page)
{
}

Page::Page(Page&& rhs) noexcept
  : View(std::move(rhs))
{
}

Page::~Page()
{
}

Page& Page::operator=(const Page& handle)
{
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

Page& Page::operator=(Page&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

Page Page::DownCast(BaseHandle handle)
{
  Page result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::PageImpl* impl = dynamic_cast<Internal::PageImpl*>(&customImpl);
    if (impl)
    {
      result = Page(customImpl.GetOwner());
    }
  }
  return result;
}

Page::Page(Internal::PageImpl& implementation)
  : View(implementation)
{
}

Page::Page(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Internal::PageImpl>(internal);
}

} // namespace UI

} // namespace Dali

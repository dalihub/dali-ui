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
#include <dali-ui-components/public-api/content-page.h>
#include <dali-ui-components/internal/content-page-impl.h>

namespace Dali
{

namespace UI
{

ContentPage::ContentPage()
{
}

ContentPage ContentPage::New()
{
  return Internal::ContentPageImpl::New();
}

ContentPage::ContentPage(const ContentPage& contentPage)
  : Page(contentPage)
{
}

ContentPage::ContentPage(ContentPage&& rhs) noexcept
  : Page(std::move(rhs))
{
}

ContentPage::~ContentPage()
{
}

ContentPage& ContentPage::operator=(const ContentPage& handle)
{
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

ContentPage& ContentPage::operator=(ContentPage&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

ContentPage ContentPage::DownCast(BaseHandle handle)
{
  ContentPage result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::ContentPageImpl* impl = dynamic_cast<Internal::ContentPageImpl*>(&customImpl);
    if (impl)
    {
      result = ContentPage(customImpl.GetOwner());
    }
  }
  return result;
}

ContentPage::ContentPage(Internal::ContentPageImpl& implementation)
  : Page(implementation)
{
}

ContentPage::ContentPage(Dali::Internal::CustomActor* internal)
  : Page(internal)
{
  VerifyCustomActorPointer<Internal::ContentPageImpl>(internal);
}

} // namespace UI

} // namespace Dali

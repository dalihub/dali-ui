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
#include <dali-ui-components/public-api/app-bar.h>
#include <dali-ui-components/internal/app-bar-impl.h>

namespace Dali
{

namespace UI
{

AppBar::AppBar()
{
}

AppBar AppBar::New()
{
  return Internal::AppBarImpl::New();
}

AppBar::AppBar(const AppBar& appBar)
  : View(appBar)
{
}

AppBar::AppBar(AppBar&& rhs) noexcept
  : View(std::move(rhs))
{
}

AppBar::~AppBar()
{
}

AppBar& AppBar::operator=(const AppBar& handle)
{
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

AppBar& AppBar::operator=(AppBar&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

AppBar AppBar::DownCast(BaseHandle handle)
{
  AppBar result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::AppBarImpl* impl = dynamic_cast<Internal::AppBarImpl*>(&customImpl);
    if (impl)
    {
      result = AppBar(customImpl.GetOwner());
    }
  }
  return result;
}

AppBar::AppBar(Internal::AppBarImpl& implementation)
  : View(implementation)
{
}

AppBar::AppBar(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Internal::AppBarImpl>(internal);
}

} // namespace UI

} // namespace Dali

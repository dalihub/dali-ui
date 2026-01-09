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
#include <dali-ui-foundations/public-api/view.h>
#include <dali-ui-foundations/internal/view-impl.h>

namespace Dali
{

namespace UI
{

View::View()
{
}

View View::New()
{
  return Internal::View::New();
}

View::View(const View& view)
: Toolkit::Control(view)
{
}

View::View(View&& rhs) noexcept
: Toolkit::Control(std::move(rhs))
{
}

View::~View()
{
}

View& View::operator=(const View& handle)
{
  if(&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

View& View::operator=(View&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

View View::DownCast(BaseHandle handle)
{
  View result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if(control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::View* impl = dynamic_cast<Internal::View*>(&customImpl);
    if(impl)
    {
      result = View(customImpl.GetOwner());
    }
  }
  return result;
}

View::View(Internal::View& implementation)
: Control(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::View>(internal);
}

} // namespace UI

} // namespace Dali

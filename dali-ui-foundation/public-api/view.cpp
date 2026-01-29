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
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{

namespace UI
{

View::View()
{
}

View View::New()
{
  return Integration::ViewImpl::New();
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
    Integration::ViewImpl* impl = dynamic_cast<Integration::ViewImpl*>(&customImpl);
    if(impl)
    {
      result = View(customImpl.GetOwner());
    }
  }
  return result;
}

View::View(Integration::ViewImpl& implementation)
: Control(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Integration::ViewImpl>(internal);
}

float View::GetSizeWidth() const
{
  return Integration::GetImpl(*this).GetSizeWidth();
}

void View::SetSizeWidth(float width)
{
  Integration::GetImpl(*this).SetSizeWidth(width);
}

float View::GetSizeHeight() const
{
  return Integration::GetImpl(*this).GetSizeHeight();
}

void View::SetSizeHeight(float height)
{
  Integration::GetImpl(*this).SetSizeHeight(height);
}

float View::GetPositionX() const
{
  return Integration::GetImpl(*this).GetPositionX();
}

void View::SetPositionX(float x)
{
  Integration::GetImpl(*this).SetPositionX(x);
}

float View::GetPositionY() const
{
  return Integration::GetImpl(*this).GetPositionY();
}

void View::SetPositionY(float y)
{
  Integration::GetImpl(*this).SetPositionY(y);
}

Vector3 View::GetParentOrigin() const
{
  return Integration::GetImpl(*this).GetParentOrigin();
}

void View::SetParentOrigin(const Vector3& point)
{
  Integration::GetImpl(*this).SetParentOrigin(point);
}

Vector3 View::GetPivotPoint() const
{
  return Integration::GetImpl(*this).GetPivotPoint();
}

void View::SetPivotPoint(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivotPoint(point);
}

} // namespace UI

} // namespace Dali

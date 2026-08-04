#pragma once

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-context.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class FocusNavigationContextImpl;
using FocusNavigationContextImplPtr = IntrusivePtr<FocusNavigationContextImpl>;

class FocusNavigationContextImpl : public BaseObject
{
public:
  FocusNavigationContextImpl(FocusDirection direction,
                             FocusDevice    device,
                             Dali::String   deviceName,
                             InputEvent     inputEvent,
                             Window         window,
                             View           focusGroup);

  FocusDirection      GetDirection() const;
  FocusDevice         GetDevice() const;
  const Dali::String& GetDeviceName() const;
  InputEvent          GetInputEvent() const;
  Window              GetWindow() const;
  View                GetFocusGroup() const;

private:
  ~FocusNavigationContextImpl() override = default;

private:
  FocusDirection mDirection;
  FocusDevice    mDevice;
  Dali::String   mDeviceName;
  InputEvent     mInputEvent;
  Window         mWindow;
  View           mFocusGroup;
};

inline FocusNavigationContextImpl& GetImpl(FocusNavigationContext& context)
{
  return static_cast<FocusNavigationContextImpl&>(context.GetBaseObject());
}

inline const FocusNavigationContextImpl& GetImpl(const FocusNavigationContext& context)
{
  return static_cast<const FocusNavigationContextImpl&>(context.GetBaseObject());
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

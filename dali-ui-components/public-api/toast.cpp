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

// CLASS HEADER
#include <dali-ui-components/public-api/toast.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/toast-impl.h>

namespace Dali
{
namespace Ui
{

Toast::Toast()  = default;
Toast::~Toast() = default;

Toast Toast::New()
{
  return Internal::ToastImpl::New(ToastStyle::Default());
}

Toast Toast::New(ToastStyle style)
{
  return Internal::ToastImpl::New(style);
}

Toast Toast::New(const Dali::String& text)
{
  Toast toast = New();
  toast.SetText(text);
  return toast;
}

Toast Toast::New(const Dali::String& text, ToastStyle style)
{
  Toast toast = New(style);
  toast.SetText(text);
  return toast;
}

Toast Toast::New(const Dali::String& text, uint32_t duration)
{
  Toast toast = New(text);
  toast.SetDuration(duration);
  return toast;
}

Toast Toast::New(const Dali::String& text, uint32_t duration, ToastStyle style)
{
  Toast toast = New(text, style);
  toast.SetDuration(duration);
  return toast;
}

Toast Toast::DownCast(BaseHandle handle)
{
  return View::DownCast<Toast, Internal::ToastImpl>(handle);
}

Toast::Toast(const Toast& handle)             = default;
Toast::Toast(Toast&& rhs) noexcept            = default;
Toast& Toast::operator=(const Toast& handle)  = default;
Toast& Toast::operator=(Toast&& rhs) noexcept = default;

#define DALI_TOAST_FORWARD_SET_GET(Type, Name)                        \
  void Toast::Set##Name(Type value)                                   \
  {                                                                   \
    GetImpl(*this).Set##Name(value);                                  \
  }                                                                   \
  auto Toast::Get##Name() const->decltype(GetImpl(*this).Get##Name()) \
  {                                                                   \
    return GetImpl(*this).Get##Name();                                \
  }

DALI_TOAST_FORWARD_SET_GET(const Dali::String&, Text)
DALI_TOAST_FORWARD_SET_GET(uint32_t, Duration)
DALI_TOAST_FORWARD_SET_GET(const UiColor&, TextColor)
DALI_TOAST_FORWARD_SET_GET(float, FontSize)
DALI_TOAST_FORWARD_SET_GET(const Dali::String&, FontFamily)
DALI_TOAST_FORWARD_SET_GET(const Dali::String&, ActionButtonText)
DALI_TOAST_FORWARD_SET_GET(const UiColor&, ActionButtonTextColor)
DALI_TOAST_FORWARD_SET_GET(float, ItemSpacing)

#undef DALI_TOAST_FORWARD_SET_GET

void Toast::Post(Dali::Window window)
{
  GetImpl(*this).Post(window);
}

void Toast::Dismiss()
{
  GetImpl(*this).Dismiss();
}

Toast::ShownSignalType& Toast::ShownSignal()
{
  return GetImpl(*this).ShownSignal();
}

Toast::HiddenSignalType& Toast::HiddenSignal()
{
  return GetImpl(*this).HiddenSignal();
}

Toast::ActionButtonClickedSignalType& Toast::ActionButtonClickedSignal()
{
  return GetImpl(*this).ActionButtonClickedSignal();
}

Toast::Toast(Internal::ToastImpl& implementation)
: View(implementation)
{
}

Toast::Toast(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::ToastImpl>(internal);
}

} // namespace Ui
} // namespace Dali

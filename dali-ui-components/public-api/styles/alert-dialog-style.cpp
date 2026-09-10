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

#include <dali-ui-components/internal/styles/alert-dialog-style-impl.h>
#include <dali-ui-components/internal/styles/style-validation.h>
#include <dali-ui-components/public-api/styles/alert-dialog-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <cmath>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace
{
Internal::AlertDialogStyleImpl& GetStyleImpl(UiStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "AlertDialogStyle builder has already been consumed");
  return static_cast<Internal::AlertDialogStyleImpl&>(style.GetBaseObject());
}
const Internal::AlertDialogStyleImpl& GetStyleImpl(const AlertDialogStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "AlertDialogStyle must be initialized");
  return static_cast<const Internal::AlertDialogStyleImpl&>(style.GetBaseObject());
}
} // namespace

namespace Internal
{
TextButtonStyle ResolveAlertActionButtonStyle(AlertDialogStyle style)
{
  auto action = style.GetActionButtonStyle();
  if(action)
  {
    return action;
  }
  // Preserve configured button geometry/effects and the legacy Alert palette.
  return TextButtonStyle::Default().Configure().SetBackgroundColor(UiColor(0x3367D6u)).SetTextColor(UiColor(0xFFFFFFu)).SetFontSize(16.0f).Build();
}
} // namespace Internal

UiStyleKey<AlertDialogStyle> AlertDialogStyle::DefaultKey()
{
  static auto key = UiStyleKey<AlertDialogStyle>::Alloc();
  return key;
}
AlertDialogStyle AlertDialogStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static auto style = Builder().Build();
  return style;
}
AlertDialogStyle AlertDialogStyle::Default()
{
  DebugAssertStyleConfigApplied();
  auto style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}
AlertDialogStyle AlertDialogStyle::DownCast(BaseHandle handle)
{
  return AlertDialogStyle(dynamic_cast<Internal::AlertDialogStyleImpl*>(handle.GetObjectPtr()));
}
AlertDialogStyle AlertDialogStyle::StaticDownCast(UiStyle style)
{
  return AlertDialogStyle(static_cast<Internal::AlertDialogStyleImpl*>(style.GetObjectPtr()));
}
AlertDialogStyle::Builder AlertDialogStyle::Configure() const
{
  return Builder(AlertDialogStyle(new Internal::AlertDialogStyleImpl(GetStyleImpl(*this))));
}
AlertDialogStyle::AlertDialogStyle(Extension::UiStyleImpl* impl)
: UiStyle(impl)
{
}

DialogStyle AlertDialogStyle::GetDialogStyle() const
{
  return GetStyleImpl(*this).values.mDialogStyle;
}
UiColor AlertDialogStyle::GetTitleTextColor() const
{
  return GetStyleImpl(*this).values.mTitleTextColor;
}
float AlertDialogStyle::GetTitleFontSize() const
{
  return GetStyleImpl(*this).values.mTitleFontSize;
}
Dali::String AlertDialogStyle::GetTitleFontFamily() const
{
  return GetStyleImpl(*this).values.mTitleFontFamily;
}
UiColor AlertDialogStyle::GetMessageTextColor() const
{
  return GetStyleImpl(*this).values.mMessageTextColor;
}
float AlertDialogStyle::GetMessageFontSize() const
{
  return GetStyleImpl(*this).values.mMessageFontSize;
}
Dali::String AlertDialogStyle::GetMessageFontFamily() const
{
  return GetStyleImpl(*this).values.mMessageFontFamily;
}
TextButtonStyle AlertDialogStyle::GetActionButtonStyle() const
{
  return GetStyleImpl(*this).values.mActionButtonStyle;
}
float AlertDialogStyle::GetActionRowHeight() const
{
  return GetStyleImpl(*this).values.mActionRowHeight;
}
float AlertDialogStyle::GetActionButtonSpacing() const
{
  return GetStyleImpl(*this).values.mActionButtonSpacing;
}

AlertDialogStyle::Builder::Builder()
: mStyle(AlertDialogStyle(new Internal::AlertDialogStyleImpl()))
{
}
AlertDialogStyle::Builder::Builder(UiStyle style)
: mStyle(std::move(style))
{
}
AlertDialogStyle::Builder::Builder(Builder&& rhs) noexcept                              = default;
AlertDialogStyle::Builder& AlertDialogStyle::Builder::operator=(Builder&& rhs) noexcept = default;
AlertDialogStyle::Builder::~Builder()                                                   = default;

AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetDialogStyle(DialogStyle value) &
{
  GetStyleImpl(mStyle).values.mDialogStyle = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetDialogStyle(DialogStyle value) &&
{
  SetDialogStyle(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetTitleTextColor(const UiColor& value) &
{
  GetStyleImpl(mStyle).values.mTitleTextColor = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetTitleTextColor(const UiColor& value) &&
{
  SetTitleTextColor(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetTitleFontSize(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "AlertDialogStyle TitleFontSize is invalid");
  GetStyleImpl(mStyle).values.mTitleFontSize = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetTitleFontSize(float value) &&
{
  SetTitleFontSize(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetTitleFontFamily(const Dali::String& value) &
{
  GetStyleImpl(mStyle).values.mTitleFontFamily = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetTitleFontFamily(const Dali::String& value) &&
{
  SetTitleFontFamily(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetMessageTextColor(const UiColor& value) &
{
  GetStyleImpl(mStyle).values.mMessageTextColor = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetMessageTextColor(const UiColor& value) &&
{
  SetMessageTextColor(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetMessageFontSize(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "AlertDialogStyle MessageFontSize is invalid");
  GetStyleImpl(mStyle).values.mMessageFontSize = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetMessageFontSize(float value) &&
{
  SetMessageFontSize(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetMessageFontFamily(const Dali::String& value) &
{
  GetStyleImpl(mStyle).values.mMessageFontFamily = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetMessageFontFamily(const Dali::String& value) &&
{
  SetMessageFontFamily(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetActionButtonStyle(TextButtonStyle value) &
{
  GetStyleImpl(mStyle).values.mActionButtonStyle = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetActionButtonStyle(TextButtonStyle value) &&
{
  SetActionButtonStyle(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetActionRowHeight(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "AlertDialogStyle ActionRowHeight is invalid");
  GetStyleImpl(mStyle).values.mActionRowHeight = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetActionRowHeight(float value) &&
{
  SetActionRowHeight(value);
  return std::move(*this);
}
AlertDialogStyle::Builder& AlertDialogStyle::Builder::SetActionButtonSpacing(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "AlertDialogStyle ActionButtonSpacing is invalid");
  GetStyleImpl(mStyle).values.mActionButtonSpacing = value;
  return *this;
}
AlertDialogStyle::Builder&& AlertDialogStyle::Builder::SetActionButtonSpacing(float value) &&
{
  SetActionButtonSpacing(value);
  return std::move(*this);
}
AlertDialogStyle AlertDialogStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mStyle && "AlertDialogStyle builder has already been consumed");
  auto style = AlertDialogStyle::StaticDownCast(mStyle);
  mStyle.Reset();
  return style;
}
} // namespace Ui
} // namespace DALI_NAMESPACE

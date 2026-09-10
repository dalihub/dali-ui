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

#include <dali-ui-components/internal/styles/dialog-container-style-impl.h>
#include <dali-ui-components/internal/styles/style-validation.h>
#include <dali-ui-components/public-api/styles/dialog-container-style.h>
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
Internal::DialogContainerStyleImpl& GetStyleImpl(UiStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "DialogContainerStyle builder has already been consumed");
  return static_cast<Internal::DialogContainerStyleImpl&>(style.GetBaseObject());
}
const Internal::DialogContainerStyleImpl& GetStyleImpl(const DialogContainerStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "DialogContainerStyle must be initialized");
  return static_cast<const Internal::DialogContainerStyleImpl&>(style.GetBaseObject());
}
} // namespace

UiStyleKey<DialogContainerStyle> DialogContainerStyle::DefaultKey()
{
  static auto key = UiStyleKey<DialogContainerStyle>::Alloc();
  return key;
}
DialogContainerStyle DialogContainerStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static auto style = Builder().Build();
  return style;
}
DialogContainerStyle DialogContainerStyle::Default()
{
  DebugAssertStyleConfigApplied();
  auto style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}
DialogContainerStyle DialogContainerStyle::DownCast(BaseHandle handle)
{
  return DialogContainerStyle(dynamic_cast<Internal::DialogContainerStyleImpl*>(handle.GetObjectPtr()));
}
DialogContainerStyle DialogContainerStyle::StaticDownCast(UiStyle style)
{
  return DialogContainerStyle(static_cast<Internal::DialogContainerStyleImpl*>(style.GetObjectPtr()));
}
DialogContainerStyle::Builder DialogContainerStyle::Configure() const
{
  return Builder(DialogContainerStyle(new Internal::DialogContainerStyleImpl(GetStyleImpl(*this))));
}
DialogContainerStyle::DialogContainerStyle(Extension::UiStyleImpl* impl)
: UiStyle(impl)
{
}

UiColor DialogContainerStyle::GetScrimColor() const
{
  return GetStyleImpl(*this).values.mScrimColor;
}
float DialogContainerStyle::GetScrimBlurRadius() const
{
  return GetStyleImpl(*this).values.mScrimBlurRadius;
}

DialogContainerStyle::Builder::Builder()
: mStyle(DialogContainerStyle(new Internal::DialogContainerStyleImpl()))
{
}
DialogContainerStyle::Builder::Builder(UiStyle style)
: mStyle(std::move(style))
{
}
DialogContainerStyle::Builder::Builder(Builder&& rhs) noexcept                                  = default;
DialogContainerStyle::Builder& DialogContainerStyle::Builder::operator=(Builder&& rhs) noexcept = default;
DialogContainerStyle::Builder::~Builder()                                                       = default;

DialogContainerStyle::Builder& DialogContainerStyle::Builder::SetScrimColor(const UiColor& value) &
{
  GetStyleImpl(mStyle).values.mScrimColor = value;
  return *this;
}
DialogContainerStyle::Builder&& DialogContainerStyle::Builder::SetScrimColor(const UiColor& value) &&
{
  SetScrimColor(value);
  return std::move(*this);
}
DialogContainerStyle::Builder& DialogContainerStyle::Builder::SetScrimBlurRadius(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsBlurRadius(value) && "DialogContainerStyle ScrimBlurRadius is invalid");
  GetStyleImpl(mStyle).values.mScrimBlurRadius = value;
  return *this;
}
DialogContainerStyle::Builder&& DialogContainerStyle::Builder::SetScrimBlurRadius(float value) &&
{
  SetScrimBlurRadius(value);
  return std::move(*this);
}
DialogContainerStyle DialogContainerStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mStyle && "DialogContainerStyle builder has already been consumed");
  auto style = DialogContainerStyle::StaticDownCast(mStyle);
  mStyle.Reset();
  return style;
}
} // namespace Ui
} // namespace DALI_NAMESPACE

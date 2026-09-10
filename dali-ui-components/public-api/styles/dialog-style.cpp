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

#include <dali-ui-components/internal/styles/dialog-style-impl.h>
#include <dali-ui-components/internal/styles/style-validation.h>
#include <dali-ui-components/public-api/styles/dialog-style.h>
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
Internal::DialogStyleImpl& GetStyleImpl(UiStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "DialogStyle builder has already been consumed");
  return static_cast<Internal::DialogStyleImpl&>(style.GetBaseObject());
}
const Internal::DialogStyleImpl& GetStyleImpl(const DialogStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "DialogStyle must be initialized");
  return static_cast<const Internal::DialogStyleImpl&>(style.GetBaseObject());
}
} // namespace

UiStyleKey<DialogStyle> DialogStyle::DefaultKey()
{
  static auto key = UiStyleKey<DialogStyle>::Alloc();
  return key;
}
DialogStyle DialogStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static auto style = Builder().Build();
  return style;
}
DialogStyle DialogStyle::Default()
{
  DebugAssertStyleConfigApplied();
  auto style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}
DialogStyle DialogStyle::DownCast(BaseHandle handle)
{
  return DialogStyle(dynamic_cast<Internal::DialogStyleImpl*>(handle.GetObjectPtr()));
}
DialogStyle DialogStyle::StaticDownCast(UiStyle style)
{
  return DialogStyle(static_cast<Internal::DialogStyleImpl*>(style.GetObjectPtr()));
}
DialogStyle::Builder DialogStyle::Configure() const
{
  return Builder(DialogStyle(new Internal::DialogStyleImpl(GetStyleImpl(*this))));
}
DialogStyle::DialogStyle(Extension::UiStyleImpl* impl)
: UiStyle(impl)
{
}

float DialogStyle::GetRequestedWidth() const
{
  return GetStyleImpl(*this).values.mRequestedWidth;
}
float DialogStyle::GetRequestedHeight() const
{
  return GetStyleImpl(*this).values.mRequestedHeight;
}
UiColor DialogStyle::GetBackgroundColor() const
{
  return GetStyleImpl(*this).values.mBackgroundColor;
}
Insets DialogStyle::GetPadding() const
{
  return GetStyleImpl(*this).values.mPadding;
}
float DialogStyle::GetSpacing() const
{
  return GetStyleImpl(*this).values.mSpacing;
}
Vector4 DialogStyle::GetCornerRadius() const
{
  return GetStyleImpl(*this).values.mCornerRadius;
}
CornerRadiusPolicy DialogStyle::GetCornerRadiusPolicy() const
{
  return GetStyleImpl(*this).values.mCornerRadiusPolicy;
}
ShadowStack DialogStyle::GetShadow() const
{
  return GetStyleImpl(*this).values.mShadow;
}
float DialogStyle::GetBorderlineWidth() const
{
  return GetStyleImpl(*this).values.mBorderlineWidth;
}
float DialogStyle::GetBorderlineOffset() const
{
  return GetStyleImpl(*this).values.mBorderlineOffset;
}
UiColor DialogStyle::GetBorderlineColor() const
{
  return GetStyleImpl(*this).values.mBorderlineColor;
}
float DialogStyle::GetBackgroundBlurRadius() const
{
  return GetStyleImpl(*this).values.mBackgroundBlurRadius;
}

DialogStyle::Builder::Builder()
: mStyle(DialogStyle(new Internal::DialogStyleImpl()))
{
}
DialogStyle::Builder::Builder(UiStyle style)
: mStyle(std::move(style))
{
}
DialogStyle::Builder::Builder(Builder&& rhs) noexcept                         = default;
DialogStyle::Builder& DialogStyle::Builder::operator=(Builder&& rhs) noexcept = default;
DialogStyle::Builder::~Builder()                                              = default;

DialogStyle::Builder& DialogStyle::Builder::SetRequestedWidth(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsRequestedDimension(value) && "DialogStyle RequestedWidth is invalid");
  GetStyleImpl(mStyle).values.mRequestedWidth = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetRequestedWidth(float value) &&
{
  SetRequestedWidth(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetRequestedHeight(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsRequestedDimension(value) && "DialogStyle RequestedHeight is invalid");
  GetStyleImpl(mStyle).values.mRequestedHeight = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetRequestedHeight(float value) &&
{
  SetRequestedHeight(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetBackgroundColor(const UiColor& value) &
{
  GetStyleImpl(mStyle).values.mBackgroundColor = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetBackgroundColor(const UiColor& value) &&
{
  SetBackgroundColor(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetPadding(const Insets& value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "DialogStyle Padding is invalid");
  GetStyleImpl(mStyle).values.mPadding = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetPadding(const Insets& value) &&
{
  SetPadding(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetSpacing(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "DialogStyle Spacing is invalid");
  GetStyleImpl(mStyle).values.mSpacing = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetSpacing(float value) &&
{
  SetSpacing(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetCornerRadius(const Vector4& value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "DialogStyle CornerRadius is invalid");
  GetStyleImpl(mStyle).values.mCornerRadius = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetCornerRadius(const Vector4& value) &&
{
  SetCornerRadius(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy value) &
{
  DALI_ASSERT_ALWAYS(((value == CornerRadiusPolicy::ABSOLUTE || value == CornerRadiusPolicy::RELATIVE)) && "DialogStyle CornerRadiusPolicy is invalid");
  GetStyleImpl(mStyle).values.mCornerRadiusPolicy = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy value) &&
{
  SetCornerRadiusPolicy(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetShadow(const ShadowStack& value) &
{
  GetStyleImpl(mStyle).values.mShadow = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetShadow(const ShadowStack& value) &&
{
  SetShadow(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetBorderlineWidth(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsNonNegative(value) && "DialogStyle BorderlineWidth is invalid");
  GetStyleImpl(mStyle).values.mBorderlineWidth = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetBorderlineWidth(float value) &&
{
  SetBorderlineWidth(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetBorderlineOffset(float value) &
{
  DALI_ASSERT_ALWAYS((std::isfinite(value)) && "DialogStyle BorderlineOffset is invalid");
  GetStyleImpl(mStyle).values.mBorderlineOffset = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetBorderlineOffset(float value) &&
{
  SetBorderlineOffset(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetBorderlineColor(const UiColor& value) &
{
  GetStyleImpl(mStyle).values.mBorderlineColor = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetBorderlineColor(const UiColor& value) &&
{
  SetBorderlineColor(value);
  return std::move(*this);
}
DialogStyle::Builder& DialogStyle::Builder::SetBackgroundBlurRadius(float value) &
{
  DALI_ASSERT_ALWAYS(Internal::StyleValidation::IsBlurRadius(value) && "DialogStyle BackgroundBlurRadius is invalid");
  GetStyleImpl(mStyle).values.mBackgroundBlurRadius = value;
  return *this;
}
DialogStyle::Builder&& DialogStyle::Builder::SetBackgroundBlurRadius(float value) &&
{
  SetBackgroundBlurRadius(value);
  return std::move(*this);
}
DialogStyle DialogStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mStyle && "DialogStyle builder has already been consumed");
  auto style = DialogStyle::StaticDownCast(mStyle);
  mStyle.Reset();
  return style;
}
} // namespace Ui
} // namespace DALI_NAMESPACE

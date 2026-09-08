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

#include <dali-ui-components/public-api/styles/toast-style.h>

#include <cmath>
#include <utility>

#include <dali-ui-components/internal/styles/toast-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/types/shadow.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace
{
bool IsFiniteNonNegative(float value)
{
  return std::isfinite(value) && value >= 0.0f;
}

bool AreValidInsets(const Insets& value)
{
  return IsFiniteNonNegative(value.start) && IsFiniteNonNegative(value.end) &&
         IsFiniteNonNegative(value.top) && IsFiniteNonNegative(value.bottom);
}

bool IsValidRadius(const Vector4& value)
{
  return IsFiniteNonNegative(value.x) && IsFiniteNonNegative(value.y) &&
         IsFiniteNonNegative(value.z) && IsFiniteNonNegative(value.w);
}

ShadowStack CreateDefaultShadow()
{
  return ShadowStack{
    Shadow(8.0f, Vector2::ZERO, UiColor(0x000000u, 0.05f), Vector2::ZERO, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS),
    Shadow(32.0f, Vector2(0.0f, 16.0f), UiColor(0x000000u, 0.10f), Vector2::ZERO, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS)};
}
} // namespace

UiStyleKey<ToastStyle> ToastStyle::DefaultKey()
{
  static UiStyleKey<ToastStyle> key = UiStyleKey<ToastStyle>::Alloc();
  return key;
}

ToastStyle ToastStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static ToastStyle style = ToastStyle::Builder().Build();
  return style;
}

ToastStyle ToastStyle::Default()
{
  DebugAssertStyleConfigApplied();
  ToastStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

ToastStyle ToastStyle::DownCast(BaseHandle handle)
{
  return ToastStyle(dynamic_cast<Internal::ToastStyleImpl*>(handle.GetObjectPtr()));
}

ToastStyle ToastStyle::StaticDownCast(UiStyle style)
{
  return ToastStyle(static_cast<Internal::ToastStyleImpl*>(style.GetObjectPtr()));
}

ToastStyle::Builder ToastStyle::Configure() const
{
  IntrusivePtr<Internal::ToastStyleImpl> impl(new Internal::ToastStyleImpl(Internal::GetImpl(*this)));
  return Builder(impl.Get());
}

#define DALI_TOAST_STYLE_GETTER(Type, Name)      \
  Type ToastStyle::Get##Name() const             \
  {                                              \
    return Internal::GetImpl(*this).Get##Name(); \
  }

DALI_TOAST_STYLE_GETTER(float, MaximumWidthRatio)
DALI_TOAST_STYLE_GETTER(float, MaximumHeight)
DALI_TOAST_STYLE_GETTER(float, BottomOffsetRatio)
DALI_TOAST_STYLE_GETTER(float, ItemSpacing)
DALI_TOAST_STYLE_GETTER(Vector4, CornerRadius)
DALI_TOAST_STYLE_GETTER(Insets, Padding)
DALI_TOAST_STYLE_GETTER(UiColor, BackgroundColor)
DALI_TOAST_STYLE_GETTER(UiColor, TextColor)
DALI_TOAST_STYLE_GETTER(float, FontSize)
DALI_TOAST_STYLE_GETTER(Dali::String, FontFamily)
DALI_TOAST_STYLE_GETTER(float, IconWidth)
DALI_TOAST_STYLE_GETTER(float, IconHeight)
DALI_TOAST_STYLE_GETTER(Vector4, IconCornerRadius)
DALI_TOAST_STYLE_GETTER(UiColor, IconColor)
DALI_TOAST_STYLE_GETTER(ShadowStack, Shadow)
DALI_TOAST_STYLE_GETTER(float, BorderlineWidth)
DALI_TOAST_STYLE_GETTER(float, BorderlineOffset)
DALI_TOAST_STYLE_GETTER(UiColor, BorderlineColor)
#undef DALI_TOAST_STYLE_GETTER

ToastStyle::ToastStyle(Internal::ToastStyleImpl* impl)
: UiStyle(impl)
{
}

ToastStyle::Builder::Builder()
: mImpl(new Internal::ToastStyleImpl())
{
}

ToastStyle::Builder::Builder(Builder&& rhs) noexcept                        = default;
ToastStyle::Builder& ToastStyle::Builder::operator=(Builder&& rhs) noexcept = default;
ToastStyle::Builder::~Builder()                                             = default;

#define DALI_TOAST_STYLE_SETTER(Name, Type)                  \
  ToastStyle::Builder& ToastStyle::Builder::Name(Type v) &   \
  {                                                          \
    mImpl->Name(v);                                          \
    return *this;                                            \
  }                                                          \
  ToastStyle::Builder&& ToastStyle::Builder::Name(Type v) && \
  {                                                          \
    Name(v);                                                 \
    return std::move(*this);                                 \
  }

DALI_TOAST_STYLE_SETTER(SetMaximumWidthRatio, float)
DALI_TOAST_STYLE_SETTER(SetMaximumHeight, float)
DALI_TOAST_STYLE_SETTER(SetBottomOffsetRatio, float)
DALI_TOAST_STYLE_SETTER(SetItemSpacing, float)
DALI_TOAST_STYLE_SETTER(SetCornerRadius, const Vector4&)
DALI_TOAST_STYLE_SETTER(SetPadding, const Insets&)
DALI_TOAST_STYLE_SETTER(SetBackgroundColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetTextColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetFontSize, float)
DALI_TOAST_STYLE_SETTER(SetFontFamily, const Dali::String&)
DALI_TOAST_STYLE_SETTER(SetIconWidth, float)
DALI_TOAST_STYLE_SETTER(SetIconHeight, float)
DALI_TOAST_STYLE_SETTER(SetIconCornerRadius, const Vector4&)
DALI_TOAST_STYLE_SETTER(SetIconColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetShadow, const ShadowStack&)
DALI_TOAST_STYLE_SETTER(SetBorderlineWidth, float)
DALI_TOAST_STYLE_SETTER(SetBorderlineOffset, float)
DALI_TOAST_STYLE_SETTER(SetBorderlineColor, const UiColor&)
#undef DALI_TOAST_STYLE_SETTER

ToastStyle ToastStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "ToastStyle::Builder has already been consumed");
  ToastStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

ToastStyle::Builder::Builder(Internal::ToastStyleImpl* impl)
: mImpl(impl)
{
}

namespace Internal
{
ToastStyleImpl::ToastStyleImpl()
: mShadow(CreateDefaultShadow())
{
}

ToastStyleImpl::ToastStyleImpl(const ToastStyleImpl& rhs)
: mMaximumWidthRatio(rhs.mMaximumWidthRatio),
  mMaximumHeight(rhs.mMaximumHeight),
  mBottomOffsetRatio(rhs.mBottomOffsetRatio),
  mItemSpacing(rhs.mItemSpacing),
  mCornerRadius(rhs.mCornerRadius),
  mPadding(rhs.mPadding),
  mBackgroundColor(rhs.mBackgroundColor),
  mTextColor(rhs.mTextColor),
  mFontSize(rhs.mFontSize),
  mFontFamily(rhs.mFontFamily),
  mIconWidth(rhs.mIconWidth),
  mIconHeight(rhs.mIconHeight),
  mIconCornerRadius(rhs.mIconCornerRadius),
  mIconColor(rhs.mIconColor),
  mShadow(rhs.mShadow),
  mBorderlineWidth(rhs.mBorderlineWidth),
  mBorderlineOffset(rhs.mBorderlineOffset),
  mBorderlineColor(rhs.mBorderlineColor)
{
}

void ToastStyleImpl::SetMaximumWidthRatio(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value > 0.0f && value <= 1.0f && "ToastStyle maximum width ratio must be finite, positive, and no greater than one");
  mMaximumWidthRatio = value;
}

void ToastStyleImpl::SetMaximumHeight(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle maximum height must be finite and non-negative");
  mMaximumHeight = value;
}

void ToastStyleImpl::SetBottomOffsetRatio(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value > 0.0f && value <= 1.0f && "ToastStyle bottom offset ratio must be finite, positive, and no greater than one");
  mBottomOffsetRatio = value;
}

void ToastStyleImpl::SetItemSpacing(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle item spacing must be finite and non-negative");
  mItemSpacing = value;
}

void ToastStyleImpl::SetCornerRadius(const Vector4& value)
{
  DALI_ASSERT_ALWAYS(IsValidRadius(value) && "ToastStyle corner radius must be finite and non-negative");
  mCornerRadius = value;
}

void ToastStyleImpl::SetPadding(const Insets& value)
{
  DALI_ASSERT_ALWAYS(AreValidInsets(value) && "ToastStyle padding must be finite and non-negative");
  mPadding = value;
}

void ToastStyleImpl::SetFontSize(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle font size must be finite and non-negative");
  mFontSize = value;
}

void ToastStyleImpl::SetIconWidth(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle icon width must be finite and non-negative");
  mIconWidth = value;
}

void ToastStyleImpl::SetIconHeight(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle icon height must be finite and non-negative");
  mIconHeight = value;
}

void ToastStyleImpl::SetIconCornerRadius(const Vector4& value)
{
  DALI_ASSERT_ALWAYS(IsValidRadius(value) && "ToastStyle icon corner radius must be finite and non-negative");
  mIconCornerRadius = value;
}

void ToastStyleImpl::SetBorderlineWidth(float value)
{
  DALI_ASSERT_ALWAYS(IsFiniteNonNegative(value) && "ToastStyle borderline width must be finite and non-negative");
  mBorderlineWidth = value;
}

void ToastStyleImpl::SetBorderlineOffset(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && "ToastStyle borderline offset must be finite");
  mBorderlineOffset = value;
}
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

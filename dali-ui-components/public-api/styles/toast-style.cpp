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
#include <dali-ui-components/public-api/styles/toast-style.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/styles/toast-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/types/shadow.h>

namespace Dali
{
namespace Ui
{
namespace
{

bool IsFiniteNonNegative(float value)
{
  return std::isfinite(value) && value >= 0.0f;
}

bool IsFinitePositive(float value)
{
  return std::isfinite(value) && value > 0.0f;
}

bool IsFiniteRatio(float value)
{
  return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

bool IsFinitePositiveRatio(float value)
{
  return std::isfinite(value) && value > 0.0f && value <= 1.0f;
}

bool AreValidInsets(const Insets& value)
{
  return std::isfinite(value.start) && value.start >= 0.0f &&
         std::isfinite(value.end) && value.end >= 0.0f &&
         std::isfinite(value.top) && value.top >= 0.0f &&
         std::isfinite(value.bottom) && value.bottom >= 0.0f;
}

bool IsValidRadius(const Vector4& value)
{
  return std::isfinite(value.x) && value.x >= 0.0f &&
         std::isfinite(value.y) && value.y >= 0.0f &&
         std::isfinite(value.z) && value.z >= 0.0f &&
         std::isfinite(value.w) && value.w >= 0.0f;
}

TextButtonStyle CreateDefaultActionButtonStyle()
{
  return TextButtonStyle::Default()
    .Configure()
    .SetMinimumHeight(56.0f)
    .SetPadding(Insets(20.0f, 20.0f, 0.0f, 0.0f))
    .SetHorizontalAlignment(LayoutAlignment::CENTER)
    .SetVerticalAlignment(LayoutAlignment::CENTER)
    .SetTextColor(UiColor(0x010102u))
    .SetFontSize(24.0f)
    .SetFontFamily("SamsungOneUI600")
    .SetBackgroundColor(UiColor("OnSurfaceContainerFixed"))
    .Build();
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
  IntrusivePtr<Internal::ToastStyleImpl> impl(new Internal::ToastStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

#define DALI_TOAST_STYLE_GETTER(Type, Name) \
  Type ToastStyle::Get##Name() const        \
  {                                         \
    return GetImpl(*this).Get##Name();      \
  }

DALI_TOAST_STYLE_GETTER(float, InitialWidth)
DALI_TOAST_STYLE_GETTER(float, InitialHeight)
DALI_TOAST_STYLE_GETTER(float, MinimumWidth)
DALI_TOAST_STYLE_GETTER(float, MinimumHeight)
DALI_TOAST_STYLE_GETTER(float, ExpandedHeight)
DALI_TOAST_STYLE_GETTER(float, MaximumHeight)
DALI_TOAST_STYLE_GETTER(float, MaximumWidthRatio)
DALI_TOAST_STYLE_GETTER(float, MaximumWidth)
DALI_TOAST_STYLE_GETTER(float, BottomOffsetRatio)
DALI_TOAST_STYLE_GETTER(float, BottomOffset)
DALI_TOAST_STYLE_GETTER(Vector4, CornerRadius)
DALI_TOAST_STYLE_GETTER(CornerRadiusPolicy, CornerRadiusPolicy)
DALI_TOAST_STYLE_GETTER(Insets, Padding)
DALI_TOAST_STYLE_GETTER(Insets, ExpandedPadding)
DALI_TOAST_STYLE_GETTER(float, ItemSpacing)
DALI_TOAST_STYLE_GETTER(float, ExpandedItemSpacing)
DALI_TOAST_STYLE_GETTER(UiColor, BackgroundColor)
DALI_TOAST_STYLE_GETTER(UiColor, TextColor)
DALI_TOAST_STYLE_GETTER(float, FontSize)
DALI_TOAST_STYLE_GETTER(Dali::String, FontFamily)
DALI_TOAST_STYLE_GETTER(ShadowStack, Shadow)
DALI_TOAST_STYLE_GETTER(float, BorderlineWidth)
DALI_TOAST_STYLE_GETTER(UiColor, BorderlineColor)
DALI_TOAST_STYLE_GETTER(float, BorderlineOffset)
DALI_TOAST_STYLE_GETTER(TextButtonStyle, ActionButtonStyle)

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

DALI_TOAST_STYLE_SETTER(SetInitialWidth, float)
DALI_TOAST_STYLE_SETTER(SetInitialHeight, float)
DALI_TOAST_STYLE_SETTER(SetMinimumWidth, float)
DALI_TOAST_STYLE_SETTER(SetMinimumHeight, float)
DALI_TOAST_STYLE_SETTER(SetExpandedHeight, float)
DALI_TOAST_STYLE_SETTER(SetMaximumHeight, float)
DALI_TOAST_STYLE_SETTER(SetMaximumWidthRatio, float)
DALI_TOAST_STYLE_SETTER(SetMaximumWidth, float)
DALI_TOAST_STYLE_SETTER(SetBottomOffsetRatio, float)
DALI_TOAST_STYLE_SETTER(SetBottomOffset, float)
DALI_TOAST_STYLE_SETTER(SetItemSpacing, float)
DALI_TOAST_STYLE_SETTER(SetExpandedItemSpacing, float)
DALI_TOAST_STYLE_SETTER(SetFontSize, float)
DALI_TOAST_STYLE_SETTER(SetBorderlineWidth, float)
DALI_TOAST_STYLE_SETTER(SetBorderlineOffset, float)
DALI_TOAST_STYLE_SETTER(SetCornerRadius, const Vector4&)
DALI_TOAST_STYLE_SETTER(SetCornerRadiusPolicy, CornerRadiusPolicy)
DALI_TOAST_STYLE_SETTER(SetPadding, const Insets&)
DALI_TOAST_STYLE_SETTER(SetExpandedPadding, const Insets&)
DALI_TOAST_STYLE_SETTER(SetBackgroundColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetTextColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetFontFamily, const Dali::String&)
DALI_TOAST_STYLE_SETTER(SetShadow, const ShadowStack&)
DALI_TOAST_STYLE_SETTER(SetBorderlineColor, const UiColor&)
DALI_TOAST_STYLE_SETTER(SetActionButtonStyle, TextButtonStyle)

#undef DALI_TOAST_STYLE_SETTER

ToastStyle::Builder& ToastStyle::Builder::SetInitialSize(float width, float height) &
{
  SetInitialWidth(width);
  SetInitialHeight(height);
  return *this;
}

ToastStyle::Builder&& ToastStyle::Builder::SetInitialSize(float width, float height) &&
{
  SetInitialSize(width, height);
  return std::move(*this);
}

ToastStyle::Builder& ToastStyle::Builder::SetMinimumSize(float width, float height) &
{
  SetMinimumWidth(width);
  SetMinimumHeight(height);
  return *this;
}

ToastStyle::Builder&& ToastStyle::Builder::SetMinimumSize(float width, float height) &&
{
  SetMinimumSize(width, height);
  return std::move(*this);
}

ToastStyle ToastStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "ToastStyle::Builder has already been consumed");
  DALI_ASSERT_ALWAYS(mImpl->GetMaximumHeight() >= mImpl->GetExpandedHeight() &&
                     "ToastStyle maximum height must be greater than or equal to expanded height");
  DALI_ASSERT_ALWAYS(mImpl->GetExpandedHeight() >= mImpl->GetMinimumHeight() &&
                     "ToastStyle expanded height must be greater than or equal to minimum height");
  DALI_ASSERT_ALWAYS((mImpl->GetMaximumWidth() == 0.0f || mImpl->GetMaximumWidth() >= mImpl->GetMinimumWidth()) &&
                     "ToastStyle maximum width must be zero or greater than or equal to minimum width");
  DALI_ASSERT_ALWAYS(mImpl->GetActionButtonStyle() && "ToastStyle action button style must be valid");

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
: mInitialWidth(10.0f),
  mInitialHeight(10.0f),
  mMinimumWidth(72.0f),
  mMinimumHeight(72.0f),
  mExpandedHeight(141.0f),
  mMaximumHeight(182.0f),
  mMaximumWidthRatio(0.68f),
  mMaximumWidth(0.0f),
  mBottomOffsetRatio(0.05f),
  mBottomOffset(0.0f),
  mCornerRadius(36.0f, 36.0f, 36.0f, 36.0f),
  mCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE),
  mPadding(32.0f, 8.0f, 0.0f, 0.0f),
  mExpandedPadding(32.0f, 32.0f, 16.0f, 16.0f),
  mItemSpacing(20.0f),
  mExpandedItemSpacing(16.0f),
  mBackgroundColor(UiColor("OnSurface")),
  mTextColor(UiColor("OnSurfaceContainerFixed")),
  mFontSize(28.0f),
  mFontFamily("SamsungOneUI400"),
  mShadow(CreateDefaultShadow()),
  mBorderlineWidth(2.0f),
  mBorderlineColor(UiColor(0xFFFFFFu, 0.0f)),
  mBorderlineOffset(-1.0f),
  mActionButtonStyle(CreateDefaultActionButtonStyle())
{
}

ToastStyleImpl::ToastStyleImpl(const ToastStyleImpl& rhs)
: mInitialWidth(rhs.mInitialWidth),
  mInitialHeight(rhs.mInitialHeight),
  mMinimumWidth(rhs.mMinimumWidth),
  mMinimumHeight(rhs.mMinimumHeight),
  mExpandedHeight(rhs.mExpandedHeight),
  mMaximumHeight(rhs.mMaximumHeight),
  mMaximumWidthRatio(rhs.mMaximumWidthRatio),
  mMaximumWidth(rhs.mMaximumWidth),
  mBottomOffsetRatio(rhs.mBottomOffsetRatio),
  mBottomOffset(rhs.mBottomOffset),
  mCornerRadius(rhs.mCornerRadius),
  mCornerRadiusPolicy(rhs.mCornerRadiusPolicy),
  mPadding(rhs.mPadding),
  mExpandedPadding(rhs.mExpandedPadding),
  mItemSpacing(rhs.mItemSpacing),
  mExpandedItemSpacing(rhs.mExpandedItemSpacing),
  mBackgroundColor(rhs.mBackgroundColor),
  mTextColor(rhs.mTextColor),
  mFontSize(rhs.mFontSize),
  mFontFamily(rhs.mFontFamily),
  mShadow(rhs.mShadow),
  mBorderlineWidth(rhs.mBorderlineWidth),
  mBorderlineColor(rhs.mBorderlineColor),
  mBorderlineOffset(rhs.mBorderlineOffset),
  mActionButtonStyle(rhs.mActionButtonStyle)
{
}

ToastStyleImpl::~ToastStyleImpl() = default;

#define DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(Name, Member, Validator, Message) \
  void ToastStyleImpl::Set##Name(float value)                                  \
  {                                                                            \
    DALI_ASSERT_ALWAYS(Validator(value) && Message);                           \
    Member = value;                                                            \
  }                                                                            \
  float ToastStyleImpl::Get##Name() const                                      \
  {                                                                            \
    return Member;                                                             \
  }

DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(InitialWidth, mInitialWidth, IsFinitePositive, "ToastStyle initial width must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(InitialHeight, mInitialHeight, IsFinitePositive, "ToastStyle initial height must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(MinimumWidth, mMinimumWidth, IsFinitePositive, "ToastStyle minimum width must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(MinimumHeight, mMinimumHeight, IsFinitePositive, "ToastStyle minimum height must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(ExpandedHeight, mExpandedHeight, IsFinitePositive, "ToastStyle expanded height must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(MaximumHeight, mMaximumHeight, IsFinitePositive, "ToastStyle maximum height must be finite and positive")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(MaximumWidthRatio, mMaximumWidthRatio, IsFinitePositiveRatio, "ToastStyle maximum width ratio must be finite, positive, and no greater than one")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(MaximumWidth, mMaximumWidth, IsFiniteNonNegative, "ToastStyle maximum width must be finite and non-negative")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(BottomOffsetRatio, mBottomOffsetRatio, IsFiniteRatio, "ToastStyle bottom offset ratio must be finite and between zero and one")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(BottomOffset, mBottomOffset, IsFiniteNonNegative, "ToastStyle bottom offset must be finite and non-negative")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(ItemSpacing, mItemSpacing, IsFiniteNonNegative, "ToastStyle item spacing must be finite and non-negative")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(ExpandedItemSpacing, mExpandedItemSpacing, IsFiniteNonNegative, "ToastStyle expanded item spacing must be finite and non-negative")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(FontSize, mFontSize, IsFiniteNonNegative, "ToastStyle font size must be finite and non-negative")
DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR(BorderlineWidth, mBorderlineWidth, IsFiniteNonNegative, "ToastStyle borderline width must be finite and non-negative")

#undef DALI_TOAST_STYLE_IMPL_FLOAT_ACCESSOR

void ToastStyleImpl::SetBorderlineOffset(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value >= -1.0f && value <= 1.0f &&
                     "ToastStyle borderline offset must be finite and between -1 and 1");
  mBorderlineOffset = value;
}

float ToastStyleImpl::GetBorderlineOffset() const
{
  return mBorderlineOffset;
}

void ToastStyleImpl::SetCornerRadius(const Vector4& radius)
{
  DALI_ASSERT_ALWAYS(IsValidRadius(radius) && "ToastStyle corner radius must be finite and non-negative");
  mCornerRadius = radius;
}

Vector4 ToastStyleImpl::GetCornerRadius() const
{
  return mCornerRadius;
}

void ToastStyleImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mCornerRadiusPolicy = policy;
}

CornerRadiusPolicy ToastStyleImpl::GetCornerRadiusPolicy() const
{
  return mCornerRadiusPolicy;
}

void ToastStyleImpl::SetPadding(const Insets& padding)
{
  DALI_ASSERT_ALWAYS(AreValidInsets(padding) && "ToastStyle padding must be finite and non-negative");
  mPadding = padding;
}

Insets ToastStyleImpl::GetPadding() const
{
  return mPadding;
}

void ToastStyleImpl::SetExpandedPadding(const Insets& padding)
{
  DALI_ASSERT_ALWAYS(AreValidInsets(padding) && "ToastStyle expanded padding must be finite and non-negative");
  mExpandedPadding = padding;
}

Insets ToastStyleImpl::GetExpandedPadding() const
{
  return mExpandedPadding;
}

#define DALI_TOAST_STYLE_IMPL_ACCESSOR(Type, Name, Member) \
  void ToastStyleImpl::Set##Name(Type value)               \
  {                                                        \
    Member = value;                                        \
  }                                                        \
  auto ToastStyleImpl::Get##Name() const->decltype(Member) \
  {                                                        \
    return Member;                                         \
  }

DALI_TOAST_STYLE_IMPL_ACCESSOR(const UiColor&, BackgroundColor, mBackgroundColor)
DALI_TOAST_STYLE_IMPL_ACCESSOR(const UiColor&, TextColor, mTextColor)
DALI_TOAST_STYLE_IMPL_ACCESSOR(const Dali::String&, FontFamily, mFontFamily)
DALI_TOAST_STYLE_IMPL_ACCESSOR(const ShadowStack&, Shadow, mShadow)
DALI_TOAST_STYLE_IMPL_ACCESSOR(const UiColor&, BorderlineColor, mBorderlineColor)

#undef DALI_TOAST_STYLE_IMPL_ACCESSOR

void ToastStyleImpl::SetActionButtonStyle(TextButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "ToastStyle action button style must be valid");
  mActionButtonStyle = style;
}

TextButtonStyle ToastStyleImpl::GetActionButtonStyle() const
{
  return mActionButtonStyle;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

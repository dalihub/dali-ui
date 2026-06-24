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

// CLASS HEADER
#include <dali-ui-components/public-api/text-button-style.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/text-button-style-impl.h>
#include <dali-ui-foundation/public-api/ui-config.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
namespace Ui
{
namespace
{

StateEffect CreateDefaultTextButtonStateEffect()
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle requires UiConfig::Apply()");
  return StateEffect::DefaultForInteractive();
}

} // namespace

UiStyleKey TextButtonStyle::DefaultKey()
{
  static UiStyleKey key = UiStyleKey::Alloc();
  return key;
}

TextButtonStyle TextButtonStyle::Builtin()
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle::Builtin() requires UiConfig::Apply()");

  static TextButtonStyle style = TextButtonStyle::Builder().Build();
  return style;
}

TextButtonStyle TextButtonStyle::Default()
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle::Default() requires UiConfig::Apply()");

  TextButtonStyle style = TextButtonStyle::DownCast(UiConfig::GetCurrent().GetStyle(DefaultKey()));
  if(style)
  {
    return style;
  }
  return Builtin();
}

TextButtonStyle TextButtonStyle::DownCast(BaseHandle handle)
{
  return TextButtonStyle(dynamic_cast<Internal::TextButtonStyleImpl*>(handle.GetObjectPtr()));
}

TextButtonStyle::Builder TextButtonStyle::Configure() const
{
  IntrusivePtr<Internal::TextButtonStyleImpl> impl(new Internal::TextButtonStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

float TextButtonStyle::GetMinimumWidth() const
{
  return GetImpl(*this).GetMinimumWidth();
}

float TextButtonStyle::GetMinimumHeight() const
{
  return GetImpl(*this).GetMinimumHeight();
}

float TextButtonStyle::GetMaximumWidth() const
{
  return GetImpl(*this).GetMaximumWidth();
}

float TextButtonStyle::GetMaximumHeight() const
{
  return GetImpl(*this).GetMaximumHeight();
}

Vector4 TextButtonStyle::GetCornerRadius() const
{
  return GetImpl(*this).GetCornerRadius();
}

CornerRadiusPolicy TextButtonStyle::GetCornerRadiusPolicy() const
{
  return GetImpl(*this).GetCornerRadiusPolicy();
}

Extents TextButtonStyle::GetPadding() const
{
  return GetImpl(*this).GetPadding();
}

UiColor TextButtonStyle::GetBackgroundColor() const
{
  return GetImpl(*this).GetBackgroundColor();
}

LayoutAlignment TextButtonStyle::GetHorizontalAlignment() const
{
  return GetImpl(*this).GetHorizontalAlignment();
}

LayoutAlignment TextButtonStyle::GetVerticalAlignment() const
{
  return GetImpl(*this).GetVerticalAlignment();
}

UiColor TextButtonStyle::GetTextColor() const
{
  return GetImpl(*this).GetTextColor();
}

float TextButtonStyle::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

Dali::String TextButtonStyle::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

StateEffect TextButtonStyle::GetStateEffect() const
{
  return GetImpl(*this).GetStateEffect();
}

bool TextButtonStyle::IsUnderlineEnabled() const
{
  return GetImpl(*this).IsUnderlineEnabled();
}

Text::Underline TextButtonStyle::GetUnderline() const
{
  return GetImpl(*this).GetUnderline();
}

TextButtonStyle::TextButtonStyle(Internal::TextButtonStyleImpl* impl)
: UiStyle(impl)
{
}

TextButtonStyle::Builder::Builder()
: mImpl(new Internal::TextButtonStyleImpl())
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle::Builder requires UiConfig::Apply()");
}

TextButtonStyle::Builder::Builder(Builder&& rhs) noexcept = default;

TextButtonStyle::Builder& TextButtonStyle::Builder::operator=(Builder&& rhs) noexcept = default;

TextButtonStyle::Builder::~Builder() = default;

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMinimumWidth(float width) &
{
  mImpl->SetMinimumWidth(width);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMinimumWidth(float width) &&
{
  SetMinimumWidth(width);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMinimumHeight(float height) &
{
  mImpl->SetMinimumHeight(height);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMinimumHeight(float height) &&
{
  SetMinimumHeight(height);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMaximumWidth(float width) &
{
  mImpl->SetMaximumWidth(width);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMaximumWidth(float width) &&
{
  SetMaximumWidth(width);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMaximumHeight(float height) &
{
  mImpl->SetMaximumHeight(height);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMaximumHeight(float height) &&
{
  SetMaximumHeight(height);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMinimumSize(const Vector2& size) &
{
  mImpl->SetMinimumSize(size);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMinimumSize(const Vector2& size) &&
{
  SetMinimumSize(size);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetMaximumSize(const Vector2& size) &
{
  mImpl->SetMaximumSize(size);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetMaximumSize(const Vector2& size) &&
{
  SetMaximumSize(size);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetCornerRadius(float radius) &
{
  return SetCornerRadius(Vector4(radius, radius, radius, radius));
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetCornerRadius(float radius) &&
{
  SetCornerRadius(radius);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetCornerRadius(const Vector4& radius) &
{
  mImpl->SetCornerRadius(radius);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetCornerRadius(const Vector4& radius) &&
{
  SetCornerRadius(radius);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy policy) &
{
  mImpl->SetCornerRadiusPolicy(policy);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy policy) &&
{
  SetCornerRadiusPolicy(policy);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetPadding(const Extents& padding) &
{
  mImpl->SetPadding(padding);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetPadding(const Extents& padding) &&
{
  SetPadding(padding);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetBackgroundColor(const UiColor& color) &
{
  mImpl->SetBackgroundColor(color);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetBackgroundColor(const UiColor& color) &&
{
  SetBackgroundColor(color);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetHorizontalAlignment(LayoutAlignment alignment) &
{
  mImpl->SetHorizontalAlignment(alignment);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetHorizontalAlignment(LayoutAlignment alignment) &&
{
  SetHorizontalAlignment(alignment);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetVerticalAlignment(LayoutAlignment alignment) &
{
  mImpl->SetVerticalAlignment(alignment);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetVerticalAlignment(LayoutAlignment alignment) &&
{
  SetVerticalAlignment(alignment);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetTextColor(const UiColor& color) &
{
  mImpl->SetTextColor(color);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetTextColor(const UiColor& color) &&
{
  SetTextColor(color);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetFontSize(float fontSize) &
{
  mImpl->SetFontSize(fontSize);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetFontSize(float fontSize) &&
{
  SetFontSize(fontSize);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetFontFamily(const Dali::String& fontFamily) &
{
  mImpl->SetFontFamily(fontFamily);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetFontFamily(const Dali::String& fontFamily) &&
{
  SetFontFamily(fontFamily);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetStateEffect(StateEffect effect) &
{
  mImpl->SetStateEffect(effect);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetStateEffect(StateEffect effect) &&
{
  SetStateEffect(effect);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::SetUnderline(const Text::Underline& underline) &
{
  mImpl->SetUnderline(underline);
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::SetUnderline(const Text::Underline& underline) &&
{
  SetUnderline(underline);
  return std::move(*this);
}

TextButtonStyle::Builder& TextButtonStyle::Builder::ClearUnderline() &
{
  mImpl->ClearUnderline();
  return *this;
}

TextButtonStyle::Builder&& TextButtonStyle::Builder::ClearUnderline() &&
{
  ClearUnderline();
  return std::move(*this);
}

TextButtonStyle TextButtonStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "TextButtonStyle::Builder has already been consumed");
  TextButtonStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

TextButtonStyle::Builder::Builder(Internal::TextButtonStyleImpl* impl)
: mImpl(impl)
{
}

namespace Internal
{

void TextButtonStyleImpl::SetMinimumWidth(float width)
{
  mMinimumWidth = width;
}

float TextButtonStyleImpl::GetMinimumWidth() const
{
  return mMinimumWidth;
}

void TextButtonStyleImpl::SetMinimumHeight(float height)
{
  mMinimumHeight = height;
}

float TextButtonStyleImpl::GetMinimumHeight() const
{
  return mMinimumHeight;
}

void TextButtonStyleImpl::SetMaximumWidth(float width)
{
  mMaximumWidth = width;
}

float TextButtonStyleImpl::GetMaximumWidth() const
{
  return mMaximumWidth;
}

void TextButtonStyleImpl::SetMaximumHeight(float height)
{
  mMaximumHeight = height;
}

float TextButtonStyleImpl::GetMaximumHeight() const
{
  return mMaximumHeight;
}

void TextButtonStyleImpl::SetMinimumSize(const Vector2& size)
{
  mMinimumWidth  = size.width;
  mMinimumHeight = size.height;
}

void TextButtonStyleImpl::SetMaximumSize(const Vector2& size)
{
  mMaximumWidth  = size.width;
  mMaximumHeight = size.height;
}

void TextButtonStyleImpl::SetCornerRadius(const Vector4& radius)
{
  mCornerRadius = radius;
}

Vector4 TextButtonStyleImpl::GetCornerRadius() const
{
  return mCornerRadius;
}

void TextButtonStyleImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mCornerRadiusPolicy = policy;
}

CornerRadiusPolicy TextButtonStyleImpl::GetCornerRadiusPolicy() const
{
  return mCornerRadiusPolicy;
}

void TextButtonStyleImpl::SetPadding(const Extents& padding)
{
  mPadding = padding;
}

Extents TextButtonStyleImpl::GetPadding() const
{
  return mPadding;
}

void TextButtonStyleImpl::SetBackgroundColor(const UiColor& color)
{
  mBackgroundColor = color;
}

UiColor TextButtonStyleImpl::GetBackgroundColor() const
{
  return mBackgroundColor;
}

void TextButtonStyleImpl::SetHorizontalAlignment(LayoutAlignment alignment)
{
  mHorizontalAlignment = alignment;
}

LayoutAlignment TextButtonStyleImpl::GetHorizontalAlignment() const
{
  return mHorizontalAlignment;
}

void TextButtonStyleImpl::SetVerticalAlignment(LayoutAlignment alignment)
{
  mVerticalAlignment = alignment;
}

LayoutAlignment TextButtonStyleImpl::GetVerticalAlignment() const
{
  return mVerticalAlignment;
}

void TextButtonStyleImpl::SetTextColor(const UiColor& color)
{
  mTextColor = color;
}

UiColor TextButtonStyleImpl::GetTextColor() const
{
  return mTextColor;
}

void TextButtonStyleImpl::SetFontSize(float fontSize)
{
  mFontSize = fontSize;
}

float TextButtonStyleImpl::GetFontSize() const
{
  return mFontSize;
}

void TextButtonStyleImpl::SetFontFamily(const Dali::String& fontFamily)
{
  mFontFamily = fontFamily;
}

Dali::String TextButtonStyleImpl::GetFontFamily() const
{
  return mFontFamily;
}

void TextButtonStyleImpl::SetStateEffect(StateEffect effect)
{
  mStateEffect = effect ? effect : StateEffect::None();
}

StateEffect TextButtonStyleImpl::GetStateEffect() const
{
  return mStateEffect;
}

void TextButtonStyleImpl::SetUnderline(const Text::Underline& underline)
{
  mUnderline        = underline;
  mUnderlineEnabled = true;
}

void TextButtonStyleImpl::ClearUnderline()
{
  mUnderlineEnabled = false;
}

bool TextButtonStyleImpl::IsUnderlineEnabled() const
{
  return mUnderlineEnabled;
}

Text::Underline TextButtonStyleImpl::GetUnderline() const
{
  return mUnderline;
}

TextButtonStyleImpl::TextButtonStyleImpl()
: mMinimumWidth(10.0f),
  mMinimumHeight(10.0f),
  mMaximumWidth(UNCONSTRAINED_MAX_SIZE),
  mMaximumHeight(UNCONSTRAINED_MAX_SIZE),
  mCornerRadius(0.5f, 0.5f, 0.5f, 0.5f),
  mCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE),
  mPadding(16u, 16u, 12u, 12u),
  mBackgroundColor(UiColor::PRIMARY),
  mHorizontalAlignment(LayoutAlignment::CENTER),
  mVerticalAlignment(LayoutAlignment::CENTER),
  mTextColor(UiColor::ON_PRIMARY),
  mFontSize(16.0f),
  mStateEffect(CreateDefaultTextButtonStateEffect())
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle requires UiConfig::Apply()");
}

TextButtonStyleImpl::TextButtonStyleImpl(const TextButtonStyleImpl& rhs)
: mMinimumWidth(rhs.mMinimumWidth),
  mMinimumHeight(rhs.mMinimumHeight),
  mMaximumWidth(rhs.mMaximumWidth),
  mMaximumHeight(rhs.mMaximumHeight),
  mCornerRadius(rhs.mCornerRadius),
  mCornerRadiusPolicy(rhs.mCornerRadiusPolicy),
  mPadding(rhs.mPadding),
  mBackgroundColor(rhs.mBackgroundColor),
  mHorizontalAlignment(rhs.mHorizontalAlignment),
  mVerticalAlignment(rhs.mVerticalAlignment),
  mTextColor(rhs.mTextColor),
  mFontSize(rhs.mFontSize),
  mFontFamily(rhs.mFontFamily),
  mStateEffect(rhs.mStateEffect),
  mUnderline(rhs.mUnderline),
  mUnderlineEnabled(rhs.mUnderlineEnabled)
{
}

TextButtonStyleImpl::~TextButtonStyleImpl() = default;

} // namespace Internal

} // namespace Ui
} // namespace Dali

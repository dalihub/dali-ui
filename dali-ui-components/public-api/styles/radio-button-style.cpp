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
#include <dali-ui-components/public-api/styles/radio-button-style.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/component-image-path.h>
#include <dali-ui-components/internal/styles/radio-button-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-color-binding.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace
{
float NormalizeIconDimension(float value)
{
  return std::isfinite(value) && value > 0.0f ? value : 0.0f;
}

StateEffect CreateDefaultRadioButtonStateEffect()
{
  return OverlayEffect::Round();
}

SelectableImageInterface MakeDefaultRadioButtonIcon()
{
  using Binding     = SelectableLottieColorBinding;
  using ColorPolicy = Binding::ColorPolicy;
  using FrameRange  = SelectableLottieImage::FrameRange;

  SelectableLottieColorBindings colorBindings;
  colorBindings.PushBack(Binding("radio_inner.inner_fill.color",
                                 LottieAnimation::VectorProperty::FILL_COLOR,
                                 ColorPolicy::ALWAYS_SELECTED));
  colorBindings.PushBack(Binding("radio_outline.outline_stroke.color",
                                 LottieAnimation::VectorProperty::STROKE_COLOR,
                                 ColorPolicy::SELECTED_IN_FRAME_RANGE,
                                 FrameRange(7, 26)));

  SelectableLottieImage image(Internal::ResolveComponentImageUrl("radio-button.json"),
                              FrameRange(0, 19),
                              FrameRange(20, 37));
  return SelectableLottieAnimationView::New(image, colorBindings);
}
} // unnamed namespace

RadioButtonStyle::RadioButtonStyle() = default;

RadioButtonStyle::~RadioButtonStyle() = default;

RadioButtonStyle::RadioButtonStyle(const RadioButtonStyle& rhs)
: UiStyle(rhs)
{
}

RadioButtonStyle::RadioButtonStyle(RadioButtonStyle&& rhs) noexcept = default;

RadioButtonStyle& RadioButtonStyle::operator=(const RadioButtonStyle& rhs)
{
  UiStyle::operator=(rhs);
  return *this;
}

RadioButtonStyle& RadioButtonStyle::operator=(RadioButtonStyle&& rhs) noexcept = default;

UiStyleKey<RadioButtonStyle> RadioButtonStyle::DefaultKey()
{
  static UiStyleKey<RadioButtonStyle> key = UiStyleKey<RadioButtonStyle>::Alloc();
  return key;
}

RadioButtonStyle RadioButtonStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static RadioButtonStyle style = RadioButtonStyle::Builder().Build();
  return style;
}

RadioButtonStyle RadioButtonStyle::Default()
{
  DebugAssertStyleConfigApplied();
  RadioButtonStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

RadioButtonStyle RadioButtonStyle::DownCast(BaseHandle handle)
{
  return RadioButtonStyle(dynamic_cast<Internal::RadioButtonStyleImpl*>(handle.GetObjectPtr()));
}

RadioButtonStyle RadioButtonStyle::StaticDownCast(UiStyle style)
{
  return RadioButtonStyle(static_cast<Internal::RadioButtonStyleImpl*>(style.GetObjectPtr()));
}

RadioButtonStyle::Builder RadioButtonStyle::Configure() const
{
  IntrusivePtr<Internal::RadioButtonStyleImpl> impl(new Internal::RadioButtonStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

float RadioButtonStyle::GetMinimumWidth() const
{
  return GetImpl(*this).GetMinimumWidth();
}

float RadioButtonStyle::GetMinimumHeight() const
{
  return GetImpl(*this).GetMinimumHeight();
}

Insets RadioButtonStyle::GetPadding() const
{
  return GetImpl(*this).GetPadding();
}

float RadioButtonStyle::GetIconWidth() const
{
  return GetImpl(*this).GetIconWidth();
}

float RadioButtonStyle::GetIconHeight() const
{
  return GetImpl(*this).GetIconHeight();
}

SelectableImageInterface RadioButtonStyle::CreateIcon() const
{
  return GetImpl(*this).CreateIcon();
}

UiColor RadioButtonStyle::GetIconColor() const
{
  return GetImpl(*this).GetIconColor();
}

UiColor RadioButtonStyle::GetSelectedIconColor() const
{
  return GetImpl(*this).GetSelectedIconColor();
}

StateEffect RadioButtonStyle::GetStateEffect() const
{
  return GetImpl(*this).GetStateEffect();
}

RadioButtonStyle::RadioButtonStyle(Internal::RadioButtonStyleImpl* impl)
: UiStyle(impl)
{
}

RadioButtonStyle::Builder::Builder()
: mImpl(new Internal::RadioButtonStyleImpl())
{
}

RadioButtonStyle::Builder::Builder(Builder&& rhs) noexcept = default;

RadioButtonStyle::Builder& RadioButtonStyle::Builder::operator=(Builder&& rhs) noexcept = default;

RadioButtonStyle::Builder::~Builder() = default;

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetMinimumWidth(float width) &
{
  mImpl->SetMinimumWidth(width);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetMinimumWidth(float width) &&
{
  SetMinimumWidth(width);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetMinimumHeight(float height) &
{
  mImpl->SetMinimumHeight(height);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetMinimumHeight(float height) &&
{
  SetMinimumHeight(height);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetPadding(const Insets& padding) &
{
  mImpl->SetPadding(padding);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetPadding(const Insets& padding) &&
{
  SetPadding(padding);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetIconWidth(float width) &
{
  mImpl->SetIconWidth(width);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetIconWidth(float width) &&
{
  SetIconWidth(width);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetIconHeight(float height) &
{
  mImpl->SetIconHeight(height);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetIconHeight(float height) &&
{
  SetIconHeight(height);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetIconGenerator(IconGenerator&& generator) &
{
  mImpl->SetIconGenerator(std::move(generator));
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetIconGenerator(IconGenerator&& generator) &&
{
  SetIconGenerator(std::move(generator));
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetIconColor(const UiColor& color) &
{
  mImpl->SetIconColor(color);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetIconColor(const UiColor& color) &&
{
  SetIconColor(color);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetSelectedIconColor(const UiColor& color) &
{
  mImpl->SetSelectedIconColor(color);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetSelectedIconColor(const UiColor& color) &&
{
  SetSelectedIconColor(color);
  return std::move(*this);
}

RadioButtonStyle::Builder& RadioButtonStyle::Builder::SetStateEffect(StateEffect effect) &
{
  mImpl->SetStateEffect(effect);
  return *this;
}

RadioButtonStyle::Builder&& RadioButtonStyle::Builder::SetStateEffect(StateEffect effect) &&
{
  SetStateEffect(effect);
  return std::move(*this);
}

RadioButtonStyle RadioButtonStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "RadioButtonStyle::Builder has already been consumed");
  RadioButtonStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

RadioButtonStyle::Builder::Builder(Internal::RadioButtonStyleImpl* impl)
: mImpl(impl)
{
}

namespace Internal
{
RadioButtonStyleImpl::RadioButtonStyleImpl()
: mMinimumWidth(0.0f),
  mMinimumHeight(0.0f),
  mPadding(8u, 8u, 8u, 8u),
  mIconWidth(36.0f),
  mIconHeight(36.0f),
  mIconGenerator(std::make_shared<RadioButtonStyle::IconGenerator>(
    RadioButtonStyle::IconGenerator::New(&MakeDefaultRadioButtonIcon))),
  mDeselectedIconColor(UiColor::OUTLINE),
  mSelectedIconColor(UiColor::PRIMARY),
  mStateEffect(CreateDefaultRadioButtonStateEffect())
{
}

RadioButtonStyleImpl::RadioButtonStyleImpl(const RadioButtonStyleImpl& rhs)
: mMinimumWidth(rhs.mMinimumWidth),
  mMinimumHeight(rhs.mMinimumHeight),
  mPadding(rhs.mPadding),
  mIconWidth(rhs.mIconWidth),
  mIconHeight(rhs.mIconHeight),
  mIconGenerator(rhs.mIconGenerator),
  mDeselectedIconColor(rhs.mDeselectedIconColor),
  mSelectedIconColor(rhs.mSelectedIconColor),
  mStateEffect(rhs.mStateEffect)
{
}

RadioButtonStyleImpl::~RadioButtonStyleImpl() = default;

void RadioButtonStyleImpl::SetMinimumWidth(float width)
{
  mMinimumWidth = width;
}

float RadioButtonStyleImpl::GetMinimumWidth() const
{
  return mMinimumWidth;
}

void RadioButtonStyleImpl::SetMinimumHeight(float height)
{
  mMinimumHeight = height;
}

float RadioButtonStyleImpl::GetMinimumHeight() const
{
  return mMinimumHeight;
}

void RadioButtonStyleImpl::SetPadding(const Insets& padding)
{
  mPadding = padding;
}

Insets RadioButtonStyleImpl::GetPadding() const
{
  return mPadding;
}

void RadioButtonStyleImpl::SetIconWidth(float width)
{
  mIconWidth = NormalizeIconDimension(width);
}

float RadioButtonStyleImpl::GetIconWidth() const
{
  return mIconWidth;
}

void RadioButtonStyleImpl::SetIconHeight(float height)
{
  mIconHeight = NormalizeIconDimension(height);
}

float RadioButtonStyleImpl::GetIconHeight() const
{
  return mIconHeight;
}

void RadioButtonStyleImpl::SetIconColor(const UiColor& color)
{
  mDeselectedIconColor = color;
}

UiColor RadioButtonStyleImpl::GetIconColor() const
{
  return mDeselectedIconColor;
}

void RadioButtonStyleImpl::SetSelectedIconColor(const UiColor& color)
{
  mSelectedIconColor = color;
}

UiColor RadioButtonStyleImpl::GetSelectedIconColor() const
{
  return mSelectedIconColor;
}

void RadioButtonStyleImpl::SetStateEffect(StateEffect effect)
{
  mStateEffect = effect ? effect : StateEffect::None();
}

StateEffect RadioButtonStyleImpl::GetStateEffect() const
{
  return mStateEffect;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

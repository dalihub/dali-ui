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
#include <dali-ui-components/public-api/styles/check-box-style.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/styles/check-box-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/integration-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>

// EXTERNAL INCLUDES
#include <string>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace
{
// Circular dim overlay on press/focus (matches the reference feedback shape).
StateEffect CreateDefaultCheckBoxStateEffect()
{
  return OverlayEffect::Round();
}
// The Lottie asset ships in dali-ui-components/images/ and installs to
// <dali image dir>/components/checkbox.json; resolve that path at runtime.
Dali::String DefaultIconUrl()
{
  const std::string path = Dali::Ui::Integration::AssetManager::GetDaliImagePath() + "components/checkbox.json";
  return Dali::String(path.c_str());
}
// The default icon generator: builds a selectable Lottie image from the shipped checkbox.json
// with the segment layout from its markers — select plays [0,19] (the "on" marker), deselect
// plays [20,38] (the "off" marker). The inner-fill key path targets the recoloured fill layer
// ("check_box " has a trailing space, matching the asset layer name). Must be a free function
// (IconGenerator = Ui::Callback<ISelectableImage()>).
ISelectableImage MakeDefaultCheckBoxIcon()
{
  return SelectableLottieAnimationView::New(SelectableLottieImage(DefaultIconUrl(),
                                                                  SelectableLottieImage::FrameRange(0, 19),
                                                                  SelectableLottieImage::FrameRange(20, 38),
                                                                  "check_box .inner_fill.color"));
}
} // namespace

UiStyleKey<CheckBoxStyle> CheckBoxStyle::DefaultKey()
{
  static UiStyleKey<CheckBoxStyle> key = UiStyleKey<CheckBoxStyle>::Alloc();
  return key;
}

CheckBoxStyle CheckBoxStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static CheckBoxStyle style = CheckBoxStyle::Builder().Build();
  return style;
}

CheckBoxStyle CheckBoxStyle::Default()
{
  DebugAssertStyleConfigApplied();
  CheckBoxStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  if(style)
  {
    return style;
  }
  return DefaultPreset();
}

CheckBoxStyle CheckBoxStyle::DownCast(BaseHandle handle)
{
  return CheckBoxStyle(dynamic_cast<Internal::CheckBoxStyleImpl*>(handle.GetObjectPtr()));
}

CheckBoxStyle CheckBoxStyle::StaticDownCast(UiStyle style)
{
  return CheckBoxStyle(static_cast<Internal::CheckBoxStyleImpl*>(style.GetObjectPtr()));
}

CheckBoxStyle::Builder CheckBoxStyle::Configure() const
{
  IntrusivePtr<Internal::CheckBoxStyleImpl> impl(new Internal::CheckBoxStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

float CheckBoxStyle::GetMinimumWidth() const
{
  return GetImpl(*this).GetMinimumWidth();
}
float CheckBoxStyle::GetMinimumHeight() const
{
  return GetImpl(*this).GetMinimumHeight();
}
Extents CheckBoxStyle::GetPadding() const
{
  return GetImpl(*this).GetPadding();
}
float CheckBoxStyle::GetBoxSize() const
{
  return GetImpl(*this).GetBoxSize();
}
float CheckBoxStyle::GetLabelGap() const
{
  return GetImpl(*this).GetLabelGap();
}
ISelectableImage CheckBoxStyle::CreateIcon() const
{
  return GetImpl(*this).CreateIcon();
}
UiColor CheckBoxStyle::GetIconColor() const
{
  return GetImpl(*this).GetIconColor();
}
UiColor CheckBoxStyle::GetSelectedIconColor() const
{
  return GetImpl(*this).GetSelectedIconColor();
}
UiColor CheckBoxStyle::GetLabelColor() const
{
  return GetImpl(*this).GetLabelColor();
}
StateEffect CheckBoxStyle::GetStateEffect() const
{
  return GetImpl(*this).GetStateEffect();
}

CheckBoxStyle::CheckBoxStyle(Internal::CheckBoxStyleImpl* impl)
: UiStyle(impl)
{
}

CheckBoxStyle::Builder::Builder()
: mImpl(new Internal::CheckBoxStyleImpl())
{
}
CheckBoxStyle::Builder::Builder(Builder&& rhs) noexcept                           = default;
CheckBoxStyle::Builder& CheckBoxStyle::Builder::operator=(Builder&& rhs) noexcept = default;
CheckBoxStyle::Builder::~Builder()                                                = default;

// --- lvalue setters mutate mImpl; rvalue setters delegate then std::move ---
#define CBS_SETTER(Name, Type)                                     \
  CheckBoxStyle::Builder& CheckBoxStyle::Builder::Name(Type v) &   \
  {                                                                \
    mImpl->Name(v);                                                \
    return *this;                                                  \
  }                                                                \
  CheckBoxStyle::Builder&& CheckBoxStyle::Builder::Name(Type v) && \
  {                                                                \
    Name(v);                                                       \
    return std::move(*this);                                       \
  }

CBS_SETTER(SetMinimumWidth, float)
CBS_SETTER(SetMinimumHeight, float)
CBS_SETTER(SetMinimumSize, const Vector2&)
CBS_SETTER(SetPadding, const Extents&)
CBS_SETTER(SetBoxSize, float)
CBS_SETTER(SetLabelGap, float)
CBS_SETTER(SetIconColor, const UiColor&)
CBS_SETTER(SetSelectedIconColor, const UiColor&)
CBS_SETTER(SetLabelColor, const UiColor&)
CBS_SETTER(SetStateEffect, StateEffect)
#undef CBS_SETTER

// SetIconGenerator cannot use the by-value CBS_SETTER macro: IconGenerator (Ui::Callback) is
// move-only, so it must be moved into the impl.
CheckBoxStyle::Builder& CheckBoxStyle::Builder::SetIconGenerator(IconGenerator&& generator) &
{
  mImpl->SetIconGenerator(std::move(generator));
  return *this;
}
CheckBoxStyle::Builder&& CheckBoxStyle::Builder::SetIconGenerator(IconGenerator&& generator) &&
{
  SetIconGenerator(std::move(generator));
  return std::move(*this);
}

CheckBoxStyle CheckBoxStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "CheckBoxStyle::Builder has already been consumed");
  CheckBoxStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

CheckBoxStyle::Builder::Builder(Internal::CheckBoxStyleImpl* impl)
: mImpl(impl)
{
}

namespace Internal
{
// Trivial member get/set forwarders.
void CheckBoxStyleImpl::SetMinimumWidth(float v)
{
  mMinimumWidth = v;
}
float CheckBoxStyleImpl::GetMinimumWidth() const
{
  return mMinimumWidth;
}
void CheckBoxStyleImpl::SetMinimumHeight(float v)
{
  mMinimumHeight = v;
}
float CheckBoxStyleImpl::GetMinimumHeight() const
{
  return mMinimumHeight;
}
void CheckBoxStyleImpl::SetMinimumSize(const Vector2& size)
{
  mMinimumWidth  = size.width;
  mMinimumHeight = size.height;
}
void CheckBoxStyleImpl::SetPadding(const Extents& v)
{
  mPadding = v;
}
Extents CheckBoxStyleImpl::GetPadding() const
{
  return mPadding;
}
void CheckBoxStyleImpl::SetBoxSize(float v)
{
  mBoxSize = v;
}
float CheckBoxStyleImpl::GetBoxSize() const
{
  return mBoxSize;
}
void CheckBoxStyleImpl::SetLabelGap(float v)
{
  mLabelGap = v;
}
float CheckBoxStyleImpl::GetLabelGap() const
{
  return mLabelGap;
}
void CheckBoxStyleImpl::SetIconColor(const UiColor& v)
{
  mIconColor = v;
}
UiColor CheckBoxStyleImpl::GetIconColor() const
{
  return mIconColor;
}
void CheckBoxStyleImpl::SetSelectedIconColor(const UiColor& v)
{
  mSelectedIconColor = v;
}
UiColor CheckBoxStyleImpl::GetSelectedIconColor() const
{
  return mSelectedIconColor;
}
void CheckBoxStyleImpl::SetLabelColor(const UiColor& v)
{
  mLabelColor = v;
}
UiColor CheckBoxStyleImpl::GetLabelColor() const
{
  return mLabelColor;
}
void CheckBoxStyleImpl::SetStateEffect(StateEffect v)
{
  mStateEffect = v ? v : StateEffect::None();
}
StateEffect CheckBoxStyleImpl::GetStateEffect() const
{
  return mStateEffect;
}

CheckBoxStyleImpl::CheckBoxStyleImpl()
: mMinimumWidth(0.0f),  // no default: the app sizes the control
  mMinimumHeight(0.0f), // no default: the app sizes the control
  mPadding(8u, 8u, 8u, 8u),
  mBoxSize(36.0f), // default checkbox glyph size
  mLabelGap(8.0f), // box<->label gap
  mIconGenerator(std::make_shared<CheckBoxStyle::IconGenerator>(
    CheckBoxStyle::IconGenerator::New(&MakeDefaultCheckBoxIcon))), // checkbox.json + [0,19]/[20,38]
  mIconColor(UiColor::OUTLINE),                                    // deselected inner-fill token
  mSelectedIconColor(UiColor::PRIMARY),                            // control-activated accent
  mLabelColor(UiColor::ON_SURFACE),
  mStateEffect(CreateDefaultCheckBoxStateEffect())
{
}

CheckBoxStyleImpl::CheckBoxStyleImpl(const CheckBoxStyleImpl& rhs)
: mMinimumWidth(rhs.mMinimumWidth),
  mMinimumHeight(rhs.mMinimumHeight),
  mPadding(rhs.mPadding),
  mBoxSize(rhs.mBoxSize),
  mLabelGap(rhs.mLabelGap),
  mIconGenerator(rhs.mIconGenerator), // shares the (stateless free-function) generator
  mIconColor(rhs.mIconColor),
  mSelectedIconColor(rhs.mSelectedIconColor),
  mLabelColor(rhs.mLabelColor),
  mStateEffect(rhs.mStateEffect)
{
}

CheckBoxStyleImpl::~CheckBoxStyleImpl() = default;

} // namespace Internal
} // namespace Ui
} // namespace Dali

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
#include <dali-ui-components/public-api/styles/switch-style.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/styles/switch-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>

namespace Dali
{
namespace Ui
{
namespace
{

OverlayEffect CreateDefaultSwitchOverlayEffect()
{
  // The 32x32 thumb container receives a 10px-expanded overlay while its
  // visible child receives recoil. This maps OneUI's primary and secondary
  // touch-effect targets onto Dali-UI's OverlayEffect model.
  return OverlayEffect::Round()
    .Configure()
    .SetOverlayPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f))
    .SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN)
    .Build();
}

void AssertFiniteNonNegative(float value, const char* message)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value >= 0.0f && message);
}

void AssertFinitePositive(float value, const char* message)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value > 0.0f && message);
}

void AssertValidInsets(const Insets& value, const char* message)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value.start) && value.start >= 0.0f &&
                     std::isfinite(value.end) && value.end >= 0.0f &&
                     std::isfinite(value.top) && value.top >= 0.0f &&
                     std::isfinite(value.bottom) && value.bottom >= 0.0f && message);
}

} // namespace

UiStyleKey<SwitchStyle> SwitchStyle::DefaultKey()
{
  static UiStyleKey<SwitchStyle> key = UiStyleKey<SwitchStyle>::Alloc();
  return key;
}

SwitchStyle SwitchStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static SwitchStyle style = SwitchStyle::Builder().Build();
  return style;
}

SwitchStyle SwitchStyle::Default()
{
  DebugAssertStyleConfigApplied();
  SwitchStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

SwitchStyle SwitchStyle::DownCast(BaseHandle handle)
{
  return SwitchStyle(dynamic_cast<Internal::SwitchStyleImpl*>(handle.GetObjectPtr()));
}

SwitchStyle SwitchStyle::StaticDownCast(UiStyle style)
{
  return SwitchStyle(static_cast<Internal::SwitchStyleImpl*>(style.GetObjectPtr()));
}

SwitchStyle::Builder SwitchStyle::Configure() const
{
  IntrusivePtr<Internal::SwitchStyleImpl> impl(new Internal::SwitchStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

#define DALI_SWITCH_STYLE_GETTER(Type, Name) \
  Type SwitchStyle::Get##Name() const        \
  {                                          \
    return GetImpl(*this).Get##Name();       \
  }

DALI_SWITCH_STYLE_GETTER(float, MinimumWidth)
DALI_SWITCH_STYLE_GETTER(float, MinimumHeight)
DALI_SWITCH_STYLE_GETTER(Insets, Padding)
DALI_SWITCH_STYLE_GETTER(float, TrackWidth)
DALI_SWITCH_STYLE_GETTER(float, TrackHeight)
DALI_SWITCH_STYLE_GETTER(Insets, ThumbPadding)
DALI_SWITCH_STYLE_GETTER(UiColor, TrackColor)
DALI_SWITCH_STYLE_GETTER(UiColor, SelectedTrackColor)
DALI_SWITCH_STYLE_GETTER(UiColor, ThumbColor)
DALI_SWITCH_STYLE_GETTER(OverlayEffect, OverlayEffect)
DALI_SWITCH_STYLE_GETTER(float, DragThreshold)

#undef DALI_SWITCH_STYLE_GETTER

SwitchStyle::SwitchStyle(Internal::SwitchStyleImpl* impl)
: UiStyle(impl)
{
}

SwitchStyle::Builder::Builder()
: mImpl(new Internal::SwitchStyleImpl())
{
}

SwitchStyle::Builder::Builder(Builder&& rhs) noexcept                         = default;
SwitchStyle::Builder& SwitchStyle::Builder::operator=(Builder&& rhs) noexcept = default;
SwitchStyle::Builder::~Builder()                                              = default;

#define DALI_SWITCH_STYLE_SETTER(Name, Type)                   \
  SwitchStyle::Builder& SwitchStyle::Builder::Name(Type v) &   \
  {                                                            \
    mImpl->Name(v);                                            \
    return *this;                                              \
  }                                                            \
  SwitchStyle::Builder&& SwitchStyle::Builder::Name(Type v) && \
  {                                                            \
    Name(v);                                                   \
    return std::move(*this);                                   \
  }

DALI_SWITCH_STYLE_SETTER(SetMinimumWidth, float)
DALI_SWITCH_STYLE_SETTER(SetMinimumHeight, float)
DALI_SWITCH_STYLE_SETTER(SetTrackWidth, float)
DALI_SWITCH_STYLE_SETTER(SetTrackHeight, float)
DALI_SWITCH_STYLE_SETTER(SetTrackColor, const UiColor&)
DALI_SWITCH_STYLE_SETTER(SetSelectedTrackColor, const UiColor&)
DALI_SWITCH_STYLE_SETTER(SetThumbColor, const UiColor&)
DALI_SWITCH_STYLE_SETTER(SetOverlayEffect, OverlayEffect)
DALI_SWITCH_STYLE_SETTER(SetDragThreshold, float)

#undef DALI_SWITCH_STYLE_SETTER

#define DALI_SWITCH_STYLE_INSETS_SETTER(Name, Message)                  \
  SwitchStyle::Builder& SwitchStyle::Builder::Name(const Insets& v) &   \
  {                                                                     \
    AssertValidInsets(v, Message);                                      \
    mImpl->Name(v);                                                     \
    return *this;                                                       \
  }                                                                     \
  SwitchStyle::Builder&& SwitchStyle::Builder::Name(const Insets& v) && \
  {                                                                     \
    Name(v);                                                            \
    return std::move(*this);                                            \
  }

DALI_SWITCH_STYLE_INSETS_SETTER(SetPadding, "SwitchStyle padding must be finite and non-negative")
DALI_SWITCH_STYLE_INSETS_SETTER(SetThumbPadding, "SwitchStyle thumb padding must be finite and non-negative")

#undef DALI_SWITCH_STYLE_INSETS_SETTER

SwitchStyle::Builder& SwitchStyle::Builder::SetTrackSize(float width, float height) &
{
  SetTrackWidth(width);
  SetTrackHeight(height);
  return *this;
}

SwitchStyle::Builder&& SwitchStyle::Builder::SetTrackSize(float width, float height) &&
{
  SetTrackSize(width, height);
  return std::move(*this);
}

SwitchStyle::Builder& SwitchStyle::Builder::SetTrackColors(const UiColor& color, const UiColor& selectedColor) &
{
  SetTrackColor(color);
  SetSelectedTrackColor(selectedColor);
  return *this;
}

SwitchStyle::Builder&& SwitchStyle::Builder::SetTrackColors(const UiColor& color, const UiColor& selectedColor) &&
{
  SetTrackColors(color, selectedColor);
  return std::move(*this);
}

SwitchStyle SwitchStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "SwitchStyle::Builder has already been consumed");

  const float  trackWidth   = mImpl->GetTrackWidth();
  const float  trackHeight  = mImpl->GetTrackHeight();
  const Insets thumbPadding = mImpl->GetThumbPadding();

  DALI_ASSERT_ALWAYS(trackWidth >= trackHeight && "SwitchStyle track width must be greater than or equal to track height");
  DALI_ASSERT_ALWAYS(thumbPadding.start + thumbPadding.end < trackHeight &&
                     thumbPadding.top + thumbPadding.bottom < trackHeight &&
                     "SwitchStyle thumb padding must leave a positive thumb size");

  SwitchStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

SwitchStyle::Builder::Builder(Internal::SwitchStyleImpl* impl)
: mImpl(impl)
{
}

namespace Internal
{

#define DALI_SWITCH_STYLE_IMPL_ACCESSOR(Type, Name, Member) \
  void SwitchStyleImpl::Set##Name(Type value)               \
  {                                                         \
    Member = value;                                         \
  }                                                         \
  auto SwitchStyleImpl::Get##Name() const->decltype(Member) \
  {                                                         \
    return Member;                                          \
  }

DALI_SWITCH_STYLE_IMPL_ACCESSOR(const Insets&, Padding, mPadding)
DALI_SWITCH_STYLE_IMPL_ACCESSOR(const Insets&, ThumbPadding, mThumbPadding)
DALI_SWITCH_STYLE_IMPL_ACCESSOR(const UiColor&, TrackColor, mTrackColor)
DALI_SWITCH_STYLE_IMPL_ACCESSOR(const UiColor&, SelectedTrackColor, mSelectedTrackColor)
DALI_SWITCH_STYLE_IMPL_ACCESSOR(const UiColor&, ThumbColor, mThumbColor)

#undef DALI_SWITCH_STYLE_IMPL_ACCESSOR

void SwitchStyleImpl::SetOverlayEffect(OverlayEffect effect)
{
  mOverlayEffect = effect;
}

OverlayEffect SwitchStyleImpl::GetOverlayEffect() const
{
  return mOverlayEffect;
}

SwitchStyleImpl::SwitchStyleImpl()
: mMinimumWidth(0.0f),
  mMinimumHeight(0.0f),
  mPadding(10.0f, 10.0f, 10.0f, 10.0f),
  mTrackWidth(56.0f),
  mTrackHeight(32.0f),
  mThumbPadding(3.25f, 3.25f, 3.25f, 3.25f),
  mTrackColor(UiColor("SurfaceContainerHigher")),
  mSelectedTrackColor(UiColor::PRIMARY),
  mThumbColor(UiColor("OnSurfaceContainerFixed")),
  mOverlayEffect(CreateDefaultSwitchOverlayEffect()),
  mDragThreshold(5.0f)
{
}

SwitchStyleImpl::SwitchStyleImpl(const SwitchStyleImpl& rhs)
: mMinimumWidth(rhs.mMinimumWidth),
  mMinimumHeight(rhs.mMinimumHeight),
  mPadding(rhs.mPadding),
  mTrackWidth(rhs.mTrackWidth),
  mTrackHeight(rhs.mTrackHeight),
  mThumbPadding(rhs.mThumbPadding),
  mTrackColor(rhs.mTrackColor),
  mSelectedTrackColor(rhs.mSelectedTrackColor),
  mThumbColor(rhs.mThumbColor),
  mOverlayEffect(rhs.mOverlayEffect),
  mDragThreshold(rhs.mDragThreshold)
{
}

SwitchStyleImpl::~SwitchStyleImpl() = default;

void SwitchStyleImpl::SetMinimumWidth(float width)
{
  AssertFiniteNonNegative(width, "SwitchStyle minimum width must be finite and non-negative");
  mMinimumWidth = width;
}

float SwitchStyleImpl::GetMinimumWidth() const
{
  return mMinimumWidth;
}

void SwitchStyleImpl::SetMinimumHeight(float height)
{
  AssertFiniteNonNegative(height, "SwitchStyle minimum height must be finite and non-negative");
  mMinimumHeight = height;
}

float SwitchStyleImpl::GetMinimumHeight() const
{
  return mMinimumHeight;
}

void SwitchStyleImpl::SetTrackWidth(float width)
{
  AssertFinitePositive(width, "SwitchStyle track width must be finite and positive");
  mTrackWidth = width;
}

float SwitchStyleImpl::GetTrackWidth() const
{
  return mTrackWidth;
}

void SwitchStyleImpl::SetTrackHeight(float height)
{
  AssertFinitePositive(height, "SwitchStyle track height must be finite and positive");
  mTrackHeight = height;
}

float SwitchStyleImpl::GetTrackHeight() const
{
  return mTrackHeight;
}

void SwitchStyleImpl::SetDragThreshold(float pixels)
{
  AssertFiniteNonNegative(pixels, "SwitchStyle drag threshold must be finite and non-negative");
  mDragThreshold = pixels;
}

float SwitchStyleImpl::GetDragThreshold() const
{
  return mDragThreshold;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

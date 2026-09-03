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

#include <dali-ui-components/public-api/styles/progress-bar-style.h>

#include <cmath>
#include <utility>

#include <dali-ui-components/internal/styles/progress-bar-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

namespace Dali
{
namespace Ui
{
namespace
{

void AssertValidInsets(const Insets& value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value.start) && value.start >= 0.0f &&
                     std::isfinite(value.end) && value.end >= 0.0f &&
                     std::isfinite(value.top) && value.top >= 0.0f &&
                     std::isfinite(value.bottom) && value.bottom >= 0.0f &&
                     "ProgressBarStyle padding must be finite and non-negative");
}

void AssertValidThickness(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value > 0.0f &&
                     "ProgressBarStyle track thickness must be finite and positive");
}

Internal::ProgressBarStyleImpl& GetStyleImpl(UiStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "ProgressBarStyle::Builder has already been consumed");
  return static_cast<Internal::ProgressBarStyleImpl&>(Extension::GetImpl(style));
}

const Internal::ProgressBarStyleImpl& GetStyleImpl(const ProgressBarStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "ProgressBarStyle must be initialized");
  return static_cast<const Internal::ProgressBarStyleImpl&>(Extension::GetImpl(static_cast<const UiStyle&>(style)));
}

} // namespace

UiStyleKey<ProgressBarStyle> ProgressBarStyle::DefaultKey()
{
  static UiStyleKey<ProgressBarStyle> key = UiStyleKey<ProgressBarStyle>::Alloc();
  return key;
}

ProgressBarStyle ProgressBarStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static ProgressBarStyle style = ProgressBarStyle::Builder().Build();
  return style;
}

ProgressBarStyle ProgressBarStyle::Default()
{
  DebugAssertStyleConfigApplied();
  ProgressBarStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

ProgressBarStyle ProgressBarStyle::DownCast(BaseHandle handle)
{
  return ProgressBarStyle(dynamic_cast<Internal::ProgressBarStyleImpl*>(handle.GetObjectPtr()));
}

ProgressBarStyle ProgressBarStyle::StaticDownCast(UiStyle style)
{
  return ProgressBarStyle(static_cast<Internal::ProgressBarStyleImpl*>(style.GetObjectPtr()));
}

ProgressBarStyle::Builder ProgressBarStyle::Configure() const
{
  IntrusivePtr<Internal::ProgressBarStyleImpl> impl(new Internal::ProgressBarStyleImpl(GetStyleImpl(*this)));
  return Builder(ProgressBarStyle(impl.Get()));
}

Insets ProgressBarStyle::GetPadding() const
{
  return GetStyleImpl(*this).GetPadding();
}

float ProgressBarStyle::GetTrackThickness() const
{
  return GetStyleImpl(*this).GetTrackThickness();
}

UiColor ProgressBarStyle::GetTrackColor() const
{
  return GetStyleImpl(*this).GetTrackColor();
}

UiColor ProgressBarStyle::GetTrailColor() const
{
  return GetStyleImpl(*this).GetTrailColor();
}

UiColor ProgressBarStyle::GetDividerColor() const
{
  return GetStyleImpl(*this).GetDividerColor();
}

ProgressBarStyle::ProgressBarStyle(Extension::UiStyleImpl* impl)
: UiStyle(impl)
{
}

ProgressBarStyle::Builder::Builder()
: mStyle(ProgressBarStyle(new Internal::ProgressBarStyleImpl()))
{
}

ProgressBarStyle::Builder::Builder(Builder&& rhs) noexcept = default;

ProgressBarStyle::Builder& ProgressBarStyle::Builder::operator=(Builder&& rhs) noexcept = default;

ProgressBarStyle::Builder::~Builder() = default;

ProgressBarStyle::Builder& ProgressBarStyle::Builder::SetPadding(const Insets& padding) &
{
  AssertValidInsets(padding);
  GetStyleImpl(mStyle).SetPadding(padding);
  return *this;
}

ProgressBarStyle::Builder&& ProgressBarStyle::Builder::SetPadding(const Insets& padding) &&
{
  SetPadding(padding);
  return std::move(*this);
}

ProgressBarStyle::Builder& ProgressBarStyle::Builder::SetTrackThickness(float thickness) &
{
  AssertValidThickness(thickness);
  GetStyleImpl(mStyle).SetTrackThickness(thickness);
  return *this;
}

ProgressBarStyle::Builder&& ProgressBarStyle::Builder::SetTrackThickness(float thickness) &&
{
  SetTrackThickness(thickness);
  return std::move(*this);
}

ProgressBarStyle::Builder& ProgressBarStyle::Builder::SetTrackColor(const UiColor& color) &
{
  GetStyleImpl(mStyle).SetTrackColor(color);
  return *this;
}

ProgressBarStyle::Builder&& ProgressBarStyle::Builder::SetTrackColor(const UiColor& color) &&
{
  SetTrackColor(color);
  return std::move(*this);
}

ProgressBarStyle::Builder& ProgressBarStyle::Builder::SetTrailColor(const UiColor& color) &
{
  GetStyleImpl(mStyle).SetTrailColor(color);
  return *this;
}

ProgressBarStyle::Builder&& ProgressBarStyle::Builder::SetTrailColor(const UiColor& color) &&
{
  SetTrailColor(color);
  return std::move(*this);
}

ProgressBarStyle::Builder& ProgressBarStyle::Builder::SetDividerColor(const UiColor& color) &
{
  GetStyleImpl(mStyle).SetDividerColor(color);
  return *this;
}

ProgressBarStyle::Builder&& ProgressBarStyle::Builder::SetDividerColor(const UiColor& color) &&
{
  SetDividerColor(color);
  return std::move(*this);
}

ProgressBarStyle ProgressBarStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mStyle && "ProgressBarStyle::Builder has already been consumed");
  ProgressBarStyle style = ProgressBarStyle::StaticDownCast(mStyle);
  mStyle.Reset();
  return style;
}

ProgressBarStyle::Builder::Builder(UiStyle style)
: mStyle(std::move(style))
{
}

namespace Internal
{

ProgressBarStyleImpl::ProgressBarStyleImpl()
: mPadding(0.0f, 0.0f, 10.0f, 10.0f),
  mTrackColor(0x1C1C1Eu, 0.1f),
  mTrailColor(UiColor::PRIMARY),
  mDividerColor(0xFFFFFFu, 0.6f)
{
}

ProgressBarStyleImpl::ProgressBarStyleImpl(const ProgressBarStyleImpl& rhs)
: mPadding(rhs.mPadding),
  mTrackThickness(rhs.mTrackThickness),
  mTrackColor(rhs.mTrackColor),
  mTrailColor(rhs.mTrailColor),
  mDividerColor(rhs.mDividerColor)
{
}

ProgressBarStyleImpl::~ProgressBarStyleImpl() = default;

void ProgressBarStyleImpl::SetPadding(const Insets& padding)
{
  mPadding = padding;
}

Insets ProgressBarStyleImpl::GetPadding() const
{
  return mPadding;
}

void ProgressBarStyleImpl::SetTrackThickness(float thickness)
{
  mTrackThickness = thickness;
}

float ProgressBarStyleImpl::GetTrackThickness() const
{
  return mTrackThickness;
}

void ProgressBarStyleImpl::SetTrackColor(const UiColor& color)
{
  mTrackColor = color;
}

UiColor ProgressBarStyleImpl::GetTrackColor() const
{
  return mTrackColor;
}

void ProgressBarStyleImpl::SetTrailColor(const UiColor& color)
{
  mTrailColor = color;
}

UiColor ProgressBarStyleImpl::GetTrailColor() const
{
  return mTrailColor;
}

void ProgressBarStyleImpl::SetDividerColor(const UiColor& color)
{
  mDividerColor = color;
}

UiColor ProgressBarStyleImpl::GetDividerColor() const
{
  return mDividerColor;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

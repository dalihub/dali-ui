
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
#include <dali-ui-components/public-api/styles/chart-view-style.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

#define DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from or consumed ChartViewStyle::Builder")

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace
{
/**
 * @brief Stores authored construction colors and spacing, including unresolved theme identifiers.
 */
class ChartViewStyleImpl : public Extension::UiStyleImpl
{
public:
  ChartViewStyleImpl() = default;

  ChartViewStyleImpl(const ChartViewStyleImpl& rhs)
  : mBackgroundColor(rhs.mBackgroundColor),
    mLabelColor(rhs.mLabelColor),
    mLegendTextColor(rhs.mLegendTextColor),
    mTooltipBackgroundColor(rhs.mTooltipBackgroundColor),
    mTooltipTextColor(rhs.mTooltipTextColor),
    mPadding(rhs.mPadding),
    mTooltipPadding(rhs.mTooltipPadding)
  {
  }

  ChartViewStyleImpl& operator=(const ChartViewStyleImpl&) = delete;
  ChartViewStyleImpl(ChartViewStyleImpl&&)                 = delete;
  ChartViewStyleImpl& operator=(ChartViewStyleImpl&&)      = delete;
  ~ChartViewStyleImpl() override                           = default;
  UiColor mBackgroundColor{UiColor(1.0f, 1.0f, 1.0f, 1.0f)};
  UiColor mLabelColor{UiColor(0.0f, 0.0f, 0.0f, 1.0f)};
  UiColor mLegendTextColor{UiColor(0.0f, 0.0f, 0.0f, 1.0f)};
  UiColor mTooltipBackgroundColor{UiColor(1.0f, 1.0f, 1.0f, 1.0f)};
  UiColor mTooltipTextColor{UiColor(0.1f, 0.1f, 0.1f, 1.0f)};
  float   mPadding{10.0f};
  float   mTooltipPadding{8.0f};
};

ChartViewStyleImpl& GetStyleImpl(UiStyle& style)
{
  DALI_ASSERT_ALWAYS(style && "ChartViewStyle builder has been consumed");
  return static_cast<ChartViewStyleImpl&>(Extension::GetImpl(style));
}

const ChartViewStyleImpl& GetStyleImpl(const ChartViewStyle& style)
{
  DALI_ASSERT_ALWAYS(style);
  return static_cast<const ChartViewStyleImpl&>(Extension::GetImpl(style));
}
} // namespace

/**
 * @brief Owns the mutable style while its builder is valid.
 */
struct ChartViewStyle::Builder::Impl
{
  explicit Impl(UiStyle style)
  : mStyle(std::move(style))
  {
  }

  UiStyle mStyle;
};

ChartViewStyle::ChartViewStyle() = default;

ChartViewStyle::~ChartViewStyle() = default;

ChartViewStyle::ChartViewStyle(const ChartViewStyle&) = default;

ChartViewStyle::ChartViewStyle(ChartViewStyle&&) noexcept = default;

ChartViewStyle& ChartViewStyle::operator=(const ChartViewStyle&) = default;

ChartViewStyle& ChartViewStyle::operator=(ChartViewStyle&&) noexcept = default;

UiStyleKey<ChartViewStyle> ChartViewStyle::DefaultKey()
{
  static UiStyleKey<ChartViewStyle> key = UiStyleKey<ChartViewStyle>::Alloc();
  return key;
}

ChartViewStyle ChartViewStyle::DefaultPreset()
{
  static_cast<void>(UiConfig::GetCurrent());
  static ChartViewStyle style = ChartViewStyle::Builder().Build();
  return style;
}

ChartViewStyle ChartViewStyle::Default()
{
  ChartViewStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

ChartViewStyle ChartViewStyle::DownCast(BaseHandle handle)
{
  return ChartViewStyle(dynamic_cast<ChartViewStyleImpl*>(handle.GetObjectPtr()));
}

ChartViewStyle ChartViewStyle::StaticDownCast(UiStyle style)
{
  return ChartViewStyle(static_cast<ChartViewStyleImpl*>(style.GetObjectPtr()));
}

ChartViewStyle::Builder ChartViewStyle::Configure() const
{
  return Builder(ChartViewStyle(new ChartViewStyleImpl(GetStyleImpl(*this))));
}

UiColor ChartViewStyle::GetBackgroundColor() const
{
  return GetStyleImpl(*this).mBackgroundColor;
}

UiColor ChartViewStyle::GetLabelColor() const
{
  return GetStyleImpl(*this).mLabelColor;
}

UiColor ChartViewStyle::GetLegendTextColor() const
{
  return GetStyleImpl(*this).mLegendTextColor;
}

UiColor ChartViewStyle::GetTooltipBackgroundColor() const
{
  return GetStyleImpl(*this).mTooltipBackgroundColor;
}

UiColor ChartViewStyle::GetTooltipTextColor() const
{
  return GetStyleImpl(*this).mTooltipTextColor;
}

float ChartViewStyle::GetPadding() const
{
  return GetStyleImpl(*this).mPadding;
}

float ChartViewStyle::GetTooltipPadding() const
{
  return GetStyleImpl(*this).mTooltipPadding;
}

ChartViewStyle::ChartViewStyle(Extension::UiStyleImpl* impl)
: UiStyle(impl)
{
}

ChartViewStyle::Builder::Builder()
: mImpl(MakeUnique<Impl>(ChartViewStyle(new ChartViewStyleImpl())))
{
}

ChartViewStyle::Builder::~Builder() = default;

ChartViewStyle::Builder::Builder(Builder&&) noexcept = default;

ChartViewStyle::Builder& ChartViewStyle::Builder::operator=(Builder&&) noexcept = default;

ChartViewStyle::Builder& ChartViewStyle::Builder::SetBackgroundColor(const UiColor& color) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  GetStyleImpl(mImpl->mStyle).mBackgroundColor = color;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetBackgroundColor(const UiColor& color) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetBackgroundColor(color);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetLabelColor(const UiColor& color) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  GetStyleImpl(mImpl->mStyle).mLabelColor = color;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetLabelColor(const UiColor& color) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetLabelColor(color);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetLegendTextColor(const UiColor& color) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  GetStyleImpl(mImpl->mStyle).mLegendTextColor = color;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetLegendTextColor(const UiColor& color) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetLegendTextColor(color);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetTooltipBackgroundColor(const UiColor& color) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  GetStyleImpl(mImpl->mStyle).mTooltipBackgroundColor = color;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetTooltipBackgroundColor(const UiColor& color) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetTooltipBackgroundColor(color);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetTooltipTextColor(const UiColor& color) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  GetStyleImpl(mImpl->mStyle).mTooltipTextColor = color;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetTooltipTextColor(const UiColor& color) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetTooltipTextColor(color);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetPadding(float value) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value >= 0.0f);
  GetStyleImpl(mImpl->mStyle).mPadding = value;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetPadding(float value) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetPadding(value);
  return std::move(*this);
}

ChartViewStyle::Builder& ChartViewStyle::Builder::SetTooltipPadding(float value) &
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value >= 0.0f);
  GetStyleImpl(mImpl->mStyle).mTooltipPadding = value;
  return *this;
}

ChartViewStyle::Builder&& ChartViewStyle::Builder::SetTooltipPadding(float value) &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  SetTooltipPadding(value);
  return std::move(*this);
}

ChartViewStyle ChartViewStyle::Builder::Build() &&
{
  DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER(mImpl);
  auto result = ChartViewStyle::StaticDownCast(mImpl->mStyle);
  mImpl.Reset();
  return result;
}

ChartViewStyle::Builder::Builder(UiStyle style)
: mImpl(MakeUnique<Impl>(std::move(style)))
{
}

} // namespace Ui
} // namespace DALI_NAMESPACE

#undef DALI_ASSERT_VALID_CHART_VIEW_STYLE_BUILDER

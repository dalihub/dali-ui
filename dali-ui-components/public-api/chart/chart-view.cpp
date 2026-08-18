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
#include <dali-ui-components/integration-api/chart/chart-view-impl.h>
#include <dali-ui-components/public-api/chart/chart-view.h>

namespace Dali
{
namespace Ui
{

namespace
{
inline Integration::ChartViewImpl& GetImpl(ChartView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  return static_cast<Integration::ChartViewImpl&>(handle.GetImplementation());
}

inline const Integration::ChartViewImpl& GetImpl(const ChartView& handle)
{
  DALI_ASSERT_ALWAYS(handle);
  return static_cast<const Integration::ChartViewImpl&>(handle.GetImplementation());
}
} // anonymous namespace

struct ChartPointEventArgs::Impl
{
  Impl() = default;

  Impl(int seriesIndexIn, int pointIndexIn, float dataXIn, float dataYIn, const Dali::String& seriesNameIn, const Dali::String& xLabelIn)
  : seriesIndex(seriesIndexIn),
    pointIndex(pointIndexIn),
    dataX(dataXIn),
    dataY(dataYIn),
    seriesName(seriesNameIn),
    xLabel(xLabelIn)
  {
  }

  int          seriesIndex{-1};
  int          pointIndex{-1};
  float        dataX{0.0f};
  float        dataY{0.0f};
  Dali::String seriesName;
  Dali::String xLabel;
};

ChartPointEventArgs::ChartPointEventArgs()
: mImpl(MakeUnique<Impl>())
{
}

ChartPointEventArgs::ChartPointEventArgs(int                 seriesIndex,
                                         int                 pointIndex,
                                         float               dataX,
                                         float               dataY,
                                         const Dali::String& seriesName,
                                         const Dali::String& xLabel)
: mImpl(MakeUnique<Impl>(seriesIndex, pointIndex, dataX, dataY, seriesName, xLabel))
{
}

ChartPointEventArgs::ChartPointEventArgs(const ChartPointEventArgs& rhs)
: mImpl(MakeUnique<Impl>(*rhs.mImpl))
{
}

ChartPointEventArgs::ChartPointEventArgs(ChartPointEventArgs&& rhs) noexcept = default;

ChartPointEventArgs& ChartPointEventArgs::operator=(const ChartPointEventArgs& rhs)
{
  if(this != &rhs)
  {
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

ChartPointEventArgs& ChartPointEventArgs::operator=(ChartPointEventArgs&& rhs) noexcept = default;

ChartPointEventArgs::~ChartPointEventArgs() = default;

int ChartPointEventArgs::GetSeriesIndex() const
{
  return mImpl->seriesIndex;
}

int ChartPointEventArgs::GetPointIndex() const
{
  return mImpl->pointIndex;
}

float ChartPointEventArgs::GetDataX() const
{
  return mImpl->dataX;
}

float ChartPointEventArgs::GetDataY() const
{
  return mImpl->dataY;
}

const Dali::String& ChartPointEventArgs::GetSeriesName() const
{
  return mImpl->seriesName;
}

const Dali::String& ChartPointEventArgs::GetXLabel() const
{
  return mImpl->xLabel;
}

ChartView::ChartView()
{
}

ChartView::ChartView(const ChartView& rhs)
: View(rhs)
{
}

ChartView::ChartView(ChartView&& rhs) noexcept = default;

ChartView& ChartView::operator=(const ChartView& rhs)
{
  if(&rhs != this)
  {
    View::operator=(rhs);
  }
  return *this;
}

ChartView& ChartView::operator=(ChartView&& rhs) noexcept = default;

ChartView::~ChartView()
{
}

ChartView ChartView::New(Type type, const Vector2& size)
{
  return Integration::ChartViewImpl::New(type, size);
}

ChartView ChartView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<ChartView, Integration::ChartViewImpl>(handle);
}

void ChartView::AddSeries(Ui::ChartSeries series)
{
  GetImpl(*this).AddSeries(series);
}

void ChartView::SetXAxis(Ui::ChartAxis axis)
{
  GetImpl(*this).SetXAxis(axis);
}

void ChartView::SetYAxis(Ui::ChartAxis axis)
{
  GetImpl(*this).SetYAxis(axis);
}

bool ChartView::RemoveSeries(const Dali::String& name)
{
  return GetImpl(*this).RemoveSeries(name);
}

void ChartView::RemoveAllSeries()
{
  GetImpl(*this).RemoveAllSeries();
}

void ChartView::SetTitle(const Dali::String& title)
{
  GetImpl(*this).SetTitle(title);
}

Dali::String ChartView::GetTitle() const
{
  return GetImpl(*this).GetTitle();
}

void ChartView::SetTitlePosition(TitlePosition position)
{
  GetImpl(*this).SetTitlePosition(position);
}

ChartView::TitlePosition ChartView::GetTitlePosition() const
{
  return GetImpl(*this).GetTitlePosition();
}

void ChartView::SetTitleColor(const Vector4& color)
{
  GetImpl(*this).SetTitleColor(color);
}

Vector4 ChartView::GetTitleColor() const
{
  return GetImpl(*this).GetTitleColor();
}

void ChartView::SetTooltipFormatter(TooltipFormatterType formatter)
{
  GetImpl(*this).SetTooltipFormatter(formatter);
}

void ChartView::SetAnimationDuration(float milliseconds)
{
  GetImpl(*this).SetAnimationDuration(milliseconds);
}

float ChartView::GetAnimationDuration() const
{
  return GetImpl(*this).GetAnimationDuration();
}

void ChartView::SetAnimationEasing(EasingType easing)
{
  GetImpl(*this).SetAnimationEasing(easing);
}

ChartView::EasingType ChartView::GetAnimationEasing() const
{
  return GetImpl(*this).GetAnimationEasing();
}

void ChartView::SetUpdateThrottle(float ms)
{
  GetImpl(*this).SetUpdateThrottle(ms);
}

float ChartView::GetUpdateThrottle() const
{
  return GetImpl(*this).GetUpdateThrottle();
}

void ChartView::SetHitThreshold(float pixels)
{
  GetImpl(*this).SetHitThreshold(pixels);
}

float ChartView::GetHitThreshold() const
{
  return GetImpl(*this).GetHitThreshold();
}

void ChartView::SetFindingStrategy(FindingStrategy strategy)
{
  GetImpl(*this).SetFindingStrategy(strategy);
}

ChartView::FindingStrategy ChartView::GetFindingStrategy() const
{
  return GetImpl(*this).GetFindingStrategy();
}

void ChartView::SetLegendToggleEnabled(bool enabled)
{
  GetImpl(*this).SetLegendToggleEnabled(enabled);
}

bool ChartView::IsLegendToggleEnabled() const
{
  return GetImpl(*this).IsLegendToggleEnabled();
}

void ChartView::SetZoomMode(int flags)
{
  GetImpl(*this).SetZoomMode(flags);
}

int ChartView::GetZoomMode() const
{
  return GetImpl(*this).GetZoomMode();
}

void ChartView::ResetZoom()
{
  GetImpl(*this).ResetZoom();
}

void ChartView::SetZoomClampEnabled(bool enabled)
{
  GetImpl(*this).SetZoomClampEnabled(enabled);
}

bool ChartView::IsZoomClampEnabled() const
{
  return GetImpl(*this).IsZoomClampEnabled();
}

void ChartView::SetAutoFitYOnPanEnabled(bool enabled)
{
  GetImpl(*this).SetAutoFitYOnPan(enabled);
}

bool ChartView::IsAutoFitYOnPanEnabled() const
{
  return GetImpl(*this).IsAutoFitYOnPan();
}

void ChartView::AddSection(ChartSection section)
{
  GetImpl(*this).AddSection(section);
}

void ChartView::RemoveSection(ChartSection section)
{
  GetImpl(*this).RemoveSection(section);
}

void ChartView::ClearSections()
{
  GetImpl(*this).ClearSections();
}

ChartView::DataPointSelectedSignalType& ChartView::DataPointSelectedSignal()
{
  return GetImpl(*this).DataPointSelectedSignal();
}

ChartView::LegendItemTappedSignalType& ChartView::LegendItemTappedSignal()
{
  return GetImpl(*this).LegendItemTappedSignal();
}

ChartView::MultiPointSelectedSignalType& ChartView::MultiPointSelectedSignal()
{
  return GetImpl(*this).MultiPointSelectedSignal();
}

ChartView::ZoomedSignalType& ChartView::ZoomedSignal()
{
  return GetImpl(*this).ZoomedSignal();
}

void ChartView::SetGaugeValue(float v)
{
  GetImpl(*this).SetGaugeValue(v);
}
float ChartView::GetGaugeValue() const
{
  return GetImpl(*this).GetGaugeValue();
}

void ChartView::SetGaugeMinimumValue(float v)
{
  GetImpl(*this).SetGaugeMinValue(v);
}
float ChartView::GetGaugeMinimumValue() const
{
  return GetImpl(*this).GetGaugeMinValue();
}

void ChartView::SetGaugeMaximumValue(float v)
{
  GetImpl(*this).SetGaugeMaxValue(v);
}
float ChartView::GetGaugeMaximumValue() const
{
  return GetImpl(*this).GetGaugeMaxValue();
}

void ChartView::SetGaugeArcSpan(float d)
{
  GetImpl(*this).SetGaugeArcSpan(d);
}
float ChartView::GetGaugeArcSpan() const
{
  return GetImpl(*this).GetGaugeArcSpan();
}

void ChartView::SetGaugeStartAngle(float d)
{
  GetImpl(*this).SetGaugeStartAngle(d);
}
float ChartView::GetGaugeStartAngle() const
{
  return GetImpl(*this).GetGaugeStartAngle();
}

void ChartView::SetGaugeArcWidth(float r)
{
  GetImpl(*this).SetGaugeArcWidth(r);
}
float ChartView::GetGaugeArcWidth() const
{
  return GetImpl(*this).GetGaugeArcWidth();
}

void ChartView::SetGaugeTrackColor(const Vector4& c)
{
  GetImpl(*this).SetGaugeTrackColor(c);
}
Vector4 ChartView::GetGaugeTrackColor() const
{
  return GetImpl(*this).GetGaugeTrackColor();
}

void ChartView::SetGaugeProgressColor(const Vector4& c)
{
  GetImpl(*this).SetGaugeProgressColor(c);
}
Vector4 ChartView::GetGaugeProgressColor() const
{
  return GetImpl(*this).GetGaugeProgressColor();
}

void ChartView::SetGaugeCenterLabel(const Dali::String& t)
{
  GetImpl(*this).SetGaugeCenterLabel(t);
}
Dali::String ChartView::GetGaugeCenterLabel() const
{
  return GetImpl(*this).GetGaugeCenterLabel();
}

void ChartView::AddGaugeRange(float from, float to, const Vector4& color)
{
  GetImpl(*this).AddGaugeRange(from, to, color);
}

void ChartView::ClearGaugeRanges()
{
  GetImpl(*this).ClearGaugeRanges();
}

ChartView::ChartView(Integration::ChartViewImpl& implementation)
: View(implementation)
{
}

ChartView::ChartView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::ChartViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali

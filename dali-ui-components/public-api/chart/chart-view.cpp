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
#include <dali-ui-components/public-api/chart/chart-view.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-view-impl.h>

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

ChartView& ChartView::AddSeries(Ui::ChartSeries series)
{
  GetImpl(*this).AddSeries(series);
  return *this;
}

ChartView& ChartView::SetXAxis(Ui::ChartAxis axis)
{
  GetImpl(*this).SetXAxis(axis);
  return *this;
}

ChartView& ChartView::SetYAxis(Ui::ChartAxis axis)
{
  GetImpl(*this).SetYAxis(axis);
  return *this;
}

bool ChartView::RemoveSeries(const Dali::String& name)
{
  return GetImpl(*this).RemoveSeries(name);
}

ChartView& ChartView::RemoveAllSeries()
{
  GetImpl(*this).RemoveAllSeries();
  return *this;
}

ChartView& ChartView::SetTitle(const Dali::String& title)
{
  GetImpl(*this).SetTitle(title);
  return *this;
}

Dali::String ChartView::GetTitle() const
{
  return GetImpl(*this).GetTitle();
}

ChartView& ChartView::SetTitlePosition(TitlePosition position)
{
  GetImpl(*this).SetTitlePosition(position);
  return *this;
}

ChartView::TitlePosition ChartView::GetTitlePosition() const
{
  return GetImpl(*this).GetTitlePosition();
}

ChartView& ChartView::SetTitleColor(const Vector4& color)
{
  GetImpl(*this).SetTitleColor(color);
  return *this;
}

Vector4 ChartView::GetTitleColor() const
{
  return GetImpl(*this).GetTitleColor();
}

ChartView& ChartView::SetTooltipFormatter(TooltipFormatterType formatter)
{
  GetImpl(*this).SetTooltipFormatter(std::move(formatter));
  return *this;
}

ChartView& ChartView::SetAnimationDuration(float milliseconds)
{
  GetImpl(*this).SetAnimationDuration(milliseconds);
  return *this;
}

float ChartView::GetAnimationDuration() const
{
  return GetImpl(*this).GetAnimationDuration();
}

ChartView& ChartView::SetAnimationEasing(EasingType easing)
{
  GetImpl(*this).SetAnimationEasing(easing);
  return *this;
}

ChartView::EasingType ChartView::GetAnimationEasing() const
{
  return GetImpl(*this).GetAnimationEasing();
}

ChartView& ChartView::SetUpdateThrottle(float ms)
{
  GetImpl(*this).SetUpdateThrottle(ms);
  return *this;
}

float ChartView::GetUpdateThrottle() const
{
  return GetImpl(*this).GetUpdateThrottle();
}

ChartView& ChartView::SetHitThreshold(float pixels)
{
  GetImpl(*this).SetHitThreshold(pixels);
  return *this;
}

float ChartView::GetHitThreshold() const
{
  return GetImpl(*this).GetHitThreshold();
}

ChartView& ChartView::SetFindingStrategy(FindingStrategy strategy)
{
  GetImpl(*this).SetFindingStrategy(strategy);
  return *this;
}

ChartView::FindingStrategy ChartView::GetFindingStrategy() const
{
  return GetImpl(*this).GetFindingStrategy();
}

ChartView& ChartView::SetLegendToggleEnabled(bool enabled)
{
  GetImpl(*this).SetLegendToggleEnabled(enabled);
  return *this;
}

bool ChartView::GetLegendToggleEnabled() const
{
  return GetImpl(*this).GetLegendToggleEnabled();
}

ChartView& ChartView::SetZoomMode(int flags)
{
  GetImpl(*this).SetZoomMode(flags);
  return *this;
}

int ChartView::GetZoomMode() const
{
  return GetImpl(*this).GetZoomMode();
}

ChartView& ChartView::ResetZoom()
{
  GetImpl(*this).ResetZoom();
  return *this;
}

ChartView& ChartView::SetZoomClampEnabled(bool enabled)
{
  GetImpl(*this).SetZoomClampEnabled(enabled);
  return *this;
}

bool ChartView::IsZoomClampEnabled() const
{
  return GetImpl(*this).IsZoomClampEnabled();
}

ChartView& ChartView::SetAutoFitYOnPan(bool enabled)
{
  GetImpl(*this).SetAutoFitYOnPan(enabled);
  return *this;
}

bool ChartView::IsAutoFitYOnPan() const
{
  return GetImpl(*this).IsAutoFitYOnPan();
}

ChartView& ChartView::AddSection(ChartSection section)
{
  GetImpl(*this).AddSection(section);
  return *this;
}

ChartView& ChartView::RemoveSection(ChartSection section)
{
  GetImpl(*this).RemoveSection(section);
  return *this;
}

ChartView& ChartView::ClearSections()
{
  GetImpl(*this).ClearSections();
  return *this;
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

// Gauge

ChartView& ChartView::SetGaugeValue(float v)
{
  GetImpl(*this).SetGaugeValue(v);
  return *this;
}
float ChartView::GetGaugeValue() const
{
  return GetImpl(*this).GetGaugeValue();
}

ChartView& ChartView::SetGaugeMinValue(float v)
{
  GetImpl(*this).SetGaugeMinValue(v);
  return *this;
}
float ChartView::GetGaugeMinValue() const
{
  return GetImpl(*this).GetGaugeMinValue();
}

ChartView& ChartView::SetGaugeMaxValue(float v)
{
  GetImpl(*this).SetGaugeMaxValue(v);
  return *this;
}
float ChartView::GetGaugeMaxValue() const
{
  return GetImpl(*this).GetGaugeMaxValue();
}

ChartView& ChartView::SetGaugeArcSpan(float d)
{
  GetImpl(*this).SetGaugeArcSpan(d);
  return *this;
}
float ChartView::GetGaugeArcSpan() const
{
  return GetImpl(*this).GetGaugeArcSpan();
}

ChartView& ChartView::SetGaugeStartAngle(float d)
{
  GetImpl(*this).SetGaugeStartAngle(d);
  return *this;
}
float ChartView::GetGaugeStartAngle() const
{
  return GetImpl(*this).GetGaugeStartAngle();
}

ChartView& ChartView::SetGaugeArcWidth(float r)
{
  GetImpl(*this).SetGaugeArcWidth(r);
  return *this;
}
float ChartView::GetGaugeArcWidth() const
{
  return GetImpl(*this).GetGaugeArcWidth();
}

ChartView& ChartView::SetGaugeTrackColor(const Vector4& c)
{
  GetImpl(*this).SetGaugeTrackColor(c);
  return *this;
}
Vector4 ChartView::GetGaugeTrackColor() const
{
  return GetImpl(*this).GetGaugeTrackColor();
}

ChartView& ChartView::SetGaugeProgressColor(const Vector4& c)
{
  GetImpl(*this).SetGaugeProgressColor(c);
  return *this;
}
Vector4 ChartView::GetGaugeProgressColor() const
{
  return GetImpl(*this).GetGaugeProgressColor();
}

ChartView& ChartView::SetGaugeCenterLabel(const Dali::String& t)
{
  GetImpl(*this).SetGaugeCenterLabel(t);
  return *this;
}
Dali::String ChartView::GetGaugeCenterLabel() const
{
  return GetImpl(*this).GetGaugeCenterLabel();
}

ChartView& ChartView::AddGaugeRange(float from, float to, const Vector4& color)
{
  GetImpl(*this).AddGaugeRange(from, to, color);
  return *this;
}

ChartView& ChartView::ClearGaugeRanges()
{
  GetImpl(*this).ClearGaugeRanges();
  return *this;
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

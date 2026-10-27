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
#include <dali-ui-components/integration-api/chart/chart-pie-geometry.h>
#include <dali-ui-components/integration-api/chart/chart-renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-color-palette.h>
#include <dali-ui-components/integration-api/chart/chart-section-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/line-series-impl.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>
#include <dali-ui-components/integration-api/chart/scatter-series-impl.h>
#include <dali-ui-components/public-api/chart/bar-series.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-components/public-api/chart/pie-series.h>
#include <dali-ui-components/public-api/chart/scatter-series.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

#include <dali-ui-components/integration-api/chart/chart-bar-geometry.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-linear-gradient.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/locale-numeric-guard.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <locale>
#include <map>
#include <numeric>
#include <sstream>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

// =============================================================================
// Anonymous helpers
// =============================================================================

namespace
{

using PointVec = std::vector<std::pair<float, float>>;

std::string FormatDataLabel(const Dali::String& format, float value)
{
  char buffer[64];
  // TODO: Replace the printf-style data-label API before removing this locale guard.
  Dali::LocaleNumericGuard localeGuard;
  std::snprintf(buffer, sizeof(buffer), format.CStr(), value);
  return buffer;
}

Dali::CanvasRenderer::LinearGradient MakeChartGradient(const Ui::Gradient::Linear& authored, const Rect<float>& area)
{
  auto gradient = Dali::CanvasRenderer::LinearGradient::New();
  auto position = [&](const Vector2& point)
  {
    if(authored.GetUnits() == Ui::Gradient::Units::USER_SPACE)
    {
      return point;
    }
    return Vector2(area.x + (point.x + 0.5f) * area.width, area.y + (point.y + 0.5f) * area.height);
  };
  gradient.SetBounds(position(authored.GetStartPosition()), position(authored.GetEndPosition()));
  gradient.SetSpread(static_cast<Dali::CanvasRenderer::Gradient::Spread>(authored.GetSpreadMethod()));
  for(const auto& stop : authored.GetStopNodes())
  {
    gradient.AddColorStop(std::clamp(stop.GetOffset() + authored.GetStartOffset(), 0.0f, 1.0f), stop.GetColor().GetRgba());
  }
  return gradient;
}

PointVec DecimateMinMax(const PointVec& src, size_t targetCount)
{
  if(src.size() <= targetCount)
  {
    return {};
  }

  PointVec out;
  out.reserve(targetCount + 2);
  out.push_back(src.front());

  const size_t buckets    = (targetCount > 2) ? (targetCount - 2) / 2 : 1;
  const double bucketSize = static_cast<double>(src.size() - 2) / static_cast<double>(buckets);

  for(size_t b = 0; b < buckets; ++b)
  {
    const size_t start      = 1u + static_cast<size_t>(static_cast<double>(b) * bucketSize);
    const size_t end        = 1u + static_cast<size_t>(static_cast<double>(b + 1) * bucketSize);
    const size_t clampedEnd = std::min(end, src.size() - 1u);

    float  minVal = src[start].second, maxVal = src[start].second;
    size_t minIdx = start, maxIdx = start;
    for(size_t i = start; i < clampedEnd; ++i)
    {
      if(std::isnan(src[i].second))
      {
        continue;
      }
      if(src[i].second < minVal)
      {
        minVal = src[i].second;
        minIdx = i;
      }
      if(src[i].second > maxVal)
      {
        maxVal = src[i].second;
        maxIdx = i;
      }
    }

    if(minIdx <= maxIdx)
    {
      out.push_back(src[minIdx]);
      if(minIdx != maxIdx)
      {
        out.push_back(src[maxIdx]);
      }
    }
    else
    {
      out.push_back(src[maxIdx]);
      if(minIdx != maxIdx)
      {
        out.push_back(src[minIdx]);
      }
    }
  }

  out.push_back(src.back());
  return out;
}

Vector4 GetSeriesDisplayColor(Ui::ChartSeries& series, size_t paletteIdx)
{
  if(auto ls = Ui::LineSeries::DownCast(series))
  {
    return ls.GetColor();
  }
  if(auto bs = Ui::BarSeries::DownCast(series))
  {
    const auto& impl = GetImplementation(bs);
    return impl.IsColorSet() ? impl.GetColor() : ChartColorPalette::Get(static_cast<int>(paletteIdx));
  }
  if(auto ss = Ui::ScatterSeries::DownCast(series))
  {
    return GetImplementation(ss).GetColor();
  }
  return ChartColorPalette::Get(static_cast<int>(paletteIdx));
}

} // anonymous namespace

// =============================================================================
// BackgroundRenderer::Render
// =============================================================================

void BackgroundRenderer::Render(Ui::CanvasView&                         canvas,
                                const ChartModel&                       model,
                                const ScaleEngine&                      scale,
                                const ChartLayoutManager::LayoutResult& layout)
{
  canvas.RemoveAllDrawables();

  const StyleConfig& style    = model.mStyle;
  const Rect<float>& plotArea = layout.plotArea;

  // 1. Background fill (entire canvas, always)
  RenderBackground(canvas, Rect<float>(0.0f, 0.0f, layout.totalSize.width, layout.totalSize.height), style.render.backgroundColor);

  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    if(layout.hasLegend && style.visibility.showLegend)
    {
      RenderLegendSwatches(canvas, model, layout);
    }
    return;
  }

  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::GAUGE))
  {
    return;
  }

  // 2. Sections
  if(!model.mSections.empty())
  {
    RenderSections(canvas, model, scale, plotArea);
  }

  // 3. Grid lines
  if(style.visibility.showGrid)
  {
    RenderGrid(canvas, scale, layout, style, model);
  }

  // 4. Axis lines
  RenderAxes(canvas, scale, plotArea, style, model);

  // 5. Tick marks
  RenderTickMarks(canvas, scale, layout, style, model);

  // 6. Legend color swatches
  if(layout.hasLegend && style.visibility.showLegend)
  {
    RenderLegendSwatches(canvas, model, layout);
  }
}

// =============================================================================
// RenderBackground
// =============================================================================

void BackgroundRenderer::RenderBackground(Ui::CanvasView&    canvas,
                                          const Rect<float>& plotArea,
                                          const Vector4&     color)
{
  auto bg = Dali::CanvasRenderer::Shape::New();
  bg.AddRect(plotArea, Vector2::ZERO);
  bg.SetFillColor(color);
  bg.SetStrokeWidth(0.0f);
  canvas.AddDrawable(bg);
}

// =============================================================================
// RenderSections
// =============================================================================

void BackgroundRenderer::RenderSections(Ui::CanvasView&    canvas,
                                        const ChartModel&  model,
                                        const ScaleEngine& scale,
                                        const Rect<float>& pa)
{
  const float paRight  = pa.x + pa.width;
  const float paBottom = pa.y + pa.height;

  for(const auto& handle : model.mSections)
  {
    const auto& s = GetImplementation(const_cast<Ui::ChartSection&>(handle));

    const float cxL = std::isnan(s.xMin) ? pa.x : std::max(pa.x, scale.ToCanvasX(s.xMin));
    const float cxR = std::isnan(s.xMax) ? paRight : std::min(paRight, scale.ToCanvasX(s.xMax));
    const float cyT = std::isnan(s.yMax) ? pa.y : std::max(pa.y, scale.ToCanvasY(s.yMax));
    const float cyB = std::isnan(s.yMin) ? paBottom : std::min(paBottom, scale.ToCanvasY(s.yMin));

    const bool isVLine = !std::isnan(s.xMin) && !std::isnan(s.xMax) && std::abs(s.xMax - s.xMin) < 1e-6f;
    const bool isHLine = !std::isnan(s.yMin) && !std::isnan(s.yMax) && std::abs(s.yMax - s.yMin) < 1e-6f;

    if(!isVLine && !isHLine && cxL >= cxR)
    {
      continue;
    }
    if(!isVLine && !isHLine && cyT >= cyB)
    {
      continue;
    }

    if(isHLine || isVLine)
    {
      if(s.strokeWidth > 0.0f)
      {
        auto shape = Dali::CanvasRenderer::Shape::New();
        if(isHLine)
        {
          const float cy = scale.ToCanvasY(s.yMin);
          if(cy < pa.y || cy > paBottom)
          {
            continue;
          }
          shape.AddMoveTo(Vector2(cxL, cy));
          shape.AddLineTo(Vector2(cxR, cy));
        }
        else
        {
          const float cx = scale.ToCanvasX(s.xMin);
          if(cx < pa.x || cx > paRight)
          {
            continue;
          }
          shape.AddMoveTo(Vector2(cx, cyT));
          shape.AddLineTo(Vector2(cx, cyB));
        }
        shape.SetStrokeColor(s.strokeColor);
        shape.SetStrokeWidth(s.strokeWidth);
        canvas.AddDrawable(shape);
      }
    }
    else
    {
      auto shape = Dali::CanvasRenderer::Shape::New();
      shape.AddRect(Rect<float>(cxL, cyT, cxR - cxL, cyB - cyT), Vector2(s.cornerRadius, s.cornerRadius));

      if(s.fillColor.a > 0.0f)
      {
        shape.SetFillColor(s.fillColor);
      }

      if(s.strokeWidth > 0.0f)
      {
        shape.SetStrokeColor(s.strokeColor);
        shape.SetStrokeWidth(s.strokeWidth);
      }

      canvas.AddDrawable(shape);
    }
    if(s.patternEnabled)
    {
      for(float offset = -(cyB - cyT); offset < cxR - cxL; offset += 8.0f)
      {
        const float startX = std::max(0.0f, offset);
        const float startY = std::max(0.0f, -offset);
        const float length = std::min(cxR - cxL - startX, cyB - cyT - startY);
        auto        hatch  = Dali::CanvasRenderer::Shape::New();
        hatch.AddMoveTo(Vector2(cxL + startX, cyT + startY));
        hatch.AddLineTo(Vector2(cxL + startX + length, cyT + startY + length));
        hatch.SetStrokeColor(s.strokeColor);
        hatch.SetStrokeWidth(1.0f);
        canvas.AddDrawable(hatch);
      }
    }
  }
}

// =============================================================================
// RenderGrid
// =============================================================================

void BackgroundRenderer::RenderGrid(Ui::CanvasView&                         canvas,
                                    const ScaleEngine&                      scale,
                                    const ChartLayoutManager::LayoutResult& layout,
                                    const StyleConfig& /*style*/,
                                    const ChartModel& model)
{
  const Rect<float>& plotArea  = layout.plotArea;
  float              plotRight = plotArea.x + plotArea.width;

  auto& yAxisImpl = GetImplementation(model.mYAxis);
  float dashLen, gapLen;
  yAxisImpl.GetGridDash(dashLen, gapLen);

  Dali::Vector<float> dashPattern;
  dashPattern.PushBack(dashLen);
  dashPattern.PushBack(gapLen);

  for(float tickVal : layout.yTicks)
  {
    if(!yAxisImpl.GetShowGridLines())
    {
      continue;
    }
    float cy = scale.ToCanvasY(tickVal);

    if(cy < plotArea.y - 1.0f || cy > plotArea.y + plotArea.height + 1.0f)
    {
      continue;
    }

    auto line = Dali::CanvasRenderer::Shape::New();
    line.AddMoveTo(Vector2(plotArea.x, cy));
    line.AddLineTo(Vector2(plotRight, cy));
    line.SetStrokeColor(yAxisImpl.GetGridColor());
    line.SetStrokeWidth(1.0f);
    line.SetStrokeDash(dashPattern);
    line.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
    canvas.AddDrawable(line);
  }
}

// =============================================================================
// RenderAxes
// =============================================================================

void BackgroundRenderer::RenderAxes(Ui::CanvasView& canvas,
                                    const ScaleEngine& /*scale*/,
                                    const Rect<float>& plotArea,
                                    const StyleConfig& /*style*/,
                                    const ChartModel& model)
{
  float plotRight  = plotArea.x + plotArea.width;
  float plotBottom = plotArea.y + plotArea.height;

  auto& xAxisImpl = GetImplementation(model.mXAxis);
  auto& yAxisImpl = GetImplementation(model.mYAxis);

  const float axisX = yAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? plotRight : plotArea.x;
  const float axisY = xAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? plotArea.y : plotBottom;
  MakeStrokedLine(canvas,
                  Vector2(axisX, plotArea.y),
                  Vector2(axisX, plotBottom),
                  yAxisImpl.GetAxisLineColor(),
                  yAxisImpl.GetAxisLineWidth());

  MakeStrokedLine(canvas,
                  Vector2(plotArea.x, axisY),
                  Vector2(plotRight, axisY),
                  xAxisImpl.GetAxisLineColor(),
                  xAxisImpl.GetAxisLineWidth());
}

// =============================================================================
// RenderTickMarks
// =============================================================================

void BackgroundRenderer::RenderTickMarks(Ui::CanvasView&                         canvas,
                                         const ScaleEngine&                      scale,
                                         const ChartLayoutManager::LayoutResult& layout,
                                         const StyleConfig& /*style*/,
                                         const ChartModel& model)
{
  constexpr float TICK_LEN = ChartLayoutManager::TICK_LENGTH;

  const Rect<float>& plotArea   = layout.plotArea;
  float              plotBottom = plotArea.y + plotArea.height;

  auto& xAxisImpl = GetImplementation(model.mXAxis);
  auto& yAxisImpl = GetImplementation(model.mYAxis);

  const float axisX = yAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? plotArea.x + plotArea.width : plotArea.x;
  const float axisY = xAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? plotArea.y : plotBottom;
  for(float tickVal : layout.yTicks)
  {
    if(!yAxisImpl.mTickMarksVisible)
    {
      continue;
    }
    float cy = scale.ToCanvasY(tickVal);
    if(cy < plotArea.y - 1.0f || cy > plotBottom + 1.0f)
    {
      continue;
    }
    MakeStrokedLine(canvas,
                    Vector2(axisX + (yAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? TICK_LEN : -TICK_LEN), cy),
                    Vector2(axisX, cy),
                    yAxisImpl.GetAxisLineColor(),
                    1.5f);
  }

  for(float tickVal : layout.xTicks)
  {
    if(!xAxisImpl.mTickMarksVisible)
    {
      continue;
    }
    const float cx = scale.ToCanvasX(tickVal);
    if(!std::isfinite(cx) || cx < plotArea.x - 1.0f || cx > plotArea.x + plotArea.width + 1.0f)
    {
      continue;
    }
    MakeStrokedLine(canvas,
                    Vector2(cx, axisY),
                    Vector2(cx, axisY + (xAxisImpl.mPosition == Ui::ChartAxis::Position::FAR ? -TICK_LEN : TICK_LEN)),
                    xAxisImpl.GetAxisLineColor(),
                    1.5f);
  }
}

// =============================================================================
// MakeStrokedLine
// =============================================================================

Dali::CanvasRenderer::Shape BackgroundRenderer::MakeStrokedLine(Ui::CanvasView& canvas,
                                                                Vector2         from,
                                                                Vector2         to,
                                                                const Vector4&  color,
                                                                float           width)
{
  if(width <= 0.0f)
  {
    return {};
  }
  auto shape = Dali::CanvasRenderer::Shape::New();
  shape.AddMoveTo(from);
  shape.AddLineTo(to);
  shape.SetStrokeColor(color);
  shape.SetStrokeWidth(width);
  shape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
  canvas.AddDrawable(shape);
  return shape;
}

// =============================================================================
// RenderLegendSwatches
// =============================================================================

void BackgroundRenderer::RenderLegendSwatches(Ui::CanvasView&                         canvas,
                                              const ChartModel&                       model,
                                              const ChartLayoutManager::LayoutResult& layout)
{
  constexpr float SW = ChartLayoutManager::LEGEND_SWATCH;

  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    if(model.mSeriesList.empty())
    {
      return;
    }
    auto ps = Ui::PieSeries::DownCast(const_cast<Ui::ChartSeries&>(model.mSeriesList[0]));
    if(!ps)
    {
      return;
    }
    const auto&  slices = GetImplementation(ps).GetSlices();
    const size_t count  = std::min(layout.legendItems.size(), slices.size());
    for(size_t i = 0; i < count; ++i)
    {
      const ChartLayoutManager::LegendItem& item = layout.legendItems[i];
      auto                                  rect = Dali::CanvasRenderer::Shape::New();
      rect.AddRect(Rect<float>(item.iconCenter.x - SW * 0.5f, item.iconCenter.y - SW * 0.5f, SW, SW),
                   Vector2(2.0f, 2.0f));
      rect.SetFillColor(slices[i].color);
      rect.SetStrokeWidth(0.0f);
      canvas.AddDrawable(rect);
    }
    return;
  }

  const size_t count = std::min(layout.legendItems.size(), model.mSeriesList.size());
  for(size_t i = 0; i < count; ++i)
  {
    const ChartLayoutManager::LegendItem& item   = layout.legendItems[i];
    Ui::ChartSeries&                      series = const_cast<Ui::ChartSeries&>(model.mSeriesList[i]);
    const bool                            vis    = GetImplementation(series).IsVisible();

    Vector4 color = GetSeriesDisplayColor(series, i);
    if(!vis)
    {
      color.w = 0.3f;
    }

    auto rect = Dali::CanvasRenderer::Shape::New();
    rect.AddRect(Rect<float>(item.iconCenter.x - SW * 0.5f, item.iconCenter.y - SW * 0.5f, SW, SW),
                 Vector2(2.0f, 2.0f));
    rect.SetFillColor(color);
    rect.SetStrokeWidth(0.0f);
    canvas.AddDrawable(rect);
  }
}

// =============================================================================
// DataRenderer
// =============================================================================

std::vector<size_t> DataRenderer::SortedSeriesOrder(const ChartModel& model)
{
  const size_t        n = model.mSeriesList.size();
  std::vector<size_t> order(n);
  std::iota(order.begin(), order.end(), 0u);
  std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b)
  {
    int za = GetImplementation(const_cast<Ui::ChartSeries&>(model.mSeriesList[a])).GetZIndex();
    int zb = GetImplementation(const_cast<Ui::ChartSeries&>(model.mSeriesList[b])).GetZIndex();
    return za < zb;
  });
  return order;
}

std::vector<DataRenderer::DataLabelInfo> DataRenderer::Render(Ui::CanvasView&                         canvas,
                                                              const ChartModel&                       model,
                                                              const ScaleEngine&                      scale,
                                                              const ChartLayoutManager::LayoutResult& layout,
                                                              const OldYValues*                       pOldYValues,
                                                              float                                   animProgress,
                                                              const std::vector<ChartBarGeometry>*    oldBars)
{
  canvas.RemoveAllDrawables();
  const auto targetBars = BuildChartBars(model, scale);
  mRenderedBars         = oldBars ? InterpolateChartBars(*oldBars, targetBars, animProgress) : targetBars;
  std::vector<DataLabelInfo> outLabels;

  if(layout.plotArea.width <= 0.0f || layout.plotArea.height <= 0.0f)
  {
    return outLabels;
  }

  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    auto group = Dali::CanvasRenderer::DrawableGroup::New();
    RenderPie(group, model, layout, outLabels);
    canvas.AddDrawable(group);
    return outLabels;
  }

  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::GAUGE))
  {
    auto group = Dali::CanvasRenderer::DrawableGroup::New();
    RenderGauge(group, model, layout, outLabels);
    canvas.AddDrawable(group);
    return outLabels;
  }

  const auto order = SortedSeriesOrder(model);

  const Rect<float>& pa        = layout.plotArea;
  const float        r         = model.mStyle.render.markerRadius;
  auto               clipShape = Dali::CanvasRenderer::Shape::New();
  clipShape.AddRect(Rect<float>(pa.x - r, pa.y - r, pa.width + 2.0f * r, pa.height + 2.0f * r), Vector2::ZERO);
  clipShape.SetFillColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

  auto group = Dali::CanvasRenderer::DrawableGroup::New();
  group.SetClipPath(clipShape);

  RenderBars(group, model, scale, layout, order);
  RenderFillAreas(group, model, scale, layout, order, pOldYValues, animProgress);
  RenderLines(group, model, scale, layout, order, pOldYValues, animProgress);
  RenderMarkers(group, model, scale, layout, order, pOldYValues, animProgress);
  RenderScatters(group, model, scale, order);

  canvas.AddDrawable(group);

  if(animProgress >= 1.0f)
  {
    RenderDataLabels(model, scale, layout, outLabels, order);
  }

  return outLabels;
}

void DataRenderer::RenderFillAreas(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& order, const OldYValues* pOldY, float t)
{
  auto lerpCY = [&](size_t si, int pi, float newDataY) -> float
  {
    float newCY = scale.ToCanvasY(newDataY);
    if(pOldY && t < 1.0f && si < pOldY->size() && pi < static_cast<int>((*pOldY)[si].size()))
    {
      float oldCY = (*pOldY)[si][static_cast<size_t>(pi)];
      if(!std::isnan(oldCY))
      {
        return oldCY + (newCY - oldCY) * t;
      }
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible())
    {
      continue;
    }

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries)
    {
      continue;
    }

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsFillEnabled())
    {
      continue;
    }

    const auto& points = impl.GetValues();
    if(points.size() < 2)
    {
      continue;
    }

    auto fillShape = Dali::CanvasRenderer::Shape::New();
    fillShape.SetFillColor(impl.GetFillColor());
    if(impl.mHasFillGradient)
    {
      fillShape.SetFillGradient(MakeChartGradient(impl.mFillGradient, scale.GetPlotArea()));
    }
    fillShape.SetStrokeWidth(0.0f);

    float baseCy     = layout.plotArea.y + layout.plotArea.height;
    bool  firstPoint = true;
    float prevCx     = 0.0f;
    int   pi         = 0;

    for(const auto& pt : points)
    {
      if(!scale.IsValid(pt.first, pt.second))
      {
        if(!firstPoint)
        {
          fillShape.AddLineTo(Vector2(prevCx, baseCy));
        }
        firstPoint = true;
        continue;
      }

      float cx = scale.ToCanvasX(pt.first);
      float cy = lerpCY(idx, pi, pt.second);
      ++pi;

      if(firstPoint)
      {
        fillShape.AddMoveTo(Vector2(cx, baseCy));
        fillShape.AddLineTo(Vector2(cx, cy));
        firstPoint = false;
      }
      else
      {
        fillShape.AddLineTo(Vector2(cx, cy));
      }
      prevCx = cx;
    }

    if(!firstPoint)
    {
      fillShape.AddLineTo(Vector2(prevCx, baseCy));
    }

    group.AddDrawable(fillShape);
  }
}

void DataRenderer::RenderLines(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& /*layout*/, const std::vector<size_t>& order, const OldYValues* pOldY, float t)
{
  auto lerpCY = [&](size_t si, int pi, float newDataY) -> float
  {
    float newCY = scale.ToCanvasY(newDataY);
    if(pOldY && t < 1.0f && si < pOldY->size() && pi < static_cast<int>((*pOldY)[si].size()))
    {
      float oldCY = (*pOldY)[si][static_cast<size_t>(pi)];
      if(!std::isnan(oldCY))
      {
        return oldCY + (newCY - oldCY) * t;
      }
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible())
    {
      continue;
    }

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries)
    {
      continue;
    }

    auto&       impl      = GetImplementation(lineSeries);
    const auto& rawPoints = impl.GetValues();
    if(rawPoints.empty())
    {
      continue;
    }

    const size_t   maxPts    = static_cast<size_t>(scale.GetPlotArea().width) * 2u;
    const PointVec decimated = (maxPts > 0u && rawPoints.size() > maxPts && impl.mPointColors.empty() && impl.mSegments.empty() &&
                                !(pOldY && t < 1.0f) &&
                                std::all_of(rawPoints.begin(), rawPoints.end(), [](const auto& point)
    {
      return std::isfinite(point.first) && std::isfinite(point.second);
    }))
                                 ? DecimateMinMax(rawPoints, maxPts)
                                 : PointVec{};
    const auto&    points    = decimated.empty() ? rawPoints : decimated;

    auto lineShape = Dali::CanvasRenderer::Shape::New();
    lineShape.SetStrokeColor(impl.GetColor());
    lineShape.SetStrokeWidth(impl.GetLineWidth());
    if(impl.mHasStrokeGradient)
    {
      lineShape.SetStrokeGradient(MakeChartGradient(impl.mStrokeGradient, scale.GetPlotArea()));
    }
    if(impl.mGapLength > 0.0f)
    {
      Dali::Vector<float> dash;
      dash.PushBack(impl.mDashLength);
      dash.PushBack(impl.mGapLength);
      lineShape.SetStrokeDash(dash);
    }
    lineShape.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    lineShape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);

    if(!impl.mSegments.empty())
    {
      for(size_t i = 1; i < rawPoints.size(); ++i)
      {
        const auto& previous = rawPoints[i - 1];
        const auto& current  = rawPoints[i];
        if(!scale.IsValid(previous.first, previous.second) || !scale.IsValid(current.first, current.second))
        {
          continue;
        }
        auto edge = Dali::CanvasRenderer::Shape::New();
        edge.SetStrokeWidth(impl.GetLineWidth());
        edge.SetStrokeColor(impl.GetColor());
        edge.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);
        if(impl.mHasStrokeGradient)
        {
          edge.SetStrokeGradient(MakeChartGradient(impl.mStrokeGradient, scale.GetPlotArea()));
        }
        float dashLength = impl.mDashLength;
        float gapLength  = impl.mGapLength;
        for(const auto& segmentStyle : impl.mSegments)
        {
          if(i - 1 >= segmentStyle.first && i - 1 < segmentStyle.last)
          {
            edge.SetStrokeColor(segmentStyle.color);
            dashLength = segmentStyle.dashLength;
            gapLength  = segmentStyle.gapLength;
          }
        }
        if(gapLength > 0.0f)
        {
          Dali::Vector<float> dash;
          dash.PushBack(dashLength);
          dash.PushBack(gapLength);
          edge.SetStrokeDash(dash);
        }
        const Vector2 start  = scale.ToCanvas(previous.first, previous.second);
        const Vector2 finish = scale.ToCanvas(current.first, current.second);
        edge.AddMoveTo(start);
        if(impl.GetSmoothness() > 0.0f)
        {
          const auto&   before = rawPoints[i > 1 ? i - 2 : i - 1];
          const auto&   after  = rawPoints[std::min(i + 1, rawPoints.size() - 1)];
          const Vector2 p0     = scale.IsValid(before.first, before.second) ? scale.ToCanvas(before.first, before.second) : start;
          const Vector2 p3     = scale.IsValid(after.first, after.second) ? scale.ToCanvas(after.first, after.second) : finish;
          edge.AddCubicTo(start + (finish - p0) * (impl.GetSmoothness() / 6.0f),
                          finish - (p3 - start) * (impl.GetSmoothness() / 6.0f), finish);
        }
        else
        {
          edge.AddLineTo(finish);
        }
        group.AddDrawable(edge);
      }
      continue;
    }

    const float smoothness = impl.GetSmoothness();

    std::vector<Vector2> segment;
    auto                 flushSegment = [&]()
    {
      if(segment.size() < 2)
      {
        if(segment.size() == 1)
        {
          lineShape.AddMoveTo(segment[0]);
        }
        segment.clear();
        return;
      }
      lineShape.AddMoveTo(segment[0]);
      if(smoothness > 0.0f)
      {
        const int n = static_cast<int>(segment.size());
        for(int i = 0; i < n - 1; ++i)
        {
          Vector2 p0  = segment[static_cast<size_t>(std::max(0, i - 1))];
          Vector2 p1  = segment[static_cast<size_t>(i)];
          Vector2 p2  = segment[static_cast<size_t>(i + 1)];
          Vector2 p3  = segment[static_cast<size_t>(std::min(n - 1, i + 2))];
          Vector2 cp1 = p1 + (p2 - p0) * (smoothness / 6.0f);
          Vector2 cp2 = p2 - (p3 - p1) * (smoothness / 6.0f);
          lineShape.AddCubicTo(cp1, cp2, p2);
        }
      }
      else
      {
        for(int i = 1; i < static_cast<int>(segment.size()); ++i)
        {
          lineShape.AddLineTo(segment[static_cast<size_t>(i)]);
        }
      }
      segment.clear();
    };

    int pi_l = 0;
    for(const auto& pt : points)
    {
      if(!scale.IsValid(pt.first, pt.second))
      {
        flushSegment();
        ++pi_l;
        continue;
      }
      segment.push_back(Vector2(scale.ToCanvasX(pt.first), lerpCY(idx, pi_l, pt.second)));
      ++pi_l;
    }
    flushSegment();
    group.AddDrawable(lineShape);
  }
}

void DataRenderer::RenderMarkers(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& /*layout*/, const std::vector<size_t>& order, const OldYValues* pOldY, float t)
{
  auto lerpCY = [&](size_t si, int pi, float newDataY) -> float
  {
    float newCY = scale.ToCanvasY(newDataY);
    if(pOldY && t < 1.0f && si < pOldY->size() && pi < static_cast<int>((*pOldY)[si].size()))
    {
      float oldCY = (*pOldY)[si][static_cast<size_t>(pi)];
      if(!std::isnan(oldCY))
      {
        return oldCY + (newCY - oldCY) * t;
      }
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible())
    {
      continue;
    }

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries)
    {
      continue;
    }

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsMarkersVisible())
    {
      continue;
    }

    const auto& rawPoints = impl.GetValues();
    if(rawPoints.empty())
    {
      continue;
    }

    const size_t   maxPts    = static_cast<size_t>(scale.GetPlotArea().width) * 2u;
    const PointVec decimated = (maxPts > 0u && rawPoints.size() > maxPts && impl.mPointColors.empty() && impl.mSegments.empty() &&
                                !(pOldY && t < 1.0f) &&
                                std::all_of(rawPoints.begin(), rawPoints.end(), [](const auto& point)
    {
      return std::isfinite(point.first) && std::isfinite(point.second);
    }))
                                 ? DecimateMinMax(rawPoints, maxPts)
                                 : PointVec{};
    const auto&    points    = decimated.empty() ? rawPoints : decimated;

    auto markerShape = Dali::CanvasRenderer::Shape::New();
    markerShape.SetFillColor(impl.GetMarkerColor());

    float bw = impl.GetMarkerBorderWidth();
    if(bw > 0.0f)
    {
      markerShape.SetStrokeColor(impl.GetMarkerBorderColor());
      markerShape.SetStrokeWidth(bw);
    }

    float r         = impl.GetMarkerRadius();
    auto  shapeType = impl.GetMarkerShape();

    int pi_m = 0;
    for(const auto& pt : points)
    {
      if(!scale.IsValid(pt.first, pt.second))
      {
        ++pi_m;
        continue;
      }
      float cx = scale.ToCanvasX(pt.first);
      float cy = lerpCY(idx, pi_m, pt.second);
      ++pi_m;

      if(!impl.mPointColors.empty())
      {
        auto pointShape = Dali::CanvasRenderer::Shape::New();
        pointShape.SetFillColor(impl.PointColor(static_cast<uint32_t>(pi_m - 1)));
        pointShape.SetStrokeColor(impl.GetMarkerBorderColor());
        pointShape.SetStrokeWidth(impl.GetMarkerBorderWidth());
        DrawMarkerShape(pointShape, static_cast<int>(shapeType), cx, cy, r);
        group.AddDrawable(pointShape);
      }
      else
      {
        DrawMarkerShape(markerShape, static_cast<int>(shapeType), cx, cy, r);
      }
    }
    group.AddDrawable(markerShape);
  }
}

// =============================================================================
// DrawMarkerShape (static)
// =============================================================================

void DataRenderer::DrawMarkerShape(Dali::CanvasRenderer::Shape& shape, int ms, float cx, float cy, float r)
{
  switch(ms)
  {
    case 0: // CIRCLE
    {
      shape.AddCircle(Vector2(cx, cy), Vector2(r, r));
      break;
    }
    case 1: // SQUARE
    {
      shape.AddRect(Rect<float>(cx - r, cy - r, r * 2.0f, r * 2.0f), Vector2::ZERO);
      break;
    }
    case 2: // TRIANGLE
    {
      shape.AddMoveTo(Vector2(cx, cy - r));
      shape.AddLineTo(Vector2(cx - r, cy + r));
      shape.AddLineTo(Vector2(cx + r, cy + r));
      shape.AddLineTo(Vector2(cx, cy - r));
      break;
    }
    case 3: // DIAMOND
    {
      shape.AddMoveTo(Vector2(cx, cy - r));
      shape.AddLineTo(Vector2(cx + r, cy));
      shape.AddLineTo(Vector2(cx, cy + r));
      shape.AddLineTo(Vector2(cx - r, cy));
      shape.AddLineTo(Vector2(cx, cy - r));
      break;
    }
    default:
    {
      break;
    }
  }
}

// =============================================================================
// RenderScatters
// =============================================================================

void DataRenderer::RenderScatters(Dali::CanvasRenderer::DrawableGroup& group,
                                  const ChartModel&                    model,
                                  const ScaleEngine&                   scale,
                                  const std::vector<size_t>&           order)
{
  for(size_t idx : order)
  {
    const auto&       baseSeries = model.mSeriesList[idx];
    Ui::ScatterSeries ss         = Ui::ScatterSeries::DownCast(
      const_cast<Ui::ChartSeries&>(baseSeries));
    if(!ss)
    {
      continue;
    }

    const auto& impl = GetImplementation(ss);
    if(!impl.IsVisible())
    {
      continue;
    }

    const float r  = impl.GetMarkerRadius();
    const int   ms = static_cast<int>(impl.GetMarkerShape());

    auto markerShape = Dali::CanvasRenderer::Shape::New();
    markerShape.SetFillColor(impl.GetColor());

    bool        hasPoints = false;
    const auto& points    = impl.GetValues();
    for(size_t pointIndex = 0u; pointIndex < points.size(); ++pointIndex)
    {
      const auto& pt = points[pointIndex];
      if(!scale.IsValid(pt.first, pt.second))
      {
        continue;
      }
      if(impl.mPointColors.empty())
      {
        DrawMarkerShape(markerShape, ms, scale.ToCanvasX(pt.first), scale.ToCanvasY(pt.second), r);
        hasPoints = true;
      }
      else
      {
        // Keep original indices through gaps, matching selection and point overrides.
        auto pointShape = Dali::CanvasRenderer::Shape::New();
        pointShape.SetFillColor(impl.PointColor(static_cast<uint32_t>(pointIndex)).GetRgba());
        DrawMarkerShape(pointShape, ms, scale.ToCanvasX(pt.first), scale.ToCanvasY(pt.second), r);
        group.AddDrawable(pointShape);
      }
    }

    if(hasPoints)
    {
      group.AddDrawable(markerShape);
    }
  }
}

// =============================================================================
// RenderBars
// =============================================================================

void DataRenderer::RenderBars(Dali::CanvasRenderer::DrawableGroup&    group,
                              const ChartModel&                       model,
                              const ScaleEngine&                      scale,
                              const ChartLayoutManager::LayoutResult& layout,
                              const std::vector<size_t>&              order)
{
  for(const auto& geometry : mRenderedBars)
  {
    if(geometry.seriesIndex >= model.mSeriesList.size() || geometry.bounds.height < .01f)
    {
      continue;
    }
    auto shape = Dali::CanvasRenderer::Shape::New();
    shape.AddRect(geometry.bounds, Vector2(geometry.radius, geometry.radius));
    shape.SetFillColor(geometry.color);
    const auto bar = Ui::BarSeries::DownCast(model.mSeriesList[geometry.seriesIndex]);
    if(!bar)
    {
      continue;
    }
    const auto& impl   = GetImplementation(bar);
    const auto  border = impl.GetPointBorder(static_cast<uint32_t>(geometry.pointIndex));
    if(border.width > 0.0f)
    {
      shape.SetStrokeColor(border.color);
      shape.SetStrokeWidth(border.width);
      if(border.dash.x > 0.0f && border.dash.y > 0.0f)
      {
        Dali::Vector<float> dash;
        dash.PushBack(border.dash.x);
        dash.PushBack(border.dash.y);
        shape.SetStrokeDash(dash);
      }
    }
    if(impl.mHasFillGradient && impl.mPointColors.find(static_cast<uint32_t>(geometry.pointIndex)) == impl.mPointColors.end())
    {
      shape.SetFillGradient(MakeChartGradient(impl.mFillGradient, geometry.bounds));
    }
    if(impl.mHasFillThreshold && impl.mPointColors.find(static_cast<uint32_t>(geometry.pointIndex)) == impl.mPointColors.end())
    {
      const float          cut = std::clamp((scale.ToCanvasY(impl.mFillThreshold) - geometry.bounds.y) / geometry.bounds.height, 0.0f, 1.0f);
      Ui::Gradient::Linear fill(Vector2(0.0f, -0.5f), Vector2(0.0f, 0.5f));
      fill.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(impl.mUpperFillColor)),
                         Ui::Gradient::StopNode(cut, Ui::UiColor(impl.mUpperFillColor)),
                         Ui::Gradient::StopNode(cut, Ui::UiColor(impl.mLowerFillColor)),
                         Ui::Gradient::StopNode(1.0f, Ui::UiColor(impl.mLowerFillColor))});
      shape.SetFillGradient(MakeChartGradient(fill, geometry.bounds));
    }
    group.AddDrawable(shape);
  }
}

// =============================================================================
// RenderStackedBars
// RenderDataLabels
// =============================================================================

void DataRenderer::RenderDataLabels(const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels, const std::vector<size_t>& order)
{
  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible())
    {
      continue;
    }

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries)
    {
      continue;
    }

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsDataLabelsVisible())
    {
      continue;
    }

    const auto& points = impl.GetValues();
    if(points.empty())
    {
      continue;
    }

    const auto&         formatter = impl.GetDataLabelFormatter();
    const Dali::String& formatStr = impl.GetDataLabelFormat();
    Vector4             color     = impl.GetDataLabelColor();
    float               size      = impl.GetDataLabelSize();
    float               offset    = impl.IsMarkersVisible() ? impl.GetMarkerRadius() + 6.0f : 6.0f;

    int pi = 0;
    for(const auto& pt : points)
    {
      if(!scale.IsValid(pt.first, pt.second))
      {
        ++pi;
        continue;
      }
      float cx = scale.ToCanvasX(pt.first);
      float cy = scale.ToCanvasY(pt.second);

      std::string text;
      if(formatter)
      {
        text = formatter(pt.second, pi).CStr();
      }
      else
      {
        text = FormatDataLabel(formatStr, pt.second);
      }

      DataLabelInfo info;
      info.text     = text.c_str();
      info.position = Vector2(cx, cy - offset);
      info.color    = color;
      info.size     = size;
      outLabels.push_back(info);
      ++pi;
    }
  }

  RenderBarDataLabels(model, scale, layout, outLabels, order);
}

// =============================================================================
// RenderBarDataLabels
// =============================================================================

void DataRenderer::RenderBarDataLabels(const ChartModel&                       model,
                                       const ScaleEngine&                      scale,
                                       const ChartLayoutManager::LayoutResult& layout,
                                       std::vector<DataLabelInfo>&             outLabels,
                                       const std::vector<size_t>&              order)
{
  for(const auto& geometry : BuildChartBars(model, scale))
  {
    const auto  bar  = Ui::BarSeries::DownCast(model.mSeriesList[geometry.seriesIndex]);
    const auto& impl = GetImplementation(bar);
    if(!impl.IsDataLabelsVisible() || geometry.bounds.height < 12.0f)
    {
      continue;
    }
    const std::string text = FormatDataLabel(impl.GetDataLabelFormat(), geometry.dataY);
    DataLabelInfo     info;
    info.text     = text.c_str();
    info.color    = impl.GetDataLabelColor();
    info.size     = impl.GetDataLabelSize();
    info.position = Vector2(geometry.bounds.x + geometry.bounds.width * 0.5f,
                            geometry.dataY >= 0.0f ? geometry.bounds.y - 4.0f : geometry.bounds.y + geometry.bounds.height + 4.0f);
    info.pivot    = geometry.dataY >= 0.0f ? Pivot::BOTTOM_CENTER : Pivot::TOP_CENTER;
    outLabels.push_back(info);
  }
}

// =============================================================================
// RenderPie
// =============================================================================

void DataRenderer::RenderPie(Dali::CanvasRenderer::DrawableGroup&    group,
                             const ChartModel&                       model,
                             const ChartLayoutManager::LayoutResult& layout,
                             std::vector<DataLabelInfo>&             outLabels)
{
  const auto sectors = BuildChartPieSlices(model, layout.plotArea);
  if(sectors.empty())
  {
    return;
  }
  const auto  pie   = Ui::PieSeries::DownCast(model.mSeriesList[sectors.front().hit.seriesIndex]);
  const auto& impl  = GetImplementation(pie);
  float       total = 0.0f;
  for(const auto& slice : impl.GetSlices())
  {
    if(std::isfinite(slice.value) && slice.value > 0.0f)
    {
      total += slice.value;
    }
  }
  for(const auto& sector : sectors)
  {
    const bool donut = sector.innerRadius > 0.0f;
    auto       shape = Dali::CanvasRenderer::Shape::New();
    if(donut)
    {
      shape.AddArc(sector.center, (sector.outerRadius + sector.innerRadius) * 0.5f, sector.startAngle, sector.sweepAngle, false);
      shape.SetFillColor(Vector4::ZERO);
      shape.SetStrokeColor(sector.hit.seriesColor);
      shape.SetStrokeWidth(sector.outerRadius - sector.innerRadius);
      shape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
    }
    else
    {
      shape.AddArc(sector.center, sector.outerRadius, sector.startAngle, sector.sweepAngle, true);
      shape.SetFillColor(sector.hit.seriesColor);
    }
    group.AddDrawable(shape);
    if(impl.IsDataLabelsVisible() && sector.sweepAngle >= impl.GetDataLabelMinAngle())
    {
      const float        angle  = (sector.startAngle + sector.sweepAngle * 0.5f) * Math::PI / 180.0f;
      const float        radius = donut ? (sector.outerRadius + sector.innerRadius) * 0.5f : sector.outerRadius * 0.65f;
      std::ostringstream labelStream;
      labelStream.imbue(std::locale::classic());
      labelStream << sector.hit.xLabel.CStr() << '\n'
                  << std::fixed << std::setprecision(0) << sector.hit.dataY / total * 100.0f << '%';
      std::string text = labelStream.str();
      if(text.size() >= 64u)
      {
        text.resize(63u);
      }
      DataLabelInfo label;
      label.text     = text.c_str();
      label.position = sector.center + Vector2(std::cos(angle), std::sin(angle)) * radius;
      label.pivot    = Pivot::CENTER;
      label.color    = Vector4::ONE;
      label.size     = 9.0f;
      outLabels.push_back(label);
    }
  }
  if(impl.GetInnerRadiusRatio() > 0.0f && !impl.GetCenterLabel().Empty())
  {
    DataLabelInfo label;
    label.text     = impl.GetCenterLabel().CStr();
    label.position = sectors.front().center;
    label.pivot    = Pivot::CENTER;
    label.color    = model.mStyle.render.labelColor;
    label.size     = 14.0f;
    outLabels.push_back(label);
  }
}

// =============================================================================
// OverlayRenderer
// =============================================================================

void OverlayRenderer::Clear(Ui::CanvasView& canvas)
{
  canvas.RemoveAllDrawables();
  canvas.RequestRasterization();
}

void OverlayRenderer::RenderCrosshair(Ui::CanvasView&    canvas,
                                      const Vector2&     hitPos,
                                      const ScaleEngine& scale,
                                      const StyleConfig& /*style*/)
{
  const Rect<float>& pa = scale.GetPlotArea();
  const Vector4      color(0.45f, 0.45f, 0.45f, 0.65f);

  Dali::Vector<float> dash;
  dash.PushBack(6.0f);
  dash.PushBack(4.0f);

  auto vLine = Dali::CanvasRenderer::Shape::New();
  vLine.AddMoveTo(Vector2(hitPos.x, pa.y));
  vLine.AddLineTo(Vector2(hitPos.x, pa.y + pa.height));
  vLine.SetStrokeColor(color);
  vLine.SetStrokeWidth(1.0f);
  vLine.SetStrokeDash(dash);
  vLine.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
  canvas.AddDrawable(vLine);

  auto hLine = Dali::CanvasRenderer::Shape::New();
  hLine.AddMoveTo(Vector2(pa.x, hitPos.y));
  hLine.AddLineTo(Vector2(pa.x + pa.width, hitPos.y));
  hLine.SetStrokeColor(color);
  hLine.SetStrokeWidth(1.0f);
  hLine.SetStrokeDash(dash);
  hLine.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
  canvas.AddDrawable(hLine);
}

void OverlayRenderer::RenderHighlight(Ui::CanvasView& canvas,
                                      const Vector2&  hitPos,
                                      const Vector4&  color,
                                      float           radius)
{
  auto circle = Dali::CanvasRenderer::Shape::New();
  circle.AddCircle(hitPos, Vector2(radius, radius));

  Vector4 fillColor = color;
  fillColor.w       = 0.25f;
  circle.SetFillColor(fillColor);
  circle.SetStrokeColor(color);
  circle.SetStrokeWidth(2.0f);
  canvas.AddDrawable(circle);
}

// =============================================================================
// RenderGauge
// =============================================================================

void DataRenderer::RenderGauge(Dali::CanvasRenderer::DrawableGroup&    group,
                               const ChartModel&                       model,
                               const ChartLayoutManager::LayoutResult& layout,
                               std::vector<DataLabelInfo>&             outLabels)
{
  const StyleConfig::GaugeConfig& g  = model.mStyle.gauge;
  const Rect<float>&              pa = layout.plotArea;

  const float range = g.maxValue - g.minValue;
  if(range < 1e-6f)
  {
    return;
  }

  const float side   = std::min(pa.width, pa.height);
  const float outerR = side * 0.5f * 0.85f;
  if(outerR < 1.0f)
  {
    return;
  }

  const Vector2 center(pa.x + pa.width * 0.5f, pa.y + pa.height * 0.5f);
  const float   arcW  = outerR * std::clamp(g.arcWidthRatio, 0.01f, 0.5f);
  const float   midR  = outerR - arcW * 0.5f;
  const float   span  = g.arcSpanDegrees;
  const float   start = g.startAngleDegrees;

  auto makeArc = [&](float fromDeg, float sweepDeg, const Vector4& color)
  {
    if(sweepDeg <= 0.0f)
    {
      return;
    }
    auto shape = Dali::CanvasRenderer::Shape::New();
    shape.AddArc(center, midR, fromDeg, sweepDeg, false);
    shape.SetFillColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    shape.SetStrokeColor(color);
    shape.SetStrokeWidth(arcW);
    shape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
    group.AddDrawable(shape);
  };

  // background track
  makeArc(start, span, g.trackColor);

  // colored range zones
  for(const auto& r : g.ranges)
  {
    const float clampedFrom = std::clamp(r.fromValue, g.minValue, g.maxValue);
    const float clampedTo   = std::clamp(r.toValue, g.minValue, g.maxValue);
    const float fromAngle   = start + (clampedFrom - g.minValue) / range * span;
    const float sweepAngle  = (clampedTo - clampedFrom) / range * span;
    makeArc(fromAngle, sweepAngle, r.color);
  }

  // progress arc
  const float clampedV      = std::clamp(g.value, g.minValue, g.maxValue);
  const float progressSweep = (clampedV - g.minValue) / range * span;
  makeArc(start, progressSweep, g.progressColor);

  // center label
  std::string labelText = g.centerLabel.CStr();
  if(labelText.empty())
  {
    std::ostringstream labelStream;
    labelStream.imbue(std::locale::classic());
    labelStream << std::fixed << std::setprecision(0) << clampedV;
    labelText = labelStream.str();
    if(labelText.size() >= 32u)
    {
      labelText.resize(31u);
    }
  }
  DataLabelInfo lbl;
  lbl.text     = labelText.c_str();
  lbl.position = center;
  lbl.pivot    = Pivot::CENTER;
  lbl.color    = model.mStyle.render.labelColor;
  lbl.size     = 18.0f;
  outLabels.push_back(lbl);
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE

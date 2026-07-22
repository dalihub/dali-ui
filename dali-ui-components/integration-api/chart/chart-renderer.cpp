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
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>

namespace Dali
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

PointVec DecimateMinMax(const PointVec& src, size_t targetCount)
{
  if(src.size() <= targetCount) return {};

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
      if(std::isnan(src[i].second)) continue;
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
      if(minIdx != maxIdx) out.push_back(src[maxIdx]);
    }
    else
    {
      out.push_back(src[maxIdx]);
      if(minIdx != maxIdx) out.push_back(src[minIdx]);
    }
  }

  out.push_back(src.back());
  return out;
}

Vector4 GetSeriesDisplayColor(Ui::ChartSeries& series, size_t paletteIdx)
{
  if(auto ls = Ui::LineSeries::DownCast(series))
    return ls.GetColor();
  if(auto bs = Ui::BarSeries::DownCast(series))
  {
    const auto& impl = GetImplementation(bs);
    return impl.IsColorSet() ? impl.GetColor() : ChartColorPalette::Get(static_cast<int>(paletteIdx));
  }
  if(auto ss = Ui::ScatterSeries::DownCast(series))
    return GetImplementation(ss).GetColor();
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
      RenderLegendSwatches(canvas, model, layout);
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

    if(!isVLine && !isHLine && cxL >= cxR) continue;
    if(!isVLine && !isHLine && cyT >= cyB) continue;

    if(isHLine || isVLine)
    {
      if(s.strokeWidth > 0.0f)
      {
        auto shape = Dali::CanvasRenderer::Shape::New();
        if(isHLine)
        {
          const float cy = scale.ToCanvasY(s.yMin);
          if(cy < pa.y || cy > paBottom) continue;
          shape.AddMoveTo(Vector2(cxL, cy));
          shape.AddLineTo(Vector2(cxR, cy));
        }
        else
        {
          const float cx = scale.ToCanvasX(s.xMin);
          if(cx < pa.x || cx > paRight) continue;
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
      shape.AddRect(Rect<float>(cxL, cyT, cxR - cxL, cyB - cyT), Vector2::ZERO);

      if(s.fillColor.a > 0.0f)
        shape.SetFillColor(s.fillColor);

      if(s.strokeWidth > 0.0f)
      {
        shape.SetStrokeColor(s.strokeColor);
        shape.SetStrokeWidth(s.strokeWidth);
      }

      canvas.AddDrawable(shape);
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
    float cy = scale.ToCanvasY(tickVal);

    if(cy < plotArea.y - 1.0f || cy > plotArea.y + plotArea.height + 1.0f)
      continue;

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

  MakeStrokedLine(canvas,
                  Vector2(plotArea.x, plotArea.y),
                  Vector2(plotArea.x, plotBottom),
                  yAxisImpl.GetAxisLineColor(),
                  yAxisImpl.GetAxisLineWidth());

  MakeStrokedLine(canvas,
                  Vector2(plotArea.x, plotBottom),
                  Vector2(plotRight, plotBottom),
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

  for(float tickVal : layout.yTicks)
  {
    float cy = scale.ToCanvasY(tickVal);
    if(cy < plotArea.y - 1.0f || cy > plotBottom + 1.0f)
      continue;
    MakeStrokedLine(canvas,
                    Vector2(plotArea.x - TICK_LEN, cy),
                    Vector2(plotArea.x, cy),
                    yAxisImpl.GetAxisLineColor(),
                    1.5f);
  }

  for(const auto& tl : layout.xTickLabels)
  {
    float cx = tl.position.x;
    if(cx < plotArea.x - 1.0f || cx > plotArea.x + plotArea.width + 1.0f)
      continue;
    MakeStrokedLine(canvas,
                    Vector2(cx, plotBottom),
                    Vector2(cx, plotBottom + TICK_LEN),
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
    if(model.mSeriesList.empty()) return;
    auto ps = Ui::PieSeries::DownCast(const_cast<Ui::ChartSeries&>(model.mSeriesList[0]));
    if(!ps) return;
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
    if(!vis) color.w = 0.3f;

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
                                                              float                                   animProgress)
{
  canvas.RemoveAllDrawables();
  std::vector<DataLabelInfo> outLabels;

  if(layout.plotArea.width <= 0.0f || layout.plotArea.height <= 0.0f)
    return outLabels;

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
    RenderDataLabels(model, scale, layout, outLabels, order);

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
        return oldCY + (newCY - oldCY) * t;
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible()) continue;

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries) continue;

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsFillEnabled()) continue;

    const auto& points = impl.GetValues();
    if(points.size() < 2) continue;

    auto fillShape = Dali::CanvasRenderer::Shape::New();
    fillShape.SetFillColor(impl.GetFillColor());
    fillShape.SetStrokeWidth(0.0f);

    float baseCy     = layout.plotArea.y + layout.plotArea.height;
    bool  firstPoint = true;
    float prevCx     = 0.0f;
    int   pi         = 0;

    for(const auto& pt : points)
    {
      if(std::isnan(pt.second))
      {
        if(!firstPoint)
          fillShape.AddLineTo(Vector2(prevCx, baseCy));
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
      fillShape.AddLineTo(Vector2(prevCx, baseCy));

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
        return oldCY + (newCY - oldCY) * t;
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible()) continue;

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries) continue;

    auto&       impl      = GetImplementation(lineSeries);
    const auto& rawPoints = impl.GetValues();
    if(rawPoints.empty()) continue;

    const size_t   maxPts    = static_cast<size_t>(scale.GetPlotArea().width) * 2u;
    const PointVec decimated = (maxPts > 0u && rawPoints.size() > maxPts)
                                 ? DecimateMinMax(rawPoints, maxPts)
                                 : PointVec{};
    const auto&    points    = decimated.empty() ? rawPoints : decimated;

    auto lineShape = Dali::CanvasRenderer::Shape::New();
    lineShape.SetStrokeColor(impl.GetColor());
    lineShape.SetStrokeWidth(impl.GetLineWidth());
    lineShape.SetStrokeJoin(Dali::CanvasRenderer::Shape::StrokeJoin::ROUND);
    lineShape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::ROUND);

    const float smoothness = impl.GetSmoothness();

    std::vector<Vector2> segment;
    auto                 flushSegment = [&]()
    {
      if(segment.size() < 2)
      {
        if(segment.size() == 1) lineShape.AddMoveTo(segment[0]);
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
          lineShape.AddLineTo(segment[static_cast<size_t>(i)]);
      }
      segment.clear();
    };

    int pi_l = 0;
    for(const auto& pt : points)
    {
      if(std::isnan(pt.second))
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
        return oldCY + (newCY - oldCY) * t;
    }
    return newCY;
  };

  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible()) continue;

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries) continue;

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsMarkersVisible()) continue;

    const auto& rawPoints = impl.GetValues();
    if(rawPoints.empty()) continue;

    const size_t   maxPts    = static_cast<size_t>(scale.GetPlotArea().width) * 2u;
    const PointVec decimated = (maxPts > 0u && rawPoints.size() > maxPts)
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
      if(std::isnan(pt.second))
      {
        ++pi_m;
        continue;
      }
      float cx = scale.ToCanvasX(pt.first);
      float cy = lerpCY(idx, pi_m, pt.second);
      ++pi_m;

      DrawMarkerShape(markerShape, static_cast<int>(shapeType), cx, cy, r);
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
      shape.AddCircle(Vector2(cx, cy), Vector2(r, r));
      break;
    case 1: // SQUARE
      shape.AddRect(Rect<float>(cx - r, cy - r, r * 2.0f, r * 2.0f), Vector2::ZERO);
      break;
    case 2: // TRIANGLE
      shape.AddMoveTo(Vector2(cx, cy - r));
      shape.AddLineTo(Vector2(cx - r, cy + r));
      shape.AddLineTo(Vector2(cx + r, cy + r));
      shape.AddLineTo(Vector2(cx, cy - r));
      break;
    case 3: // DIAMOND
      shape.AddMoveTo(Vector2(cx, cy - r));
      shape.AddLineTo(Vector2(cx + r, cy));
      shape.AddLineTo(Vector2(cx, cy + r));
      shape.AddLineTo(Vector2(cx - r, cy));
      shape.AddLineTo(Vector2(cx, cy - r));
      break;
    default:
      break;
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
    if(!ss) continue;

    const auto& impl = GetImplementation(ss);
    if(!impl.IsVisible()) continue;

    const float r  = impl.GetMarkerRadius();
    const int   ms = static_cast<int>(impl.GetMarkerShape());

    auto markerShape = Dali::CanvasRenderer::Shape::New();
    markerShape.SetFillColor(impl.GetColor());

    bool hasPoints = false;
    for(const auto& pt : impl.GetValues())
    {
      if(std::isnan(pt.second)) continue;
      DrawMarkerShape(markerShape, ms, scale.ToCanvasX(pt.first), scale.ToCanvasY(pt.second), r);
      hasPoints = true;
    }

    if(hasPoints) group.AddDrawable(markerShape);
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
  const Rect<float>& pa = layout.plotArea;

  std::vector<size_t> groupedIdx, stackedIdx;
  for(size_t idx : order)
  {
    auto bs = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
    if(!bs) continue;
    const auto& impl = GetImplementation(bs);
    if(!impl.IsVisible()) continue;
    if(impl.IsStacked())
      stackedIdx.push_back(idx);
    else
      groupedIdx.push_back(idx);
  }

  if(!stackedIdx.empty())
    RenderStackedBars(group, model, scale, layout, stackedIdx);

  if(groupedIdx.empty()) return;

  const bool hasCat     = !GetImplementation(model.mXAxis).GetLabels().empty();
  float      slotPixelW = scale.ToCanvasX(1.0f) - scale.ToCanvasX(0.0f);

  if(!hasCat)
  {
    float       minGap = std::numeric_limits<float>::max();
    const auto& pts0   = GetImplementation(
                         const_cast<Ui::ChartSeries&>(model.mSeriesList[groupedIdx[0]]))
                         .GetValues();
    for(size_t i = 1; i < pts0.size(); ++i)
      minGap = std::min(minGap, scale.ToCanvasX(pts0[i].first) - scale.ToCanvasX(pts0[i - 1].first));
    if(minGap > 1.0f && minGap < std::numeric_limits<float>::max())
      slotPixelW = minGap;
    else
      slotPixelW = 40.0f;
  }

  const auto& firstImpl = GetImplementation(
    Ui::BarSeries::DownCast(model.mSeriesList[groupedIdx[0]]));
  const float groupW  = slotPixelW * firstImpl.GetBarGroupWidth();
  const int   nBars   = static_cast<int>(groupedIdx.size());
  const float singleW = groupW / static_cast<float>(nBars);

  const float baseCY = std::clamp(scale.ToCanvasY(0.0f), pa.y, pa.y + pa.height);

  int k = 0;
  for(size_t idx : groupedIdx)
  {
    auto        bs   = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
    const auto& impl = GetImplementation(bs);

    const Vector4 color = impl.IsColorSet()
                            ? impl.GetColor()
                            : ChartColorPalette::Get(static_cast<int>(idx));

    const auto& pts = GetImplementation(model.mSeriesList[idx]).GetValues();

    auto barsShape = Dali::CanvasRenderer::Shape::New();
    barsShape.SetFillColor(color);

    for(const auto& pt : pts)
    {
      if(std::isnan(pt.second)) continue;

      const float cx      = scale.ToCanvasX(pt.first);
      const float barLeft = cx - groupW * 0.5f + static_cast<float>(k) * singleW;
      const float topCY   = std::clamp(scale.ToCanvasY(pt.second), pa.y, pa.y + pa.height);

      if(std::abs(topCY - baseCY) < 0.5f) continue;

      if(pt.second >= 0.0f)
        barsShape.AddRect(Rect<float>(barLeft, topCY, singleW, baseCY - topCY), Vector2::ZERO);
      else
        barsShape.AddRect(Rect<float>(barLeft, baseCY, singleW, topCY - baseCY), Vector2::ZERO);
    }
    group.AddDrawable(barsShape);
    ++k;
  }
}

// =============================================================================
// RenderStackedBars
// =============================================================================

void DataRenderer::RenderStackedBars(Dali::CanvasRenderer::DrawableGroup&    group,
                                     const ChartModel&                       model,
                                     const ScaleEngine&                      scale,
                                     const ChartLayoutManager::LayoutResult& layout,
                                     const std::vector<size_t>&              stackedIdx)
{
  const Rect<float>& pa         = layout.plotArea;
  const float        slotPixelW = scale.ToCanvasX(1.0f) - scale.ToCanvasX(0.0f);
  const auto&        firstImpl  = GetImplementation(
    Ui::BarSeries::DownCast(model.mSeriesList[stackedIdx[0]]));
  const float barW = slotPixelW * firstImpl.GetBarGroupWidth();

  std::map<int, float> accumPos;
  std::map<int, float> accumNeg;

  for(size_t idx : stackedIdx)
  {
    auto        bs   = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
    const auto& impl = GetImplementation(bs);

    const Vector4 color = impl.IsColorSet()
                            ? impl.GetColor()
                            : ChartColorPalette::Get(static_cast<int>(idx));

    const auto& pts = GetImplementation(model.mSeriesList[idx]).GetValues();

    for(const auto& pt : pts)
    {
      if(std::isnan(pt.second)) continue;

      const int   xKey = static_cast<int>(std::round(pt.first));
      const float cx   = scale.ToCanvasX(pt.first);
      const float left = cx - barW * 0.5f;

      auto bar  = Dali::CanvasRenderer::Shape::New();
      bool drew = false;

      if(pt.second >= 0.0f)
      {
        const float bottomCY = std::clamp(scale.ToCanvasY(accumPos[xKey]), pa.y, pa.y + pa.height);
        const float topCY    = std::clamp(scale.ToCanvasY(accumPos[xKey] + pt.second), pa.y, pa.y + pa.height);
        if(bottomCY - topCY >= 0.5f)
        {
          bar.AddRect(Rect<float>(left, topCY, barW, bottomCY - topCY), Vector2::ZERO);
          drew = true;
        }
        accumPos[xKey] += pt.second;
      }
      else
      {
        const float topCY    = std::clamp(scale.ToCanvasY(accumNeg[xKey]), pa.y, pa.y + pa.height);
        const float bottomCY = std::clamp(scale.ToCanvasY(accumNeg[xKey] + pt.second), pa.y, pa.y + pa.height);
        if(bottomCY - topCY >= 0.5f)
        {
          bar.AddRect(Rect<float>(left, topCY, barW, bottomCY - topCY), Vector2::ZERO);
          drew = true;
        }
        accumNeg[xKey] += pt.second;
      }

      if(drew)
      {
        bar.SetFillColor(color);
        group.AddDrawable(bar);
      }
    }
  }
}

// =============================================================================
// RenderDataLabels
// =============================================================================

void DataRenderer::RenderDataLabels(const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels, const std::vector<size_t>& order)
{
  for(size_t idx : order)
  {
    const auto& baseSeries = model.mSeriesList[idx];
    if(!GetImplementation(const_cast<Ui::ChartSeries&>(baseSeries)).IsVisible()) continue;

    Ui::LineSeries lineSeries = Ui::LineSeries::DownCast(baseSeries);
    if(!lineSeries) continue;

    auto& impl = GetImplementation(lineSeries);
    if(!impl.IsDataLabelsVisible()) continue;

    const auto& points = impl.GetValues();
    if(points.empty()) continue;

    const auto&         formatter = impl.GetDataLabelFormatter();
    const Dali::String& formatStr = impl.GetDataLabelFormat();
    Vector4             color     = impl.GetDataLabelColor();
    float               size      = impl.GetDataLabelSize();
    float               offset    = impl.IsMarkersVisible() ? impl.GetMarkerRadius() + 6.0f : 6.0f;

    char buf[64];
    int  pi = 0;
    for(const auto& pt : points)
    {
      if(std::isnan(pt.second))
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
        std::snprintf(buf, sizeof(buf), formatStr.CStr(), pt.second);
        text = buf;
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
  const Rect<float>& pa         = layout.plotArea;
  constexpr float    MIN_BAR_PX = 12.0f;
  constexpr float    LABEL_GAP  = 4.0f;

  std::vector<size_t> allGroupedIdx, allStackedIdx;
  for(size_t idx : order)
  {
    auto bs = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
    if(!bs) continue;
    const auto& impl = GetImplementation(bs);
    if(!impl.IsVisible()) continue;
    if(impl.IsStacked())
      allStackedIdx.push_back(idx);
    else
      allGroupedIdx.push_back(idx);
  }

  const bool hasCat     = !GetImplementation(model.mXAxis).GetLabels().empty();
  float      slotPixelW = scale.ToCanvasX(1.0f) - scale.ToCanvasX(0.0f);

  // ── Grouped ──────────────────────────────────────────────────────────────
  if(!allGroupedIdx.empty())
  {
    if(!hasCat)
    {
      float       minGap = std::numeric_limits<float>::max();
      const auto& pts0   = GetImplementation(
                           const_cast<Ui::ChartSeries&>(model.mSeriesList[allGroupedIdx[0]]))
                           .GetValues();
      for(size_t i = 1; i < pts0.size(); ++i)
        minGap = std::min(minGap, scale.ToCanvasX(pts0[i].first) - scale.ToCanvasX(pts0[i - 1].first));
      if(minGap > 1.0f && minGap < std::numeric_limits<float>::max())
        slotPixelW = minGap;
      else
        slotPixelW = 40.0f;
    }

    const auto& firstImpl = GetImplementation(
      Ui::BarSeries::DownCast(model.mSeriesList[allGroupedIdx[0]]));
    const float groupW  = slotPixelW * firstImpl.GetBarGroupWidth();
    const int   nBars   = static_cast<int>(allGroupedIdx.size());
    const float singleW = groupW / static_cast<float>(nBars);
    const float baseCY  = std::clamp(scale.ToCanvasY(0.0f), pa.y, pa.y + pa.height);

    int k = 0;
    for(size_t idx : allGroupedIdx)
    {
      auto        bs   = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
      const auto& impl = GetImplementation(bs);

      if(!impl.IsDataLabelsVisible())
      {
        ++k;
        continue;
      }

      char        buf[64];
      const auto& pts = GetImplementation(model.mSeriesList[idx]).GetValues();
      for(const auto& pt : pts)
      {
        if(std::isnan(pt.second)) continue;

        const float cx       = scale.ToCanvasX(pt.first);
        const float barCx    = cx - groupW * 0.5f + static_cast<float>(k) * singleW + singleW * 0.5f;
        const float barEndCY = std::clamp(scale.ToCanvasY(pt.second), pa.y, pa.y + pa.height);
        const float barH     = std::abs(baseCY - barEndCY);
        if(barH < MIN_BAR_PX) continue;

        std::snprintf(buf, sizeof(buf), impl.GetDataLabelFormat().CStr(), pt.second);

        DataLabelInfo info;
        info.text  = buf;
        info.color = impl.GetDataLabelColor();
        info.size  = impl.GetDataLabelSize();
        if(pt.second >= 0.0f)
        {
          info.position = Vector2(barCx, barEndCY - LABEL_GAP);
          info.pivot    = Pivot::BOTTOM_CENTER;
        }
        else
        {
          info.position = Vector2(barCx, barEndCY + LABEL_GAP);
          info.pivot    = Pivot::TOP_CENTER;
        }
        outLabels.push_back(info);
      }
      ++k;
    }
  }

  // ── Stacked ──────────────────────────────────────────────────────────────
  if(!allStackedIdx.empty())
  {
    std::map<int, float> accumPos, accumNeg;

    for(size_t idx : allStackedIdx)
    {
      auto        bs        = Ui::BarSeries::DownCast(model.mSeriesList[idx]);
      const auto& impl      = GetImplementation(bs);
      const bool  showLabel = impl.IsDataLabelsVisible();

      char        buf[64];
      const auto& pts = GetImplementation(model.mSeriesList[idx]).GetValues();
      for(const auto& pt : pts)
      {
        if(std::isnan(pt.second)) continue;

        const int   xKey = static_cast<int>(std::round(pt.first));
        const float cx   = scale.ToCanvasX(pt.first);

        if(pt.second >= 0.0f)
        {
          const float bottomCY = std::clamp(scale.ToCanvasY(accumPos[xKey]), pa.y, pa.y + pa.height);
          const float topCY    = std::clamp(scale.ToCanvasY(accumPos[xKey] + pt.second), pa.y, pa.y + pa.height);
          const float layerH   = bottomCY - topCY;
          accumPos[xKey] += pt.second;

          if(!showLabel || layerH < MIN_BAR_PX) continue;

          std::snprintf(buf, sizeof(buf), impl.GetDataLabelFormat().CStr(), pt.second);
          DataLabelInfo info;
          info.text     = buf;
          info.color    = impl.GetDataLabelColor();
          info.size     = impl.GetDataLabelSize();
          info.position = Vector2(cx, topCY - LABEL_GAP);
          info.pivot    = Pivot::BOTTOM_CENTER;
          outLabels.push_back(info);
        }
        else
        {
          const float topCY    = std::clamp(scale.ToCanvasY(accumNeg[xKey]), pa.y, pa.y + pa.height);
          const float bottomCY = std::clamp(scale.ToCanvasY(accumNeg[xKey] + pt.second), pa.y, pa.y + pa.height);
          const float layerH   = bottomCY - topCY;
          accumNeg[xKey] += pt.second;

          if(!showLabel || layerH < MIN_BAR_PX) continue;

          std::snprintf(buf, sizeof(buf), impl.GetDataLabelFormat().CStr(), pt.second);
          DataLabelInfo info;
          info.text     = buf;
          info.color    = impl.GetDataLabelColor();
          info.size     = impl.GetDataLabelSize();
          info.position = Vector2(cx, bottomCY + LABEL_GAP);
          info.pivot    = Pivot::TOP_CENTER;
          outLabels.push_back(info);
        }
      }
    }
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
  Integration::PieSeries* pieImpl = nullptr;
  for(const auto& s : model.mSeriesList)
  {
    auto ps = Ui::PieSeries::DownCast(const_cast<Ui::ChartSeries&>(s));
    if(ps && GetImplementation(ps).IsVisible())
    {
      pieImpl = &GetImplementation(ps);
      break;
    }
  }
  if(!pieImpl || pieImpl->GetSliceCount() == 0) return;

  const auto& slices = pieImpl->GetSlices();

  float total = 0.0f;
  for(const auto& sl : slices) total += sl.value;
  if(total <= 0.0f) return;

  const Rect<float>& pa     = layout.plotArea;
  const float        side   = std::min(pa.width, pa.height);
  const float        outerR = side * 0.5f;
  const Vector2      center(pa.x + pa.width * 0.5f, pa.y + pa.height * 0.5f);
  const float        innerR  = outerR * pieImpl->GetInnerRadiusRatio();
  const bool         isDonut = pieImpl->GetInnerRadiusRatio() > 0.0f;
  const float        gap     = pieImpl->GetSliceGap();

  float startAngle = 270.0f;

  for(const auto& sl : slices)
  {
    const float fullSweep   = (sl.value / total) * 360.0f;
    const float renderStart = startAngle + gap * 0.5f;
    const float renderSweep = fullSweep - gap;
    startAngle += fullSweep;

    if(renderSweep <= 0.0f) continue;

    auto shape = Dali::CanvasRenderer::Shape::New();

    if(!isDonut)
    {
      shape.AddArc(center, outerR, renderStart, renderSweep, true);
      shape.SetFillColor(sl.color);
    }
    else
    {
      const float midR    = (outerR + innerR) * 0.5f;
      const float strokeW = outerR - innerR;
      shape.AddArc(center, midR, renderStart, renderSweep, false);
      shape.SetFillColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
      shape.SetStrokeColor(sl.color);
      shape.SetStrokeWidth(strokeW);
      shape.SetStrokeCap(Dali::CanvasRenderer::Shape::StrokeCap::BUTT);
    }

    group.AddDrawable(shape);

    if(pieImpl->IsDataLabelsVisible() && renderSweep >= pieImpl->GetDataLabelMinAngle())
    {
      const float midRad = (renderStart + renderSweep * 0.5f) * Math::PI / 180.0f;
      const float labelR = isDonut ? (outerR + innerR) * 0.5f : outerR * 0.65f;
      const float pct    = (sl.value / total) * 100.0f;
      char        buf[64];
      std::snprintf(buf, sizeof(buf), "%s\n%.0f%%", sl.label.CStr(), pct);

      DataLabelInfo lbl;
      lbl.text     = buf;
      lbl.position = Vector2(center.x + labelR * std::cos(midRad),
                             center.y + labelR * std::sin(midRad));
      lbl.pivot    = Pivot::CENTER;
      lbl.color    = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
      lbl.size     = 9.0f;
      outLabels.push_back(lbl);
    }
  }

  const Dali::String& centerText = pieImpl->GetCenterLabel();
  if(isDonut && !centerText.Empty())
  {
    DataLabelInfo info;
    info.text     = centerText.CStr();
    info.position = Vector2(center.x, center.y);
    info.pivot    = Pivot::CENTER;
    info.color    = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    info.size     = 14.0f;
    outLabels.push_back(info);
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
  if(range < 1e-6f) return;

  const float side   = std::min(pa.width, pa.height);
  const float outerR = side * 0.5f * 0.85f;
  if(outerR < 1.0f) return;

  const Vector2 center(pa.x + pa.width * 0.5f, pa.y + pa.height * 0.5f);
  const float   arcW  = outerR * std::clamp(g.arcWidthRatio, 0.01f, 0.5f);
  const float   midR  = outerR - arcW * 0.5f;
  const float   span  = g.arcSpanDegrees;
  const float   start = g.startAngleDegrees;

  auto makeArc = [&](float fromDeg, float sweepDeg, const Vector4& color)
  {
    if(sweepDeg <= 0.0f) return;
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
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.0f", clampedV);
    labelText = buf;
  }
  DataLabelInfo lbl;
  lbl.text     = labelText.c_str();
  lbl.position = center;
  lbl.pivot    = Pivot::CENTER;
  lbl.color    = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
  lbl.size     = 18.0f;
  outLabels.push_back(lbl);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

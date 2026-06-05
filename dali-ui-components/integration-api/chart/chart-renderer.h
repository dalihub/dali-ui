#pragma once

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable-group.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-string.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-layout-manager.h>
#include <dali-ui-components/integration-api/chart/chart-model.h>
#include <dali-ui-components/integration-api/chart/chart-scale-engine.h>
#include <dali-ui-foundation/public-api/canvas-view.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

// =============================================================================
// BackgroundRenderer
// =============================================================================

/**
 * @brief Renders the static background layer: chart background fill, grid lines,
 *        X and Y axis lines, and tick marks.
 */
class BackgroundRenderer
{
public:
  /**
   * @brief Render all background elements into the canvas.
   *
   * @param[in,out] canvas  The background CanvasView layer.
   * @param[in]     model   Chart model (style, data).
   * @param[in]     scale   Scale engine with current plotArea.
   * @param[in]     layout  Computed layout (yTicks, xTickCount, plotArea).
   */
  void Render(Ui::CanvasView&                         canvas,
              const ChartModel&                       model,
              const ScaleEngine&                      scale,
              const ChartLayoutManager::LayoutResult& layout);

private:
  void RenderBackground(Ui::CanvasView&    canvas,
                        const Rect<float>& plotArea,
                        const Vector4&     color);

  void RenderSections(Ui::CanvasView&    canvas,
                      const ChartModel&  model,
                      const ScaleEngine& scale,
                      const Rect<float>& plotArea);

  void RenderGrid(Ui::CanvasView&                         canvas,
                  const ScaleEngine&                      scale,
                  const ChartLayoutManager::LayoutResult& layout,
                  const StyleConfig&                      style,
                  const ChartModel&                       model);

  void RenderAxes(Ui::CanvasView&    canvas,
                  const ScaleEngine& scale,
                  const Rect<float>& plotArea,
                  const StyleConfig& style,
                  const ChartModel&  model);

  void RenderTickMarks(Ui::CanvasView&                         canvas,
                       const ScaleEngine&                      scale,
                       const ChartLayoutManager::LayoutResult& layout,
                       const StyleConfig&                      style,
                       const ChartModel&                       model);

  static Dali::CanvasRenderer::Shape MakeStrokedLine(Ui::CanvasView& canvas,
                                                     Vector2         from,
                                                     Vector2         to,
                                                     const Vector4&  color,
                                                     float           width);

  void RenderLegendSwatches(Ui::CanvasView&                         canvas,
                            const ChartModel&                       model,
                            const ChartLayoutManager::LayoutResult& layout);
};

// =============================================================================
// DataRenderer
// =============================================================================

/**
 * @brief Renders the dynamic data layer: lines, bars, markers, etc.
 */
class DataRenderer
{
public:
  struct DataLabelInfo
  {
    Dali::String text;
    Vector2      position;
    Vector4      color;
    float        size;
    Vector3      pivot{Pivot::BOTTOM_CENTER};
  };

  /**
   * @brief Render all data series into the canvas.
   *
   * @param[in,out] canvas       The data CanvasView layer.
   * @param[in]     model        Chart model (data, style).
   * @param[in]     scale        Scale engine with current plotArea.
   * @param[in]     layout       Computed layout.
   * @param[in]     pOldYValues  Per-series Y-value snapshot for animation (nullptr = no animation).
   * @param[in]     animProgress Animation progress in [0,1].
   */
  std::vector<DataLabelInfo> Render(Ui::CanvasView&                         canvas,
                                    const ChartModel&                       model,
                                    const ScaleEngine&                      scale,
                                    const ChartLayoutManager::LayoutResult& layout,
                                    const std::vector<std::vector<float>>*  pOldYValues  = nullptr,
                                    float                                   animProgress = 1.0f);

private:
  static std::vector<size_t> SortedSeriesOrder(const ChartModel& model);

  using OldYValues = std::vector<std::vector<float>>;

  void RenderFillAreas(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& order, const OldYValues* pOldY, float t);
  void RenderLines(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& order, const OldYValues* pOldY, float t);
  void RenderMarkers(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& order, const OldYValues* pOldY, float t);
  void RenderDataLabels(const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels, const std::vector<size_t>& order);

  void RenderScatters(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const std::vector<size_t>& order);

  void RenderBars(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& order);
  void RenderStackedBars(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, const std::vector<size_t>& stackedIdx);

  void RenderBarDataLabels(const ChartModel& model, const ScaleEngine& scale, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels, const std::vector<size_t>& order);

  void RenderPie(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels);
  void RenderGauge(Dali::CanvasRenderer::DrawableGroup& group, const ChartModel& model, const ChartLayoutManager::LayoutResult& layout, std::vector<DataLabelInfo>& outLabels);

  static void DrawMarkerShape(Dali::CanvasRenderer::Shape& shape, int markerShape, float cx, float cy, float r);
};

// =============================================================================
// OverlayRenderer
// =============================================================================

/**
 * @brief Renders interactive overlay elements onto the overlay CanvasView layer.
 */
class OverlayRenderer
{
public:
  void RenderCrosshair(Ui::CanvasView&    canvas,
                       const Vector2&     hitPos,
                       const ScaleEngine& scale,
                       const StyleConfig& style);

  void RenderHighlight(Ui::CanvasView& canvas,
                       const Vector2&  hitPos,
                       const Vector4&  color,
                       float           radius);

  void Clear(Ui::CanvasView& canvas);
};

} // namespace Integration
} // namespace Ui
} // namespace Dali

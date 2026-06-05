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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-model.h>
#include <dali-ui-components/integration-api/chart/chart-scale-engine.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Result of a touch hit-test operation.
 */
struct HitResult
{
  Dali::String seriesName;
  Dali::String xLabel; ///< X-axis label string, or stringified dataX if none
  Vector4      seriesColor;
  Vector2      canvasPos; ///< Canvas pixel position of the matched data point
  float        dataX{0.0f};
  float        dataY{0.0f};
  int          seriesIndex{-1};
  int          pointIndex{-1};
  bool         isValid{false};
};

/**
 * @brief Finds the nearest data point to a touch position.
 *
 * Invisible series (SetVisible(false)) are excluded from search.
 * Coordinate space: ChartView actor-local == CanvasView pixel coordinates.
 */
class ChartHitTester
{
public:
  static constexpr float DEFAULT_THRESHOLD = 30.0f;

  /**
   * @brief Find the nearest data point within threshold pixels.
   *
   * @param[in] touchPos  ChartView actor-local coordinates
   * @param[in] model     Chart model (series list, axes)
   * @param[in] scale     Scale engine with current plotArea and data range
   * @param[in] threshold Maximum hit distance in pixels
   */
  HitResult FindNearest(const Vector2&     touchPos,
                        const ChartModel&  model,
                        const ScaleEngine& scale,
                        float              threshold = DEFAULT_THRESHOLD) const;

  /**
   * @brief Return true if touchPos is inside the current plotArea.
   */
  bool IsInsidePlotArea(const Vector2& touchPos, const ScaleEngine& scale) const;

  /**
   * @brief Find one HitResult per visible series at the X position nearest to touchPos.
   *
   * @param[in] touchPos   ChartView actor-local coordinates
   * @param[in] model      Chart model
   * @param[in] scale      Scale engine
   * @param[in] threshold  Maximum X-pixel distance to accept a hit
   * @param[in] nearestY   When true, among same-X candidates keep only the one nearest in Y
   */
  std::vector<HitResult> FindBySameX(const Vector2&     touchPos,
                                     const ChartModel&  model,
                                     const ScaleEngine& scale,
                                     float              threshold,
                                     bool               nearestY) const;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali

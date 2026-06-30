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
#include <dali/public-api/object/property-index-ranges.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for CanvasView.
 *
 * This struct is the single source of truth for all property indices.
 * It is shared between:
 *   - CanvasView::Property  (public-api, for application code)
 *   - CanvasViewImpl        (internal/canvas-view, for DALI_PROPERTY_REGISTRATION_EXTERNAL)
 */
struct CanvasViewPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000, ///< Reserve property indices.
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the CanvasView class.
   */
  enum
  {
    /**
     * @brief The viewbox of the CanvasView (ThorVG logical canvas size).
     * @details Name "viewBox", type Property::VECTOR2.
     * @note See also: CanvasView::SetViewBox(), CanvasView::GetViewBox().
     */
    VIEW_BOX = PROPERTY_START_INDEX,

    /**
     * @brief Whether to rasterize the canvas synchronously on the main thread.
     * @details Name "synchronousLoading", type Property::BOOLEAN.
     * @note Default is true.
     * @note See also: CanvasView::SetSynchronousLoading(), CanvasView::IsSynchronousLoading().
     */
    SYNCHRONOUS_LOADING,

    /**
     * @brief Whether to trigger rasterization manually via RequestRasterization().
     * @details Name "rasterizationRequestManually", type Property::BOOLEAN.
     * @note Default is false. When false, rasterization is triggered automatically
     *       whenever the canvas changes.
     * @note See also: CanvasView::SetRasterizationRequestManually(),
     *                 CanvasView::IsRasterizationRequestManually(),
     *                 CanvasView::RequestRasterization().
     */
    RASTERIZATION_REQUEST_MANUALLY,

    /**
     * @brief Internal visual slot for the rasterized canvas texture.
     * @details Name "canvasContentVisual", type Property::MAP.
     * @note Not intended for application use. Managed internally by CanvasViewImpl.
     */
    CANVAS_CONTENT_VISUAL,
  };
};

} // namespace Ui
} // namespace Dali

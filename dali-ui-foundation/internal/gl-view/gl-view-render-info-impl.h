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
#include <dali/devel-api/signals/render-callback.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
/**
 * @brief Implementation of GlViewRenderInfo.
 *
 * Owned by GlViewImpl. A direct backend points it at whatever RenderCallbackInput the
 * current callback was given, so a frame costs one pointer store and nothing is copied.
 *
 * An offscreen backend has no such input - there is no render task drawing the view, only
 * a buffer it fills - so it fills these in instead. They are written to describe the same
 * thing the direct backends report, which is what lets one set of application callbacks
 * work on either: vertices in the view's own pixel space with the origin at its centre,
 * placed by the MVP matrix.
 */
struct GlViewRenderInfoImpl
{
  const Dali::RenderCallbackInput* input{nullptr}; ///< Direct backends

  /// Offscreen backend, read only while input is null.
  Matrix        mvp{Matrix::IDENTITY};
  Matrix        projection{Matrix::IDENTITY};
  Size          size;
  BoundsInteger clippingBox;
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

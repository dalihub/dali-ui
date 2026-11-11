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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
struct GlViewRenderInfoImpl;
}

/**
 * @brief Describes the rendering context handed to a GlView GL callback.
 *
 * The application must draw into the region this object describes.
 *
 * @code
 * bool OnRenderFrame(const GlViewRenderInfo& info)
 * {
 *   // The surface and the context are the application's own, so nothing sets their
 *   // viewport for it: size it to the view.
 *   glViewport(0, 0, info.GetSize().width, info.GetSize().height);
 *
 *   // Vertices are in the view's own pixel space, origin at its centre.
 *   glUniformMatrix4fv(mMvpLocation, 1, GL_FALSE, info.GetMvpMatrix().AsFloat());
 *   ...
 *   return true;
 * }
 * @endcode
 *
 * @SINCE_3_0.0
 * @note Only valid for the duration of the callback, and deliberately not copyable so it
 *       cannot outlive it. It reads the rendering thread's own state rather than a copy,
 *       so touching it from another thread is a data race.
 */
class DALI_UI_API GlViewRenderInfo
{
public:
  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~GlViewRenderInfo();

  /**
   * @brief Not copyable.
   * @SINCE_3_0.0
   */
  GlViewRenderInfo(const GlViewRenderInfo&) = delete;

  /**
   * @brief Not copy assignable.
   * @SINCE_3_0.0
   */
  GlViewRenderInfo& operator=(const GlViewRenderInfo&) = delete;

  /**
   * @brief Returns the model-view-projection matrix for the view.
   *
   * @SINCE_3_0.0
   * @return The model-view-projection matrix
   */
  const Matrix& GetMvpMatrix() const;

  /**
   * @brief Returns the view matrix of the render task drawing the view.
   *
   * @SINCE_3_0.0
   * @return The view matrix
   */
  const Matrix& GetViewMatrix() const;

  /**
   * @brief Returns the projection matrix of the render task drawing the view.
   *
   * @SINCE_3_0.0
   * @return The projection matrix
   */
  const Matrix& GetProjectionMatrix() const;

  /**
   * @brief Returns the size of the view in its own local space, in pixels.
   *
   * This is the size the layout gave the view, without the scale or rotation applied to
   * it. Vertices expressed in this space are placed on screen by GetMvpMatrix().
   *
   * @SINCE_3_0.0
   * @return The size of the view
   *
   * @note For GlView::BackendMode::OFFSCREEN_RENDERING this is also the size of the
   *       buffer being drawn into, and therefore the viewport the application has to
   *       set - nothing else sets it, and GL leaves it at the size of whichever surface
   *       the context was first made current with.
   */
  const Size& GetSize() const;

  /**
   * @brief Returns the region the view is allowed to draw into.
   *
   * This is the view's bounds in the coordinates of the render target the callback draws
   * into, clipped to what is actually visible there, in the convention glScissor()
   * expects. Unlike GetSize() it accounts for the scale, rotation and parent transforms
   * applied to the view.
   *
   * @SINCE_3_0.0
   * @return The clipping region
   *
   * @note This is a scissor rectangle, not a viewport. Where the view sits is already
   *       carried by GetMvpMatrix(), which maps the view's local space to the viewport
   *       that is current when the callback runs - so narrowing the viewport to this
   *       rectangle would place the content twice.
   */
  const BoundsInteger& GetClippingBox() const;

  /**
   * @brief Returns the inherited world color multiplier of the View.
   *
   * Use this to honour the opacity and color multiplier inherited from the actor hierarchy.
   *
   * @SINCE_3_0.0
   * @return The inherited world color multiplier
   */
  const Vector4& GetWorldColorMultiplier() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates an object with its own implementation.
   */
  GlViewRenderInfo();

  /**
   * @brief Returns the implementation, so the owning view can point it at the frame.
   *
   * @return The implementation
   */
  Internal::GlViewRenderInfoImpl& GetImplementation();

  /**
   * @brief Returns the implementation.
   *
   * @return The implementation
   */
  const Internal::GlViewRenderInfoImpl& GetImplementation() const;
  /// @endcond

private:
  UniquePtr<Internal::GlViewRenderInfoImpl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE

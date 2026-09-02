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

namespace Dali
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
 *   // Confine drawing to the view. The viewport is left alone - the placement of the
 *   // view is already carried by the MVP matrix, which expects it unchanged.
 *   const auto& box = info.GetClippingBox();
 *   glScissor(box.x, box.y, box.width, box.height);
 *   glEnable(GL_SCISSOR_TEST);
 *
 *   // Vertices are in the view's own pixel space, origin at its centre.
 *   glUniformMatrix4fv(mMvpLocation, 1, GL_FALSE, info.GetMvpMatrix().AsFloat());
 *   ...
 *   return true;
 * }
 * @endcode
 *
 * @note Only valid for the duration of the callback, and deliberately not copyable so it
 *       cannot outlive it. It reads the rendering thread's own state rather than a copy,
 *       so touching it from another thread is a data race.
 */
class DALI_UI_API GlViewRenderInfo
{
public:
  /**
   * @brief Destructor.
   */
  ~GlViewRenderInfo();

  /**
   * @brief Not copyable.
   */
  GlViewRenderInfo(const GlViewRenderInfo&) = delete;

  /**
   * @brief Not copy assignable.
   */
  GlViewRenderInfo& operator=(const GlViewRenderInfo&) = delete;

  /**
   * @brief Returns the model-view-projection matrix for the view.
   *
   * @return The model-view-projection matrix
   */
  const Matrix& GetMvpMatrix() const;

  /**
   * @brief Returns the view matrix of the render task drawing the view.
   *
   * @return The view matrix
   */
  const Matrix& GetViewMatrix() const;

  /**
   * @brief Returns the projection matrix of the render task drawing the view.
   *
   * @return The projection matrix
   */
  const Matrix& GetProjectionMatrix() const;

  /**
   * @brief Returns the size of the view in its own local space, in pixels.
   *
   * This is the size the layout gave the view, without the scale or rotation applied to
   * it. Vertices expressed in this space are placed on screen by GetMvpMatrix().
   *
   * @return The size of the view
   */
  const Size& GetSize() const;

  /**
   * @brief Returns the region the view is allowed to draw into.
   *
   * This is the view's bounds projected to screen coordinates and clipped to what is
   * actually visible, in the convention glScissor() expects. Unlike GetSize() it accounts
   * for the scale, rotation and parent transforms applied to the view.
   *
   * @return The clipping region
   *
   * @note This is a scissor rectangle, not a viewport. Where the view sits on screen is
   *       already carried by GetMvpMatrix(), which maps the view's local space to the
   *       viewport that is current when the callback runs - so narrowing the viewport to
   *       this rectangle would place the content twice.
   */
  const BoundsInteger& GetClippingBox() const;

  /**
   * @brief Returns the inherited world color multiplier of the View.
   *
   * Use this to honour the opacity and color multiplier inherited from the actor hierarchy.
   *
   * @return The inherited world color multiplier
   */
  const Vector4& GetWorldColorMultiplier() const;

  /**
   * @brief Returns the inherited world color.
   * @deprecated Use GetWorldColorMultiplier() instead.
   * @return The inherited world color
   */
  const Vector4& GetWorldColor() const;

  /**
   * @brief Returns the native handles (GL texture names) of the bound textures.
   *
   * The handles are in the order the textures were passed to
   * GlView::BindTextureResources(), so the position in this list is the only thing
   * identifying which texture a handle belongs to.
   *
   * @return The native handles of the bound textures
   *
   * @note An entry is 0 until DALi has created the GL texture behind it, and keeps its
   *       position meanwhile. Check for 0 before using an entry.
   * @note The texture target is not reported - use the one matching what was bound,
   *       GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES.
   * @see Dali::Ui::GlView::BindTextureResources()
   */
  const Dali::Vector<uint32_t>& GetBoundTextureNativeHandles() const;

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
  /// @endcond

private:
  UniquePtr<Internal::GlViewRenderInfoImpl> mImpl;
};

} // namespace Ui
} // namespace Dali

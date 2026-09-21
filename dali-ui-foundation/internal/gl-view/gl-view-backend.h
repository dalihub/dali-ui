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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class GlViewImpl;

/**
 * @brief How a GlView gets the application's GL code onto the screen.
 *
 * The backends differ in almost everything that matters - which thread runs the
 * callbacks, what they draw into, and what has to happen for a frame to appear -
 * so they are kept apart rather than branched through. GlViewImpl holds the part
 * that is the same either way: the registered callbacks, the render info handed
 * to them, and the terminate handshake with the event thread.
 *
 * A backend reaches those through its owner, which outlives it.
 */
class GlViewBackend
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~GlViewBackend() = default;

  /**
   * @brief Attaches whatever draws the view.
   *
   * @param[in] owner The view this backend belongs to
   *
   * @note Called from GlViewImpl::OnInitialize(), on the event thread.
   */
  virtual void Initialize(GlViewImpl& owner) = 0;

  /**
   * @copydoc Dali::Ui::GlView::SetRenderingMode
   */
  virtual void SetRenderingMode(GlView::RenderingMode mode) = 0;

  /**
   * @copydoc Dali::Ui::GlView::RenderOnce
   */
  virtual void RenderOnce() = 0;

  /**
   * @copydoc Dali::Ui::GlView::BindTextureResources
   */
  virtual void BindTextureResources(Dali::Vector<Dali::Texture> textures) = 0;

  /**
   * @brief The size the layout gave the view.
   *
   * @param[in] size The new size, in pixels
   */
  virtual void OnArrange(const Size& size)
  {
  }

  /**
   * @brief Whether the view is currently visible on a visible window.
   *
   * @param[in] visible True if the view can be seen
   */
  virtual void OnVisibilityChanged(bool visible)
  {
  }

  /**
   * @brief Starts releasing the application's GL resources.
   *
   * The application's terminate callback has to run wherever its GL resources
   * were created, so this only starts the sequence.
   *
   * @return True if the backend will report back through
   *         GlViewImpl::NotifyTerminateCompleted(). False if nothing will, and
   *         the caller should complete the sequence itself.
   */
  virtual bool StartTerminate() = 0;

  /**
   * @brief Stops without invoking any callback the application registered.
   *
   * Used where the application released its last handle without terminating, and
   * where the terminate sequence has to be completed without the rendering side
   * reporting back. Either way the application's callbacks may no longer be safe
   * to call into, and whatever GL resources they created are left to the graphics
   * context.
   */
  virtual void AbandonQuietly() = 0;

  /**
   * @brief Releases what was waiting on the terminate handshake.
   *
   * @note Called on the event thread, once the sequence has completed.
   */
  virtual void OnTerminateCompleted()
  {
  }

protected:
  GlViewBackend()                                = default;
  GlViewBackend(const GlViewBackend&)            = delete;
  GlViewBackend& operator=(const GlViewBackend&) = delete;
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE

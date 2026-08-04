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
#include <dali-ui-foundation/public-api/views/canvas/canvas-view-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class CanvasViewImpl;
}

/**
 * @brief CanvasView is a View for displaying vector primitives via ThorVG.
 *
 * Drawables (shapes, groups, pictures) are added with AddDrawable() and rendered
 * asynchronously by default.  The viewbox defines the logical coordinate space;
 * the view pixel size is controlled by the layout system.
 *
 * @code
 * using namespace Dali::Ui;
 *
 * CanvasView canvasView = CanvasView::New(Vector2(200.f, 200.f));
 *
 * auto shape = Dali::CanvasRenderer::Shape::New();
 * shape.AddRect(0, 0, 100, 100, 0, 0);
 * shape.SetFillColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
 * canvasView.AddDrawable(shape);
 * @endcode
 */
class DALI_UI_API CanvasView : public View
{
public:
  /**
   * @brief Property indices for CanvasView.
   */
  struct Property
  {
    enum
    {
      VIEW_BOX                       = CanvasViewPropertyIndex::VIEW_BOX,
      SYNCHRONOUS_LOADING            = CanvasViewPropertyIndex::SYNCHRONOUS_LOADING,
      RASTERIZATION_REQUEST_MANUALLY = CanvasViewPropertyIndex::RASTERIZATION_REQUEST_MANUALLY,
    };
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized CanvasView handle.
   */
  CanvasView();

  /**
   * @brief Creates an initialized CanvasView with a zero viewbox.
   *
   * @return A handle to a newly allocated CanvasView
   */
  static CanvasView New();

  /**
   * @brief Creates an initialized CanvasView with the given viewbox.
   *
   * @param[in] viewBox The logical width and height of the canvas coordinate space
   * @return A handle to a newly allocated CanvasView
   */
  static CanvasView New(const Vector2& viewBox);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~CanvasView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] canvasView Handle to copy
   */
  CanvasView(const CanvasView& canvasView);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  CanvasView(CanvasView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] canvasView Object to assign this to
   * @return Reference to this
   */
  CanvasView& operator=(const CanvasView& canvasView);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  CanvasView& operator=(CanvasView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(CanvasView)

public: // Static methods
  /**
   * @brief Downcasts a handle to CanvasView handle.
   *
   * If handle points to a CanvasView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a CanvasView or an uninitialized handle
   */
  static CanvasView DownCast(BaseHandle handle);

public: // Drawable management
  /**
   * @brief Adds a drawable object to the canvas.
   *
   * The drawable is rendered on the next rasterization pass.
   *
   * @param[in] drawable The drawable to add
   * @return True if the drawable was successfully added, false otherwise
   */
  bool AddDrawable(Dali::CanvasRenderer::Drawable drawable);

  /**
   * @brief Removes a drawable object from the canvas.
   *
   * @param[in] drawable The drawable to remove
   * @return True if the drawable was found and removed, false otherwise
   */
  bool RemoveDrawable(Dali::CanvasRenderer::Drawable drawable);

  /**
   * @brief Removes all drawable objects from the canvas.
   *
   * @return True if all drawables were successfully removed, false otherwise
   */
  bool RemoveAllDrawables();

  /**
   * @brief Applies a drop shadow to the whole canvas content.
   *
   * Backed by the ThorVG scene effect (tvg::SceneEffect::DropShadow).
   *
   * @param[in] color      Shadow color (RGBA, each channel 0.0 ~ 1.0)
   * @param[in] offsetX    Shadow X offset in pixels (positive X is right)
   * @param[in] offsetY    Shadow Y offset in pixels (positive Y is down)
   * @param[in] blurRadius Blur radius in pixels (Gaussian sigma = blurRadius * 0.5)
   *
   * @note ThorVG supports isotropic blur only, so separate X/Y blur radii are not supported.
   *       The shadow is clipped to the canvas buffer (the layout size) unless effect
   *       auto-padding is enabled (see SetEffectAutoPaddingEnabled()).
   */
  void SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius);

  /**
   * @brief Removes the drop shadow set by SetDropShadow().
   */
  void ClearDropShadow();

  /**
   * @brief Returns whether a drop shadow is currently set.
   *
   * @return True if a drop shadow is set, false otherwise
   */
  bool HasDropShadow() const;

  /**
   * @brief Applies a Gaussian blur to the whole canvas content.
   *
   * Scene effects are mutually exclusive: setting a Gaussian blur replaces a drop shadow and
   * vice versa.
   *
   * @param[in] blurRadius Blur radius in pixels (internally converted to sigma = blurRadius * 0.5)
   */
  void SetGaussianBlur(float blurRadius);

  /**
   * @brief Removes the Gaussian blur set by SetGaussianBlur().
   */
  void ClearGaussianBlur();

  /**
   * @brief Returns whether a Gaussian blur is currently set.
   *
   * @return True if a Gaussian blur is set, false otherwise
   */
  bool HasGaussianBlur() const;

  /**
   * @brief Enables or disables automatic padding for scene effects (enabled by default).
   *
   * When enabled, the shadow/blur is not clipped at the canvas edges; the content is rendered into
   * a larger buffer and scaled back into the view, so the content appears slightly smaller. When
   * disabled, size the CanvasView larger than its content to leave room for the effect.
   *
   * @param[in] enable True to auto-pad, false otherwise
   */
  void SetEffectAutoPaddingEnabled(bool enable);

  /**
   * @brief Returns whether automatic effect padding is enabled.
   *
   * @return True if auto-padding is enabled, false otherwise
   */
  bool IsEffectAutoPaddingEnabled() const;

public: // Rasterization control
  /**
   * @brief Sets the logical coordinate space (viewbox) of the canvas.
   *
   * @param[in] viewBox The new width and height of the viewbox
   * @return Reference to this for fluent chaining
   */
  void SetViewBox(const Vector2& viewBox);

  /**
   * @brief Gets the current viewbox.
   *
   * @return The current viewbox size
   */
  Vector2 GetViewBox();

  /**
   * @brief Sets whether rasterization is performed synchronously on the main thread.
   *
   * @param[in] synchronous True to rasterize synchronously, false for background thread
   * @return Reference to this for fluent chaining
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets whether synchronous rasterization is enabled.
   *
   * @return True if rasterization is synchronous
   */
  bool IsSynchronousLoading() const;

  /**
   * @brief Sets whether rasterization must be triggered manually.
   *
   * When true, rasterization only happens when RequestRasterization() is called.
   * When false (default), rasterization is triggered automatically on canvas changes.
   *
   * @param[in] manually True to require explicit RequestRasterization() calls
   * @return Reference to this for fluent chaining
   */
  void SetRasterizationRequestManually(bool manually);

  /**
   * @brief Gets whether manual rasterization mode is active.
   *
   * @return True if rasterization must be triggered manually
   */
  bool IsRasterizationRequestManually() const;

  /**
   * @brief Requests an immediate rasterization pass on the next processor cycle.
   *
   * In automatic mode (default), rasterization is already triggered whenever
   * the canvas changes, so explicit calls are rarely needed.
   * In manual mode (RASTERIZATION_REQUEST_MANUALLY = true), this is the
   * only way to trigger a rasterize; call it after finishing all drawable updates.
   */
  void RequestRasterization();

public: // Not intended for application developers
  /// @cond internal
  explicit CanvasView(Internal::CanvasViewImpl& implementation);
  explicit CanvasView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} // namespace Dali

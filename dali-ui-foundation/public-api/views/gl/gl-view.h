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
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/gl/gl-view-offscreen-config.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view-render-info.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class GlViewImpl;
}

/**
 * @brief GlView is a View that renders its content with the application's own GL code.
 *
 * The application registers three callbacks - one to create GL resources, one to render a frame,
 * and one to release the resources - and GlView invokes them on a rendering thread.
 * The BackendMode chosen at construction decides how that GL code reaches the screen.
 *
 * @code
 * using namespace Dali;
 * using namespace Dali::Ui;
 *
 * GlView glView = GlView::New();
 * glView.RegisterGlCallbacks(Dali::Callback<void(const GlViewRenderInfo&)>::New(this, &MyApp::OnGlInit),
 *                            Dali::Callback<bool(const GlViewRenderInfo&)>::New(this, &MyApp::OnGlRenderFrame),
 *                            Dali::Callback<void()>::New(this, &MyApp::OnGlTerminate));
 * parent.Add(glView);
 * @endcode
 *
 * @SINCE_3_0.0
 * @note The callbacks run on a rendering thread, not the event thread. Do not call DALi APIs from inside them.
 */
class DALI_UI_API GlView : public View
{
public: // Types
  /**
   * @brief Determines how GlView executes the application's GL code.
   * @SINCE_3_0.0
   */
  enum class BackendMode
  {
    /**
     * @brief Executes on a rendering thread of its own, into an offscreen buffer.
     *
     * The callback runs whenever that thread is ready rather than at a point in
     * the DALi frame, so the application's GL code neither waits for the UI nor
     * holds it up. What it draws becomes a texture, which DALi then composites
     * like any other content - so it is a frame behind the UI around it, and
     * costs the memory of the buffers it is drawn into.
     *
     * The whole context is the application's own, with no state to preserve and
     * no context switch per frame. That cuts both ways: nothing is set up for
     * the application either, so it must set the viewport itself. The size to
     * set it to is GlViewRenderInfo::GetSize().
     *
     * @SINCE_3_0.0
     * @note The buffer is described by the GlViewOffscreenConfig given to New().
     * @note GlViewRenderInfo describes the buffer rather than the screen, so its
     *       clipping box is the whole buffer: the view's own placement is
     *       applied by DALi when it composites what was drawn.
     */
    OFFSCREEN_RENDERING,
  };

  /**
   * @brief Determines when GlView asks the application to render.
   * @SINCE_3_0.0
   */
  enum class RenderingMode
  {
    CONTINUOUS, ///< The render frame callback is invoked every frame. @SINCE_3_0.0
    ON_DEMAND,  ///< The render frame callback is invoked only when GlView::RenderOnce() is called. @SINCE_3_0.0
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized GlView handle.
   * @SINCE_3_0.0
   */
  GlView();

  /**
   * @brief Creates an initialized GlView using the given backend.
   *
   * @SINCE_3_0.0
   * @param[in] backendMode The backend used to execute the application's GL code
   * @param[in] offscreenConfig Describes the offscreen buffer. Only read for
   *                            BackendMode::OFFSCREEN_RENDERING
   * @return A handle to a newly allocated GlView
   */
  static GlView New(BackendMode                  backendMode     = BackendMode::OFFSCREEN_RENDERING,
                    const GlViewOffscreenConfig& offscreenConfig = {});

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_3_0.0
   */
  ~GlView();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_3_0.0
   * @param[in] glView Handle to copy. The copy will point at the same implementation
   */
  GlView(const GlView& glView);

  /**
   * @brief Move constructor.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Handle to move
   */
  GlView(GlView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] glView Object to assign this to
   * @return Reference to this
   */
  GlView& operator=(const GlView& glView);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  GlView& operator=(GlView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(GlView)

public: // Static methods
  /**
   * @brief Downcasts a handle to a GlView handle.
   *
   * If handle points to a GlView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a GlView or an uninitialized handle
   */
  static GlView DownCast(BaseHandle handle);

public: // Callback registration
  /**
   * @brief Registers the GL callbacks.
   *
   * @SINCE_3_0.0
   * @param[in] initCallback        Creates GL resources. Invoked once, immediately before
   *                                the first render frame callback.
   * @param[in] renderFrameCallback Renders one frame. Return true if new content was
   *                                produced. Backends that need an explicit present step
   *                                will present it; others ignore the value, so return
   *                                it accurately regardless of the current backend.
   * @param[in] terminateCallback   Releases GL resources. Invoked at most once, as part of
   *                                the Terminate() sequence rather than during the call
   *                                itself. Skipped when there is nothing it could release
   *                                - the init callback never ran - or when the render
   *                                target it was drawn into has been destroyed, leaving
   *                                nothing to issue GL calls against. Use the Terminate()
   *                                completion callback for anything that has to run
   *                                unconditionally.
   *
   * @note Register before adding the view to the scene. The rendering thread reads these
   *       once the view starts being drawn, so registering after that races with it.
   * @note The callbacks run on a rendering thread, never the event thread. Which thread
   *       depends on the backend. Do not call DALi APIs from them.
   */
  void RegisterGlCallbacks(Callback<void(const GlViewRenderInfo&)> initCallback,
                           Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                           Callback<void()>                        terminateCallback);

public: // Rendering control
  /**
   * @brief Sets when the render frame callback is invoked.
   *
   * @SINCE_3_0.0
   * @param[in] mode The rendering mode to use
   *
   * @note The default is RenderingMode::CONTINUOUS.
   */
  void SetRenderingMode(RenderingMode mode);

  /**
   * @brief Returns when the render frame callback is invoked.
   *
   * @SINCE_3_0.0
   * @return The current rendering mode
   */
  RenderingMode GetRenderingMode() const;

  /**
   * @brief Requests one frame while in RenderingMode::ON_DEMAND.
   *
   * @SINCE_3_0.0
   * @note Has no effect while the window is hidden, as no frames are produced then.
   */
  void RenderOnce();

  /**
   * @brief Returns the backend this view was created with.
   *
   * @SINCE_3_0.0
   * @return The backend mode
   */
  BackendMode GetBackendMode() const;

  /**
   * @brief Starts releasing the GL resources. Idempotent and irreversible.
   *
   * Releasing GL resources means running client code on the rendering thread, which
   * cannot happen while this call is on the stack, so the request completes
   * asynchronously:
   *
   *   1. The render frame callback stops being invoked.
   *   2. The terminate callback runs on the rendering thread, in the context the GL
   *      resources were created in.
   *   3. @p onTerminated is invoked on the event thread. Nothing registered on this view
   *      is invoked afterwards.
   *
   * Everything the registered callbacks point at has to stay alive until @p onTerminated
   * is invoked. There is no need to keep a handle to the view itself - it holds a
   * reference to itself for the duration - but it is not revived by doing so either, so
   * replacing a terminated view means keeping the retired callback target around rather
   * than reusing it.
   *
   * @SINCE_3_0.0
   * @param[in] onTerminated Invoked on the event thread once the sequence is complete
   *
   * @note This must be called explicitly. It is deliberately not called on scene
   *       disconnection, because a view may be removed from the scene and added back
   *       later - for instance when recycled by a RecyclerView - and a terminated GlView
   *       cannot be revived.
   * @note @p onTerminated is invoked even when the terminate callback could not be - see
   *       the terminate callback conditions on RegisterGlCallbacks(). It is the reliable
   *       point to release client-side state; the terminate callback is only for GL.
   * @note @p onTerminated is normally invoked on a later event, but can be invoked before
   *       this call returns when there is no rendering side to hear back from - the view
   *       was never shown, so its rendering thread never started, or the Adaptor has
   *       already shut down. The terminate callback is not invoked at all then.
   * @note Where the rendering side never reports back, the sequence is completed when the
   *       application shuts down, so @p onTerminated arrives late rather than not at all -
   *       that covers a request the application then shuts down on top of.
   * @note Destroying the last handle without calling this does not terminate the view.
   *       No callback registered here or on RegisterGlCallbacks() is invoked in that
   *       case, so no client code runs against a view the application has let go of, and
   *       whatever GL resources the callbacks created are left to the graphics context.
   */
  void Terminate(Callback<void()> onTerminated = {});

public: // Not intended for application developers
  /// @cond internal
  explicit GlView(Internal::GlViewImpl& implementation);
  explicit GlView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE

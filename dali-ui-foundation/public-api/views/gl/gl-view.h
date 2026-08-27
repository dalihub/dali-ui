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
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view-render-info.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
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
 * GlView glView = GlView::New(GlView::BackendMode::UNSAFE_DIRECT_RENDERING);
 * glView.RegisterGlCallbacks(Ui::Callback<void(const GlViewRenderInfo&)>::New(this, &MyApp::OnGlInit),
 *                            Ui::Callback<bool(const GlViewRenderInfo&)>::New(this, &MyApp::OnGlRenderFrame),
 *                            Ui::Callback<void()>::New(this, &MyApp::OnGlTerminate));
 * parent.Add(glView);
 * @endcode
 *
 * @note The callbacks run on a rendering thread, not the event thread. Do not call DALi APIs from inside them.
 */
class DALI_UI_API GlView : public View
{
public: // Types
  /**
   * @brief Determines how GlView executes the application's GL code.
   */
  enum class BackendMode
  {
    /**
     * @brief Executes on the DALi render thread using an isolated EGL context.
     *
     * The callback runs at the point in the frame where the view would be drawn, so its
     * output composites with the DALi content around it, and it draws straight into the
     * window surface rather than into an offscreen buffer.
     *
     * The application's GL code cannot corrupt DALi's render state, at the cost of two EGL
     * context switches every frame.
     *
     * The context shares resources with DALi's, so DALi's textures and buffers are
     * reachable, but context-local objects such as vertex array objects and framebuffer
     * objects are not.
     */
    DIRECT_RENDERING,

    /**
     * @brief Executes on the DALi render thread using DALi's window context directly.
     *
     * The callback runs at the point in the frame where the view would be drawn, so its
     * output composites with the DALi content around it, and it draws straight into the
     * window surface rather than into an offscreen buffer.
     *
     * There is no context switch, and the whole main context is available including
     * context-local objects. The cost is that restoring GL state is entirely the
     * application's responsibility - failing to restore it corrupts the rest of the
     * DALi rendering in the same frame.
     *
     * @warning The framework resets a baseline GL state before and after the callback,
     *          but the following are NOT covered and must be restored by the application:
     *          viewport and scissor box values, pixel store alignment, the bound program,
     *          and the bound framebuffer.
     */
    UNSAFE_DIRECT_RENDERING,
  };

  /**
   * @brief Determines when GlView asks the application to render.
   */
  enum class RenderingMode
  {
    CONTINUOUS, ///< The render frame callback is invoked every frame.
    ON_DEMAND,  ///< The render frame callback is invoked only when GlView::RenderOnce() is called.
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized GlView handle.
   */
  GlView();

  /**
   * @brief Creates an initialized GlView using the given backend.
   *
   * @param[in] backendMode The backend used to execute the application's GL code
   * @return A handle to a newly allocated GlView
   */
  static GlView New(BackendMode backendMode);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GlView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] glView Handle to copy. The copy will point at the same implementation
   */
  GlView(const GlView& glView);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  GlView(GlView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] glView Object to assign this to
   * @return Reference to this
   */
  GlView& operator=(const GlView& glView);

  /**
   * @brief Move assignment operator.
   *
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
   * @param[in] handle Handle to an object
   * @return A handle to a GlView or an uninitialized handle
   */
  static GlView DownCast(BaseHandle handle);

public: // Callback registration
  /**
   * @brief Registers the GL callbacks.
   *
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
   * @note The callbacks run on a rendering thread. Which thread depends on the backend -
   *       direct rendering backends run them on the DALi render thread, so blocking
   *       inside a callback stalls the whole UI. Do not call DALi APIs from them.
   */
  void RegisterGlCallbacks(Callback<void(const GlViewRenderInfo&)> initCallback,
                           Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                           Callback<void()>                        terminateCallback);

  /**
   * @brief Makes DALi textures reachable from the GL callbacks.
   *
   * The GL callbacks run on a rendering thread that must not be blocked, so decoding and
   * uploading an image from inside them is not an option. Binding a DALi texture instead
   * lets the application reuse DALi's own asynchronous loading pipeline and pick the
   * result up as a native handle.
   *
   * Read the handles back with GlViewRenderInfo::GetBoundTextureNativeHandles(). The
   * position of a texture in @p textures is the only thing identifying it there, so keep
   * the order stable.
   *
   * @param[in] textures The textures to bind
   *
   * @note May be called at any time, including while the view is rendering, which is
   *       where an asynchronous load usually finishes.
   * @note The bound textures are kept alive for as long as they stay bound. Binding a
   *       different list releases the previous one.
   */
  void BindTextureResources(Dali::Vector<Dali::Texture> textures);

public: // Rendering control
  /**
   * @brief Sets when the render frame callback is invoked.
   *
   * @param[in] mode The rendering mode to use
   *
   * @note The default is RenderingMode::CONTINUOUS.
   */
  void SetRenderingMode(RenderingMode mode);

  /**
   * @brief Returns when the render frame callback is invoked.
   *
   * @return The current rendering mode
   */
  RenderingMode GetRenderingMode() const;

  /**
   * @brief Requests one frame while in RenderingMode::ON_DEMAND.
   *
   * @note Direct rendering backends have no rendering thread of their own, so this drives
   *       a whole DALi frame - the entire scene is redrawn, not just this view.
   * @note Has no effect while the window is hidden, as no frames are produced then.
   */
  void RenderOnce();

  /**
   * @brief Returns the backend this view was created with.
   *
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
   * @param[in] onTerminated Invoked on the event thread once the sequence is complete
   *
   * @note This must be called explicitly. It is deliberately not called on scene
   *       disconnection, because a view may be removed from the scene and added back
   *       later - for instance when recycled by a RecyclerView - and a terminated GlView
   *       cannot be revived.
   * @note @p onTerminated is invoked even when the terminate callback could not be - see
   *       the terminate callback conditions on RegisterGlCallbacks(). It is the reliable
   *       point to release client-side state; the terminate callback is only for GL.
   * @note @p onTerminated is not invoked at all where the graphics backend refuses to run
   *       native rendering - currently when the view is only ever drawn into an offscreen
   *       target, or where the backend has no separate surface context. None of the
   *       registered callbacks are invoked in those configurations either, so the view
   *       draws nothing to begin with.
   * @note @p onTerminated is normally invoked on a later event, but is invoked before this
   *       call returns if the Adaptor has already shut down - there is no rendering thread
   *       left to hear back from then, and the terminate callback is not invoked at all.
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
} // namespace Dali

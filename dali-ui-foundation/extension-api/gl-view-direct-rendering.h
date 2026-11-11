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
#include <dali-ui-foundation/public-api/views/gl/gl-view-render-info.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Extension
{
/**
 * @brief GlView backends that run the application's GL code inside the DALi frame.
 *
 * The public backend, GlView::BackendMode::OFFSCREEN_RENDERING, keeps the application's
 * GL code on a thread and a context of its own and composites what it drew as a texture.
 * The two backends here instead run it on the DALi render thread, at the point in the
 * frame where the view would be drawn, so its output composites with the DALi content
 * around it and goes straight into the window surface - no offscreen buffers to pay for,
 * and no frame of latency.
 *
 * Pass one of these values straight to GlView::New():
 * @code
 * GlView glView = GlView::New(Extension::GlViewDirectRendering::DIRECT_RENDERING);
 * @endcode
 *
 * Unlike the offscreen backend, the viewport is inherited from DALi and must be left
 * alone - the placement of the view is already carried by the MVP matrix, which expects
 * it unchanged. Drawing is confined to the view with the scissor box instead:
 * @code
 * bool OnRenderFrame(const GlViewRenderInfo& info)
 * {
 *   const auto& box = info.GetClippingBox();
 *   glScissor(box.x, box.y, box.width, box.height);
 *   glEnable(GL_SCISSOR_TEST);
 *   ...
 *   return true;
 * }
 * @endcode
 *
 * These backends are extension API rather than public because a mistake no longer stays
 * inside the view: the callbacks run on the DALi render thread, so blocking inside one
 * stalls the whole UI, and with UNSAFE_DIRECT_RENDERING an unrestored piece of GL state
 * corrupts the rest of the frame.
 *
 * @SINCE_3_0.1
 * @note They are also implemented on the graphics backend's native-rendering hook, which
 *       not every configuration provides - currently a view that is only ever drawn into
 *       an offscreen target, or a graphics backend with no separate surface context. None
 *       of the registered callbacks are invoked there, the view draws nothing, and a
 *       GlView::Terminate() request is completed only when the application shuts down.
 * @note GlView::Terminate() completes through the rendering side, which guarantees the
 *       handover even for a view that was never shown - @p onTerminated then arrives on a
 *       later event rather than before the call returns, unlike the public backend.
 */
namespace GlViewDirectRendering
{
// The public enumerators count up from 0, so these are numbered well clear of them: the
// public enum can grow without ever colliding, and these values never have to move.

/**
 * @brief Executes on the DALi render thread using an isolated EGL context.
 *
 * The application's GL code cannot corrupt DALi's render state, at the cost of two EGL
 * context switches every frame.
 *
 * The context shares resources with DALi's, so DALi's textures and buffers are
 * reachable, but context-local objects such as vertex array objects and framebuffer
 * objects are not.
 * @SINCE_3_0.1
 */
inline constexpr GlView::BackendMode DIRECT_RENDERING = static_cast<GlView::BackendMode>(1000);

/**
 * @brief Executes on the DALi render thread using DALi's window context directly.
 *
 * There is no context switch, and the whole main context is available including
 * context-local objects. The cost is that restoring GL state is entirely the
 * application's responsibility - failing to restore it corrupts the rest of the
 * DALi rendering in the same frame.
 *
 * @SINCE_3_0.1
 * @warning The framework resets a baseline GL state before and after the callback,
 *          but the following are NOT covered and must be restored by the application:
 *          viewport and scissor box values, pixel store alignment, the bound program,
 *          and the bound framebuffer.
 */
inline constexpr GlView::BackendMode UNSAFE_DIRECT_RENDERING = static_cast<GlView::BackendMode>(1001);

/**
 * @brief Makes DALi textures reachable from the GL callbacks.
 *
 * The GL callbacks run on a rendering thread that must not be blocked, so decoding and
 * uploading an image from inside them is not an option. Binding a DALi texture instead
 * lets the application reuse DALi's own asynchronous loading pipeline and pick the
 * result up as a native handle.
 *
 * Read the handles back with GetBoundTextureNativeHandles(). The position of a texture
 * in @p textures is the only thing identifying it there, so keep the order stable.
 *
 * @SINCE_3_0.1
 * @param[in] glView The view whose GL callbacks the textures are for
 * @param[in] textures The textures to bind
 *
 * @note May be called at any time, including while the view is rendering, which is
 *       where an asynchronous load usually finishes.
 * @note The bound textures are kept alive for as long as they stay bound. Binding a
 *       different list releases the previous one.
 * @note Only the backends in this namespace can do this. The public
 *       GlView::BackendMode::OFFSCREEN_RENDERING runs on a context of its own that
 *       shares nothing with DALi's, so DALi's textures have no name there to hand
 *       over, and this call is ignored.
 */
DALI_UI_API void BindTextureResources(GlView glView, Dali::Vector<Dali::Texture> textures);

/**
 * @brief Returns the native handles (GL texture names) of the bound textures.
 *
 * The handles are in the order the textures were passed to BindTextureResources(), so
 * the position in this list is the only thing identifying which texture a handle
 * belongs to.
 *
 * @SINCE_3_0.1
 * @param[in] renderInfo The rendering context handed to the GL callback
 * @return The native handles of the bound textures
 *
 * @note An entry is 0 until DALi has created the GL texture behind it, and keeps its
 *       position meanwhile. Check for 0 before using an entry.
 * @note The texture target is not reported - use the one matching what was bound,
 *       GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES.
 * @note Empty on a view created with the public backend, which has no bound textures
 *       to report.
 * @see BindTextureResources()
 */
DALI_UI_API const Dali::Vector<uint32_t>& GetBoundTextureNativeHandles(const GlViewRenderInfo& renderInfo);

} // namespace GlViewDirectRendering
} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE

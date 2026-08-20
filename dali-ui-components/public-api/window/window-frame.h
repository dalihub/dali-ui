#pragma once

/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/window/window-frame-command.h>
#include <dali-ui-components/public-api/window/window-frame-decoration.h>
#include <dali-ui-components/public-api/window/window-frame-geometry.h>
#include <dali-ui-components/public-api/window/window-frame-interaction.h>
#include <dali-ui-components/public-api/window/window-frame-options.h>
#include <dali-ui-components/public-api/window/window-frame-style.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{
namespace Internal DALI_INTERNAL
{
class WindowFrameImpl;
}

/**
 * @brief Determines how interactive resize requests are handled.
 */
enum class WindowFrameInteractiveResizePolicy
{
  FREE              = 0, ///< Allows free interactive resize
  DISABLED          = 1, ///< Disables interactive resize
  KEEP_ASPECT_RATIO = 2  ///< Preserves native surface aspect ratio during interactive resize
};

/**
 * @brief Determines which size a single frame-changing call preserves.
 *
 * This policy is not stored. SetDecoration(), SetDecorationLayout(), and
 * SetFrameStyle() apply the policy passed to that call only. A later call uses
 * its own policy independently.
 *
 * KEEP_CONTENT_SIZE requests a native resize in the normal state so the
 * application content keeps its previous size. KEEP_WINDOW_SIZE leaves the
 * native window surface size unchanged, so frame or content bounds may change.
 * Maximized geometry remains compositor-owned for both policies.
 */
enum class WindowFrameSizePolicy
{
  KEEP_CONTENT_SIZE = 0,
  KEEP_WINDOW_SIZE  = 1
};

/**
 * @brief Adds a client-rendered frame and window controls to a DALi window.
 *
 * Application content is added to GetContentRoot(). WindowFrame owns the
 * frame layout, so applications do not subtract decoration or shadow insets.
 * Decoration views can be replaced while attached without replacing the
 * content root.
 *
 * Five calls change the frame, and the narrower ones keep the installed
 * decoration views alive:
 *
 * - SetDecoration() with a style replaces views, layout and style.
 * - SetDecoration() without one replaces views and layout, keeping style.
 * - SetDecorationLayout() replaces layout only.
 * - SetFrameStyle() replaces style only.
 * - ClearDecoration() empties the decoration, keeping the window size.
 *
 * Only the SetDecoration() forms take views, so only they validate topology
 * and return a result worth checking.
 *
 * DefaultWindowDecoration builds and installs a complete decoration in one
 * call, so use the lower-level APIs only when a product needs its own.
 *
 * CommandInterceptor and every signal pass the originating WindowFrame as
 * their first argument. This allows one observer or policy object to serve
 * multiple frames.
 */
class DALI_UI_COMPONENTS_API WindowFrame : public Dali::BaseHandle
{
public:
  /**
   * @brief State reported by the window system for the underlying window.
   */
  enum class WindowState
  {
    NORMAL    = 0, ///< Window is neither maximized nor minimized
    MAXIMIZED = 1, ///< Window occupies its compositor-defined maximized bounds
    MINIMIZED = 2  ///< Window is minimized and has no presented frame
  };

  using CloseCallback = Dali::Ui::Callback<void()>;

  /**
   * @brief Callback that can replace WindowFrame's default command handling.
   *
   * WindowFrame invokes this callback before dispatching its default native
   * operation. The callback can observe the request or take it over entirely.
   */
  using CommandInterceptor                    = Dali::Ui::Callback<WindowFrameCommandDisposition(WindowFrame, const WindowFrameCommandRequest&)>;
  using WindowStateChangedSignalType          = Dali::Signal<void(WindowFrame, WindowState)>;
  using WindowStatePresentedSignalType        = Dali::Signal<void(WindowFrame, WindowState)>;
  using GeometryChangedSignalType             = Dali::Signal<void(WindowFrame, const WindowFrameGeometry&)>;
  using GeometryChangeCompletedSignalType     = Dali::Signal<void(WindowFrame, const WindowFrameGeometry&)>;
  using DecorationVisibilityChangedSignalType = Dali::Signal<void(WindowFrame, bool)>;
  using InteractionChangedSignalType          = Dali::Signal<void(WindowFrame, const WindowFrameInteraction&)>;
  using CommandProcessedSignalType            = Dali::Signal<void(WindowFrame, const WindowFrameCommandRequest&, WindowFrameCommandResult)>;

  /**
   * @brief Creates an uninitialized WindowFrame handle.
   */
  WindowFrame();
  ~WindowFrame();
  WindowFrame(const WindowFrame& rhs);
  WindowFrame(WindowFrame&& rhs) noexcept;
  WindowFrame& operator=(const WindowFrame& rhs);
  WindowFrame& operator=(WindowFrame&& rhs) noexcept;

  /**
   * @brief Creates a WindowFrame with default options for an existing window.
   * @param[in] window Window whose surface will host the frame and content
   * @param[in] closeCallback Callback used by the default close command
   * @return An initialized handle, or an uninitialized handle when window is empty
   */
  static WindowFrame New(Dali::Window window, CloseCallback closeCallback);

  /**
   * @brief Creates a WindowFrame with initial options for an existing window.
   * @param[in] window Window whose surface will host the frame and content
   * @param[in] closeCallback Callback used by the default close command
   * @param[in] options Initial state and feature options
   * @return An initialized handle, or an uninitialized handle when window is empty
   */
  static WindowFrame New(Dali::Window window, CloseCallback closeCallback, const WindowFrameOptions& options);

  /**
   * @brief Downcasts a handle to WindowFrame.
   * @param[in] handle Handle to an object
   * @return A WindowFrame handle, or an uninitialized handle on failure
   */
  static WindowFrame DownCast(Dali::BaseHandle handle);

  /**
   * @brief Attaches the persistent frame view tree to the window.
   *
   * A window can have only one attached WindowFrame. Calling this again on the
   * same WindowFrame succeeds without changing anything.
   * @return true when attached, or false when another WindowFrame owns the window
   */
  bool Attach();

  /**
   * @brief Unparents the frame view tree and disconnects native callbacks.
   *
   * Content and custom decoration remain owned by this WindowFrame and are
   * restored by the next Attach().
   */
  void Detach();

  /**
   * @brief Returns whether the frame is currently attached.
   * @return true when the frame view tree and native callbacks are attached
   */
  bool IsAttached() const;

  /**
   * @brief Returns the layout-managed root for application content.
   *
   * The returned root is available before Attach() and remains stable across
   * decoration changes, Detach(), and subsequent Attach() calls. It becomes
   * invalid only after the last WindowFrame handle is destroyed.
   * @return Persistent application content root
   */
  Dali::Ui::View GetContentRoot() const;

  /**
   * @brief Returns the effective setting for a window feature.
   *
   * This is the value the caller configured, not a runtime backend capability
   * query. See WindowFrameFeature.
   * @param[in] feature Feature to query
   * @return true when the feature is enabled
   */
  bool IsFeatureEnabled(WindowFrameFeature feature) const;

  /**
   * @brief Returns the compositor-confirmed window state.
   * @return Current window state
   */
  WindowState GetWindowState() const;

  /**
   * @brief Returns whether the compositor confirmed the maximized state.
   * @return true when maximized
   */
  bool IsMaximized() const;

  /**
   * @brief Returns whether the compositor confirmed the minimized state.
   * @return true when minimized
   */
  bool IsMinimized() const;

  /**
   * @brief Returns current surface, frame, and content geometry.
   * @return Current geometry
   */
  WindowFrameGeometry GetGeometry() const;

  /**
   * @brief Installs custom decoration views and their layout configuration.
   *
   * Slot views must be unparented, or already installed in this WindowFrame
   * so that the installed decoration can be resubmitted with new layout or
   * style. Role views must belong to a supplied slot. Invalid topology is
   * rejected without changing the current frame.
   * @param[in] decoration Slot roots and command roles
   * @param[in] layout Decoration layout configuration
   * @param[in] sizePolicy Size preservation policy for this installation only
   * @return Installation or validation result
   */
  WindowFrameDecorationResult SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);

  /**
   * @brief Atomically installs custom decoration, layout, and frame style.
   * @param[in] decoration Slot roots and command roles
   * @param[in] layout Decoration layout configuration
   * @param[in] style Frame and shadow style
   * @param[in] sizePolicy Size preservation policy for this installation only
   * @return Installation or validation result
   */
  WindowFrameDecorationResult SetDecoration(const WindowFrameDecoration& decoration, const WindowFrameLayout& layout, const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);

  /**
   * @brief Updates decoration layout without replacing decoration views.
   *
   * Use this to change decoration insets or resize-handle sizes while keeping
   * the installed slot views, their contents, and any signal connections the
   * application made to them.
   *
   * The size policy applies only to this layout update. For example, a
   * KEEP_CONTENT_SIZE layout update can resize the window to preserve content;
   * a following SetFrameStyle(..., KEEP_WINDOW_SIZE) then keeps that current
   * window size and may change the frame or content bounds through new shadow
   * outsets.
   * @param[in] layout New decoration layout configuration
   * @param[in] sizePolicy Size preservation policy for this update only
   */
  void SetDecorationLayout(const WindowFrameLayout& layout, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);

  /**
   * @brief Removes custom decoration while preserving the content root.
   *
   * The window size is preserved, so the content root grows into the space the
   * decoration occupied.
   */
  void ClearDecoration();

  /**
   * @brief Sets requested decoration visibility.
   *
   * In active overlay mode, auto-hide can temporarily suppress decoration even
   * while visibility is requested.
   *
   * @param[in] visible true to request visible decoration
   */
  void SetDecorationVisible(bool visible);

  /**
   * @brief Returns effective decoration visibility.
   * @return true when decoration is currently visible after overlay resolution
   */
  bool IsDecorationVisible() const;

  /**
   * @brief Sets frame colour, corner radius and shadow.
   *
   * Keeps the installed decoration views and layout, so use this rather than
   * SetDecoration() when only the look changes.
   * @param[in] style New style
   * @param[in] sizePolicy Size preservation policy for this update only
   */
  void SetFrameStyle(const WindowFrameStyle& style, WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE);

  /**
   * @brief Returns a copy of the configured frame style.
   * @return Frame style
   */
  WindowFrameStyle GetFrameStyle() const;

  /**
   * @brief Enables overlay layout while maximized.
   *
   * In overlay mode the content bounds remain the full maximized frame while
   * decoration can auto-hide independently.
   * @param[in] enabled true to enable
   */
  void SetOverlayEnabled(bool enabled);

  /**
   * @brief Returns whether maximized overlay layout is enabled.
   * @return true when enabled
   */
  bool IsOverlayEnabled() const;

  /**
   * @brief Sets overlay auto-hide delay.
   * @param[in] milliseconds Delay, or 0 to keep overlay decoration visible
   *            indefinitely. Passing 0 also reveals decoration that auto-hide
   *            had already hidden.
   */
  void SetOverlayAutoHideDelay(uint32_t milliseconds);

  /**
   * @brief Returns overlay auto-hide delay.
   * @return Delay in milliseconds, or 0 when auto-hide is disabled
   */
  uint32_t GetOverlayAutoHideDelay() const;

  /**
   * @brief Reveals an auto-hidden overlay and starts a fresh hide timeout.
   */
  void ShowOverlayTemporarily();

  /**
   * @brief Returns whether decoration is suppressed by auto-hide.
   * @return true when auto-hidden
   */
  bool IsOverlayAutoHidden() const;

  /**
   * @brief Sets interactive resize policy.
   *
   * KEEP_ASPECT_RATIO requests native surface aspect-ratio preservation from
   * the window system. A backend that does not support the corresponding
   * native policy can continue to resize freely.
   *
   * @param[in] policy Resize policy
   */
  void SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy policy);

  /**
   * @brief Returns interactive resize policy.
   * @return Current policy
   */
  WindowFrameInteractiveResizePolicy GetInteractiveResizePolicy() const;

  /**
   * @brief Sets minimum frame size.
   * @param[in] size Minimum size excluding shadow
   * @return true when applied, or false when it conflicts with the maximum
   * @note Native constraints are active only while WindowFrame is attached.
   */
  bool SetMinimumFrameSize(const Dali::Vector2& size);

  /**
   * @brief Sets maximum frame size.
   * @param[in] size Maximum size excluding shadow
   * @return true when applied, or false when it conflicts with the minimum
   */
  bool SetMaximumFrameSize(const Dali::Vector2& size);

  /**
   * @brief Clears the minimum frame-size constraint.
   */
  void ClearMinimumFrameSize();

  /**
   * @brief Clears the maximum frame-size constraint.
   */
  void ClearMaximumFrameSize();

  /**
   * @brief Reads minimum frame size.
   * @param[out] size Configured size
   * @return true when configured
   */
  bool GetMinimumFrameSize(Dali::Vector2& size) const;

  /**
   * @brief Reads maximum frame size.
   * @param[out] size Configured size
   * @return true when configured
   */
  bool GetMaximumFrameSize(Dali::Vector2& size) const;

  /**
   * @brief Requests resizing the window to a target frame size.
   *
   * The target excludes shadow outsets. While attached, native resize is
   * deferred to the next event-loop tick and repeated requests are coalesced.
   * @param[in] size Target frame size
   * @return true when accepted for resize, or false when no window is available
   *         or maximized geometry is compositor-owned
   */
  bool RequestFrameResize(const Dali::Vector2& size);

  /**
   * @brief Requests resizing the window to a target application content size.
   *
   * The target is converted through the current decoration layout and shadow
   * outsets. While attached, native resize is deferred to the next event-loop
   * tick to avoid re-entering window event processing. Repeated requests are
   * coalesced and a pending request is dropped by Detach().
   * @param[in] size Target application content size
   * @return true when accepted for resize, or false when no window is available
   *         or maximized geometry is compositor-owned
   */
  bool RequestContentResize(const Dali::Vector2& size);

  /**
   * @brief Returns current frame size excluding shadow outsets.
   * @return Frame size
   */
  Dali::Vector2 GetFrameSize() const;

  /**
   * @brief Returns current layout-managed content size.
   * @return Content size
   */
  Dali::Vector2 GetContentSize() const;

  /**
   * @brief Requests minimize through the command interceptor and native backend.
   *
   * DISPATCHED reports that the underlying request was dispatched, not that the window
   * system accepted or completed it. Observe WindowStateChangedSignal() for the
   * resulting state.
   *
   * The request is dispatched even when the window already appears minimized.
   * Filtering on the last observed state would drop a request made while that
   * state is stale, such as one sent just after the user restored the window.
   *
   * There is no matching request to leave a minimized window: that belongs to
   * the window system, and WindowFrame follows the state it reports.
   * @return Command dispatch result
   */
  WindowFrameCommandResult RequestMinimize();

  /**
   * @brief Requests maximize through the command interceptor and native backend.
   *
   * Normal maximize/restore geometry is compositor-owned. A configured restore
   * size is supplied only for the initial full-size launch policy.
   *
   * The request is dispatched even when the window already appears to be maximized,
   * so a RESTORE sent straight after this one is the state that lands.
   * @return Command dispatch result
   */
  WindowFrameCommandResult RequestMaximize();

  /**
   * @brief Requests restore through the command interceptor and native backend.
   *
   * Normal restore geometry is compositor-owned. A configured restore size is
   * supplied only for the initial full-size launch policy.
   *
   * The request is dispatched even when the window already appears to be restored.
   * @return Command dispatch result
   */
  WindowFrameCommandResult RequestRestore();

  /**
   * @brief Requests the opposite of the current maximize state.
   *
   * Unlike branching on IsMaximized(), this also covers states that cannot be
   * toggled, such as a minimized window.
   *
   * A toggle has no target of its own, so unlike the explicit requests it does
   * read the last reported state. Two toggles dispatched before that state changes
   * therefore ask for the same thing twice.
   * @return Command dispatch result, or NOT_DISPATCHED when the current state
   *         cannot be toggled
   */
  WindowFrameCommandResult ToggleMaximize();

  /**
   * @brief Requests close through interceptor or close callback.
   *
   * @return Command dispatch result
   */
  WindowFrameCommandResult RequestClose();

  /**
   * @brief Sets the single policy interceptor invoked before default commands.
   *
   * Returning HANDLED prevents the default native operation, and the
   * interceptor then owns the command entirely.
   *
   * The callback is not connection-tracked. Clear it before destroying a
   * receiver that does not outlive this WindowFrame.
   * @param[in] interceptor Callback, or an empty callback to clear it
   */
  void SetCommandInterceptor(CommandInterceptor interceptor);

  /**
   * @brief Emitted when the observed window state changes.
   *
   * The state reported by the window system is the only source. This signal
   * therefore fires for a change the application requested and for a change the
   * compositor made on its own, and it does not fire when the window system
   * repeats a state WindowFrame already observed.
   * @return State-change signal
   */
  WindowStateChangedSignalType& WindowStateChangedSignal();

  /**
   * @brief Emitted once a frame showing a newly observed window state is drawn.
   *
   * This follows WindowStateChangedSignal() and carries the same state, so it
   * also fires for compositor-driven changes. Two states never present:
   * MINIMIZED, because a minimized window draws no frame, and a state that a
   * newer state supersedes while its frame is still awaited.
   *
   * WindowFrame registers the frame callback only while this signal has an
   * observer, so applications do not need a separate enable flag.
   * @return Presented-state signal
   */
  WindowStatePresentedSignalType& WindowStatePresentedSignal();

  /**
   * @brief Emitted when laid-out surface, frame, or content geometry changes.
   * @return Geometry-change signal
   */
  GeometryChangedSignalType& GeometryChangedSignal();

  /**
   * @brief Emitted after compositor move or resize completion.
   *
   * This signal is distinct from GeometryChangedSignal(), which can be emitted
   * for intermediate geometry changes. Unlike that signal it is not suppressed
   * when the geometry is unchanged, because it reports a completed operation
   * rather than a new value.
   *
   * @return Geometry-completion signal
   */
  GeometryChangeCompletedSignalType& GeometryChangeCompletedSignal();

  /**
   * @brief Emitted when effective decoration visibility changes.
   * @return Decoration visibility signal
   */
  DecorationVisibilityChangedSignalType& DecorationVisibilityChangedSignal();

  /**
   * @brief Emitted for move and resize interaction state changes.
   * @return Interaction signal
   */
  InteractionChangedSignalType& InteractionChangedSignal();

  /**
   * @brief Emitted with the result of every command dispatch.
   *
   * @return Command-result signal
   */
  CommandProcessedSignalType& CommandProcessedSignal();

public: // Internal
  explicit DALI_INTERNAL WindowFrame(Internal::WindowFrameImpl* impl);
};

} // namespace Dali::Ui

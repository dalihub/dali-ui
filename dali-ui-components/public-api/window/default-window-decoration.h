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
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/window/window-frame.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{
namespace Internal DALI_INTERNAL
{
class DefaultWindowDecorationImpl;
}

/**
 * @brief Colour theme used by the default window decoration.
 */
enum class DefaultWindowDecorationTheme
{
  DARK  = 0, ///< Dark frame with light icons
  LIGHT = 1  ///< Light frame with dark icons
};

/**
 * @brief Creation-time configuration for a DefaultWindowDecoration.
 *
 * Defaults provide a dark frame with thin top and side edges. The bottom bar
 * contains an expandable move region, built-in minimize,
 * maximize/restore, and close buttons, and a resize handle in each corner.
 */
class DALI_UI_COMPONENTS_API DefaultWindowDecorationOptions
{
public:
  DefaultWindowDecorationOptions();
  DefaultWindowDecorationOptions(const DefaultWindowDecorationOptions& rhs);
  DefaultWindowDecorationOptions(DefaultWindowDecorationOptions&& rhs) noexcept;
  DefaultWindowDecorationOptions& operator=(const DefaultWindowDecorationOptions& rhs);
  DefaultWindowDecorationOptions& operator=(DefaultWindowDecorationOptions&& rhs) noexcept;
  ~DefaultWindowDecorationOptions();

  /**
   * @brief Sets the decoration colour theme.
   * @param[in] theme Requested theme
   */
  void SetTheme(DefaultWindowDecorationTheme theme);

  /**
   * @brief Returns the decoration colour theme.
   * @return Configured theme
   */
  DefaultWindowDecorationTheme GetTheme() const;

  /**
   * @brief Sets the height of the bottom bar.
   * @param[in] height Bar height, defaults to 50
   */
  void SetBarHeight(float height);

  /**
   * @brief Returns the height of the bottom bar.
   * @return Bar height
   */
  float GetBarHeight() const;

  /**
   * @brief Sets the width of the left and right frame edges.
   * @param[in] thickness Edge thickness, defaults to 5
   */
  void SetEdgeThickness(float thickness);

  /**
   * @brief Returns the width of the left and right frame edges.
   * @return Edge thickness
   */
  float GetEdgeThickness() const;

  /**
   * @brief Turns the top edge into an area the application can fill.
   *
   * The top edge is normally a thin line of SetEdgeThickness(). Giving it a
   * height makes it a usable strip instead, reachable through
   * DefaultWindowDecoration::GetTopArea(), for a title or product branding. The
   * built-in minimize, maximize/restore, and close buttons stay in the bottom
   * bar.
   *
   * A height of 0, the default, keeps the thin edge and leaves GetTopArea()
   * uninitialized. A height below the edge thickness is raised to it, since the
   * frame still needs its border.
   * @param[in] height Top area height, defaults to 0
   */
  void SetTopAreaHeight(float height);

  /**
   * @brief Returns the configured top area height.
   * @return Top area height, or 0 when the top is a thin edge
   */
  float GetTopAreaHeight() const;

  /**
   * @brief Sets the frame corner radius used while the window is not maximized.
   * @param[in] radius Corner radius, defaults to 12
   */
  void SetCornerRadius(float radius);

  /**
   * @brief Returns the frame corner radius.
   * @return Corner radius
   */
  float GetCornerRadius() const;

  /**
   * @brief Overrides the bar and frame colour selected by the theme.
   * @param[in] color Background colour
   */
  void SetBackgroundColor(const Dali::Ui::UiColor& color);

  /**
   * @brief Returns the bar and frame colour.
   * @return Background colour
   */
  Dali::Ui::UiColor GetBackgroundColor() const;

  /**
   * @brief Sets whether the bar carries a minimize control.
   * @param[in] enabled true to include the control
   */
  void SetMinimizeControlEnabled(bool enabled);

  /**
   * @brief Returns whether the bar carries a minimize control.
   * @return true when included
   */
  bool IsMinimizeControlEnabled() const;

  /**
   * @brief Sets whether the bar carries a maximize and restore control.
   * @param[in] enabled true to include the control
   */
  void SetMaximizeRestoreControlEnabled(bool enabled);

  /**
   * @brief Returns whether the bar carries a maximize and restore control.
   * @return true when included
   */
  bool IsMaximizeRestoreControlEnabled() const;

  /**
   * @brief Sets whether the bar carries a close control.
   * @param[in] enabled true to include the control
   */
  void SetCloseControlEnabled(bool enabled);

  /**
   * @brief Returns whether the bar carries a close control.
   * @return true when included
   */
  bool IsCloseControlEnabled() const;

  /**
   * @brief Sets whether the frame casts a shadow.
   *
   * The shadow is drawn in the window surface outside the frame, so the window
   * has to be created transparent for it to be visible. It is not drawn while
   * the window is maximized.
   * @param[in] enabled true to cast a shadow
   */
  void SetShadowEnabled(bool enabled);

  /**
   * @brief Returns whether the frame casts a shadow.
   * @return true when a shadow is cast
   */
  bool IsShadowEnabled() const;

  /**
   * @brief Sets whether a resize handle is placed in each bottom corner.
   * @param[in] enabled true to include the handles
   */
  void SetResizeHandlesEnabled(bool enabled);

  /**
   * @brief Returns whether resize handles are included.
   * @return true when included
   */
  bool IsResizeHandlesEnabled() const;

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief A ready-made window decoration for a WindowFrame.
 *
 * New() builds the decoration, layout and frame style, installs them, and
 * keeps the maximize and restore icon in step with the window, so a complete
 * window decoration takes one call:
 *
 * @code
 * mWindowFrame = WindowFrame::New(window, closeCallback);
 * mWindowDecoration = DefaultWindowDecoration::New(mWindowFrame);
 * mWindowFrame.Attach();
 * @endcode
 *
 * The default layout has an optional top area above the application content.
 * Its bottom decoration slot is a bar arranged from start to end as follows:
 * bottom-left resize handle, expandable move region, application actions,
 * built-in minimize button, built-in maximize/restore button, built-in close
 * button, and bottom-right resize handle. Disabled controls keep no visible
 * width. In this class, "bar" always means that bottom bar, and "window
 * controls" means the built-in minimize, maximize/restore, and close buttons.
 *
 * The returned handle owns application action callbacks and window-state
 * synchronisation, so it belongs in a member such as mWindowDecoration rather
 * than a local. WindowFrame owns the decoration and built-in minimize,
 * maximize/restore, and close control wiring, so releasing the handle leaves
 * the decoration drawn and those controls working.
 * Application actions stop, however, as do the maximize/restore icon update and
 * the frame corner radius update that squares the decoration while maximized.
 *
 * The decoration is installed with WindowFrameSizePolicy::KEEP_WINDOW_SIZE,
 * so the window keeps its size and the content shrinks by the decoration. Replacing
 * a custom frame with a DefaultWindowDecoration at runtime therefore changes the content
 * size.
 *
 * A product that needs a different look can either adjust
 * DefaultWindowDecorationOptions, add its own content to GetMoveRegion(), adjust the
 * installed views through GetDecoration(), or ignore this class and build a
 * WindowFrameDecoration directly.
 */
class DALI_UI_COMPONENTS_API DefaultWindowDecoration : public Dali::BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized DefaultWindowDecoration handle.
   */
  DefaultWindowDecoration();
  ~DefaultWindowDecoration();
  DefaultWindowDecoration(const DefaultWindowDecoration& rhs);
  DefaultWindowDecoration(DefaultWindowDecoration&& rhs) noexcept;
  DefaultWindowDecoration& operator=(const DefaultWindowDecoration& rhs);
  DefaultWindowDecoration& operator=(DefaultWindowDecoration&& rhs) noexcept;

  /**
   * @brief Builds and installs the default window decoration.
   * @param[in] windowFrame Window to decorate
   * @return An initialized handle, or an uninitialized handle when the decoration
   *         could not be installed
   */
  static DefaultWindowDecoration New(WindowFrame windowFrame);

  /**
   * @brief Builds and installs the default window decoration.
   * @param[in] windowFrame Window to decorate
   * @param[in] options Appearance and control options
   * @return An initialized handle, or an uninitialized handle when the decoration
   *         could not be installed
   */
  static DefaultWindowDecoration New(WindowFrame windowFrame, const DefaultWindowDecorationOptions& options);

  /**
   * @brief Downcasts a handle to DefaultWindowDecoration.
   * @param[in] handle Handle to an object
   * @return A DefaultWindowDecoration handle, or an uninitialized handle on failure
   */
  static DefaultWindowDecoration DownCast(Dali::BaseHandle handle);

  /**
   * @brief Returns a copy of the installed decoration.
   *
   * The copy holds the same View handles, so reaching a view through it and
   * changing a property, a child, or a colour changes the installed decoration.
   *
   * Assigning a different view to a slot or a role does not, because that only
   * rewrites the returned copy. To swap a view, set it on the copy and
   * reinstall the copy with WindowFrame::SetDecoration(). Reinstalling makes
   * the frame the caller's, so the state synchronisation this class performs no
   * longer applies to a replaced maximize control.
   * @return Copy of the installed decoration
   */
  WindowFrameDecoration GetDecoration() const;

  /**
   * @brief Returns the expandable move region in the bottom bar.
   *
   * This area lies between the bottom-left resize handle and application action
   * buttons. Non-interactive content such as a title or branding can be added
   * here. The region becomes insensitive while maximized, so its children stop
   * receiving input in that state. Use AddBarAction() for a
   * bottom-bar action that must remain interactive, or GetTopArea() for custom
   * interactive content above the application content.
   * @return Bottom-bar move region
   */
  Dali::Ui::View GetMoveRegion() const;

  /**
   * @brief Returns the optional application area in the top decoration slot.
   *
   * This horizontal strip is above the application content. It is available
   * only when DefaultWindowDecorationOptions::SetTopAreaHeight() requested a
   * positive height; otherwise the top is a thin frame edge and this returns an
   * uninitialized handle.
   *
   * Unlike GetMoveRegion(), this area does not start a window move and its
   * children keep working while the window is maximized.
   * @return Top decoration area, or an uninitialized handle for a thin top edge
   */
  Dali::Ui::View GetTopArea() const;

  /**
   * @brief Adds an application action to the bottom bar.
   *
   * The button takes a column between the move region and the built-in
   * minimize, maximize/restore, and close buttons. It is beside the move region
   * rather than inside it, so it keeps working while the window is maximized.
   * Actions appear from start to end in the order they were added.
   *
   * The column widens what the bar needs, so GetMinimumFrameSize() grows with
   * each action. Apply it again after adding one.
   *
   * Anything the caller already put in GetMoveRegion() or GetTopArea() is kept.
   *
   * The icon is the caller's own, so the URL is used as given and the theme
   * colour is not applied to it. That colour exists to tint the white glyphs
   * this class ships with, and would multiply through an application icon.
   *
   * The callback is owned by this handle and is not connection-tracked. Its
   * receiver must outlive this DefaultWindowDecoration, or the handle must be
   * released before the receiver.
   * @param[in] iconUrl Full URL of the icon shown in the button
   * @param[in] onClick Invoked when the button is clicked
   * @return The installed button, for styling, or an uninitialized handle when
   *         the bottom bar could not take it
   */
  Dali::Ui::View AddBarAction(Dali::StringView iconUrl, Dali::Callback<void()> onClick);

  /**
   * @brief Returns the smallest frame this decoration lays out correctly.
   *
   * The bottom bar gives each control and each corner resize handle a fixed
   * column, so below this width the move region collapses and the last control
   * ends up under the corner handle. Pass this to
   * WindowFrame::SetMinimumFrameSize(),
   * or take the larger of it and the application's own minimum. Options that
   * remove a control or the resize handles lower it.
   *
   * The decoration does not apply this itself, because the window's size
   * constraints belong to the application.
   * @return Smallest frame size that lays out without overlap, excluding shadow
   */
  Dali::Vector2 GetMinimumFrameSize() const;

public: // Internal
  explicit DALI_INTERNAL DefaultWindowDecoration(Internal::DefaultWindowDecorationImpl* impl);
};

} // namespace Dali::Ui

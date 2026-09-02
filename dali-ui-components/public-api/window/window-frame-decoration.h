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
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Custom views installed in WindowFrame decoration slots and roles.
 *
 * The APIs are divided into three groups:
 * - SetTopSlot(), SetBottomSlot(), SetLeftSlot(), and SetRightSlot() install
 *   the layout roots owned by WindowFrame.
 * - SetMoveRegion(), SetMinimizeControl(), SetMaximizeRestoreControl(), and
 *   SetCloseControl() assign window-operation roles to views within those slot
 *   trees. A slot root may also be assigned a role when the entire slot should
 *   handle that operation.
 * - SetBottomLeftResizeHandle() and SetBottomRightResizeHandle() install views
 *   that act as both independent layout slots and resize roles.
 *
 * Views assigned a control role are made interactive while installed. Their
 * interactive trait remains part of the View if it is later removed from the
 * decoration, while WindowFrame disconnects its own command callbacks.
 */
class DALI_UI_COMPONENTS_API WindowFrameDecoration
{
public:
  /**
   * @brief Creates an empty decoration.
   */
  WindowFrameDecoration();
  WindowFrameDecoration(const WindowFrameDecoration& rhs);
  WindowFrameDecoration(WindowFrameDecoration&& rhs) noexcept;
  WindowFrameDecoration& operator=(const WindowFrameDecoration& rhs);
  WindowFrameDecoration& operator=(WindowFrameDecoration&& rhs) noexcept;
  ~WindowFrameDecoration();

  /**
   * @brief Sets the top slot root.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetTopSlot(const Dali::Ui::View& view);

  /**
   * @brief Returns the top slot root.
   * @return Configured view
   */
  Dali::Ui::View GetTopSlot() const;

  /**
   * @brief Sets the bottom slot root.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetBottomSlot(const Dali::Ui::View& view);

  /**
   * @brief Returns the bottom slot root.
   * @return Configured view
   */
  Dali::Ui::View GetBottomSlot() const;

  /**
   * @brief Sets the left slot root.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetLeftSlot(const Dali::Ui::View& view);

  /**
   * @brief Returns the left slot root.
   * @return Configured view
   */
  Dali::Ui::View GetLeftSlot() const;

  /**
   * @brief Sets the right slot root.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetRightSlot(const Dali::Ui::View& view);

  /**
   * @brief Returns the right slot root.
   * @return Configured view
   */
  Dali::Ui::View GetRightSlot() const;

  /**
   * @brief Assigns the native-move role to a view.
   *
   * Interactive children of the move region keep their own input, because a
   * child is hit-tested before its parent. A title bar can therefore be the
   * move region while the buttons inside it stay clickable.
   * @param[in] view View contained by a slot root
   */
  void SetMoveRegion(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the native-move role.
   * @return Configured view
   */
  Dali::Ui::View GetMoveRegion() const;

  /**
   * @brief Assigns the minimize-control role to a view.
   * @param[in] view View contained by a slot root
   */
  void SetMinimizeControl(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the minimize-control role.
   * @return Configured view
   */
  Dali::Ui::View GetMinimizeControl() const;

  /**
   * @brief Assigns the maximize/restore-control role to a view.
   * @param[in] view View contained by a slot root
   */
  void SetMaximizeRestoreControl(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the maximize/restore-control role.
   * @return Configured view
   */
  Dali::Ui::View GetMaximizeRestoreControl() const;

  /**
   * @brief Assigns the close-control role to a view.
   * @param[in] view View contained by a slot root
   */
  void SetCloseControl(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the close-control role.
   * @return Configured view
   */
  Dali::Ui::View GetCloseControl() const;

  /**
   * @brief Sets the view that acts as the bottom-left resize slot and role.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetBottomLeftResizeHandle(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the bottom-left resize slot and role.
   * @return Configured view
   */
  Dali::Ui::View GetBottomLeftResizeHandle() const;

  /**
   * @brief Sets the view that acts as the bottom-right resize slot and role.
   * @param[in] view Unparented slot view, or an empty handle
   */
  void SetBottomRightResizeHandle(const Dali::Ui::View& view);

  /**
   * @brief Returns the view assigned the bottom-right resize slot and role.
   * @return Configured view
   */
  Dali::Ui::View GetBottomRightResizeHandle() const;

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Configures the space occupied by decoration slots and resize handles.
 *
 * Insets reserve space inside the visible frame for the top, bottom, left, and
 * right slots. Resize-handle sizes are independent because the corner handles
 * can overlap the bottom slot.
 */
class DALI_UI_COMPONENTS_API WindowFrameLayout
{
public:
  /**
   * @brief Creates a layout with the default decoration and handle sizes.
   */
  WindowFrameLayout();
  WindowFrameLayout(const WindowFrameLayout& rhs);
  WindowFrameLayout(WindowFrameLayout&& rhs) noexcept;
  WindowFrameLayout& operator=(const WindowFrameLayout& rhs);
  WindowFrameLayout& operator=(WindowFrameLayout&& rhs) noexcept;
  ~WindowFrameLayout();

  /**
   * @brief Sets space reserved inside the frame for decoration slots.
   * @param[in] insets Decoration insets
   */
  void SetDecorationInsets(const Dali::Ui::Insets& insets);

  /**
   * @brief Returns decoration insets.
   * @return Configured insets
   */
  Dali::Ui::Insets GetDecorationInsets() const;

  /**
   * @brief Sets bottom-left resize-handle size.
   * @param[in] size Handle size
   */
  void SetBottomLeftResizeHandleSize(const Dali::Vector2& size);

  /**
   * @brief Returns bottom-left resize-handle size.
   * @return Configured size
   */
  Dali::Vector2 GetBottomLeftResizeHandleSize() const;

  /**
   * @brief Sets bottom-right resize-handle size.
   * @param[in] size Handle size
   */
  void SetBottomRightResizeHandleSize(const Dali::Vector2& size);

  /**
   * @brief Returns bottom-right resize-handle size.
   * @return Configured size
   */
  Dali::Vector2 GetBottomRightResizeHandleSize() const;

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief Result of validating and installing custom decoration views.
 */
enum class WindowFrameDecorationResult
{
  INSTALLED               = 0,
  SLOT_ALREADY_PARENTED   = 1,
  DUPLICATE_SLOT          = 2,
  DUPLICATE_ROLE          = 3,
  ROLE_OUTSIDE_DECORATION = 4
};

} // namespace Dali::Ui

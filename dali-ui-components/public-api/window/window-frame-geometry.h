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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Window, frame, and content bounds captured after frame layout.
 *
 * Surface bounds use screen coordinates. Frame and content bounds use
 * surface-local coordinates, so application content never subtracts
 * decoration insets.
 */
class DALI_UI_COMPONENTS_API WindowFrameGeometry
{
public:
  /**
   * @brief Creates an empty geometry value.
   */
  WindowFrameGeometry();
  WindowFrameGeometry(const WindowFrameGeometry& rhs);
  WindowFrameGeometry(WindowFrameGeometry&& rhs) noexcept;
  WindowFrameGeometry& operator=(const WindowFrameGeometry& rhs);
  WindowFrameGeometry& operator=(WindowFrameGeometry&& rhs) noexcept;
  ~WindowFrameGeometry();

  /**
   * @brief Returns window surface bounds in screen coordinates.
   * @return Surface bounds
   */
  Dali::Ui::LayoutRect GetSurfaceBounds() const;

  /**
   * @brief Returns visible frame bounds in surface-local coordinates.
   * @return Frame bounds
   */
  Dali::Ui::LayoutRect GetFrameBounds() const;

  /**
   * @brief Returns application content bounds in surface-local coordinates.
   * @return Content bounds
   */
  Dali::Ui::LayoutRect GetContentBounds() const;

  /**
   * @brief Returns the part of the content bounds that decoration is laid out over.
   *
   * This is zero unless overlay layout is active, because otherwise decoration
   * sits outside the content bounds. It describes the layout contract, so it
   * does not change while overlay decoration is temporarily auto-hidden.
   *
   * Use it to keep content that must stay readable, such as a header or a
   * primary action, clear of the decoration.
   * @return Overlay decoration insets
   */
  Dali::Ui::Insets GetDecorationOverlayInsets() const;

public: // Not intended for application developers
  DALI_INTERNAL WindowFrameGeometry(const Dali::Ui::LayoutRect& surfaceBounds,
                                    const Dali::Ui::LayoutRect& frameBounds,
                                    const Dali::Ui::LayoutRect& contentBounds,
                                    const Dali::Ui::Insets&     decorationOverlayInsets);

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Dali::Ui

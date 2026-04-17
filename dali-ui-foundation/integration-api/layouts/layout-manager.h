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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
class View;
class ViewImpl;

/**
 * @brief Abstract base class for layout managers.
 *
 * Layout managers are responsible for measuring and arranging child views
 * within a container. They implement the specific layout algorithms
 * for different layout types (stack, grid, flex, etc.).
 *
 * LayoutManager works with ViewImpl for direct access to
 * implementation details during layout calculations. This allows any View
 * (or derived class) to have layout capability.
 *
 * @note This is an internal class. It is not intended to be used directly
 * by applications.
 */
class DALI_UI_API LayoutManager
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~LayoutManager();

  /**
   * @brief Default constructor.
   */
  LayoutManager();

  /**
   * @brief Measures the view and its children.
   *
   * This method calculates the desired size of the view based on the
   * constraints and the sizes of its children.
   *
   * @param[in] view The internal view to measure
   * @param[in] widthConstraint The available width constraint
   * @param[in] heightConstraint The available height constraint
   * @return The measured size
   */
  virtual MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) = 0;

  /**
   * @brief Arranges the children within the view bounds.
   *
   * This method positions and sizes the children within the given bounds
   * according to the layout algorithm.
   *
   * @param[in] view The internal view whose children to arrange
   * @param[in] bounds The available bounds for arranging children
   * @return The final arranged size
   */
  virtual MeasuredSize ArrangeChildren(ViewImpl* view, const LayoutRect& bounds) = 0;

protected:
  /**
   * @brief Measures a child view.
   *
   * Helper method for derived classes to measure a child view.
   *
   * @param[in] child The child to measure
   * @param[in] widthConstraint The width constraint for the child
   * @param[in] heightConstraint The height constraint for the child
   * @return The measured size of the child
   */
  MeasuredSize MeasureChild(ViewImpl* child, float widthConstraint, float heightConstraint);

  /**
   * @brief Arranges a child view within the given bounds.
   *
   * @param[in] child  The child to arrange
   * @param[in] bounds The visual bounds for the child
   * @return The final arranged size
   */
  MeasuredSize ArrangeChild(ViewImpl* child, const LayoutRect& bounds);

  /**
   * @brief Gets the children container from a view.
   *
   * Helper method for derived classes to access children.
   *
   * @param[in] view The view to get children from
   * @return Reference to the children container
   */
  IntegrationView::ChildContainer& GetChildren(ViewImpl* view);

private:
  // Not copyable or movable
  LayoutManager(const LayoutManager&)            = delete;
  LayoutManager(LayoutManager&&)                 = delete;
  LayoutManager& operator=(const LayoutManager&) = delete;
  LayoutManager& operator=(LayoutManager&&)      = delete;
};

} // namespace Ui
} // namespace Dali

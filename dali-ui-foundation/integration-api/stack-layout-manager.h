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
#include <dali-ui-foundation/integration-api/layout-manager.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/stack-layout.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

/**
 * @brief Implements the stack layout algorithm.
 *
 * Can be used with ViewImpl::SetLayoutManager() or returned from
 * StackLayoutImpl::CreateLayoutManager(). Subclass to customize stack layout behaviour.
 */
class StackLayoutManager : public LayoutManager
{
public:
  /**
   * @brief Creates a new StackLayoutManager.
   *
   * @param[in] orientation Stack orientation (vertical or horizontal)
   * @param[in] spacing Spacing between children
   */
  StackLayoutManager(StackOrientation orientation, float spacing);

  /**
   * @brief Virtual destructor.
   */
  ~StackLayoutManager() override;

  void SetOrientation(StackOrientation orientation);
  void SetSpacing(float spacing);

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  MeasuredSize ArrangeChildren(ViewImpl* view, const LayoutRect& bounds) override;

protected:
  StackOrientation mOrientation;
  float mSpacing;
};

} // namespace Integration
} // namespace UI
} // namespace Dali

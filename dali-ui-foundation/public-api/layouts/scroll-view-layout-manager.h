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
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Implements the scroll view layout algorithm.
 *
 * ScrollViewLayoutManager allows content to have its natural size without
 * being constrained by the parent size, except when the child uses MatchParent
 * in which case the ScrollView's size is used as constraint.
 *
 * This layout manager is primarily designed for ScrollView to enable
 * scrolling of content that can be larger than the viewport. Scrollable
 * dimension updates only take effect when attached to a ScrollView; for
 * general layout use cases prefer the other LayoutManager subclasses.
 */
class DALI_UI_API ScrollViewLayoutManager : public LayoutManager
{
public:
  ScrollViewLayoutManager();
  ~ScrollViewLayoutManager() override;

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  MeasuredSize Arrange(ViewImpl* view, const LayoutRect& bounds) override;

private:
  class Impl;
};

} // namespace Ui
} // namespace Dali

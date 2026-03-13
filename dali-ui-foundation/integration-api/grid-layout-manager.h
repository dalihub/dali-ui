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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layout-manager.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implements the grid layout algorithm.
 *
 * Can be returned from GridLayoutImpl::CreateLayoutManager().
 * Subclass to customize grid layout behaviour.
 */
class DALI_UI_API GridLayoutManager : public LayoutManager
{
public:
  GridLayoutManager(const std::vector<GridLength>& rows, const std::vector<GridLength>& columns, float rowSpacing,
                    float columnSpacing);
  ~GridLayoutManager() override;

  void SetRowDefinitions(const std::vector<GridLength>& rows);
  void SetColumnDefinitions(const std::vector<GridLength>& columns);
  void SetRowSpacing(float spacing);
  void SetColumnSpacing(float spacing);

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  MeasuredSize ArrangeChildren(ViewImpl* view, const LayoutRect& bounds) override;

protected:
  std::vector<GridLength> mRowDefinitions;
  std::vector<GridLength> mColumnDefinitions;
  float                   mRowSpacing;
  float                   mColumnSpacing;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali

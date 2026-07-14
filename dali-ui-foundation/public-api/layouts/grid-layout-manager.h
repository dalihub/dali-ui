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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Implements the grid layout algorithm.
 *
 * Arranges children in a row/column grid described by row/column GridLength
 * definitions. Per-child row, column, row-span, column-span and alignment
 * are read from GridLayoutParams.
 */
class DALI_UI_API GridLayoutManager : public LayoutManager
{
public:
  GridLayoutManager(const Dali::Vector<GridLength>& rows, const Dali::Vector<GridLength>& columns, float rowSpacing,
                    float columnSpacing);
  ~GridLayoutManager() override;

  void                            SetRowDefinitions(const Dali::Vector<GridLength>& rows);
  const Dali::Vector<GridLength>& GetRowDefinitions() const;
  void                            SetColumnDefinitions(const Dali::Vector<GridLength>& columns);
  const Dali::Vector<GridLength>& GetColumnDefinitions() const;
  void                            SetRowSpacing(float spacing);
  float                           GetRowSpacing() const;
  void                            SetColumnSpacing(float spacing);
  float                           GetColumnSpacing() const;

  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override;
  void         Arrange(ViewImpl* view, const LayoutRect& bounds) override;

private:
  class Impl;
};

} // namespace Ui
} // namespace Dali

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class GridLayoutImpl;
using GridLayoutImplPtr = IntrusivePtr<GridLayoutImpl>;

/**
 * @brief This is the internal implementation class for GridLayout.
 */
class DALI_UI_API GridLayoutImpl : public LayoutImpl
{
public:
  /**
   * @brief Creates a new GridLayout implementation.
   *
   * @return An IntrusivePtr to the new GridLayoutImpl
   */
  static GridLayoutImplPtr New();

protected:
  virtual ~GridLayoutImpl();
  void OnInitialize() override;
  GridLayoutImpl();

public: // Row/Column Definition API
  void                     AddRowDefinition(GridLength height);
  void                     AddColumnDefinition(GridLength width);
  void                     SetRowDefinitions(const Dali::Vector<GridLength>& rows);
  void                     SetColumnDefinitions(const Dali::Vector<GridLength>& columns);
  Dali::Vector<GridLength> GetRowDefinitions() const;
  Dali::Vector<GridLength> GetColumnDefinitions() const;
  uint32_t                 GetRowCount() const;
  uint32_t                 GetColumnCount() const;
  void                     ClearRowDefinitions();
  void                     ClearColumnDefinitions();

public: // Spacing API
  void  SetRowSpacing(float spacing);
  float GetRowSpacing() const;
  void  SetColumnSpacing(float spacing);
  float GetColumnSpacing() const;

private:
  GridLayoutImpl(const GridLayoutImpl&)            = delete;
  GridLayoutImpl(GridLayoutImpl&&)                 = delete;
  GridLayoutImpl& operator=(const GridLayoutImpl&) = delete;
  GridLayoutImpl& operator=(GridLayout&&)          = delete;
};

} // namespace Integration

inline DALI_UI_API Integration::GridLayoutImpl& GetImpl(GridLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::GridLayoutImpl&>(handle);
}

inline DALI_UI_API const Integration::GridLayoutImpl& GetImpl(const GridLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::GridLayoutImpl&>(handle);
}
} // namespace Ui
} // namespace Dali

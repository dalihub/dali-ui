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
#include <dali-ui-foundation/integration-api/grid-layout-manager.h>
#include <dali-ui-foundation/integration-api/layout-impl.h>
#include <dali-ui-foundation/public-api/grid-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief This is the internal implementation class for GridLayout.
 */
class GridLayoutImpl : public LayoutImpl
{
public:
  static Ui::GridLayout New();

protected:
  virtual ~GridLayoutImpl();
  GridLayoutImpl();

public: // Row/Column Definition API
  void AddRowDefinition(GridLength height);
  void AddColumnDefinition(GridLength width);
  void SetRowDefinitions(const std::vector<GridLength>& rows);
  void SetColumnDefinitions(const std::vector<GridLength>& columns);
  std::vector<GridLength> GetRowDefinitions() const;
  std::vector<GridLength> GetColumnDefinitions() const;
  uint32_t GetRowCount() const;
  uint32_t GetColumnCount() const;
  void ClearRowDefinitions();
  void ClearColumnDefinitions();

public: // Spacing API
  void SetRowSpacing(float spacing);
  float GetRowSpacing() const;
  void SetColumnSpacing(float spacing);
  float GetColumnSpacing() const;

protected: // From Layout
  LayoutManager* CreateLayoutManager() override;

private:
  GridLayoutImpl(const GridLayoutImpl&) = delete;
  GridLayoutImpl(GridLayoutImpl&&) = delete;
  GridLayoutImpl& operator=(const GridLayoutImpl&) = delete;
  GridLayoutImpl& operator=(GridLayout&&) = delete;

private:
  std::vector<GridLength> mRowDefinitions;
  std::vector<GridLength> mColumnDefinitions;
  float mRowSpacing;
  float mColumnSpacing;
};

inline Integration::GridLayoutImpl& GetImpl(Ui::GridLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::GridLayoutImpl&>(handle);
}

inline const Integration::GridLayoutImpl& GetImpl(const Ui::GridLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::GridLayoutImpl&>(handle);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

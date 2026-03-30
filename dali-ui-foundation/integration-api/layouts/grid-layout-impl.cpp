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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/layouts/grid-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

BaseHandle Create()
{
  return GridLayout::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::GridLayoutImpl, Ui::Integration::LayoutImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

GridLayoutImplPtr GridLayoutImpl::New()
{
  return GridLayoutImplPtr(new GridLayoutImpl());
}

GridLayoutImpl::GridLayoutImpl()
: LayoutImpl()
{
}

GridLayoutImpl::~GridLayoutImpl()
{
}

void GridLayoutImpl::OnInitialize()
{
  LayoutImpl::OnInitialize();
  SetLayoutManager(new GridLayoutManager({}, {}, 0.0f, 0.0f));
}

void GridLayoutImpl::AddRowDefinition(GridLength height)
{
  auto*                   manager = static_cast<GridLayoutManager*>(GetLayoutManager());
  std::vector<GridLength> rows    = manager->GetRowDefinitions();
  rows.push_back(height);
  manager->SetRowDefinitions(rows);
  InvalidateMeasure();
}

void GridLayoutImpl::AddColumnDefinition(GridLength width)
{
  auto*                   manager = static_cast<GridLayoutManager*>(GetLayoutManager());
  std::vector<GridLength> cols    = manager->GetColumnDefinitions();
  cols.push_back(width);
  manager->SetColumnDefinitions(cols);
  InvalidateMeasure();
}

void GridLayoutImpl::SetRowDefinitions(const std::vector<GridLength>& rows)
{
  static_cast<GridLayoutManager*>(GetLayoutManager())->SetRowDefinitions(rows);
  InvalidateMeasure();
}

void GridLayoutImpl::SetColumnDefinitions(const std::vector<GridLength>& columns)
{
  static_cast<GridLayoutManager*>(GetLayoutManager())->SetColumnDefinitions(columns);
  InvalidateMeasure();
}

std::vector<GridLength> GridLayoutImpl::GetRowDefinitions() const
{
  return static_cast<GridLayoutManager*>(GetLayoutManager())->GetRowDefinitions();
}

std::vector<GridLength> GridLayoutImpl::GetColumnDefinitions() const
{
  return static_cast<GridLayoutManager*>(GetLayoutManager())->GetColumnDefinitions();
}

uint32_t GridLayoutImpl::GetRowCount() const
{
  return static_cast<uint32_t>(static_cast<GridLayoutManager*>(GetLayoutManager())->GetRowDefinitions().size());
}

uint32_t GridLayoutImpl::GetColumnCount() const
{
  return static_cast<uint32_t>(static_cast<GridLayoutManager*>(GetLayoutManager())->GetColumnDefinitions().size());
}

void GridLayoutImpl::ClearRowDefinitions()
{
  static_cast<GridLayoutManager*>(GetLayoutManager())->SetRowDefinitions({});
  InvalidateMeasure();
}

void GridLayoutImpl::ClearColumnDefinitions()
{
  static_cast<GridLayoutManager*>(GetLayoutManager())->SetColumnDefinitions({});
  InvalidateMeasure();
}

void GridLayoutImpl::SetRowSpacing(float spacing)
{
  auto* manager = static_cast<GridLayoutManager*>(GetLayoutManager());
  if(manager->GetRowSpacing() != spacing)
  {
    manager->SetRowSpacing(spacing);
    InvalidateMeasure();
  }
}

float GridLayoutImpl::GetRowSpacing() const
{
  return static_cast<GridLayoutManager*>(GetLayoutManager())->GetRowSpacing();
}

void GridLayoutImpl::SetColumnSpacing(float spacing)
{
  auto* manager = static_cast<GridLayoutManager*>(GetLayoutManager());
  if(manager->GetColumnSpacing() != spacing)
  {
    manager->SetColumnSpacing(spacing);
    InvalidateMeasure();
  }
}

float GridLayoutImpl::GetColumnSpacing() const
{
  return static_cast<GridLayoutManager*>(GetLayoutManager())->GetColumnSpacing();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

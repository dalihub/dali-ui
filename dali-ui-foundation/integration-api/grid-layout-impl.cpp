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
#include <dali-ui-foundation/integration-api/grid-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

UI::GridLayout GridLayoutImpl::New()
{
  IntrusivePtr<Integration::GridLayoutImpl> impl = new Integration::GridLayoutImpl();
  UI::GridLayout handle = UI::GridLayout(*impl);
  impl->Initialize();
  return handle;
}

GridLayoutImpl::GridLayoutImpl()
  : LayoutImpl(),
    mRowDefinitions(),
    mColumnDefinitions(),
    mRowSpacing(0.0f),
    mColumnSpacing(0.0f)
{
}

GridLayoutImpl::~GridLayoutImpl()
{
}

void GridLayoutImpl::AddRowDefinition(GridLength height)
{
  mRowDefinitions.push_back(height);
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetRowDefinitions(mRowDefinitions);
  }
  InvalidateMeasure();
}

void GridLayoutImpl::AddColumnDefinition(GridLength width)
{
  mColumnDefinitions.push_back(width);
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetColumnDefinitions(mColumnDefinitions);
  }
  InvalidateMeasure();
}

void GridLayoutImpl::SetRowDefinitions(const std::vector<GridLength>& rows)
{
  mRowDefinitions = rows;
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetRowDefinitions(mRowDefinitions);
  }
  InvalidateMeasure();
}

void GridLayoutImpl::SetColumnDefinitions(const std::vector<GridLength>& columns)
{
  mColumnDefinitions = columns;
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetColumnDefinitions(mColumnDefinitions);
  }
  InvalidateMeasure();
}

std::vector<GridLength> GridLayoutImpl::GetRowDefinitions() const
{
  return mRowDefinitions;
}

std::vector<GridLength> GridLayoutImpl::GetColumnDefinitions() const
{
  return mColumnDefinitions;
}

uint32_t GridLayoutImpl::GetRowCount() const
{
  return static_cast<uint32_t>(mRowDefinitions.size());
}

uint32_t GridLayoutImpl::GetColumnCount() const
{
  return static_cast<uint32_t>(mColumnDefinitions.size());
}

void GridLayoutImpl::ClearRowDefinitions()
{
  mRowDefinitions.clear();
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetRowDefinitions(mRowDefinitions);
  }
  InvalidateMeasure();
}

void GridLayoutImpl::ClearColumnDefinitions()
{
  mColumnDefinitions.clear();
  LayoutManager* manager = GetLayoutManager();
  if (manager)
  {
    static_cast<GridLayoutManager*>(manager)->SetColumnDefinitions(mColumnDefinitions);
  }
  InvalidateMeasure();
}

void GridLayoutImpl::SetRowSpacing(float spacing)
{
  if (mRowSpacing != spacing)
  {
    mRowSpacing = spacing;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<GridLayoutManager*>(manager)->SetRowSpacing(spacing);
    }
    InvalidateMeasure();
  }
}

float GridLayoutImpl::GetRowSpacing() const
{
  return mRowSpacing;
}

void GridLayoutImpl::SetColumnSpacing(float spacing)
{
  if (mColumnSpacing != spacing)
  {
    mColumnSpacing = spacing;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<GridLayoutManager*>(manager)->SetColumnSpacing(spacing);
    }
    InvalidateMeasure();
  }
}

float GridLayoutImpl::GetColumnSpacing() const
{
  return mColumnSpacing;
}

LayoutManager* GridLayoutImpl::CreateLayoutManager()
{
  return new GridLayoutManager(mRowDefinitions, mColumnDefinitions, mRowSpacing, mColumnSpacing);
}

} // namespace Integration
} // namespace UI
} // namespace Dali

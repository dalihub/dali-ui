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
#include <dali-ui-foundation/public-api/grid-layout.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/grid-layout-impl.h>

namespace Dali
{
namespace Ui
{

GridLayout::GridLayout()
{
}

GridLayout::GridLayout(const GridLayout& layout)
  : Layout(layout)
{
}

GridLayout::GridLayout(GridLayout&& rhs) noexcept
  : Layout(std::move(rhs))
{
}

GridLayout::~GridLayout()
{
}

GridLayout& GridLayout::operator=(const GridLayout& handle)
{
  if (&handle != this)
  {
    Layout::operator=(handle);
  }
  return *this;
}

GridLayout& GridLayout::operator=(GridLayout&& rhs) noexcept
{
  Layout::operator=(std::move(rhs));
  return *this;
}

GridLayout GridLayout::New()
{
  Integration::GridLayoutImplPtr impl = Integration::GridLayoutImpl::New();

  // Pass ownership to handle
  GridLayout handle(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

GridLayout GridLayout::DownCast(BaseHandle handle)
{
  return Ui::Control::DownCast<GridLayout, Integration::GridLayoutImpl>(handle);
}

GridLayout::GridLayout(Integration::GridLayoutImpl& implementation)
  : Layout(implementation)
{
}

GridLayout::GridLayout(Dali::Internal::CustomActor* internal)
  : Layout(internal)
{
  VerifyCustomActorPointer<Integration::GridLayoutImpl>(internal);
}

void GridLayout::AddRowDefinition(GridLength height)
{
  Integration::GetImpl(*this).AddRowDefinition(height);
}

void GridLayout::AddColumnDefinition(GridLength width)
{
  Integration::GetImpl(*this).AddColumnDefinition(width);
}

void GridLayout::SetRowDefinitions(const std::vector<GridLength>& rows)
{
  Integration::GetImpl(*this).SetRowDefinitions(rows);
}

void GridLayout::SetColumnDefinitions(const std::vector<GridLength>& columns)
{
  Integration::GetImpl(*this).SetColumnDefinitions(columns);
}

std::vector<GridLength> GridLayout::GetRowDefinitions() const
{
  return Integration::GetImpl(*this).GetRowDefinitions();
}

std::vector<GridLength> GridLayout::GetColumnDefinitions() const
{
  return Integration::GetImpl(*this).GetColumnDefinitions();
}

uint32_t GridLayout::GetRowCount() const
{
  return Integration::GetImpl(*this).GetRowCount();
}

uint32_t GridLayout::GetColumnCount() const
{
  return Integration::GetImpl(*this).GetColumnCount();
}

void GridLayout::ClearRowDefinitions()
{
  Integration::GetImpl(*this).ClearRowDefinitions();
}

void GridLayout::ClearColumnDefinitions()
{
  Integration::GetImpl(*this).ClearColumnDefinitions();
}

void GridLayout::SetRowSpacing(float spacing)
{
  Integration::GetImpl(*this).SetRowSpacing(spacing);
}

float GridLayout::GetRowSpacing() const
{
  return Integration::GetImpl(*this).GetRowSpacing();
}

void GridLayout::SetColumnSpacing(float spacing)
{
  Integration::GetImpl(*this).SetColumnSpacing(spacing);
}

float GridLayout::GetColumnSpacing() const
{
  return Integration::GetImpl(*this).GetColumnSpacing();
}

// Static attached property methods
namespace
{
/**
 * @brief Helper to register or update an integer attached property.
 */
void SetIntProperty(View view, const std::string& name, int value)
{
  Property::Index index = view.GetPropertyIndex(name);
  if (index == Dali::Property::INVALID_INDEX)
  {
    view.RegisterProperty(name, value);
  }
  else
  {
    view.SetProperty(index, value);
  }
}
} // namespace

void GridLayout::SetRow(View view, uint32_t row)
{
  if (view)
  {
    SetIntProperty(view, "gridRow", static_cast<int>(row));
    view.InvalidateMeasure();
  }
}

uint32_t GridLayout::GetRow(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("gridRow");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<uint32_t>(view.GetProperty<int>(index));
    }
  }
  return 0;
}

void GridLayout::SetColumn(View view, uint32_t column)
{
  if (view)
  {
    SetIntProperty(view, "gridColumn", static_cast<int>(column));
    view.InvalidateMeasure();
  }
}

uint32_t GridLayout::GetColumn(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("gridColumn");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<uint32_t>(view.GetProperty<int>(index));
    }
  }
  return 0;
}

void GridLayout::SetRowSpan(View view, uint32_t span)
{
  if (view)
  {
    SetIntProperty(view, "gridRowSpan", static_cast<int>(span));
    view.InvalidateMeasure();
  }
}

uint32_t GridLayout::GetRowSpan(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("gridRowSpan");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<uint32_t>(view.GetProperty<int>(index));
    }
  }
  return 1;
}

void GridLayout::SetColumnSpan(View view, uint32_t span)
{
  if (view)
  {
    SetIntProperty(view, "gridColumnSpan", static_cast<int>(span));
    view.InvalidateMeasure();
  }
}

uint32_t GridLayout::GetColumnSpan(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("gridColumnSpan");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<uint32_t>(view.GetProperty<int>(index));
    }
  }
  return 1;
}

} // namespace Ui
} // namespace Dali

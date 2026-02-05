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
#include <dali-ui-foundation/integration-api/stack-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

UI::StackLayout StackLayoutImpl::New(StackOrientation orientation)
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Integration::StackLayoutImpl> impl = new Integration::StackLayoutImpl(orientation);

  // Pass ownership to CustomActor handle
  UI::StackLayout handle = UI::StackLayout(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

StackLayoutImpl::StackLayoutImpl(StackOrientation orientation)
  : LayoutImpl(),
    mOrientation(orientation),
    mSpacing(0.0f)
{
}

StackLayoutImpl::~StackLayoutImpl()
{
}

void StackLayoutImpl::SetOrientation(StackOrientation orientation)
{
  if (mOrientation != orientation)
  {
    mOrientation = orientation;

    // Update layout manager if it exists
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<StackLayoutManager*>(manager)->SetOrientation(orientation);
    }

    InvalidateMeasure();
  }
}

StackOrientation StackLayoutImpl::GetOrientation() const
{
  return mOrientation;
}

void StackLayoutImpl::SetSpacing(float spacing)
{
  if (mSpacing != spacing)
  {
    mSpacing = spacing;

    // Update layout manager if it exists
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<StackLayoutManager*>(manager)->SetSpacing(spacing);
    }

    InvalidateMeasure();
  }
}

float StackLayoutImpl::GetSpacing() const
{
  return mSpacing;
}

LayoutManager* StackLayoutImpl::CreateLayoutManager()
{
  return new StackLayoutManager(mOrientation, mSpacing);
}

} // namespace Integration
} // namespace UI
} // namespace Dali

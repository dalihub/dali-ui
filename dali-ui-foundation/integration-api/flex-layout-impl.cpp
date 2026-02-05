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
#include <dali-ui-foundation/integration-api/flex-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

UI::FlexLayout FlexLayoutImpl::New()
{
  IntrusivePtr<Integration::FlexLayoutImpl> impl = new Integration::FlexLayoutImpl();
  UI::FlexLayout handle = UI::FlexLayout(*impl);
  impl->Initialize();
  return handle;
}

FlexLayoutImpl::FlexLayoutImpl()
  : LayoutImpl(),
    mDirection(FlexDirection::Row),
    mWrap(FlexWrap::NoWrap),
    mJustifyContent(FlexJustify::FlexStart),
    mAlignItems(FlexAlign::Stretch),
    mAlignContent(FlexAlign::Stretch)
{
}

FlexLayoutImpl::~FlexLayoutImpl()
{
}

void FlexLayoutImpl::SetDirection(FlexDirection direction)
{
  if (mDirection != direction)
  {
    mDirection = direction;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<FlexLayoutManager*>(manager)->SetDirection(direction);
    }
    InvalidateMeasure();
  }
}

FlexDirection FlexLayoutImpl::GetDirection() const
{
  return mDirection;
}

void FlexLayoutImpl::SetWrap(FlexWrap wrap)
{
  if (mWrap != wrap)
  {
    mWrap = wrap;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<FlexLayoutManager*>(manager)->SetWrap(wrap);
    }
    InvalidateMeasure();
  }
}

FlexWrap FlexLayoutImpl::GetWrap() const
{
  return mWrap;
}

void FlexLayoutImpl::SetJustifyContent(FlexJustify justify)
{
  if (mJustifyContent != justify)
  {
    mJustifyContent = justify;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<FlexLayoutManager*>(manager)->SetJustifyContent(justify);
    }
    InvalidateArrange();
  }
}

FlexJustify FlexLayoutImpl::GetJustifyContent() const
{
  return mJustifyContent;
}

void FlexLayoutImpl::SetAlignItems(FlexAlign align)
{
  if (mAlignItems != align)
  {
    mAlignItems = align;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<FlexLayoutManager*>(manager)->SetAlignItems(align);
    }
    InvalidateArrange();
  }
}

FlexAlign FlexLayoutImpl::GetAlignItems() const
{
  return mAlignItems;
}

void FlexLayoutImpl::SetAlignContent(FlexAlign align)
{
  if (mAlignContent != align)
  {
    mAlignContent = align;
    LayoutManager* manager = GetLayoutManager();
    if (manager)
    {
      static_cast<FlexLayoutManager*>(manager)->SetAlignContent(align);
    }
    InvalidateArrange();
  }
}

FlexAlign FlexLayoutImpl::GetAlignContent() const
{
  return mAlignContent;
}

LayoutManager* FlexLayoutImpl::CreateLayoutManager()
{
  return new FlexLayoutManager(mDirection, mWrap, mJustifyContent, mAlignItems, mAlignContent);
}

} // namespace Integration
} // namespace UI
} // namespace Dali

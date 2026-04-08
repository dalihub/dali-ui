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
#include <dali-ui-foundation/integration-api/interactive-view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-trait-impl.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

InteractiveViewImplPtr InteractiveViewImpl::New()
{
  return new InteractiveViewImpl();
}

void InteractiveViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  mCachedTrait = EnsureInteractiveTrait();
}

Ui::InteractiveTrait InteractiveViewImpl::GetInteractiveTrait() const
{
  return mCachedTrait;
}

InteractiveViewImpl::InteractiveViewImpl()
{
}

InteractiveViewImpl::~InteractiveViewImpl()
{
}

void InteractiveViewImpl::EnableLongPressDetection()
{
  GetImpl(mCachedTrait).EnableLongPressDetection();
}

void InteractiveViewImpl::OnClicked(Ui::View /*view*/, const Ui::InputEvent& /*event*/)
{
}

void InteractiveViewImpl::OnPressedChanged(Ui::View /*view*/, bool /*pressed*/, const Ui::InputEvent& /*event*/)
{
}

bool InteractiveViewImpl::OnLongPressed(Ui::View /*view*/, const Ui::InputEvent& /*event*/)
{
  return false;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali

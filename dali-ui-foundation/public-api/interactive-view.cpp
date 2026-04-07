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
#include <dali-ui-foundation/public-api/interactive-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-view-impl.h>

namespace Dali
{

namespace Ui
{

InteractiveView::InteractiveView()
{
}

InteractiveView InteractiveView::New()
{
  Integration::InteractiveViewImplPtr impl = Integration::InteractiveViewImpl::New();

  InteractiveView handle(*impl);

  impl->Initialize();

  return handle;
}

InteractiveView InteractiveView::DownCast(BaseHandle handle)
{
  return View::DownCast<InteractiveView, Integration::InteractiveViewImpl>(handle);
}

InteractiveView::InteractiveView(const InteractiveView& view) = default;

InteractiveView::InteractiveView(InteractiveView&& rhs) noexcept = default;

InteractiveView::~InteractiveView()
{
}

InteractiveView::InteractiveView(Integration::InteractiveViewImpl& implementation)
: View(implementation)
{
}

InteractiveView::InteractiveView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::InteractiveViewImpl>(internal);
}

// =============================================================================
// Signals
// =============================================================================

Signal<void(View, bool, const InputEvent&)>& InteractiveView::PressedChangedSignal()
{
  return Integration::GetImpl(*this).GetInteractiveTrait().PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveView::PseudoDisabledChangedSignal()
{
  return Integration::GetImpl(*this).GetInteractiveTrait().PseudoDisabledChangedSignal();
}

Signal<void(View, const InputEvent&)>& InteractiveView::ClickedSignal()
{
  return Integration::GetImpl(*this).GetInteractiveTrait().ClickedSignal();
}

Signal<bool(View, const InputEvent&)>& InteractiveView::LongPressedSignal()
{
  return Integration::GetImpl(*this).GetInteractiveTrait().LongPressedSignal();
}

// =============================================================================
// API
// =============================================================================

bool InteractiveView::IsPressed() const
{
  return Integration::GetImpl(*this).GetInteractiveTrait().IsPressed();
}

bool InteractiveView::IsPseudoDisabled() const
{
  return Integration::GetImpl(*this).GetInteractiveTrait().IsPseudoDisabled();
}

InteractiveView& InteractiveView::SetPseudoDisabled(bool pseudoDisabled)
{
  Integration::GetImpl(*this).GetInteractiveTrait().SetPseudoDisabled(pseudoDisabled);
  return *this;
}

bool InteractiveView::IsClickable() const
{
  return Integration::GetImpl(*this).GetInteractiveTrait().IsClickable();
}

InteractiveView& InteractiveView::SetClickable(bool clickable)
{
  Integration::GetImpl(*this).GetInteractiveTrait().SetClickable(clickable);
  return *this;
}

KeyClickPolicy InteractiveView::GetKeyClickPolicy() const
{
  return Integration::GetImpl(*this).GetInteractiveTrait().GetKeyClickPolicy();
}

InteractiveView& InteractiveView::SetKeyClickPolicy(KeyClickPolicy policy)
{
  Integration::GetImpl(*this).GetInteractiveTrait().SetKeyClickPolicy(policy);
  return *this;
}

} // namespace Ui

} // namespace Dali

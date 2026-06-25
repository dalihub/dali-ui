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
#include <dali-ui-foundation/provider-api/interactive-view-impl.h>
#include <dali-ui-foundation/public-api/interactive-view.h>

namespace Dali
{

namespace Ui
{

InteractiveView::InteractiveView()
{
}

InteractiveView InteractiveView::New()
{
  Provider::InteractiveViewImplPtr impl = Provider::InteractiveViewImpl::New();

  InteractiveView handle(*impl);

  impl->Initialize();

  return handle;
}

InteractiveView InteractiveView::DownCast(BaseHandle handle)
{
  return View::DownCast<InteractiveView, Provider::InteractiveViewImpl>(handle);
}

InteractiveView::InteractiveView(const InteractiveView& view) = default;

InteractiveView::InteractiveView(InteractiveView&& rhs) noexcept = default;

InteractiveView::~InteractiveView()
{
}

InteractiveView::InteractiveView(Provider::InteractiveViewImpl& implementation)
: View(implementation)
{
}

InteractiveView::InteractiveView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Provider::InteractiveViewImpl>(internal);
}

Signal<void(View, bool, InputEvent)>& InteractiveView::PressedChangedSignal()
{
  return Provider::GetImpl(*this).PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveView::PseudoDisabledChangedSignal()
{
  return Provider::GetImpl(*this).PseudoDisabledChangedSignal();
}

Signal<void(View, InputEvent)>& InteractiveView::ClickedSignal()
{
  return Provider::GetImpl(*this).ClickedSignal();
}

Signal<bool(View, InputEvent)>& InteractiveView::LongPressedSignal()
{
  return Provider::GetImpl(*this).LongPressedSignal();
}

bool InteractiveView::IsPressed() const
{
  return Provider::GetImpl(*this).IsPressed();
}

bool InteractiveView::IsPseudoDisabled() const
{
  return Provider::GetImpl(*this).IsPseudoDisabled();
}

void InteractiveView::SetPseudoDisabled(bool pseudoDisabled)
{
  Provider::GetImpl(*this).SetPseudoDisabled(pseudoDisabled);
}

bool InteractiveView::IsClickable() const
{
  return Provider::GetImpl(*this).IsClickable();
}

void InteractiveView::SetClickable(bool clickable)
{
  Provider::GetImpl(*this).SetClickable(clickable);
}

KeyClickPolicy InteractiveView::GetKeyClickPolicy() const
{
  return Provider::GetImpl(*this).GetKeyClickPolicy();
}

void InteractiveView::SetKeyClickPolicy(KeyClickPolicy policy)
{
  Provider::GetImpl(*this).SetKeyClickPolicy(policy);
}

} // namespace Ui

} // namespace Dali

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
#include <dali-ui-foundation/extension-api/interactive-view-impl.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>

namespace Dali
{

namespace Ui
{

InteractiveView::InteractiveView()
{
}

InteractiveView InteractiveView::New()
{
  Extension::InteractiveViewImplPtr impl = Extension::InteractiveViewImpl::New();

  InteractiveView handle(*impl);

  impl->Initialize();

  return handle;
}

InteractiveView InteractiveView::DownCast(BaseHandle handle)
{
  return View::DownCast<InteractiveView, Extension::InteractiveViewImpl>(handle);
}

InteractiveView::InteractiveView(const InteractiveView& view) = default;

InteractiveView::InteractiveView(InteractiveView&& rhs) noexcept = default;

InteractiveView::~InteractiveView()
{
}

InteractiveView::InteractiveView(Extension::InteractiveViewImpl& implementation)
: View(implementation)
{
}

InteractiveView::InteractiveView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Extension::InteractiveViewImpl>(internal);
}

Signal<void(View, bool, InputEvent)>& InteractiveView::PressedChangedSignal()
{
  return Extension::GetImpl(*this).PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveView::PseudoDisabledChangedSignal()
{
  return Extension::GetImpl(*this).PseudoDisabledChangedSignal();
}

Signal<void(View, InputEvent)>& InteractiveView::ClickedSignal()
{
  return Extension::GetImpl(*this).ClickedSignal();
}

Signal<bool(View, InputEvent)>& InteractiveView::LongPressedSignal()
{
  return Extension::GetImpl(*this).LongPressedSignal();
}

Signal<void(View, bool, InputEvent)>& InteractiveView::HoveredChangedSignal()
{
  return Extension::GetImpl(*this).HoveredChangedSignal();
}

bool InteractiveView::IsPressed() const
{
  return Extension::GetImpl(*this).IsPressed();
}

bool InteractiveView::IsPseudoDisabled() const
{
  return Extension::GetImpl(*this).IsPseudoDisabled();
}

bool InteractiveView::IsHovered() const
{
  return Extension::GetImpl(*this).IsHovered();
}

void InteractiveView::SetPseudoDisabled(bool pseudoDisabled)
{
  Extension::GetImpl(*this).SetPseudoDisabled(pseudoDisabled);
}

bool InteractiveView::IsClickable() const
{
  return Extension::GetImpl(*this).IsClickable();
}

void InteractiveView::SetClickable(bool clickable)
{
  Extension::GetImpl(*this).SetClickable(clickable);
}

bool InteractiveView::IsLongPressEnabled() const
{
  return Extension::GetImpl(*this).IsLongPressEnabled();
}

void InteractiveView::SetLongPressEnabled(bool enabled)
{
  Extension::GetImpl(*this).SetLongPressEnabled(enabled);
}

KeyClickPolicy InteractiveView::GetKeyClickPolicy() const
{
  return Extension::GetImpl(*this).GetKeyClickPolicy();
}

void InteractiveView::SetKeyClickPolicy(KeyClickPolicy policy)
{
  Extension::GetImpl(*this).SetKeyClickPolicy(policy);
}

} // namespace Ui

} // namespace Dali

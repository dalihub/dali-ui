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
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/interactive-view-impl.h>
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>

namespace Dali
{

namespace Ui
{
namespace Extension
{

namespace
{

BaseHandle Create()
{
  return InteractiveView::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(InteractiveViewImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

InteractiveViewImplPtr InteractiveViewImpl::New()
{
  return new InteractiveViewImpl();
}

void InteractiveViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  EnsureInteractiveTrait();
}

Signal<void(View, bool, InputEvent)>& InteractiveViewImpl::PressedChangedSignal()
{
  return GetInteractiveTrait().PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveViewImpl::PseudoDisabledChangedSignal()
{
  return GetInteractiveTrait().PseudoDisabledChangedSignal();
}

Signal<void(View, InputEvent)>& InteractiveViewImpl::ClickedSignal()
{
  return GetInteractiveTrait().ClickedSignal();
}

Signal<bool(View, InputEvent)>& InteractiveViewImpl::LongPressedSignal()
{
  return GetInteractiveTrait().LongPressedSignal();
}

bool InteractiveViewImpl::IsPressed() const
{
  return GetInteractiveTrait().IsPressed();
}

bool InteractiveViewImpl::IsPseudoDisabled() const
{
  return GetInteractiveTrait().IsPseudoDisabled();
}

void InteractiveViewImpl::SetPseudoDisabled(bool pseudoDisabled)
{
  GetInteractiveTrait().SetPseudoDisabled(pseudoDisabled);
}

bool InteractiveViewImpl::IsClickable() const
{
  return GetInteractiveTrait().IsClickable();
}

void InteractiveViewImpl::SetClickable(bool clickable)
{
  GetInteractiveTrait().SetClickable(clickable);
}

KeyClickPolicy InteractiveViewImpl::GetKeyClickPolicy() const
{
  return GetInteractiveTrait().GetKeyClickPolicy();
}

void InteractiveViewImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetInteractiveTrait().SetKeyClickPolicy(policy);
}

InteractiveTrait InteractiveViewImpl::GetInteractiveTrait() const
{
  auto* traitObject = Internal::ViewDataImpl::Get(*this).GetCoreInteractionObject();
  DALI_ASSERT_ALWAYS(traitObject && "InteractiveViewImpl requires InteractiveTrait");

  InteractiveTrait trait = InteractiveTrait::DownCast(BaseHandle(static_cast<BaseObject*>(traitObject)));
  DALI_ASSERT_ALWAYS(trait && "InteractiveViewImpl requires InteractiveTrait");

  return trait;
}

InteractiveViewImpl::InteractiveViewImpl()
{
}

InteractiveViewImpl::~InteractiveViewImpl()
{
}

} // namespace Extension
} // namespace Ui

} // namespace Dali

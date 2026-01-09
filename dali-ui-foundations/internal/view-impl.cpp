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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-foundations/internal/view-impl.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

UI::View View::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Internal::View> impl = new Internal::View();

  // Pass ownership to CustomActor handle
  UI::View handle(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

View::View()
: Toolkit::Internal::Control(Toolkit::Internal::Control::CONTROL_BEHAVIOUR_DEFAULT)
{
}

View::~View()
{
}

void View::OnInitialize()
{
  // Call base class initialization
  Toolkit::Internal::Control::OnInitialize();
}

bool View::OnKeyEvent(const KeyEvent& event)
{
  // Call base class implementation
  return Toolkit::Internal::Control::OnKeyEvent(event);
}

} // namespace Internal

} // namespace UI

} // namespace Dali

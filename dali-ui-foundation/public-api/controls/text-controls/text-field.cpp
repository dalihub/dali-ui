/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/controls/text-controls/text-field.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{
namespace UI
{
TextField TextField::New()
{
  return Internal::TextField::New();
}

TextField TextField::New(ControlBehaviour additionalBehaviour)
{
  return Internal::TextField::New(static_cast<UI::Internal::Control::ControlBehaviour>(additionalBehaviour));
}

TextField::TextField()
{
}

TextField::TextField(const TextField& handle) = default;

TextField::TextField(TextField&& rhs) noexcept = default;

TextField& TextField::operator=(const TextField& handle) = default;

TextField& TextField::operator=(TextField&& rhs) noexcept = default;

TextField::~TextField()
{
}

TextField TextField::DownCast(BaseHandle handle)
{
  return Control::DownCast<TextField, Internal::TextField>(handle);
}

TextField::TextChangedSignalType& TextField::TextChangedSignal()
{
  return Dali::UI::GetImpl(*this).TextChangedSignal();
}

TextField::MaxLengthReachedSignalType& TextField::MaxLengthReachedSignal()
{
  return Dali::UI::GetImpl(*this).MaxLengthReachedSignal();
}

TextField::InputStyleChangedSignalType& TextField::InputStyleChangedSignal()
{
  return Dali::UI::GetImpl(*this).InputStyleChangedSignal();
}

TextField::TextField(Internal::TextField& implementation)
  : Control(implementation)
{
}

TextField::TextField(Dali::Internal::CustomActor* internal)
  : Control(internal)
{
  VerifyCustomActorPointer<Internal::TextField>(internal);
}

} // namespace UI

} // namespace Dali

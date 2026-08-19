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
#include <dali-ui-foundation/internal/animation/input-field-animation-spec-impl.autogen.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
Dali::TypeRegistration INPUT_FIELD_ANIMATION_SPEC_TYPE(typeid(Dali::Ui::InputFieldAnimationSpec), typeid(Dali::Ui::Internal::InputFieldAnimationSpecImpl), typeid(Dali::Ui::ViewAnimationSpec), nullptr);

Dali::Ui::Integration::InputFieldImpl* GetInputFieldImpl(InputField inputField)
{
  if(!inputField)
  {
    return nullptr;
  }

  Dali::RefObject& handle = inputField.GetImplementation();
  return &static_cast<Dali::Ui::Integration::InputFieldImpl&>(handle);
}
} // namespace

InputFieldAnimationSpecImpl::InputFieldAnimationSpecImpl() = default;

InputFieldAnimationSpecImpl::~InputFieldAnimationSpecImpl() = default;

InputFieldAnimationSpecImplPtr InputFieldAnimationSpecImpl::New()
{
  return InputFieldAnimationSpecImplPtr(new InputFieldAnimationSpecImpl());
}

void InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetTo(Animation& animation, InputField inputField, const Entry& entry)
{
  if(auto* inputFieldImpl = GetInputFieldImpl(inputField))
  {
    const Property::Index index = inputFieldImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(inputField, index), entry.value, entry.alpha, period);
  }
}

void InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetBy(Animation& animation, InputField inputField, const Entry& entry)
{
  if(auto* inputFieldImpl = GetInputFieldImpl(inputField))
  {
    const Property::Index index = inputFieldImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(inputField, index), entry.value, entry.alpha, period);
  }
}

void InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(Animation& animation, InputField inputField, const Entry& entry)
{
  if(auto* inputFieldImpl = GetInputFieldImpl(inputField))
  {
    const Property::Index index = inputFieldImpl->EnsurePlaceholderGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(inputField, index), entry.value, entry.alpha, period);
  }
}

void InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(Animation& animation, InputField inputField, const Entry& entry)
{
  if(auto* inputFieldImpl = GetInputFieldImpl(inputField))
  {
    const Property::Index index = inputFieldImpl->EnsurePlaceholderGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(inputField, index), entry.value, entry.alpha, period);
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

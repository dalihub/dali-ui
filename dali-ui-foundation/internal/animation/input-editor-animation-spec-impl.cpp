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
#include <dali-ui-foundation/internal/animation/input-editor-animation-spec-impl.autogen.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
Dali::TypeRegistration INPUT_EDITOR_ANIMATION_SPEC_TYPE(typeid(Dali::Ui::InputEditorAnimationSpec), typeid(Dali::Ui::Internal::InputEditorAnimationSpecImpl), typeid(Dali::Ui::ViewAnimationSpec), nullptr);

Dali::Ui::Integration::InputEditorImpl* GetInputEditorImpl(InputEditor inputEditor)
{
  if(!inputEditor)
  {
    return nullptr;
  }

  Dali::RefObject& handle = inputEditor.GetImplementation();
  return &static_cast<Dali::Ui::Integration::InputEditorImpl&>(handle);
}
} // namespace

InputEditorAnimationSpecImpl::InputEditorAnimationSpecImpl() = default;

InputEditorAnimationSpecImpl::~InputEditorAnimationSpecImpl() = default;

InputEditorAnimationSpecImplPtr InputEditorAnimationSpecImpl::New()
{
  return InputEditorAnimationSpecImplPtr(new InputEditorAnimationSpecImpl());
}

void InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetTo(Animation& animation, InputEditor inputEditor, const Entry& entry)
{
  if(auto* inputEditorImpl = GetInputEditorImpl(inputEditor))
  {
    const Property::Index index = inputEditorImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(inputEditor, index), entry.value, entry.alpha, period);
  }
}

void InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetBy(Animation& animation, InputEditor inputEditor, const Entry& entry)
{
  if(auto* inputEditorImpl = GetInputEditorImpl(inputEditor))
  {
    const Property::Index index = inputEditorImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(inputEditor, index), entry.value, entry.alpha, period);
  }
}

void InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(Animation& animation, InputEditor inputEditor, const Entry& entry)
{
  if(auto* inputEditorImpl = GetInputEditorImpl(inputEditor))
  {
    const Property::Index index = inputEditorImpl->EnsurePlaceholderGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(inputEditor, index), entry.value, entry.alpha, period);
  }
}

void InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(Animation& animation, InputEditor inputEditor, const Entry& entry)
{
  if(auto* inputEditorImpl = GetInputEditorImpl(inputEditor))
  {
    const Property::Index index = inputEditorImpl->EnsurePlaceholderGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(inputEditor, index), entry.value, entry.alpha, period);
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

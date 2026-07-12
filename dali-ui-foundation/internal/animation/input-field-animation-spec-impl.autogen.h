#pragma once

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/animation/input-field-animation-spec.autogen.h>

namespace Dali
{
namespace Ui
{

class InputField;

namespace Internal
{

class InputFieldAnimationSpecImpl;
using InputFieldAnimationSpecImplPtr = IntrusivePtr<InputFieldAnimationSpecImpl>;

/**
 * @brief Internal implementation of InputFieldAnimationSpec.
 *
 * Inherits from ViewAnimationSpecImpl; reuses AddAnimateTo/ByEntry, ApplyEntries, ApplyAnimateTo/By.
 */
class DALI_UI_API InputFieldAnimationSpecImpl : public ViewAnimationSpecImpl
{
public:
  static InputFieldAnimationSpecImplPtr New();

  static void ApplyTextGradientStartOffsetTo(Animation& animation, InputField view, const Entry& entry);
  static void ApplyTextGradientStartOffsetBy(Animation& animation, InputField view, const Entry& entry);
  static void ApplyPlaceholderTextGradientStartOffsetTo(Animation& animation, InputField view, const Entry& entry);
  static void ApplyPlaceholderTextGradientStartOffsetBy(Animation& animation, InputField view, const Entry& entry);

protected:
  InputFieldAnimationSpecImpl();
  ~InputFieldAnimationSpecImpl() override;

private:
  InputFieldAnimationSpecImpl(const InputFieldAnimationSpecImpl&) = delete;
  InputFieldAnimationSpecImpl(InputFieldAnimationSpecImpl&&) = delete;
  InputFieldAnimationSpecImpl& operator=(const InputFieldAnimationSpecImpl&) = delete;
  InputFieldAnimationSpecImpl& operator=(InputFieldAnimationSpecImpl&&) = delete;
};

inline Internal::InputFieldAnimationSpecImpl& GetImpl(InputFieldAnimationSpec& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::InputFieldAnimationSpecImpl&>(handle);
}

inline const Internal::InputFieldAnimationSpecImpl& GetImpl(const InputFieldAnimationSpec& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::InputFieldAnimationSpecImpl&>(handle);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

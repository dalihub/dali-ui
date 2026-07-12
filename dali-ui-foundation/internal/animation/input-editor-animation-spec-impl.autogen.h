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
#include <dali-ui-foundation/public-api/animation/input-editor-animation-spec.autogen.h>

namespace Dali
{
namespace Ui
{

class InputEditor;

namespace Internal
{

class InputEditorAnimationSpecImpl;
using InputEditorAnimationSpecImplPtr = IntrusivePtr<InputEditorAnimationSpecImpl>;

/**
 * @brief Internal implementation of InputEditorAnimationSpec.
 *
 * Inherits from ViewAnimationSpecImpl; reuses AddAnimateTo/ByEntry, ApplyEntries, ApplyAnimateTo/By.
 */
class DALI_UI_API InputEditorAnimationSpecImpl : public ViewAnimationSpecImpl
{
public:
  static InputEditorAnimationSpecImplPtr New();

  static void ApplyTextGradientStartOffsetTo(Animation& animation, InputEditor view, const Entry& entry);
  static void ApplyTextGradientStartOffsetBy(Animation& animation, InputEditor view, const Entry& entry);
  static void ApplyPlaceholderTextGradientStartOffsetTo(Animation& animation, InputEditor view, const Entry& entry);
  static void ApplyPlaceholderTextGradientStartOffsetBy(Animation& animation, InputEditor view, const Entry& entry);

protected:
  InputEditorAnimationSpecImpl();
  ~InputEditorAnimationSpecImpl() override;

private:
  InputEditorAnimationSpecImpl(const InputEditorAnimationSpecImpl&) = delete;
  InputEditorAnimationSpecImpl(InputEditorAnimationSpecImpl&&) = delete;
  InputEditorAnimationSpecImpl& operator=(const InputEditorAnimationSpecImpl&) = delete;
  InputEditorAnimationSpecImpl& operator=(InputEditorAnimationSpecImpl&&) = delete;
};

inline Internal::InputEditorAnimationSpecImpl& GetImpl(InputEditorAnimationSpec& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::InputEditorAnimationSpecImpl&>(handle);
}

inline const Internal::InputEditorAnimationSpecImpl& GetImpl(const InputEditorAnimationSpec& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::InputEditorAnimationSpecImpl&>(handle);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class FocusManager;
struct FocusIndicationContextImpl;
} // namespace Internal

namespace Extension
{

/**
 * @brief Read-only information about one automatic focus indication decision.
 *
 * When focus changes, GetPreviousFocusView() loses focus and GetFocusedView()
 * gains focus. For indication-only changes both refer to the focused View.
 * If the previous View is empty, IsPreviousFocusIndicated() returns false.
 *
 * The context and references returned by its getters are valid only during
 * the policy invocation. Do not retain their addresses or references. Copy
 * individual View or InputEvent handles if they are needed after the callback.
 * View handles preserve object lifetime, not a snapshot of mutable View state.
 *
 * @note FocusManager owns the implementation on its call stack. Keeping only
 *       an opaque pointer here permits new decision data without changing the
 *       extension ABI or allocating a context body on the heap. Future fields
 *       belong in the internal implementation, with out-of-line getters here.
 */
class DALI_UI_API FocusIndicationContext
{
public:
  FocusIndicationContext(const FocusIndicationContext&)            = delete;
  FocusIndicationContext(FocusIndicationContext&&)                 = delete;
  FocusIndicationContext& operator=(const FocusIndicationContext&) = delete;
  FocusIndicationContext& operator=(FocusIndicationContext&&)      = delete;
  ~FocusIndicationContext()                                        = default;

  /**
   * @brief Gets the previous focused View, or an empty handle if none.
   * @return A reference valid only during the policy invocation
   */
  const Dali::Ui::View& GetPreviousFocusView() const;

  /**
   * @brief Gets the View that will be focused after this decision.
   * @return A reference valid only during the policy invocation
   */
  const Dali::Ui::View& GetFocusedView() const;

  /**
   * @brief Gets the device associated with this decision.
   */
  FocusDevice GetDevice() const;

  /**
   * @brief Gets the input event associated with this decision.
   * @return A reference valid only during the policy invocation
   */
  const InputEvent& GetInputEvent() const;

  /**
   * @brief Checks whether the previous View was focus indicated.
   */
  bool IsPreviousFocusIndicated() const;

  /**
   * @brief Gets the framework's proposed focus indication state.
   */
  bool IsFocusIndicationProposed() const;

private:
  DALI_INTERNAL explicit FocusIndicationContext(const Internal::FocusIndicationContextImpl& impl);

  const Internal::FocusIndicationContextImpl* mImpl;

  friend class Internal::FocusManager;
};

namespace FocusIndicationPolicy
{
/**
 * @brief Function pointer type for resolving automatic focus indication changes.
 *
 * The returned value becomes the final FOCUS_INDICATED state.
 *
 * @note This is a plain function pointer. Capturing lambdas and non-static
 *       member functions are not accepted.
 */
using Function = bool (*)(const FocusIndicationContext& context);

/**
 * @brief Uses the framework's proposed focus indication state.
 *
 * @param[in] context Information about the focus indication decision
 * @return FocusIndicationContext::IsFocusIndicationProposed()
 */
DALI_UI_API bool Default(const FocusIndicationContext& context);

/**
 * @brief Selects focus indication for every automatic decision while a view is focused.
 *
 * Explicit focus indication changes, such as
 * FocusManager::ClearFocusIndication(), are not overridden.
 *
 * @param[in] context Information about the focus indication decision
 * @return True
 */
DALI_UI_API bool FollowFocus(const FocusIndicationContext& context);

} // namespace FocusIndicationPolicy
} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE

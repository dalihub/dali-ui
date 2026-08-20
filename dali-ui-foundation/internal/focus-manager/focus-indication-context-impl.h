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

#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

// Each policy invocation owns its data on the stack. Handle copies keep the
// decision's objects alive even when the callback removes Views or reenters
// focus processing. Do not replace these with references to mutable manager
// state or reuse a single shared implementation across nested invocations.
struct FocusIndicationContextImpl
{
  View        previousFocusView;
  View        focusedView;
  FocusDevice device;
  InputEvent  inputEvent;
  bool        previousFocusIndicated;
  bool        focusIndicationProposed;
};

} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE

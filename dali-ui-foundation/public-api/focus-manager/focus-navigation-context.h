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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>

namespace Dali
{
class Window;

namespace Ui
{
class View;

namespace Internal
{
class FocusManager;
class FocusNavigationContextImpl;
} // namespace Internal

/**
 * @brief Immutable information about one focus-navigation request.
 *
 * Instances are created by FocusManager and are valid for the callback and
 * virtual-policy invocations made for that request.
 */
class DALI_UI_API FocusNavigationContext : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized context handle.
   */
  FocusNavigationContext() = default;

  /**
   * @brief Gets the requested navigation direction.
   */
  FocusDirection GetDirection() const;

  /**
   * @brief Gets the device category that initiated navigation.
   */
  FocusDevice GetDevice() const;

  /**
   * @brief Gets the input device name, if available.
   */
  const Dali::String& GetDeviceName() const;

  /**
   * @brief Gets the originating input event.
   */
  InputEvent GetInputEvent() const;

  /**
   * @brief Gets the Window in which this navigation request is resolved.
   */
  Window GetWindow() const;

  /**
   * @brief Gets the closest Focus Group containing the current focus.
   *
   * An empty View means that navigation is scoped only to GetWindow().
   */
  View GetFocusGroup() const;

private:
  explicit FocusNavigationContext(Internal::FocusNavigationContextImpl* impl);

  friend class Internal::FocusManager;
};

} // namespace Ui
} // namespace Dali

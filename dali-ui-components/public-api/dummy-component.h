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

// EXTERNAL INCLUDES
#include <functional>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

// INTERNAL INCLUDES

// None

namespace Dali
{

namespace Ui
{

// Forward declarations
namespace Internal
{
class DummyComponentImpl;
}

#include "dummy-component.autogen.h"
/**
 * @brief Dummy
 */
class DALI_UI_API DummyComponent : public View
{
public:

  // Typedefs

public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized DummyComponent handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  DummyComponent();

  /**
   * @brief Creates an initialized DummyComponent.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static DummyComponent New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] DummyComponent Handle to copy
   */
  DummyComponent(const DummyComponent& DummyComponent);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  DummyComponent(DummyComponent&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DummyComponent();

public: // Operators

  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  DummyComponent& operator=(const DummyComponent& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  DummyComponent& operator=(DummyComponent&& rhs) noexcept;

public: // Static Methods

  /**
   * @brief Downcasts a handle to DummyComponent handle.
   *
   * If handle points to a DummyComponent, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a DummyComponent or an uninitialized handle
   */
  static DummyComponent DownCast(BaseHandle handle);

public: // Setters for chaining

  // @CHAIN_START(DummyComponent, View)
  // @CHAIN_END

public: // Signals

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL DummyComponent(Internal::DummyComponentImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL DummyComponent(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(DummyComponent)
};

} // namespace Ui

} // namespace Dali

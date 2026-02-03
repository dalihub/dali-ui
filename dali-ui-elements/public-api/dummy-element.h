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

namespace UI
{

// Forward declarations
namespace Internal
{
class DummyElementImpl;
}

/**
 * @brief Dummy
 */
class DALI_UI_API DummyElement : public View
{
public:

  // Typedefs

public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized DummyElement handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  DummyElement();

  /**
   * @brief Creates an initialized DummyElement.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static DummyElement New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] DummyElement Handle to copy
   */
  DummyElement(const DummyElement& DummyElement);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  DummyElement(DummyElement&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DummyElement();

public: // Operators

  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  DummyElement& operator=(const DummyElement& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  DummyElement& operator=(DummyElement&& rhs) noexcept;

public: // Static Methods

  /**
   * @brief Downcasts a handle to DummyElement handle.
   *
   * If handle points to a DummyElement, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a DummyElement or an uninitialized handle
   */
  static DummyElement DownCast(BaseHandle handle);

public: // API

  DEFINE_CHAINING_METHOD_VIEW(DummyElement)

public: // Signals

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL DummyElement(Internal::DummyElementImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL DummyElement(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace UI

} // namespace Dali

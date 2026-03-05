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
#include <dali-ui-foundation/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
// None

namespace Dali
{

namespace UI
{

#ifndef DEFINE_CHAINING_METHOD_VIEW_DERIVED
#define DEFINE_CHAINING_METHOD_VIEW_DERIVED(ClassType) DEFINE_CHAINING_METHOD_VIEW(ClassType)
#endif

/**
 * @brief Sample class to show method chaining
 *
 */
class DALI_UI_API ViewDerived : public View
{
public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized View handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  ViewDerived();

  /**
   * @brief Creates an initialized ViewDerived.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static ViewDerived New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] view Handle to copy
   */
  ViewDerived(const ViewDerived& viewDerived);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  ViewDerived(ViewDerived&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ViewDerived();

public: // Operators

  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  ViewDerived& operator=(const ViewDerived& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  ViewDerived& operator=(ViewDerived&& rhs) noexcept;

public: // Static Methods

  /**
   * @brief Downcasts a handle to View handle.
   *
   * If handle points to a View, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a View or an uninitialized handle
   */
  static ViewDerived DownCast(BaseHandle handle);

public: // API

  DEFINE_CHAINING_METHOD_VIEW_DERIVED(ViewDerived)

public: // Signals

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL ViewDerived(Internal::View& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL ViewDerived(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace UI

} // namespace Dali

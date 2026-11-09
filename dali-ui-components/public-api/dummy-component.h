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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <functional>

namespace DALI_NAMESPACE
{

namespace Ui
{
namespace Internal
{
class DummyComponentImpl;
}

/**
 * @brief A minimal example type demonstrating a View-based component structure.
 *
 * DummyComponent has no dedicated behavior, style, or signals.
 * Use View::New() for a general-purpose visual element or child view container.
 * Its public header and creation and casting APIs are retained for compatibility with existing client code.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API DummyComponent : public View
{
public:
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized DummyComponent handle.
   *
   * Use New() to create the underlying object; no derived type is required.
   * Do not call member functions that access the underlying object on an uninitialized handle.
   * @SINCE_3_0.0
   */
  DummyComponent();

  /**
   * @brief Creates an initialized DummyComponent.
   *
   * @SINCE_3_0.0
   * @return A handle to the newly created object. Default behavior is inherited from View.
   */
  static DummyComponent New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @SINCE_3_0.0
   * @param[in] DummyComponent Handle to copy
   */
  DummyComponent(const DummyComponent& DummyComponent);

  /**
   * @brief Move constructor.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Handle to move
   */
  DummyComponent(DummyComponent&& rhs) noexcept;

  /**
   * @brief Destroys the DummyComponent handle.
   *
   * This destructor is non-virtual. Do not add data or virtual functions to derived handle types.
   * @SINCE_3_0.0
   */
  ~DummyComponent();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_3_0.0
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  DummyComponent& operator=(const DummyComponent& handle);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  DummyComponent& operator=(DummyComponent&& rhs) noexcept;

  DALI_UI_VIEW_WITH(DummyComponent)

public: // Static Methods
  /**
   * @brief Downcasts a handle to DummyComponent handle.
   *
   * If handle points to a DummyComponent, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a DummyComponent or an uninitialized handle
   */
  static DummyComponent DownCast(BaseHandle handle);

public: // Setters for chaining
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
};

} // namespace Ui

} //namespace DALI_NAMESPACE

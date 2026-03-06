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
#include <dali-ui-foundation/integration-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/dummy-component.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for DummyImpl.
 *
 * DummyComponentImpl extends ControlImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the DummyImpl public API.
 *
 * @see Dali::Ui::DummyImpl
 */
class DummyComponentImpl : public Integration::ViewImpl
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new DummyImpl.
   */
  static Ui::DummyComponent New();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DummyComponentImpl();

public:

  // API

protected:

  // Construction

  /**
   * @brief DummyImpl constructor.
   *
   */
  DummyComponentImpl();

private:

  // Not copyable or movable
  DummyComponentImpl(const DummyComponentImpl&) = delete;
  DummyComponentImpl(DummyComponentImpl&&) = delete;
  DummyComponentImpl& operator=(const DummyComponentImpl&) = delete;
  DummyComponentImpl& operator=(DummyComponentImpl&&) = delete;

private:

  // Data
};

// Helpers for public-api forwarding methods

inline Internal::DummyComponentImpl& GetImpl(Ui::DummyComponent& dummyElement)
{
  DALI_ASSERT_ALWAYS(dummyElement);

  Dali::RefObject& handle = dummyElement.GetImplementation();

  return static_cast<Internal::DummyComponentImpl&>(handle);
}

inline const Internal::DummyComponentImpl& GetImpl(const Ui::DummyComponent& dummyElement)
{
  DALI_ASSERT_ALWAYS(dummyElement);

  const Dali::RefObject& handle = dummyElement.GetImplementation();

  return static_cast<const Internal::DummyComponentImpl&>(handle);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali

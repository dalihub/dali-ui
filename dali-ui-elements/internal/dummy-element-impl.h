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
#include <dali-ui-elements/public-api/dummy-element.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for DummyImpl.
 *
 * DummyElementImpl extends ControlImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the DummyImpl public API.
 *
 * @see Dali::Ui::DummyImpl
 */
class DummyElementImpl : public Integration::ViewImpl
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new DummyImpl.
   */
  static Ui::DummyElement New();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DummyElementImpl();

public:

  // API

protected:

  // Construction

  /**
   * @brief DummyImpl constructor.
   *
   */
  DummyElementImpl();

private:

  // Not copyable or movable
  DummyElementImpl(const DummyElementImpl&) = delete;
  DummyElementImpl(DummyElementImpl&&) = delete;
  DummyElementImpl& operator=(const DummyElementImpl&) = delete;
  DummyElementImpl& operator=(DummyElementImpl&&) = delete;

private:

  // Data
};

// Helpers for public-api forwarding methods

inline Internal::DummyElementImpl& GetImpl(Ui::DummyElement& dummyElement)
{
  DALI_ASSERT_ALWAYS(dummyElement);

  Dali::RefObject& handle = dummyElement.GetImplementation();

  return static_cast<Internal::DummyElementImpl&>(handle);
}

inline const Internal::DummyElementImpl& GetImpl(const Ui::DummyElement& dummyElement)
{
  DALI_ASSERT_ALWAYS(dummyElement);

  const Dali::RefObject& handle = dummyElement.GetImplementation();

  return static_cast<const Internal::DummyElementImpl&>(handle);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali

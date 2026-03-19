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
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/layout.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{

class LayoutImpl;
using LayoutImplPtr = IntrusivePtr<LayoutImpl>;

/**
 * @brief This is the internal implementation class for Layout.
 *
 * Layout is a convenience class that extends View with a pre-configured
 * LayoutManager.
 *
 * Since View now supports optional LayoutManager and child management,
 * Layout simply ensures a LayoutManager is always set up and provides
 * a factory method for derived classes to specify their layout algorithm.
 *
 * @see Integration::ViewImpl for LayoutManager and child management functionality
 */
class DALI_UI_API LayoutImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new Layout implementation.
   *
   * @return An IntrusivePtr to the new LayoutImpl
   */
  static LayoutImplPtr New();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LayoutImpl();

  /**
   * @brief LayoutImpl constructor.
   *
   * @param[in] layoutManager The layout manager for this layout (ownership transferred).
   *            Derived classes must pass their specific LayoutManager.
   */
  explicit LayoutImpl(LayoutManager* layoutManager = nullptr);

private:
  // Not copyable or movable
  LayoutImpl(const LayoutImpl&)            = delete;
  LayoutImpl(LayoutImpl&&)                 = delete;
  LayoutImpl& operator=(const LayoutImpl&) = delete;
  LayoutImpl& operator=(LayoutImpl&&)      = delete;
};

// Helpers for public-api forwarding methods
inline Integration::LayoutImpl& GetImpl(Ui::Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::LayoutImpl&>(handle);
}

inline const Integration::LayoutImpl& GetImpl(const Ui::Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::LayoutImpl&>(handle);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

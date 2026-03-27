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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class StackLayoutImpl;
using StackLayoutImplPtr = IntrusivePtr<StackLayoutImpl>;

/**
 * @brief This is the internal implementation class for StackLayout.
 */
class DALI_UI_API StackLayoutImpl : public LayoutImpl
{
public:
  /**
   * @brief Creates a new StackLayout implementation.
   *
   * @param[in] orientation The orientation of the stack
   * @return An IntrusivePtr to the new StackLayoutImpl
   */
  static StackLayoutImplPtr New(StackOrientation orientation = StackOrientation::VERTICAL);

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~StackLayoutImpl();

  void OnInitialize() override;

  /**
   * @brief StackLayoutImpl constructor.
   *
   * @param[in] orientation The orientation of the stack
   */
  explicit StackLayoutImpl(StackOrientation orientation);

public: // API
  /**
   * @brief Sets the orientation of the stack layout.
   *
   * @param[in] orientation The orientation to set
   */
  void SetOrientation(StackOrientation orientation);

  /**
   * @brief Gets the orientation of the stack layout.
   *
   * @return The current orientation
   */
  StackOrientation GetOrientation() const;

  /**
   * @brief Sets the spacing between children.
   *
   * @param[in] spacing The spacing to set
   */
  void SetSpacing(float spacing);

  /**
   * @brief Gets the spacing between children.
   *
   * @return The current spacing
   */
  float GetSpacing() const;

private:
  // Not copyable or movable
  StackLayoutImpl(const StackLayoutImpl&)            = delete;
  StackLayoutImpl(StackLayoutImpl&&)                 = delete;
  StackLayoutImpl& operator=(const StackLayoutImpl&) = delete;
  StackLayoutImpl& operator=(StackLayoutImpl&&)      = delete;

private:
  StackOrientation mOrientation;
  float            mSpacing;
};

// Helpers for public-api forwarding methods
inline Integration::StackLayoutImpl& GetImpl(Ui::StackLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::StackLayoutImpl&>(handle);
}

inline const Integration::StackLayoutImpl& GetImpl(const Ui::StackLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::StackLayoutImpl&>(handle);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

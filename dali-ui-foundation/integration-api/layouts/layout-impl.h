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
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class LayoutImpl;
using LayoutImplPtr = IntrusivePtr<LayoutImpl>;

/**
 * @brief Implementation class for the Layout handle.
 *
 * LayoutImpl is the base implementation for layout containers. It does not
 * implement any layout algorithm by itself; layout behaviour is supplied by
 * a LayoutManager attached via View::AttachLayoutManager, or by
 * MeasureCallback / ArrangeCallback set on the View.
 *
 * Layout is preserved as a semantic type for "container view" so that
 * features such as focus child-first and container-only extensions can
 * remain distinct from plain leaf Views.
 */
class DALI_UI_API LayoutImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new Layout implementation without a LayoutManager.
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
   */
  LayoutImpl();

private:
  // Not copyable or movable
  LayoutImpl(const LayoutImpl&)            = delete;
  LayoutImpl(LayoutImpl&&)                 = delete;
  LayoutImpl& operator=(const LayoutImpl&) = delete;
  LayoutImpl& operator=(LayoutImpl&&)      = delete;
};

} // namespace Integration

// Helpers for public-api forwarding methods
inline DALI_UI_API Integration::LayoutImpl& GetImpl(Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::LayoutImpl&>(handle);
}

inline DALI_UI_API const Integration::LayoutImpl& GetImpl(const Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::LayoutImpl&>(handle);
}
} // namespace Ui
} // namespace Dali

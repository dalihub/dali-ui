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
#include <dali-ui-foundation/integration-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class AbsoluteLayoutImpl;
using AbsoluteLayoutImplPtr = IntrusivePtr<AbsoluteLayoutImpl>;

/**
 * @brief This is the internal implementation class for AbsoluteLayout.
 */
class DALI_UI_API AbsoluteLayoutImpl : public LayoutImpl
{
public:
  /**
   * @brief Creates a new AbsoluteLayout implementation.
   *
   * @return An IntrusivePtr to the new AbsoluteLayoutImpl
   */
  static AbsoluteLayoutImplPtr New();

protected:
  virtual ~AbsoluteLayoutImpl();
  AbsoluteLayoutImpl();

private:
  AbsoluteLayoutImpl(const AbsoluteLayoutImpl&)            = delete;
  AbsoluteLayoutImpl(AbsoluteLayoutImpl&&)                 = delete;
  AbsoluteLayoutImpl& operator=(const AbsoluteLayoutImpl&) = delete;
  AbsoluteLayoutImpl& operator=(AbsoluteLayoutImpl&&)      = delete;
};

inline Integration::AbsoluteLayoutImpl& GetImpl(Ui::AbsoluteLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::AbsoluteLayoutImpl&>(handle);
}

inline const Integration::AbsoluteLayoutImpl& GetImpl(const Ui::AbsoluteLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::AbsoluteLayoutImpl&>(handle);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali

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
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class FlexLayoutImpl;
using FlexLayoutImplPtr = IntrusivePtr<FlexLayoutImpl>;

/**
 * @brief This is the internal implementation class for FlexLayout.
 */
class DALI_UI_API FlexLayoutImpl : public LayoutImpl
{
public:
  /**
   * @brief Creates a new FlexLayout implementation.
   *
   * @return An IntrusivePtr to the new FlexLayoutImpl
   */
  static FlexLayoutImplPtr New();

protected:
  virtual ~FlexLayoutImpl();
  void OnInitialize() override;
  FlexLayoutImpl();

public: // API
  void          SetDirection(FlexDirection direction);
  FlexDirection GetDirection() const;

  void     SetWrap(FlexWrap wrap);
  FlexWrap GetWrap() const;

  void        SetJustifyContent(FlexJustify justify);
  FlexJustify GetJustifyContent() const;

  void      SetAlignItems(FlexAlign align);
  FlexAlign GetAlignItems() const;

  void      SetAlignContent(FlexAlign align);
  FlexAlign GetAlignContent() const;

private:
  FlexLayoutImpl(const FlexLayoutImpl&)            = delete;
  FlexLayoutImpl(FlexLayoutImpl&&)                 = delete;
  FlexLayoutImpl& operator=(const FlexLayoutImpl&) = delete;
  FlexLayoutImpl& operator=(FlexLayoutImpl&&)      = delete;
};

} // namespace Integration

inline Integration::FlexLayoutImpl& GetImpl(FlexLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::FlexLayoutImpl&>(handle);
}

inline const Integration::FlexLayoutImpl& GetImpl(const FlexLayout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::FlexLayoutImpl&>(handle);
}
} // namespace Ui
} // namespace Dali

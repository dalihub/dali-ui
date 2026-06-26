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
#include <dali-ui-foundation/provider-api/ui-config-impl.h>

namespace Dali
{
namespace Ui
{
namespace Components
{
namespace Internal
{

class UiConfigImpl;
using UiConfigImplPtr = IntrusivePtr<UiConfigImpl>;

/**
 * @brief Components UiConfig implementation.
 */
class UiConfigImpl : public Dali::Ui::Provider::UiConfigImpl
{
public:
  /**
   * @brief Creates a components UiConfig implementation.
   *
   * @return A new implementation
   */
  static UiConfigImplPtr New();

protected:
  /**
   * @brief Constructor.
   */
  UiConfigImpl();

  /**
   * @brief Destructor.
   */
  ~UiConfigImpl() override;
};

} // namespace Internal
} // namespace Components
} // namespace Ui
} // namespace Dali

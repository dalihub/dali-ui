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
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Polymorphic base for a LayoutManager's implementation storage.
 *
 * A single instance is owned by LayoutManager (base-owned mImpl) and deleted
 * through this virtual destructor, so each concrete manager can subclass it to
 * hold its own state without changing the manager's instance size.
 */
class LayoutManager::Impl
{
public:
  virtual ~Impl() = default;
};

} // namespace Ui
} // namespace Dali

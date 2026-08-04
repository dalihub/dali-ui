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

namespace Dali::Ui
{
class ViewAccessible;
class ViewImpl;

namespace Integration::ViewAccessibility
{
/**
 * @brief Function used by an internal View implementation to create its
 * custom Accessible object.
 */
using AccessibleObjectCreator = ViewAccessible* (*)(Dali::Ui::View);

/**
 * @brief Registers the custom Accessible creator for a View implementation.
 *
 * Passing nullptr restores the default ViewAccessible creator. The creator is
 * consulted only when the accessibility infrastructure first requests an
 * Accessible object, so existing ownership and caching behavior is unchanged.
 *
 * @param[in] viewImpl The View implementation
 * @param[in] creator The creator, or nullptr for the default creator
 */
DALI_UI_API void SetAccessibleObjectCreator(ViewImpl& viewImpl, AccessibleObjectCreator creator);

/**
 * @brief Registers View accessibility object creation with the adaptor.
 *
 * This is idempotent and is called by View initialization.
 */
DALI_UI_API void Register();

} // namespace Integration::ViewAccessibility
} // namespace Dali::Ui

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

class ViewImpl;

namespace IntegrationView
{

/**
 * @brief Adds a raw (non-View) Actor as a child of the given View.
 *
 * View::OnChildAdd normally asserts when the added child cannot be DownCast to
 * Ui::View. This Integration helper bypasses that check so internal modules can
 * attach supporting Actors (clipping stencils, render-effect roots, decoration
 * layers, etc.) without violating the View-only invariant for application code.
 *
 * The added Actor is NOT tracked in the View's child container and therefore is
 * excluded from the View's measure/arrange layout pass. The caller is fully
 * responsible for the Actor's size, position, and lifecycle.
 *
 * @param[in] view  The parent View to add to.
 * @param[in] actor The raw Actor to attach as a child.
 */
DALI_UI_API void AddActorChild(Ui::View view, Dali::Actor actor);

/**
 * @brief Sets trait data on a View.
 *
 * If the stored object implements TraitInterface, lifecycle callbacks
 * (OnBeforeAttached, OnAttached, OnDetached, OnViewDestroying) are invoked.
 * Otherwise the object is stored without callbacks.
 *
 * @param[in] viewImpl The view implementation to attach to
 * @param[in] id The key to identify the trait
 * @param[in] handle The object to store
 */
DALI_UI_API void SetTrait(ViewImpl& viewImpl, TraitId id, Dali::BaseHandle handle);

/**
 * @brief Gets trait data from a View.
 *
 * @param[in] viewImpl The view implementation to query
 * @param[in] id The key to identify the trait
 * @return The stored handle, or an empty handle if not found
 */
DALI_UI_API Dali::BaseHandle GetTrait(const ViewImpl& viewImpl, TraitId id);

/**
 * @brief Gets trait data from a View and DownCasts to the specified handle type.
 *
 * @tparam HandleType A handle class that provides a static DownCast(BaseHandle) method
 * @param[in] viewImpl The view implementation to query
 * @param[in] id The key to identify the trait
 * @return The DownCast handle, or an empty handle if not found or cast fails
 */
template<typename HandleType>
HandleType GetTrait(const ViewImpl& viewImpl, TraitId id)
{
  return HandleType::DownCast(GetTrait(viewImpl, id));
}

/**
 * @brief Removes trait data from a View.
 *
 * @param[in] viewImpl The view implementation to modify
 * @param[in] id The key to identify the trait
 * @return true if the trait was found and removed
 */
DALI_UI_API bool RemoveTrait(ViewImpl& viewImpl, TraitId id);

} // namespace IntegrationView
} // namespace Ui
} // namespace Dali

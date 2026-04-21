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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
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

} // namespace IntegrationView
} // namespace Ui
} // namespace Dali

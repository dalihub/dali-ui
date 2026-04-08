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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/trait-id.h>

// EXTERNAL INCLUDES
#include <atomic>

namespace Dali
{

namespace Ui
{

namespace Integration
{

TraitId TraitId::Alloc()
{
  static std::atomic<uint32_t> counter{0};
  return TraitId{counter.fetch_add(1u, std::memory_order_relaxed)};
}

// --- ReservedTraitId definitions ---
// Defined in a single TU so initialisation order is top-to-bottom guaranteed.
// Framework code always references these by name; the numeric values are internal.

namespace ReservedTraitId
{

const TraitId INTERACTION_TRAIT   = TraitId::Alloc();
const TraitId SELECTABLE_TRAIT    = TraitId::Alloc();
const TraitId STATE_HANDLER_TRAIT = TraitId::Alloc();

const TraitId ABSOLUTE_LAYOUT_PARAMS = TraitId::Alloc();
const TraitId STACK_LAYOUT_PARAMS    = TraitId::Alloc();
const TraitId GRID_LAYOUT_PARAMS     = TraitId::Alloc();
const TraitId FLEX_LAYOUT_PARAMS     = TraitId::Alloc();

const TraitId LAYOUT_MANAGER = TraitId::Alloc();
const TraitId LAYOUT_SIGNALS = TraitId::Alloc();

const TraitId INTERACTION_EFFECT      = TraitId::Alloc();
const TraitId INTERACTION_EFFECT_DATA = TraitId::Alloc();

} // namespace ReservedTraitId

} // namespace Integration

} // namespace Ui

} // namespace Dali

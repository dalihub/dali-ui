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
#include <cstdint>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Reserved identifiers used internally by the framework.
 *
 * These IDs are allocated in the range [0, MAX_RESERVED] to ensure that
 * system-level traits or elements do not conflict with user-defined ones.
 * Application code should always use @c TraitId with raw integers; an offset
 * is automatically applied so that user-defined IDs never overlap with the
 * reserved range.
 */
enum class ReservedTraitId : uint32_t
{
  /**
   * @brief Interaction trait attached to a View.
   *
   * This is used for "interaction traits" such as clickable behavior
   * that define how a View reacts to input focus and key events.
   * A View may have at most one interaction trait, which is attached via
   * ViewImpl::SetTrait(ReservedTraitId::INTERACTION_TRAIT, ...).
   */
  INTERACTION_TRAIT = 0,

  /**
   * @brief Selectable trait attached to a View.
   *
   * This is used for "selectable traits" such as selectable or
   * group-selectable behavior that manage selection state on a View.
   * A View may have at most one selectable trait.
   */
  SELECTABLE_TRAIT = 1,

  /**
   * @brief Named state-change handler trait attached to a View.
   *
   * Manages named callbacks that are invoked when the View's UiState changes.
   * Each handler is identified by a string key and can be individually registered
   * or removed. A View may have at most one of this trait.
   */
  STATE_HANDLER_TRAIT = 2,

  /**
   * @brief Layout parameter traits for each layout type.
   *
   * These are attached to child views to store per-child layout properties
   * (e.g., position, weight, row/column) used by the corresponding layout manager.
   * @{
   */
  ABSOLUTE_LAYOUT_PARAMS = 10,
  STACK_LAYOUT_PARAMS    = 11,
  GRID_LAYOUT_PARAMS     = 12,
  FLEX_LAYOUT_PARAMS     = 13,
  /** @} */

  /**
   * @brief Layout manager trait for attaching a LayoutManager to a View.
   *
   * Only used by LayoutImpl and its derived classes to store the layout
   * algorithm. Accessed during measure/arrange passes.
   */
  LAYOUT_MANAGER = 20,

  /**
   * @brief Layout callback trait for custom measure/arrange behavior.
   *
   * Stores application-provided measure and arrange callbacks that
   * override the default layout algorithm on any View.
   */
  LAYOUT_SIGNALS = 21,

  // Effects
  INTERACTION_EFFECT = 50,

  // Reserved
  MAX_RESERVED = 1000
};

/**
 * @brief A lightweight handle representing a unique identifier for traits.
 * * TraitId acts as a wrapper around a uint32_t. It provides implicit conversion
 * from both ReservedTraitId and raw integers. When a raw integer is provided by
 * the application developer, an offset (1000) is automatically added to
 * prevent collisions with the framework's reserved IDs.
 */
struct DALI_UI_API TraitId
{
  /**
   * @constant OFFSET
   * The boundary between reserved framework IDs and application-defined IDs.
   */
  static constexpr uint32_t OFFSET = static_cast<uint32_t>(ReservedTraitId::MAX_RESERVED) + 1;

  /**
   * @brief Implicit constructor for user-defined IDs.
   * @param id A raw integer ID provided by the user (e.g., 0, 1, 2...).
   */
  TraitId(uint32_t id)
  : value(id + OFFSET)
  {
  }

  /**
   * @brief Implicit constructor for framework-reserved IDs.
   * @param id A specific ReservedTraitId defined by the framework.
   */
  TraitId(ReservedTraitId id)
  : value(static_cast<uint32_t>(id))
  {
  }

  /**
   * @brief Equality operator for efficient lookups in containers.
   */
  bool operator==(const TraitId& other) const
  {
    return value == other.value;
  }

  /**
   * @brief Inequality operator.
   */
  bool operator!=(const TraitId& other) const
  {
    return value != other.value;
  }

  uint32_t value;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali

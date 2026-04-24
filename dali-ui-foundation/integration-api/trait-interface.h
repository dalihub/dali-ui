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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/trait-id.h>

namespace Dali
{
namespace Ui
{

class View;
class ViewImpl;

namespace Internal
{
class ViewDataImpl;
} // namespace Internal

namespace Integration
{

/**
 * @brief Interface for trait lifecycle callbacks.
 *
 * Implement this interface on a BaseObject subclass to receive lifecycle
 * notifications when the object is attached to or detached from a View
 * via IntegrationView::SetTrait().
 *
 * Objects stored as trait data that do NOT implement this interface will
 * simply be stored without any lifecycle callbacks.
 */
class DALI_UI_API TraitInterface
{
public:
  virtual ~TraitInterface() = default;

protected:
  friend class Internal::ViewDataImpl;

  /**
   * @brief Called when the trait is about to being attached to a host view.
   *
   * This method is invoked right before the trait is added to the view's internal storage.
   *
   * @param[in] id The key to identify the trait
   * @param[in] view The host view this trait is being attached to.
   */
  virtual void OnBeforeAttached(TraitId id, View& view) = 0;

  /**
   * @brief Called when the trait is attached to a host view.
   *
   * This method is invoked after the trait is added to the view's internal storage.
   *
   * @param[in] id The id to identify the trait
   * @param[in] view The host view this trait is being attached to.
   */
  virtual void OnAttached(TraitId id, View& view) = 0;

  /**
   * @brief Called when the trait is being detached from the host view.
   *
   * This method is invoked before the trait is removed from the view's storage.
   * Use this to release the strong reference and perform cleanup, such as
   * disconnecting from signals to prevent dangling pointers or leaks.
   *
   * @note This will not be called if the attached view is being destroyed.
   * OnViewDestroying is called instead.
   *
   * @param[in] id The key to identify the trait
   * @param[in] view The host view this trait is being detached from.
   */
  virtual void OnDetached(TraitId id, View& view) = 0;

  /**
   * @brief Called when the attached View is in the process of being destroyed.
   *
   * At this point, member variables of ViewImpl are still accessible,
   * but the object may no longer function as a derived type (vtable shifting).
   *
   * @note Do not create the View handle with the passed object, as it may interfere
   * with the ongoing destruction process.
   *
   * @param[in] viewImpl The implementation object being destroyed.
   */
  virtual void OnViewDestroying(ViewImpl* viewImpl) = 0;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali

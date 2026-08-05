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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

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
 *
 * This storage owns the non-virtual arrange policy so adding the policy changes
 * neither LayoutManager's frozen virtual API nor its public instance size. External
 * subclasses use LayoutManager's protected SetArrangePolicy() entry; the default
 * constructor and every in-library implementation share this same storage contract.
 */
class LayoutManager::Impl
{
public:
  virtual ~Impl() = default;

  /**
   * @brief Updates this manager's arrange execution policy.
   *
   * @param[in] policy The new policy
   * @return True when the stored policy changed
   */
  bool SetArrangePolicy(ArrangePolicy policy)
  {
    if(mArrangePolicy == policy)
    {
      return false;
    }
    mArrangePolicy = policy;
    return true;
  }

  /**
   * @brief Returns this manager's arrange execution policy.
   * @return The stored policy
   */
  ArrangePolicy GetArrangePolicy() const
  {
    return mArrangePolicy;
  }

  /**
   * @brief Records the View this manager is attached to.
   *
   * Called once, from ViewDataImpl::AttachLayoutManager. A manager can never be
   * replaced or detached (AttachLayoutManager asserts on a second attach), so there
   * is no reverse edge and no re-attach to mirror.
   *
   * @param[in] owner The attaching View
   */
  void SetOwner(ViewImpl* owner)
  {
    mOwner = owner;
  }

  /**
   * @brief Returns the View this manager is attached to, or nullptr before attach.
   * @return The owning View
   */
  ViewImpl* GetOwner() const
  {
    return mOwner;
  }

private:
  /// The execution policy for Arrange(). The default reuses an unchanged result.
  ArrangePolicy mArrangePolicy{ArrangePolicy::IF_CHANGED};

  /// The attached View, or nullptr while unattached. A RAW pointer, deliberately: the
  /// View owns this manager (through the LAYOUT_MANAGER trait) and destroys it, so the
  /// pointee strictly outlives the pointer and an owning reference would be a cycle.
  ViewImpl* mOwner{nullptr};
};

} // namespace Ui
} // namespace Dali

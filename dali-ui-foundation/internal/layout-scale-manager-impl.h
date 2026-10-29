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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

// Forward declaration
class View;

namespace Internal
{

/**
 * @brief Internal implementation of LayoutScaleManager.
 *
 * Stores the UI scale, the process-wide scaling switch and a list of layout root weak
 * handles. On a UI scale or switch change, all registered roots are invalidated and
 * re-layout is triggered.
 *
 * This object is the only owner of that state in the process. RegisterLayoutRoot() and
 * UnregisterLayoutRoot() are reached through GetImpl(); the public LayoutScaleManager
 * handle does not expose them.
 */
class DALI_UI_API LayoutScaleManagerImpl : public BaseObject
{
public:
  /**
   * @brief Returns the singleton LayoutScaleManager handle.
   *
   * Creates the implementation on first call. The singleton is a function-local
   * static, so the handle is never empty, also before the adaptor or Core exist.
   *
   * @return A LayoutScaleManager handle wrapping the singleton impl
   */
  static LayoutScaleManager Get();

  /**
   * @copydoc Dali::Ui::LayoutScaleManager::GetUiScale
   */
  float GetUiScale() const;

  /**
   * @copydoc Dali::Ui::LayoutScaleManager::SetUiScale
   */
  void SetUiScale(float scale);

  /**
   * @copydoc Dali::Ui::LayoutScaleManager::IsScalable
   */
  bool IsScalable() const;

  /**
   * @copydoc Dali::Ui::LayoutScaleManager::SetScalable
   */
  void SetScalable(bool enable);

  /**
   * @brief Registers a layout root view to be invalidated on scale change.
   *
   * Called when a view becomes a layout root (no parent layout/view).
   *
   * @param[in] root The root view to register
   */
  void RegisterLayoutRoot(Ui::View root);

  /**
   * @brief Unregisters a layout root view.
   *
   * @param[in] root The root view to unregister
   */
  void UnregisterLayoutRoot(Ui::View root);

protected:
  LayoutScaleManagerImpl();
  ~LayoutScaleManagerImpl() override;

private:
  LayoutScaleManagerImpl(const LayoutScaleManagerImpl&)            = delete;
  LayoutScaleManagerImpl(LayoutScaleManagerImpl&&)                 = delete;
  LayoutScaleManagerImpl& operator=(const LayoutScaleManagerImpl&) = delete;
  LayoutScaleManagerImpl& operator=(LayoutScaleManagerImpl&&)      = delete;

  // Drops the cached scale + layout caches of every registered layout root's
  // subtree and re-triggers layout. Shared by SetUiScale and SetScalable.
  void InvalidateAllLayoutRoots();

private:
  float                                            mUiScale{1.0f};
  bool                                             mIsScalable{true};
  std::unordered_map<RefObject*, WeakHandle<View>> mLayoutRoots;
};

} // namespace Internal

/**
 * @brief Retrieves the LayoutScaleManagerImpl from a LayoutScaleManager handle.
 *
 * @param[in] obj The LayoutScaleManager handle
 * @return A reference to the internal implementation
 */
inline Internal::LayoutScaleManagerImpl& GetImpl(LayoutScaleManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::LayoutScaleManagerImpl&>(handle);
}

/**
 * @brief Retrieves the LayoutScaleManagerImpl from a const LayoutScaleManager handle.
 *
 * @param[in] obj The LayoutScaleManager handle
 * @return A const reference to the internal implementation
 */
inline const Internal::LayoutScaleManagerImpl& GetImpl(const LayoutScaleManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::LayoutScaleManagerImpl&>(handle);
}

} // namespace Ui
} //namespace DALI_NAMESPACE

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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

// Forward declaration
class View;

namespace Internal
{

/**
 * @brief Internal implementation of UiScaleManager.
 *
 * Stores the current system scale factor and a list of layout root weak handles.
 * On scale change, all registered roots are invalidated and re-layout is triggered.
 */
class DALI_UI_API UiScaleManagerImpl : public BaseObject
{
public:
  /**
   * @brief Returns the singleton UiScaleManager handle.
   *
   * Creates the implementation on first call.
   *
   * @return A UiScaleManager handle wrapping the singleton impl
   */
  static UiScaleManager Get();

  /**
   * @copydoc Dali::Ui::UiScaleManager::GetScale
   */
  float GetScale() const;

  /**
   * @copydoc Dali::Ui::UiScaleManager::SetScale
   */
  void SetScale(float scale);

  /**
   * @copydoc Dali::Ui::UiScaleManager::IsScalable
   */
  bool IsScalable() const;

  /**
   * @copydoc Dali::Ui::UiScaleManager::SetScalable
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
  UiScaleManagerImpl();
  ~UiScaleManagerImpl() override;

private:
  UiScaleManagerImpl(const UiScaleManagerImpl&)            = delete;
  UiScaleManagerImpl(UiScaleManagerImpl&&)                 = delete;
  UiScaleManagerImpl& operator=(const UiScaleManagerImpl&) = delete;
  UiScaleManagerImpl& operator=(UiScaleManagerImpl&&)      = delete;

  // Drops the cached scale + layout caches of every registered layout root's
  // subtree and re-triggers layout. Shared by SetScale and SetScalable.
  void InvalidateAllLayoutRoots();

private:
  float                               mScale{1.0f};
  bool                                mIsScalable{true};
  std::vector<WeakHandle<BaseHandle>> mLayoutRoots;
};

} // namespace Internal

/**
 * @brief Retrieves the UiScaleManagerImpl from a UiScaleManager handle.
 *
 * @param[in] obj The UiScaleManager handle
 * @return A reference to the internal implementation
 */
inline Internal::UiScaleManagerImpl& GetImpl(UiScaleManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::UiScaleManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiScaleManagerImpl from a const UiScaleManager handle.
 *
 * @param[in] obj The UiScaleManager handle
 * @return A const reference to the internal implementation
 */
inline const Internal::UiScaleManagerImpl& GetImpl(const UiScaleManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::UiScaleManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali

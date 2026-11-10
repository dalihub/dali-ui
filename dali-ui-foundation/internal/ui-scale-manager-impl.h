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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layout-scale-manager-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
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
 * @brief Internal implementation of the deprecated UiScaleManager.
 *
 * Holds no scale state of its own. Every member forwards to the LayoutScaleManagerImpl
 * singleton, which owns the UI scale, the scaling switch and the layout-root registry,
 * so a value set through either public class is read back through both.
 *
 * It holds exactly one counted reference to that object, taken when this object is
 * created. A UiScaleManager handle therefore keeps the shared state alive, as it kept
 * its own state alive before the state moved, in whatever order the two function-local
 * statics are destroyed at exit. The destructor only releases that reference; it calls
 * no LayoutScaleManagerImpl member.
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
   * @brief Registers a layout root in the shared layout-root registry.
   *
   * @param[in] root The root view to register
   */
  void RegisterLayoutRoot(Ui::View root);

  /**
   * @brief Unregisters a layout root from the shared layout-root registry.
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

private:
  IntrusivePtr<LayoutScaleManagerImpl> mLayoutScaleManager; ///< The owner of the shared state; the one counted reference this object holds.
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
} //namespace DALI_NAMESPACE
